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
#include "main.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USART_Smartcard USART Smartcard
  @{
*/

/** @defgroup USART_Smartcard_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

#define Smartcard_T0_Protocol       0x00    /*!< T0 PROTOCOL */
#define Smartcard_Setup_Lenght      20
#define Smartcard_Hist_Lenght       20
#define Smartcard_Receive_Timeout   0x4000 /*!< direction to reader */

/**@} end of group USART_Smartcard_Macros */

/** @defgroup USART_Smartcard_Enumerations Enumerations
  @{
*/

/**@} end of group USART_Smartcard_Enumerations */

/** @defgroup USART_Smartcard_Structures Structures
  @{
*/

/**
 * @brief    Smartcard ATR structure definition
 */
typedef  struct
{
    uint8_t TS;                           /*!< Bit Convention */
    uint8_t T0;                           /*!< High Nibble = N. of setup byte; low nibble = N. of historical byte */
    uint8_t T[Smartcard_Setup_Lenght];    /*!< Setup array */
    uint8_t H[Smartcard_Hist_Lenght];     /*!< Historical array */
    uint8_t Tlenght;                      /*!< Setup array dimension */
    uint8_t Hlenght;                      /*!< Historical array dimension */
} Smartcard_ATR_T;

/**@} end of group USART_Smartcard_Structures */

/** @defgroup USART_Smartcard_Variables Variables
  @{
*/

Smartcard_ATR_T  Smartcard_A2RStruct;

__IO uint8_t ReceivedBuffer[50] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

volatile uint32_t tick = 0;

/**@} end of group USART_Smartcard_Variables */

/** @defgroup USART_Smartcard_Functions Functions
  @{
*/

void SmartcardInit(void);
uint32_t SmartcardTest(void);
uint8_t Smartcard_decode_Answer2reset(__IO uint8_t* cardBuff);

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
    APM_MINI_LEDInit(LED3);

    APM_MINI_COMInit(COM1);

    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOff(LED3);

    SmartcardInit();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000); /*!< 1ms */

    printf("\r\nThis example provides a basic communication USART2 smartcard mode.\r\n");

    if (SmartcardTest() == SUCCESS)
    {
        APM_MINI_LEDOn(LED3);
        printf(">> Read Smartcard ATR response success.\r\n");
    }
    else
    {
        APM_MINI_LEDOff(LED3);
        printf(">> Read Smartcard ATR response failed.\r\n");
    }

    while (1)
    {
        Delay();

        APM_MINI_LEDToggle(LED2);

    }
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

/*!
 * @brief       USART2 smartcard mode initializes functions
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void SmartcardInit(void)
{
    GPIO_Config_T GPIO_ConfigStruct;
    USART_Config_T USART_ConfigStruct;
    USART_SyncClockConfig_T USART_ClockConfigStruct;

    /* System Clocks Configuration */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);

    /* USART2 GPIO Configuration
    PA2 (USART2_TX) ------> Smartcard Tx Pin
    PA3 (USART2_Rx) ------> Smartcard Reset Pin
    PA4 (USART2_CK) ------> Smartcard CK Pin
    */

    /* Configure Smartcard USART (USART2) Tx as alternate function open-drain */
    GPIO_ConfigStruct.pin = GPIO_PIN_10;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_OD;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_NO;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* Configure Smartcard Reset Pin as open-drain */
    GPIO_ConfigStruct.pin = GPIO_PIN_11;
    GPIO_ConfigStruct.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    /* Configure Smartcard USART (USART2) CK as alternate function push-pull */
    GPIO_ConfigStruct.pin = GPIO_PIN_12;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_4, GPIO_AF_PIN1);

    /* Set Smartcard Reset Pin to High */
    GPIO_SetBit(GPIOA, GPIO_PIN_11);

    /* Smartcard USART (USART2) configuration */
    /* Smartcard USART (USART2) configured as follow:
          - Word Length = 9 Bits
          - 1.5 Stop Bit
          - Even parity
          - BaudRate = 9677 baud
          - Hardware flow control disabled (RTS and CTS signals)
          - Tx and Rx enabled
          - USART Clock enabled
          - USART CPOL Low
          - USART CPHA on first edge
          - USART Last Bit Clock Enabled
    */

    USART_ClockConfigStruct.enable       = USART_CLKEN_ENABLE;
    USART_ClockConfigStruct.polarity     = USART_CLKPOL_LOW;
    USART_ClockConfigStruct.phase        = USART_CLKPHA_1EDGE;
    USART_ClockConfigStruct.lastBitClock = USART_LBCP_ENABLE;

    USART_ConfigSyncClock(USART2, &USART_ClockConfigStruct);

    USART_ConfigStruct.baudRate = 9677;
    USART_ConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_EVEN;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1_5;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_9B;

    USART_Config(USART2, &USART_ConfigStruct);

    /* Smartcard USART (USART2) Clock set to 4.5MHz (PCLK1 = 48 MHZ / 10) */
    USART_ConfigDivider(USART2, 0x06);

    /* Smartcard USART (USART2) Guard Time set to 16 Bit */
    USART_ConfigGuardTime(USART2, 16);

    /* Configure the USART (USART2) Interrupt */
    USART_EnableInterrupt(USART2, USART_INT_PEIE);
    NVIC_EnableIRQRequest(USART2_IRQn, 0);

    /* Enable Smartcard USART (USART2) */
    USART_EnableSmartCard(USART2);

    /* Enable the NACK Transmission */
    USART_EnableSmartCardNACK(USART2);

    /* Enable the USART (USART2) */
    USART_Enable(USART2);
    USART_Enable(USART2);

    /* Clear USART (USART2) TXC Flag */
    USART_ClearStatusFlag(USART2, USART_FLAG_TXC | USART_FLAG_TXBE);
}


/*!
 * @brief       Test the inserted card is ISO7816-3 T=0
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
uint32_t SmartcardTest(void)
{
    uint32_t Counter = 0;

    /* Set Smartcard Reset Pin to Low */
    GPIO_ClearBit(GPIOA, GPIO_PIN_2);

    /* Set Smartcard Reset Pin to High */
    GPIO_SetBit(GPIOA, GPIO_PIN_2);

    /* Read Smartcard ATR response */
    for (uint16_t i = 0; i < 40; i++, Counter = 0)
    {
        while ((USART_ReadStatusFlag(USART2, USART_FLAG_RXBNE) == RESET) \
                && (Counter != Smartcard_Receive_Timeout))
        {
            Counter++;
        }

        if (Counter != Smartcard_Receive_Timeout)
        {
            ReceivedBuffer[i] = USART_RxData(USART2);
        }
    }

    Counter = 0;

    for (uint16_t i = 0; i < 40; i++)
    {
        if (ReceivedBuffer[i] == 0)
        {
            Counter ++;
        }
    }

    if (Counter == 40)
    {
        /* The data received by the Smartcard is abnormal */
        return ERROR;
    }

    /* Decode ATR, Test if the inserted card is ISO7816-3 T=0 compatible */
    if (Smartcard_decode_Answer2reset(ReceivedBuffer) == 0)
    {
        /* Inserted card is ISO7816-3 T=0 compatible */
        return SUCCESS;
    }
    else
    {
        /* Inserted Smartcard is not ISO7816-3 T=0 compatible */
        return ERROR;
    }
}

/*!
 * @brief       Decode the Card ATR Response.
 *
 * @param       card: pointer to the buffer containing the Card ATR.
 *
 * @retval      Card protocol
 *
 */
uint8_t Smartcard_decode_Answer2reset(__IO uint8_t* cardBuff)
{
    uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

    Smartcard_A2RStruct.TS = 0;
    Smartcard_A2RStruct.T0 = 0;

    for (i = 0; i < Smartcard_Setup_Lenght; i++)
    {
        Smartcard_A2RStruct.T[i] = 0;
    }

    for (i = 0; i < Smartcard_Hist_Lenght; i++)
    {
        Smartcard_A2RStruct.H[i] = 0;
    }

    Smartcard_A2RStruct.Tlenght = 0;
    Smartcard_A2RStruct.Hlenght = 0;

    Smartcard_A2RStruct.TS = cardBuff[0]; /*!< INITIAL CHARACTER */
    Smartcard_A2RStruct.T0 = cardBuff[1]; /*!< FORMAT  CHARACTER */

    Smartcard_A2RStruct.Hlenght = Smartcard_A2RStruct.T0 & 0x0F;

    if ((Smartcard_A2RStruct.T0 & 0x80) == 0x80)
    {
        flag = 1;
    }

    for (i = 0; i < 4; i++)
    {
        Smartcard_A2RStruct.Tlenght = Smartcard_A2RStruct.Tlenght + \
                                      (((Smartcard_A2RStruct.T0 & 0xF0) >> (4 + i)) & 0x1);
    }

    for (i = 0; i < Smartcard_A2RStruct.Tlenght; i++)
    {
        Smartcard_A2RStruct.T[i] = cardBuff[i + 2];
    }

    protocol = Smartcard_A2RStruct.T[Smartcard_A2RStruct.Tlenght - 1] & 0x0F;

    while (flag)
    {
        if ((Smartcard_A2RStruct.T[Smartcard_A2RStruct.Tlenght - 1] & 0x80) == 0x80)
        {
            flag = 1;
        }
        else
        {
            flag = 0;
        }

        buf = Smartcard_A2RStruct.Tlenght;
        Smartcard_A2RStruct.Tlenght = 0;

        for (i = 0; i < 4; i++)
        {
            Smartcard_A2RStruct.Tlenght = Smartcard_A2RStruct.Tlenght + \
                                          (((Smartcard_A2RStruct.T[buf - 1] & 0xF0) >> (4 + i)) & 0x1);
        }

        for (i = 0; i < Smartcard_A2RStruct.Tlenght; i++)
        {
            Smartcard_A2RStruct.T[buf + i] = cardBuff[i + 2 + buf];
        }

        Smartcard_A2RStruct.Tlenght += buf;
    }

    for (i = 0; i < Smartcard_A2RStruct.Hlenght; i++)
    {
        Smartcard_A2RStruct.H[i] = cardBuff[i + 2 + Smartcard_A2RStruct.Tlenght];
    }

    return ((uint8_t)protocol);
}

/*!
 * @brief       This function handles Smartcard USART global interrupt request.
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SmartcardIRQHandler(void)
{
    /* If the SC_USART detects a parity error */
    if (USART_ReadIntFlag(USART2, USART_INT_FLAG_PE) != RESET)
    {
        /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
        USART_EnableInterrupt(USART2, USART_INT_RXBNEIE);
        /* Flush the SC_USART DR register */
        USART_RxData(USART2);
    }

    if (USART_ReadIntFlag(USART2, USART_INT_FLAG_RXBNE) != RESET)
    {
        /* Disable SC_USART RXNE Interrupt */
        USART_DisableInterrupt(USART2, USART_INT_RXBNEIE);
        USART_RxData(USART2);
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

/**@} end of group USART_Smartcard_Functions */
/**@} end of group USART_LIN */
/**@} end of group Examples */
