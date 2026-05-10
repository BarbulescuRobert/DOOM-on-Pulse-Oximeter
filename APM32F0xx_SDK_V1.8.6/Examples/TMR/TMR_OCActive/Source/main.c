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

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_OCActive
  @{
*/

/** @defgroup TMR_OCActive_Macros Macros
  @{
*/

/**@} end of group TMR_OCActive_Macros */

/** @defgroup TMR_OCActive_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_OCActive_Enumerations */

/** @defgroup TMR_OCActive_Structures Structures
  @{
*/

/**@} end of group TMR_OCActive_Structures */

/** @defgroup TMR_OCActive_Variables Variables
  @{
*/

/**@} end of group TMR_OCActive_Variables */

/** @defgroup TMR_OCActive_Functions Functions
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

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);

    APM_MINI_LEDInit(LED2);

    /* TMR3_CH1 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_4, GPIO_AF_PIN1);
    /* TMR3_CH2 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_5, GPIO_AF_PIN1);
    /* TMR3_CH3 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_0, GPIO_AF_PIN1);
    /* TMR3_CH4 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_1, GPIO_AF_PIN1);

    /* Config TMR3 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStruct.counterMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.div = 47;
    TMR_TimeBaseStruct.period = 999;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_OCConfigStructInit(&OCcongigStruct);

    OCcongigStruct.OC_Mode = TMR_OC_MODE_ACTIVE;
    OCcongigStruct.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity = TMR_OC_POLARITY_HIGH;

    /* TMR3 CH1 delay = 800/1MHz = 800us */
    OCcongigStruct.Pulse = 800;
    TMR_OC1Config(TMR3, &OCcongigStruct);
    TMR_OC1PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH2 delay = 600/1MHz = 600us */
    OCcongigStruct.Pulse = 600;
    TMR_OC2Config(TMR3, &OCcongigStruct);
    TMR_OC2PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH3 delay = 400/1MHz = 400us */
    OCcongigStruct.Pulse = 400;
    TMR_OC3Config(TMR3, &OCcongigStruct);
    TMR_OC3PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* TMR3 CH4 delay = 200/1MHz = 200us */
    OCcongigStruct.Pulse = 200;
    TMR_OC4Config(TMR3, &OCcongigStruct);
    TMR_OC4PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    TMR_DisableAUTOReload(TMR3);

    /* PE6 generate falling edge */
    APM_MINI_LEDOn(LED2);

    /* start generate four different signals */
    TMR_Enable(TMR3);

    while (1)
    {
    }
}

/**@} end of group TMR_OCActive_Functions */
/**@} end of group TMR_OCActive */
/**@} end of group Examples */
