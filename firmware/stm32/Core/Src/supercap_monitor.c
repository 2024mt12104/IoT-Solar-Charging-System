/**
 * @file    supercap_monitor.c
 * @brief   Supercapacitor state-of-charge and energy tracking
 *
 * Energy stored:  E = 0.5 × C × V²
 *   Using integer arithmetic (avoid floating-point on Cortex-M4 without FPU
 *   option set): E_mJ = (C_mF × V_mV²) / (2 × 1 000 000)
 *
 * SoC definition: ratio of stored energy to maximum stored energy.
 *   SoC = (V² − Vmin²) / (Vmax² − Vmin²) × 100 %
 */

#include "supercap_monitor.h"

/* -------------------------------------------------------------------------
 * Internal state
 * ---------------------------------------------------------------------- */

static Supercap_Status_t s_status;
static uint8_t           s_initialised;

/* -------------------------------------------------------------------------
 * Public implementation
 * ---------------------------------------------------------------------- */

void Supercap_Init(void)
{
    s_status.voltage_mV = 0U;
    s_status.soc_pct    = 0U;
    s_status.energy_mJ  = 0U;
    s_status.state      = SUPERCAP_STATE_IDLE;
    s_initialised       = 1U;
}

void Supercap_Update(uint16_t v_mV)
{
    if (!s_initialised) {
        return;
    }

    s_status.voltage_mV = v_mV;
    s_status.energy_mJ  = Supercap_CalcEnergy_mJ(v_mV);
    s_status.soc_pct    = Supercap_CalcSoC(v_mV);

    /* State machine with hysteresis */
    switch (s_status.state) {
        case SUPERCAP_STATE_FAULT:
            /* Stay in fault until voltage drops below OVP threshold */
            if (v_mV < SUPERCAP_VMAX_MV - SUPERCAP_VHYST_MV) {
                s_status.state = SUPERCAP_STATE_IDLE;
            }
            break;

        case SUPERCAP_STATE_FULL:
            /* Transition back to charging when voltage drops slightly */
            if (v_mV < SUPERCAP_VMAX_MV - SUPERCAP_VHYST_MV) {
                s_status.state = SUPERCAP_STATE_CHARGING;
            }
            break;

        case SUPERCAP_STATE_LOW:
            /* Need to charge before use */
            if (v_mV > SUPERCAP_VMIN_MV + SUPERCAP_VHYST_MV) {
                s_status.state = SUPERCAP_STATE_CHARGING;
            }
            break;

        default:
            /* Over-voltage check */
            if (v_mV >= SUPERCAP_VMAX_MV) {
                s_status.state = SUPERCAP_STATE_FAULT;
            } else if (v_mV >= (SUPERCAP_VMAX_MV * SUPERCAP_SOC_FULL_PCT / 100U)) {
                s_status.state = SUPERCAP_STATE_FULL;
            } else if (v_mV <= SUPERCAP_VMIN_MV) {
                s_status.state = SUPERCAP_STATE_LOW;
            } else if (s_status.soc_pct > SUPERCAP_SOC_FULL_PCT) {
                s_status.state = SUPERCAP_STATE_FULL;
            } else {
                /* Keep existing CHARGING / DISCHARGING / IDLE state unchanged */
            }
            break;
    }
}

void Supercap_GetStatus(Supercap_Status_t *out)
{
    if (out) {
        *out = s_status;
    }
}

uint8_t Supercap_CalcSoC(uint16_t v_mV)
{
    /* SoC = (V² − Vmin²) / (Vmax² − Vmin²) × 100 */
    uint32_t v2     = (uint32_t)v_mV * (uint32_t)v_mV;
    uint32_t vmin2  = (uint32_t)SUPERCAP_VMIN_MV * (uint32_t)SUPERCAP_VMIN_MV;
    uint32_t vmax2  = (uint32_t)SUPERCAP_VMAX_MV * (uint32_t)SUPERCAP_VMAX_MV;

    if (v2 <= vmin2) {
        return 0U;
    }
    if (v2 >= vmax2) {
        return 100U;
    }

    uint32_t numerator   = v2    - vmin2;
    uint32_t denominator = vmax2 - vmin2;
    return (uint8_t)((numerator * 100U) / denominator);
}

uint32_t Supercap_CalcEnergy_mJ(uint16_t v_mV)
{
    /* E_mJ = C_mF × V_mV² / (2 × 1 000 000)
     * Use 64-bit to avoid overflow (50000 × 10800² = 5.8 × 10^12) */
    uint64_t v2      = (uint64_t)v_mV * (uint64_t)v_mV;
    uint64_t energy  = ((uint64_t)SUPERCAP_CAPACITANCE_MF * v2) / 2000000ULL;
    return (uint32_t)energy;
}

uint8_t Supercap_IsReady(void)
{
    return (s_status.state == SUPERCAP_STATE_IDLE      ||
            s_status.state == SUPERCAP_STATE_CHARGING  ||
            s_status.state == SUPERCAP_STATE_FULL) ? 1U : 0U;
}
