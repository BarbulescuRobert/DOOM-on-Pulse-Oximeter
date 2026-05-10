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

/** @addtogroup Examples
  @{
  */

/** @addtogroup USART_Interrupt
  @{
  */

/** @defgroup USART_Interrupt_Macros Macros
  @{
*/

/**@} end of group USART_Interrupt_Macros */

/** @defgroup USART_Interrupt_Enumerations Enumerations
  @{
*/

/**@} end of group USART_Interrupt_Enumerations */

/** @defgroup USART_Interrupt_Structures Structures
  @{
*/

/**@} end of group USART_Interrupt_Structures */

/** @defgroup USART_Interrupt_Variables Variables
  @{
*/

/**@} end of group USART_Interrupt_Variables */

/** @defgroup USART_Interrupt_Functions Functions
  @{
  */

void USART_Receive_Isr(void);
void USART_Send_Isr(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/**@} end of group USART_Interrupt_Functions */
/**@} end of group USART_Interrupt */
/**@} end of group Examples */
