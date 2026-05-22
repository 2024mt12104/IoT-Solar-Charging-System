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
 * @file    config.h
 * @brief   WiFi credentials and cloud API configuration for the ESP8266
 *
 * IMPORTANT – Do NOT commit real credentials to version control.
 * Copy this file to config_local.h (which is .gitignored) and fill in
 * your actual values, then include config_local.h in iot_gateway.ino.
 */

#ifndef CONFIG_H
#define CONFIG_H

/* -------------------------------------------------------------------------
 * WiFi credentials
 * ---------------------------------------------------------------------- */
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"

/* -------------------------------------------------------------------------
 * Cloud API endpoints
 * Set CLOUD_USE_HTTPS to 1 when TLS is available (requires BearSSL client).
 * ---------------------------------------------------------------------- */
#define CLOUD_USE_HTTPS   0

#define CLOUD_HOST        "your-backend-host.example.com"
#define CLOUD_PORT        5000          /* Flask default; change for production */
#define CLOUD_API_PATH    "/api/data"   /* POST endpoint */

/* -------------------------------------------------------------------------
 * Device identity
 * ---------------------------------------------------------------------- */
#define DEVICE_ID         "solar-charger-001"
#define DEVICE_LOCATION   "Lab-A"

/* -------------------------------------------------------------------------
 * Timing
 * ---------------------------------------------------------------------- */
/** How often to attempt a cloud upload [ms] */
#define UPLOAD_INTERVAL_MS  5000U

/** UART baud rate matching STM32 USART2 configuration */
#define STM32_UART_BAUD     115200U

/** WiFi reconnect retry interval [ms] */
#define WIFI_RETRY_INTERVAL_MS  10000U

/* -------------------------------------------------------------------------
 * Buffer sizes
 * ---------------------------------------------------------------------- */
#define UART_RX_BUF_LEN    256U
#define HTTP_TX_BUF_LEN    512U

#endif /* CONFIG_H */
