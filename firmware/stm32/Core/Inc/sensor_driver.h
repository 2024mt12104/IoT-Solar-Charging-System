/**
 * @file    sensor_driver.h
 * @brief   Voltage and current sensor acquisition via STM32 ADC
 *
 * Provides functions to initialise ADC channels, read raw samples,
 * apply a moving-average filter, and return calibrated physical values
 * for the solar-assisted battery charging system.
 *
 * Hardware mapping (STM32F446RE Nucleo):
 *   - ADC1 CH0  (PA0) : Solar/input voltage divider
 *   - ADC1 CH1  (PA1) : Battery/output voltage divider
 *   - ADC1 CH4  (PA4) : Input current  (ACS712 / INA219)
 *   - ADC1 CH5  (PA5) : Output current (ACS712 / INA219)
 *   - ADC1 CH8  (PB0) : Supercapacitor voltage divider
 */

#ifndef SENSOR_DRIVER_H
#define SENSOR_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* -------------------------------------------------------------------------
 * Configuration
 * ---------------------------------------------------------------------- */

/** ADC reference voltage in millivolts */
#define SENSOR_VREF_MV          3300U

/** ADC resolution (12-bit => 4095) */
#define SENSOR_ADC_MAX          4095U

/** Moving-average filter window size (must be power-of-2 for speed) */
#define SENSOR_FILTER_SIZE      8U

/**
 * Voltage-divider ratio for input / output voltage channels.
 *   R1 = 47 kΩ, R2 = 10 kΩ  →  ratio = (R1+R2)/R2 = 5.7
 *   Scaled ×100 to avoid floating-point in integer math.
 */
#define SENSOR_VDIV_RATIO_X100  570U   /* (R1+R2)/R2 * 100 */

/**
 * ACS712-5A sensitivity: 185 mV/A, offset ≈ Vcc/2 = 1650 mV.
 * Sensitivity scaled ×1000 for mA resolution.
 */
#define SENSOR_ISENSE_SENSITIVITY_MV_PER_A   185U
#define SENSOR_ISENSE_OFFSET_MV              1650U

/* -------------------------------------------------------------------------
 * Data types
 * ---------------------------------------------------------------------- */

/** Aggregated sensor snapshot returned to the application */
typedef struct {
    uint16_t v_solar_mV;     /**< Solar/input voltage  [mV] */
    uint16_t v_battery_mV;   /**< Battery/output voltage [mV] */
    uint16_t v_supercap_mV;  /**< Supercapacitor voltage [mV] */
    int16_t  i_input_mA;     /**< Input current  [mA], signed */
    int16_t  i_output_mA;    /**< Output current [mA], signed */
} SensorReading_t;

/* -------------------------------------------------------------------------
 * Public API
 * ---------------------------------------------------------------------- */

/**
 * @brief  Initialise sensor driver (ADC, DMA, filter buffers).
 *         Must be called once before any other sensor function.
 */
void Sensor_Init(void);

/**
 * @brief  Trigger a new ADC conversion cycle and update filter buffers.
 *         Call periodically from a 10 ms timer ISR or the main loop.
 */
void Sensor_Update(void);

/**
 * @brief  Return the latest calibrated sensor snapshot.
 * @param  out  Pointer to caller-allocated SensorReading_t structure.
 */
void Sensor_GetReading(SensorReading_t *out);

/**
 * @brief  Convert a raw 12-bit ADC code to millivolts.
 * @param  raw  Raw ADC value [0-4095].
 * @return Voltage in millivolts.
 */
uint16_t Sensor_AdcToMillivolts(uint16_t raw);

/**
 * @brief  Apply voltage-divider back-calculation to a sensor mV reading.
 * @param  mV   Voltage measured at ADC pin [mV].
 * @return Actual node voltage [mV] before the divider.
 */
uint32_t Sensor_ApplyVdivRatio(uint16_t mV);

/**
 * @brief  Convert ACS712 ADC voltage to current in milliamps.
 * @param  mV   Voltage at ACS712 output pin [mV].
 * @return Signed current [mA].
 */
int16_t Sensor_VoltsToMilliamps(uint16_t mV);

#ifdef __cplusplus
}
#endif

#endif /* SENSOR_DRIVER_H */
