/*!
 * @file        main.h
 *
 * @brief       Main program body
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
#ifndef __MAIN_H
#define __MAIN_H

/* Includes */
#include "Board.h"
#include "apm32f0xx.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @defgroup IAP_BootLoader_Macros Macros
  @{
  */

/**@} end of group IAP_BootLoader_Macros */

/** @defgroup IAP_BootLoader_Enumerations Enumerations
  @{
  */

/**@} end of group IAP_BootLoader_Enumerations */

/** @defgroup IAP_BootLoader_Structures Structures
  @{
  */

/**@} end of group IAP_BootLoader_Structures */

/** @defgroup IAP_BootLoader_Variables Variables
  @{
  */

/**@} end of group IAP_BootLoader_Variables */

/** @defgroup IAP_BootLoader_Functions Functions
  @{
  */

void Delay(void);
extern volatile uint32_t tick;

#endif

/**@} end of group IAP_BootLoader_Functions */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
