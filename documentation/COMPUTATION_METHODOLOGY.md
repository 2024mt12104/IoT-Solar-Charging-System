# Computation Methodology

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Institution:** BITS Pilani WILP  

---

## 1. Overview

The computation layer of this system is implemented entirely in the STM32F446RE microcontroller firmware. It transforms raw ADC measurements into meaningful electrical quantities, drives closed-loop PWM control, and packages data for IoT transmission. All computations target a control loop execution time of **< 10 ms**.

---

## 2. Signal Acquisition and Conversion

### 2.1 ADC Configuration

The STM32 ADC operates in **12-bit continuous DMA mode**:
- Resolution: 12 bits → 4096 quantization levels
- Reference voltage: V_REF = 3.3 V
- Sampling rate: 1 kHz (configurable up to 2.4 MHz)

**Raw ADC to Voltage Conversion:**

```
V_adc = (ADC_raw / 4095) × V_REF
```

Where:
- `ADC_raw` = digital value from ADC register (0–4095)
- `V_REF` = 3.3 V (STM32 internal reference)
- `V_adc` = voltage at the ADC pin (0–3.3 V)

### 2.2 Voltage Measurement

A resistor voltage divider scales the system voltage to the ADC input range:

```
V_actual = V_adc × (R1 + R2) / R2
```

**Example (12V system):**
- R1 = 30 kΩ, R2 = 10 kΩ
- Scale factor = (30k + 10k) / 10k = 4.0
- V_actual = V_adc × 4.0
- At V_actual = 12V: V_adc = 3.0V → ADC_raw ≈ 3723

**Calibration:**
```
V_calibrated = V_actual × (1 + gain_error) + offset_error
```

### 2.3 Current Measurement

**Using ACS712 Hall-Effect Sensor:**
- Sensitivity: 185 mV/A (ACS712-5A), 100 mV/A (ACS712-20A)
- Zero-current output: V_zero = 2.5 V (at V_CC = 5V)

```
I_actual = (V_adc_current - V_zero) / Sensitivity
```

**Example (ACS712-5A, 5V supply):**
```
I = (V_sensor - 2.5) / 0.185   [Amperes]
```

**Note:** Since STM32 ADC input max is 3.3V, a resistor divider or op-amp level shifter may be required on the ACS712 output.

---

## 3. Signal Filtering

Raw ADC readings contain noise from switching transients, sensor noise, and electromagnetic interference. A **moving average filter** is applied:

### 3.1 Moving Average Filter

```c
#define FILTER_WINDOW 16

float moving_average(float *buffer, float new_sample) {
    // Shift buffer and add new sample
    memmove(buffer, buffer + 1, (FILTER_WINDOW - 1) * sizeof(float));
    buffer[FILTER_WINDOW - 1] = new_sample;
    
    float sum = 0;
    for (int i = 0; i < FILTER_WINDOW; i++) sum += buffer[i];
    return sum / FILTER_WINDOW;
}
```

**Effect:** Reduces high-frequency noise; introduces a lag of ~N/2 samples.

### 3.2 Exponential Moving Average (EMA)

For faster response with noise reduction:

```
V_filtered[n] = α × V_raw[n] + (1 - α) × V_filtered[n-1]
```

Where α ∈ (0, 1) is the smoothing factor:
- α = 0.1 → Heavy smoothing, slow response
- α = 0.5 → Moderate balance
- α = 0.9 → Light smoothing, fast response

---

## 4. Power and Energy Calculations

### 4.1 Instantaneous Power

```
P(t) = V(t) × I(t)   [Watts]
```

**Firmware Implementation:**
```c
float compute_power(float voltage, float current) {
    return voltage * current;  // Watts
}
```

### 4.2 Cumulative Energy

Energy is computed by numerically integrating power over time using the **trapezoidal rule** (discrete approximation):

```
E[n] = E[n-1] + (P[n] + P[n-1]) / 2 × Δt
```

Simplified for fixed Δt (constant sampling interval):
```
E[n] = E[n-1] + P[n] × Δt
```

**Conversion to Watt-hours:**
```
E_Wh = E_joules / 3600
```

**Firmware Implementation:**
```c
float energy_joules = 0.0f;  // Running total

void update_energy(float power_watts, float delta_t_sec) {
    energy_joules += power_watts * delta_t_sec;
}

float get_energy_wh(void) {
    return energy_joules / 3600.0f;
}
```

### 4.3 System Efficiency

```
η = (P_out / P_in) × 100%
```

Where:
- `P_in` = V_in × I_in (input power from DC supply)
- `P_out` = V_battery × I_battery (power delivered to battery)

**Losses include:** DC-DC converter switching losses, resistive losses in wiring, supercapacitor ESR losses.

---

## 5. Supercapacitor Energy and State of Charge

### 5.1 Energy Stored in Supercapacitor

```
E_supercap = ½ × C × V²   [Joules]
```

Where:
- `C` = Capacitance in Farads
- `V` = Supercapacitor terminal voltage at time t

**Example:** C = 50F, V = 8V → E = ½ × 50 × 64 = 1600 J = 0.444 Wh

### 5.2 State of Charge (SoC) Estimation

```
SoC = ((V² - V_min²) / (V_max² - V_min²)) × 100%
```

Where:
- `V` = Current voltage
- `V_min` = Minimum usable voltage (e.g., 1V for 2.7V-rated supercap)
- `V_max` = Maximum rated voltage (e.g., 2.7V per cell)

**Firmware:**
```c
float compute_supercap_soc(float v_current, float v_min, float v_max) {
    float soc = (v_current * v_current - v_min * v_min) /
                (v_max * v_max - v_min * v_min) * 100.0f;
    return (soc < 0) ? 0.0f : (soc > 100) ? 100.0f : soc;
}
```

### 5.3 Supercapacitor Charge/Discharge Power

```
P_supercap = V_supercap × I_supercap
           = V_supercap × C × (dV/dt)
```

This is used to estimate the contribution of the supercapacitor to maintaining charging current during input dips.

---

## 6. PWM Control Algorithm

### 6.1 Open-Loop PWM

A fixed duty cycle sets the DC-DC converter output:

```
V_out ≈ V_in × D   (Buck converter, ideal)
```

Where D = duty cycle (0.0–1.0).

### 6.2 Closed-Loop Voltage Regulation (P-Controller)

```
error = V_target - V_measured
duty_cycle = duty_cycle_base + K_p × error
```

Constrained to safe range:
```
duty_cycle = clamp(duty_cycle, D_min, D_max)
```

### 6.3 PI Controller (Recommended for Stability)

```
error[n] = V_target - V_measured[n]
integral[n] = integral[n-1] + error[n] × Δt

duty_cycle = K_p × error[n] + K_i × integral[n]
```

**Typical Gain Values (requires tuning):**
- K_p = 0.05 – 0.20
- K_i = 0.001 – 0.01

### 6.4 Anti-Windup

To prevent integral saturation:
```c
if (duty_cycle > D_MAX) {
    duty_cycle = D_MAX;
    integral -= error * delta_t;  // Back-calculate
}
```

---

## 7. Protection Logic

### 7.1 Overvoltage Protection

```c
if (V_measured > V_OVP_THRESHOLD) {
    pwm_disable();
    system_state = STATE_FAULT;
    log_event(EVENT_OVP);
}
```

### 7.2 Overcurrent Protection

```c
if (I_measured > I_OCP_THRESHOLD) {
    pwm_reduce_duty(10);  // Reduce duty cycle by 10%
    if (I_measured > I_OCP_HARD_LIMIT) {
        pwm_disable();
        system_state = STATE_FAULT;
    }
}
```

### 7.3 Protection Thresholds (Example Values)

| Parameter | Warning Level | Shutdown Level |
|-----------|--------------|----------------|
| Input Voltage | > 13.0 V | > 14.0 V |
| Output Voltage | > 8.5 V | > 9.0 V |
| Input Current | > 2.0 A | > 2.5 A |
| Supercap Voltage | > V_rated × 1.1 | > V_rated × 1.15 |

---

## 8. UART Data Transmission Format

### 8.1 Packet Structure (JSON)

Each data packet transmitted to ESP8266 at ~1 Hz:

```json
{
  "v_in": 11.42,
  "v_sc": 8.71,
  "v_bat": 7.08,
  "i_in": 1.24,
  "i_out": 0.97,
  "p_in": 14.16,
  "p_out": 6.87,
  "e_wh": 0.341,
  "eff": 48.5,
  "pwm": 72,
  "sc_soc": 85.0,
  "state": 1,
  "flags": 0
}
```

### 8.2 Transmission Rate

- Normal operation: 1 Hz (1 packet/second)
- During transient events: 10 Hz (burst mode)
- Idle: 0.1 Hz (heartbeat)

---

## 9. Control Loop Timing

| Task | Target Execution Time |
|------|-----------------------|
| ADC sampling (DMA) | Background, ~100 µs/sample |
| Moving average filter | < 0.5 ms |
| Power calculation | < 0.1 ms |
| Energy integration | < 0.1 ms |
| PWM update | < 0.1 ms |
| Protection check | < 0.1 ms |
| UART packet assembly | < 1 ms |
| **Total loop time** | **< 10 ms** |

---

## 10. Measurement Accuracy and Error Analysis

### 10.1 ADC Quantization Error

```
±ΔV_quantization = ±(V_REF / 2^N) / 2 = ±(3.3 / 4096) / 2 ≈ ±0.4 mV
```

After voltage divider scaling (×4): ±1.6 mV in actual voltage measurement.

### 10.2 Current Measurement Error (ACS712)

- Sensitivity error: ±1.5% typical
- Zero-point offset: ±1.5% of V_CC
- Temperature coefficient: ±0.5 mV/°C

**Total current error at 1A:** ≈ ±2–3% of full scale

### 10.3 Power Measurement Error

```
ΔP/P = √((ΔV/V)² + (ΔI/I)²)
```

For ΔV/V = 0.5%, ΔI/I = 2%:
```
ΔP/P = √(0.0025 + 0.04) ≈ 2.06%
```

---

*Document Version: 1.0 | Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
