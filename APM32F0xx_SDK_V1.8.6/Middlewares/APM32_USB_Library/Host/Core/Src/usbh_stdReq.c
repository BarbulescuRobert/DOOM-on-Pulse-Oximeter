/*!
 * @file        usbh_stdReq.c
 *
 * @brief       USB standard request process
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
#include "usbh_stdReq.h"
#include "usbh_dataXfer.h"
#include "usbh_core.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_Core
  @{
  */

/** @defgroup USBH_Core_Functions Functions
  @{
  */

static USBH_STA_T USBH_REQ_SetAddr(USBH_INFO_T* usbInfo, uint8_t reqType, uint8_t addr);
static USBH_STA_T USBH_REQ_SetConfiguration(USBH_INFO_T* usbInfo, uint8_t reqType, uint16_t configuration);
static USBH_STA_T USBH_REQ_SetInterface(USBH_INFO_T* usbInfo, uint8_t reqType, uint8_t epNum, uint8_t interface);
static USBH_STA_T USBH_REQ_ClearFeature(USBH_INFO_T* usbInfo, uint8_t reqType, uint8_t epNum);
static USBH_STA_T USBH_REQ_SetFeature(USBH_INFO_T* usbInfo, uint8_t reqType, uint8_t feature);

static USBH_STA_T USBH_DevDescParse(USBH_INFO_T* usbInfo, uint8_t* buffer, uint16_t length);
static USBH_STA_T USBH_CfgDescParse(USBH_INFO_T* usbInfo, uint8_t* buffer, uint16_t length);
static USBH_STA_T USBH_StringDescParse(uint8_t* stringDesc, uint8_t* buffer, uint16_t length);

static void USBH_CopyBuffer(uint8_t* desBuffer, uint8_t* srcBuf, uint32_t len);

/*********************** USB host standard request ******************************/

/*!
 * @brief     USB host get device description
 *
 * @param     usbInfo : usb handler information
 *
 * @param     desLength : length of description
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_GetDevDesc(USBH_INFO_T* usbInfo, uint16_t desLength)
{
    USBH_STA_T usbStatus;

    if (desLength > sizeof(usbInfo->devInfo.data))
    {
        USBH_USR_DBG("USBH_GetDevDesc: descriptor length is longer than buffer size");
        return USBH_ERR_NOT_SUP;
    }

    usbStatus = USBH_REQ_GetDescriptor(usbInfo, ((USBH_REQ_DIR_IN << 7) | \
                                       (USBH_REQ_TYPE_STANDARD << 5) | \
                                       (USBH_RECIPIENT_DEVICE)), \
                                       USBH_DESC_DEVICE,
                                       usbInfo->devInfo.data,
                                       desLength);

    if (usbStatus == USBH_OK)
    {
        /* Parse device descriptor */
        usbStatus = USBH_DevDescParse(usbInfo, usbInfo->devInfo.data, desLength);
    }

    return usbStatus;
}

/*!
 * @brief     USB host get string description
 *
 * @param     usbInfo : usb handler information
 *
 * @param     stringIndex : string index
 *
 * @param     buffer : buffer to store descriptor
 *
 * @param     desLength : length of description
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_GetStringDesc(USBH_INFO_T* usbInfo, uint8_t stringIndex, uint8_t* buffer, uint16_t desLength)
{
    USBH_STA_T usbStatus;

    if ((desLength > sizeof(usbInfo->devInfo.data)) || \
        (buffer == NULL))
    {
        USBH_USR_DBG("USBH_GetStringDesc: descriptor length is longer than buffer size");
        return USBH_ERR_NOT_SUP;
    }

    usbStatus = USBH_REQ_GetDescriptor(usbInfo, ((USBH_REQ_DIR_IN << 7) | \
                                       (USBH_REQ_TYPE_STANDARD << 5) | \
                                       (USBH_RECIPIENT_DEVICE)), \
                                       (USBH_DESC_STRING | (stringIndex << 8)),
                                       usbInfo->devInfo.data,
                                       desLength);

    if (usbStatus == USBH_OK)
    {
        /* Parse string descriptor */
        usbStatus = USBH_StringDescParse(usbInfo->devInfo.data, buffer, desLength);
    }

    return usbStatus;
}

/*!
 * @brief     USB host get configuration description
 *
 * @param     usbInfo : usb handler information
 *
 * @param     desLength : length of description
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_GetCfgDesc(USBH_INFO_T* usbInfo, uint16_t desLength)
{
    USBH_STA_T usbStatus;

    if (desLength > sizeof(usbInfo->devInfo.cfgDescBuf))
    {
        USBH_USR_DBG("USBH_GetCfgDesc: descriptor length is longer than buffer size");
        return USBH_ERR_NOT_SUP;
    }

    usbStatus = USBH_REQ_GetDescriptor(usbInfo, ((USBH_REQ_DIR_IN << 7) | \
                                       (USBH_REQ_TYPE_STANDARD << 5) | \
                                       (USBH_RECIPIENT_DEVICE)), \
                                       USBH_DESC_CONFIGURATION,
                                       usbInfo->devInfo.cfgDescBuf,
                                       desLength);

    if (usbStatus == USBH_OK)
    {
        /* Parse configuration descriptor */
        usbStatus = USBH_CfgDescParse(usbInfo,\
                                      usbInfo->devInfo.cfgDescBuf, \
                                      desLength);
    }

    return usbStatus;   
}

/*!
 * @brief     USB host set device address
 *
 * @param     usbInfo : usb handler information
 *
 * @param     address : device address
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_SetAddress(USBH_INFO_T* usbInfo, uint8_t address)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_SetAddr(usbInfo, ((USBH_REQ_DIR_OUT << 7) | \
                                 (USBH_REQ_TYPE_STANDARD << 5) | \
                                 (USBH_RECIPIENT_DEVICE)), \
                                 address);

    return usbStatus;
}

/*!
 * @brief     USB host set configuration
 *
 * @param     usbInfo : usb handler information
 *
 * @param     configuration : value of configuration
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_SetConfiguration(USBH_INFO_T* usbInfo, uint16_t configuration)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_SetConfiguration(usbInfo, ((USBH_REQ_DIR_OUT << 7) | \
                                          (USBH_REQ_TYPE_STANDARD << 5) | \
                                          (USBH_RECIPIENT_DEVICE)), \
                                          configuration);

    return usbStatus;
}

/*!
 * @brief     USB host set interface
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epNum : number of endpoint
 * 
 * @param     interface : value of interface
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_SetInterface(USBH_INFO_T* usbInfo, uint8_t epNum, uint8_t interface)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_SetInterface(usbInfo, ((USBH_REQ_DIR_OUT << 7) | \
                                      (USBH_REQ_TYPE_STANDARD << 5) | \
                                      (USBH_RECIPIENT_INTERFACE)), \
                                      epNum, \
                                      interface);

    return usbStatus;
}

/*!
 * @brief     USB host set feature
 *
 * @param     usbInfo : usb handler information
 *
 * @param     feature : value of feature
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_SetFeature(USBH_INFO_T* usbInfo, uint8_t feature)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_SetFeature(usbInfo, ((USBH_REQ_DIR_OUT << 7) | \
                                    (USBH_REQ_TYPE_STANDARD << 5) | \
                                    (USBH_RECIPIENT_DEVICE)), \
                                    feature);

    return usbStatus;
}

/*!
 * @brief     USB host clear feature
 *
 * @param     usbInfo : usb handler information
 *
 * @param     epNum : number of endpoint
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_ClearFeature(USBH_INFO_T* usbInfo, uint8_t epNum)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_ClearFeature(usbInfo, ((USBH_REQ_DIR_OUT << 7) | \
                                      (USBH_REQ_TYPE_STANDARD << 5) | \
                                      (USBH_RECIPIENT_ENDPOINT)), \
                                      epNum);

    return usbStatus;
}

/*!
 * @brief     Config standard get descriptor request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type
 *
 * @param     desType : Specifies descriptor type
 *                  This value can be one of the following values:
 *                  @arg USBH_DESC_DEVICE
 *                  @arg USBH_DESC_CONFIGURATION
 *                  @arg USBH_DESC_STRING
 *                  @arg USBH_DESC_INTERFACE
 *                  @arg USBH_DESC_ENDPOINT
 *                  @arg USBH_DESC_DEVICE_QUALIFIER
 *                  @arg USBH_DESC_OTHER_SPEED
 *                  @arg USBH_INTERFACE_POWER
 *
 * @param     buffer : buffer to store descriptor
 *
 * @param     length : Specifies len of request
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_REQ_GetDescriptor(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                  uint16_t desType, uint8_t* buffer, uint16_t length)
{
    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_GET_DESCRIPTOR;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = (desType >> 8) & 0xFF;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = desType & 0xFF;

        if ((desType & 0xFF) == USBH_DESC_STRING)
        {
            usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = USBH_LANG_ID & 0xFF;
            usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = USBH_LANG_ID >> 8;
        }
        else
        {
            usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
            usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;
        }

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = length & 0xFF;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = length >> 8;
    }

    /* Control transfer handler */
    return USBH_REQ_CtrlXferHandler(usbInfo, buffer, length);
}

/*!
 * @brief     Handle control setup transfer.
 *
 * @param     usbInfo : usb handler information
 *
 * @param     buffer : control transfer buffer
 *
 * @param     length : length of response
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_REQ_CtrlXferHandler(USBH_INFO_T* usbInfo, uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_BUSY;

    switch (usbInfo->xferState)
    {
        case USBH_XFER_START:
            /* Set control transfer buffer and length */
            usbInfo->ctrlInfo.buffer = buffer;
            usbInfo->ctrlInfo.length = length;

            /* Set control transfer state to setup */
            usbInfo->ctrlInfo.state  = USBH_CTRL_SETUP;

            /* Set control transfer state to waiting */
            usbInfo->xferState = USBH_XFER_WAITING;
            break;

        case USBH_XFER_WAITING:
            /* Control transfer handler */
            usbStatus = USBH_CtrlXferHandler(usbInfo);

            if ((usbStatus == USBH_OK) || (usbStatus == USBH_ERR_NOT_SUP))
            {
                usbInfo->xferState = USBH_XFER_START;

                /* Set control transfer state to idle */
                usbInfo->ctrlInfo.state = USBH_CTRL_IDLE;
            }
            else if (usbStatus == USBH_FAIL)
            {
                usbInfo->xferState = USBH_XFER_START;
            }
            else
            {

            }
            break;

        default:
            break;

    }

    return usbStatus;
}

/*********************** USB host control request *******************************/

/*!
 * @brief     USB host set device address request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type.
 *
 * @param     addr : device address
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_REQ_SetAddr(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                   uint8_t addr)
{
    USBH_STA_T usbStatus;

    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_SET_ADDRESS;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = addr;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;
    }

    /* Control transfer handler */
    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host set device configuration request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type.
 *
 * @param     configuration : configuration value
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_REQ_SetConfiguration(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                            uint16_t configuration)
{
    USBH_STA_T usbStatus;

    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_SET_CONFIGURATION;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = configuration & 0xFF;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = configuration >> 8;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;
    }

    /* Control transfer handler */
    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host set device interface request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type.
 *
 * @param     epNum : number of endpoint
 * 
 * @param     interface : value of interface
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_REQ_SetInterface(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                       uint8_t epNum, uint8_t interface)
{
    USBH_STA_T usbStatus;

    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_SET_INTERFACE;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = interface;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = epNum;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;
    }

    /* Control transfer handler */
    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host clear device feature request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type.
 *
 * @param     feature : feature value
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_REQ_ClearFeature(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                        uint8_t epNum)
{
    USBH_STA_T usbStatus;

    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_CLEAR_FEATURE;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = USBH_FEATURE_SELECTOR_ENDPOINT_HALT;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = epNum;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;
    }

    /* Control transfer handler */
    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host set device feature request
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reqType : Select request type.
 *
 * @param     feature : feature value
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_REQ_SetFeature(USBH_INFO_T* usbInfo, uint8_t reqType, \
                                      uint8_t feature)
{
    USBH_STA_T usbStatus;

    if (usbInfo->xferState == USBH_XFER_START)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir = ((reqType & 0x80) >> 7);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type = ((reqType & 0x60) >> 5);
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = (reqType & 0x1F);

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_STD_SET_FEATURE;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = feature;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;
    }

    /* Control transfer handler */
    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*********************** USB host descriptor phase ******************************/

/*!
 * @brief     Parse device descriptor
 *
 * @param     usbInfo : usb handler information
 *
 * @param     buffer : source recevice data
 *
 * @param     length : device descriptor length
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_DevDescParse(USBH_INFO_T* usbInfo, uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_OK;
    USBH_DEV_DESC_T* devDesc = &usbInfo->devInfo.device;

    /* Make sure the buffer is not NULL */
    if (buffer == NULL)
    {
        USBH_USR_DBG("USBH_DevDescParse: buffer is NULL");
        return USBH_FAIL;
    }

    /* Phase device descriptor */
    devDesc->bLength            = buffer[0];
    devDesc->bDescriptorType    = buffer[1];
    devDesc->bcdUSB[0]          = buffer[2];
    devDesc->bcdUSB[1]          = buffer[3];
    devDesc->bDeviceClass       = buffer[4];
    devDesc->bDeviceSubClass    = buffer[5];
    devDesc->bDeviceProtocol    = buffer[6];
    devDesc->bMaxPacketSize0    = buffer[7];

    if ((usbInfo->devInfo.speed == (uint8_t)USBH_DEVICE_SPEED_HS) || \
        (usbInfo->devInfo.speed == (uint8_t)USBH_DEVICE_SPEED_FS))
    {
        /* Make sure the max packet size is valid */
        switch (devDesc->bMaxPacketSize0)
        {
            case 8:
            case 16:
            case 32:
            case 64:
                break;

            default:
                devDesc->bMaxPacketSize0 = 8;
                break;
        }
    }
    else if (usbInfo->devInfo.speed == (uint8_t)USBH_DEVICE_SPEED_LS)
    {
        /* Make sure the max packet size is valid */
        if (devDesc->bMaxPacketSize0 != 8)
        {
            devDesc->bMaxPacketSize0 = 8;
        }
    }
    else
    {
        usbStatus = USBH_ERR_NOT_SUP;
    }

    if (length > 8)
    {
        devDesc->idVendor[0]        = buffer[8];
        devDesc->idVendor[1]        = buffer[9];
        devDesc->idProduct[0]       = buffer[10];
        devDesc->idProduct[1]       = buffer[11];
        devDesc->bcdDevice[0]       = buffer[12];
        devDesc->bcdDevice[1]       = buffer[13];
        devDesc->iManufacturer      = buffer[14];
        devDesc->iProduct           = buffer[15];
        devDesc->iSerialNumber      = buffer[16];
        devDesc->bNumConfigurations = buffer[17];
    }

    return usbStatus;
}

/*!
 * @brief     Parse configuration descriptor
 *
 * @param     usbInfo : usb handler information
 * 
 * @param     buffer : source recevice data
 *
 * @param     length : device descriptor length
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_CfgDescParse(USBH_INFO_T* usbInfo, uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_OK;
    USBH_CFG_DESC_T* cfgDesc = &usbInfo->devInfo.configuration;
    uint16_t totalLengthTemp;
    uint8_t itfNumTemp;
    uint16_t parseIndex;
    uint8_t descLength;
    uint8_t itfIndex = 0;
    uint8_t epIndex = 0;

    /* Make sure the buffer is not NULL */
    if (buffer == NULL)
    {
        USBH_USR_DBG("USBH_CfgDescParse: buffer is NULL");
        return USBH_FAIL;
    }

    /* Phase configuration descriptor */
    totalLengthTemp = (uint16_t)(buffer[2] | (buffer[3] << 8));
    totalLengthTemp = (totalLengthTemp > CFG_DESC_MAX_LEN) ? CFG_DESC_MAX_LEN : totalLengthTemp;

    cfgDesc->bLength             = buffer[0];
    cfgDesc->bDescriptorType     = buffer[1];
    cfgDesc->wTotalLength[0]     = totalLengthTemp & 0xFF;
    cfgDesc->wTotalLength[1]     = (totalLengthTemp >> 8) & 0xFF;
    cfgDesc->bNumInterfaces      = buffer[4];
    cfgDesc->bConfigurationValue = buffer[5];
    cfgDesc->iConfiguration      = buffer[6];
    cfgDesc->bmAttributes        = buffer[7];
    cfgDesc->bMaxPower           = buffer[8];

    /* Make sure the configuration descriptor length is valid */
    if (cfgDesc->bLength != STD_CFG_DESC_SIZE)
    {
        cfgDesc->bLength = STD_CFG_DESC_SIZE;
    }

    if (length > STD_CFG_DESC_SIZE)
    {
        /* Parse interface descriptor */
        parseIndex = STD_CFG_DESC_SIZE;

        while ((parseIndex < totalLengthTemp) && (itfIndex < INTERFACE_DESC_MAX_NUM))
        {
            /* Get descriptor length */
            descLength = buffer[parseIndex];

            if (buffer[parseIndex + 1] == USBH_DESC_INTERFACE)
            {
                /* Make sure the interface descriptor length is valid */
                if (buffer[parseIndex] != STD_INTERFACE_DESC_SIZE)
                {
                    buffer[parseIndex] = STD_INTERFACE_DESC_SIZE;
                }

                USBH_CopyBuffer((uint8_t*)&usbInfo->devInfo.configuration.interface[itfIndex], \
                                &buffer[parseIndex], \
                                STD_INTERFACE_DESC_SIZE);

                epIndex = 0;

                /* Parse endpoint descriptor */
                while ((parseIndex < totalLengthTemp) && (epIndex < usbInfo->devInfo.configuration.interface[itfIndex].bNumEndpoints))
                {
                    /* Move to next descriptor */
                    parseIndex += buffer[parseIndex];

                    if (buffer[parseIndex + 1] == USBH_DESC_ENDPOINT)
                    {
                        if (buffer[parseIndex] != STD_EP_DESC_SIZE)
                        {
                            buffer[parseIndex] = STD_EP_DESC_SIZE;
                        }

                        USBH_CopyBuffer((uint8_t*)&usbInfo->devInfo.configuration.interface[itfIndex].endpoint[epIndex],
                                        &buffer[parseIndex], \
                                        STD_EP_DESC_SIZE);

                        epIndex++;
                    }
                }

                if (epIndex < usbInfo->devInfo.configuration.interface[itfIndex].bNumEndpoints)
                {
                    return USBH_ERR_NOT_SUP;
                }

                itfIndex++;
            }
            else
            {
                parseIndex += descLength;
            }
        }

        itfNumTemp = cfgDesc->bNumInterfaces < INTERFACE_DESC_MAX_NUM ? cfgDesc->bNumInterfaces : INTERFACE_DESC_MAX_NUM;
        if (itfIndex < itfNumTemp)
        {
            return USBH_ERR_NOT_SUP;
        }
    }

    return usbStatus;
}

/*!
 * @brief     Parse string descriptor
 *
 * @param     stringDesc : string descriptor
 *
 * @param     buffer : source recevice data
 *
 * @param     length : device descriptor length
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_StringDescParse(uint8_t* stringDesc, uint8_t* buffer, uint16_t length)
{
    USBH_STA_T usbStatus = USBH_OK;
    uint16_t strLength;
    uint16_t i;

    if (buffer == NULL)
    {
        USBH_USR_DBG("USBH_StringDescParse: buffer is NULL");
        return USBH_FAIL;
    }

    if (buffer[1] == USBH_DESC_STRING)
    {
        strLength = buffer[0] - 2;
        if (strLength > length)
        {
            strLength = length;
        }

        buffer += 2;

        for (i = 0; i < strLength; i += 2)
        {
            *stringDesc = buffer[i];
            stringDesc++;
        }

        *stringDesc = 0;
    }

    return usbStatus;
}

/*!
 * @brief       Copy data from source buffer to destination buffer.
 *
 * @param       desBuffer: point to destination buffer
 *
 * @param       srcBuf   : point to source buffer
 *
 * @param       len: copy length
 *
 * @retval      None
 */
static void USBH_CopyBuffer(uint8_t* desBuffer, uint8_t* srcBuf, uint32_t len)
{
    memcpy(desBuffer, srcBuf, len);
}

/**@} end of group USBH_Core_Functions */
/**@} end of group USBH_Core */
/**@} end of group APM32_USB_Library */
