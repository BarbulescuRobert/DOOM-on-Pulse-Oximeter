/*!
 * @file        main.c
 *
 * @brief       Main program body
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

/* Includes */
#include "Board.h"
#include "stdio.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_rcm.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup RCM_ClockConfig
  @{
  */

/** @defgroup RCM_ClockConfig_Functions Functions
  @{
  */

static void SystemClock_HSE_Init(void);

/*!
 * @brief Main program
 *
 * @param None
 *
 * @retval None
 */
int main(void)
{
    /* LED initialize */
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    /* COM1 initialize */
    APM_MINI_COMInit(COM1);

    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOff(LED3);

    /* Try to start HSE and set it as system clock */
    SystemClock_HSE_Init();

    for (;;)
    {
    }
}

/*!
 * @brief HSE as System Clock
 *
 * @param None
 *
 * @retval None
 */
void SystemClock_HSE_Init(void)
{
    /* Open HSE */
    RCM_ConfigHSE(RCM_HSE_OPEN);

    /* Wait until HSE is ready */
    if (RCM_WaitHSEReady() == ERROR)
    {
        /* Open HSI48 */
        RCM_EnableHSI48();
        /* select HSI48 as SYSCLK */
        RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSI48);

        APM_MINI_LEDOff(LED2);
        APM_MINI_LEDOn(LED3);
    }
    else
    {
        /* Open CCS */
        RCM_EnableCCS();

        /* select HSE as SYSCLK */
        RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSE);

        APM_MINI_LEDOff(LED3);
        APM_MINI_LEDOn(LED2);
    }
}

/*!
 * @brief This function handles NMI exception
 *
 * @param None
 *
 * @retval None
 */
void NMI_Handler(void)
{
    /* Clear CSS flag */
    RCM_ClearIntFlag(RCM_INT_CSS);

    /* Switch to the 48Hz HSI */
    RCM_EnableHSI48();

    /* Switch system clock to the 48Hz HSI */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSI48);

    APM_MINI_LEDOn(LED3);
    APM_MINI_LEDOn(LED2);
}

/**@} end of group RCM_ClockConfig_Functions */
/**@} end of group RCM_ClockConfig */
/**@} end of group Examples */
