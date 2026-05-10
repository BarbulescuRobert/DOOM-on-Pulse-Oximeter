/*!
 * @file        apm32f0xx_int.c
 *
 * @brief       Main Interrupt Service Routines
 *
 * @version     V1.0.0
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
#include "apm32f0xx_int.h"
#include "apm32f0xx_tmr.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_6Steps
  @{
*/

/** @defgroup TMR_6Steps_INT_Macros INT_Macros
  @{
*/

/**@} end of group TMR_6Steps_INT_Macros */

/** @defgroup TMR_6Steps_INT_Enumerations INT_Enumerations
  @{
*/

/**@} end of group TMR_6Steps_INT_Enumerations */

/** @defgroup TMR_6Steps_INT_Structures INT_Structures
  @{
*/

/**@} end of group TMR_6Steps_INT_Structures */

/** @defgroup TMR_6Steps_INT_Variables INT_Variables
  @{
*/

/* extern variables */
extern uint32_t cntUs;
extern uint32_t cntMs;

extern uint8_t PWMStep;

/**@} end of group TMR_6Steps_INT_Variables */

/** @defgroup TMR_6Steps_INT_Functions INT_Functions
  @{
*/

/*!
 * @brief        This function handles NMI exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void NMI_Handler(void)
{
}

/*!
 * @brief        This function handles Hard Fault exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void HardFault_Handler(void)
{
}

/*!
 * @brief        This function handles SVCall exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief        This function handles PendSV_Handler exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief        This function handles SysTick Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SysTick_Handler(void)
{
    if (cntMs)
    {
        cntMs --;
    }

    if (cntUs)
    {
        cntUs --;
    }
}

/*!
 * @brief       TMR1 Trigger and Commutation IRQ Handler
 *
 * @param       None
 *
 * @retval      None
 */
void TMR1_BRK_UP_TRG_COM_IRQHandler(void)
{
    /* Clear the TMR1 COM pending bit */
    TMR_ClearIntFlag(TMR1, TMR_INT_CCU);

    PWMStep++;

    switch (PWMStep)
    {
        case 1:
            /* configuration: Channel1 and Channel2 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_3);
            break;

        case 2:
            /* configuration: Channel2 and Channel3 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_3);
            break;

        case 3:
            /* configuration: Channel3 and Channel1 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_3);
            break;

        case 4:
            /* configuration: Channel1 and Channel2 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_3);
            break;

        case 5:
            /* configuration: Channel2 and Channel3 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_2, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_3);
            break;

        case 6:
            /* configuration: Channel3 and Channel1 PWM1 Mode */
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_3, TMR_OC_MODE_PWM1);
            TMR_SelectOCxMode(TMR1, TMR_CHANNEL_1, TMR_OC_MODE_PWM1);

            /* configuration: Channel1 */
            TMR_EnableCCxChannel(TMR1, TMR_CHANNEL_1);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_1);

            /* configuration: Channel2 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_2);
            TMR_DisableCCxNChannel(TMR1, TMR_CHANNEL_2);

            /* configuration: Channel3 */
            TMR_DisableCCxChannel(TMR1, TMR_CHANNEL_3);
            TMR_EnableCCxNChannel(TMR1, TMR_CHANNEL_3);

            PWMStep = 0;
            break;

        default:
            PWMStep = 0;
            break;
    }
}
/**@} end of group TMR_6Steps_INT_Functions */
/**@} end of group TMR_6Steps */
/**@} end of group Examples */
