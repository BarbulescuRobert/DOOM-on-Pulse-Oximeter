/*!
 * @file        usbh_msc_bot.c
 *
 * @brief       USB host MSC bot
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
#include "usbh_msc_bot.h"
#include "usbh_msc.h"
#include "usbh_dataXfer.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_MSC_Class
  @{
  */

/** @defgroup USBH_MSC_Functions Functions
  @{
  */

static USBH_BOT_CSW_STA_T USBH_MSC_BOT_DecodeCSW(USBH_INFO_T* usbInfo);

/*!
 * @brief     Init BOT of USB host MSC
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    USB host operation status
 */
USBH_STA_T USBH_MSC_BOT_Init(USBH_INFO_T* usbInfo)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    /* Initialize BOT information */
    mscInfo->botInfo.cbw.DATA_FIELD.dSignature = USBH_MSC_BOT_CBW_SIGNATURE;
    mscInfo->botInfo.cbw.DATA_FIELD.dTag = USBH_MSC_BOT_CBW_TAG;

    /* Set BOT state */
    mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

    /* Set BOT transfer state */
    mscInfo->botInfo.xferState = USBH_BOT_XFER_START;

    return USBH_OK;
}

/*!
 * @brief     Reset request of MSC BOT
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    USB host operation status
 */
USBH_STA_T USBH_MSC_BOT_REQ_Reset(USBH_INFO_T* usbInfo)
{
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = USBH_REQ_TYPE_CLASS;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_CLASS_BOT_RESET;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 0x00;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0x00;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0x00;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0x00;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0x00;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0x00;

    return USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);
}

/*!
 * @brief     Get the max lun of MSC BOT
 *
 * @param     usbInfo : usb handler information
 *
 * @param     maxLun: max of logic unit number
 *
 * @retval    USB host operation status
 */
USBH_STA_T USBH_MSC_BOT_REQ_GetMaxLun(USBH_INFO_T* usbInfo, uint8_t* maxLun)
{
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_IN;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_CLASS_GET_MAX_LUN;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 1;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    return USBH_REQ_CtrlXferHandler(usbInfo, maxLun, 1);
}

/*!
 * @brief     USB host MSC BOT state handler
 *
 * @param     usbInfo : usb handler information
 *
 * @param     lun : logical unit numer
 *
 * @retval    USB host operation status
 */
USBH_STA_T USBH_MSC_BOT_StateHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t usbUrbStatus = USB_URB_IDLE;
    uint8_t reqStatus;
    uint8_t toggle;
    USBH_BOT_CSW_STA_T cswStatus = USBH_BOT_CSW_FAIL;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    switch (mscInfo->botInfo.state)
    {
        case USBH_BOT_SEND_CBW:
            mscInfo->botInfo.cbw.DATA_FIELD.bLUN = lun;

            /* Send CBW */
            (void)USBH_BulkSendDataReq(usbInfo, \
                                       mscInfo->outChNum, \
                                       mscInfo->botInfo.cbw.buffer, \
                                       USBH_MSC_BOT_CBW_LEN, \
                                       ENABLE);

            /* Set BOT state to wait for CBW send */
            mscInfo->botInfo.state = USBH_BOT_SEND_CBW_WAIT;
            break;

        case USBH_BOT_SEND_CBW_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, mscInfo->outChNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    if (mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen == 0)
                    {
                        /* Set BOT state to receive CSW */
                        mscInfo->botInfo.state = USBH_BOT_RECEIVE_CSW;
                    }
                    else
                    {
                        if (mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir == USBH_REQ_DIR_IN)
                        {
                            /* Set BOT state to data in */
                            mscInfo->botInfo.state = USBH_BOT_DATA_IN;
                        }
                        else
                        {
                            /* Set BOT state to data out */
                            mscInfo->botInfo.state = USBH_BOT_DATA_OUT;
                        }
                    }
                    break;

                case USB_URB_NOREADY:
                    /* Set BOT state to send CBW */
                    mscInfo->botInfo.state = USBH_BOT_SEND_CBW;
                    break;

                case USB_URB_STALL:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_OUT;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_DATA_IN:
            /* Receive data */
            (void)USBH_BulkReceiveDataReq(usbInfo, \
                                          mscInfo->inChNum, \
                                          mscInfo->botInfo.buffer, \
                                          mscInfo->bulkInEpSize);

            /* Set BOT state to wait for data in */
            mscInfo->botInfo.state = USBH_BOT_DATA_IN_WAIT;
            break;

        case USBH_BOT_DATA_IN_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, mscInfo->inChNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    if (mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen <= mscInfo->bulkInEpSize)
                    {
                        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = 0;
                    }
                    else
                    {
                        mscInfo->botInfo.buffer += mscInfo->bulkInEpSize;
                        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen -= mscInfo->bulkInEpSize;
                    }

                    if (mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen > 0)
                    {
                        /* Receive next data */
                        (void)USBH_BulkReceiveDataReq(usbInfo, \
                                                      mscInfo->inChNum, \
                                                      mscInfo->botInfo.buffer, \
                                                      mscInfo->bulkInEpSize);
                    }
                    else
                    {
                        /* Set BOT state to receive CSW */
                        mscInfo->botInfo.state = USBH_BOT_RECEIVE_CSW;
                    }
                    break;

                case USB_URB_STALL:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_IN;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_DATA_OUT:
            /* Send data */
            (void)USBH_BulkSendDataReq(usbInfo, \
                                       mscInfo->outChNum, \
                                       mscInfo->botInfo.buffer, \
                                       mscInfo->bulkOutEpSize, \
                                       ENABLE);

            /* Set BOT state to wait for data out */
            mscInfo->botInfo.state = USBH_BOT_DATA_OUT_WAIT;
            break;

        case USBH_BOT_DATA_OUT_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, mscInfo->outChNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    if (mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen <= mscInfo->bulkOutEpSize)
                    {
                        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = 0;
                    }
                    else
                    {
                        mscInfo->botInfo.buffer += mscInfo->bulkOutEpSize;
                        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen -= mscInfo->bulkOutEpSize;
                    }

                    if (mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen > 0)
                    {
                        /* Send next data */
                        (void)USBH_BulkSendDataReq(usbInfo, \
                                                   mscInfo->outChNum, \
                                                   mscInfo->botInfo.buffer, \
                                                   mscInfo->bulkOutEpSize, \
                                                   ENABLE);
                    }
                    else
                    {
                        /* Set BOT state to receive CSW */
                        mscInfo->botInfo.state = USBH_BOT_RECEIVE_CSW;
                    }
                    break;

                case USB_URB_NOREADY:
                    /* Set BOT state to send data out */
                    mscInfo->botInfo.state = USBH_BOT_DATA_OUT;
                    break;

                case USB_URB_STALL:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_OUT;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_RECEIVE_CSW:
            /* Receive CSW */
            (void)USBH_BulkReceiveDataReq(usbInfo, \
                                          mscInfo->inChNum, \
                                          mscInfo->botInfo.csw.buffer, \
                                          USBH_MSC_BOT_CSW_LEN);

            /* Set BOT state to wait for CSW */
            mscInfo->botInfo.state = USBH_BOT_RECEIVE_CSW_WAIT;
            break;

        case USBH_BOT_RECEIVE_CSW_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, mscInfo->inChNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Set BOT state to send CBW */
                    mscInfo->botInfo.state = USBH_BOT_SEND_CBW;
                    mscInfo->botInfo.xferState = USBH_BOT_XFER_START;

                    /* Decode CSW */
                    cswStatus = USBH_MSC_BOT_DecodeCSW(usbInfo);
                    if (cswStatus == USBH_BOT_CSW_OK)
                    {
                        usbStatus = USBH_OK;
                    }
                    else
                    {
                        usbStatus = USBH_FAIL;
                    }
                    break;

                case USB_URB_STALL:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_IN;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_ERROR_IN:
            reqStatus = USBH_ClearFeature(usbInfo, mscInfo->bulkInEpAddr);

            switch (reqStatus)
            {
                case USBH_OK:
                    /* Set BOT state to receive CSW */
                    mscInfo->botInfo.state = USBH_BOT_RECEIVE_CSW;
                    break;

                case USBH_ERR_UNRECOVERED:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_UNRECOVERED;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_ERROR_OUT:
            reqStatus = USBH_ClearFeature(usbInfo, mscInfo->bulkOutEpAddr);

            switch (reqStatus)
            {
                case USBH_OK:
                    toggle = USBH_ReadToggleCallback(usbInfo, mscInfo->outChNum);

                    USBH_ConfigToggleCallback(usbInfo, mscInfo->outChNum, 1- toggle);
                    USBH_ConfigToggleCallback(usbInfo, mscInfo->inChNum, 0);

                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_IN;
                    break;

                case USBH_ERR_UNRECOVERED:
                    /* Set BOT state to error */
                    mscInfo->botInfo.state = USBH_BOT_ERROR_UNRECOVERED;
                    break;

                default:
                    break;
            }
            break;

        case USBH_BOT_ERROR_UNRECOVERED:
            reqStatus = USBH_MSC_BOT_REQ_Reset(usbInfo);

            switch (reqStatus)
            {
                case USBH_OK:
                    /* Set BOT state to send CBW */
                    mscInfo->botInfo.state = USBH_BOT_SEND_CBW;
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return usbStatus;
}

/*!
 * @brief     USB host MSC BOT send CBW handler
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    CSW decode status
 */
static USBH_BOT_CSW_STA_T USBH_MSC_BOT_DecodeCSW(USBH_INFO_T* usbInfo)
{
    USBH_BOT_CSW_STA_T cswStatus = USBH_BOT_CSW_FAIL;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    uint32_t lastXferSize;

    lastXferSize = USBH_ReadLastXferSizeCallback(usbInfo, mscInfo->inChNum);

    if (lastXferSize == USBH_MSC_BOT_CSW_LEN)
    {
        if (mscInfo->botInfo.csw.DATA_FIELD.dSignature == USBH_MSC_BOT_CSW_SIGNATURE)
        {
            if (mscInfo->botInfo.csw.DATA_FIELD.dTag == mscInfo->botInfo.cbw.DATA_FIELD.dTag)
            {
                switch (mscInfo->botInfo.csw.DATA_FIELD.bStatus)
                {
                    case USBH_BOT_CSW_OK:
                        cswStatus = USBH_BOT_CSW_OK;
                        break;

                    case USBH_BOT_CSW_FAIL:
                        cswStatus = USBH_BOT_CSW_FAIL;
                        break;

                    case USBH_BOT_CSW_ERR:
                        cswStatus = USBH_BOT_CSW_ERR;
                        break;

                    default:
                        break;
                }
            }
            else
            {
                cswStatus = USBH_BOT_CSW_ERR;
            }
        }
        else
        {
            cswStatus = USBH_BOT_CSW_ERR;
        }
    }
    else
    {
        cswStatus = USBH_BOT_CSW_ERR;
    }

    return cswStatus;
}

/**@} end of group USBH_MSC_Functions */
/**@} end of group USBH_MSC_Class */
/**@} end of group APM32_USB_Library */
