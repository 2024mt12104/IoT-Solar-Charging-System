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
 * @file    power_calc.c
 * @brief   Real-time power and energy calculations
 *
 * Trapezoidal integration of instantaneous power over time:
 *   E[n] = E[n-1] + (P[n-1] + P[n]) × Δt / 2
 * With Δt = POWER_CALC_DT_MS / 1000 s, and all quantities in mW/mJ,
 * the integer approximation is:
 *   ΔE_mJ = (P_prev_mW + P_curr_mW) × dt_ms / 2000
 */

#include "power_calc.h"

/* -------------------------------------------------------------------------
 * Internal state
 * ---------------------------------------------------------------------- */

static PowerMetrics_t s_metrics;
static uint32_t       s_prev_p_in_mW;
static uint32_t       s_prev_p_out_mW;
static uint32_t       s_tick_ms;         /* millisecond counter */
static uint8_t        s_initialised;

/* -------------------------------------------------------------------------
 * Public implementation
 * ---------------------------------------------------------------------- */

void Power_Init(void)
{
    s_metrics.p_input_mW     = 0U;
    s_metrics.p_output_mW    = 0U;
    s_metrics.e_input_mJ     = 0U;
    s_metrics.e_output_mJ    = 0U;
    s_metrics.efficiency_pct = 0U;
    s_metrics.session_time_s = 0U;
    s_prev_p_in_mW           = 0U;
    s_prev_p_out_mW          = 0U;
    s_tick_ms                = 0U;
    s_initialised            = 1U;
}

void Power_Update(uint16_t v_in_mV,  int16_t i_in_mA,
                  uint16_t v_out_mV, int16_t i_out_mA)
{
    if (!s_initialised) {
        return;
    }

    /* Instantaneous power (unsigned – ignore reverse-flow sign for energy) */
    uint32_t p_in  = Power_CalcInstant_mW(v_in_mV,  i_in_mA);
    uint32_t p_out = Power_CalcInstant_mW(v_out_mV, i_out_mA);

    s_metrics.p_input_mW  = p_in;
    s_metrics.p_output_mW = p_out;

    /* Trapezoidal energy integration */
    s_metrics.e_input_mJ  += (s_prev_p_in_mW  + p_in)  * POWER_CALC_DT_MS / 2000U;
    s_metrics.e_output_mJ += (s_prev_p_out_mW + p_out) * POWER_CALC_DT_MS / 2000U;

    s_prev_p_in_mW  = p_in;
    s_prev_p_out_mW = p_out;

    /* Efficiency */
    s_metrics.efficiency_pct = Power_CalcEfficiency(p_out, p_in);

    /* Session timer – increment when power is above minimum threshold */
    s_tick_ms += POWER_CALC_DT_MS;
    if (p_in >= POWER_CALC_MIN_PWR_MW) {
        s_metrics.session_time_s = s_tick_ms / 1000U;
    }
}

void Power_GetMetrics(PowerMetrics_t *out)
{
    if (out) {
        *out = s_metrics;
    }
}

void Power_ResetSession(void)
{
    s_metrics.e_input_mJ     = 0U;
    s_metrics.e_output_mJ    = 0U;
    s_metrics.session_time_s = 0U;
    s_prev_p_in_mW           = 0U;
    s_prev_p_out_mW          = 0U;
    s_tick_ms                = 0U;
}

uint32_t Power_CalcInstant_mW(uint16_t v_mV, int16_t i_mA)
{
    /* P_mW = V_mV × |I_mA| / 1000 */
    int32_t i_abs = (i_mA < 0) ? -(int32_t)i_mA : (int32_t)i_mA;
    return ((uint32_t)v_mV * (uint32_t)i_abs) / 1000U;
}

uint8_t Power_CalcEfficiency(uint32_t p_out_mW, uint32_t p_in_mW)
{
    if (p_in_mW == 0U) {
        return 0U;
    }
    uint32_t eff = (p_out_mW * 100U) / p_in_mW;
    return (eff > 100U) ? 100U : (uint8_t)eff;
}
