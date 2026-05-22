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
 * @file    sensor_driver.c
 * @brief   ADC-based voltage and current sensor acquisition
 *
 * Uses STM32 HAL polling mode for simplicity.  In a production build,
 * replace HAL_ADC_PollForConversion() with DMA-circular mode for
 * continuous background sampling with zero CPU overhead.
 *
 * Calibration constants are derived from a 3.3 V reference and the
 * resistor-divider network described in sensor_driver.h.
 */

#include "sensor_driver.h"
#include <string.h>

/* -------------------------------------------------------------------------
 * HAL stub types – replaced by real stm32f4xx_hal.h in the actual build.
 * These stubs allow the file to be reviewed/compiled on a host machine.
 * ---------------------------------------------------------------------- */
#ifndef STM32F4XX_HAL_H
typedef struct { volatile uint32_t SR; volatile uint32_t CR1; } ADC_TypeDef;
typedef struct { ADC_TypeDef *Instance; } ADC_HandleTypeDef;
#define HAL_ADC_Start(h)             (0)
#define HAL_ADC_PollForConversion(h,t) (0)
#define HAL_ADC_GetValue(h)          (0U)
#define HAL_ADC_Stop(h)              (0)
#define HAL_MAX_DELAY                (0xFFFFFFFFU)
extern ADC_HandleTypeDef hadc1;  /* defined in main.c / CubeMX output */
#endif

/* -------------------------------------------------------------------------
 * Internal state
 * ---------------------------------------------------------------------- */

/** ADC channel index mapping */
#define ADC_CH_VSOLAR   0U
#define ADC_CH_VBAT     1U
#define ADC_CH_IIN      2U
#define ADC_CH_IOUT     3U
#define ADC_CH_VCAP     4U
#define ADC_NUM_CH      5U

static uint16_t s_raw[ADC_NUM_CH];                          /* latest raw samples */
static uint16_t s_filter_buf[ADC_NUM_CH][SENSOR_FILTER_SIZE]; /* ring buffer */
static uint8_t  s_filter_idx;                               /* ring buffer index */
static uint8_t  s_initialised;

/* -------------------------------------------------------------------------
 * Private helpers
 * ---------------------------------------------------------------------- */

static void prv_read_all_channels(void)
{
    /* In a real HAL-based build the ADC is configured for scan mode with
     * DMA.  Here we simulate sequential polling for each virtual channel. */
    for (uint8_t ch = 0U; ch < ADC_NUM_CH; ch++) {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        s_raw[ch] = (uint16_t)HAL_ADC_GetValue(&hadc1);
        HAL_ADC_Stop(&hadc1);
    }
}

static uint16_t prv_filter_channel(uint8_t ch)
{
    uint32_t sum = 0U;
    for (uint8_t i = 0U; i < SENSOR_FILTER_SIZE; i++) {
        sum += s_filter_buf[ch][i];
    }
    return (uint16_t)(sum / SENSOR_FILTER_SIZE);
}

/* -------------------------------------------------------------------------
 * Public implementation
 * ---------------------------------------------------------------------- */

void Sensor_Init(void)
{
    memset(s_filter_buf, 0, sizeof(s_filter_buf));
    s_filter_idx  = 0U;
    s_initialised = 1U;
}

void Sensor_Update(void)
{
    if (!s_initialised) {
        return;
    }

    prv_read_all_channels();

    /* Push raw values into ring buffers */
    for (uint8_t ch = 0U; ch < ADC_NUM_CH; ch++) {
        s_filter_buf[ch][s_filter_idx] = s_raw[ch];
    }
    s_filter_idx = (uint8_t)((s_filter_idx + 1U) & (SENSOR_FILTER_SIZE - 1U));
}

void Sensor_GetReading(SensorReading_t *out)
{
    if (!out) {
        return;
    }

    uint16_t adc_solar  = prv_filter_channel(ADC_CH_VSOLAR);
    uint16_t adc_bat    = prv_filter_channel(ADC_CH_VBAT);
    uint16_t adc_cap    = prv_filter_channel(ADC_CH_VCAP);
    uint16_t adc_iin    = prv_filter_channel(ADC_CH_IIN);
    uint16_t adc_iout   = prv_filter_channel(ADC_CH_IOUT);

    uint16_t mv_solar   = Sensor_AdcToMillivolts(adc_solar);
    uint16_t mv_bat     = Sensor_AdcToMillivolts(adc_bat);
    uint16_t mv_cap     = Sensor_AdcToMillivolts(adc_cap);
    uint16_t mv_iin     = Sensor_AdcToMillivolts(adc_iin);
    uint16_t mv_iout    = Sensor_AdcToMillivolts(adc_iout);

    out->v_solar_mV    = (uint16_t)Sensor_ApplyVdivRatio(mv_solar);
    out->v_battery_mV  = (uint16_t)Sensor_ApplyVdivRatio(mv_bat);
    out->v_supercap_mV = (uint16_t)Sensor_ApplyVdivRatio(mv_cap);
    out->i_input_mA    = Sensor_VoltsToMilliamps(mv_iin);
    out->i_output_mA   = Sensor_VoltsToMilliamps(mv_iout);
}

uint16_t Sensor_AdcToMillivolts(uint16_t raw)
{
    /* mV = (raw / 4095) × 3300  →  avoid division, use multiply-then-divide */
    return (uint16_t)(((uint32_t)raw * SENSOR_VREF_MV) / SENSOR_ADC_MAX);
}

uint32_t Sensor_ApplyVdivRatio(uint16_t mV)
{
    /* Actual voltage = mV × (R1+R2)/R2  (ratio stored ×100) */
    return ((uint32_t)mV * SENSOR_VDIV_RATIO_X100) / 100U;
}

int16_t Sensor_VoltsToMilliamps(uint16_t mV)
{
    /* ACS712: I = (Vmeasured − Voffset) / sensitivity */
    int32_t delta_mV = (int32_t)mV - (int32_t)SENSOR_ISENSE_OFFSET_MV;
    /* mA = delta_mV × 1000 / sensitivity_mV_per_A */
    return (int16_t)((delta_mV * 1000) / (int32_t)SENSOR_ISENSE_SENSITIVITY_MV_PER_A);
}
