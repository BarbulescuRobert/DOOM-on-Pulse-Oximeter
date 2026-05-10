/*!
 * @file        usbh_cdc.h
 *
 * @brief       USB CDC core function head file
 *
 * @attention
 *
 *  Copyright (C) 2023-2024 Geehy Semiconductor
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
#ifndef USBH_CDC_H
#define USBH_CDC_H

/* Includes */
#include "usbh_core.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_CDC_Class
  @{
  */

/** @defgroup USBH_CDC_Macros Macros
  @{
*/

#define USBH_CDC_ACM_CODE           0x02
#define USBH_CDC_AT_COMMAND_CODE    0x01
#define USBH_CDC_LINE_CODING_NUM    0x07

/**@} end of group USBH_CDC_Macros*/

/** @defgroup USBH_CDC_Enumerates Enumerates
  @{
  */

/**
 * @brief    USB CDC state table
 */
typedef enum
{
    USBH_CDC_IDLE = 0,
    USBH_CDC_SET_LINE_CODING_STATE,
    USBH_CDC_GET_LINE_CODING_STATE,
    USBH_CDC_SET_CONTROL_LINE_STATE,
    USBH_CDC_TRANSFER_DATA_STATE,
    USBH_CDC_ERROR_STATE,
} USBH_CDC_STATE_T;

/**
 * @brief    USB CDC tx data state table
 */
typedef enum
{
    USBH_CDC_DATA_TX_IDLE = 0,
    USBH_CDC_DATA_SEND,
    USBH_CDC_DATA_SEND_WAIT,
} USBH_CDC_TX_DATA_STATE_T;

/**
 * @brief    USB CDC rx data state table
 */
typedef enum
{
    USBH_CDC_DATA_RX_IDLE = 0,
    USBH_CDC_DATA_RECEIVE,
    USBH_CDC_DATA_RECEIVE_WAIT,
} USBH_CDC_RX_DATA_STATE_T;

/**
 * @brief   USB CDC device class requests type
 */
typedef enum
{
    USBH_CDC_REQ_SEND_ENCAPSULATED_COMMAND = 0,
    USBH_CDC_REQ_SET_LINE_CODING = 0x20,
    USBH_CDC_REQ_GET_LINE_CODING,
    USBH_CDC_REQ_SET_CONTROL_LINE_STATE,
} USBH_CDC_REQ_TYPE_T;

/**@} end of group USBH_CDC_Enumerates*/

/** @defgroup USBH_CDC_Structures Structures
  @{
  */

/* Host CDC class state handler function */
typedef USBH_STA_T(*USBH_CDCStateHandler_T)(USBH_INFO_T* usbInfo);
typedef USBH_STA_T(*USBH_CDCDataHandler_T)(USBH_INFO_T* usbInfo);

/**
 * @brief    CDC line coding structure
 */
typedef union
{
    uint8_t data[USBH_CDC_LINE_CODING_NUM];

    struct
    {
        uint32_t        dwDTERate;
        uint8_t         bCharFormat;
        uint8_t         bParityType;
        uint8_t         bDataBits;
    } DATA_B;
} USBH_CDC_LINE_CODING_T;

/**
 * @brief    CDC control line state structure
 */
typedef union
{
    uint8_t bitmap;

    struct
    {
        uint8_t     DTR         : 1;
        uint8_t     RTS         : 1;
        uint8_t     RESERVED    : 6;
    } DATA_B;

} USBH_CDC_CONTROL_LINE_STATE_T;

/**
 * @brief    CDC communication data
 */
typedef struct
{
    uint8_t     notifyChNum;        /* Notify channel number */
    uint8_t     notifyEpAddr;       /* Notify endpoint address */
    uint8_t     notifyBuffer[8];    /* Notify buffer */
    uint16_t    notifyEpSize;       /* Notify endpoint size */
} USBH_CDC_COMMUNICATION_T;

/**
 * @brief    CDC transfer data
 */
typedef struct
{
    uint8_t     inChNum;        /* IN channel number */
    uint8_t     outChNum;       /* OUT channel number */
    uint8_t     inEpAddr;       /* IN endpoint address */
    uint8_t     outEpAddr;      /* OUT endpoint address */
    uint8_t     dataBuffer[8];  /* Data buffer */
    uint16_t    inEpsize;       /* IN endpoint size */
    uint16_t    outEpsize;      /* OUT endpoint size */
} USBH_CDC_DATA_T;

/**
 * @brief    CDC information management
 */
typedef struct
{
    USBH_CDC_COMMUNICATION_T        commXfer;           /* Communication transfer */
    USBH_CDC_DATA_T                 dataXfer;           /* Data transfer */
    uint8_t                         *txBuffer;          /* Transmit buffer */
    uint8_t                         *rxBuffer;          /* Receive buffer */
    uint32_t                        txLength;           /* Transmit length */
    uint32_t                        rxLength;           /* Receive length */
    USBH_CDC_LINE_CODING_T          lineCoding;         /* Line coding */
    USBH_CDC_LINE_CODING_T          *userLineCoding;    /* User line coding */
    USBH_CDC_CONTROL_LINE_STATE_T   *controlLine;       /* Control line state */
    USBH_CDC_STATE_T                state;              /* CDC state */
    USBH_CDC_TX_DATA_STATE_T        dataTxState;        /* CDC tx data state */
    USBH_CDC_RX_DATA_STATE_T        dataRxState;        /* CDC rx data state */
} USBH_CDC_INFO_T;

extern USBH_CLASS_T USBH_CDC_CLASS;

/**@} end of group USBH_CDC_Structures*/

/** @defgroup USBH_CDC_Functions Functions
  @{
  */

uint16_t USBH_CDC_ReadRevDataSize(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_CDC_SendData(USBH_INFO_T* usbInfo, uint8_t* buffer, uint32_t length);
USBH_STA_T USBH_CDC_ReceiveData(USBH_INFO_T* usbInfo, uint8_t* buffer, uint32_t length);
USBH_CDC_TX_DATA_STATE_T USBH_CDC_GetTxDataState(USBH_INFO_T* usbInfo);
USBH_CDC_RX_DATA_STATE_T USBH_CDC_GetRxDataState(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_CDC_GetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T *lineCoding);
USBH_STA_T USBH_CDC_SetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T *lineCoding);
USBH_STA_T USBH_CDC_Stop(USBH_INFO_T* usbInfo);

void USBH_CDC_LineCodingIsChangeCallback(USBH_INFO_T* usbInfo);
void USBH_CDC_XferEndCallback(USBH_INFO_T* usbInfo);
void USBH_CDC_RevEndCallback(USBH_INFO_T* usbInfo);

/**@} end of group USBH_CDC_Functions */
/**@} end of group USBH_CDC_Class */
/**@} end of group APM32_USB_Library */

#endif
