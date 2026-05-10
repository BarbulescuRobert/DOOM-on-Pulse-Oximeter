/*!
 * @file        delay.h
 *
 * @brief       This file contains external declarations of the led.c file.
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

/* Define to prevent recursive inclusion */
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f0xx.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_rcm.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup TSC_KeyLinearRotate
  @{
  */

/** @addtogroup LED_Driver
  @{
  */

/** @defgroup LED_Macros Macros
  @{
  */

#define LEDn            4

#define LED_PORT        GPIOC
#define LED1_PIN        GPIO_PIN_0
#define LED2_PIN        GPIO_PIN_1
#define LED3_PIN        GPIO_PIN_2
#define LED4_PIN        GPIO_PIN_3
#define LED_ALLPIN      GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3

/**@} end of group LED_Macros */

/** @defgroup LED_Enumerations Enumerations
  @{
  */

typedef enum
{
    LED1 = 0,
    LED2 = 1,
    LED3 = 2,
    LED4 = 3
} Led_TypeDef;

/**@} end of group LED_Enumerations */

/** @defgroup LED_Structures Structures
  @{
  */

/**@} end of group LED_Structures */

/** @defgroup LED_Variables Variables
  @{
  */

/**@} end of group LED_Variables */

/** @defgroup LED_Functions Functions
  @{
  */

void LED_Config(void);
void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);

void LED_Action(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */

/**@} end of group LED_Functions */
/**@} end of group LED_Driver */
/**@} end of group TSC_KeyLinearRotate */
/**@} end of group Examples */
