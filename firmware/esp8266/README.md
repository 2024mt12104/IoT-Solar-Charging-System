<!---
Project Title: IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING SUPERCAPACITOR ENERGY BUFFERING
Author: Ajeesh Kumar R | BITS ID: 2024MT12104
Programme: M.Tech in Software Systems - Specialization in IoT
Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division

Academic Purpose Notice:
  This document is developed solely for academic learning, research, experimentation,
  and project evaluation purposes under the M.Tech in Software Systems - Specialization
  in IoT programme at BITS Pilani WILP.

Ownership: All project work is the intellectual work of the above-mentioned author unless otherwise referenced.
Usage Restriction: Unauthorized copying, redistribution, or commercial usage without prior permission is discouraged.
--->

# ESP8266 IoT Gateway – Setup & Configuration Guide

## Overview

The ESP8266 NodeMCU acts as the **WiFi gateway** between the STM32 microcontroller and the cloud backend.

```
STM32 USART2 → SoftwareSerial → ESP8266 → WiFi → Cloud REST API
```

---

## Hardware Connections

| ESP8266 Pin | STM32 Pin | Signal |
|-------------|-----------|--------|
| D5 (GPIO14) | PA2 (USART2 TX) | RX from STM32 |
| D6 (GPIO12) | PA3 (USART2 RX) | TX to STM32 |
| GND | GND | Common ground |
| 3.3V | 3.3V | (optional shared power) |

### Communication Architecture

![Communication Architecture](../../documentation/images/Communication_Architecture_Diagram.png)

> **Voltage levels**: Both STM32 and ESP8266 operate at 3.3V logic – direct connection is safe.

---

## Arduino IDE Setup

### 1. Install ESP8266 Board Support

1. Open Arduino IDE → `File → Preferences`
2. Add to **Additional Boards Manager URLs**:
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. `Tools → Board → Boards Manager` → install **esp8266 by ESP8266 Community** ≥ 3.1.2

### 2. Required Libraries

Install via `Sketch → Include Library → Manage Libraries`:

| Library | Version | Purpose |
|---------|---------|---------|
| ESP8266WiFi | (built-in) | WiFi management |
| ESP8266HTTPClient | (built-in) | HTTP POST |

### 3. Board Settings

| Setting | Value |
|---------|-------|
| Board | NodeMCU 1.0 (ESP-12E Module) |
| Upload Speed | 115200 |
| CPU Frequency | 80 MHz |
| Flash Size | 4MB (FS: 2MB, OTA: ~1019KB) |
| Port | COMx / /dev/ttyUSB0 |

---

## Configuration

Edit `config.h` (or create `config_local.h`) with your settings:

```c
#define WIFI_SSID       "MyHomeNetwork"
#define WIFI_PASSWORD   "MySecret123"
#define CLOUD_HOST      "192.168.1.100"   // or your server hostname
#define CLOUD_PORT      5000
#define DEVICE_ID       "solar-charger-001"
```

> **Security note**: Never commit real credentials. Add `config_local.h` to `.gitignore`.

---

## Flashing

1. Connect NodeMCU via USB
2. Open `iot_gateway.ino` in Arduino IDE
3. Select the correct COM port
4. Click **Upload** (Ctrl+U)

---

## Serial Monitor (debugging)

Open `Tools → Serial Monitor` at **115200 baud** to see:

```
[IoT Gateway] Booting...
[WiFi] Connecting to MyHomeNetwork........
[WiFi] Connected. IP: 192.168.1.105
[IoT Gateway] STM32 UART ready at 115200 baud
[IoT Gateway] Ready.
[IoT Gateway] RX: {"vs":12450,"vb":13200,...}
[IoT Gateway] Parsed OK | Vsolar=12450 mV | Vbat=13200 mV | η=83%
[Cloud] POST 200 → http://192.168.1.100:5000/api/data
```

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| No RX data | UART wiring or baud mismatch | Check D5 connection; verify STM32 baud = 115200 |
| WiFi keeps reconnecting | Wrong SSID/password | Double-check config.h |
| HTTP POST fails (–1) | Backend unreachable | Confirm server IP and port |
| Buffer overflow messages | STM32 sending too fast | Increase `UART_RX_BUF_LEN` or add flow control |

---

*Last updated: May 2026*
