/*!
 * @file        delay.h
 *
 * @brief       This file contains external declarations of the delay.c file.
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

/* Define to prevent recursive inclusion */
#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f0xx.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup TSC_KeyLinearRotate
  @{
  */

/** @addtogroup DELAY_Driver
  @{
  */

/** @defgroup DELAY_Macros Macros
  @{
  */

/**@} end of group DELAY_Macros */

/** @defgroup DELAY_Enumerations Enumerations
  @{
  */

/**@} end of group DELAY_Enumerations */

/** @defgroup DELAY_Structures Structures
  @{
  */

/**@} end of group DELAY_Structures */

/** @defgroup DELAY_Variables Variables
  @{
  */

/**@} end of group DELAY_Variables */

/** @defgroup DELAY_Functions Functions
  @{
  */

void Delay_Config(void);
void Delay_us(int32_t us);
void Delay_ms(int16_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __DELAY_H */

/**@} end of group DELAY_Functions */
/**@} end of group DELAY_Driver */
/**@} end of group TSC_KeyLinearRotate */
/**@} end of group Examples */
