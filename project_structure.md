# Repository Structure Guide

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Institution:** BITS Pilani WILP, Engineering  

---

## Top-Level Layout

```
IoT-Solar-Charging-System/
├── presentations/          # Viva and conference presentations
├── documentation/          # Project documentation and reports
├── firmware/               # Embedded firmware source code
├── cloud/                  # Backend API and frontend dashboard
├── hardware/               # Schematics and BOM
├── simulation/             # Software simulation & demo scripts
├── data/                   # Experimental data and calibration logs
├── project_structure.md    # This file
└── README.md               # Main repository readme
```

---

## Directory Details

### `/presentations/`
Stores all slide decks used for academic and project milestones.

| File | Description |
|------|-------------|
| `Mid_Sem_Viva_Presentation.pptx` | Mid-semester viva (April 17, 2026) |
| `README.md` | Guide for presenting, key talking points |

---

### `/documentation/`
All project documentation files written in Markdown for version control compatibility.

| File | Description |
|------|-------------|
| `PROJECT_OVERVIEW.md` | High-level project description, goals, and scope |
| `SYSTEM_ARCHITECTURE.md` | Detailed hardware/software architecture |
| `COMPUTATION_METHODOLOGY.md` | Mathematical formulations and algorithms |
| `DESIGN_CONSIDERATIONS.md` | Design rationale and trade-off analysis |
| `LITERATURE_REVIEW.md` | Annotated bibliography and research gaps |

---

### `/firmware/`
Embedded source code for the two main controllers.

```
firmware/
├── stm32/
│   ├── Core/
│   │   ├── Src/
│   │   │   ├── main.c                  # Main application entry
│   │   │   ├── sensor_driver.c         # ADC voltage/current acquisition
│   │   │   ├── pwm_control.c           # DC-DC converter PWM regulation
│   │   │   ├── supercap_monitor.c      # Supercapacitor state management
│   │   │   ├── power_calc.c            # P = V×I, E = ∫P dt calculations
│   │   │   └── communication.c         # UART serial to ESP8266
│   │   └── Inc/
│   │       ├── sensor_driver.h
│   │       ├── pwm_control.h
│   │       ├── supercap_monitor.h
│   │       ├── power_calc.h
│   │       └── communication.h
│   ├── Drivers/                        # STM32 HAL drivers (auto-generated)
│   └── README.md                       # Build & flash instructions
└── esp8266/
    ├── iot_gateway/
    │   ├── iot_gateway.ino             # Main Arduino sketch
    │   ├── config.h                    # WiFi credentials & API endpoints
    │   ├── wifi_manager.h              # WiFi connection handling
    │   └── cloud_uploader.h            # HTTP/MQTT cloud transmission
    └── README.md                       # Setup & configuration guide
```

**Key Firmware Modules:**

| Module | File | Responsibility |
|--------|------|----------------|
| Sensor Driver | `sensor_driver.c` | ADC sampling, filtering, calibration |
| PWM Control | `pwm_control.c` | Duty cycle regulation, protection |
| Supercap Monitor | `supercap_monitor.c` | Buffer state, energy tracking |
| Power Calculator | `power_calc.c` | Real-time P, E, efficiency metrics |
| Communication | `communication.c` | UART framing and transmission |
| IoT Gateway | `iot_gateway.ino` | WiFi, JSON packaging, cloud upload |

---

### `/cloud/`
Backend API server and frontend dashboard for remote monitoring.

```
cloud/
├── backend/
│   ├── app.py / server.js              # REST API (Flask or Node.js/Express)
│   ├── models/                         # Data models (sensor readings, sessions)
│   ├── routes/                         # API endpoints (/data, /status, /history)
│   ├── database/                       # SQLite or InfluxDB config
│   └── README.md                       # Backend setup guide
└── frontend/
    ├── src/
    │   ├── App.jsx                      # React root component
    │   ├── components/
    │   │   ├── VoltageChart.jsx         # Real-time voltage chart
    │   │   ├── CurrentChart.jsx         # Real-time current chart
    │   │   ├── PowerGauge.jsx           # Instantaneous power gauge
    │   │   ├── EnergyMeter.jsx          # Cumulative energy counter
    │   │   └── StatusPanel.jsx          # System state indicator
    │   └── utils/
    │       └── api.js                   # API call helpers
    ├── public/
    └── README.md                        # Frontend setup guide
```

**API Endpoints (Planned):**

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/api/data` | POST | Receive sensor data from ESP8266 |
| `/api/data/latest` | GET | Fetch latest reading |
| `/api/data/history` | GET | Retrieve historical data |
| `/api/status` | GET | System health status |
| `/api/session` | GET/POST | Charging session management |

---

### `/hardware/`
Circuit schematics, PCB layouts, and component specifications.

```
hardware/
├── schematics/
│   ├── system_schematic.pdf            # Full circuit diagram
│   ├── stm32_connections.pdf           # STM32 pin mapping
│   └── sensor_circuits.pdf             # Sensor interface circuits
├── pcb_layout/                         # PCB files (KiCad/Eagle, if applicable)
└── bill_of_materials.md                # Component list with part numbers & costs
```

**Bill of Materials Summary:**

| Component | Model | Qty | Purpose |
|-----------|-------|-----|---------|
| Microcontroller | STM32F446RE Nucleo | 1 | Main controller |
| WiFi Module | ESP8266 NodeMCU | 1 | IoT gateway |
| DC-DC Converter | Adjustable Buck Module | 1 | Voltage regulation |
| Supercapacitor | 2.7V, 10–100F | 1–2 | Energy buffer |
| Current Sensor | ACS712 / INA219 | 2 | I measurement |
| Voltage Divider | Resistor pair (R1,R2) | 2 sets | V measurement |
| Power Supply | Adjustable DC, 5–12V | 1 | Input source |
| Breadboard/PCB | Standard | 1 | Prototyping |
| Connecting Wires | Jumper wires | — | Interconnects |

---

### `/simulation/`
Software simulation scripts for demonstrating system behavior without hardware.

```
simulation/
├── demo/
│   ├── stm32_simulator.py              # Simulates STM32 sensor readings & control
│   ├── charging_model.py               # Realistic battery/supercap charging curves
│   ├── scenario_generator.py           # Test scenario creation
│   └── README.md                       # How to run the simulation
└── data/
    └── simulated_charging_data.csv     # Sample output data
```

---

### `/data/`
Experimental measurement logs and calibration data.

```
data/
├── test_results/
│   ├── buffered_charging/              # Results with supercapacitor buffer
│   ├── unbuffered_charging/            # Baseline without buffer
│   └── comparison_analysis/           # Performance comparison
└── calibration_data/
    ├── voltage_calibration.csv
    └── current_calibration.csv
```

---

## Naming Conventions

| Item | Convention | Example |
|------|-----------|---------|
| C source files | `snake_case.c` | `sensor_driver.c` |
| C header files | `snake_case.h` | `sensor_driver.h` |
| Python scripts | `snake_case.py` | `charging_model.py` |
| React components | `PascalCase.jsx` | `VoltageChart.jsx` |
| Documentation | `UPPER_SNAKE.md` | `SYSTEM_ARCHITECTURE.md` |
| Data files | `descriptive_name.csv` | `buffered_charging_run1.csv` |

---

## Development Status

| Component | Status | Notes |
|-----------|--------|-------|
| Documentation | ✅ Complete | All docs created |
| Presentations | ✅ Complete | Mid-sem viva ready |
| STM32 Firmware | 🔄 In Progress | ADC, PWM modules underway |
| ESP8266 Gateway | 🔄 In Progress | Communication protocol designed |
| Cloud Backend | 📅 Planned | API design finalized |
| Frontend Dashboard | 📅 Planned | Mockup available |
| Hardware Assembly | 📅 Planned | Components being sourced |
| Simulation Demo | 📅 Planned | Architecture designed |

---

*Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
