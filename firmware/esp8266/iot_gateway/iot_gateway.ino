/*
 * =============================================================================
 * Project Title:
 *   IOT-ENABLED SOLAR-ASSISTED HIGH-EFFICIENCY BATTERY CHARGING USING
 *   SUPERCAPACITOR ENERGY BUFFERING
 *
 * Author: Ajeesh Kumar R | BITS ID: 2024MT12104
 * Programme: M.Tech in Software Systems - Specialization in IoT
 * Institution: BITS Pilani - Work Integrated Learning Program (WILP) Division
 *
 * Academic Purpose Notice:
 *   This source code, dataset, documentation, and associated project files are
 *   developed solely for academic learning, research, experimentation, and project
 *   evaluation purposes under the M.Tech in Software Systems - Specialization in
 *   IoT programme at BITS Pilani WILP.
 *
 * Ownership Declaration:
 *   The complete project work, including source code, datasets, documentation,
 *   design files, reports, diagrams, and related resources, are the intellectual
 *   work of the above-mentioned author unless otherwise referenced.
 *
 * Usage Restriction:
 *   Unauthorized copying, redistribution, commercial usage, or modification of
 *   this project material without prior permission from the author is discouraged.
 *   This project is intended strictly for educational and research-oriented use.
 * =============================================================================
 */

/**
 * @file    iot_gateway.ino
 * @brief   ESP8266 IoT Gateway – Main Arduino sketch
 *
 * Responsibilities:
 *   1. Read JSON data frames from STM32 over Software Serial (or HW UART)
 *   2. Maintain WiFi connection
 *   3. Upload parsed sensor data to the cloud backend every UPLOAD_INTERVAL_MS
 *   4. Echo status to USB Serial for debugging
 *
 * Wiring:
 *   ESP8266 NodeMCU D5 (GPIO14) ← STM32 USART2 TX (PA2)
 *   ESP8266 NodeMCU D6 (GPIO12) → STM32 USART2 RX (PA3)
 *   Common GND
 *
 * Arduino IDE Board Settings:
 *   Board: NodeMCU 1.0 (ESP-12E Module)
 *   Upload speed: 115200
 *   CPU Frequency: 80 MHz
 *   Flash size: 4MB (FS: 2MB)
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "wifi_manager.h"
#include "cloud_uploader.h"

/* -------------------------------------------------------------------------
 * Software Serial for STM32 ↔ ESP8266 UART bridge
 * D5 = GPIO14 = RX from STM32, D6 = GPIO12 = TX to STM32
 * ---------------------------------------------------------------------- */
SoftwareSerial stm32Serial(14, 12);  /* RX, TX */

/* -------------------------------------------------------------------------
 * State
 * ---------------------------------------------------------------------- */
static char     rxBuf[UART_RX_BUF_LEN];
static uint16_t rxIdx       = 0U;
static uint32_t lastUpload  = 0U;
static SensorFrame lastFrame;

/* -------------------------------------------------------------------------
 * setup()
 * ---------------------------------------------------------------------- */
void setup() {
    /* USB debug serial */
    Serial.begin(115200);
    Serial.println("\n[IoT Gateway] Booting...");

    /* UART to STM32 */
    stm32Serial.begin(STM32_UART_BAUD);
    Serial.printf("[IoT Gateway] STM32 UART ready at %u baud\n", STM32_UART_BAUD);

    /* Connect to WiFi */
    WiFiManager::begin();

    lastFrame.valid = false;
    Serial.println("[IoT Gateway] Ready.");
}

/* -------------------------------------------------------------------------
 * loop()
 * ---------------------------------------------------------------------- */
void loop() {
    /* --- 1. Maintain WiFi connection ---------------------------------- */
    WiFiManager::maintain();

    /* --- 2. Read characters from STM32 UART -------------------------- */
    while (stm32Serial.available() > 0) {
        char c = (char)stm32Serial.read();

        if (c == '\n') {
            /* End of frame – null-terminate and process */
            rxBuf[rxIdx] = '\0';
            processFrame(String(rxBuf));
            rxIdx = 0U;
        } else {
            if (rxIdx < UART_RX_BUF_LEN - 1U) {
                rxBuf[rxIdx++] = c;
            } else {
                /* Buffer overflow – discard and reset */
                Serial.println("[IoT Gateway] RX buffer overflow – discarding frame");
                rxIdx = 0U;
            }
        }
    }

    /* --- 3. Upload to cloud at UPLOAD_INTERVAL_MS -------------------- */
    uint32_t now = millis();
    if ((now - lastUpload) >= UPLOAD_INTERVAL_MS) {
        lastUpload = now;

        if (!WiFiManager::isConnected()) {
            Serial.println("[IoT Gateway] WiFi not ready – skipping upload");
        } else if (!lastFrame.valid) {
            Serial.println("[IoT Gateway] No valid frame yet – skipping upload");
        } else {
            int code = CloudUploader::upload(lastFrame);
            if (code == 200 || code == 201) {
                Serial.println("[IoT Gateway] Upload OK");
            } else {
                Serial.printf("[IoT Gateway] Upload failed (code %d)\n", code);
            }
        }
    }
}

/* -------------------------------------------------------------------------
 * processFrame() – parse and cache the latest STM32 data frame
 * ---------------------------------------------------------------------- */
void processFrame(const String &raw) {
    if (raw.length() < 10) {
        return;  /* Too short to be a valid frame */
    }

    Serial.print("[IoT Gateway] RX: ");
    Serial.println(raw);

    SensorFrame frame = CloudUploader::parseFrame(raw);
    if (frame.valid) {
        lastFrame = frame;
        Serial.printf("[IoT Gateway] Parsed OK | Vsolar=%d mV | Vbat=%d mV | η=%d%%\n",
                      frame.v_solar, frame.v_battery, frame.efficiency);
    } else {
        Serial.println("[IoT Gateway] Frame parse failed");
    }
}
