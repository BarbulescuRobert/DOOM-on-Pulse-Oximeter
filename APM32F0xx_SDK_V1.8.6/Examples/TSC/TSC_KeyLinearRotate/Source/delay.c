/*!
 * @file        delay.c
 *
 * @brief       This file contains the Delay Driver main functions.
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

/* Includes */
#include "delay.h"

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

static int8_t  fac_us = 0;
static int16_t fac_ms = 0;

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 */
void Delay_Config()
{
    SysTick_ConfigCLKSource(SysTick_CLKSource_HCLK_Div8);
    fac_us = SystemCoreClock / 8000000;
    fac_ms = (int16_t)fac_us * 1000;
}

/*!
 * @brief       Delay us
 *
 * @param       us: microsecond
 *
 * @retval      None
 */
void Delay_us(int32_t us)
{
    int32_t temp;
    SysTick->LOAD = us * fac_us;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    do
    {
        temp = SysTick->CTRL;
    }
    while (temp & 0x01 && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

/*!
 * @brief       Delay ms
 *
 * @param       ms: millisecond
 *
 * @retval      None
 */
void Delay_ms(int16_t ms)
{
    int32_t temp;
    SysTick->LOAD = (int32_t)ms * fac_ms;
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;

    do
    {
        temp = SysTick->CTRL;
    }
    while (temp & 0x01 && !(temp & (1 << 16)));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->VAL = 0x00;
}

/**@} end of group DELAY_Functions */
/**@} end of group DELAY_Driver */
/**@} end of group TSC_KeyLinearRotate */
/**@} end of group Examples */
