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
#include "apm32f0xx_rcm.h"
#include "apm32f0xx_pmu.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup PMU_Monitor
  @{
  */

/** @defgroup PMU_Monitor_Functions Functions
  @{
  */

void PMU_Monitor_Initialize(void);

/*!
 * @brief main program
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

    APM_MINI_LEDOn(LED2);
    APM_MINI_LEDOff(LED3);

    /* PMU and PVD initialize */
    PMU_Monitor_Initialize();

    for (;;)
    {
    }
}

/*!
 * @brief PMU initialize
 *
 * @param None
 *
 * @retval None
 */
void PMU_Monitor_Initialize(void)
{
    EINT_Config_T eintConfig;

    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    EINT_Reset();

    /* Initialize EINT16 for PVD interrupt */
    eintConfig.mode = EINT_MODE_INTERRUPT;
    eintConfig.line = EINT_LINE16;
    eintConfig.trigger = EINT_TRIGGER_RISING;
    eintConfig.lineCmd = ENABLE;
    EINT_Config(&eintConfig);

    /* Start PVD */
    PMU_EnablePVD();
    /* Configure PVD's threshold value to 2.20V */
    PMU_ConfigPVDLevel(PMU_PVDLEVEL_0);

    /* Enable PVD's interrupt */
    NVIC_EnableIRQRequest(PVD_VDDIO2_IRQn, 0x01);
}

/*!
 * @brief This function handles PVD Handler
 *
 * @param None
 *
 * @retval None
 */
void PVD_VDDIO2_IRQHandler(void)
{
    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOn(LED3);

    /* Clear PVD interrupt flag */
    PMU_ClearStatusFlag(PMU_FLAG_PVDOF);
}

/**@} end of group PMU_Monitor_Functions */
/**@} end of group PMU_Monitor */
/**@} end of group Examples */
