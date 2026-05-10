/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.2
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
#include "apm32f0xx_iwdt.h"
#include "stdio.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup IWDT_FeedDog_Window
  @{
*/

/** @defgroup IWDT_FeedDog_Window_Marcos Marcos
  @{
  */

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group IWDT_FeedDog_Window_Marcos */

/** @defgroup IWDT_FeedDog_Window_Enumerations Enumerations
  @{
  */

/**@} end of group IWDT_FeedDog_Window_Enumerations */

/** @defgroup IWDT_FeedDog_Window_Structures Structures
  @{
  */

/**@} end of group IWDT_FeedDog_Window_Structures */

/** @defgroup IWDT_FeedDog_Window_Variables Variables
  @{
  */

/**@} end of group IWDT_FeedDog_Window_Variables */

/** @defgroup IWDT_FeedDog_Window_Functions Functions
  @{
  */

/* delay */
void Delay(uint32_t count);
/* IWDT init */
void IWDTInit(void);
/* IWDT Feed */
void IWDT_Feed(void);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    APM_MINI_COMInit(COM1);
    IWDTInit();

    printf("System Reset\r\n");

    while (1)
    {
        printf("beyond window period\r\n");
        Delay(2000000);
        /* System will not be reset if you press the BUTTON_KEY1 within window period */
        printf("window period\r\n");

        while (1)
        {
            Delay(500000);
            printf("running\r\n");
        }
    }
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
.
 * @retval      None
 *
 * @note
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;

    while (delay--);
}

/*!
 * @brief     Initialize the IWDT
 *
 * @param     None
 *
 * @retval    None
 *
 * @note
 */
void IWDTInit(void)
{
    if (RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) != RESET)
    {
        RCM_ClearStatusFlag();
    }

    IWDT_EnableWriteAccess();

    IWDT_ConfigDivider(IWDT_DIV_64);

    IWDT_ConfigReload(80000 / 32);

    /* Wait until CNTU Flag is reset */
    while (IWDT_ReadStatusFlag(IWDT_FLAG_CNTU));

    /* set IWDT Window Value */
    IWDT_ConfigWindowValue(40000 / 32);

    IWDT_Enable();
}

/*!
 * @brief   Feed dog and the system will not be reset.
 *
 * @param   None
 *
 * @retval  None
 *
 * @note
 */
void IWDT_Feed(void)
{
    IWDT_Refresh();
}

/*!
 * @brief   Feed dog if press the button_key_1
 *
 * @param   None
 *
 * @retval  None
 *
 * @note    this function need to put into
            EINT0_1_IRQHandler() in apm32f0xx_it.c
 */
void IWDT_Feed_Isr(void)
{
    IWDT_Feed();
    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOff(LED3);
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

/**@} end of group IWDT_FeedDog_Window_Functions */
/**@} end of group IWDT_FeedDog_Window */
/**@} end of group Examples */
