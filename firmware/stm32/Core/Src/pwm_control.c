/**
 * @file    pwm_control.c
 * @brief   DC-DC converter PWM duty-cycle regulation
 *
 * Implements:
 *   1. Soft-start  – ramps duty cycle from 0 to operating point
 *   2. Constant-voltage regulation – simple bang-bang with dead-band
 *   3. MPPT perturb-and-observe  – maximises input power from solar
 *   4. OVP / OCP protection      – shuts output on fault
 */

#include "pwm_control.h"

/* -------------------------------------------------------------------------
 * HAL stubs (replaced by real HAL in actual build)
 * ---------------------------------------------------------------------- */
#ifndef STM32F4XX_HAL_H
typedef struct { volatile uint32_t CR1; } TIM_TypeDef;
typedef struct {
    TIM_TypeDef *Instance;
    struct { uint32_t Period; } Init;
} TIM_HandleTypeDef;
#define TIM_CHANNEL_1             (0x00000000U)
#define HAL_TIM_PWM_Start(h,ch)   (0)
#define HAL_TIM_PWM_Stop(h,ch)    (0)
#define __HAL_TIM_SET_COMPARE(h,ch,v) do{}while(0)
extern TIM_HandleTypeDef htim1;
#endif

/* -------------------------------------------------------------------------
 * Internal state
 * ---------------------------------------------------------------------- */

static uint8_t      s_duty_pct;
static PWM_State_t  s_state;
static uint32_t     s_prev_p_in_mW;
static int8_t       s_mppt_direction;  /* +1 / -1 */

/* -------------------------------------------------------------------------
 * Private helpers
 * ---------------------------------------------------------------------- */

static void prv_apply_duty(uint8_t duty_pct)
{
    uint32_t period = htim1.Init.Period;
    uint32_t compare = ((uint32_t)duty_pct * period) / 100U;
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare);
    s_duty_pct = duty_pct;
}

static uint8_t prv_clamp_duty(uint8_t d)
{
    if (d < PWM_DUTY_MIN_PCT) {
        return PWM_DUTY_MIN_PCT;
    }
    if (d > PWM_DUTY_MAX_PCT) {
        return PWM_DUTY_MAX_PCT;
    }
    return d;
}

/* -------------------------------------------------------------------------
 * Public implementation
 * ---------------------------------------------------------------------- */

void PWM_Init(void)
{
    s_duty_pct       = 0U;
    s_state          = PWM_STATE_STOPPED;
    s_prev_p_in_mW   = 0U;
    s_mppt_direction = 1;
    prv_apply_duty(0U);
}

void PWM_Start(void)
{
    if (s_state == PWM_STATE_FAULT) {
        return;  /* Fault must be cleared explicitly */
    }
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    s_state    = PWM_STATE_SOFTSTART;
    s_duty_pct = 0U;
}

void PWM_Stop(void)
{
    prv_apply_duty(0U);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    s_state = PWM_STATE_STOPPED;
}

void PWM_SetDuty(uint8_t duty_pct)
{
    prv_apply_duty(prv_clamp_duty(duty_pct));
}

uint8_t PWM_GetDuty(void)
{
    return s_duty_pct;
}

void PWM_RegulationStep(uint16_t v_out_mV, int16_t i_out_mA)
{
    /* --- Protection checks -------------------------------------------- */
    if (v_out_mV > PWM_OVP_THRESHOLD_MV) {
        PWM_Stop();
        s_state = PWM_STATE_FAULT;
        return;
    }
    if (i_out_mA > (int16_t)PWM_OCP_THRESHOLD_MA) {
        PWM_Stop();
        s_state = PWM_STATE_FAULT;
        return;
    }

    /* --- Soft-start ---------------------------------------------------- */
    if (s_state == PWM_STATE_SOFTSTART) {
        uint8_t new_duty = prv_clamp_duty(s_duty_pct + PWM_SOFTSTART_STEP_PCT);
        prv_apply_duty(new_duty);
        if (new_duty >= PWM_DUTY_MIN_PCT + 5U) {
            s_state = PWM_STATE_RUNNING;
        }
        return;
    }

    if (s_state != PWM_STATE_RUNNING) {
        return;
    }

    /* --- Constant-voltage bang-bang with dead-band -------------------- */
    int32_t error = (int32_t)v_out_mV - (int32_t)PWM_TARGET_VOUT_MV;

    if (error > (int32_t)PWM_VREG_BAND_MV) {
        /* Output too high – decrease duty cycle */
        if (s_duty_pct > PWM_DUTY_MIN_PCT) {
            prv_apply_duty(s_duty_pct - 1U);
        }
    } else if (error < -(int32_t)PWM_VREG_BAND_MV) {
        /* Output too low – increase duty cycle */
        if (s_duty_pct < PWM_DUTY_MAX_PCT) {
            prv_apply_duty(s_duty_pct + 1U);
        }
    }
    /* Within dead-band: no change */
}

void PWM_MpptStep(uint32_t p_in_mW)
{
    if (s_state != PWM_STATE_RUNNING) {
        return;
    }

    /* Perturb-and-observe MPPT */
    if (p_in_mW >= s_prev_p_in_mW) {
        /* Power increased – continue in same direction */
        uint8_t new_d = prv_clamp_duty((uint8_t)((int8_t)s_duty_pct + s_mppt_direction));
        prv_apply_duty(new_d);
    } else {
        /* Power decreased – reverse direction */
        s_mppt_direction = (int8_t)-s_mppt_direction;
        uint8_t new_d = prv_clamp_duty((uint8_t)((int8_t)s_duty_pct + s_mppt_direction));
        prv_apply_duty(new_d);
    }

    s_prev_p_in_mW = p_in_mW;
}

PWM_State_t PWM_GetState(void)
{
    return s_state;
}

void PWM_ClearFault(void)
{
    if (s_state == PWM_STATE_FAULT) {
        s_state    = PWM_STATE_STOPPED;
        s_duty_pct = 0U;
    }
}
