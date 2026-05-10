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

/** @addtogroup IAP_Application2
  @{
  */

/** @defgroup IAP_Application2_Macros Macros
  @{
  */

/**@} end of group IAP_Application2_Macros */

/** @defgroup IAP_Application2_Enumerations Enumerations
  @{
  */

/**@} end of group IAP_Application2_Enumerations */

/** @defgroup IAP_Application2_Structures Structures
  @{
  */

/**@} end of group IAP_Application2_Structures */

/** @defgroup IAP_Application2_Variables Variables
  @{
  */

volatile uint32_t tick = 0;

/**@} end of group IAP_Application2_Variables */

/** @defgroup IAP_Application2_Functions Functions
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
    SCB->VTOR = FMC_BASE | 0x6000;

    SysTick_Config(SystemCoreClock / 1000);
    APM_MINI_LEDInit(LED3);

    while (1)
    {
        Delay();
        APM_MINI_LEDToggle(LED3);
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

/**@} end of group IAP_Application2_Functions */
/**@} end of group IAP_Application2 */
/**@} end of group Examples */
