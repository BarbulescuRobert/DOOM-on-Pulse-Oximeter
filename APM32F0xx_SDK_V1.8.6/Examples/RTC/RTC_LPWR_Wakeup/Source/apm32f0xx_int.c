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
#include "apm32f0xx_int.h"
#include "apm32f0xx_eint.h"
#include "main.h"


/** @addtogroup Examples
  @{
*/

/** @addtogroup RTC_LPWR_Wakeup
  @{
*/

/** @defgroup RTC_LPWR_Wakeup_INT_Macros INT_Macros
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_INT_Macros */

/** @defgroup RTC_LPWR_Wakeup_INT_Enumerations INT_Enumerations
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_INT_Enumerations */

/** @defgroup RTC_LPWR_Wakeup_INT_Structures INT_Structures
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_INT_Structures */

/** @defgroup RTC_LPWR_Wakeup_INT_Variables INT_Variables
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_INT_Variables */

/** @defgroup RTC_LPWR_Wakeup_INT_Functions INT_Functions
  @{
*/

#ifndef STANDBY_MODE
    extern void rtc_isr(void);
#endif

extern void eint_isr(void);

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
}

/*!
 * @brief        This function handles RTC Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void RTC_IRQHandler(void)
{
#ifndef STANDBY_MODE
    rtc_isr();
#endif
}

/**@} end of group RTC_LPWR_Wakeup_INT_Functions */
/**@} end of group RTC_LPWR_Wakeup */
/**@} end of group Examples */
