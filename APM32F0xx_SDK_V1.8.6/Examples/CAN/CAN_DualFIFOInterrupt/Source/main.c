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
#include "apm32f0xx_misc.h"
#include "apm32f0xx_can.h"
#include "apm32f0xx_eint.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup CAN_LoopBack
  @{
  */

/** @defgroup CAN_LoopBack_Macros Macros
  @{
  */

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group CAN_LoopBack_Macros */

/** @defgroup CAN_LoopBack_Enumerations Enumerations
  @{
  */

/**@} end of group CAN_LoopBack_Enumerations */

/** @defgroup CAN_LoopBack_Structures Structures
  @{
  */

/**@} end of group CAN_LoopBack_Structures */

/** @defgroup CAN_LoopBack_Variables Variables
  @{
  */

/* System tick */
uint32_t sysTick = 0;

/* TxMailbox transmit complete flag */
volatile uint8_t TxMailbox0Complete = 0;
volatile uint8_t TxMailbox1Complete = 0;
volatile uint8_t TxMailbox2Complete = 0;

/* CAN Tx message */
CAN_Tx_Message TxMessage0;
CAN_Tx_Message TxMessage1;

/* CAN Rx message */
CAN_Rx_Message RxMessage0;
CAN_Rx_Message RxMessage1;

/**@} end of group CAN_LoopBack_Variables */

/** @defgroup CAN_LoopBack_Functions Functions
  @{
  */

void Delay(uint32_t count);
void CAN_GPIO_Config(void);
void CAN_Init(void);
void CAN_TxMessageInit(void);
uint8_t CAN_TxMessageSpecifiedMailbox(CAN_Tx_Message *TxMessage, CAN_MAILBOX_T TxMailbox);

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
    /* Initialize LEDs/Buttons/COMs */
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    APM_MINI_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);
    APM_MINI_COMInit(COM1);
    APM_MINI_LEDOn(LED2);
    
    /* System tick Configuration */
    SysTick_Config(SystemCoreClock / 1000);

    /* CAN Configuration */
    CAN_GPIO_Config();
    CAN_Init();
    CAN_TxMessageInit();
    
    printf("\r\nCAN Dual FIFO Interrupt.\r\n");

    while (1)
    {
        if (APM_MINI_PBGetState(BUTTON_KEY1) != SET)
        {
            /* Reset the CAN send complete flag */
            TxMailbox0Complete = 0;
            TxMailbox1Complete = 0;
            TxMailbox2Complete = 0;

            /* CAN send frame to FIFO_0 */
            TxMessage0.data[0] = 0x12;
            printf("\r\nCAN start send frame to FIFO_0: 0x%2X\r\n", TxMessage0.data[0]);

            CAN_TxMessageSpecifiedMailbox(&TxMessage0, CAN_MAILBOX_0);
            CAN_TxMessageSpecifiedMailbox(&TxMessage0, CAN_MAILBOX_1);
            CAN_TxMessageSpecifiedMailbox(&TxMessage0, CAN_MAILBOX_2);
            
            /* Wait for send complete */
            while ((TxMailbox0Complete == 0) || (TxMailbox1Complete == 0) || (TxMailbox2Complete == 0));
            
            /* Wait for button release */
            while (APM_MINI_PBGetState(BUTTON_KEY1) != SET);
        }

        if (APM_MINI_PBGetState(BUTTON_KEY2) != SET)
        {
            /* Reset the CAN send complete flag */
            TxMailbox0Complete = 0;
            TxMailbox1Complete = 0;
            TxMailbox2Complete = 0;
            
            /* CAN send frame to FIFO_1 */
            TxMessage1.data[0] = 0x34;
            printf("\r\nCAN send frame to FIFO_1: 0x%2X\r\n", TxMessage1.data[0]);

            CAN_TxMessageSpecifiedMailbox(&TxMessage1, CAN_MAILBOX_0);
            CAN_TxMessageSpecifiedMailbox(&TxMessage1, CAN_MAILBOX_1);
            CAN_TxMessageSpecifiedMailbox(&TxMessage1, CAN_MAILBOX_2);

            /* Wait for send complete */
            while ((TxMailbox0Complete == 0) || (TxMailbox1Complete == 0) || (TxMailbox2Complete == 0));
            
            /* Wait for button release */
            while (APM_MINI_PBGetState(BUTTON_KEY2) != SET);
        }
    }
}

/*!
 * @brief       CAN GPIO config
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void CAN_GPIO_Config(void)
{
    GPIO_Config_T gpioConfigStruct;

    /* Enable CAN GPIO clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    
    /* Configure CAN RX(PB8)/TX(PB9) Pins Alternate function */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_8, GPIO_AF_PIN4);
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_9, GPIO_AF_PIN4);
    
    /* Configure CAN RX(PB8)/TX(PB9) Pins */
    gpioConfigStruct.pin        = GPIO_PIN_8 | GPIO_PIN_9;
    gpioConfigStruct.mode       = GPIO_MODE_AF;
    gpioConfigStruct.speed      = GPIO_SPEED_50MHz;
    gpioConfigStruct.outtype    = GPIO_OUT_TYPE_PP;
    gpioConfigStruct.pupd       = GPIO_PUPD_PU;
    GPIO_Config(GPIOB, &gpioConfigStruct);
}

/*!
 * @brief       CAN init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void CAN_Init(void)
{
    CAN_Config_T       canConfigStruct;
    CAN_FilterConfig_T filterConfigStruct;
    
    /* Enable CAN clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_CAN);
    
    /* CAN register reset */
    CAN_Reset();
    
    /* CAN configuration */
    CAN_ConfigStructInit(&canConfigStruct);
    canConfigStruct.autoBusOffManage = DISABLE;
    canConfigStruct.autoWakeUpMode   = DISABLE;
    canConfigStruct.nonAutoRetran    = DISABLE;
    canConfigStruct.timeTrigComMode  = DISABLE;
    canConfigStruct.txFIFOPriority   = DISABLE;
    canConfigStruct.rxFIFOLockMode   = DISABLE;
    canConfigStruct.mode             = CAN_MODE_NORMAL;
    canConfigStruct.syncJumpWidth    = CAN_SJW_1;
    
    /* CAN Baudrate = APB1CLK / ((timeSegment1 + timeSegment2 + 1) * prescaler)
     *              = 48MHz / ((15 + 8 + 1) * 4) = 500Kbps */
    canConfigStruct.timeSegment1     = CAN_TIME_SEGMENT1_15;
    canConfigStruct.timeSegment2     = CAN_TIME_SEGMENT2_8;
    canConfigStruct.prescaler        = 4;
    CAN_Config(&canConfigStruct);
    
    /* CAN filter init for FIFO_0 */
    filterConfigStruct.filterNumber = CAN_FILTER_NUMBER_0;
    filterConfigStruct.filterMode   = CAN_FILTER_MODE_IDMASK;
    filterConfigStruct.filterScale  = CAN_FILTER_SCALE_32BIT;
    filterConfigStruct.filterIdHigh = 0x6420;
    filterConfigStruct.filterIdLow  = 0x2461;
    filterConfigStruct.filterMaskIdHigh = 0x0000;
    filterConfigStruct.filterMaskIdLow  = 0x0000;
    filterConfigStruct.filterFIFO       = CAN_FIFO_0;
    filterConfigStruct.filterActivation = ENABLE;
    CAN_ConfigFilter(&filterConfigStruct);
    
    /* CAN filter init for FIFO_1 */
    filterConfigStruct.filterNumber = CAN_FILTER_NUMBER_1;
    filterConfigStruct.filterMode   = CAN_FILTER_MODE_IDLIST;
    filterConfigStruct.filterScale  = CAN_FILTER_SCALE_32BIT;
    filterConfigStruct.filterIdHigh = 0x2460;
    filterConfigStruct.filterIdLow  = 0x0000;
    filterConfigStruct.filterMaskIdHigh = 0x0000;
    filterConfigStruct.filterMaskIdLow  = 0x0000;
    filterConfigStruct.filterFIFO       = CAN_FIFO_1;
    filterConfigStruct.filterActivation = ENABLE;
    CAN_ConfigFilter(&filterConfigStruct);

    /* Enable FIFO 0/1 message pending interrupt */
    CAN_EnableInterrupt(CAN_INT_F0MP | CAN_INT_F1MP);

    /* Enable transmit mailbox empty Interrupt */
    CAN_EnableInterrupt(CAN_INT_TXME);

    /* Enable Error warning Interrupt */
    CAN_EnableInterrupt(CAN_INT_EWIE);

    /* Enable Bus-off Interrupt */
    CAN_EnableInterrupt(CAN_INT_BOIE);

    /* NVIC configuration */
    NVIC_EnableIRQRequest(CEC_CAN_IRQn, 0x01);
}

/*!
 * @brief       CAN Tx message init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void CAN_TxMessageInit(void)
{
    /* CAN Tx Message0 configuration */
    TxMessage0.stanID         = 0x0321;
    TxMessage0.extenID        = 0x0000;
    TxMessage0.typeID         = CAN_TYPEID_STD;
    TxMessage0.remoteTxReq    = CAN_RTXR_DATA;
    TxMessage0.dataLengthCode = 1;

    /* CAN Tx Message1 configuration */
    TxMessage1.stanID         = 0x0123;
    TxMessage1.extenID        = 0x0000;
    TxMessage1.typeID         = CAN_TYPEID_STD;
    TxMessage1.remoteTxReq    = CAN_RTXR_DATA;
    TxMessage1.dataLengthCode = 1;
}

/*!
 * @brief       CAN IRQHandler
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void CAN_Isr(void)
{
    static uint32_t txMailbox0Count = 0;
    static uint32_t txMailbox1Count = 0;
    static uint32_t txMailbox2Count = 0;

    /* CAN FIFO_0 receive data */
    if (CAN_PendingMessage(CAN_FIFO_0) != 0)
    {
        CAN_RxMessage(CAN_FIFO_0, &RxMessage0);
        
        printf("CAN FIF0_0 receive data: 0x%02X\r\n", RxMessage0.data[0]);
    }

    /* CAN FIFO_0 receive data */
    if (CAN_PendingMessage(CAN_FIFO_1) != 0)
    {
        CAN_RxMessage(CAN_FIFO_1, &RxMessage1);
        
        printf("CAN FIF0_1 receive data: 0x%02X\r\n", RxMessage1.data[0]);
    }
    
    /* CAN Transmit data complete */
    if (CAN_ReadIntFlag(CAN_INT_TXME) == SET)
    {
        if (CAN->TXSTS_B.REQCFLG0 && CAN->TXSTS_B.TXSUSFLG0 && CAN->TXSTS_B.TXMEFLG0)
        {
            TxMailbox0Complete = 1;
            txMailbox0Count++;
            printf("TxMailbox0 send complete count: %d\r\n", txMailbox0Count);
        }
        
        if (CAN->TXSTS_B.REQCFLG1 && CAN->TXSTS_B.TXSUSFLG1 && CAN->TXSTS_B.TXMEFLG1)
        {
            TxMailbox1Complete = 1;
            txMailbox1Count++;
            printf("TxMailbox1 send complete count: %d\r\n", txMailbox1Count);
        }
        
        if (CAN->TXSTS_B.REQCFLG2 && CAN->TXSTS_B.TXSUSFLG2 && CAN->TXSTS_B.TXMEFLG2)
        {
            TxMailbox2Complete = 1;
            txMailbox2Count++;
            printf("TxMailbox2 send complete count: %d\r\n", txMailbox2Count);
        }
        
        CAN_ClearIntFlag(CAN_INT_TXME);
    }
    
    /* Error warning interrupt handler */
    if (CAN_ReadIntFlag(CAN_INT_EWIE) == SET)
    {
        /* User can add error handing code */
        
        CAN_ClearIntFlag(CAN_INT_EWIE);
    }
    
    /* Bus-off interrupt handler */
    if (CAN_ReadIntFlag(CAN_INT_BOIE) == SET)
    {
        /* User can add error handing code */
        
        CAN_ClearIntFlag(CAN_INT_BOIE);
    }
}

/*!
 * @brief     Transmit a message from TxMessage of CAN_Tx_Message structure.
 *
 * @param     TxMessage : Pointer to a structure which contains CAN Id, CAN DLCODE and CAN data.
 * @param     TxMailbox : Specified Tx mailbox.
 *
 * @retval    Returns Transmit status:
 *                 @arg 0: Transmit fail.
 *                 @arg 1: Transmit successful.
 */
uint8_t CAN_TxMessageSpecifiedMailbox(CAN_Tx_Message *TxMessage, CAN_MAILBOX_T TxMailbox)
{
    uint8_t txMailboxEmptyFlag = RESET;
    uint8_t txStatus = 0;
    
    switch (TxMailbox)
    {
        case (CAN_MAILBOX_0):
            txMailboxEmptyFlag = CAN->TXSTS_B.TXMEFLG0;
            break;

        case (CAN_MAILBOX_1):
            txMailboxEmptyFlag = CAN->TXSTS_B.TXMEFLG1;
            break;

        case (CAN_MAILBOX_2):
            txMailboxEmptyFlag = CAN->TXSTS_B.TXMEFLG2;
            break;

        default:
            break;
    }
    
    if (txMailboxEmptyFlag == SET)
    {
        /* Set up the Id */
        CAN->sTxMailBox[TxMailbox].TXMID &= BIT0;
        if (TxMessage->typeID == CAN_TYPEID_STD)
        {
            CAN->sTxMailBox[TxMailbox].TXMID |= (((uint32_t)TxMessage->stanID << 21) | \
                                                 TxMessage->typeID        | \
                                                 TxMessage->remoteTxReq);
        }
        else
        {
            CAN->sTxMailBox[TxMailbox].TXMID |= (((uint32_t)TxMessage->extenID << 3) | \
                                                 TxMessage->typeID       | \
                                                 TxMessage->remoteTxReq);
        }

        /* Set up the DLCODE */
        CAN->sTxMailBox[TxMailbox].TXDLEN_B.DLCODE = (TxMessage->dataLengthCode & 0x0F);

        /* Set up the data field */
        CAN->sTxMailBox[TxMailbox].TXMDH_B.DATABYTE7 = TxMessage->data[7];
        CAN->sTxMailBox[TxMailbox].TXMDH_B.DATABYTE6 = TxMessage->data[6];
        CAN->sTxMailBox[TxMailbox].TXMDH_B.DATABYTE5 = TxMessage->data[5];
        CAN->sTxMailBox[TxMailbox].TXMDH_B.DATABYTE4 = TxMessage->data[4];

        CAN->sTxMailBox[TxMailbox].TXMDL_B.DATABYTE3 = TxMessage->data[3];
        CAN->sTxMailBox[TxMailbox].TXMDL_B.DATABYTE2 = TxMessage->data[2];
        CAN->sTxMailBox[TxMailbox].TXMDL_B.DATABYTE1 = TxMessage->data[1];
        CAN->sTxMailBox[TxMailbox].TXMDL_B.DATABYTE0 = TxMessage->data[0];

        /* Request transmission */
        CAN->sTxMailBox[TxMailbox].TXMID |= BIT0;
        
        txStatus = 1;
    }
    
    return txStatus;
}

/*!
 * @brief       Delay
 *
 * @param       count:  delay count
 *
 * @retval      None
 *
 * @note
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;

    while (delay--);
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

/**@} end of group CAN_LoopBack_Functions */
/**@} end of group CAN_LoopBack */
/**@} end of group Examples */
