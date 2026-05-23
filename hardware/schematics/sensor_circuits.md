<!---
Project Title: IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING SUPERCAPACITOR ENERGY BUFFERING
Author: Ajeesh Kumar R | BITS ID: 2024MT12104
Programme: M.Tech in Software Systems - Specialization in IoT
Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division

Academic Purpose Notice:
  This document is developed solely for academic learning, research, experimentation,
  and project evaluation purposes under the M.Tech in Software Systems - Specialization
  in IoT programme at BITS Pilani WILP.

Ownership: All project work is the intellectual work of the above-mentioned author unless otherwise referenced.
Usage Restriction: Unauthorized copying, redistribution, or commercial usage without prior permission is discouraged.
--->

# Sensor Interface Circuits

## 1. Voltage Sensing — Resistive Divider

### Circuit Diagram

```
                R1 = 30 kΩ             R2 = 10 kΩ
    V_actual ──/\/\/\/──────┬──────────/\/\/\/──── GND
                            │
                       C_filter = 10nF
                            │
                          ──┴──  GND
                            │
                     R_prot = 1 kΩ
                            │
                            ▼
                      STM32 ADC Pin
```

### Design Calculations

$$V_{adc} = V_{actual} \times \frac{R_2}{R_1 + R_2} = V_{actual} \times \frac{10k}{30k + 10k} = V_{actual} \times 0.25$$

| Parameter | Value | Notes |
|-----------|-------|-------|
| Scale Factor | 4.0 (÷0.25) | Multiply ADC reading by 4 to get actual |
| Max Measurable Voltage | 13.2V | At ADC full-scale (3.3V × 4) |
| Resolution | 3.22 mV/bit | 13.2V / 4096 steps |
| Input Impedance | 40 kΩ | R1 + R2 (negligible loading) |
| Filter Cutoff | 15.9 kHz | 1/(2π × 1kΩ × 10nF) |

### Three Voltage Dividers Used

| Instance | Measures | ADC Channel | STM32 Pin |
|----------|----------|-------------|-----------|
| Divider 1 | Input voltage (solar/DC supply) | ADC1_CH0 | PA0 |
| Divider 2 | Battery voltage (output) | ADC1_CH1 | PA1 |
| Divider 3 | Supercapacitor voltage | ADC1_CH8 | PB0 |

---

## 2. Current Sensing — ACS712 Hall-Effect Sensor

### Circuit Diagram

```
                         ACS712-5A Module
                    ┌──────────────────────┐
                    │                      │
    I_measured ────►│ IP+          VCC ────┼──── 5V (from STM32 5V pin)
                    │                      │
    I_measured ◄────│ IP-          GND ────┼──── GND
                    │                      │
                    │             OUT ─────┼──── R_filter (1kΩ) ──► ADC Pin
                    │                      │                   │
                    │            FILTER ───┼──── C_ext (1nF)   C_filter (10nF)
                    │                      │                   │
                    └──────────────────────┘                  GND
```

### Specifications

| Parameter | ACS712-5A | ACS712-20A |
|-----------|-----------|------------|
| Sensitivity | 185 mV/A | 100 mV/A |
| Zero-current output | 2.5V (VCC/2) | 2.5V (VCC/2) |
| Measurement range | ±5A | ±20A |
| Bandwidth | 80 kHz | 80 kHz |
| Supply voltage | 5V | 5V |
| Output at max current | 2.5V ± 0.925V | 2.5V ± 2.0V |

### Current Calculation (Firmware)

$$I_{actual} = \frac{V_{sensor} - V_{zero}}{\text{Sensitivity}} = \frac{V_{sensor} - 2.5}{0.185} \quad [\text{Amperes}]$$

### Two Current Sensors Used

| Instance | Measures | ADC Channel | STM32 Pin | Position |
|----------|----------|-------------|-----------|----------|
| ACS712 #1 | Input current | ADC1_CH4 | PA4 | Between supply and converter |
| ACS712 #2 | Output current | ADC1_CH5 | PA5 | Between converter and battery |

> **Note:** ACS712 output is 0–5V but centered at 2.5V. Since STM32 ADC max is 3.3V,
> a voltage divider or level shifter is needed if measuring currents > 4.3A.
> For this project's range (0–3A), output stays within 2.5V ± 0.555V = safe range.

---

## 3. Supercapacitor Monitoring Circuit

### Circuit Diagram

```
    Supercapacitor Stack (4S × 2.7V)
    ┌─────┐  ┌─────┐  ┌─────┐  ┌─────┐
    │ C1  ├──┤ C2  ├──┤ C3  ├──┤ C4  │
    │2.7V │  │2.7V │  │2.7V │  │2.7V │     V_max = 10.8V
    └──┬──┘  └─────┘  └─────┘  └──┬──┘
       │ (+)                       │ (-)
       │                          GND
       │
       ├──── R1 = 30kΩ ──┬── R2 = 10kΩ ──── GND
       │                  │
       │                  ├── C_filter (10nF) ── GND
       │                  │
       │                  └── R_prot (1kΩ) ──► PB0 (ADC1_CH8)
       │
       ├──── To DC-DC converter (energy buffer node)
       │
       └──── Relay/FET switch (enable/disable for A/B testing)
```

### Supercapacitor Specifications

| Parameter | Value | Notes |
|-----------|-------|-------|
| Configuration | 4 cells in series | Balancing resistors recommended |
| Cell capacitance | 50F per cell | Series: C_total = 12.5F |
| Max voltage | 10.8V (4 × 2.7V) | OVP set at 10.8V in firmware |
| Min usable voltage | 2.0V | Below this, energy too low |
| Max stored energy | 729 J | E = ½ × 12.5F × 10.8² |
| Usable energy | 704 J | Between 10.8V and 2.0V |
| ESR (typical) | ~40 mΩ (4 × 10mΩ) | Low loss at high currents |

---

## 4. DC-DC Buck Converter Stage

### Circuit Diagram

```
    V_in ──┬────────────────────────────────────────── V_out
           │            L = 47µH
           │     ┌──────/\/\/\/──────┐
           │     │                   │
           ├─── MOSFET (Q1) ────────┤────── C_out (470µF) ──── GND
           │     │  Gate ← PA8 PWM  │
           │     │                   │
           │    Diode (D1)           │
           │     │ (Schottky)        │
           │     ▼                   │
           │    GND                  │
           │                         │
           └── C_in (100µF) ──── GND
```

### Buck Converter Parameters

| Parameter | Value | Notes |
|-----------|-------|-------|
| Input voltage | 5–12V | From DC supply |
| Output voltage | Configurable (PWM) | Default: 14.4V (CC-CV) |
| Switching frequency | 100 kHz | TIM1 CH1 (PA8) |
| Inductor | 47 µH | Saturation current > 5A |
| Output capacitor | 470 µF electrolytic | Low ESR preferred |
| Input capacitor | 100 µF | Decoupling |
| MOSFET | N-channel, logic-level | R_DS(on) < 20 mΩ |
| Schottky diode | 40V, 5A | Fast recovery |
| Duty cycle range | 5% – 90% | Software-limited |
| Max efficiency | ~92–95% | At optimal load |

---

## 5. Signal Conditioning Summary

All analog signals pass through identical RC low-pass filters before reaching the STM32 ADC:

```
    Sensor ──── R_series (1kΩ) ──┬──── ADC Pin
                                  │
                             C_shunt (10nF)
                                  │
                                 GND
```

| Property | Value |
|----------|-------|
| Filter type | 1st-order RC low-pass |
| Cutoff frequency | 15.9 kHz |
| Purpose | Reject PWM switching noise (100 kHz) |
| Attenuation at 100 kHz | ~-16 dB |
| Pass band | DC to ~1 kHz (measurement signals) |
| Additional protection | Clamp diodes internal to STM32 |
