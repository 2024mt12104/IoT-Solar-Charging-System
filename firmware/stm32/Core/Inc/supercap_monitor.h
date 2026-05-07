/**
 * @file    supercap_monitor.h
 * @brief   Supercapacitor state-of-charge and energy tracking
 *
 * Monitors the supercapacitor voltage, estimates stored energy, and
 * implements a simple charge/discharge state machine that interfaces
 * with the PWM controller to protect the capacitor from over-/under-voltage.
 *
 * Energy stored in a supercapacitor: E = 0.5 × C × V²
 *
 * Hardware:
 *   - Supercapacitor nominal capacitance C = 50 F (configurable)
 *   - Voltage measured via ADC1 CH8 (PB0) with voltage divider
 *   - Rated voltage: 2.7 V per cell, series-stack of 4 → 10.8 V max
 */

#ifndef SUPERCAP_MONITOR_H
#define SUPERCAP_MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------- */

/** Supercapacitor capacitance [mF] (50 F = 50 000 mF) */
#define SUPERCAP_CAPACITANCE_MF       50000U

/** Maximum safe voltage [mV] */
#define SUPERCAP_VMAX_MV              10800U   /* 4 × 2.7 V */

/** Minimum usable voltage [mV] – below this, switch to battery */
#define SUPERCAP_VMIN_MV              2000U

/** Voltage hysteresis band [mV] to prevent rapid state toggling */
#define SUPERCAP_VHYST_MV             200U

/** State-of-charge full threshold [%] */
#define SUPERCAP_SOC_FULL_PCT         90U

/** State-of-charge low threshold [%] */
#define SUPERCAP_SOC_LOW_PCT          15U

/* -------------------------------------------------------------------------
 * Data types
 * ---------------------------------------------------------------------- */

typedef enum {
    SUPERCAP_STATE_IDLE = 0,    /**< Neither charging nor discharging */
    SUPERCAP_STATE_CHARGING,    /**< Being charged from solar/input */
    SUPERCAP_STATE_DISCHARGING, /**< Supplying energy to battery */
    SUPERCAP_STATE_FULL,        /**< At maximum safe charge */
    SUPERCAP_STATE_LOW,         /**< Below minimum usable voltage */
    SUPERCAP_STATE_FAULT        /**< Over-voltage fault */
} Supercap_State_t;

typedef struct {
    uint16_t       voltage_mV;  /**< Current voltage [mV] */
    uint8_t        soc_pct;     /**< State of charge [0–100 %] */
    uint32_t       energy_mJ;   /**< Stored energy [mJ] */
    Supercap_State_t state;     /**< Current state machine state */
} Supercap_Status_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/**
 * @brief  Initialise supercapacitor monitor module.
 */
void Supercap_Init(void);

/**
 * @brief  Update supercapacitor state from latest voltage measurement.
 *         Call from the 10 ms sensor update loop.
 * @param  v_mV  Current supercapacitor voltage [mV].
 */
void Supercap_Update(uint16_t v_mV);

/**
 * @brief  Retrieve current supercapacitor status snapshot.
 * @param  out  Pointer to caller-allocated Supercap_Status_t.
 */
void Supercap_GetStatus(Supercap_Status_t *out);

/**
 * @brief  Calculate state-of-charge from voltage using energy formula.
 * @param  v_mV  Measured voltage [mV].
 * @return SoC [0–100 %].
 */
uint8_t Supercap_CalcSoC(uint16_t v_mV);

/**
 * @brief  Calculate stored energy from voltage.
 * @param  v_mV  Measured voltage [mV].
 * @return Energy in millijoules [mJ].
 */
uint32_t Supercap_CalcEnergy_mJ(uint16_t v_mV);

/**
 * @brief  Returns non-zero if supercapacitor is ready to supply load.
 */
uint8_t Supercap_IsReady(void);

#ifdef __cplusplus
}
#endif

#endif /* SUPERCAP_MONITOR_H */
