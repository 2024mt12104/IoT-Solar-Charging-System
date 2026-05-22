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
 * @file    communication.c
 * @brief   UART framing and transmission from STM32 to ESP8266
 *
 * Builds a compact JSON frame from the latest sensor/power/supercap
 * snapshots and transmits it over USART2 at 115 200 baud.
 *
 * Example output frame:
 * {"vs":12450,"vb":13200,"vc":8500,"ii":1200,"io":950,
 *  "pi":14940,"po":12540,"ei":45200,"eo":37900,"ef":83,
 *  "sc_soc":72,"sc_e":18170,"ts":38}
 */

#include "communication.h"
#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------
 * HAL stubs (replaced by real HAL in actual build)
 * ---------------------------------------------------------------------- */
#ifndef STM32F4XX_HAL_H
typedef struct { volatile uint32_t SR; } USART_TypeDef;
typedef struct { USART_TypeDef *Instance; } UART_HandleTypeDef;
#define HAL_OK        (0U)
#define HAL_UART_Transmit(h,b,l,t) (0U)
extern UART_HandleTypeDef huart2;
#endif

/* -------------------------------------------------------------------------
 * Internal state
 * ---------------------------------------------------------------------- */

static char    s_tx_buf[COMM_FRAME_MAX_LEN];
static uint8_t s_tx_busy;

/* -------------------------------------------------------------------------
 * Public implementation
 * ---------------------------------------------------------------------- */

void Comm_Init(void)
{
    s_tx_busy = 0U;
    memset(s_tx_buf, 0, sizeof(s_tx_buf));
    /* USART2 peripheral is configured by CubeMX / main.c MX_USART2_UART_Init */
}

Comm_Status_t Comm_SendDataFrame(const SensorReading_t  *sensors,
                                 const PowerMetrics_t   *power,
                                 const Supercap_Status_t *supercap)
{
    if (!sensors || !power || !supercap) {
        return COMM_ERR_TIMEOUT;
    }
    if (s_tx_busy) {
        return COMM_ERR_BUSY;
    }

    int len = snprintf(s_tx_buf, COMM_FRAME_MAX_LEN,
        "{\"vs\":%u,\"vb\":%u,\"vc\":%u,"
        "\"ii\":%d,\"io\":%d,"
        "\"pi\":%lu,\"po\":%lu,"
        "\"ei\":%lu,\"eo\":%lu,\"ef\":%u,"
        "\"sc_soc\":%u,\"sc_e\":%lu,\"ts\":%lu}\n",
        sensors->v_solar_mV,
        sensors->v_battery_mV,
        sensors->v_supercap_mV,
        sensors->i_input_mA,
        sensors->i_output_mA,
        (unsigned long)power->p_input_mW,
        (unsigned long)power->p_output_mW,
        (unsigned long)power->e_input_mJ,
        (unsigned long)power->e_output_mJ,
        power->efficiency_pct,
        supercap->soc_pct,
        (unsigned long)supercap->energy_mJ,
        (unsigned long)power->session_time_s);

    if (len <= 0 || len >= (int)COMM_FRAME_MAX_LEN) {
        return COMM_ERR_TIMEOUT;
    }

    return Comm_SendString(s_tx_buf);
}

Comm_Status_t Comm_SendString(const char *str)
{
    if (!str) {
        return COMM_ERR_TIMEOUT;
    }
    if (s_tx_busy) {
        return COMM_ERR_BUSY;
    }

    uint16_t len = (uint16_t)strlen(str);
    s_tx_busy    = 1U;

    uint32_t result = HAL_UART_Transmit(&huart2,
                                        (uint8_t *)str,
                                        len,
                                        1000U   /* 1 s timeout */);
    s_tx_busy = 0U;

    return (result == HAL_OK) ? COMM_OK : COMM_ERR_TIMEOUT;
}

uint8_t Comm_IsTxReady(void)
{
    return (s_tx_busy == 0U) ? 1U : 0U;
}

void Comm_ProcessRx(void)
{
    /* Optional: read one-byte ACK from ESP8266.
     * In the current design the ESP8266 does not send ACKs, so this
     * function is a no-op placeholder for future handshaking. */
}
