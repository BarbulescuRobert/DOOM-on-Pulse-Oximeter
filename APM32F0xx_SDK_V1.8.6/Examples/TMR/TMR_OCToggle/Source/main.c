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

/** @addtogroup TMR_OCToggle
  @{
  */

/** @defgroup TMR_OCToggle_Macros Macros
  @{
  */

/**@} end of group TMR_OCToggle_Macros */

/** @defgroup TMR_OCToggle_Enumerations Enumerations
  @{
  */

/**@} end of group TMR_OCToggle_Enumerations */

/** @defgroup TMR_OCToggle_Structures Structures
  @{
  */

/**@} end of group TMR_OCToggle_Structures */

/** @defgroup TMR_OCToggle_Variables Variables
  @{
  */

/**@} end of group TMR_OCToggle_Variables */


/** @defgroup TMR_OCToggle_Functions Functions
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
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    /* TMR3_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_PIN1);
    /* TMR3_CH2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN1);
    /* TMR3_CH3 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_0, GPIO_AF_PIN1);
    /* TMR3_CH4 */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_1, GPIO_AF_PIN1);

    /* Config TMR3 GPIO for output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    /* config TMR3 */
    TMR_TimeBaseStruct.clockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStruct.counterMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.div = 0;
    TMR_TimeBaseStruct.period = 0xFFFF;
    TMR_ConfigTimeBase(TMR3, &TMR_TimeBaseStruct);

    /* Configure channel1 */
    TMR_OCConfigStructInit(&OCcongigStruct);

    OCcongigStruct.OC_Mode = TMR_OC_MODE_TOGGEL;
    OCcongigStruct.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity = TMR_OC_POLARITY_LOW;

    OCcongigStruct.Pulse = (SystemCoreClock / 5000);
    TMR_OC1Config(TMR3, &OCcongigStruct);
    TMR_OC1PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel2 */
    OCcongigStruct.Pulse = (SystemCoreClock / 10000);
    TMR_OC2Config(TMR3, &OCcongigStruct);
    TMR_OC2PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel3 */
    OCcongigStruct.Pulse = (SystemCoreClock / 50000);
    TMR_OC3Config(TMR3, &OCcongigStruct);
    TMR_OC3PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* Configure channel4 */
    OCcongigStruct.Pulse = (SystemCoreClock / 100000);
    TMR_OC4Config(TMR3, &OCcongigStruct);
    TMR_OC4PreloadConfig(TMR3, TMR_OC_PRELOAD_DISABLE);

    /* Timer3 Commutation Interrupt */
    TMR_EnableInterrupt(TMR3, TMR_INT_CH1 | TMR_INT_CH2 | TMR_INT_CH3 | TMR_INT_CH4);
    NVIC_EnableIRQ(TMR3_IRQn);

    TMR_Enable(TMR3);

    while (1)
    {
    }
}

/*!
 * @brief       TMR3 Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR3_IRQHandler(void)
{
    uint16_t capValue = 0;

    /* TMR3_CH1 toggling with frequency = 5KHz */
    if (TMR_ReadIntFlag(TMR3, TMR_INT_FLAG_CH1))
    {
        /* Clear the TMR3 CH1 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CH1);
        capValue = TMR_ReadCaputer1(TMR3);
        TMR_SetCompare1(TMR3, capValue + (SystemCoreClock / 5000));
    }

    /* TMR3_CH2 toggling with frequency = 50KHz */
    if (TMR_ReadIntFlag(TMR3, TMR_INT_FLAG_CH2))
    {
        /* Clear the TMR3 CH2 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CH2);
        capValue = TMR_ReadCaputer2(TMR3);
        TMR_SetCompare2(TMR3, capValue + (SystemCoreClock / 10000));
    }
    /* TMR3_CH3 toggling with frequency  */
    if (TMR_ReadIntFlag(TMR3, TMR_INT_FLAG_CH3))
    {
        /* Clear the TMR3 CH3 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CH3);
        capValue = TMR_ReadCaputer3(TMR3);
        TMR_SetCompare3(TMR3, capValue + (SystemCoreClock / 50000));
    }

    /* TMR3_CH4 toggling with frequency  */
    if (TMR_ReadIntFlag(TMR3, TMR_INT_FLAG_CH4))
    {
        /* Clear the TMR3 CH4 pending bit */
        TMR_ClearIntFlag(TMR3, TMR_INT_CH4);
        capValue = TMR_ReadCaputer4(TMR3);
        TMR_SetCompare4(TMR3, capValue + (SystemCoreClock / 100000));
    }
}

/**@} end of group TMR_OCToggle_Functions */
/**@} end of group TMR_OCToggle */
/**@} end of group Examples */
