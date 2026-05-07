/**
 * @file    power_calc.h
 * @brief   Real-time power and energy calculations
 *
 * Computes:
 *   - Instantaneous input/output power:  P = V × I
 *   - Cumulative energy:                 E = ∫ P dt  (trapezoidal rule)
 *   - System efficiency:                 η = P_out / P_in × 100 %
 *   - Charging session tracking (time, energy delivered)
 */

#ifndef POWER_CALC_H
#define POWER_CALC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------- */

/** Integration time step [ms] – must match the Sensor_Update call rate */
#define POWER_CALC_DT_MS      10U

/** Minimum power threshold [mW] to count as an active charging session */
#define POWER_CALC_MIN_PWR_MW 100U

/* -------------------------------------------------------------------------
 * Data types
 * ---------------------------------------------------------------------- */

typedef struct {
    uint32_t p_input_mW;      /**< Input power [mW]  */
    uint32_t p_output_mW;     /**< Output power [mW] */
    uint32_t e_input_mJ;      /**< Cumulative input energy  [mJ] */
    uint32_t e_output_mJ;     /**< Cumulative output energy [mJ] */
    uint8_t  efficiency_pct;  /**< η = P_out/P_in × 100 [%] */
    uint32_t session_time_s;  /**< Active charging session duration [s] */
} PowerMetrics_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/**
 * @brief  Initialise power calculator (reset accumulators).
 */
void Power_Init(void);

/**
 * @brief  Update power metrics from latest sensor readings.
 *         Call every POWER_CALC_DT_MS milliseconds.
 * @param  v_in_mV    Input voltage  [mV].
 * @param  i_in_mA    Input current  [mA].
 * @param  v_out_mV   Output voltage [mV].
 * @param  i_out_mA   Output current [mA].
 */
void Power_Update(uint16_t v_in_mV,  int16_t i_in_mA,
                  uint16_t v_out_mV, int16_t i_out_mA);

/**
 * @brief  Retrieve current power metrics snapshot.
 * @param  out  Pointer to caller-allocated PowerMetrics_t.
 */
void Power_GetMetrics(PowerMetrics_t *out);

/**
 * @brief  Reset energy accumulators and session timer to zero.
 */
void Power_ResetSession(void);

/**
 * @brief  Calculate instantaneous power from voltage and current.
 * @param  v_mV  Voltage [mV].
 * @param  i_mA  Current [mA].
 * @return Power [mW].
 */
uint32_t Power_CalcInstant_mW(uint16_t v_mV, int16_t i_mA);

/**
 * @brief  Calculate efficiency percentage.
 * @param  p_out_mW Output power [mW].
 * @param  p_in_mW  Input power  [mW].
 * @return Efficiency [0–100 %].  Returns 0 when p_in == 0.
 */
uint8_t Power_CalcEfficiency(uint32_t p_out_mW, uint32_t p_in_mW);

#ifdef __cplusplus
}
#endif

#endif /* POWER_CALC_H */
