# Computations Complete Guide

## Table of Contents
1. [All 10 Computations](#all-10-computations)
2. [Power Calculation (P=V×I)](#power-calculation-pv)i
3. [Energy Calculation (E=P×t)](#energy-calculation-ept)
4. [Supercapacitor Energy (E=0.5×C×V²)](#supercapacitor-energy-e05cv2)
5. [PWM Control](#pwm-control)
6. [Charging Efficiency](#charging-efficiency)
7. [Charging Time Calculation](#charging-time-calculation)
8. [Voltage Drop in Wiring (V_drop = I×R)](#voltage-drop-in-wiring-v_drop-i-r)
9. [Power Dissipation as Heat (P_heat = I²×R)](#power-dissipation-as-heat-p_heat-i2r)
10. [Supercapacitor Charge/Discharge Time](#supercapacitor-chargedischarge-time)
11. [System SOC (State of Charge)](#system-soc-state-of-charge)
12. [Quick Reference Table](#quick-reference-table)
13. [Real-World Complete Charging Scenario](#real-world-complete-charging-scenario)
14. [Summary Parameter Table](#summary-parameter-table)
15. [Key Takeaways for Viva Preparation](#key-takeaways-for-viva-preparation)

---

## All 10 Computations
1. Computation 1: [Description & Formula]
   - Formula: `Formula`
   - Explanation: `Simple explanation in layman's terms.`
   - Example: `Practical example using numbers.`

2. Computation 2: [Description & Formula]
   - Formula: `Formula`
   - Explanation: `Simple explanation in layman's terms.`
   - Example: `Practical example using numbers.`

...

## Power Calculation (P=V×I)
- **Formula**: P = V × I
- **Layman's Explanation**: Power is the rate at which energy is used or produced.
- **Example**: If a device operates at 5 V and draws 2 A, then:
  - P = 5 V × 2 A = 10 W

### Phone Example
- Suppose a smartphone uses 5 V and draws 1 A:
  - Power = 5 V × 1 A = 5 W

### System Example
- Consider a solar charging system using 12 V and 3 A:
  - Power = 12 V × 3 A = 36 W

## Energy Calculation (E=P×t)
- **Formula**: E = P × t
- **Conversion to Joules**: 1 Wh = 3600 J
- **Example**: If a device uses 10 W for 2 hours:
  - E = 10 W × 2 h = 20 Wh = 20 × 3600 J = 72000 J

## Supercapacitor Energy (E=0.5×C×V²)
- **Formula**: E = 0.5 × C × V²
- **Storage Capacity Example**: For a supercapacitor of 100 F charged to 5 V:
  - E = 0.5 × 100 F × (5 V)² = 1250 J

## PWM Control
- **Duty Cycle Calculation**: Duty Cycle = (Ton / (Ton + Toff)) × 100%
- **Example**: If Ton = 2 ms and Toff = 3 ms:
  - Duty Cycle = (2 / (2 + 3)) × 100 = 40%
- **DC-DC Converter Example**: Control voltage from 12 V to 5 V using PWM.

## Charging Efficiency Formula
- **Formula**: Charging Efficiency = (Output Energy / Input Energy) × 100%
- **Buffered vs Non-buffered**: Buffered helps achieve higher efficiencies in variable loads.

## Charging Time Calculation
- **Example**: If charging at 1 A, with a battery capacity of 2000 mAh:
  - Time = Capacity / Rate = 2000 mAh / 1000 mA = 2 hours

## Voltage Drop in Wiring (V_drop = I×R)
- **Formula**: V_drop = I × R
- **Example**: If current is 5 A and resistance of wire is 2 ohms:
  - V_drop = 5 A × 2 Ω = 10 V

## Power Dissipation as Heat (P_heat = I²×R)
- **Formula**: P_heat = I² × R
- **Example**: For a current of 3 A:
  - P_heat = 3² × 2 Ω = 18 W

## Supercapacitor Charge/Discharge Time
- **Time Calculation**: Depends on series resistance and load conditions.

## System SOC (State of Charge)
- **Definition**: SOC measures the current charge level of a battery relative to its capacity.
- **Example**: A battery with 4000 mAh capacity that has 2000 mAh remaining = 50% SOC.

## Quick Reference Table
| Computation | Formula           |
|-------------|-------------------|
| Power       | P = V × I         |
| Energy      | E = P × t         |
| Supercap E  | E = 0.5 × C × V²  |
| ...         | ...               |

## Real-World Complete Charging Scenario 
1. **Scenario**: Charging from 0 to 1440 seconds.
   - Define charging rate, calculations performed every 60 seconds.
   - Show voltage, current, energy through the period.
   - Demonstrate calculations for each interval.

## Summary Parameter Table
| Parameter      | Value           |
|----------------|-----------------|
| Total Energy   | ...             |
| Total Time     | ...             |
| Efficiency      | ...             |

## Key Takeaways for Viva Preparation
- Understand formulas and practical applications.
- Be prepared to calculate real-world scenarios based on provided data.
- Familiarize with energy conservation principles.

---

This comprehensive guide is designed for academic presentation, covering essential calculations for the IoT Solar Charging System.