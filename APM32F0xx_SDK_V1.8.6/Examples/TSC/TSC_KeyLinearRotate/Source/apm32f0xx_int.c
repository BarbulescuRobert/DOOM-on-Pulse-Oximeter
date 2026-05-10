/*!
 * @file        apm32f0xx_int.c
 *
 * @brief       Main Interrupt Service Routines
 *
 * @version     V1.0.1
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
#include "main.h"

/** @addtogroup Examples
 @{
 */

/** @addtogroup TSC_KeyLinearRotate
  @{
  */

/** @addtogroup TSC_KeyLinearRotate_INT
 @{
 */

/** @defgroup TSC_KeyLinearRotate_INT_Macros INT_Macros
  @{
  */

/**@} end of group TSC_KeyLinearRotate_INT_Macros */

/** @defgroup TSC_KeyLinearRotate_INT_Enumerations INT_Enumerations
  @{
  */

/**@} end of group TSC_KeyLinearRotate_INT_Enumerations */

/** @defgroup TSC_KeyLinearRotate_INT_Structures INT_Structures
  @{
  */

/**@} end of group TSC_KeyLinearRotate_INT_Structures */

/** @defgroup TSC_KeyLinearRotate_INT_Variables Variables
 @{
 */

extern __IO uint32_t Global_SystickCounter;
extern __IO uint32_t Global_EOA;

/**@} end of group TSC_KeyLinearRotate_INT_Variables */

/** @defgroup TSC_KeyLinearRotate_INT_Functions Functions
 @{
 */

/*!
 * @brief       This function handles NMI exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void NMI_Handler(void)
{
}

/*!
 * @brief       This function handles Hard Fault exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void HardFault_Handler(void)
{
}

/*!
 * @brief       This function handles SVCall exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief       This function handles PendSV_Handler exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief       This function handles SysTick exception
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SysTick_Handler(void)
{
    /* Global variables used by the Systick_Delay() routine (in main.c) */
    if (Global_SystickCounter != 0)
    {
        Global_SystickCounter--;
    }
    /* TSC timing for ECS, DTO, ... */
    TSC_Time_ProcessInterrupt();
}

/*!
 * @brief       This function handles Touch Sensing Controller interrupt requests.
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void TSC_IRQHandler(void)
{
#if TOUCH_TSC_IODEF > 0
    /* Set IO default in Output PP Low to discharge all capacitors */
    TSC->CTRL &= (uint32_t)(~(1 << 4));
#endif
    /* Clear EOAICLR and MCEICLR flags */
    TSC->INTFCLR |= 0x03;
    /* To inform the main loop routine of the End Of Acquisition */
    Global_EOA = 1;
}

/**@} end of group TSC_KeyLinearRotate_INT_Functions */
/**@} end of group TSC_KeyLinearRotate_INT */
/**@} end of group TSC_KeyLinearRotate */
/**@} end of group Examples */
