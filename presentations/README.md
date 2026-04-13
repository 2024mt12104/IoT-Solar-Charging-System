# Presentation Guide

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Viva Date:** April 17, 2026  

---

## Presentation File

| File | Description |
|------|-------------|
| `Mid_Sem_Viva_Presentation.pptx` | Complete 21-slide PowerPoint for mid-semester viva |

---

## How to Present

### Before the Viva

1. **Practice timing:** Each slide should take approximately 1–2 minutes. Total presentation: ~20–25 minutes.
2. **Know your references:** Be able to explain any of the 5 cited papers in detail.
3. **Understand the computations:** Review `documentation/COMPUTATION_METHODOLOGY.md`.
4. **Prepare for questions on:** System architecture, design choices, progress status.

### Slide-by-Slide Guide

| Slide | Title | Key Points to Emphasize | Time |
|-------|-------|------------------------|------|
| 1 | Title | Introduce yourself and your guide | 1 min |
| 2 | Introduction – Background | EV growth, solar energy trends | 2 min |
| 3 | Introduction – Problem & Novelty | Intermittency, no IoT in small-scale | 2 min |
| 4 | Objectives | 5 clear objectives, all measurable | 1.5 min |
| 5 | Scope of Work | What is and isn't in scope | 1.5 min |
| 6 | Literature – Overview | State of art in 4 categories | 2 min |
| 7 | Literature – Key Papers | Briefly summarize each paper | 2 min |
| 8 | Literature – Gaps & Contribution | Emphasize YOUR contribution | 2 min |
| 9 | Architecture – Block Diagram | Walk through signal/data flow | 2 min |
| 10 | Architecture – Component Details | Explain each component's role | 1.5 min |
| 11 | Design – Safety & Architecture | Modular design benefits | 1.5 min |
| 12 | Design – Computation & Control | P=V×I, PWM control, loop time | 2 min |
| 13 | Methodology – Development Phases | 5 phases, what's done | 1.5 min |
| 14 | Methodology – Modular Implementation | Firmware modules | 1.5 min |
| 15 | Data Collection Plan | Sampling rates, logging strategy | 1.5 min |
| 16 | Current Progress | Be honest about what's done/pending | 2 min |
| 17 | Simulation Approach | How you'll demo without hardware | 1.5 min |
| 18 | Expected Outcomes | All 7 deliverables | 1 min |
| 19 | Timeline | Phases and current status | 1 min |
| 20 | Challenges & Solutions | 5 challenges, 5 solutions | 1 min |
| 21 | References & Conclusion | Future scope, GitHub link | 1 min |

---

## Anticipated Viva Questions and Model Answers

### Technical Questions

**Q1: Why use a supercapacitor instead of a larger battery?**  
A: Supercapacitors have extremely high power density and can absorb rapid transients in milliseconds, whereas batteries respond in seconds. For the buffering application (handling intermittent solar input fluctuations), the supercapacitor's fast response and long cycle life (>500,000 cycles vs. ~1,000 for batteries) make it the right choice. The battery handles long-term energy storage; the supercapacitor handles short-term power fluctuations.

**Q2: How does the PWM control the DC-DC converter?**  
A: The STM32 generates a PWM signal on TIM1. The DC-DC converter's switching transistor is controlled by this signal. A higher duty cycle (longer ON time) transfers more energy per cycle, increasing the output voltage. By measuring the actual output voltage with ADC and comparing it to the target, the firmware adjusts the duty cycle in a closed-loop manner using a PI controller.

**Q3: What formula do you use for energy calculation?**  
A: E[n] = E[n-1] + P[n] × Δt, where P = V × I and Δt is the sampling interval (1 ms for 1 kHz sampling). Energy is accumulated in Joules and converted to Wh by dividing by 3600.

**Q4: How do you calculate supercapacitor State of Charge?**  
A: SoC = ((V² - V_min²) / (V_max² - V_min²)) × 100%, because energy stored in a capacitor is proportional to V², not V linearly. This gives accurate SoC in energy terms.

**Q5: What are the accuracy limitations of your ADC measurements?**  
A: The 12-bit ADC has a quantization error of ±0.4 mV at the pin. After voltage divider scaling (×4 for a 12V system), this becomes ±1.6 mV. The current sensor (ACS712) adds ~2% error. Total power measurement error is approximately 2.06% of reading. These are acceptable for prototype-level validation.

**Q6: Why UART between STM32 and ESP8266 instead of SPI?**  
A: UART is simpler (only 2 wires: TX, RX), more tolerant of timing variations, and has excellent library support on both platforms. For our data rate (1 Hz, ~200 bytes/packet), 115200 baud UART provides more than adequate bandwidth. SPI would add complexity without benefit for this use case.

### Project Management Questions

**Q7: What is your current status?**  
A: Phase 1 (Design & Planning) is complete. We have finalized the system architecture, selected all components, defined the computation methodology, and completed the literature review. STM32 firmware development is in progress (ADC and PWM modules). Hardware procurement is underway.

**Q8: What is the main challenge you foresee?**  
A: The main technical challenge is achieving a stable closed-loop PWM control loop within the 10 ms timing budget while simultaneously handling sensor acquisition, protection logic, and UART transmission. The mitigation strategy is to use DMA for ADC (background operation) and UART (non-blocking), so the control algorithm runs uninterrupted.

**Q9: How will you validate your results?**  
A: Three-part validation: (1) Software simulation with realistic charging models, (2) Hardware testing with a regulated DC supply as solar simulator, (3) Comparison of charging efficiency, voltage ripple, and supercapacitor contribution between buffered and non-buffered configurations.

---

## Presentation Tips

### For Slide 8 (Research Gaps)
This is a **critical slide** — examiners want to see that you understand where your work fits in the academic landscape.
- Point to specific references for each gap
- Be clear about YOUR contribution vs. existing work
- Use phrases like "This project directly addresses..."

### For Slide 12 (Computation)
Faculty specifically requested computation content. **Highlight:**
- P = V × I (instantaneous power)
- E = ∫P dt (energy integration)
- SoC formula for supercapacitor
- PI controller for PWM regulation
- Error analysis figures

### For Slide 16 (Progress)
Be **honest and specific**. Examiners appreciate accurate status updates over overstatements. Clearly distinguish completed vs. in-progress vs. upcoming items.

---

## Backup Questions to Ask the Panel

To demonstrate engagement and forward thinking:
1. "Would you recommend any additional metrics for the experimental comparison?"
2. "Are there specific test scenarios you'd like to see in the demo phase?"
3. "Should I consider any additional protection mechanisms for the final prototype?"

---

*Last Updated: April 2026*
