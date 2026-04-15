# Presentation Guide

**Project:** IoT-Enabled Solar-Assisted High-Efficiency Battery Charging Using Supercapacitor Energy Buffering  
**Student:** Ajeesh Kumar R (2024MT12104)  
**Viva Date:** April 17, 2026  

---

## Presentation File

| File | Description |
|------|-------------|
| `Mid_Sem_Viva_Presentation.pptx` | Complete 12-slide PowerPoint for mid-semester viva |

---

## How to Present

### Before the Viva

1. **Practice timing:** Each slide should take approximately 1.5–2.5 minutes. Total presentation: ~20 minutes.
2. **Know your references:** Be able to explain any of the 5 cited papers in detail.
3. **Understand the computations:** Review `documentation/COMPUTATION_METHODOLOGY.md` and `documentation/Computations_Complete_Guide.md`. Slides 9–10 focus on computational metrics per faculty feedback.
4. **Prepare for questions on:** System architecture, design choices, computational metrics, progress status.

### Slide-by-Slide Guide

| Slide | Title | Key Points to Emphasize | Time |
|-------|-------|------------------------|------|
| 1 | Title Slide | Introduce yourself, guide, institution, viva date | 1 min |
| 2 | Introduction – Background & Problem | EV growth, solar intermittency, 5 key problems, innovation statement | 2 min |
| 3 | Introduction – Objectives & Scope | 5 objectives, in-scope vs out-of-scope items | 2 min |
| 4 | Literature Review – Thematic Overview | 4 themes: Solar, Supercap, HBSC, IoT with key findings | 2 min |
| 5 | Literature Review – Key Papers & Gaps | 5 references table, gaps identified, project contribution | 2 min |
| 6 | Research Methodology – Approach & Phases | Methodology, 3-part validation, 6-phase timeline with status | 2 min |
| 7 | Research Methodology – System Architecture | Block diagram, STM32 control, ESP8266 IoT layer, components | 2.5 min |
| 8 | Data Collection – Plan & Progress | Sampling strategy, JSON packet, progress checklist | 2 min |
| 9 | Computational Metrics – What & Why (**Faculty Focus**) | Definition, importance, 6 core metrics overview | 2.5 min |
| 10 | Computational Metrics – Formulas & Application (**Faculty Focus**) | 4 key formulas with examples, real-world scenario flow | 2.5 min |
| 11 | Expected Outcomes & Challenges | 7 deliverables, 5 challenges with mitigations, simulation approach | 2 min |
| 12 | References & Thank You | 5 references, future scope, contact info | 1 min |

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

### For Slide 5 (Research Gaps)
This is a **critical slide** — examiners want to see that you understand where your work fits in the academic landscape.
- Point to specific references for each gap
- Be clear about YOUR contribution vs. existing work
- Use phrases like "This project directly addresses..."

### For Slides 9 & 10 (Computational Metrics — Faculty Feedback Focus)
Faculty specifically requested computation content. These two slides address that feedback directly. **Highlight:**
- What computational metrics are and why they are needed
- P = V × I (instantaneous power)
- E = ΣP×Δt (energy integration)
- E = ½CV² (supercapacitor energy)
- η = P_out/P_in × 100% (efficiency)
- SoC formula for supercapacitor (energy-proportional, V²-based)
- PWM duty cycle control
- Error analysis (±2.06% total)
- The 6-step real-world scenario flow (Sense → Compute → Integrate → Control → Protect → Transmit)
- Timing: <10ms cycle, 1kHz sampling, 1Hz cloud update

### For Slide 8 (Progress)
Be **honest and specific**. Examiners appreciate accurate status updates over overstatements. Clearly distinguish completed vs. in-progress vs. upcoming items.

---

## Backup Questions to Ask the Panel

To demonstrate engagement and forward thinking:
1. "Would you recommend any additional metrics for the experimental comparison?"
2. "Are there specific test scenarios you'd like to see in the demo phase?"
3. "Should I consider any additional protection mechanisms for the final prototype?"

---

*Last Updated: April 2026*
