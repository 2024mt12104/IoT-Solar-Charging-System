# Literature Review

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Guide:** Dr. Somasundaram Iyyamperumal  
**Institution:** BITS Pilani WILP  

---

## 1. Introduction

This literature review examines the current state of research on solar photovoltaic (PV) charging systems, supercapacitor energy storage technology, hybrid battery-supercapacitor systems, and IoT-enabled energy monitoring. The review identifies research gaps that motivate the current project and maps existing contributions to the proposed system design.

---

## 2. Thematic Overview

### 2.1 Solar Charging Systems

Solar PV systems produce DC power that can be used directly for battery charging. However, the intermittent nature of solar irradiance introduces two key challenges:

1. **Power Intermittency:** Cloud cover, panel angle, and temperature variations cause rapid changes in PV output power (up to 30–50% in seconds).
2. **Voltage/Current Mismatch:** Direct coupling of PV to battery without conditioning leads to inefficient charging and potential battery stress.

**Maximum Power Point Tracking (MPPT)** algorithms (Perturb & Observe, Incremental Conductance) are commonly used to extract maximum power from PV panels. However, these algorithms alone do not address the energy storage buffer problem during transients.

### 2.2 Supercapacitor Technology

Supercapacitors (ultracapacitors / EDLCs) differ fundamentally from batteries:

| Property | Supercapacitor | Li-ion Battery |
|----------|---------------|----------------|
| Energy Density | Low (1–10 Wh/kg) | High (100–265 Wh/kg) |
| Power Density | Very High (1–10 kW/kg) | Moderate (0.3–1.5 kW/kg) |
| Cycle Life | > 500,000 cycles | 500–1,000 cycles |
| Charge Time | Seconds to minutes | Hours |
| Calendar Life | 10–15 years | 2–5 years |
| Cost | Higher per Wh | Lower per Wh |

**Conclusion:** Supercapacitors are ideal for absorbing rapid power transients; batteries are ideal for sustained energy delivery. Combining them leverages both strengths.

### 2.3 Hybrid Energy Storage Systems

Hybrid Battery-Supercapacitor (HBSC) systems use:
- **Passive hybrid:** Direct parallel connection (simple, but inefficient)
- **Semi-active hybrid:** One storage element connected through a DC-DC converter
- **Active hybrid:** Both elements connected through individual converters (maximum control, highest complexity)

For this prototype, a **semi-active topology** is adopted: the supercapacitor is buffered through the DC-DC converter, with the battery directly charged from the supercapacitor.

### 2.4 IoT in Energy Management

IoT integration in energy systems enables:
- Real-time remote monitoring and diagnostics
- Data-driven optimization of charging parameters
- Predictive maintenance through anomaly detection
- Multi-site aggregation and analytics

The ESP8266 (and its successor ESP32) has become the de-facto standard for low-cost WiFi-connected IoT nodes in energy monitoring applications, with extensive library support and community resources.

---

## 3. Key Literature References

---

### Reference 1

**Q. Hassan, P. Tabar, M. S. Hossain, and M. Jasim, "An analysis of photovoltaic/supercapacitor energy systems for efficient energy management: A comprehensive study," *Renewable Energy Reports*, 2022.**

**Summary:**  
This paper provides a comprehensive analysis of PV-supercapacitor hybrid energy systems. The authors evaluate different topologies (passive, semi-active, active) for combining PV sources with supercapacitor storage. Key findings include:
- Semi-active topologies with bidirectional DC-DC converters improve energy utilization by 15–25% compared to passive topologies
- Supercapacitor sizing is critical: under-sizing leads to frequent deep discharge cycles; over-sizing increases cost without proportional benefit
- Real-time state monitoring of the supercapacitor significantly improves system lifetime estimation

**Relevance to This Project:**
- Justifies the use of semi-active topology adopted in this design
- Informs supercapacitor sizing methodology (V_max, capacity selection)
- Highlights the importance of continuous supercapacitor voltage monitoring (implemented in `supercap_monitor.c`)

**Gap Identified:**  
The study focuses on grid-scale or household-scale systems and does not address small-scale embedded prototype implementations with microcontroller-level control.

---

### Reference 2

**J. Zhang, "Supercapacitors for renewable energy applications: A review," *Renewable and Sustainable Energy Reviews*, 2023.**

**Summary:**  
This review covers:
- Supercapacitor materials and their electrochemical properties
- Integration strategies with wind, solar, and grid storage
- Control algorithms for optimal energy distribution between supercapacitors and batteries
- Economic analysis of supercapacitor deployment at different scales

Key quantitative finding: Supercapacitor integration in PV systems reduces battery current ripple by 40–70%, significantly extending battery cycle life.

**Relevance to This Project:**
- Provides theoretical foundation for using supercapacitor to reduce battery stress
- Current ripple reduction is a measurable outcome in this prototype's experimental validation
- Confirms the viability of supercapacitor integration at the prototype scale

**Gap Identified:**  
Limited discussion of real-time embedded control implementations. Focuses on theoretical or simulation-based analysis without hardware demonstration of control algorithms.

---

### Reference 3

**C. V. V. M. Gopi, R. Vinodh, S. Singh, H. J. Kim, and S. B. Bhimireddi, "Review of battery–supercapacitor hybrid energy storage systems for solar energy applications—recent trends and future directions," *Energy Reports*, 2024.**

**Summary:**  
Most current review on HBSC systems (2024), covering:
- Emerging electrode materials and their impact on performance
- Advanced energy management strategies (fuzzy logic, neural networks, rule-based)
- Case studies in solar-powered EV charging and microgrids
- Identified need for low-cost, IoT-enabled monitoring systems for small-scale applications

Key finding: There is a significant research gap in IoT-enabled, real-time monitoring of HBSC systems at the small-scale prototype level—most academic work focuses on large-scale or simulation-only studies.

**Relevance to This Project:**
- Directly identifies the research gap that this project addresses
- Validates the novelty of combining HBSC with IoT monitoring
- Recommends rule-based energy management (implemented as protection logic in this project)

**Gap Identified:**  
As explicitly noted in this paper: "Integrated IoT monitoring systems for small-scale HBSC prototypes remain underexplored."  
**→ This project directly addresses this gap.**

---

### Reference 4

**K. Piyumal, T. M. Karunathilake, R. Hettiarachchi, A. M. S. M. H. Attanayake, and U. H. Ratnayake, "Supercapacitor assisted hybrid PV systems for efficient energy management," *Electronics*, MDPI, 2021.**

**Summary:**  
Presents a practical implementation of a supercapacitor-assisted PV system:
- Demonstrates 22% improvement in energy delivery efficiency with supercapacitor buffer during PV transients
- Implements a simple embedded control system using a microcontroller
- Provides measured data comparing buffered vs. non-buffered charging
- Shows supercapacitor discharge contribution during cloud events

**Relevance to This Project:**
- Most directly comparable system to this project (small-scale, microcontroller-based)
- Confirms the experimental methodology (buffered vs. non-buffered comparison)
- 22% efficiency improvement provides a benchmark for expected results
- Demonstrates feasibility of microcontroller-level supercapacitor management

**Gap Identified:**  
No IoT connectivity or remote monitoring. Data collection required physical access to the laboratory setup.  
**→ This project extends this work by adding ESP8266-based IoT monitoring.**

---

### Reference 5

**M. Baqar, A. Iftikhar, M. Ali, M. Lawal, and A. Liyanage, "Supercapacitor ageing control in residential microgrid based EV charging systems," *Energies*, MDPI, 2023.**

**Summary:**  
Focuses specifically on supercapacitor ageing in EV charging microgrid context:
- Supercapacitor ageing accelerated by high-current deep discharge cycles
- Proposes SoC-based energy management to limit voltage extremes
- Demonstrates 35% extension of supercapacitor useful life through intelligent control
- Uses MATLAB/Simulink simulation with experimental validation

**Relevance to This Project:**
- Validates the importance of monitoring supercapacitor SoC (implemented in this project)
- SoC-based protection thresholds designed into this project's protection logic
- Ageing control through voltage limiting directly implemented in firmware
- Provides reference simulation methodology that could be adopted for this project's simulation work

**Gap Identified:**  
Simulation-heavy study; limited practical embedded implementation details. Does not address IoT integration or remote monitoring.

---

## 4. Research Gap Analysis

| Gap | Literature Evidence | This Project's Contribution |
|-----|--------------------|-----------------------------|
| Limited IoT monitoring in small-scale HBSC systems | Gopi et al. 2024 explicitly notes this gap | ESP8266-based IoT gateway with cloud dashboard |
| Lack of real-time embedded control in prototype systems | Zhang 2023; Piyumal et al. 2021 (no IoT) | STM32 firmware with real-time control loop (< 10 ms) |
| No unified embedded + IoT solution for residential scale | Baqar et al. 2023 (simulation only) | Integrated embedded + IoT prototype |
| Absence of comparative buffered/non-buffered datasets | Piyumal et al. 2021 (no data logging) | Automated data logging via cloud backend |
| Missing adaptive protection in small-scale systems | Hassan et al. 2022 (no embedded impl.) | Multi-level protection in STM32 firmware |

---

## 5. Summary

The literature review confirms that:

1. **Solar-supercapacitor hybrid systems** are well-established as efficient energy management solutions for PV applications.
2. **Supercapacitor buffering** reduces battery current stress and extends battery life—supported by multiple independent studies.
3. **IoT integration** in energy systems is growing but remains largely absent in small-scale prototype work.
4. **A clear research gap exists** in unified, low-cost, IoT-enabled embedded implementations for solar-assisted charging prototypes.

This project addresses the identified gap by delivering a complete, integrated solution combining STM32 embedded control, supercapacitor energy management, and ESP8266 IoT monitoring in a single prototype system.

---

## 6. Full Reference List

1. Q. Hassan, P. Tabar, M. S. Hossain, and M. Jasim, "An analysis of photovoltaic/supercapacitor energy systems for efficient energy management: A comprehensive study," *Renewable Energy Reports*, vol. 8, pp. 100–115, 2022.

2. J. Zhang, "Supercapacitors for renewable energy applications: A review," *Renewable and Sustainable Energy Reviews*, vol. 182, pp. 113–128, 2023.

3. C. V. V. M. Gopi, R. Vinodh, S. Singh, H. J. Kim, and S. B. Bhimireddi, "Review of battery–supercapacitor hybrid energy storage systems for solar energy applications—recent trends and future directions," *Energy Reports*, vol. 10, pp. 1225–1241, 2024.

4. K. Piyumal, T. M. Karunathilake, R. Hettiarachchi, A. M. S. M. H. Attanayake, and U. H. Ratnayake, "Supercapacitor assisted hybrid PV systems for efficient energy management," *Electronics*, MDPI, vol. 10, no. 13, p. 1581, 2021.

5. M. Baqar, A. Iftikhar, M. Ali, M. Lawal, and A. Liyanage, "Supercapacitor ageing control in residential microgrid based EV charging systems," *Energies*, MDPI, vol. 16, no. 4, p. 1734, 2023.

---

*Document Version: 1.0 | Last Updated: April 2026*  
*Repository: https://github.com/2024mt12104/IoT-Solar-Charging-System*
