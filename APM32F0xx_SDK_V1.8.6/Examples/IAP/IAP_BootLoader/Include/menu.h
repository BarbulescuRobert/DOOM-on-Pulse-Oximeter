/*!
 * @file        menu.h
 *
 * @brief       Header for menu.c module
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
#ifndef __MENU_H
#define __MENU_H

/* Includes */
#include "main.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @addtogroup MENU
  @{
  */

/** @defgroup MENU_Macros Macros
  @{
  */

#define MENU_WAIT_TIMEOUT  0xFFFFFF

/**@} end of group MENU_Macros */

/** @defgroup MENU_Enumerations Enumerations
  @{
  */


/**@} end of group MENU_Enumerations */

/** @defgroup MENU_Structures Structures
  @{
  */

/**@} end of group MENU_Structures */

/** @defgroup MENU_Variables Variables
  @{
  */

/* extern variables*/
extern uint8_t FileN[];

/**@} end of group MENU_Variables */

/** @defgroup MENU_Functions Functions
  @{
  */

/* function declaration*/
void Select_Menu(void);

/**@} end of group MENU_Functions */
/**@} end of group MENU */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
#endif
