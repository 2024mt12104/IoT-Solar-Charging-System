# STM32 Firmware – Build & Flash Guide

## Overview

The STM32 firmware runs on the **STM32F446RE Nucleo** board and performs:

| Module | File | Role |
|--------|------|------|
| Sensor Driver | `sensor_driver.c/.h` | ADC acquisition, filtering, calibration |
| PWM Control | `pwm_control.c/.h` | DC-DC converter regulation + MPPT |
| Supercap Monitor | `supercap_monitor.c/.h` | Supercapacitor SoC & energy tracking |
| Power Calculator | `power_calc.c/.h` | P = V×I, E = ∫P dt, efficiency |
| Communication | `communication.c/.h` | JSON frames over USART2 to ESP8266 |
| Main | `main.c` | 10 ms control loop integrating all modules |

---

## Prerequisites

| Tool | Version | Download |
|------|---------|----------|
| STM32CubeIDE | ≥ 1.13 | https://www.st.com/en/development-tools/stm32cubeide.html |
| STM32CubeMX | ≥ 6.9 | Bundled with CubeIDE |
| ST-Link V2 driver | Latest | https://www.st.com/en/development-tools/stsw-link009.html |
| GNU Arm Embedded Toolchain | ≥ 12.2 | Bundled with CubeIDE |

---

## Project Setup in STM32CubeIDE

1. **Create new project**  
   `File → New → STM32 Project` → select **NUCLEO-F446RE**.

2. **Configure peripherals in CubeMX** (`.ioc` file):

   | Peripheral | Configuration |
   |------------|---------------|
   | ADC1 | Scan mode, 5 channels (IN0–IN1, IN4–IN5, IN8), 12-bit, software trigger, 480-cycle sample |
   | TIM1 CH1 (PA8) | PWM mode 1, PSC=0, ARR=1679 → 100 kHz |
   | USART2 (PA2/PA3) | 115 200 baud, 8N1, asynchronous |
   | SYS | SysTick as timebase |

3. **Copy source files** into `Core/Src/` and `Core/Inc/`.

4. **Generate code** – CubeMX produces the `MX_xxx_Init()` functions in `main.c`.  
   Replace the placeholder `main.c` with the project-supplied version, keeping
   the `/* USER CODE BEGIN/END */` sections.

---

## Pin Mapping

| Signal | STM32 Pin | Function |
|--------|-----------|----------|
| Solar voltage sense | PA0 (ADC1 IN0) | Voltage divider output |
| Battery voltage sense | PA1 (ADC1 IN1) | Voltage divider output |
| Input current sense | PA4 (ADC1 IN4) | ACS712 output |
| Output current sense | PA5 (ADC1 IN5) | ACS712 output |
| Supercap voltage sense | PB0 (ADC1 IN8) | Voltage divider output |
| PWM gate drive | PA8 (TIM1 CH1) | To DC-DC gate driver |
| UART TX → ESP8266 RX | PA2 (USART2 TX) | Serial data |
| UART RX ← ESP8266 TX | PA3 (USART2 RX) | ACK (optional) |

---

## Build & Flash

```bash
# Inside STM32CubeIDE
# 1. Build
Project → Build Project    (Ctrl+B)

# 2. Flash & Debug
Run → Debug As → STM32 Cortex-M C/C++ Application
```

Or using OpenOCD from the command line:

```bash
openocd -f interface/stlink.cfg \
        -f target/stm32f4x.cfg \
        -c "program build/IoT-Solar-Charging.elf verify reset exit"
```

---

## Serial Monitor (verify operation)

```bash
# Linux / macOS
screen /dev/ttyACM0 115200

# Windows: use PuTTY or TeraTerm at COM port, 115200 baud
```

Expected output (1 Hz):

```json
{"vs":12450,"vb":13200,"vc":8500,"ii":1200,"io":950,"pi":14940,"po":12540,"ei":45200,"eo":37900,"ef":83,"sc_soc":72,"sc_e":18170,"ts":38}
```

---

## Calibration

Edit constants in the header files to match your actual components:

| Constant | File | Adjust for |
|----------|------|-----------|
| `SENSOR_VDIV_RATIO_X100` | `sensor_driver.h` | Actual R1, R2 values |
| `SENSOR_ISENSE_SENSITIVITY_MV_PER_A` | `sensor_driver.h` | ACS712 variant (5A/20A/30A) |
| `SUPERCAP_CAPACITANCE_MF` | `supercap_monitor.h` | Actual capacitance in mF |
| `PWM_TARGET_VOUT_MV` | `pwm_control.h` | Target battery float voltage |
| `PWM_SWITCHING_FREQ_HZ` | `pwm_control.h` | DC-DC converter requirement |

---

*Last updated: May 2026*
