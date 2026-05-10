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

/* includes */
#include "apm32f0xx_int.h"
#include "main.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup FreeRTOS
  @{
  */

/** @defgroup FreeRTOS_INT_Macros INT_Macros
  @{
  */

/**@} end of group FreeRTOS_INT_Macros */

/** @defgroup FreeRTOS_INT_Enumerations INT_Enumerations
  @{
  */

/**@} end of group FreeRTOS_INT_Enumerations */

/** @defgroup FreeRTOS_INT_Structures INT_Structures
  @{
  */

/**@} end of group FreeRTOS_INT_Structures */

/** @defgroup FreeRTOS_INT_Variables INT_Variables
  @{
  */

/**@} end of group FreeRTOS_INT_Variables */

/** @defgroup FreeRTOS_INT_Functions INT_Functions
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


/**@} end of group FreeRTOS_INT_Functions */
/**@} end of group FreeRTOS */
/**@} end of group Examples */
