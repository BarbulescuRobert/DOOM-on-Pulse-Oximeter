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

/** @addtogroup TMR_TimeBase
  @{
*/

/** @defgroup TMR_TimeBase_Macros Macros
  @{
*/

/**@} end of group TMR_TimeBase_Macros */

/** @defgroup TMR_TimeBase_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_TimeBase_Enumerations */

/** @defgroup TMR_TimeBase_Structures Structures
  @{
*/

/**@} end of group TMR_TimeBase_Structures */

/** @defgroup TMR_TimeBase_Variables Variables
  @{
*/

/**@} end of group TMR_TimeBase_Variables */

/** @defgroup TMR_TimeBase_Functions Functions
  @{
*/

void TMR14Isr(void);

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group TMR_TimeBase_Functions */
/**@} end of group TMR_TimeBase */
/**@} end of group Examples */
