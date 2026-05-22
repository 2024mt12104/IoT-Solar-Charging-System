# IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering

[![BITS WILP](https://img.shields.io/badge/BITS%20WILP-Engineering-blue)](https://www.bits-pilani.ac.in/wilp/)
[![Semester](https://img.shields.io/badge/Semester-2%2C%202026-green)](https://github.com/2024mt12104/IoT-Solar-Charging-System)
[![Status](https://img.shields.io/badge/Status-In%20Progress-orange)](https://github.com/2024mt12104/IoT-Solar-Charging-System)

---

## Project Overview

This project presents a **solar-assisted smart battery charging system** integrating **supercapacitor energy buffering** with **IoT-based real-time monitoring and control**. The system addresses the challenges of intermittent solar power availability and grid voltage fluctuations by using a supercapacitor as an energy buffer between the solar/DC input and the battery load.

**Key Innovation:** Combining supercapacitor buffering with embedded STM32 control and cloud-based IoT monitoring in a unified, low-cost prototype.

---

## Student Information

| Field | Details |
|-------|---------|
| **Student** | Ajeesh Kumar R |
| **Roll Number** | 2024MT12104 |
| **Guide** | Dr. Somasundaram Iyyamperumal |
| **Institution** | BITS Pilani WILP |
| **Department** | Engineering |
| **Semester** | 2, 2026 |

---

## System Architecture

![System Architecture Diagram](documentation/images/System_Architecture_Diagram.png)

---

## Objectives

1. Study conventional charging mechanisms and their limitations
2. Design a solar-assisted smart charging system with supercapacitor buffering
3. Implement real-time monitoring and control using STM32 embedded controller
4. Develop an IoT-based edge gateway (ESP8266) for data visualization and logging
5. Experimentally evaluate charging performance (buffered vs. non-buffered)

---

## Repository Structure

```
IoT-Solar-Charging-System/
├── presentations/
│   ├── Mid_Sem_Viva_Presentation.pptx     # Mid-semester viva presentation
│   └── README.md                          # Presentation guide
├── documentation/
│   ├── PROJECT_OVERVIEW.md                # Complete project documentation
│   ├── SYSTEM_ARCHITECTURE.md             # Detailed architecture guide
│   ├── COMPUTATION_METHODOLOGY.md         # Computation methods
│   ├── Computations_Complete_Guide.md     # End-to-end computation reference
│   ├── DESIGN_CONSIDERATIONS.md           # Design rationale
│   └── LITERATURE_REVIEW.md               # Annotated literature review
├── firmware/
│   ├── stm32/                             # STM32 firmware (C/HAL)
│   └── esp8266/                           # ESP8266 IoT gateway (Arduino)
├── cloud/
│   ├── backend/                           # Python Flask REST API server
│   └── frontend/                          # React dashboard
├── ml_model/
│   ├── charging_time_prediction.ipynb     # ML pipeline notebook
│   ├── dataset_generation.py              # Synthetic dataset generator
│   ├── charging_dataset.csv               # Generated dataset (1000 samples)
│   ├── requirements.txt                   # ML dependencies
│   └── README.md                          # ML module documentation
├── hardware/
│   ├── schematics/                        # Circuit diagrams
│   └── bill_of_materials.md               # Component list with specs
├── simulation/
│   ├── demo/                              # Software simulation & demo
│   └── data/                              # Simulated scenario CSVs
├── data/
│   └── test_results/                      # Experimental data logs
├── project_structure.md                   # Repository organization guide
└── README.md                              # This file
```

---

## Key Components

| Component | Model | Function |
|-----------|-------|----------|
| Microcontroller | STM32F446RE (Nucleo) | Central processing, PWM, ADC |
| IoT Gateway | ESP8266 (NodeMCU) | WiFi communication, cloud upload |
| DC-DC Converter | Adjustable Buck/Boost | Voltage regulation |
| Supercapacitor | 2.7V / 10–100F | Energy buffering |
| Voltage Sensor | Resistor divider / INA219 | Voltage measurement |
| Current Sensor | ACS712 / INA219 | Current measurement |
| Power Supply | Regulated DC 5–12V | Input source (simulates solar) |

---

## Project Phases

| Phase | Description | Status |
|-------|-------------|--------|
| Phase 1 | Design & Planning | ✅ Completed (Feb 2026) |
| Phase 2 | Hardware Setup | 🔄 In Progress (Mar 2026) |
| Phase 3 | Firmware Development | 🔄 In Progress (Mar–Apr 2026) |
| Phase 4 | IoT Integration | 📅 Next (Apr 2026) |
| Phase 5 | Testing & Validation | 📅 Next (Apr–May 2026) |
| Phase 6 | Final Documentation | 📅 Upcoming (May 2026) |

---

## Tools & Technologies

- **Embedded:** STM32CubeIDE, HAL Library, C/C++
- **IoT Gateway:** Arduino IDE, ESP8266 SDK
- **Cloud/Backend:** Python (Flask), REST API
- **Frontend:** React.js, Chart.js
- **Simulation:** Python, Matplotlib, NumPy
- **ML/Analytics:** scikit-learn, XGBoost, Pandas
- **Documentation:** Markdown, LaTeX (optional)
- **Version Control:** Git, GitHub

---

## Quick Start – Environment Setup

```powershell
# One-command setup (installs ALL dependencies):
.\setup_environment.ps1 -Component all

# Or install individual components:
.\setup_environment.ps1 -Component python     # Flask + ML + Simulation
.\setup_environment.ps1 -Component esp8266    # Arduino CLI + ESP8266 board
.\setup_environment.ps1 -Component frontend   # Node.js + React packages
.\setup_environment.ps1 -Component stm32      # Verify ARM toolchain
```

**Or manually:**
```powershell
# Python (Backend + ML + Simulation)
pip install -r requirements_all.txt --user

# Frontend (React Dashboard)
cd cloud/frontend && npm install

# ESP8266 (Arduino CLI)
arduino-cli core install esp8266:esp8266 --additional-urls https://arduino.esp8266.com/stable/package_esp8266com_index.json
```

See `firmware/esp8266/dependencies.txt` and `firmware/stm32/dependencies.txt` for detailed toolchain requirements.

---

## Literature References

1. Q. Hassan et al., "An analysis of photovoltaic/supercapacitor energy systems," *Renewable Energy Reports*, 2022.
2. J. Zhang, "Supercapacitors for renewable energy applications: A review," *Renewable and Sustainable Energy Reviews*, 2023.
3. C. V. V. M. Gopi et al., "Review of battery–supercapacitor hybrid energy storage systems," *Energy Reports*, 2024.
4. K. Piyumal et al., "Supercapacitor assisted hybrid PV systems for efficient energy management," *Electronics*, MDPI, 2021.
5. Baqar et al., "Supercapacitor ageing control in residential microgrid based EV charging systems," *Energies*, MDPI, 2023.

---

## Viva Presentation

The Mid-Semester Viva presentation is available in `presentations/Mid_Sem_Viva_Presentation.pptx`.

**Viva Date:** April 17, 2026  
**Covers:** Introduction, Literature Review, System Architecture, Design Considerations, Research Methodology, Data Collection Plan, Progress Update

---

## License

This project is developed for academic purposes at BITS Pilani WILP.  
© 2026 Ajeesh Kumar R (2024MT12104). All rights reserved.

---

## Academic Disclaimer

```
Project Title:
IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
SUPERCAPACITOR ENERGY BUFFERING

Author:
Ajeesh Kumar R
BITS ID: 2024MT12104

Programme:
M.Tech in Software Systems – Specialization in IoT

Institution:
BITS Pilani – Work Integrated Learning Program (WILP) Division

Academic Purpose Notice:
This source code, dataset, documentation, and associated project files are
developed solely for academic learning, research, experimentation, and project
evaluation purposes under the M.Tech in Software Systems – Specialization in
IoT programme at BITS Pilani WILP.

Ownership Declaration:
The complete project work, including source code, datasets, documentation,
design files, reports, diagrams, and related resources, are the intellectual
work of the above-mentioned author unless otherwise referenced.

Usage Restriction:
Unauthorized copying, redistribution, commercial usage, or modification of
this project material without prior permission from the author is discouraged.
This project is intended strictly for educational and research-oriented use.
```
