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

```
[DC Power Supply / Solar Input (5V–12V)]
            |
      [DC-DC Converter]  ←── PWM Control ──→ [STM32F446RE]
            |                                      |
   [Supercapacitor Buffer]              [Voltage & Current Sensors]
            |                                      |
       [Battery / Load]           [UART] → [ESP8266 WiFi Module]
                                                   |
                                         [Cloud Dashboard / Analytics]
```

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
│   ├── DESIGN_CONSIDERATIONS.md           # Design rationale
│   └── LITERATURE_REVIEW.md               # Annotated literature review
├── firmware/
│   ├── stm32/                             # STM32 firmware (C/HAL)
│   └── esp8266/                           # ESP8266 IoT gateway (Arduino)
├── cloud/
│   ├── backend/                           # Node.js/Python API server
│   └── frontend/                          # React dashboard
├── hardware/
│   ├── schematics/                        # Circuit diagrams
│   └── bill_of_materials.md               # Component list with specs
├── simulation/
│   └── demo/                              # Software simulation & demo
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
- **Cloud/Backend:** Node.js, Python (Flask/FastAPI)
- **Frontend:** React.js, Chart.js
- **Simulation:** Python, Matplotlib, NumPy
- **Documentation:** Markdown, LaTeX (optional)
- **Version Control:** Git, GitHub

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
