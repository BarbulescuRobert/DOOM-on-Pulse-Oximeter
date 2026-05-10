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
#include "main.h"
#include "board.h"
#include "apm32f0xx_dac.h"
#include "apm32f0xx_adc.h"
#include "apm32f0xx_misc.h"
#include "stdio.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup DAC_NoiseWave
  @{
  */

/** @defgroup DAC_NoiseWave_Macros Macros
  @{
  */

/**@} end of group DAC_NoiseWave_Macros */

/** @defgroup DAC_NoiseWave_Enumerations Enumerations
  @{
  */

/**@} end of group DAC_NoiseWave_Enumerations */

/** @defgroup DAC_NoiseWave_Structures Structures
  @{
  */

/**@} end of group DAC_NoiseWave_Structures */

/** @defgroup DAC_NoiseWave_Variables Variables
  @{
  */

/**@} end of group DAC_NoiseWave_Variables */

/** @defgroup DAC_NoiseWave_Functions Functions
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
    DAC_Config_T    DAC_ConfigStruct;
    GPIO_Config_T   gpioConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* DAC out PA4 pin configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NO;
    gpioConfig.pin     = GPIO_PIN_4;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable DAC clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_DAC);

    DAC_ConfigStruct.trigger = DAC_TRIGGER_SOFTWARE;
    DAC_ConfigStruct.waveGeneration = DAC_WAVE_GENERATION_NOISE;
    DAC_ConfigStruct.maskAmplitudeSelect = DAC_LFSRUNAMASK_BITS5_0;
    DAC_ConfigStruct.outputBuff = DAC_OUTPUTBUFF_ENABLE;
    DAC_Config((uint32_t)DAC_CHANNEL_1, &DAC_ConfigStruct);

    DAC_Enable(DAC_CHANNEL_1);

    DAC_ConfigChannel1Data(DAC_ALIGN_12B_L, 0XFFF0);

    while (1)
    {
        DAC_EnableSoftwareTrigger(DAC_CHANNEL_1);
    }
}

/**@} end of group DAC_NoiseWave_Functions */
/**@} end of group DAC_NoiseWave */
/**@} end of group Examples */
