/*!
 * @file        main.c
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

/* Includes */
#include "main.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_Application1
  @{
  */

/** @defgroup IAP_Application1_Macros Macros
  @{
  */

/**@} end of group IAP_Application1_Macros */

/** @defgroup IAP_Application1_Enumerations Enumerations
  @{
  */

/**@} end of group IAP_Application1_Enumerations */

/** @defgroup IAP_Application1_Structures Structures
  @{
  */

/**@} end of group IAP_Application1_Structures */

/** @defgroup IAP_Application1_Variables Variables
  @{
  */

volatile uint32_t tick = 0;

/**@} end of group IAP_Application1_Variables */

/** @defgroup IAP_Application1_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    SCB->VTOR = FMC_BASE | 0x4000;

    SysTick_Config(SystemCoreClock / 1000);
    APM_MINI_LEDInit(LED2);

    while (1)
    {
        Delay();
        APM_MINI_LEDToggle(LED2);
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;

    while (tick < 500);
}

/**@} end of group IAP_Application1_Functions */
/**@} end of group IAP_Application1 */
/**@} end of group Examples */
