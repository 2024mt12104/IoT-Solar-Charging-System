# System Architecture

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Institution:** BITS Pilani WILP  

---

## 1. Functional Block Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                         POWER FLOW                                  │
│                                                                     │
│  [DC Power Supply]  ──►  [DC-DC Converter]  ──►  [Supercapacitor]  │
│    (5V–12V, sim.           (Buck/Boost,             (Energy Buffer, │
│     solar input)            PWM-controlled)          Fast buffer)   │
│                                          │                    │     │
│                                          └──────────┬─────────┘     │
│                                                     ▼               │
│                                              [Battery / Load]       │
│                                             (Charging target)       │
└─────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────┐
│                        SENSING & CONTROL                            │
│                                                                     │
│  [Voltage Sensor] ──┐                                               │
│  (Resistor divider) │                                               │
│                     ├──► [STM32F446RE] ──► [PWM Signal]            │
│  [Current Sensor] ──┘    (ADC, PWM,         to DC-DC Converter     │
│  (ACS712/INA219)          Calculations,                             │
│                           Protection)                               │
│                               │                                     │
│                          [UART TX/RX]                               │
└───────────────────────────────┼─────────────────────────────────────┘

┌───────────────────────────────┼─────────────────────────────────────┐
│                        IoT LAYER              ▼                     │
│                                                                     │
│                          [ESP8266 NodeMCU]                          │
│                          (WiFi Gateway,                             │
│                           JSON packaging,                           │
│                           HTTP/MQTT upload)                         │
│                               │                                     │
│                          [WiFi Network]                             │
│                               │                                     │
│                         [Cloud Backend]                             │
│                         (REST API server,                           │
│                          Database)                                  │
│                               │                                     │
│                        [Web Dashboard]                              │
│                        (Real-time charts,                           │
│                         Historical data,                            │
│                         System status)                              │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Hardware Architecture

### 2.1 Input Stage

**Component:** Regulated DC Power Supply (5V–12V)  
**Role:** Simulates the variable output of a solar PV panel.  
**Characteristics:**
- Adjustable voltage to simulate cloud cover / angle variations
- Regulated with minimal ripple for controlled testing
- Current-limited to protect downstream components

### 2.2 DC-DC Converter

**Type:** PWM-controlled Buck or Buck-Boost converter  
**Role:** Regulates voltage level between input source and supercapacitor/load  
**Key Specifications:**
- Input: 5–15V DC
- Output: Adjustable (controlled by STM32 PWM duty cycle)
- Switching frequency: 20–100 kHz (configurable)
- Protection: Overvoltage, overcurrent detection

**Control Interface:**
- PWM signal from STM32 TIM peripheral
- Duty cycle adjusted in real time based on voltage/current feedback

### 2.3 Supercapacitor Energy Buffer

**Type:** Electric Double-Layer Capacitor (EDLC)  
**Role:** Absorbs input energy transients; provides short-term backup during input dips  
**Specifications:**
- Rated voltage: 2.7V per cell (cells in series for higher voltage)
- Capacitance: 10–100 F
- Energy stored: E = ½ × C × V²
- Time constant: τ = R_ESR × C

**Operating Principle:**
- Charges rapidly when input > supercap voltage
- Discharges to maintain battery charging current during input drops
- Acts as a "shock absorber" for voltage transients

### 2.4 Battery / Load

**Type:** Low-voltage battery or resistive load (prototype)  
**Role:** Target of the charging system  
**Specifications (prototype):**
- Voltage range: 3.7V–7.4V (Li-ion equivalent)
- Capacity: Small (< 1 Ah for prototype)

### 2.5 Sensing Circuit

#### Voltage Sensing
- **Method:** Resistive voltage divider (R1, R2) + STM32 ADC input
- **Formula:** V_measured = V_actual × R2 / (R1 + R2)
- **Scaling:** Calibrated to ADC reference voltage (3.3V)

#### Current Sensing
- **Method:** ACS712 Hall-effect sensor or INA219 I2C module
- **Output:** Analog voltage proportional to current (ACS712) or I2C data (INA219)
- **Range:** 0–5A (ACS712-5A variant) or configurable (INA219)

---

## 3. Firmware Architecture (STM32F446RE)

### 3.1 Module Structure

```
main.c
  ├── System initialization (HAL_Init, clock config)
  ├── Peripheral initialization (ADC, TIM/PWM, UART)
  ├── Main control loop (< 10 ms cycle)
  │     ├── sensor_driver.c   → Read V, I via ADC (DMA)
  │     ├── power_calc.c      → Compute P = V×I, E = ΣP×Δt
  │     ├── supercap_monitor.c → Track supercap state, SoC
  │     ├── pwm_control.c     → Adjust duty cycle (feedback)
  │     └── communication.c   → Package & transmit via UART
  └── Interrupt handlers (protection, ADC DMA complete)
```

### 3.2 Control Loop Flow

```
START
  │
  ▼
Initialize Peripherals
  │
  ▼
┌─────────────────────────────┐
│         MAIN LOOP            │
│  (Target: < 10 ms/cycle)     │
│                             │
│  1. Read ADC (V, I)         │
│  2. Filter & calibrate      │
│  3. Compute P = V × I       │
│  4. Update E += P × Δt      │
│  5. Check protection limits │
│     - V > V_max? → Shutdown │
│     - I > I_max? → Limit    │
│  6. Adjust PWM duty cycle   │
│  7. Update supercap state   │
│  8. Transmit via UART       │
└─────────────────────────────┘
  │
  ▼
Interrupt Handlers:
  - ADC DMA complete → Update sensor buffer
  - UART TX complete → Ready next packet
  - Protection ISR  → Emergency stop
```

### 3.3 STM32 Peripheral Usage

| Peripheral | Function | Configuration |
|-----------|----------|---------------|
| ADC1 | Voltage sensing (Ch0, Ch1) | DMA, 12-bit, continuous |
| ADC2 | Current sensing (Ch2, Ch3) | DMA, 12-bit, continuous |
| TIM1 | PWM for DC-DC converter | Center-aligned, 20 kHz |
| USART2 | Serial to ESP8266 | 115200 baud, 8N1 |
| TIM2 | Timestamp / tick counter | 1 µs resolution |
| SysTick | Control loop timing | 1 ms base tick |

---

## 4. IoT Gateway Architecture (ESP8266)

### 4.1 Functional Roles

1. **UART Reception:** Receives JSON-formatted data packets from STM32 via serial
2. **WiFi Management:** Maintains connection to local WiFi network with auto-reconnect
3. **Data Packaging:** Adds timestamps, device ID, and metadata to received data
4. **Cloud Upload:** Sends data via HTTP POST or MQTT to backend server
5. **Error Handling:** Buffers data locally (SPIFFS) if cloud is unreachable

### 4.2 Data Flow

```
STM32 → [UART JSON] → ESP8266 → [WiFi] → Cloud API
                         │
                    [SPIFFS buffer]
                    (if offline)
```

### 4.3 JSON Data Packet Format

```json
{
  "device_id": "solar-charger-01",
  "timestamp": 1713340800,
  "session_id": "sess_20260417_001",
  "voltage_in": 11.45,
  "voltage_supercap": 8.72,
  "voltage_battery": 7.10,
  "current_in": 1.23,
  "current_out": 0.98,
  "power_in": 14.08,
  "power_out": 9.76,
  "energy_total_wh": 0.342,
  "efficiency_pct": 69.3,
  "pwm_duty_cycle": 72,
  "supercap_soc_pct": 85.2,
  "system_state": "CHARGING",
  "protection_flags": 0
}
```

---

## 5. Cloud Backend Architecture

### 5.1 REST API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/data` | POST | Ingest sensor data from ESP8266 |
| `/api/data/latest` | GET | Fetch most recent reading |
| `/api/data/history` | GET | Time-range query (pagination) |
| `/api/session` | GET | Current charging session info |
| `/api/status` | GET | System health check |

### 5.2 Database Schema

**Table: `sensor_readings`**

| Column | Type | Description |
|--------|------|-------------|
| id | INTEGER PK | Auto-increment |
| timestamp | DATETIME | UTC timestamp |
| session_id | TEXT | Charging session ID |
| voltage_in | REAL | Input voltage (V) |
| voltage_supercap | REAL | Supercap voltage (V) |
| voltage_battery | REAL | Battery voltage (V) |
| current_in | REAL | Input current (A) |
| current_out | REAL | Output current (A) |
| power_in | REAL | Input power (W) |
| power_out | REAL | Output power (W) |
| energy_wh | REAL | Cumulative energy (Wh) |
| efficiency_pct | REAL | System efficiency (%) |
| pwm_duty | INTEGER | PWM duty cycle (0–100) |
| supercap_soc | REAL | Supercapacitor SoC (%) |
| system_state | TEXT | State (CHARGING/IDLE/FAULT) |

---

## 6. Frontend Dashboard Architecture

### 6.1 Components

| Component | Description |
|-----------|-------------|
| `VoltageChart` | Real-time line chart for V_in, V_supercap, V_battery |
| `CurrentChart` | Real-time line chart for I_in, I_out |
| `PowerGauge` | Gauge showing instantaneous input/output power |
| `EnergyMeter` | Counter showing cumulative energy transferred |
| `EfficiencyMeter` | % efficiency (P_out / P_in × 100) |
| `StatusPanel` | System state, protection flags, PWM duty cycle |
| `HistoryView` | Selectable time-range with download option |

### 6.2 Data Refresh Strategy

- **Real-time:** WebSocket or Server-Sent Events (SSE) at 1–5 Hz
- **Historical:** REST API call on demand
- **Alerts:** Browser notification on protection flags

---

## 7. Communication Interfaces Summary

| Interface | Devices | Protocol | Speed |
|-----------|---------|----------|-------|
| ADC | Sensors → STM32 | Analog (12-bit) | 1 kHz sampling |
| PWM | STM32 → DC-DC | PWM signal | 20 kHz carrier |
| UART | STM32 → ESP8266 | Serial 8N1 | 115200 baud |
| WiFi | ESP8266 → Router | IEEE 802.11 b/g/n | Up to 72 Mbps |
| HTTP/MQTT | ESP8266 → Cloud | REST / MQTT | Depends on network |
| WebSocket | Cloud → Browser | WS/WSS | Real-time |

---

*Document Version: 1.0 | Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
