/*!
 * @file        apm32f0xx_int.c
 *
 * @brief       Main Interrupt Service Routines
 *
 * @version     V1.0.2
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

/** @addtogroup IWDT_FeedDog
  @{
*/

/** @defgroup IWDT_FeedDog_INT_Macros INT_Macros
  @{
  */

/**@} end of group IWDT_FeedDog_INT_Macros */

/** @defgroup IWDT_FeedDog_INT_Enumerations INT_Enumerations
  @{
  */

/**@} end of group IWDT_FeedDog_INT_Enumerations */

/** @defgroup IWDT_FeedDog_INT_Structures INT_Structures
  @{
  */

/**@} end of group IWDT_FeedDog_INT_Structures */

/** @defgroup IWDT_FeedDog_INT_Variables INT_Variables
  @{
  */

/**@} end of group IWDT_FeedDog_INT_Variables */

/** @defgroup IWDT_FeedDog_INT_Functions
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
}

/**@} end of group IWDT_FeedDog_INT_Functions */
/**@} end of group IWDT_FeedDog */
/**@} end of group Examples */
