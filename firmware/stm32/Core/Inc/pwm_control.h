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
 * @file    pwm_control.h
 * @brief   DC-DC converter PWM duty-cycle regulation
 *
 * Controls TIM1 CH1 on the STM32F446RE to generate a variable-frequency
 * PWM signal driving a buck/boost converter.  Includes soft-start,
 * over-voltage / over-current protection and MPPT step.
 *
 * Hardware:
 *   - TIM1 CH1 (PA8) → Gate driver → DC-DC converter
 *   - Timer base: 168 MHz / PSC / ARR → switching frequency ~100 kHz
 */

#ifndef PWM_CONTROL_H
#define PWM_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------- */

/** PWM switching frequency [Hz] */
#define PWM_SWITCHING_FREQ_HZ     100000U

/** Minimum safe duty cycle [%] – prevents discontinuous mode issues */
#define PWM_DUTY_MIN_PCT          5U

/** Maximum allowed duty cycle [%] */
#define PWM_DUTY_MAX_PCT          90U

/** Soft-start step size [% per call] */
#define PWM_SOFTSTART_STEP_PCT    1U

/** Target output voltage for constant-voltage charging [mV] */
#define PWM_TARGET_VOUT_MV        14400U   /* 14.4 V for 12 V lead-acid */

/** Over-voltage threshold [mV] – triggers protection */
#define PWM_OVP_THRESHOLD_MV      15000U

/** Over-current threshold [mA] – triggers protection */
#define PWM_OCP_THRESHOLD_MA      3000U

/** Voltage regulation band [mV] – dead-band for duty cycle adjustments */
#define PWM_VREG_BAND_MV          100U

/* -------------------------------------------------------------------------
 * Data types
 * ---------------------------------------------------------------------- */

typedef enum {
    PWM_STATE_STOPPED = 0,  /**< Output disabled */
    PWM_STATE_SOFTSTART,    /**< Ramping duty cycle to operating point */
    PWM_STATE_RUNNING,      /**< Normal regulation */
    PWM_STATE_FAULT         /**< OVP / OCP triggered – output disabled */
} PWM_State_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/**
 * @brief  Initialise TIM1 for PWM generation and start with 0 % duty.
 */
void PWM_Init(void);

/**
 * @brief  Enable PWM output and begin soft-start sequence.
 */
void PWM_Start(void);

/**
 * @brief  Disable PWM output immediately (e.g. fault condition).
 */
void PWM_Stop(void);

/**
 * @brief  Set duty cycle directly.
 * @param  duty_pct  Desired duty cycle [0-100 %].
 *                   Clamped to [PWM_DUTY_MIN_PCT, PWM_DUTY_MAX_PCT].
 */
void PWM_SetDuty(uint8_t duty_pct);

/**
 * @brief  Get current duty cycle [%].
 */
uint8_t PWM_GetDuty(void);

/**
 * @brief  Voltage-feedback regulation step.
 *         Call from a 1 ms control-loop timer.
 * @param  v_out_mV   Measured output voltage [mV].
 * @param  i_out_mA   Measured output current [mA].
 */
void PWM_RegulationStep(uint16_t v_out_mV, int16_t i_out_mA);

/**
 * @brief  MPPT perturb-and-observe step.
 *         Call from a 100 ms timer for solar tracking.
 * @param  p_in_mW    Current input power [mW].
 */
void PWM_MpptStep(uint32_t p_in_mW);

/**
 * @brief  Return current PWM state machine state.
 */
PWM_State_t PWM_GetState(void);

/**
 * @brief  Clear fault state and attempt restart.
 */
void PWM_ClearFault(void);

#ifdef __cplusplus
}
#endif

#endif /* PWM_CONTROL_H */
