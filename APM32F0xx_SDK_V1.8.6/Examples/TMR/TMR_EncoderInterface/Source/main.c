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
#include "stdio.h"
#include "Board.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_tmr.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_EncoderInterface
  @{
*/

/** @addtogroup TMR_EncoderInterface_Macros Macros
  @{
*/

/* printf function configs to USART2 */
#define DEBUG_USART  USART2

/**@} end of group TMR_EncoderInterface_Macros*/

/** @defgroup TMR_EncoderInterface_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_EncoderInterface_Enumerations */

/** @defgroup TMR_EncoderInterface_Structures Structures
  @{
*/

/**@} end of group TMR_EncoderInterface_Structures */

/** @defgroup TMR_EncoderInterface_Variables Variables
  @{
*/

uint16_t encoderNum = 0;
volatile uint32_t tick = 0;

/**@} end of group TMR_EncoderInterface_Variables */

/** @defgroup TMR_EncoderInterface_Functions Functions
  @{
*/

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    GPIO_Config_T    gpioConfig;
    TMR_TimeBase_T baseConfig;
    TMR_ICConfig_T   ICConfig;

    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDOff(LED2);

    APM_MINI_COMInit(COM2);

    /* Enable GPIOA Periph Clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* Config GPIO */
    gpioConfig.pin = GPIO_PIN_8 | GPIO_PIN_9;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_OD;
    gpioConfig.pupd = GPIO_PUPD_PU;
    GPIO_Config(GPIOA, &gpioConfig);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_PIN2);

    /* Enable TMR1,TMR3 Periph Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);

    /* Config TMR1 */
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    baseConfig.div = 71;
    baseConfig.period = 0x0008;
    baseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &baseConfig);

    /* Config TMR3 */
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    baseConfig.div = 48;
    baseConfig.period = 999;
    baseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR3, &baseConfig);

    /* Encoder */
    TMR_ConfigEncodeInterface(TMR1, TMR_ENCODER_MODE_TI1, TMR_IC_POLARITY_RISING, TMR_IC_POLARITY_RISING);

    /* Input Capture */
    ICConfig.channel = TMR_CHANNEL_1;
    ICConfig.ICpolarity = TMR_IC_POLARITY_RISING;
    ICConfig.ICselection = TMR_IC_SELECTION_DIRECT_TI;
    ICConfig.ICprescaler = TMR_ICPSC_DIV1;
    ICConfig.ICfilter = 0;
    TMR_ICConfig(TMR1, &ICConfig);

    ICConfig.channel = TMR_CHANNEL_2;
    TMR_ICConfig(TMR1, &ICConfig);

    /* Counter */
    TMR_SetCounter(TMR1, 0x0004);
    TMR_ClearStatusFlag(TMR1, TMR_FLAG_UPDATE);
    TMR_ClearStatusFlag(TMR3, TMR_FLAG_UPDATE);

    /* Enable TMR1,TMR3 Interrupt */
    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    TMR_EnableInterrupt(TMR3, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_BRK_UP_TRG_COM_IRQn, 0);
    NVIC_EnableIRQRequest(TMR3_IRQn, 0);

    /* Enable TMR1,TMR3 */
    TMR_Enable(TMR1);
    TMR_Enable(TMR3);

    while (1)
    {
        APM_MINI_LEDToggle(LED2);
        Delay();
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 */
void Delay(void)
{
    tick = 0;
    while (tick < 500)
    {
    }
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group TMR_EncoderInterface_Functions */
/**@} end of group TMR_EncoderInterface */
/**@} end of group Examples */
