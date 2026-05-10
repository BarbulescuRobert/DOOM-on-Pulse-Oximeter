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
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USART_IrDA USART IrDA
  @{
*/

/** @defgroup USART_IrDA_Macros Macros
  @{
*/

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group USART_IrDA_Macros */

/** @defgroup USART_IrDA_Enumerations Enumerations
  @{
*/

/**@} end of group USART_IrDA_Enumerations */

/** @defgroup USART_IrDA_Structures Structures
  @{
*/

/**@} end of group USART_IrDA_Structures */

/** @defgroup USART_IrDA_Variables Variables
  @{
*/

volatile uint32_t tick = 0;

uint8_t USART2ReceivedData = 0x00;
uint8_t USART2SendData = 0x01;

uint8_t USART3ReceivedData = 0xFF;
uint8_t USART3SendData = 0xFF;

/**@} end of group USART_IrDA_Variables */

/** @defgroup USART_IrDA_Functions Functions
  @{
*/

void IrDAInit(void);

void IrDA_USART2_TxData(uint8_t data);
uint8_t IrDA_USART2_RxData(void);

void IrDA_USART3_TxData(uint8_t data);
uint8_t IrDA_USART3_RxData(void);

void Delay(void);

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

    APM_MINI_COMInit(COM1);

    IrDAInit();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000); /*!< 1ms */

    printf("\r\nThis example provides a basic communication USARTx IrDA receive and transmit mode.\r\n");

    while (1)
    {
        Delay();

        APM_MINI_COMInit(COM1);
        APM_MINI_LEDToggle(LED2);

        IrDA_USART2_TxData(USART2SendData);
        USART3ReceivedData = IrDA_USART3_RxData();
        printf(">> USART2 Send Data : 0x%02X ,USART3 Received Data : 0x%02X. \r\n", USART2SendData, USART3ReceivedData);

        IrDA_USART3_TxData(USART3SendData);
        USART2ReceivedData = IrDA_USART2_RxData();
        printf(">> USART3 Send Data : 0x%02X ,USART2 Received Data : 0x%02X. \r\n", USART3SendData, USART2ReceivedData);

        USART2SendData++;
        USART3SendData--;
    }

}

/*!
 * @brief       USART2 and USART3 IrDA mode initializes functions
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void IrDAInit(void)
{
    GPIO_Config_T GPIO_ConfigStruct;
    USART_Config_T USART_ConfigStruct;

    /* System Clocks Configuration */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA | RCM_AHB_PERIPH_GPIOB);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART3);

    /* Configure the GPIO ports */
    /* USART2 GPIO Configuration
    PA2 ------> USART2_TX
    PA3 ------> USART2_RX
    */
    GPIO_ConfigStruct.pin     = GPIO_PIN_2;
    GPIO_ConfigStruct.mode    = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed   = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.pupd    = GPIO_PUPD_NO;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_3;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_PIN1);

    /* USART3 GPIO Configuration
    PB10 ------> USART3_TX
    PB11 ------> USART3_RX
    */
    GPIO_ConfigStruct.pin      = GPIO_PIN_10;
    GPIO_ConfigStruct.mode     = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype  = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed    = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_NO;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_11;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_PIN4);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_11, GPIO_AF_PIN4);

    /* USARTx configuration */
    /* USARTx configured as follow:
        - BaudRate    = 115200 baud
        - Word Length = USART_WordLength_8b
        - Stop Bit    = USART_StopBits_1
        - Parity      = USART_Parity_No
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    USART_ConfigStruct.mode             = USART_MODE_TX_RX;
    USART_ConfigStruct.parity           = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits         = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength       = USART_WORD_LEN_8B;

    /* Configure the USARTx */
    USART_Config(USART2, &USART_ConfigStruct);
    USART_Config(USART3, &USART_ConfigStruct);

    /* Enable the USARTx */
    USART_Enable(USART2);
    USART_Enable(USART3);

    /* Set the USARTx prescaler */
    USART_ConfigDivider(USART2, 0x01);
    USART_ConfigDivider(USART3, 0x01);

    /* Configure the USARTx IrDA mode */
    USART_ConfigIrDAMode(USART2, USART_IRDA_MODE_NORMAL);
    USART_ConfigIrDAMode(USART3, USART_IRDA_MODE_NORMAL);

    /* Enable the USARTx IrDA mode */
    USART_EnableIrDA(USART2);
    USART_EnableIrDA(USART3);

    USART_ClearStatusFlag(USART2, USART_FLAG_TXC | USART_FLAG_TXBE);
    USART_ClearStatusFlag(USART3, USART_FLAG_TXC | USART_FLAG_TXBE);
}

/*!
 * @brief       USART2 Sends data function in IrDA mode
 *
 * @param       data : The sent data.
 *
 * @retval      None
 *
 * @note
 */
void IrDA_USART2_TxData(uint8_t data)
{
    USART_TxData(USART2, data);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(USART2, USART_FLAG_TXC) == RESET);
}

/*!
 * @brief       USART3 Receive data function in IrDA mode.
 *
 * @param       None
 *
 * @retval      The received data.
 *
 * @note
 */
uint8_t IrDA_USART2_RxData(void)
{
    while (USART_ReadStatusFlag(USART2, USART_FLAG_RXBNE) == RESET);

    return (USART_RxData(USART2));
}

/*!
 * @brief       USART3 Sends data function in IrDA mode.
 *
 * @param       data : The sent data.
 *
 * @retval      None
 *
 * @note
 */
void IrDA_USART3_TxData(uint8_t data)
{
    USART_TxData(USART3, data);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(USART3, USART_FLAG_TXC) == RESET);
}

/*!
 * @brief       USART3 Receive data function in IrDA mode.
 *
 * @param       None
 *
 * @retval      The received data.
 *
 * @note
 */
uint8_t IrDA_USART3_RxData(void)
{
    while (USART_ReadStatusFlag(USART3, USART_FLAG_RXBNE) == RESET);

    return (USART_RxData(USART3));
}

/*!
 * @brief       Delay 1 second
 *
 * @param       None
 *
 * @retval      None
 *
 */
void Delay(void)
{
    tick = 0;

    while (tick < 1000);
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

/**@} end of group USART_IrDA_Functions */
/**@} end of group USART_IrDA */
/**@} end of group Examples */
