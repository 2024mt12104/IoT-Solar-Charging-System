/**
 * @file    main.h
 * @brief   STM32 application top-level header
 *
 * Includes the STM32 HAL and defines common macros used across all
 * firmware modules.  In the CubeIDE project this file is auto-generated;
 * this version documents the expected definitions.
 */

#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include the correct HAL header for the target MCU */
#ifdef USE_HAL_DRIVER
#include "stm32f4xx_hal.h"
#else
/* Host-compilation stub */
#include <stdint.h>
#define HAL_Init()      do{}while(0)
#define HAL_Delay(ms)   do{}while(0)
#define __disable_irq() do{}while(0)

/* PWM state type forward (defined in pwm_control.h) */
typedef enum {
    PWM_STATE_STOPPED = 0,
    PWM_STATE_SOFTSTART,
    PWM_STATE_RUNNING,
    PWM_STATE_FAULT
} PWM_State_t;

/* Peripheral handle stubs */
typedef struct { volatile uint32_t SR; } ADC_TypeDef;
typedef struct { ADC_TypeDef *Instance; } ADC_HandleTypeDef;
typedef struct { volatile uint32_t CR1; } TIM_TypeDef;
typedef struct {
    TIM_TypeDef *Instance;
    struct { uint32_t Period; } Init;
} TIM_HandleTypeDef;
typedef struct { volatile uint32_t SR; } USART_TypeDef;
typedef struct { USART_TypeDef *Instance; } UART_HandleTypeDef;

/* Global peripheral handles – defined in main.c */
extern ADC_HandleTypeDef  hadc1;
extern TIM_HandleTypeDef  htim1;
extern UART_HandleTypeDef huart2;

/* HAL function stubs */
static inline uint32_t HAL_ADC_Start(ADC_HandleTypeDef *h) { (void)h; return 0; }
static inline uint32_t HAL_ADC_PollForConversion(ADC_HandleTypeDef *h, uint32_t t) { (void)h; (void)t; return 0; }
static inline uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *h) { (void)h; return 2048U; }
static inline uint32_t HAL_ADC_Stop(ADC_HandleTypeDef *h) { (void)h; return 0; }
static inline uint32_t HAL_TIM_PWM_Start(TIM_HandleTypeDef *h, uint32_t ch) { (void)h; (void)ch; return 0; }
static inline uint32_t HAL_TIM_PWM_Stop(TIM_HandleTypeDef *h, uint32_t ch) { (void)h; (void)ch; return 0; }
#define __HAL_TIM_SET_COMPARE(h,ch,v) do{}while(0)
static inline uint32_t HAL_UART_Transmit(UART_HandleTypeDef *h, uint8_t *b, uint16_t l, uint32_t t) { (void)h;(void)b;(void)l;(void)t; return 0; }

#define HAL_OK            (0U)
#define HAL_MAX_DELAY     (0xFFFFFFFFU)
#define TIM_CHANNEL_1     (0x00000000U)

ADC_HandleTypeDef  hadc1;
TIM_HandleTypeDef  htim1;
UART_HandleTypeDef huart2;
#endif /* USE_HAL_DRIVER */

void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
