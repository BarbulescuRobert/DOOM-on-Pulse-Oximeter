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
#include "Board.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_PWMInput
  @{
*/

/** @defgroup TMR_PWMInput_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group TMR_PWMInput_Macros*/

/** @defgroup TMR_PWMInput_Variables Variables
  @{
*/

uint8_t  irqFlag = 0;
uint32_t IC2Value = 0;
uint32_t DutyCycle = 0;
uint32_t Frequency = 0;

volatile uint32_t tick = 0;

/**@} end of group TMR_PWMInput_Variables */

/** @defgroup TMR_PWMInput_Functions Functions
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
    GPIO_Config_T gpioConfig;
    TMR_ICConfig_T ICConfig;
    TMR_TimeBase_T baseConfig;

    /* Init LED */
    APM_MINI_LEDInit(LED2);

    /* COM1 Init */
    APM_MINI_COMInit(COM1);

    /* Enable Periph Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

#if defined(APM32F030)

    /* TMR3 chennel2 for PA7 */
    gpioConfig.pin = GPIO_PIN_7;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    GPIO_Config(GPIOA, &gpioConfig);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN1);

#endif /* APM32F030 */

#if defined (APM32F072)

    /* TMR3 chennel2 for PE4 */
    gpioConfig.pin = GPIO_PIN_4;
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    GPIO_Config(GPIOE, &gpioConfig);
    GPIO_ConfigPinAF(GPIOE, GPIO_PIN_SOURCE_4, GPIO_AF_PIN0);

#endif /* APM32F072 */

    /* TMR3 PWM Input mode */
    ICConfig.channel = TMR_CHANNEL_2;
    ICConfig.ICpolarity = TMR_IC_POLARITY_RISING;
    ICConfig.ICselection = TMR_IC_SELECTION_DIRECT_TI;
    ICConfig.ICprescaler = TMR_ICPSC_DIV1;
    ICConfig.ICfilter = 0x00;
    TMR_PWMConfig(TMR3, &ICConfig);

    /* TMR3 Slave Mode */
    TMR_SelectInputTrigger(TMR3, TMR_TS_TI2FP2);
    TMR_SelectSlaveMode(TMR3, TMR_SLAVEMODE_RESET);
    TMR_EnableMasterSlaveMode(TMR3);

    /* Enable TMR3 Interrupt */
    TMR_Enable(TMR3);
    NVIC_EnableIRQRequest(TMR3_IRQn, 0);
    TMR_EnableInterrupt(TMR3, TMR_INT_CH2);

    /* Config TMR1 */
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    baseConfig.div = 167;
    baseConfig.period = 999;
    baseConfig.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &baseConfig);

    /* Enable TMR1 Interrupt */
    TMR_Enable(TMR1);
    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_BRK_UP_TRG_COM_IRQn, 0);

    while (1)
    {
        if (irqFlag > 1)
        {
            printf("DutyCycle: %d %%, Frequency: %d Hz\r\n", DutyCycle, Frequency);
            APM_MINI_LEDToggle(LED2);
            irqFlag = 0;
            Delay();
        }
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

/**@} end of group TMR_PWMInput_Functions */
/**@} end of group TMR_PWMInput */
/**@} end of group Examples */
