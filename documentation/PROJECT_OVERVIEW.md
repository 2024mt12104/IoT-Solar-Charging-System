# Project Overview

**Project Title:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Guide:** Dr. Somasundaram Iyyamperumal  
**Institution:** BITS Pilani WILP, Department of Engineering  
**Semester:** 2, 2026  

---

## 1. Introduction

### 1.1 Background

The proliferation of electric vehicles (EVs) and battery-powered devices has accelerated the demand for efficient and reliable charging infrastructure. Simultaneously, the integration of renewable energy sources—particularly solar photovoltaic (PV) systems—into charging stations offers significant potential for sustainable energy management. However, solar energy is inherently intermittent, and direct coupling of PV systems to battery chargers can result in voltage fluctuations, reduced charging efficiency, and accelerated battery degradation.

Supercapacitors (also known as ultracapacitors or electric double-layer capacitors, EDLCs) have emerged as a compelling solution for energy buffering due to their high power density, fast charge/discharge capability, and long cycle life. By interposing a supercapacitor between the solar/DC input and the battery load, transient power mismatches can be absorbed, providing a smoother and more stable charging current.

### 1.2 Problem Statement

Current small-scale and residential solar charging systems face the following challenges:

1. **Grid Limitations:** Voltage sags and fluctuations in low-quality grid supplies affect charger performance.
2. **Intermittent Solar Availability:** Cloud cover and angle changes cause rapid variations in PV output.
3. **Lack of Real-Time Monitoring:** Most small-scale systems do not provide live data on charging state, efficiency, or anomalies.
4. **Limited Adaptive Control:** Existing systems lack intelligent control that adjusts to changing input conditions.
5. **No Integrated IoT Interface:** Remote monitoring and data logging are absent in most prototype systems.

### 1.3 Motivation

- Growing global adoption of EVs and distributed energy resources (DERs)
- Need for cost-effective, smart charging solutions in power-constrained environments
- Academic opportunity to integrate embedded systems, power electronics, and IoT in a single platform
- Alignment with national and global energy efficiency and sustainability goals

---

## 2. Objectives

1. **Study** conventional battery charging mechanisms and identify their limitations in renewable energy contexts.
2. **Design** a solar-assisted smart charging system that incorporates supercapacitor energy buffering for improved charging stability.
3. **Implement** real-time monitoring and adaptive control using an STM32 embedded controller (ADC, PWM, UART).
4. **Develop** an IoT-based edge gateway using ESP8266 for cloud connectivity, data visualization, and remote logging.
5. **Evaluate** and compare charging performance metrics (efficiency, ripple, response time) in buffered versus non-buffered configurations.

---

## 3. Scope of Work

### 3.1 In Scope

- **Scaled low-voltage prototype** (5–12 V DC) for safe laboratory demonstration
- **Hardware integration** using standard, commercially available components
- **Embedded firmware development** in C using STM32 HAL library
- **IoT gateway firmware** for ESP8266 (Arduino framework)
- **Cloud dashboard creation** for real-time and historical data visualization
- **Experimental analysis** comparing buffered vs. non-buffered charging scenarios

### 3.2 Out of Scope

- Large-scale grid integration or high-voltage EV charging
- Custom PCB design (breadboard/module-based prototyping only)
- Machine learning or advanced AI-based optimization (future work)
- Multi-battery management systems (single battery/load prototype)

---

## 4. Novelty and Contributions

| Aspect | Conventional Approach | This Project |
|--------|-----------------------|--------------|
| Energy Input | Fixed DC supply | Regulated DC simulating solar variability |
| Buffering | None or simple capacitor | Supercapacitor with monitored state |
| Controller | Simple analog circuit | Digital STM32 with PWM + ADC |
| Monitoring | Bench multimeter | Real-time IoT dashboard |
| Data Logging | Manual notes | Automated cloud logging |
| Control | Open-loop | Closed-loop with protection logic |

**Unified Contribution:** A fully integrated, low-cost embedded + IoT solution that enables real-time energy management for a solar-assisted charging prototype.

---

## 5. Expected Outcomes

1. **STM32 Firmware** with modules for sensor reading, PWM control, power calculation, and UART communication
2. **ESP8266 IoT Gateway Code** for WiFi connectivity and cloud data transmission
3. **Cloud-Based Monitoring Dashboard** with real-time charts, historical trends, and system status
4. **Complete Technical Documentation** including architecture, design rationale, and user guides
5. **Experimental Results** and performance analysis comparing buffered vs. non-buffered operation
6. **GitHub Repository** with full source code, data, and documentation
7. **Project Report and Presentation Materials** ready for academic evaluation

---

## 6. Project Timeline

| Phase | Activity | Target Period | Status |
|-------|----------|---------------|--------|
| Phase 1 | Design & Planning | Feb 2026 | ✅ Completed |
| Phase 2 | Hardware Setup & Sourcing | Mar 2026 | 🔄 In Progress |
| Phase 3 | Firmware Development | Mar–Apr 2026 | 🔄 In Progress |
| Phase 4 | IoT Integration | Apr 2026 | 📅 Upcoming |
| Phase 5 | Testing & Validation | Apr–May 2026 | 📅 Upcoming |
| Phase 6 | Final Documentation & Presentation | May 2026 | 📅 Upcoming |

---

## 7. Tools and Technologies

| Category | Tools |
|----------|-------|
| Embedded IDE | STM32CubeIDE |
| IoT Firmware | Arduino IDE |
| Backend | Python (Flask) / Node.js (Express) |
| Frontend | React.js, Chart.js |
| Simulation | Python, NumPy, Matplotlib |
| Database | SQLite / InfluxDB (cloud) |
| Version Control | Git, GitHub |
| Documentation | Markdown |

---

## 8. Challenges and Mitigation Strategies

| Challenge | Mitigation |
|-----------|------------|
| Component availability | Using readily available IoT modules (STM32 Nucleo, ESP8266 NodeMCU) |
| Power constraints in low-voltage system | Supercapacitor buffering to absorb transients |
| Real-time monitoring complexity | Optimized STM32 firmware with < 10 ms control loop |
| Cloud connectivity in research lab | WiFi-based ESP8266 with flexible backend options |
| System complexity management | Modular architecture with clearly defined interfaces |

---

*Document Version: 1.0 | Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
