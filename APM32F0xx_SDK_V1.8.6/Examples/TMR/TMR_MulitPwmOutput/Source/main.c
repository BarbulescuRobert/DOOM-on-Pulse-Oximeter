/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.3
 *
 * @date        2022-09-20
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be useful and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

/* Includes */
#include "Board.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_tmr.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_MulitPwmOutput
  @{
*/

/** @defgroup TMR_MulitPwmOutput_Variables Variables
  @{
*/

static TMR_OCConfig_T g_ocConfig =
{
    .OC_Mode         = TMR_OC_MODE_PWM1,
    .OC_Idlestate    = TMR_OCIDLESTATE_RESET,
    .OC_NIdlestate   = TMR_OCNIDLESTATE_RESET,
    .OC_OutputNState = TMR_OUTPUT_NSTATE_DISABLE,
    .OC_OutputState  = TMR_OUTPUT_STATE_ENABLE,
    .OC_Polarity     = TMR_OC_POLARITY_HIGH,
    .OC_NPolarity    = TMR_OC_NPOLARITY_HIGH,
};

static GPIO_Config_T g_gpioConfig =
{
    .mode    = GPIO_MODE_AF,
    .outtype = GPIO_OUT_TYPE_PP,
    .pupd    = GPIO_PUPD_PU,
    .speed   = GPIO_SPEED_50MHz
};

static TMR_TimeBase_T g_timeBaseConfig =
{
    .div = 47999,   /* fraq = 48MHz / (47999 + 1) = 1KHz */
    .period = 999,
    .counterMode = TMR_COUNTER_MODE_UP,
    .clockDivision = TMR_CKD_DIV1,
    .repetitionCounter = 0
};

/**@} end of group TMR_MulitPwmOutput_Variables */

/** @defgroup TMR_MulitPwmOutput_Functions Functions
  @{
*/

void TMR1_PWMOutPut_Initialize(void);
void TMR2_PWMOutPut_Initialize(void);
void TMR3_PWMOutPut_Initialize(void);
void TMR14_PWMOutPut_Initialize(void);
void TMR15_PWMOutPut_Initialize(void);
void TMR16_PWMOutPut_Initialize(void);
void TMR17_PWMOutPut_Initialize(void);
void GPIO_Initialize(void);

/*!
 * @brief Main program
 *
 * @param None
 *
 * @retval None
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDOn(LED2);

    GPIO_Initialize();

    TMR1_PWMOutPut_Initialize();
    TMR2_PWMOutPut_Initialize();
    TMR3_PWMOutPut_Initialize();
    TMR14_PWMOutPut_Initialize();
    TMR15_PWMOutPut_Initialize();
    TMR16_PWMOutPut_Initialize();
    TMR17_PWMOutPut_Initialize();

    for (;;)
    {
    }
}

/*!
 * @brief TMRx_OCy gpio initialize:
 * | TMRx  | OC1  | OC2  | OC3  | OC4  |
 * | ----- | ---- | ---- | ---- | ---- |
 * | TMR1  | PE9  | PE11 | PE13 | PE14 |
 * | TMR2  | PA0  | PA1  | PA2  | PA3  |
 * | TMR3  | PC6  | PC7  | PC8  | PC9  |
 * | TMR14 | PA7  | -    | -    | -    |
 * | TMR15 | PB14 | PB15 | -    | -    |
 * | TMR16 | PB8  | -    | -    | -    |
 * | TMR17 | PB9  | -    | -    | -    |
 *
 * @param None
 *
 * @retval None
 */
void GPIO_Initialize(void)
{
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOC);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOE);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /* TMR1 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_9 | GPIO_PIN_11 | GPIO_PIN_13 | GPIO_PIN_14;
    GPIO_Config(GPIOE, &g_gpioConfig);
    /* Connect TMR1_CH1 to PE9 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_9, GPIO_AF_PIN0);
    /* Connect TMR1_CH2 to PE11 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_PIN0);
    /* Connect TMR1_CH3 to PE13 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_13, GPIO_AF_PIN0);
    /* Connect TMR1_CH4 to PE14 */
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_14, GPIO_AF_PIN0);

    /* TMR2 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Config(GPIOA, &g_gpioConfig);
    /* Connect TMR2_CH1 to PA0 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_PIN2);
    /* Connect TMR2_CH2 to PA1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_1, GPIO_AF_PIN2);
    /* Connect TMR2_CH3 to PA2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_PIN2);
    /* Connect TMR2_CH4 to PA3 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_PIN2);

    /* TMR3 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_Config(GPIOC, &g_gpioConfig);
    /* Connect TMR3_CH1 to PC6 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_6, GPIO_AF_PIN0);
    /* Connect TMR3_CH2 to PC7 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_7, GPIO_AF_PIN0);
    /* Connect TMR3_CH3 to PC8 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_8, GPIO_AF_PIN0);
    /* Connect TMR3_CH4 to PC9 */
    GPIO_ConfigPinAF(GPIOC, GPIO_PIN_SOURCE_9, GPIO_AF_PIN0);

    /* TMR14 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_7;
    GPIO_Config(GPIOA, &g_gpioConfig);
    /* Connect TMR14_CH1 to PA7 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN4);

    /* TMR15 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Config(GPIOB, &g_gpioConfig);
    /* Connect TMR15_CH1 to PB14 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_PIN1);
    /* Connect TMR15_CH2 to PB15 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_PIN1);

    /* TMR16 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_8;
    GPIO_Config(GPIOB, &g_gpioConfig);
    /* Connect TMR16_CH1 to PB8 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);

    /* TMR17 GPIO Config */
    g_gpioConfig.pin = GPIO_PIN_9;
    GPIO_Config(GPIOB, &g_gpioConfig);
    /* Connect TMR17_CH1 to PB9 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF_PIN2);
}

/*!
 * @brief TMR1 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR1_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    TMR_ConfigTimeBase(TMR1, &g_timeBaseConfig);

    /* Set duty cycle to 10% */
    g_ocConfig.Pulse = 100;
    TMR_OC1Config(TMR1, &g_ocConfig);
    /* Set duty cycle to 30% */
    g_ocConfig.Pulse = 300;
    TMR_OC2Config(TMR1, &g_ocConfig);
    /* Set duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC3Config(TMR1, &g_ocConfig);
    /* Set duty cycle to 70% */
    g_ocConfig.Pulse = 700;
    TMR_OC4Config(TMR1, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR1);
    /* Enable TMR1 */
    TMR_Enable(TMR1);
}

/*!
 * @brief TMR2 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR2_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2);

    TMR_ConfigTimeBase(TMR2, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 10% */
    g_ocConfig.Pulse = 100;
    TMR_OC1Config(TMR2, &g_ocConfig);
    /* Set OC2 duty cycle to 30% */
    g_ocConfig.Pulse = 300;
    TMR_OC2Config(TMR2, &g_ocConfig);
    /* Set OC3 duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC3Config(TMR2, &g_ocConfig);
    /* Set OC4 duty cycle to 70% */
    g_ocConfig.Pulse = 700;
    TMR_OC4Config(TMR2, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR2);
    /* Enable TMR2 */
    TMR_Enable(TMR2);
}

/*!
 * @brief TMR3 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR3_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);

    TMR_ConfigTimeBase(TMR3, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 10% */
    g_ocConfig.Pulse = 100;
    TMR_OC1Config(TMR3, &g_ocConfig);
    /* Set OC2 duty cycle to 30% */
    g_ocConfig.Pulse = 300;
    TMR_OC2Config(TMR3, &g_ocConfig);
    /* Set OC3 duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC3Config(TMR3, &g_ocConfig);
    /* Set OC4 duty cycle to 70% */
    g_ocConfig.Pulse = 700;
    TMR_OC4Config(TMR3, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR3);
    /* Enable TMR3 */
    TMR_Enable(TMR3);
}

/*!
 * @brief TMR14 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR14_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR14);

    TMR_ConfigTimeBase(TMR14, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC1Config(TMR14, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR14);
    /* Enable TMR14 */
    TMR_Enable(TMR14);
}

/*!
 * @brief TMR15 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR15_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR15);

    TMR_ConfigTimeBase(TMR15, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 30% */
    g_ocConfig.Pulse = 300;
    TMR_OC1Config(TMR15, &g_ocConfig);
    /* Set OC2 duty cycle to 70% */
    g_ocConfig.Pulse = 700;
    TMR_OC2Config(TMR15, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR15);
    /* Enable TMR1 */
    TMR_Enable(TMR15);
}

/*!
 * @brief TMR15 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR16_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR16);

    TMR_ConfigTimeBase(TMR16, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC1Config(TMR16, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR16);
    /* Enable TMR16 */
    TMR_Enable(TMR16);
}

/*!
 * @brief TMR17 PWM Output Init
 *
 * @param None
 *
 * @retval None
 */
void TMR17_PWMOutPut_Initialize(void)
{
    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR17);

    TMR_ConfigTimeBase(TMR17, &g_timeBaseConfig);

    /* Set OC1 duty cycle to 50% */
    g_ocConfig.Pulse = 500;
    TMR_OC1Config(TMR17, &g_ocConfig);

    /* Enable PWM output */
    TMR_EnablePWMOutputs(TMR17);
    /* Enable TMR17 */
    TMR_Enable(TMR17);
}

/**@} end of group TMR_MulitPwmOutput_Functions */
/**@} end of group TMR_MulitPwmOutput */
/**@} end of group Examples */
