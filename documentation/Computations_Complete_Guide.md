# Complete Computation Guide — Smart Charging System
## with Practical Examples and Layman's Explanations

**Student:** Ajeesh Kumar R (2024MT12104)
**Guide:** Dr. Somasundaram Iyyamperumal
**Institution:** BITS Pilani WILP
**Subject:** IoT-Enabled Solar-Assisted Smart Charging System
**Date:** April 2026

> 📄 **A professionally formatted PDF version is available at:**
> [`documentation/Complete_Computation_Guide.pdf`](./Complete_Computation_Guide.pdf)
> *(26 pages, color-coded sections, tables, formula boxes, real-world scenario)*

---

## Table of Contents

1. [Power Calculation (P = V × I)](#1-power-calculation-p--v--i)
2. [Energy Calculation (E = P × t)](#2-energy-calculation-e--p--t)
3. [Supercapacitor Energy Storage (E = ½ × C × V²)](#3-supercapacitor-energy-storage-e---c--v)
4. [PWM Control (D = V_out / V_in × 100%)](#4-pwm-control-d--vout--vin--100)
5. [Charging Efficiency (η = E_out / E_in × 100%)](#5-charging-efficiency--eout--ein--100)
6. [Charging Time (t = Q / I)](#6-charging-time-t--q--i)
7. [Voltage Drop in Wiring (V_drop = I × R)](#7-voltage-drop-in-wiring-vdrop--i--r)
8. [Power Dissipation as Heat (P_heat = I² × R)](#8-power-dissipation-as-heat-pheat--i--r)
9. [Supercapacitor Charge/Discharge Time (t = C × ΔV / I)](#9-supercapacitor-chargedischarge-time-t--c--v--i)
10. [System State of Charge (SOC = Level / Max × 100%)](#10-system-state-of-charge-soc--level--max--100)
11. [Quick Reference Table](#11-quick-reference-table)
12. [Real-World Complete Charging Scenario](#12-real-world-complete-charging-scenario)
13. [Summary Table — System Parameters](#13-summary-table--system-parameters)
14. [Key Takeaways for Your Viva](#14-key-takeaways-for-your-viva)

---

## 1. Power Calculation (P = V × I)

### Formula

```
P = V × I
```

| Symbol | Meaning | Unit |
|--------|---------|------|
| P | Power | Watts (W) |
| V | Voltage | Volts (V) |
| I | Current | Amperes (A) |

### Layman's Explanation — Water Analogy

Think of electricity as water flowing through a pipe:
- **Voltage (V)** = Water pressure — how hard the water is being pushed
- **Current (I)** = How much water flows per second
- **Power (P)** = How much work is done per second (pressure × flow rate)

Just as a high-pressure, high-flow hose does more work than a low-pressure trickle, higher voltage AND higher current together produce more electrical power.

### Practical Example 1 — Phone Charging (5V, 2A)

```
P = 5V × 2A = 10 Watts
```

| Parameter | Value | Unit | Meaning |
|-----------|-------|------|---------|
| Voltage (V) | 5 | V | USB charger output |
| Current (I) | 2 | A | Current drawn by phone |
| Power (P) | 10 | W | Energy consumed per second |

### Practical Example 2 — Smart Charging System (12V, 3A)

```
P = 12V × 3A = 36 Watts
```

| Parameter | Value | Unit | Meaning |
|-----------|-------|------|---------|
| Voltage (V) | 12 | V | Solar panel / supply voltage |
| Current (I) | 3 | A | Charging current to battery |
| Power (P) | 36 | W | Energy delivered per second |

### What STM32 Will Measure

The STM32 reads voltage and current every millisecond (1000 times/second):
1. ADC Channel 1 → measures V across voltage divider → converts to actual voltage
2. ADC Channel 2 → measures current via shunt resistor → converts to amperes
3. Computes `P = V × I` in firmware (takes < 1 µs)
4. Stores result in a circular buffer for energy integration
5. Sends real-time power reading to ESP8266 → Cloud Dashboard every 1 second

### Real-World Meaning

- 36W means the system delivers enough energy to charge a phone ~3.6 times per hour
- Power changes every millisecond as solar intensity varies
- The STM32 monitors this in real time to adjust PWM duty cycle
- High power = fast charging; if power drops (cloudy day), the supercapacitor compensates

---

## 2. Energy Calculation (E = P × t)

### Formula

```
E = P × t          (for constant power)
E = ∫ P(t) dt      (for varying power — used in firmware)
```

**Unit conversion: 1 Wh = 3,600 Joules** (multiply Wh by 3600 to get J)

### Layman's Explanation — Speed × Time = Distance

Energy is like distance travelled:
- Speed (km/h) × Time (h) = Distance (km)
- Power (W) × Time (h) = Energy (Wh)

If you drive at 60 km/h for 2 hours → you travel 120 km.
If you use 10W for 2 hours → you consume 20 Wh of energy.

**Key insight: energy accumulates over time.** Even a small power draw, sustained for a long time, adds up to a large amount of energy!

### Practical Example 1 — Phone Charging Over 2 Hours

```
E = 10W × 2h = 20 Wh = 20 × 3600 = 72,000 Joules
```

- Phone charger: 5V × 2A = 10W
- Charged for 2 hours continuously
- Total energy: 20 Wh (enough to lift a 72 kg person about 100 metres!)

### Practical Example 2 — System with Varying Power (Integration)

```
Interval 1: P=36W for 10 min  → E₁ = 36 × (10/60) = 6.0 Wh
Interval 2: P=20W for 5 min   → E₂ = 20 × (5/60)  = 1.67 Wh  (cloud event)
Interval 3: P=36W for 15 min  → E₃ = 36 × (15/60) = 9.0 Wh
Total Energy = 6.0 + 1.67 + 9.0 = 16.67 Wh
```

### What STM32 Will Do

1. Sample V and I every 1 ms (1000 Hz)
2. Calculate instantaneous power: `P_inst = V × I`
3. Add to energy accumulator: `E_total += P_inst × Δt` (where Δt = 0.001 s)
4. Store in circular buffer (last 3600 samples = last 1 hour at 1 Hz logging)
5. Log cumulative energy to flash memory every second
6. Transmit to cloud every 30 seconds via ESP8266 Wi-Fi

---

## 3. Supercapacitor Energy Storage (E = ½ × C × V²)

### Formula

```
E = ½ × C × V²
```

| Symbol | Meaning | Unit |
|--------|---------|------|
| E | Stored Energy | Joules (J) |
| C | Capacitance | Farads (F) |
| V | Voltage across capacitor | Volts (V) |

### Layman's Explanation — Water Tank on a Hill

Imagine a water tank raised on a hill:
- **Tank size (C)** = how much water it can hold
- **Height of the hill (V)** = how much pressure the water has
- **Energy stored (E)** = how much work the falling water can do

**The V² factor means doubling the voltage quadruples the stored energy!**

### Practical Example — 100F Supercapacitor at 2.7V

```
E = 0.5 × 100F × (2.7V)² = 0.5 × 100 × 7.29 = 364.5 Joules
```

### Energy at Different Voltages (100F Supercapacitor)

| Voltage (V) | Energy Stored (J) | Energy (mWh) | % of Maximum |
|-------------|-------------------|--------------|--------------|
| 0.5 V | 12.5 J | 3.47 mWh | 3.4% |
| 1.0 V | 50 J | 13.9 mWh | 13.7% |
| 1.5 V | 112.5 J | 31.25 mWh | 30.8% |
| 2.0 V | 200 J | 55.6 mWh | 54.9% |
| 2.5 V | 312.5 J | 86.8 mWh | 85.7% |
| **2.7 V (max)** | **364.5 J** | **101.3 mWh** | **100%** |

### Why the Supercapacitor is Useful

- Solar panels produce fluctuating power (clouds, angle, shadows)
- The supercapacitor acts as a 'buffer' — absorbs excess power instantly and releases during dips
- Batteries prefer steady charging current (extending battery life)
- The supercap can charge/discharge **millions of times** (vs. ~500 cycles for Li-ion batteries)
- Response time: **milliseconds** vs. seconds for batteries — perfect for fast fluctuations

### Before / After Comparison

| Condition | Without Supercap | With Supercap (100F) |
|-----------|-----------------|----------------------|
| Cloud passes (power drops) | Charging stops abruptly | Supercap supplies 364.5 J — charging continues |
| Voltage fluctuation | PWM loses control momentarily | Supercap stabilises voltage |
| Battery stress | High (variable current) | Low (steady current from buffer) |
| System efficiency | ~89% | ~92% (+3% improvement) |
| Charging smoothness | Jerky, unstable | Smooth and continuous |

---

## 4. PWM Control (D = V_out / V_in × 100%)

### Formula

```
D = (V_out / V_in) × 100%
```

| Symbol | Meaning | Unit |
|--------|---------|------|
| D | Duty Cycle | % |
| V_out | Desired output voltage | V |
| V_in | Input voltage | V |

### Layman's Explanation

**Analogy 1 — Flickering Light:** Imagine rapidly switching a light on and off 1000 times per second. If it's ON for 75% of each cycle and OFF for 25%, the light appears 75% bright. PWM does the same with voltage.

**Analogy 2 — Water Tap:** Think of PWM as a tap that opens and closes very quickly. Open for 75% of the time = 75% of full water pressure applied on average.

### Practical Example 1 — 12V Input, 9V Output Desired

```
D = (9V / 12V) × 100% = 75% Duty Cycle
```

**1ms PWM Cycle Timeline (75% Duty Cycle):**
```
|←────────────── ON (750µs) ──────────────→|←── OFF (250µs) ──→|
0µs                                        750µs              1000µs
[12V ██████████████████████████████████████][0V ████████]
```

### Practical Example 2 — Adaptive Control

| Condition | V_in | V_out Target | Duty Cycle | System Response |
|-----------|------|--------------|------------|-----------------|
| Normal sunny day | 12V | 9V | 75% | Normal operation |
| Partial cloud | 10V | 9V | 90% | STM32 increases duty cycle |
| Heavy cloud | 8V | 9V | 112.5% (IMPOSSIBLE!) | Supercap kicks in |
| Supercap assists (+2V) | 10V | 9V | 90% | Normal operation resumes |
| After cloud clears | 12V | 9V | 75% | STM32 restores normal duty |

### Real Meaning and Benefit

- PWM enables efficient DC-DC voltage conversion (~95% vs. 60-70% for linear regulators)
- The STM32 adjusts duty cycle every 1ms — **real-time adaptive control**
- This system uses **20 kHz PWM** — above human hearing range, preventing audible noise

---

## 5. Charging Efficiency (η = E_out / E_in × 100%)

### Formula

```
η = (E_out / E_in) × 100%
```

### Layman's Explanation — Fuel Gauge Analogy

Think of a car engine: if you put in 10 litres of fuel and the car uses 8 litres to move forward (2 litres lost as heat), the efficiency is 8/10 = 80%.

In our charging system: if 100 Wh enters from the solar panel and 92 Wh actually goes into the battery (8 Wh lost as heat in wiring and electronics), efficiency = **92%**.

### Practical Example

```
η = (92 Wh / 100 Wh) × 100% = 92% Efficiency
```

- Energy input (from solar panel): 100 Wh
- Energy losses (wiring heat + PWM switching + converter): 8 Wh
- Energy delivered to battery: 92 Wh

### Buffered vs Non-Buffered Comparison

| Metric | Without Supercap | With Supercap (100F) |
|--------|-----------------|----------------------|
| Input Energy (E_in) | 100 Wh | 100 Wh |
| Wiring losses | 3 Wh | 2.5 Wh |
| PWM/switching losses | 5 Wh | 4.5 Wh |
| Charge interruption losses | 1 Wh (restarts) | 0 Wh (continuous) |
| Output Energy (E_out) | 91 Wh | 93 Wh |
| **Charging Efficiency (η)** | **91%** | **93% (+2% improvement)** |

### What a 2-3% Efficiency Gain Means Practically

For a system running 8 hours/day, 365 days/year, on a 100W solar panel:
- Without supercap: 91% of 292 kWh/year = **265.7 kWh stored**
- With supercap: 93% of 292 kWh/year = **271.6 kWh stored**
- **Extra energy per year: 5.9 kWh** — enough to charge a phone ~700 extra times!

---

## 6. Charging Time (t = Q / I)

### Formula

```
t = Q / I
```

| Symbol | Meaning | Unit |
|--------|---------|------|
| t | Time to charge | hours (h) |
| Q | Battery capacity | Ampere-hours (Ah) |
| I | Charging current | Amperes (A) |

### Layman's Explanation — Filling a Tank

Imagine filling a swimming pool with a hose:
- Pool size (Q) = 5,000 litres (like a 5 Ah battery)
- Hose flow rate (I) = 1,000 litres/hour (like 1A charging current)
- Time to fill (t) = 5,000 / 1,000 = **5 hours**

A bigger hose fills the pool faster! But pour too fast and you risk overflow (overcharge). The sweet spot is **1C rate** (1× capacity per hour) = 5A for a 5Ah battery.

### Practical Examples

```
5 Ah battery at 1A:  t = 5Ah / 1A  = 5 hours
5 Ah battery at 2A:  t = 5Ah / 2A  = 2.5 hours
5 Ah battery at 5A:  t = 5Ah / 5A  = 1 hour
```

| Charging Current | Battery Capacity | Time to Charge | Charge Rate | Notes |
|-----------------|-----------------|----------------|-------------|-------|
| 0.5 A | 5 Ah | 10 hours | 0.1C | Very gentle, battery-friendly |
| 1 A | 5 Ah | 5 hours | 0.2C | Standard overnight charging |
| 2 A | 5 Ah | 2.5 hours | 0.4C | Fast charge — moderate stress |
| 5 A | 5 Ah | 1 hour | 1C | 1C rate — maximum recommended |
| 10 A | 5 Ah | 0.5 hours | 2C | Very fast — reduces battery life |

---

## 7. Voltage Drop in Wiring (V_drop = I × R)

### Formula

```
V_drop = I × R
```

### Layman's Explanation — Narrow Water Pipes

As water travels through a long, narrow pipe, friction reduces pressure at the far end. Similarly, electric current through a wire with resistance causes a **voltage drop** — less voltage arrives at the battery than left the charger!

### Practical Example — 2m Cable, 3A Current

```
V_drop = 3A × 0.5Ω = 1.5 Volts
```

| Parameter | Value | Unit | Detail |
|-----------|-------|------|--------|
| Cable length | 2 | metres | 1m positive + 1m negative |
| Wire gauge | 22 AWG | — | 0.25 Ω/m for this gauge |
| Total resistance | 0.5 | Ω | 2m × 0.25 Ω/m |
| Charging current | 3 | A | Current flowing through wire |
| **Voltage drop** | **1.5** | **V** | V_drop = 3A × 0.5Ω |
| Voltage at source | 12 | V | Solar panel / supply |
| **Voltage at battery** | **10.5** | **V** | 12V − 1.5V = **10.5V !** |

### Solution — Use Thicker Wires

| Wire Gauge | Resistance (Ω/m) | V_drop at 3A (2m) | Voltage at Battery |
|------------|------------------|--------------------|--------------------|
| 28 AWG (thin) | 0.213 Ω/m | 1.28 V | 10.72 V |
| 22 AWG | 0.053 Ω/m | 0.32 V | 11.68 V |
| 18 AWG (recommended) | 0.021 Ω/m | 0.13 V | 11.87 V |
| 14 AWG (very thick) | 0.008 Ω/m | 0.05 V | 11.95 V |

**Recommendation:** Use **18 AWG or thicker** for all connections carrying >2A.

---

## 8. Power Dissipation as Heat (P_heat = I² × R)

### Formula

```
P_heat = I² × R
Also: P_heat = V_drop × I = V_drop² / R
```

### Layman's Explanation — Rubbing Your Hands

Rub your hands together quickly — they get warm. Friction converts mechanical energy to heat. In wires, resistance does the same to electricity.

**The I² factor is crucial: doubling current QUADRUPLES heat generation!**

```
At 1A through 0.5Ω: P_heat = 1² × 0.5 = 0.5W  (barely warm)
At 2A through 0.5Ω: P_heat = 4  × 0.5 = 2.0W  (noticeable)
At 3A through 0.5Ω: P_heat = 9  × 0.5 = 4.5W  (hot — fire risk if wire too thin!)
```

### Practical Example — Same 2m Cable, 3A

```
P_heat = (3A)² × 0.5Ω = 9 × 0.5 = 4.5 Watts wasted as heat!

Alternative: P_heat = V_drop × I = 1.5V × 3A = 4.5 Watts ✓ (same answer)
```

### Impact Table

| Scenario | Current | R (wire) | P_heat | Energy wasted/hour |
|----------|---------|----------|--------|--------------------|
| Thin 28AWG, 2m | 3 A | 0.85 Ω | 7.65 W | 7.65 Wh |
| Standard 22AWG, 2m | 3 A | 0.21 Ω | 1.89 W | 1.89 Wh |
| **Thick 18AWG, 2m** | **3 A** | **0.084 Ω** | **0.76 W** | **0.76 Wh** |
| Very thick 14AWG, 2m | 3 A | 0.032 Ω | 0.29 W | 0.29 Wh |

### Thermal Management

- STM32 monitors temperature via thermistor near the charging circuit
- If temperature exceeds 60°C, charging current is automatically reduced
- MOSFET switching losses also contribute to heating

---

## 9. Supercapacitor Charge/Discharge Time (t = C × ΔV / I)

### Formula

```
t = (C × ΔV) / I
```

| Symbol | Meaning | Unit |
|--------|---------|------|
| t | Time | seconds (s) |
| C | Capacitance | Farads (F) |
| ΔV | Voltage change | Volts (V) |
| I | Charging/discharging current | Amperes (A) |

### Practical Example — 100F Supercap, 0V to 2V at 5A

```
t = (100F × 2V) / 5A = 200 / 5 = 40 seconds
```

Faster at 10A:
```
t = (100F × 2V) / 10A = 200 / 10 = 20 seconds
```

### Charge Time Reference Table

| Current | ΔV (0V → 2.7V) | Time to Charge | Notes |
|---------|----------------|----------------|-------|
| 1 A | 2.7 V | 270 seconds (4.5 min) | Very slow — gentle |
| 5 A | 2.7 V | 54 seconds (~1 min) | Good for buffering |
| 10 A | 2.7 V | 27 seconds | Fast — ready quickly |
| 5 A | 0V → 2V | 40 seconds | Partial charge |
| 10 A | 0V → 2V | 20 seconds | Fast partial charge |

### Impact on Buffering Effectiveness

- Cloud duration: typically 30–90 seconds for passing clouds
- Discharge at 5A from 2.7V to 1.8V: t = (100 × 0.9) / 5 = **18 seconds of buffer**
- Reduce to 3A → t = (100 × 0.9) / 3 = **30 seconds of buffer**
- **Time is inversely proportional to current** — double current = half charge time

---

## 10. System State of Charge (SOC = Level / Max × 100%)

### Formula

```
SOC = (Current Level / Maximum Capacity) × 100%
```

### Layman's Explanation — Fuel Gauge

SOC is exactly like your car's fuel gauge:
- Full tank = 100% SOC
- Half tank = 50% SOC
- Empty = 0% SOC

### Practical Example

```
3500 mAh remaining in a 5000 mAh battery:
SOC = (3500 / 5000) × 100% = 70%
```

### SOC Interpretation Table

| SOC Level | Interpretation | System Action | Display |
|-----------|---------------|---------------|---------|
| 90–100% | Nearly full | Reduce to trickle charge (0.1C) | 🟢 Green |
| 80–90% | Charging complete target | Maintain / Log completion | 🟢 Green |
| 50–80% | Good — normal operation | Continue charging at set rate | 🟢 Green |
| 20–50% | Moderate — needs charging | Charging at full rate | 🟡 Yellow |
| 10–20% | Low — priority charging | Alert sent to dashboard | 🟠 Orange |
| 0–10% | Critical | Max charge rate, alarm triggered | 🔴 Red |

### Battery Health Monitoring

- STM32 tracks SOC via **Coulomb counting** (integrating charge current)
- Periodic voltage measurement calibrates SOC (voltage-SOC curve lookup)
- Battery degradation shows as reduced max capacity — tracked over lifetime
- Alert if SOC drops below 20% or rises above 95%

---

## 11. Quick Reference Table

| # | Computation | Formula | Units | What It Measures |
|---|------------|---------|-------|-----------------|
| 1 | Power | `P = V × I` | Watts (W) | Energy per second |
| 2 | Energy | `E = P × t` (or `∫P dt`) | Wh or J | Total energy over time |
| 3 | Supercap Storage | `E = ½ × C × V²` | Joules (J) | Energy in capacitor |
| 4 | PWM Duty Cycle | `D = (V_out/V_in) × 100%` | % | Switching ratio for voltage control |
| 5 | Efficiency | `η = (E_out/E_in) × 100%` | % | How much energy is usefully transferred |
| 6 | Charging Time | `t = Q / I` | hours (h) | Time to fully charge battery |
| 7 | Voltage Drop | `V_drop = I × R` | Volts (V) | Voltage lost in wiring |
| 8 | Heat Dissipation | `P_heat = I² × R` | Watts (W) | Power wasted as heat in wires |
| 9 | Supercap Time | `t = (C × ΔV) / I` | seconds (s) | Time to charge/discharge supercap |
| 10 | State of Charge | `SOC = (Level/Max) × 100%` | % | Battery charge level |

### Unit Conversions

| From | To | Multiply by | Example |
|------|----|-------------|---------|
| Watt-hours (Wh) | Joules (J) | × 3600 | 1 Wh = 3,600 J |
| Joules (J) | Watt-hours (Wh) | ÷ 3600 | 3600 J = 1 Wh |
| mAh | Ah | ÷ 1000 | 5000 mAh = 5 Ah |
| Ah | mAh | × 1000 | 2 Ah = 2000 mAh |

---

## 12. Real-World Complete Charging Scenario

### Scenario Setup

| Component | Specification | Initial Condition |
|-----------|--------------|-------------------|
| Power Supply | 12V, 5A maximum | Powered on, stable |
| Battery | 12V, 2Ah Li-ion | SOC = 30% (0.6 Ah charged) |
| Supercapacitor | 100F, max 2.7V | Fully charged at 2.7V |
| Target SOC | 80% | Need to add 1.0 Ah |
| Charging Current | 2.5A target | STM32 sets PWM accordingly |
| Wire resistance | 0.1 Ω total | 18 AWG, 2m total |

---

### T = 0 Seconds — Starting Conditions

| Sensor Reading | Value | Computation | Result |
|---------------|-------|-------------|--------|
| Voltage (V) | 12.0 V | V_drop = 2.5A × 0.1Ω | V_drop = 0.25V |
| Current (I) | 2.5 A | P_heat = (2.5)² × 0.1 | P_heat = 0.625W |
| Power (P) | 30 W | P = 12V × 2.5A | P = 30W |
| Battery voltage | 11.75 V | 12V − 0.25V drop | Battery gets 11.75V |
| Supercap voltage | 2.7V (100%) | E = 0.5 × 100 × 2.7² | E = 364.5 J stored |
| Battery SOC | 30% | SOC = (0.6/2.0) × 100 | SOC = 30.0% |

---

### T = 0 → 60 Seconds — Phase 1: Charging Begins

- **Action 1:** STM32 reads sensors — V = 12V, I = 2.5A every 1ms
- **Action 2:** Calculate instantaneous power: P = 12V × 2.5A = **30W**
- **Action 3:** Calculate voltage drop: V_drop = 2.5A × 0.1Ω = **0.25V** → Battery gets 11.75V
- **Action 4:** Calculate wiring losses: P_heat = (2.5)² × 0.1 = **0.625W** lost as heat
- **Action 5:** Track supercapacitor: E_supercap = 364.5J — stable (not needed yet)
- **Action 6:** First energy (after 1s): E = 30W × (1/3600)h = **0.00833 Wh**

```
After 60 seconds:
  Cumulative energy: E = 30W × (60/3600)h = 0.5 Wh
  Efficiency so far: ~98%
  Battery SOC: ~30.7%
```

---

### T = 300 Seconds (5 Minutes) — Steady Charging

```
Cumulative energy:  E = 30W × (300/3600)h = 2.5 Wh
Charge delivered:   Q = 2.5 Wh / 12V = 0.208 Ah
Battery SOC:        (0.6 + 0.208) / 2.0 × 100% = ~40.4%
Supercap:           still at 2.7V — hasn't been needed yet
```

---

### T = 600 Seconds (10 Minutes) — ☁️ CLOUD PASSES!

**Scenario:** Voltage drops suddenly from 12V to 8V!

**❌ Without Supercapacitor (Problem):**
- Required duty cycle = (9V / 8V) × 100% = **112.5% — IMPOSSIBLE!**
- Charging stops or becomes unstable
- Battery receives no charge during cloud event

**✅ With Supercapacitor (Solution):**

| Step | Action | Result |
|------|--------|--------|
| Step 1 | STM32 detects voltage drop | V drops from 12V to 8V — ADC triggers alert |
| Step 2 | Calculate required duty cycle | D = (9V/8V)×100% = 112.5% → **IMPOSSIBLE** |
| Step 3 | Supercap kicks in automatically | Adds 2V → effective V_in = 10V |
| Step 4 | Recalculate duty cycle | D = (9V/10V)×100% = **90% → ACHIEVABLE!** |
| Step 5 | Supercap discharges over 60s | V_supercap: 2.0V → 1.8V |
| **Result** | **Charging continues smoothly!** | Battery never notices the cloud 🎉 |

---

### T = 1200 Seconds (20 Minutes)

```
Total energy delivered: E = 30W × (1200/3600)h = 10 Wh
Charge delivered:       Q = 10 Wh / 12V = 0.833 Ah
Battery SOC:            (0.6 + 0.833) / 2.0 × 100% = ~71.7%
Time remaining:         ~4 minutes to reach 80%
Efficiency trend:       97.8%
```

---

### T = 1440 Seconds (24 Minutes) — ✅ CHARGING COMPLETE!

| Final Parameter | Value | Computation Used |
|----------------|-------|-----------------|
| Battery SOC | **80% ✓ (Target Reached!)** | SOC = (1.6/2.0) × 100% |
| Total Energy Delivered | 12 Wh | E = 30W × (24min / 60) |
| Total Energy Input | 12.3 Wh | Including wiring losses |
| Overall Efficiency | **97.6%** | η = (12/12.3) × 100% |
| Supercap Final State | 1.5V | E = 0.5 × 100 × 1.5² = 112.5J remaining |
| Average Charging Current | 2.08 A | I_avg = Q/t = 1.0Ah / (24/60)h |
| Charge Delivered | 1.0 Ah (30%→80%) | ΔSOC × Capacity = 0.5 × 2Ah |

---

## 13. Summary Table — System Parameters

| Parameter | Symbol | Value | Unit | Plain English Meaning |
|-----------|--------|-------|------|----------------------|
| Supply Voltage | V_in | 12.0 | V | Electricity pressure from solar panel |
| Charging Current | I | 2.5 | A | Rate of electron flow to battery |
| Instantaneous Power | P | 30 | W | Energy delivered per second |
| Wiring Resistance | R | 0.1 | Ω | How much the wire opposes current |
| Voltage Drop | V_drop | 0.25 | V | Voltage lost in wire (Ohm's Law) |
| Voltage at Battery | V_bat | 11.75 | V | Actual voltage the battery receives |
| Heat in Wires | P_heat | 0.625 | W | Power wasted as heat |
| PWM Duty Cycle | D | 75 | % | Fraction of time switch is ON |
| Supercap Capacitance | C | 100 | F | Energy storage capacity |
| Supercap Energy (full) | E_sc | 364.5 | J | Energy stored when fully charged |
| Battery Capacity | Q | 2.0 | Ah | Total charge battery can hold |
| Initial SOC | SOC_0 | 30 | % | Battery level at start |
| Target SOC | SOC_t | 80 | % | Desired battery level |
| Total Energy Delivered | E_out | 12 | Wh | Energy that went into battery |
| Total Energy Input | E_in | 12.3 | Wh | Energy drawn from solar panel |
| Overall Efficiency | η | 97.6 | % | How efficiently energy was transferred |
| Charging Time | t | 24 | minutes | Time to charge from 30% to 80% |
| Average Current | I_avg | 2.08 | A | Mean charging current over session |

---

## 14. Key Takeaways for Your Viva

### ⚡ Power (P = V × I)
- Power is **REAL-TIME** — it changes every millisecond
- Higher voltage AND higher current both increase power
- STM32 measures both and computes power 1000 times/second

### 🔋 Energy (E = P × t and ∫P dt)
- Energy is **CUMULATIVE** — it always increases during charging
- Energy = how long power flows, multiplied by the power
- Convert: **1 Wh = 3600 J**

### 📊 Efficiency (η = E_out / E_in × 100%)
- Aim for **>95%** efficiency in a well-designed system
- Our system achieves ~97.6% — excellent!
- Supercapacitor adds ~2-3% by preventing charge interruptions

### 🔁 PWM Control (D = V_out / V_in × 100%)
- PWM adjusts duty cycle **dynamically** — every 1ms
- Cloud passes → V_in drops → duty cycle increases automatically
- Maximum duty cycle = 100% — cannot boost voltage without supercap

### ⚡ Supercapacitor (E = ½CV² and t = CΔV/I)
- Acts as a **'shock absorber'** for voltage fluctuations
- Stores 364.5 J at 2.7V — enough for ~18 seconds of buffering at 5A
- V² factor: **doubling voltage = 4× more stored energy**

### 📶 Buffered vs Non-Buffered Charging
- Non-buffered: charging stops when voltage drops → battery stress
- Buffered (with supercap): charging continues smoothly
- Efficiency improvement: ~89% → ~93% with supercapacitor

### 🔬 STM32 Firmware Loop (every 1ms)
1. Read V and I from ADC channels
2. Compute `P = V × I`
3. Accumulate `E += P × 0.001`
4. Update `SOC = (Q_charged / Q_max) × 100%`
5. Adjust PWM duty cycle via PI controller
6. Check supercap voltage — charge/discharge as needed
7. Every 1 second: log data and transmit to ESP8266 → Cloud

---

> 📄 **Full PDF with color diagrams, highlighted formula boxes, and detailed tables:**
> [`documentation/Complete_Computation_Guide.pdf`](./Complete_Computation_Guide.pdf)

*This comprehensive guide covers all essential calculations for the IoT Solar-Assisted Smart Charging System. Good luck with your viva on April 17, 2026! 🎓*