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
 * @file    main.c
 * @brief   IoT Solar Charging System – STM32F446RE main application
 *
 * Execution flow:
 *   1. HAL / CubeMX-generated peripheral initialisation (clocks, ADC, TIM, UART)
 *   2. Module initialisation (sensor, PWM, supercap, power, communication)
 *   3. PWM soft-start
 *   4. Main control loop at 10 ms cadence:
 *        a. Acquire sensor data
 *        b. Update power metrics
 *        c. Update supercapacitor state
 *        d. Execute PWM voltage regulation
 *        e. Every 1 s: transmit data frame to ESP8266
 *        f. Every 100 ms: MPPT perturb-and-observe step
 *
 * Note: Peripheral initialisation functions (MX_xxx_Init) are generated
 * by STM32CubeMX and are assumed to exist in the CubeIDE project.
 * The HAL_Delay() call is a blocking substitute; in production use a
 * hardware timer ISR to achieve accurate 10 ms scheduling.
 */

#include "main.h"
#include "sensor_driver.h"
#include "pwm_control.h"
#include "supercap_monitor.h"
#include "power_calc.h"
#include "communication.h"

/* -------------------------------------------------------------------------
 * Forward declarations of CubeMX-generated init functions
 * ---------------------------------------------------------------------- */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART2_UART_Init(void);

/* -------------------------------------------------------------------------
 * Timing constants
 * ---------------------------------------------------------------------- */
#define CTRL_LOOP_MS        10U     /* Main control loop period */
#define COMM_PERIOD_LOOPS   100U    /* Send frame every 100 × 10 ms = 1 s */
#define MPPT_PERIOD_LOOPS   10U     /* MPPT step every 10 × 10 ms = 100 ms */

/* -------------------------------------------------------------------------
 * Main entry point
 * ---------------------------------------------------------------------- */
int main(void)
{
    /* --- Hardware Abstraction Layer reset & initialise ----------------- */
    HAL_Init();
    SystemClock_Config();

    /* --- Peripheral initialisation (CubeMX generated) ----------------- */
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_TIM1_Init();
    MX_USART2_UART_Init();

    /* --- Application module initialisation ---------------------------- */
    Sensor_Init();
    Power_Init();
    Supercap_Init();
    Comm_Init();
    PWM_Init();

    /* --- Begin PWM soft-start ----------------------------------------- */
    PWM_Start();

    /* --- Main control loop -------------------------------------------- */
    uint32_t loop_counter  = 0U;

    SensorReading_t  sensor_data;
    PowerMetrics_t   power_data;
    Supercap_Status_t supercap_data;

    while (1)
    {
        /* 1. Acquire sensor data */
        Sensor_Update();
        Sensor_GetReading(&sensor_data);

        /* 2. Update power metrics */
        Power_Update(sensor_data.v_solar_mV,   sensor_data.i_input_mA,
                     sensor_data.v_battery_mV, sensor_data.i_output_mA);

        /* 3. Update supercapacitor state */
        Supercap_Update(sensor_data.v_supercap_mV);
        Supercap_GetStatus(&supercap_data);

        /* 4. PWM voltage regulation (1 ms inner loop via HAL_Delay here,
         *    but should be driven by TIM interrupt in production) */
        PWM_RegulationStep(sensor_data.v_battery_mV, sensor_data.i_output_mA);

        /* 5. MPPT step every MPPT_PERIOD_LOOPS × CTRL_LOOP_MS */
        if ((loop_counter % MPPT_PERIOD_LOOPS) == 0U)
        {
            Power_GetMetrics(&power_data);
            PWM_MpptStep(power_data.p_input_mW);
        }

        /* 6. Transmit data frame every COMM_PERIOD_LOOPS × CTRL_LOOP_MS */
        if ((loop_counter % COMM_PERIOD_LOOPS) == 0U)
        {
            Power_GetMetrics(&power_data);
            Comm_SendDataFrame(&sensor_data, &power_data, &supercap_data);
        }

        /* 7. Fault handling – attempt auto-recovery after OVP/OCP */
        if (PWM_GetState() == PWM_STATE_FAULT)
        {
            /* Simple strategy: wait 5 s then try to restart */
            HAL_Delay(5000U);
            PWM_ClearFault();
            PWM_Start();
        }

        loop_counter++;
        HAL_Delay(CTRL_LOOP_MS);
    }

    /* Should never reach here */
    return 0;
}

/* -------------------------------------------------------------------------
 * Placeholder CubeMX init functions
 * In the actual STM32CubeIDE project these are auto-generated.
 * ---------------------------------------------------------------------- */

void SystemClock_Config(void)
{
    /* 180 MHz HSE PLL configuration for STM32F446RE.
     * Configure RCC registers via HAL_RCC_OscConfig / HAL_RCC_ClockConfig. */
}

static void MX_GPIO_Init(void)
{
    /* Enable GPIO clocks and configure pin modes (output/input/alternate). */
}

static void MX_ADC1_Init(void)
{
    /* Configure ADC1 for 5-channel sequential scan, 12-bit resolution,
     * software trigger, 480-cycle sample time for accurate acquisition. */
}

static void MX_TIM1_Init(void)
{
    /* Configure TIM1 CH1 (PA8) for PWM mode 1.
     * PSC = 0, ARR = 1679 → 168 MHz / 1680 = 100 kHz switching frequency. */
}

static void MX_USART2_UART_Init(void)
{
    /* Configure USART2 on PA2/PA3, 115 200 baud, 8N1, no flow control. */
}

/* -------------------------------------------------------------------------
 * Error handler (CubeMX default)
 * ---------------------------------------------------------------------- */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        /* Spin – signal error via LED or watchdog reset in production */
    }
}
