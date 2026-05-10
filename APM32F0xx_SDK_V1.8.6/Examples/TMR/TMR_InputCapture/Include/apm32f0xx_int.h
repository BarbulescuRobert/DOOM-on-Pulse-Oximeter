/*!
 * @file        apm32f0xx_int.h
 *
 * @brief       This file contains the headers of the interrupt handlers
 *
 * @version     V1.0.3
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

/* Define to prevent recursive inclusion */
#ifndef __APM32F0XX_INT_H
#define __APM32F0XX_INT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f0xx.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_InputCapture
  @{
*/

/** @defgroup TMR_InputCapture_INT_Macros INT_Macros
  @{
*/

/**@} end of group TMR_InputCapture_INT_Macros */

/** @defgroup TMR_InputCapture_INT_Enumerations INT_Enumerations
  @{
*/

/**@} end of group TMR_InputCapture_INT_Enumerations */

/** @defgroup TMR_InputCapture_INT_Structures INT_Structures
  @{
*/

/**@} end of group TMR_InputCapture_INT_Structures */

/** @defgroup TMR_InputCapture_INT_Variables INT_Variables
  @{
*/

/**@} end of group TMR_InputCapture_INT_Variables */

/** @defgroup TMR_InputCapture_INT_Functions INT_Functions
  @{
*/

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /*__APM32F0XX_INT_H */

/**@} end of group TMR_InputCapture_INT_Functions */
/**@} end of group TMR_InputCapture */
/**@} end of group Examples */
