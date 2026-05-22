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
 * @file    wifi_manager.h
 * @brief   WiFi connection management for ESP8266 (Arduino framework)
 *
 * Handles initial connection, auto-reconnect, and exposes a simple
 * status API used by the main sketch.
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "config.h"

/* -------------------------------------------------------------------------
 * WiFi Manager class
 * ---------------------------------------------------------------------- */

class WiFiManager {
public:
    /** Initialise and attempt first connection.  Blocks until connected
     *  or timeout expires (WIFI_RETRY_INTERVAL_MS × 3). */
    static void begin() {
        Serial.print("[WiFi] Connecting to ");
        Serial.println(WIFI_SSID);

        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED &&
               (millis() - start) < (WIFI_RETRY_INTERVAL_MS * 3UL)) {
            delay(500);
            Serial.print('.');
        }

        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("\n[WiFi] Connected. IP: " + WiFi.localIP().toString());
        } else {
            Serial.println("\n[WiFi] Initial connection failed – will retry.");
        }
    }

    /** Call from main loop to maintain the connection. */
    static void maintain() {
        static uint32_t lastRetry = 0U;
        if (WiFi.status() != WL_CONNECTED) {
            uint32_t now = millis();
            if ((now - lastRetry) >= WIFI_RETRY_INTERVAL_MS) {
                lastRetry = now;
                Serial.println("[WiFi] Disconnected – retrying...");
                WiFi.disconnect();
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            }
        }
    }

    /** Returns true if WiFi is currently connected. */
    static bool isConnected() {
        return WiFi.status() == WL_CONNECTED;
    }

    /** Returns the assigned IP address as a string. */
    static String ipAddress() {
        return WiFi.localIP().toString();
    }

    /** Returns RSSI of the current connection [dBm]. */
    static int32_t rssi() {
        return WiFi.RSSI();
    }
};

#endif /* WIFI_MANAGER_H */
