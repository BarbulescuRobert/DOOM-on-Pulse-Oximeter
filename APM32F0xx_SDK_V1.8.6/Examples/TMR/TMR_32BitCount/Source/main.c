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

/** @addtogroup TMR_32BitCount
  @{
*/

/** @defgroup TMR_32BitCount_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group TMR_32BitCount_Macros */

/** @defgroup TMR_32BitCount_Variables Variables
  @{
  */

volatile uint32_t tick = 0;

/**@} end of group TMR_32BitCount_Variables */

/** @defgroup TMR_32BitCount_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note        TMR1 as time base for delay,
 *              TMR3 as High 16 bit count value,
 *              TMR4 as Low  16 bit count value.
 *
 */
int main(void)
{
    uint32_t temp;
    TMR_TimeBase_T baseConfig;

    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDOff(LED2);

    APM_MINI_COMInit(COM1);

    /* TMR1,3,15 Clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_TMR3);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1 | RCM_APB2_PERIPH_TMR15);

    /* TMR15 Configure */
    baseConfig.div = 0;
    baseConfig.period = 0xFFFF;
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    TMR_ConfigTimeBase(TMR15, &baseConfig);
    TMR_ConfigDIV(TMR15, 0, TMR_PRESCALER_RELOAD_UPDATA);
    TMR_DisableNGUpdate(TMR15);

    /* Configure TMR15 Input */
    TMR_SelectInputTrigger(TMR15, TMR_TS_ITR1);
    TMR_SelectSlaveMode(TMR15, TMR_SLAVEMODE_EXTERNALL);
    TMR_EnableMasterSlaveMode(TMR15);
    TMR_EnableAUTOReload(TMR15);

    /* TMR3 Configure */
    baseConfig.div = 167;
    baseConfig.period = 0xFFFF;
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    TMR_ConfigTimeBase(TMR3, &baseConfig);
    TMR_EnableAUTOReload(TMR3);

    /* Configure TMR3 Output */
    TMR_SelectSlaveMode(TMR3, TMR_SLAVEMODE_RESET);
    TMR_ConfigUPdateRequest(TMR3, TMR_UPDATE_SOURCE_REGULAR);
    TMR_SelectOutputTrigger(TMR3, TMR_TRGOSOURCE_UPDATE);
    TMR_EnableMasterSlaveMode(TMR3);

    /* TMR1 Configure */
    baseConfig.clockDivision = TMR_CKD_DIV1;
    baseConfig.counterMode = TMR_COUNTER_MODE_UP;
    baseConfig.div = TMR_CKD_DIV1;
    baseConfig.period = 4800;
    TMR_ConfigTimeBase(TMR1, &baseConfig);

    /* Enable TMR1 Interrupt */
    TMR_EnableInterrupt(TMR1, TMR_INT_UPDATE);
    NVIC_EnableIRQRequest(TMR1_BRK_UP_TRG_COM_IRQn, 0);

    /* Enable TMR */
    TMR_Enable(TMR1);
    TMR_Enable(TMR3);
    TMR_Enable(TMR15);

    while (1)
    {
        temp = TMR15->CNT << 16;
        temp |= TMR3->CNT;
        printf(" Count Value: 0x%08x\r\n", temp);
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
 *
 */
void Delay(void)
{
    tick = 0;

    while (tick < 500);
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

/**@} end of group TMR_32BitCount_Functions */
/**@} end of group TMR_32BitCount */
/**@} end of group Examples */
