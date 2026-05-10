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

/** @addtogroup PMU_WakeUp
  @{
  */

/** @defgroup PMU_WakeUp_Macros Macros
  @{
*/

/**@} end of group PMU_WakeUp_Macros */

/** @defgroup PMU_WakeUp_Enumerations Enumerations
  @{
*/

/**@} end of group PMU_WakeUp_Enumerations */

/** @defgroup PMU_WakeUp_Structures Structures
  @{
*/

/**@} end of group PMU_WakeUp_Structures */

/** @defgroup PMU_WakeUp_Variables Variables
  @{
*/

/**@} end of group PMU_WakeUp_Variables */

/** @defgroup PMU_WakeUp_Functions Functions
  @{
  */

void APM_MINI_PB_PMU_Isr(void);

void EINT_WakeUp_From_Sleep_Isr(void);
void APM_MINI_PMU_Enter_STOP_Mode_Isr(void);
void EINT_WakeUp_From_STOP_Isr(void);

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group PMU_WakeUp_Functions */
/**@} end of group PMU_WakeUp */
/**@} end of group Examples */
