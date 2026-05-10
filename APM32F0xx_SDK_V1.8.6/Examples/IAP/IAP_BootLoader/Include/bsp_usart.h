/*!
 * @file        bsp_usart.h
 *
 * @brief       Header for bsp_usart.c module
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

/* Define to prevent recursive inclusion */
#ifndef __BSP_USART_H
#define __BSP_USART_H

/* Includes */
#include "Board.h"
#include "apm32f0xx.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @addtogroup BSP_USART
  @{
  */

/** @defgroup BSP_USART_Macros Macros
  @{
  */

#define DEBUG_USART                         USART1
#define DEBUG_USART_CLK                     RCM_APB2_PERIPH_USART1
#define DEBUG_USART_TX_CLK                  RCM_AHB_PERIPH_GPIOA
#define DEBUG_USART_TX_PORT                 GPIOA
#define DEUBG_USART_TX_SOURCE               GPIO_PIN_SOURCE_9
#define DEBUG_USART_TX_PIN                  GPIO_PIN_9
#define DEBUG_USART_RX_CLK                  RCM_AHB_PERIPH_GPIOA
#define DEBUG_USART_RX_PORT                 GPIOA
#define DEUBG_USART_RX_SOURCE               GPIO_PIN_SOURCE_10
#define DEBUG_USART_RX_PIN                  GPIO_PIN_10
#define DEBUG_USART_BAUDRATE                115200

/**@} end of group BSP_USART_Macros */

/** @defgroup BSP_USART_Enumerations Enumerations
  @{
  */


/**@} end of group BSP_USART_Enumerations */

/** @defgroup BSP_USART_Structures Structures
  @{
  */

/**@} end of group BSP_USART_Structures */

/** @defgroup BSP_USART_Variables Variables
  @{
  */

/**@} end of group BSP_USART_Variables */

/** @defgroup BSP_USART_Functions Functions
  @{
  */

/* function declaration*/
void USART_Init(uint32_t baudRate);

/**@} end of group BSP_USART_Functions */
/**@} end of group BSP_USART */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */

#endif
