/*!
 * @file        main.h
 *
 * @brief       Header for main.c module
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"
#include "apm32f0xx_misc.h"
#include "Board.h"
#include "stdio.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USART_Smartcard USART Smartcard
  @{
*/

/** @defgroup USART_Smartcard_Macros
  @{
*/

/**@} end of group USART_Smartcard_Macros */

/** @defgroup USART_Smartcard_Enumerations
  @{
*/

/**@} end of group USART_Smartcard_Enumerations */

/** @defgroup USART_Smartcard_Structures
  @{
*/

/**@} end of group USART_Smartcard_Structures */

/** @defgroup USART_Smartcard_Variables
  @{
*/

extern volatile uint32_t tick;

/**@} end of group USART_Smartcard_Variables */

/** @defgroup USART_Smartcard_Functions
  @{
*/

void SmartcardIRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/**@} end of group USART_Smartcard_Functions */
/**@} end of group USART_Smartcard */
/**@} end of group Examples */
