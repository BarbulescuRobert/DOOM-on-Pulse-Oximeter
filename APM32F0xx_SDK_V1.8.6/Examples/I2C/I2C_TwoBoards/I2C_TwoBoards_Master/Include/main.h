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

/** @addtogroup I2C_TwoBoards_Master
  @{
  */

/** @defgroup I2C_TwoBoards_Master_Macros Macros
  @{
  */

/**@} end of group I2C_TwoBoards_Master_Macros */

/** @defgroup I2C_TwoBoards_Master_Enumerations Enumerations
  @{
  */

/**@} end of group I2C_TwoBoards_Master_Enumerations */

/** @defgroup I2C_TwoBoards_Master_Structures Structures
  @{
  */

/**@} end of group I2C_TwoBoards_Master_Structures */

/** @defgroup I2C_TwoBoards_Master_Variables Variables
  @{
  */

/**@} end of group I2C_TwoBoards_Master_Variables */

/** @defgroup I2C_TwoBoards_Master_Functions Functions
  @{
  */

void I2C_Isr(void);
void APM_MINI_PB_I2C_Isr(void);

#ifdef __cplusplus
}
#endif

#endif

/**@} end of group I2C_TwoBoards_Master_Functions */
/**@} end of group I2C_TwoBoards_Master */
/**@} end of group Examples */
