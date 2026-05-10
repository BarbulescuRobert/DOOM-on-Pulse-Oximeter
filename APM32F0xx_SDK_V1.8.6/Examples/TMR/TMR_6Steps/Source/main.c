/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2022-09-20
 *
 * @attention
 *
 *  Copyright (C) 2022 Geehy Semiconductor
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
#include "main.h"
#include "board.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_tmr.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_gpio.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_6Steps
  @{
*/

/** @defgroup TMR_6Steps_Macros Macros
  @{
*/

/**@} end of group TMR_6Steps_Macros */

/** @defgroup TMR_6Steps_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_6Steps_Enumerations */

/** @defgroup TMR_6Steps_Structures Structures
  @{
*/

/**@} end of group TMR_6Steps_Structures */

/** @defgroup TMR_6Steps_Variables Variables
  @{
*/

uint8_t PWMStep = 0;
/* public variables */
/* The count of microseconds */
uint32_t cntUs = 0;
/* The count of milliseconds */
uint32_t cntMs = 0;
/*@} end of group TMR_6Steps_Variables */

/** @defgroup TMR_6Steps_Functions Functions
  @{
*/

void Delay_Init(void);
void Delay_ms(uint32_t nms);
void Delay_us(uint32_t nus);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    Delay_Init();

    GPIO_Config_T GPIO_ConfigStruct;
    TMR_TimeBase_T baseConfig;
    TMR_OCConfig_T OCcongigStruct;
    TMR_BDTInit_T BDTConfig;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA | RCM_AHB_PERIPH_GPIOB);

    /* TMR1_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);
    /* TMR1_CH2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_PIN2);
    /* TMR1_CH3 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_PIN2);
    /* TMR1_BKIN */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_PIN2);
    /* TMR1_CH1N */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN2);
    /* TMR1_CH2N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_0, GPIO_AF_PIN2);
    /* TMR1_CH3N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_1, GPIO_AF_PIN2);

    /* Config TMR1 GPIO for complementary output PWM */
    GPIO_ConfigStruct.pin     = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_6;
    GPIO_ConfigStruct.mode    = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed   = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR1 */
    baseConfig.clockDivision     = TMR_CKD_DIV1;
    baseConfig.counterMode       = TMR_COUNTER_MODE_UP;
    baseConfig.div               = 0;
    baseConfig.period            = 2047;
    baseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &baseConfig);

    /* Configure channel */
    OCcongigStruct.OC_Mode         = TMR_OC_MODE_TMRING;
    OCcongigStruct.Pulse           = 1023;
    OCcongigStruct.OC_Idlestate    = TMR_OCIDLESTATE_SET;
    OCcongigStruct.OC_OutputState  = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity     = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.OC_NIdlestate   = TMR_OCNIDLESTATE_SET;
    OCcongigStruct.OC_OutputNState = TMR_OUTPUT_NSTATE_ENABLE;
    OCcongigStruct.OC_NPolarity    = TMR_OC_NPOLARITY_HIGH;

    TMR_OC1Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 511;
    TMR_OC2Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 255;
    TMR_OC3Config(TMR1, &OCcongigStruct);

    /* Configures the Break feature, dead time, Lock level, the IMOS */
    BDTConfig.RMOS_State      = TMR_RMOS_STATE_ENABLE;
    BDTConfig.IMOS_State      = TMR_IMOS_STATE_ENABLE;
    BDTConfig.lockLevel       = TMR_LOCK_LEVEL_OFF;
    BDTConfig.deadTime        = 0x01;
    BDTConfig.breakState      = TMR_BREAK_STATE_DISABLE;
    BDTConfig.breakPolarity   = TMR_BREAK_POLARITY_HIGH;
    BDTConfig.automaticOutput = TMR_AUTOMATIC_OUTPUT_ENABLE;
    TMR_ConfigBDT(TMR1, &BDTConfig);

    /* Timer1 Commutation Interrupt */
    TMR_EnableInterrupt(TMR1, TMR_INT_CCU);
    NVIC_EnableIRQ(TMR1_BRK_UP_TRG_COM_IRQn);

    /* Enable Capture Compare Preload Control */
    TMR_EnableCCPreload(TMR1);

    TMR_EnablePWMOutputs(TMR1);
    TMR_Enable(TMR1);

    while (1)
    {
        /* Generate the TMR1 Commutation event by software every 1 second */
        Delay_ms(100);
        TMR_GenerateEvent(TMR1, TMR_EVENT_CCU);
    }
}

/*!
 * @brief       Update SystemCoreClock variable according to Clock Register Values
 *              The SystemCoreClock variable contains the core clock (HCLK)
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay_Init(void)
{
    SystemCoreClockUpdate();
}
/*!
 * @brief       Configures Delay ms.
 *
 * @param       nms: Specifies the delay to be configured.
 *              This parameter can be one of following parameters:
 *              @arg nms
 *
 *
 * @retval      None
 */
void Delay_ms(__IO uint32_t nms)
{
    SysTick_Config(SystemCoreClock / 1000);

    cntMs = nms;
    while (cntMs != 0);
}

/*!
 * @brief       Configures Delay us.
 *
 * @param       nus: Specifies the delay to be configured.
 *              This parameter can be one of following parameters:
 *              @arg nus
 *
 *
 * @retval      None
 */
void Delay_us(__IO uint32_t nus)
{
    SysTick_Config(SystemCoreClock / 1000000);

    cntUs = nus;
    while (cntUs != 0);
}

/**@} end of group TMR_6Steps_Functions */
/**@} end of group TMR_6Steps */
/**@} end of group Examples */
