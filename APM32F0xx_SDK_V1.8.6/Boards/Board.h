/*!
 * @file        Board.h
 *
 * @brief       Header file for Board
 *
 * @version     V1.0.3
 *
 * @date        2025-07-07
 *
 * @attention
 *
 *  Copyright (C) 2020-2025 Geehy Semiconductor
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
#ifndef __BOARD_H
#define __BOARD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#ifdef BOARD_APM32F030_MINI
#include "Board_APM32F030_MINI/inc/Board_APM32F030_MINI.h"
#endif

#ifdef BOARD_APM32F051_MINI
#include "Board_APM32F051_MINI/inc/Board_APM32F051_MINI.h"
#endif

#ifdef BOARD_APM32F072_MINI
#include "Board_APM32F072_MINI/inc/Board_APM32F072_MINI.h"
#endif

#ifdef BOARD_APM32F091_MINI
#include "Board_APM32F091_MINI/inc/Board_APM32F091_MINI.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H */
