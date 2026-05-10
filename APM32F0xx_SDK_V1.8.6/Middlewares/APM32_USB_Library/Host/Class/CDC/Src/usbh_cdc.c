/*!
 * @file        usbh_cdc.h
 *
 * @brief       USB CDC core function
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

/* Includes */
#include "usbh_cdc.h"
#include "usbh_stdReq.h"
#include "usbh_dataXfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_CDC_Class
  @{
  */

/** @defgroup USBH_CDC_Functions Functions
  @{
  */

static USBH_STA_T USBH_CDC_ClassInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_ClassDeInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_ClassReqHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_SOFHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_CoreHandler(USBH_INFO_T* usbInfo);

static USBH_STA_T USBH_CDC_IdleHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_SetLineCodingHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_GetLineCodingHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_SetControlLineHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_TransferDataHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_ErrorHandler(USBH_INFO_T* usbInfo);

static USBH_STA_T USBH_CDC_DataIdleHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_DataSendHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_DataSendWaitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_DataRevHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_CDC_DataRevWaitHandler(USBH_INFO_T* usbInfo);
/**@} end of group USBH_CDC_Functions */

/** @defgroup USBH_CDC_Structures Structures
  @{
  */

/* CDC class handler */
USBH_CLASS_T USBH_CDC_CLASS =
{
    "Class CDC",
    USBH_CLASS_CDCC,
    USBH_CDC_ClassInitHandler,
    USBH_CDC_ClassDeInitHandler,
    USBH_CDC_ClassReqHandler,
    USBH_CDC_CoreHandler,
    USBH_CDC_SOFHandler,
    NULL,
};

/* USB host CDC state handler function */
USBH_CDCStateHandler_T USBH_CDC_Handler[] =
{
    USBH_CDC_IdleHandler,
    USBH_CDC_SetLineCodingHandler,
    USBH_CDC_GetLineCodingHandler,
    USBH_CDC_SetControlLineHandler,
    USBH_CDC_TransferDataHandler,
    USBH_CDC_ErrorHandler,
};

/* USB host CDC tx data state handler function */
USBH_CDCDataHandler_T USBH_CDC_TX_DATA_Handler[] =
{
    USBH_CDC_DataIdleHandler,
    USBH_CDC_DataSendHandler,
    USBH_CDC_DataSendWaitHandler,
};

/* USB host CDC rx data state handler function */
USBH_CDCDataHandler_T USBH_CDC_RX_DATA_Handler[] =
{
    USBH_CDC_DataIdleHandler,
    USBH_CDC_DataRevHandler,
    USBH_CDC_DataRevWaitHandler,
};

/**@} end of group USBH_CDC_Structures*/

/** @defgroup USBH_CDC_Functions Functions
  @{
  */

/*!
 * @brief       USB host CDC get TX data state
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host transfer data state
 */
USBH_CDC_TX_DATA_STATE_T USBH_CDC_GetTxDataState(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    return cdcInfo->dataTxState;
}

/*!
 * @brief       USB host CDC get RX data state
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host transfer data state
 */
USBH_CDC_RX_DATA_STATE_T USBH_CDC_GetRxDataState(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    return cdcInfo->dataRxState;
}

/*!
 * @brief       USB host CDC set line coding
 *
 * @param       usbInfo: usb host information
 * 
 * @param       lineCoding: line coding structure
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_SetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T* lineCoding)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        cdcInfo->userLineCoding = lineCoding;

        /* Set CDC state to set line coding */
        cdcInfo->state = USBH_CDC_SET_LINE_CODING_STATE;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host CDC get line coding
 *
 * @param       usbInfo: usb host information
 * 
 * @param       lineCoding: line coding structure
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_GetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T* lineCoding)
{
    USBH_STA_T  usbStatus = USBH_FAIL;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if ((usbInfo->hostState == USBH_HOST_CLASS) || (usbInfo->hostState == USBH_HOST_CLASS_REQ))
    {
        *lineCoding = cdcInfo->lineCoding;
        usbStatus = USBH_OK;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC configure control line state
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_ConfigControlLineState(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (cdcInfo->state == USBH_CDC_IDLE)
    {
        cdcInfo->controlLine->DATA_B.DTR = 0;
        cdcInfo->controlLine->DATA_B.RTS = 0;

        /* Set CDC state to set control line state */
        cdcInfo->state = USBH_CDC_SET_CONTROL_LINE_STATE;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host CDC read receive data size
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host CDC receive data size
 */
uint16_t USBH_CDC_ReadRevDataSize(USBH_INFO_T* usbInfo)
{
    uint32_t size = 0;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        size = USBH_ReadLastXferSizeCallback(usbInfo, cdcInfo->dataXfer.inChNum);
    }

    return size;
}

/*!
 * @brief       USB host CDC send data
 *
 * @param       usbInfo: usb host information
 *
 * @param       buffer: buffer point to send data
 *
 * @param       length: length of send data
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_SendData(USBH_INFO_T* usbInfo, uint8_t* buffer, uint32_t length)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if ((cdcInfo->state == USBH_CDC_TRANSFER_DATA_STATE) || \
        (cdcInfo->state == USBH_CDC_IDLE))
    {
        cdcInfo->txBuffer = buffer;
        cdcInfo->txLength = length;

        /* Set CDC state to send data */
        cdcInfo->state = USBH_CDC_TRANSFER_DATA_STATE;

        /* Set CDC state to send data */
        cdcInfo->dataTxState = USBH_CDC_DATA_SEND;

        usbStatus = USBH_OK;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC receive data
 *
 * @param       usbInfo: usb host information
 *
 * @param       buffer: buffer point to receive data
 *
 * @param       length: length of receive data
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_ReceiveData(USBH_INFO_T* usbInfo, uint8_t* buffer, uint32_t length)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if ((cdcInfo->state == USBH_CDC_TRANSFER_DATA_STATE) || \
        (cdcInfo->state == USBH_CDC_IDLE))
    {
        cdcInfo->rxBuffer = buffer;
        cdcInfo->rxLength = length;

        /* Set CDC state to receive data */
        cdcInfo->state = USBH_CDC_TRANSFER_DATA_STATE;

        /* Set CDC state to receive data */
        cdcInfo->dataRxState = USBH_CDC_DATA_RECEIVE;

        usbStatus = USBH_OK;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC stop
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_CDC_Stop(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        cdcInfo->state = USBH_CDC_IDLE;

        /* Close channel */
        USBH_CloseChannelCallback(usbInfo, cdcInfo->commXfer.notifyChNum);
        USBH_CloseChannelCallback(usbInfo, cdcInfo->dataXfer.inChNum);
        USBH_CloseChannelCallback(usbInfo, cdcInfo->dataXfer.outChNum);
    }

    return USBH_OK;
}

/*********************** USB host CDC request handler ***************************/

/*!
 * @brief     USB host get CDC line coding
 *
 * @param     usbInfo : usb handler information
 *
 * @param     lineCoding : line coding structure
 *
 * @retval    usb host status
 */
static USBH_STA_T CDC_GetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T* lineCoding)
{
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_IN;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_CDC_REQ_GET_LINE_CODING;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = USBH_CDC_LINE_CODING_NUM & 0xFF;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = USBH_CDC_LINE_CODING_NUM >> 8;

    return USBH_REQ_CtrlXferHandler(usbInfo, lineCoding->data, USBH_CDC_LINE_CODING_NUM);
}

/*!
 * @brief     USB host set CDC line coding
 *
 * @param     usbInfo : usb handler information
 *
 * @param     lineCoding : line coding structure
 *
 * @retval    usb host status
 */
static USBH_STA_T CDC_SetLineCoding(USBH_INFO_T* usbInfo, USBH_CDC_LINE_CODING_T* lineCoding)
{
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_CDC_REQ_SET_LINE_CODING;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = USBH_CDC_LINE_CODING_NUM & 0xFF;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = USBH_CDC_LINE_CODING_NUM >> 8;

    return USBH_REQ_CtrlXferHandler(usbInfo, lineCoding->data, USBH_CDC_LINE_CODING_NUM);
}

/*!
 * @brief     USB host set CDC control line state
 *
 * @param     usbInfo : usb handler information
 *
 * @param     controlLine : control line status
 *
 * @retval    usb host status
 */
static USBH_STA_T CDC_SetControlLineState(USBH_INFO_T* usbInfo, USBH_CDC_CONTROL_LINE_STATE_T* controlLine)
{
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_CDC_REQ_SET_CONTROL_LINE_STATE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = controlLine->bitmap;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    return USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);
}

/*********************** USB host CDC transfer data handler ***************************/

/*!
 * @brief       USB host CDC data idle handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_DataIdleHandler(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);

    return USBH_OK;
}

/*!
 * @brief       USB host CDC send data handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_DataSendHandler(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (cdcInfo->txLength > cdcInfo->dataXfer.outEpsize)
    {
        (void)USBH_BulkSendDataReq(usbInfo, cdcInfo->dataXfer.outChNum, \
                                   cdcInfo->txBuffer, \
                                   cdcInfo->dataXfer.outEpsize, \
                                   ENABLE);
    }
    else
    {
        (void)USBH_BulkSendDataReq(usbInfo, cdcInfo->dataXfer.outChNum, \
                                   cdcInfo->txBuffer, \
                                   cdcInfo->txLength, \
                                   ENABLE);
    }

    /* Set CDC state to send data wait */
    cdcInfo->dataTxState = USBH_CDC_DATA_SEND_WAIT;

    return USBH_BUSY;
}

/*!
 * @brief       USB host CDC send data wait handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_DataSendWaitHandler(USBH_INFO_T* usbInfo)
{
    uint8_t reqStatus;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_ReadUrbStatusCallback(usbInfo, cdcInfo->dataXfer.outChNum);

    switch (reqStatus)
    {
        case USB_URB_OK:
            if (cdcInfo->txLength > cdcInfo->dataXfer.outEpsize)
            {
                cdcInfo->txBuffer += cdcInfo->dataXfer.outEpsize;
                cdcInfo->txLength -= cdcInfo->dataXfer.outEpsize;
            }
            else
            {
                cdcInfo->txLength = 0;
            }

            if (cdcInfo->txLength > 0)
            {
                cdcInfo->dataTxState = USBH_CDC_DATA_SEND;
            }
            else
            {
                cdcInfo->dataTxState = USBH_CDC_DATA_TX_IDLE;

                /* Notify user send data finish */
                USBH_CDC_XferEndCallback(usbInfo);
            }
            break;

        case USB_URB_NOREADY:
            /* Set CDC state to send data */
            cdcInfo->dataTxState = USBH_CDC_DATA_SEND;
            break;

        default:
            break;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host CDC receive data handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_DataRevHandler(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    (void)USBH_BulkReceiveDataReq(usbInfo, cdcInfo->dataXfer.inChNum, \
                                  cdcInfo->rxBuffer, \
                                  cdcInfo->dataXfer.inEpsize);

    /* Set CDC state to receive data wait */
    cdcInfo->dataRxState = USBH_CDC_DATA_RECEIVE_WAIT;

    return USBH_BUSY;
}

/*!
 * @brief       USB host CDC receive data wait handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_DataRevWaitHandler(USBH_INFO_T* usbInfo)
{
    uint8_t reqStatus;
    uint32_t length;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_ReadUrbStatusCallback(usbInfo, cdcInfo->dataXfer.inChNum);

    switch (reqStatus)
    {
        case USB_URB_OK:
            length = USBH_ReadLastXferSizeCallback(usbInfo, cdcInfo->dataXfer.inChNum);

            if ((length > cdcInfo->dataXfer.inEpsize) && \
                ((cdcInfo->rxLength - length) > 0))
                {
                    cdcInfo->rxBuffer += length;
                    cdcInfo->rxLength -= length;

                    /* Set CDC state to receive data */
                    cdcInfo->dataRxState = USBH_CDC_DATA_RECEIVE;
                }
                else
                {
                    cdcInfo->dataRxState = USBH_CDC_DATA_RX_IDLE;

                    /* Notify user receive data finish */
                    USBH_CDC_RevEndCallback(usbInfo);
                }
            break;

        default:
            break;
    }

    return USBH_OK;
}

/*********************** USB host CDC state handler ***************************/

/*!
 * @brief       USB host CDC idle handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_IdleHandler(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);

    return USBH_OK;
}

/*!
 * @brief       USB host CDC set line coding handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_SetLineCodingHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = CDC_SetLineCoding(usbInfo, cdcInfo->userLineCoding);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Set CDC state to get line coding */
            cdcInfo->state = USBH_CDC_GET_LINE_CODING_STATE;
            break;

        case USBH_BUSY:
            break;

        default:
            cdcInfo->state = USBH_CDC_ERROR_STATE;
            break;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC get line coding handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_GetLineCodingHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    uint32_t i;
    uint8_t lineCodingStatus = USBH_OK;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = CDC_GetLineCoding(usbInfo, &cdcInfo->lineCoding);

    switch (reqStatus)
    {
        case USBH_OK:
            for (i = 0; i < USBH_CDC_LINE_CODING_NUM; i++)
            {
                if (cdcInfo->lineCoding.data[i] != cdcInfo->userLineCoding->data[i])
                {
                    lineCodingStatus = USBH_FAIL;
                }
            }

            if (lineCodingStatus == USBH_OK)
            {
                /* Notify user line coding is change */
                USBH_CDC_LineCodingIsChangeCallback(usbInfo);
            }

            /* Set CDC state to idle */
            cdcInfo->state = USBH_CDC_IDLE;
            break;

        case USBH_BUSY:
            break;

        default:
            cdcInfo->state = USBH_CDC_ERROR_STATE;
            break;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC set control line state handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_SetControlLineHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = CDC_SetControlLineState(usbInfo, cdcInfo->controlLine);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Set CDC state to idle */
            cdcInfo->state = USBH_CDC_IDLE;
            break;

        case USBH_BUSY:
            break;

        default:
            cdcInfo->state = USBH_CDC_ERROR_STATE;
            break;
    }

    return usbStatus;
}

/*!
 * @brief       USB host CDC transfer data state handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_TransferDataHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    (void)USBH_CDC_TX_DATA_Handler[cdcInfo->dataTxState](usbInfo);
    (void)USBH_CDC_RX_DATA_Handler[cdcInfo->dataRxState](usbInfo);

    return usbStatus;
}

/*!
 * @brief       USB host CDC error state handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_ErrorHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_ClearFeature(usbInfo, 0);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Set CDC state to idle */
            cdcInfo->state = USBH_CDC_IDLE;
            break;

        default:
            break;
    }

    return usbStatus;
}

/*********************** USB host CDC class handler ***************************/

/*!
 * @brief       USB host CDC configuration handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_ClassInitHandler(USBH_INFO_T* usbInfo)
{
    uint8_t itfNum = 0;
    uint8_t epNumMax;
    uint8_t subClass;
    uint8_t itfClass;
    uint8_t itfProtocol;
    uint32_t i;
    USBH_CDC_INFO_T* cdcInfo;

    /* Find the CDC notification interface */
    while (itfNum < INTERFACE_DESC_MAX_NUM)
    {
        itfClass = USBH_ReadInterfaceClass(usbInfo, itfNum);
        subClass = USBH_ReadInterfaceSubClass(usbInfo, itfNum);
        itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, itfNum);

        if ((itfClass == USBH_CLASS_CDCC) && \
            (subClass == USBH_CDC_ACM_CODE) && \
            (itfProtocol == USBH_CDC_AT_COMMAND_CODE))
            {
                break;
            }

        itfNum++;
    }

    if (itfNum >= INTERFACE_DESC_MAX_NUM)
    {
        USBH_USR_DBG("USB host CDC notification interface is not found");
        return USBH_FAIL;
    }

    if (itfNum < USBH_ReadConfigurationItfNum(usbInfo))
    {
        usbInfo->devInfo.curItf = itfNum;
        USBH_USR_DBG("USB host switch to interface %d", itfNum);
        USBH_USR_DBG("USB Class: 0x%02X, SubClass: 0x%02X, Protocol: 0x%02X", \
                    USBH_ReadInterfaceClass(usbInfo, itfNum), \
                    USBH_ReadInterfaceSubClass(usbInfo, itfNum), \
                    USBH_ReadInterfaceProtocol(usbInfo, itfNum));
    }
    else
    {
        USBH_USR_DBG("USB host CDC class not found");
        return USBH_FAIL;
    }

    /* Link class data */
    usbInfo->activeClass->classData = (USBH_CDC_INFO_T*)malloc(sizeof(USBH_CDC_INFO_T));
    cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (cdcInfo == NULL)
    {
        USBH_USR_DBG("USB host CDC class malloc failed");
        return USBH_FAIL;
    }
    else
    {
        (void)memset(cdcInfo, 0, sizeof(USBH_CDC_INFO_T));
    }

    /* Notify channel */
    if ((USBH_ReadEndpointAddress(usbInfo, itfNum, 0) & 0x80) != 0)
    {
        cdcInfo->commXfer.notifyEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, 0);
        cdcInfo->commXfer.notifyEpSize = USBH_ReadEndpointMPS(usbInfo, itfNum, 0);
    }

    cdcInfo->commXfer.notifyChNum = USBH_CH_AllocChannel(usbInfo, cdcInfo->commXfer.notifyEpAddr);

    USBH_OpenChannelCallback(usbInfo, cdcInfo->commXfer.notifyChNum, \
                             cdcInfo->commXfer.notifyEpAddr, \
                             usbInfo->devInfo.address, \
                             usbInfo->devInfo.speed, \
                             EP_TYPE_INTERRUPT, \
                             cdcInfo->commXfer.notifyEpSize);

    USBH_ConfigToggleCallback(usbInfo, cdcInfo->commXfer.notifyChNum, 0);

    itfNum = 0;

    /* Find the CDC data interface */
    while (itfNum < INTERFACE_DESC_MAX_NUM)
    {
        itfClass = USBH_ReadInterfaceClass(usbInfo, itfNum);
        subClass = USBH_ReadInterfaceSubClass(usbInfo, itfNum);
        itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, itfNum);

        if ((itfClass == USBH_CLASS_CDCD) && \
            (subClass == 0) && \
            (itfProtocol == 0))
            {
                break;
            }

        itfNum++;
    }

    if (itfNum >= INTERFACE_DESC_MAX_NUM)
    {
        USBH_USR_DBG("USB host CDC data interface is not found");
        return USBH_FAIL;
    }

    /* Find MSC class endpoint */
    epNumMax = USBH_ReadInterfaceEpNum(usbInfo, itfNum);

    if (epNumMax > ENDPOINT_DESC_MAX_NUM)
    {
        epNumMax = ENDPOINT_DESC_MAX_NUM;
    }

    for (i = 0; i < epNumMax; i++)
    {
        if (USBH_ReadEndpointAddress(usbInfo, itfNum, i) & 0x80)
        {
            cdcInfo->dataXfer.inEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            cdcInfo->dataXfer.inChNum = USBH_CH_AllocChannel(usbInfo, cdcInfo->dataXfer.inEpAddr);
            cdcInfo->dataXfer.inEpsize = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open the new IN channel */
            USBH_OpenChannelCallback(usbInfo, cdcInfo->dataXfer.inChNum, \
                                     cdcInfo->dataXfer.inEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_BULK, \
                                     cdcInfo->dataXfer.inEpsize);

            USBH_ConfigToggleCallback(usbInfo, cdcInfo->dataXfer.inChNum, 0);
        }
        else
        {
            cdcInfo->dataXfer.outEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            cdcInfo->dataXfer.outChNum = USBH_CH_AllocChannel(usbInfo, cdcInfo->dataXfer.outEpAddr);
            cdcInfo->dataXfer.outEpsize = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open the new OUT channel */
            USBH_OpenChannelCallback(usbInfo, cdcInfo->dataXfer.outChNum, \
                                     cdcInfo->dataXfer.outEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_BULK, \
                                     cdcInfo->dataXfer.outEpsize);

            USBH_ConfigToggleCallback(usbInfo, cdcInfo->dataXfer.outChNum, 0);
        }
    }

    /* Set CDC state */
    cdcInfo->state = USBH_CDC_IDLE;
    cdcInfo->dataTxState = USBH_CDC_DATA_TX_IDLE;
    cdcInfo->dataRxState = USBH_CDC_DATA_RX_IDLE;

    return USBH_OK;
}

/*!
 * @brief       USB host CDC class reset handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_ClassDeInitHandler(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    if (cdcInfo->commXfer.notifyChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, cdcInfo->commXfer.notifyChNum);
        USBH_CH_FreeChannel(usbInfo, cdcInfo->commXfer.notifyChNum);
        cdcInfo->commXfer.notifyChNum = 0;
    }

    if (cdcInfo->dataXfer.inChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, cdcInfo->dataXfer.inChNum);
        USBH_CH_FreeChannel(usbInfo, cdcInfo->dataXfer.inChNum);
        cdcInfo->dataXfer.inChNum = 0;
    }

    if (cdcInfo->dataXfer.outChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, cdcInfo->dataXfer.outChNum);
        USBH_CH_FreeChannel(usbInfo, cdcInfo->dataXfer.outChNum);
        cdcInfo->dataXfer.outChNum = 0;
    }

    if (usbInfo->activeClass->classData != NULL)
    {
        free(usbInfo->activeClass->classData);
        usbInfo->activeClass->classData = 0;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host CDC class reguest handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_ClassReqHandler(USBH_INFO_T* usbInfo)
{
    uint8_t reqStatus;
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = CDC_GetLineCoding(usbInfo, &cdcInfo->lineCoding);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Notify user class launched */
            if (usbInfo->userCallback != NULL)
            {
                usbInfo->userCallback(usbInfo, USBH_USER_CLASS_LAUNCHED);
            }
            break;

        case USBH_ERR_NOT_SUP:
            USBH_USR_DBG("USB host CDC get line coding request failed");
            break;

        default:
            break;
    }

    return (USBH_STA_T)reqStatus;
}

/*!
 * @brief       USB host CDC SOF handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_SOFHandler(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);

    return USBH_OK;
}

/*!
 * @brief       USB host CDC handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_CDC_CoreHandler(USBH_INFO_T* usbInfo)
{
    USBH_CDC_INFO_T* cdcInfo = (USBH_CDC_INFO_T*)usbInfo->activeClass->classData;

    return USBH_CDC_Handler[cdcInfo->state](usbInfo);
}

/*!
 * @brief       USB host CDC send data finish callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
__weak void USBH_CDC_XferEndCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief       USB host CDC receive data finish callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
__weak void USBH_CDC_RevEndCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief       USB host CDC line coding status is change callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      None
 */
__weak void USBH_CDC_LineCodingIsChangeCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/**@} end of group USBH_CDC_Functions */
/**@} end of group USBH_CDC_Class */
/**@} end of group APM32_USB_Library */
