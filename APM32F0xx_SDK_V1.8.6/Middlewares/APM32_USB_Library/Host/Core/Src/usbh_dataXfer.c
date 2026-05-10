/*!
 * @file        usbh_dataXfer.c
 *
 * @brief       USB host input and output hander function
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
#include "usbh_dataXfer.h"
#include "usbh_core.h"
#include "usbh_stdReq.h"
#include "usbh_channel.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_Core
  @{
  */

/** @defgroup USBH_Core_Functions Functions
  @{
  */

/*********************** Control transfer request function ************************/

/*!
 * @brief     Control transfer send data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @param     pingStatus : ping status
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_CtrlSendDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                            uint8_t* buffer, uint16_t length, \
                            uint8_t pingStatus)
{
    USBH_STA_T usbStatus = USBH_OK;

    if ((usbInfo->devInfo.speed != USBH_DEVICE_SPEED_HS) && \
        (pingStatus == ENABLE))
        {
            pingStatus = DISABLE;
        }

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_OUT, \
                           EP_TYPE_CONTROL, \
                           USBH_PID_DATA, \
                           buffer, \
                           length, \
                           pingStatus);

    return usbStatus;
}

/*!
 * @brief     Control transfer receive data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_CtrlReceiveDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                                      uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_IN, \
                           EP_TYPE_CONTROL, \
                           USBH_PID_DATA, \
                           buffer, \
                           length, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Start control setup transfer
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be sent
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_CtrlSendSetupReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                             uint8_t* buffer)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_OUT, \
                           EP_TYPE_CONTROL, \
                           USBH_PID_SETUP, \
                           buffer, \
                           USBH_SETUP_PACKET_SIZE, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Bulk transfer send data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @param     pingStatus : ping status
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_BulkSendDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                            uint8_t* buffer, uint16_t length, \
                            uint8_t pingStatus)
{
    USBH_STA_T usbStatus = USBH_OK;

    if ((usbInfo->devInfo.speed != USBH_DEVICE_SPEED_HS) && \
        (pingStatus == ENABLE))
        {
            pingStatus = DISABLE;
        }

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_OUT, \
                           EP_TYPE_BULK, \
                           USBH_PID_DATA, \
                           buffer, \
                           length, \
                           pingStatus);

    return usbStatus;
}

/*!
 * @brief     Bulk transfer receive data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_BulkReceiveDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                               uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_IN, \
                           EP_TYPE_BULK, \
                           USBH_PID_DATA, \
                           buffer, \
                           length, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Interrupt transfer receive data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_IntReceiveDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                              uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_IN, \
                           EP_TYPE_INTERRUPT, \
                           USBH_PID_DATA, \
                           buffer, \
                           (uint16_t)length, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Interrupt transfer send data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_IntSendDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                           uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_OUT, \
                           EP_TYPE_INTERRUPT, \
                           USBH_PID_DATA, \
                           buffer, \
                           (uint16_t)length, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Isochronous transfer receive data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_IsoReceiveDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                              uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_IN, \
                           EP_TYPE_ISO, \
                           USBH_PID_DATA, \
                           buffer, \
                           (uint16_t)length, \
                           DISABLE);

    return usbStatus;
}

/*!
 * @brief     Isochronous transfer send data request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @param     buffer : buffer to be receive data
 *
 * @param     length : transfer length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_IsoSendDataReq(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                            uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus = USBH_OK;

    /* Submit URB */
    USBH_UrbSubmitCallback(usbInfo, channelNum, \
                           EP_DIR_OUT, \
                           EP_TYPE_ISO, \
                           USBH_PID_DATA, \
                           buffer, \
                           (uint16_t)length, \
                           DISABLE);

    return usbStatus;
}

/*********************** Control transfer state handler function ************************/

/*!
 * @brief     Handle control transfer state
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_CtrlXferHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t usbUrbStatus = USB_URB_IDLE;
    static uint16_t reqLenght;
    static uint8_t reqDirTemp;

    switch (usbInfo->ctrlInfo.state)
    {
        case USBH_CTRL_IDLE:
            break;

        case USBH_CTRL_SETUP:
            /* Send setup request packet */
            USBH_CtrlSendSetupReq(usbInfo, \
                                usbInfo->ctrlInfo.channelOutNum, \
                                (uint8_t *)(void *)usbInfo->ctrlInfo.steupReq.data);

            /* Change control state to setup wait */
            usbInfo->ctrlInfo.state = USBH_CTRL_SETUP_WAIT;
            break;

        case USBH_CTRL_SETUP_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, \
                                                    usbInfo->ctrlInfo.channelOutNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Get request direction */
                    reqDirTemp = usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir;
                    /* Get request length */
                    reqLenght = (uint16_t)(usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] << 8 | \
                                usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0]);

                    /* Check data state */
                    if (reqLenght != 0)
                    {
                        if (reqDirTemp == USB_REQ_DIR_OUT)
                        {
                            /* Change control state to OUT data */
                            usbInfo->ctrlInfo.state = USBH_CTRL_DATA_OUT;
                        }
                        else
                        {
                            /* Change control state to IN data */
                            usbInfo->ctrlInfo.state = USBH_CTRL_DATA_IN;
                        }
                    }
                    else
                    {
                        if (reqDirTemp == USB_REQ_DIR_OUT)
                        {
                            /* Change control state to IN status */
                            usbInfo->ctrlInfo.state = USBH_CTRL_STA_IN;
                        }
                        else
                        {
                            /* Change control state to OUT status */
                            usbInfo->ctrlInfo.state = USBH_CTRL_STA_OUT;
                        }
                    }
                    break;

                case USB_URB_ERROR:
                case USB_URB_NOREADY:
                    /* Change control state to error */
                    usbInfo->ctrlInfo.state = USBH_CTRL_ERROR;
                    break;

                default:
                    break;
            }
            break;

        case USBH_CTRL_DATA_IN:
            /* Receive data */
            USBH_CtrlReceiveDataReq(usbInfo, \
                                    usbInfo->ctrlInfo.channelInNum, \
                                    usbInfo->ctrlInfo.buffer, \
                                    usbInfo->ctrlInfo.length);

            /* Change control state to IN data wait */
            usbInfo->ctrlInfo.state = USBH_CTRL_DATA_IN_WAIT;
            break;

        case USBH_CTRL_DATA_IN_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, \
                                                    usbInfo->ctrlInfo.channelInNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Change control state to OUT status */
                    usbInfo->ctrlInfo.state = USBH_CTRL_STA_OUT;
                    break;

                case USB_URB_STALL:
                    usbStatus = USBH_ERR_NOT_SUP;
                    break;

                case USB_URB_ERROR:
                    /* Change control state to error */
                    usbInfo->ctrlInfo.state = USBH_CTRL_ERROR;
                    break;

                default:
                    break;
            }
            break;

        case USBH_CTRL_DATA_OUT:
            /* Send data */
            USBH_CtrlSendDataReq(usbInfo, \
                                usbInfo->ctrlInfo.channelOutNum, \
                                usbInfo->ctrlInfo.buffer, \
                                usbInfo->ctrlInfo.length, \
                                ENABLE);

            /* Change control state to OUT data wait */
            usbInfo->ctrlInfo.state = USBH_CTRL_DATA_OUT_WAIT;
            break;

        case USBH_CTRL_DATA_OUT_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, \
                                                    usbInfo->ctrlInfo.channelOutNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Change control state to IN status */
                    usbInfo->ctrlInfo.state = USBH_CTRL_STA_IN;
                    break;

                case USB_URB_STALL:
                    /* Change control state to STALL */
                    usbInfo->ctrlInfo.state = USBH_CTRL_STALL;
                    usbStatus = USBH_ERR_NOT_SUP;
                    break;

                case USB_URB_NOREADY:
                    /* Change control state to OUT data */
                    usbInfo->ctrlInfo.state = USBH_CTRL_DATA_OUT;
                    break;

                case USB_URB_ERROR:
                    /* Change control state to error */
                    usbInfo->ctrlInfo.state = USBH_CTRL_ERROR;
                    usbStatus = USBH_FAIL;
                    break;

                default:
                    break;
            }
            break;

        case USBH_CTRL_STA_IN:
            /* Receive 0 length status packet */
            USBH_CtrlReceiveDataReq(usbInfo, \
                                    usbInfo->ctrlInfo.channelInNum, \
                                    NULL, \
                                    0);

            /* Change control state to IN status wait */
            usbInfo->ctrlInfo.state = USBH_CTRL_STA_IN_WAIT;
            break;

        case USBH_CTRL_STA_IN_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, \
                                                    usbInfo->ctrlInfo.channelInNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Change control state to OK */
                    usbInfo->ctrlInfo.state = USBH_CTRL_OK;
                    usbStatus = USBH_OK;
                    break;

                case USB_URB_STALL:
                    usbStatus = USBH_ERR_NOT_SUP;
                    break;

                case USB_URB_ERROR:
                    /* Change control state to error */
                    usbInfo->ctrlInfo.state = USBH_CTRL_ERROR;
                    break;

                default:
                    break;
            }
            break;

        case USBH_CTRL_STA_OUT:
            /* Send 0 length status packet */
            USBH_CtrlSendDataReq(usbInfo, \
                                usbInfo->ctrlInfo.channelOutNum, \
                                NULL, \
                                0, \
                                ENABLE);

            /* Change control state to OUT status wait */
            usbInfo->ctrlInfo.state = USBH_CTRL_STA_OUT_WAIT;
            break;

        case USBH_CTRL_STA_OUT_WAIT:
            /* Get URB status */
            usbUrbStatus = USBH_ReadUrbStatusCallback(usbInfo, \
                                                    usbInfo->ctrlInfo.channelOutNum);

            switch (usbUrbStatus)
            {
                case USB_URB_OK:
                    /* Change control state to OK */
                    usbInfo->ctrlInfo.state = USBH_CTRL_OK;
                    usbStatus = USBH_OK;
                    break;

                case USB_URB_NOREADY:
                    /* Change control state to OUT status */
                    usbInfo->ctrlInfo.state = USBH_CTRL_STA_OUT;
                    break;

                case USB_URB_ERROR:
                    /* Change control state to error */
                    usbInfo->ctrlInfo.state = USBH_CTRL_ERROR;
                    break;

                default:
                    break;
            }
            break;

        case USBH_CTRL_STALL:
            break;

        case USBH_CTRL_OK:
            break;

        case USBH_CTRL_ERROR:
            /**
            If the next Setup PID is not accepted, then the device needs 
            to be reset to clear the halt or error state.
            */
            usbInfo->ctrlInfo.errCnt++;

            if (usbInfo->ctrlInfo.errCnt > USBH_MAX_ERROR)
            {
                usbInfo->ctrlInfo.errCnt = 0;

                if (usbInfo->userCallback != NULL)
                {
                    usbInfo->userCallback(usbInfo, USBH_USER_ERROR);
                }

                USBH_USR_LOG("Control transfer error, device is not responding");

                /* Free channel */
                USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
                USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);

                /* Set host state to idle */
                usbInfo->hostState = USBH_HOST_IDLE;
                usbStatus = USBH_FAIL;
            }
            else
            {
                /* Retry control transfer */
                usbInfo->ctrlInfo.state = USBH_CTRL_SETUP;
                usbInfo->xferState = USBH_XFER_START;
            }
            break;

        default:
            break;
    }

    return usbStatus;
}

/**@} end of group USBH_Core_Functions */
/**@} end of group USBH_Core */
/**@} end of group APM32_USB_Library */
