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

/** @addtogroup TMR_ParallelSynchro
  @{
  */

/** @defgroup TMR_ParallelSynchro_Macros Macros
  @{
*/

/**@} end of group TMR_ParallelSynchro_Macros */

/** @defgroup TMR_ParallelSynchro_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_ParallelSynchro_Enumerations */

/** @defgroup TMR_ParallelSynchro_Structures Structures
  @{
*/

/**@} end of group TMR_ParallelSynchro_Structures */

/** @defgroup TMR_ParallelSynchro_Variables Variables
  @{
*/

/**@} end of group TMR_ParallelSynchro_Variables */

/** @defgroup TMR_ParallelSynchro_Functions Functions
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

    /* Enable peripheral clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3 | RCM_APB1_PERIPH_TMR2);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA | RCM_AHB_PERIPH_GPIOB);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_15, GPIO_AF_PIN2);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);

    /* Configure TMR1, TMR2, TMR3 channel 1 GPIO */
    GPIO_ConfigStruct.pin = GPIO_PIN_15 | GPIO_PIN_8;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_4;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR1,TMR2,TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStruct.counterMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.div = 0;
    TMR_TimeBaseStruct.period = 127;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 3;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    TMR_TimeBaseStruct.period = 1;
    TMR_ConfigTimeBase(TMR2, &TMR_TimeBaseStruct);

    TMR_OCConfigStructInit(&OCcongigStruct);

    /* Configure TMR1, TMR2, TMR3 channel 1 */
    OCcongigStruct.OC_Mode = TMR_OC_MODE_PWM1;
    OCcongigStruct.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.Pulse = 64;
    TMR_OC1Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 2;
    TMR_OC1Config(TMR3, &OCcongigStruct);

    OCcongigStruct.Pulse = 1;
    TMR_OC1Config(TMR2, &OCcongigStruct);

    /* Enable TMR1 the Master Slave Mode */
    TMR_EnableMasterSlaveMode(TMR1);
    TMR_SelectOutputTrigger(TMR1, TMR_TRGOSOURCE_UPDATE);
    TMR_EnablePWMOutputs(TMR1);
    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR3, TMR_SLAVEMODE_GATED);
    TMR_SelectInputTrigger(TMR3, TMR_TS_ITR0);

    /* Selects the Slave Mode and Input Trigger source */
    TMR_SelectSlaveMode(TMR2, TMR_SLAVEMODE_GATED);
    TMR_SelectInputTrigger(TMR2, TMR_TS_ITR0);

    /* Enable the specified TMR peripheral */
    TMR_Enable(TMR2);
    TMR_Enable(TMR3);
    TMR_Enable(TMR1);

    while (1)
    {
    }
}

/**@} end of group TMR_ParallelSynchro_Functions */
/**@} end of group TMR_ParallelSynchro */
/**@} end of group Examples */
