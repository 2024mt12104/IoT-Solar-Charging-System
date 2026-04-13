# Design Considerations

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Institution:** BITS Pilani WILP  

---

## 1. Introduction

The design of this system balances **simplicity**, **safety**, **modularity**, and **educational clarity**. Each design decision was made with the practical constraints of a university prototype in mind, while ensuring the system remains a credible demonstration of the proposed concepts.

---

## 2. Design Consideration 1: Low-Power Prototype for Safety

**Principle:** The system operates within safe, low-voltage DC levels (5–12 V) to eliminate high-voltage safety risks in a laboratory environment.

**Implications:**
- No mains AC voltage handling required
- Standard bench power supply serves as the solar source simulator
- All components rated within 5–15 V DC range
- Safe for handling and demonstration without specialized safety equipment

**Trade-offs:**
- Results are at prototype scale, not representative of real-world EV charging currents
- Efficiency figures may differ from full-scale systems due to scaling effects

**Mitigation:** Results are presented as relative comparisons (buffered vs. non-buffered) rather than absolute efficiency claims, which remains valid across scales.

---

## 3. Design Consideration 2: Modular Architecture

**Principle:** The system is divided into independent functional modules, each testable and replaceable without affecting others.

**Module Breakdown:**

| Module | Hardware | Firmware File | Testability |
|--------|----------|---------------|-------------|
| Sensing | Voltage divider, ACS712 | `sensor_driver.c` | Standalone with bench supply |
| Control | DC-DC converter | `pwm_control.c` | Scope measurement of PWM |
| Buffering | Supercapacitor | `supercap_monitor.c` | Bypassed for baseline tests |
| Computation | STM32 CPU | `power_calc.c` | Unit-tested in software |
| Communication | ESP8266 | `communication.c` / `iot_gateway.ino` | Tested with serial monitor |
| Visualization | Dashboard | React app | Testable with mock data |

**Benefits:**
- Development can proceed in parallel on different modules
- A fault in one module does not halt development of others
- Easy to swap components (e.g., replace ACS712 with INA219) without full redesign

---

## 4. Design Consideration 3: Real-Time Continuous Monitoring

**Principle:** Continuous, high-frequency monitoring of voltage (V), current (I), power (P), and energy (E) ensures accurate characterization of charging behavior.

**Sampling Strategy:**

| Signal | Sampling Rate | Method |
|--------|--------------|--------|
| Input voltage | 1 kHz | STM32 ADC + DMA |
| Input current | 1 kHz | STM32 ADC + DMA |
| Supercap voltage | 1 kHz | STM32 ADC + DMA |
| Battery voltage | 100 Hz | Polled ADC |
| Cloud transmission | 1 Hz | UART → ESP8266 → WiFi |

**Why 1 kHz?**
- Supercapacitor charge/discharge dynamics occur on millisecond timescales
- PWM ripple at 20 kHz is filtered before ADC but transients at ~1–10 ms are captured
- Balance between data resolution and firmware processing capacity

---

## 5. Design Consideration 4: PWM-Based Regulation

**Principle:** PWM control of the DC-DC converter provides a flexible, software-configurable method to regulate output voltage and current.

**Advantages of PWM Control:**
- Easily adjustable duty cycle in firmware without hardware changes
- Enables both open-loop (fixed duty cycle) and closed-loop (PID) control
- Low power dissipation compared to linear regulators
- Wide range of DC-DC converter modules available with PWM enable pins

**PWM Configuration:**
- Timer: STM32 TIM1 (advanced control timer, precise control)
- Frequency: 20 kHz (above human hearing, below ADC sampling complications)
- Resolution: 16-bit (65535 steps) → very fine duty cycle control

**Protection Integration:**
- PWM can be disabled instantly via hardware pin (emergency stop)
- Duty cycle ramping prevents inrush current on startup

---

## 6. Design Consideration 5: Basic Protection Mechanisms

**Principle:** Software-based protection prevents damage to components and ensures safe demo operation.

**Protection Hierarchy:**

```
Level 1: Warning (log event, reduce PWM)
Level 2: Soft shutdown (disable PWM, alert dashboard)
Level 3: Hard fault (disable all outputs, require manual reset)
```

**Monitored Parameters:**

| Parameter | Warning | Soft Shutdown | Hard Fault |
|-----------|---------|---------------|------------|
| Input Voltage | > 13.0V | > 13.5V | > 14.0V |
| Output Voltage | > 8.0V | > 8.5V | > 9.0V |
| Input Current | > 2.0A | > 2.3A | > 2.5A |
| Supercap Voltage | > V_max×1.05 | > V_max×1.10 | > V_max×1.15 |

**Watchdog Timer:**
- STM32 IWDG (Independent Watchdog) configured to reset the system if firmware hangs
- Ensures the system returns to a known safe state after any unexpected error

---

## 7. Design Consideration 6: Standard Communication Interfaces

**Principle:** Use widely supported, documented interfaces to ensure reliability and ease of debugging.

**Choices and Rationale:**

| Interface | Standard | Alternative Considered | Reason for Choice |
|-----------|----------|----------------------|-------------------|
| STM32 to ESP8266 | UART 115200 baud | SPI, I2C | Simplest, only 2 wires (TX/RX), widely used |
| Data format | JSON | Binary, CSV | Human-readable, easy to parse on both sides |
| ESP8266 to Cloud | HTTP REST | MQTT | Simpler setup, no broker needed for prototype |
| Dashboard updates | WebSocket | REST polling | Real-time capability with low overhead |

**Error Handling:**
- UART: Checksum or start/end delimiters for packet validation
- WiFi: Auto-reconnect with exponential backoff
- Cloud: Local buffering (SPIFFS) when offline

---

## 8. Design Consideration 7: Noise Minimization

**Principle:** Accurate ADC readings require minimization of electromagnetic interference (EMI) from the switching converter and other sources.

**Strategies:**

| Strategy | Implementation |
|----------|----------------|
| Moving average filter | 16-sample window in firmware |
| EMA filter | α = 0.2 for slow-response signals |
| Analog ground separation | Separate AGND plane for sensor signals |
| Decoupling capacitors | 100nF + 10µF at each IC power pin |
| Shielded signal cables | For voltage/current sensor lines |
| RC low-pass filter | On ADC inputs: R=1kΩ, C=10nF → f_c = 16 kHz |
| Grounding star topology | All ground connections to single point |

**RC Filter Cutoff Frequency:**
```
f_c = 1 / (2π × R × C) = 1 / (2π × 1000 × 10×10⁻⁹) ≈ 15.9 kHz
```

This filters PWM switching noise (20 kHz) while passing useful charging dynamics (< 1 kHz).

---

## 9. Design Consideration 8: Supercapacitor Integration Strategy

**Principle:** The supercapacitor is integrated as a buffering element between the DC-DC converter output and the battery, with active monitoring of its state.

**Integration Points:**
- Supercapacitor connected in parallel with battery charging node
- Separate voltage measurement channel for supercapacitor monitoring
- Energy contribution calculated as: ΔE_sc = ½ × C × (V₁² - V₂²)
- Toggle mechanism (relay or software-controlled FET) to enable/disable buffer for experimental comparison

**Testing Protocol:**
1. Baseline test: Supercapacitor bypassed, direct DC-DC to battery
2. Buffered test: Supercapacitor connected, same input profile
3. Comparison: Voltage ripple, charge time, efficiency, input current stability

---

## 10. Design Consideration 9: Simplicity and Code Quality

**Principle:** Clear, well-documented code is as important as functional code for a research prototype.

**Coding Standards:**
- C99 standard for STM32 firmware
- Meaningful variable names (e.g., `voltage_input_v` not `v1`)
- All physical units in variable names (suffix `_v`, `_a`, `_w`, `_wh`)
- Function-level comments explaining algorithm and purpose
- No magic numbers—all thresholds defined as `#define` constants

**Documentation Standard:**
- Each module has a corresponding `.h` header with function descriptions
- README in each directory explains build and test steps
- All formulas documented in `COMPUTATION_METHODOLOGY.md`

---

## 11. Design Consideration 10: Cost-Effectiveness and Availability

**Principle:** Use commercially available, low-cost components to ensure the design is reproducible and practical.

**Component Cost Summary (Approximate):**

| Component | Estimated Cost (INR) |
|-----------|---------------------|
| STM32F446RE Nucleo Board | ₹2,000–₹3,000 |
| ESP8266 NodeMCU | ₹200–₹400 |
| DC-DC Buck Converter Module | ₹100–₹300 |
| Supercapacitor (2.7V, 10–100F) | ₹500–₹2,000 |
| ACS712 Current Sensor | ₹100–₹200 |
| Resistors, capacitors, wires | ₹200–₹500 |
| Breadboard | ₹100–₹200 |
| **Total Estimate** | **₹3,200–₹6,600** |

**Alternative Components (if primary unavailable):**

| Primary | Alternative |
|---------|-------------|
| STM32F446RE | STM32F401RE or STM32F103C8T6 (Blue Pill) |
| ACS712 | INA219 (I2C, higher accuracy) |
| ESP8266 | ESP32 (more RAM, dual-core) |
| Single supercap | Series/parallel combination of smaller units |

---

*Document Version: 1.0 | Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
