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
#include "Board.h"
#include "apm32f0xx_tmr.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_ExtTriggerSynchro
  @{
*/

/** @defgroup TMR_ExtTriggerSynchro_Macros Macros
  @{
*/

/**@} end of group TMR_ExtTriggerSynchro_Macros */

/** @defgroup TMR_ExtTriggerSynchro_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_ExtTriggerSynchro_Enumerations */

/** @defgroup TMR_ExtTriggerSynchro_Structures Structures
  @{
*/

/**@} end of group TMR_ExtTriggerSynchro_Structures */

/** @defgroup TMR_ExtTriggerSynchro_Variables Variables
  @{
*/

/**@} end of group TMR_ExtTriggerSynchro_Variables */

/** @defgroup TMR_ExtTriggerSynchro_Functions Functions
  @{
*/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    GPIO_Config_T GPIO_ConfigStruct;
    TMR_TimeBase_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;
    TMR_ICConfig_T ICConfig;

    /* Enable peripheral clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR2 | RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA | RCM_AHB_PERIPH_GPIOB | RCM_AHB_PERIPH_GPIOE);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_0, GPIO_AF_PIN2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_PIN1);

    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_11, GPIO_AF_PIN0);

    /* Configure TMR1, TMR2, TMR3 channel 1 GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_0 | GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_4;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_11;
    GPIO_Config(GPIOE, &GPIO_ConfigStruct);

    /* TMR1 Input Capture mode */
    ICConfig.channel = TMR_CHANNEL_2;
    ICConfig.ICpolarity = TMR_IC_POLARITY_RISING;
    ICConfig.ICselection = TMR_IC_SELECTION_DIRECT_TI;
    ICConfig.ICprescaler = TMR_ICPSC_DIV1;

    ICConfig.ICfilter = 0x00;
    TMR_ICConfig(TMR1, &ICConfig);

    /* config TMR1,TMR2,TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStruct.counterMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.div = 0;
    TMR_TimeBaseStruct.repetitionCounter = 0;

    /* TMR1 frequency = 48MHz/2*(23+ 1) = 1MHz */
    TMR_TimeBaseStruct.period = 23;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    /* TMR2 frequency = 48MHz/2*(47 + 1) = 500KHz */
    TMR_TimeBaseStruct.period = 47;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    /* TMR3 frequency = 48MHz/2*(95 + 1) = 250KHz */
    TMR_TimeBaseStruct.period = 95;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    TMR_OCConfigStructInit(&OCcongigStruct);

    /* Configure TMR1, TMR2, TMR3 channel 1 */
    OCcongigStruct.OC_Mode         = TMR_OC_MODE_TOGGEL;
    OCcongigStruct.OC_OutputState  = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity     = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.OC_Idlestate    = TMR_OCIDLESTATE_SET;
    OCcongigStruct.OC_OutputNState = TMR_OUTPUT_NSTATE_DISABLE;
    OCcongigStruct.OC_NPolarity    = TMR_OC_NPOLARITY_HIGH;
    OCcongigStruct.OC_NIdlestate   = TMR_OCNIDLESTATE_RESET;

    TMR_OC1Config(TMR1, &OCcongigStruct);
    TMR_OC1Config(TMR2, &OCcongigStruct);
    TMR_OC1Config(TMR3, &OCcongigStruct);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR1, TMR_SLAVEMODE_GATED);
    TMR_SelectInputTrigger(TMR1, TMR_TS_TI2FP2);

    /* Enable TMR1 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR1);
    TMR_SelectOutputTrigger(TMR1, TMR_TRGOSOURCE_ENABLE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR2, TMR_SLAVEMODE_GATED);
    TMR_SelectInputTrigger(TMR2, TMR_TS_ITR0);

    /* Enable TMR2 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR2);
    TMR_SelectOutputTrigger(TMR2, TMR_TRGOSOURCE_ENABLE);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR3, TMR_SLAVEMODE_GATED);
    TMR_SelectInputTrigger(TMR3, TMR_TS_ITR1);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR1);
    TMR_Enable(TMR2);
    TMR_Enable(TMR3);

    TMR_EnablePWMOutputs(TMR1);

    while (1)
    {
    }
}

/**@} end of group TMR_ExtTriggerSynchro_Functions */
/**@} end of group TMR_ExtTriggerSynchro */
/**@} end of group Examples */
