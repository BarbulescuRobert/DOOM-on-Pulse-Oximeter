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
#include "main.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_usart.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup USART_LIN USART LIN
  @{
*/

/** @defgroup USART_LIN_Macros Macros
  @{
*/

/* printf function configs to USART3 */
#define DEBUG_USART  USART3

/**@} end of group USART_LIN_Macros */

/** @defgroup USART_LIN_Enumerations Enumerations
  @{
*/

/**@} end of group USART_LIN_Enumerations */

/** @defgroup USART_LIN_Structures Structures
  @{
*/

typedef struct
{
    uint8_t     ID;         /*!< Specifies the id */
    uint8_t     data[8];    /*!< Specifies the data */
    uint8_t     CheckSum;   /*!< Specifies the check sum */
} LIN_Message_T;

/**@} end of group USART_LIN_Structures */

/** @defgroup USART_LIN_Variables Variables
  @{
*/

LIN_Message_T MasterSend;
LIN_Message_T MasterReceived;

uint8_t MasterReceivedProces = 0;
uint8_t MasterReceiveDataProces = 0;
uint8_t MasterReceivedOverFlag = 0;

LIN_Message_T SlaveSend;
LIN_Message_T SlaveReceived;

uint8_t SlaveReceivedProces = 0;
uint8_t SlaveReceivedDataProces = 0;
uint8_t SlaveReceivedOverFlag = 0;

volatile uint32_t tick = 0;

/**@} end of group USART_LIN_Variables */

/** @defgroup USART_LIN_Functions Functions
  @{
*/

void USART3Init(void);
void LINInit(void);
void LIN_MasterSendSlave(void);
void LIN_MasterReadSlave(void);

uint8_t LIN_CheckPID(uint8_t id);
uint8_t LIN_CheckSum(uint8_t id, uint8_t* data);

void LIN_SendHead(USART_T* usart, LIN_Message_T data);
void LIN_SendAnswer(USART_T* usart, LIN_Message_T data);

uint8_t LIN_SlaveReceivedProces(void);
uint8_t LIN_MasterReceivedProces(void);

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

    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOff(LED3);

    USART3Init();
    LINInit();

    SystemCoreClockUpdate();
    SysTick_Config(SystemCoreClock / 1000); /*!< 1ms */

    printf("\r\nThis example provides a basic communication USARTx LIN receive and transmit mode.\r\n");

    LIN_MasterSendSlave();
    LIN_MasterReadSlave();

    while (1)
    {
        Delay();

        APM_MINI_LEDToggle(LED2);

    }
}

/*!
 * @brief       Init USART3
 *
 * @param       None
 *
 * @retval      None
 *
 */
void USART3Init(void)
{
    GPIO_Config_T gpioConfig;
    USART_Config_T usartConfigStruct;
    /* Enable GPIO clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);

    /* Enable USART3 clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART3);

    /* Connect PXx to USARTx_Tx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_10, GPIO_AF_PIN4);

    /* Connect PXx to USARTx_Rx */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_11, GPIO_AF_PIN4);

    /* Configure USART Tx as alternate function push-pull */
    gpioConfig.mode = GPIO_MODE_AF;
    gpioConfig.pin = GPIO_PIN_10;
    gpioConfig.speed = GPIO_SPEED_50MHz;
    gpioConfig.outtype = GPIO_OUT_TYPE_PP;
    gpioConfig.pupd = GPIO_PUPD_PU;
    GPIO_Config(GPIOB, &gpioConfig);

    /* Configure USART Rx as input floating */
    gpioConfig.pin = GPIO_PIN_11;
    GPIO_Config(GPIOB, &gpioConfig);


    /* MINI_USARTs configured as follow:
    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - One Stop Bit
    - No parity
    - Hardware flow control disabled (RTS and CTS signals)
    - Receive and transmit enabled
    */
    /*

    */
    usartConfigStruct.baudRate = 115200;
    usartConfigStruct.mode     = USART_MODE_TX_RX;
    usartConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    usartConfigStruct.parity   = USART_PARITY_NONE;
    usartConfigStruct.stopBits =  USART_STOP_BIT_1;
    usartConfigStruct.wordLength = USART_WORD_LEN_8B;
    USART_Config(USART3, &usartConfigStruct);

    /* Enable USART_Interrupt_RXBNEIE */
    USART_EnableInterrupt(USART3, USART_INT_RXBNEIE);

    NVIC_EnableIRQRequest(USART3_4_IRQn, 3);

    /* Enable USART */
    USART_Enable(USART3);
}

/*!
 * @brief       USART1 and USART2 IrDA mode initializes functions
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void LINInit(void)
{
    GPIO_Config_T GPIO_ConfigStruct;
    USART_Config_T USART_ConfigStruct;

    /* System Clocks Configuration */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_USART1);
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_USART2);

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

    GPIO_ConfigStruct.pin     = GPIO_PIN_3;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_2, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_3, GPIO_AF_PIN1);

    /* USART1 GPIO Configuration
    PA9  ------> USART1_TX
    PA10 ------> USART1_RX
    */
    GPIO_ConfigStruct.pin      = GPIO_PIN_9;
    GPIO_ConfigStruct.mode     = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype  = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed    = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.pupd = GPIO_PUPD_NO;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin      = GPIO_PIN_10;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_PIN1);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_PIN1);

    /* USARTx configuration */
    /* USARTx configured as follow:
        - BaudRate    = 115200
        - Word Length = USART_WordLength_8b
        - Stop Bit    = USART_StopBits_1
        - Parity      = USART_Parity_No
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
    */
    USART_ConfigStruct.baudRate = 115200;
    USART_ConfigStruct.hardwareFlowCtrl = USART_FLOW_CTRL_NONE;
    USART_ConfigStruct.mode = USART_MODE_TX_RX;
    USART_ConfigStruct.parity = USART_PARITY_NONE;
    USART_ConfigStruct.stopBits = USART_STOP_BIT_1;
    USART_ConfigStruct.wordLength = USART_WORD_LEN_8B;

    /* Configure the USARTx */
    USART_Config(USART1, &USART_ConfigStruct);
    USART_Config(USART2, &USART_ConfigStruct);

    /* Configure the USARTx Interrupt */
    USART_EnableInterrupt(USART1, USART_INT_RXBNEIE);
    NVIC_EnableIRQRequest(USART1_IRQn, 0x00);

    USART_EnableInterrupt(USART2, USART_INT_RXBNEIE);
    NVIC_EnableIRQRequest(USART2_IRQn, 0x01);

    /* Configure the USARTx LIN mode */
    USART_ConfigLINbreakDetectLength(USART1, USART_BREAK_LENGTH_10B);
    USART_ConfigLINbreakDetectLength(USART2, USART_BREAK_LENGTH_10B);

    /* Enable the USARTx LIN mode */
    USART_EnableLINmode(USART1);
    USART_EnableLINmode(USART2);

    /* Enable the USARTx */
    USART_Enable(USART1);
    USART_Enable(USART2);

    /* Clear USARTx TXC Flag */
    USART_ClearStatusFlag(USART1, USART_FLAG_TXC | USART_FLAG_TXBE);
    USART_ClearStatusFlag(USART2, USART_FLAG_TXC | USART_FLAG_TXBE);
}

/*!
 * @brief       The master send data to the slave function
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LIN_MasterSendSlave(void)
{
    MasterSend.ID = 0x23;

    /* The following data is the data sent from the master to the slave. */
    MasterSend.data[0] = 0x11;
    MasterSend.data[1] = 0x22;
    MasterSend.data[2] = 0x33;
    MasterSend.data[3] = 0x44;
    MasterSend.data[4] = 0x55;
    MasterSend.data[5] = 0x66;
    MasterSend.data[6] = 0x77;
    MasterSend.data[7] = 0x88;

    LIN_SendHead(USART2, MasterSend);
    LIN_SendAnswer(USART2, MasterSend);

    while (SlaveReceivedOverFlag == 0);

    Delay();
    printf("\r\nThe master sends data to the slave.\r\n");
    printf("Master Send ID : 0x%X ,\r\n", MasterSend.ID);
    printf("Slave Received ID : 0x%X ,\r\n", SlaveReceived.ID & 0x3f);

    printf("Master Send Data : ");

    for (uint8_t i = 0; i < 8; i++)
    {
        printf("0x%X ,", MasterSend.data[i]);
    }

    printf(" CheckSum : 0x%X \r\n",  LIN_CheckSum(MasterSend.ID, MasterSend.data));

    printf("Slave Received Data : ");

    for (uint8_t i = 0; i < 8; i++)
    {
        printf("0x%X ,", SlaveReceived.data[i]);
    }

    printf(" CheckSum : 0x%X \r\n", SlaveReceived.CheckSum);

    if (LIN_SlaveReceivedProces() == SUCCESS)
    {
        printf("The master sends data to the slave successfully.\r\n");
    }
    else
    {
        printf("The master fails to send data to the slave.\r\n");
    }
}

/*!
 * @brief       Master read slave data function
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LIN_MasterReadSlave(void)
{
    MasterSend.ID = 0x33;

    /* The following data is slave data read by master. */
    SlaveSend.data[0] = 0xFF;
    SlaveSend.data[1] = 0xDD;
    SlaveSend.data[2] = 0xCC;
    SlaveSend.data[3] = 0xBB;
    SlaveSend.data[4] = 0xAA;
    SlaveSend.data[5] = 0x99;
    SlaveSend.data[6] = 0x88;
    SlaveSend.data[7] = 0x77;

    LIN_SendHead(USART2, MasterSend);

    while (MasterReceivedOverFlag == 0);

    printf("\r\nThe master reads the slave data.\r\n");
    printf("Master Send ID : 0x%X ,\r\n", MasterSend.ID);
    printf("Slave Received ID : 0x%X ,\r\n", SlaveReceived.ID & 0x3f);

    printf("Slave Send Data : ");

    for (uint8_t i = 0; i < 8; i++)
    {
        printf("0x%X ,", SlaveSend.data[i]);
    }

    printf("CheckSum : 0x%X \r\n",  LIN_CheckSum(MasterSend.ID, SlaveSend.data));

    printf("Master Received Data :");

    for (uint8_t i = 0; i < 8; i++)
    {
        printf("0x%X ,", MasterReceived.data[i]);
    }

    printf("CheckSum : 0x%X \r\n", MasterReceived.CheckSum);

    if (LIN_MasterReceivedProces() == SUCCESS)
    {
        printf("The master reads the slave data and the verification is successful.\r\n");
    }
    else
    {
        printf("The master reads the slave data and the verification is failed.\r\n");
    }
}

/*!
 * @brief       Transmits break characters
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @retval      None
 *
 */
void LIN_SendBreak(USART_T* usart)
{
    usart->REQUEST_B.TXBFQ = BIT_SET;
}

/*!
 * @brief       Transmits Sync Segment
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @retval      None
 *
 */
void LIN_SendSyncSegment(USART_T* usart)
{
    USART_TxData(usart, 0x55);

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}

/*!
 * @brief       ID check
 *
 * @param       id: frame ID
 *
 * @retval      checked id
 *
 */
uint8_t LIN_CheckPID(uint8_t id)
{
    uint8_t returnpid ;
    uint8_t P0 ;
    uint8_t P1 ;

    P0 = (((id) ^ (id >> 1) ^ (id >> 2) ^ (id >> 4)) & 0x01) << 6 ;
    P1 = ((~((id >> 1) ^ (id >> 3) ^ (id >> 4) ^ (id >> 5))) & 0x01) << 7 ;

    returnpid = id | P0 | P1 ;

    return returnpid ;
}

/*!
 * @brief       The master header frame sending function.
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       LINMessage: LIN Message structure @LIN_Message_T
 *
 * @retval      None
 *
 */
void LIN_SendHead(USART_T* usart, LIN_Message_T LINMessage)
{
    LIN_SendBreak(usart);
    LIN_SendSyncSegment(usart);
    USART_TxData(usart, LIN_CheckPID(LINMessage.ID));

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}


/*!
 * @brief       Check data sum
 *
 * @param       id: frame ID
 *
 * @param       data: frame data
 *
 * @retval      checked sum
 *
 */
uint8_t LIN_CheckSum(uint8_t id, uint8_t* data)
{
    uint8_t t ;
    uint16_t sum ;

    sum = data[0];

    for (t = 1; t < 8; t++)
    {
        sum += data[t];

        if (sum & 0xff00)
        {
            sum &= 0x00ff;
            sum += 1;
        }
    }

    /* In the case of diagnostic frames, the classic checksum is used. */
    if (id != 0x3C)
    {
        sum += LIN_CheckPID(id);

        if (sum & 0xff00)
        {
            sum &= 0x00ff;
            sum += 1;
        }
    }

    sum = ~sum;
    return (uint8_t)sum ;
}

/*!
 * @brief       Transmits data
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       data: frame data
 *
 * @retval      None
 *
 */
void LIN_TxData(USART_T* usart, uint8_t* data)
{
    uint8_t t, txdata ;

    for (t = 0; t < 8; t++)
    {
        txdata = data[t];

        USART_TxData(usart, txdata);

        /* wait for the data to be send */
        while (USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
    }
}

/*!
 * @brief       Transmits Answer
 *
 * @param       usart: Select the USART or the UART peripheral
 *
 * @param       LINMessage: LIN Message strucstruct @LIN_Message_T
 *
 * @retval      None
 *
 */
void LIN_SendAnswer(USART_T* usart, LIN_Message_T LINMessage)
{
    LIN_TxData(usart, LINMessage.data);
    USART_TxData(usart, LIN_CheckSum(LINMessage.ID, LINMessage.data));

    /* wait for the data to be send */
    while (USART_ReadStatusFlag(usart, USART_FLAG_TXBE) == RESET);
}


/*!
 * @brief       Slave received data flame process
 *
 * @param       None
 *
 * @retval      ERROR/SUCCESS: data verify is failed/successful.
 *
 */
uint8_t LIN_SlaveReceivedProces(void)
{

    if (MasterSend.ID != (SlaveReceived.ID & 0x3f))
    {
        return ERROR;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        if (MasterSend.data[i] != SlaveReceived.data[i])
        {
            return ERROR;
        }
    }

    if (LIN_CheckSum(MasterSend.ID, MasterSend.data) != SlaveReceived.CheckSum)
    {
        return ERROR;
    }

    return SUCCESS;
}

/*!
 * @brief       Master received data flame process
 *
 * @param       None
 *
 * @retval      ERROR/SUCCESS: data verify is failed/successful.
 *
 */
uint8_t LIN_MasterReceivedProces(void)
{

    if (MasterSend.ID != (SlaveReceived.ID & 0x3f))
    {
        return ERROR;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        if (SlaveSend.data[i] != MasterReceived.data[i])
        {
            return ERROR;
        }
    }

    if (LIN_CheckSum(MasterSend.ID, SlaveSend.data) != MasterReceived.CheckSum)
    {
        return ERROR;
    }

    return SUCCESS;
}

/*!
 * @brief       Master interrupt server functions
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LIN_MasterISR(void)
{
    uint8_t ReceiveData;

    if (USART_ReadIntFlag(USART2, USART_INT_FLAG_RXBNE) == SET)
    {
        ReceiveData = USART_RxData(USART2);

        MasterReceivedOverFlag = 0 ;

        if (MasterReceivedProces == 0)
        {
            if (MasterReceiveDataProces < 8)
            {
                MasterReceived.data[MasterReceiveDataProces] = ReceiveData ;
                MasterReceiveDataProces += 1 ;

                if (MasterReceiveDataProces == 8)
                {
                    MasterReceiveDataProces = 0 ;
                    MasterReceivedProces = 1 ;
                }
            }
        }
        else
        {
            MasterReceived.CheckSum = ReceiveData;
            MasterReceivedProces = 0 ;
            MasterReceivedOverFlag = 1 ;
        }
    }
}

/*!
 * @brief       Slave interrupt server functions
 *
 * @param       None
 *
 * @retval      None
 *
 */
void LIN_SlaveISR(void)
{
    uint8_t ReceiveData;
    uint8_t ReceiveID;

    if (USART_ReadIntFlag(USART1, USART_INT_FLAG_RXBNE) == SET)
    {
        ReceiveData = USART_RxData(USART1);

        SlaveReceivedOverFlag = 0;

        if (SlaveReceivedProces == 0)       /*!< step1: sync */
        {
            /* Sync frame processing */
            if (ReceiveData == 0x55)
            {
                SlaveReceivedProces = 1 ;
            }
        }
        else if (SlaveReceivedProces == 1)  /*!< step2: id */
        {
            SlaveReceived.ID = ReceiveData ;
            ReceiveID = SlaveReceived.ID & 0x3f ;

            /* The master reads the slave data */
            if (ReceiveID == 0x33)
            {
                LIN_TxData(USART1, SlaveSend.data);
                USART_TxData(USART1, LIN_CheckSum(ReceiveID, SlaveSend.data));

                /* wait for the data to be send */
                while (USART_ReadStatusFlag(USART1, USART_FLAG_TXBE) == RESET);
            }

            SlaveReceivedProces = 2 ;
        }
        else if (SlaveReceivedProces == 2) /*!< step3: data */
        {
            /* data frame processing */
            if (SlaveReceivedDataProces < 8)
            {
                SlaveReceived.data[SlaveReceivedDataProces] = ReceiveData ;
                SlaveReceivedDataProces += 1 ;

                if (SlaveReceivedDataProces == 8)
                {
                    SlaveReceivedDataProces = 0 ;
                    SlaveReceivedProces = 3 ;
                    return ;
                }
            }
        }
        else if (SlaveReceivedProces == 3) /*!< step4: checksum */
        {
            /* Check frame processing */
            SlaveReceived.CheckSum = ReceiveData ;
            SlaveReceivedProces = 0 ;
            SlaveReceivedOverFlag = 1 ;
        }
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

/**@} end of group USART_LIN_Functions */
/**@} end of group USART_LIN */
/**@} end of group Examples */
