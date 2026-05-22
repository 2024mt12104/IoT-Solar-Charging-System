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
 * @file    communication.h
 * @brief   UART framing and transmission from STM32 to ESP8266
 *
 * Serialises sensor data into a compact JSON-like ASCII frame and
 * transmits over USART2 at 115 200 baud.
 *
 * Frame format (newline-terminated ASCII):
 *   {"vs":<mV>,"vb":<mV>,"vc":<mV>,"ii":<mA>,"io":<mA>,
 *    "pi":<mW>,"po":<mW>,"ei":<mJ>,"eo":<mJ>,"ef":<pct>,
 *    "sc_soc":<pct>,"sc_e":<mJ>,"ts":<s>}\n
 *
 * Hardware:
 *   - USART2 TX (PA2) → ESP8266 RX
 *   - USART2 RX (PA3) ← ESP8266 TX  (for ACK, optional)
 *   - Baud rate: 115 200 N 8 1
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "sensor_driver.h"
#include "power_calc.h"
#include "supercap_monitor.h"

/* -------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------- */

/** UART baud rate */
#define COMM_BAUDRATE           115200U

/** Transmit interval [ms] – how often to send a data frame */
#define COMM_TX_INTERVAL_MS     1000U

/** Maximum frame length in bytes (including null terminator) */
#define COMM_FRAME_MAX_LEN      256U

/* -------------------------------------------------------------------------
 * Data types
 * ---------------------------------------------------------------------- */

typedef enum {
    COMM_OK = 0,
    COMM_ERR_BUSY,
    COMM_ERR_TIMEOUT
} Comm_Status_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/**
 * @brief  Initialise USART2 peripheral at COMM_BAUDRATE.
 */
void Comm_Init(void);

/**
 * @brief  Build and transmit a JSON data frame.
 *         Non-blocking: uses DMA/interrupt if available, else polled.
 * @param  sensors   Pointer to latest sensor reading.
 * @param  power     Pointer to latest power metrics.
 * @param  supercap  Pointer to latest supercap status.
 * @return COMM_OK on success, COMM_ERR_BUSY if previous TX in progress.
 */
Comm_Status_t Comm_SendDataFrame(const SensorReading_t  *sensors,
                                 const PowerMetrics_t   *power,
                                 const Supercap_Status_t *supercap);

/**
 * @brief  Transmit a raw null-terminated string over USART2.
 * @param  str  C-string to transmit (must remain valid until TX complete).
 * @return COMM_OK or error code.
 */
Comm_Status_t Comm_SendString(const char *str);

/**
 * @brief  Non-blocking check – returns 1 when TX is idle.
 */
uint8_t Comm_IsTxReady(void);

/**
 * @brief  Process any incoming ACK/NACK bytes from ESP8266.
 *         Call from USART2 RX interrupt handler or main loop.
 */
void Comm_ProcessRx(void);

#ifdef __cplusplus
}
#endif

#endif /* COMMUNICATION_H */
