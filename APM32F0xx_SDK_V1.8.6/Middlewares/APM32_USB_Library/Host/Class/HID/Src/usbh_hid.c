/*!
 * @file        usbh_hid.h
 *
 * @brief       USB HID core function
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
#include "usbh_hid.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keyboard.h"
#include "usbh_stdReq.h"
#include "usbh_dataXfer.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_HID_Class
  @{
  */

/** @defgroup USBH_HID_Functions Functions
  @{
  */

static USBH_STA_T USBH_HID_ClassInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_HID_ClassDeInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_HID_ClassReqHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_HID_SOFHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_HID_CoreHandler(USBH_INFO_T* usbInfo);

static USBH_STA_T USBH_HidDescParse(USBH_HID_DESC_T* hidDesc, uint8_t* buffer);

/**@} end of group USBH_HID_Functions */

/** @defgroup USBH_HID_Structures Structures
  @{
  */

/* HID class handler */
USBH_CLASS_T USBH_HID_CLASS =
{
    "Class HID",
    USBH_CLASS_HID,
    USBH_HID_ClassInitHandler,
    USBH_HID_ClassDeInitHandler,
    USBH_HID_ClassReqHandler,
    USBH_HID_CoreHandler,
    USBH_HID_SOFHandler,
    NULL,
};

/**@} end of group USBH_HID_Structures*/

/** @defgroup USBH_HID_Functions Functions
  @{
  */

/*!
 * @brief     USB host get HID report descriptor
 *
 * @param     usbInfo : usb handler information
 *
 * @param     desLength : length of description
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_GetHIDReportDesc(USBH_INFO_T* usbInfo, uint16_t desLength)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_GetDescriptor(usbInfo, ((USBH_REQ_DIR_IN << 7) | \
                                       (USBH_REQ_TYPE_STANDARD << 5) | \
                                       (USBH_RECIPIENT_INTERFACE)), \
                                       USBH_DESC_HID_REPORT, \
                                       usbInfo->devInfo.data, \
                                       desLength);

    return usbStatus;
}

/*!
 * @brief     USB host get HID descriptor
 *
 * @param     usbInfo : usb handler information
 * 
 * @param     desLength : length of description
 * 
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_GetHIDDesc(USBH_INFO_T* usbInfo, uint16_t desLength)
{
    USBH_STA_T usbStatus;

    usbStatus = USBH_REQ_GetDescriptor(usbInfo, ((USBH_REQ_DIR_IN << 7) | \
                                       (USBH_REQ_TYPE_STANDARD << 5) | \
                                       (USBH_RECIPIENT_INTERFACE)), \
                                       USBH_DESC_HID, \
                                       usbInfo->devInfo.data, \
                                       desLength);

    return usbStatus;
}

/*!
 * @brief     USB host get HID report
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reportType : type of report
 *
 * @param     reportID : ID of report
 *
 * @param     buffer : report buffer
 *
 * @param     length : report length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_GetReport(USBH_INFO_T* usbInfo, uint8_t reportType, \
        uint8_t reportID, uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_IN;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_HID_GET_REPORT;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = reportID;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = reportType;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = length;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, buffer, (uint16_t)length);

    return usbStatus;
}

/*!
 * @brief     USB host set HID report
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reportType : type of report
 *
 * @param     reportID : ID of report
 *
 * @param     buffer : report buffer
 *
 * @param     length : report length
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_SetReport(USBH_INFO_T* usbInfo, uint8_t reportType, \
        uint8_t reportID, uint8_t* buffer, uint8_t length)
{
    USBH_STA_T usbStatus;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_HID_SET_REPORT;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = reportID;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = reportType;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = length;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, buffer, (uint16_t)length);

    return usbStatus;
}

/*!
 * @brief     USB host set HID idle
 *
 * @param     usbInfo : usb handler information
 *
 * @param     reportID : traget report ID
 *
 * @param     duration : duration for idle request
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_SetIdle(USBH_INFO_T* usbInfo, uint8_t reportID, uint8_t duration)
{
    USBH_STA_T usbStatus;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_HID_SET_IDLE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = reportID;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = duration;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host set HID protocol
 *
 * @param     usbInfo : usb handler information
 *
 * @param     protocol : boot / report protocol
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_HID_SetProtocol(USBH_INFO_T* usbInfo, uint8_t protocol)
{
    USBH_STA_T usbStatus;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.dir       = USBH_REQ_DIR_OUT;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.type      = USBH_REQ_TYPE_CLASS;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bmRequestType.REQ_TYPE_B.recipient = USBH_RECIPIENT_INTERFACE;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.bRequest = USBH_HID_SET_PROTOCOL;

    if (protocol)
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 0;
    }
    else
    {
        usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[0] = 1;
    }

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wValue[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wIndex[1] = 0;

    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[0] = 0;
    usbInfo->ctrlInfo.steupReq.DATA_FIELD.wLength[1] = 0;

    usbStatus = USBH_REQ_CtrlXferHandler(usbInfo, NULL, 0);

    return usbStatus;
}

/*!
 * @brief     USB host get device type
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    hid device type
 */
USBH_HID_DEV_T USBH_HID_ReadDevType(USBH_INFO_T* usbInfo)
{
    USBH_HID_DEV_T devType = HID_UNKNOWN;
    uint8_t itfProtocol;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, usbInfo->devInfo.curItf);

        if (itfProtocol == USBH_HID_MOUSE_BOOT_CODE)
        {
            devType = HID_MOUSE;
        }
        else if (itfProtocol == USBH_HID_KEYBOARD_BOOT_CODE)
        {
            devType = HID_KEYBOARD;
        }
    }

    return devType;
}

/*!
 * @brief     USB host get polling interval
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    polling interval
 */
uint8_t USBH_HID_ReadPollInterval(USBH_INFO_T* usbInfo)
{
    USBH_HID_INFO_T *hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;

    if ((usbInfo->hostState == USBH_HOST_USER_INPUT) || \
        (usbInfo->hostState == USBH_HOST_CLASS) || \
        (usbInfo->hostState == USBH_HOST_CLASS_REQ) || \
        (usbInfo->hostState == USBH_HOST_CLASS_ACTIVE) || \
        (usbInfo->hostState == USBH_HOST_SET_CONFIGURATION))
        {
            return (uint8_t)hidInfo->pollInterval;
        }

    return 0;
}

/*!
 * @brief     Parse HID descriptor
 *
 * @param     hidDesc : HID descriptor
 *
 * @param     buffer : source data of configuration descriptor
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_HidDescParse(USBH_HID_DESC_T* hidDesc, uint8_t* buffer)
{
    uint16_t totalLength;
    uint16_t parseIndex;
    uint8_t subLength;

    if (buffer == NULL)
    {
        return USBH_FAIL;
    }

    totalLength = buffer[2] | (buffer[3] << 8);

    if (totalLength > STD_CFG_DESC_SIZE)
    {
        parseIndex = STD_CFG_DESC_SIZE;

        while (totalLength > parseIndex)
        {
            subLength = buffer[parseIndex];

            if (buffer[parseIndex + 1] == USBH_DESC_HID)
            {
                hidDesc->bLength = buffer[parseIndex];
                hidDesc->bDescriptorType = buffer[parseIndex + 1];
                hidDesc->bcdHID[0] = buffer[parseIndex + 2];
                hidDesc->bcdHID[1] = buffer[parseIndex + 3];
                hidDesc->bCountryCode = buffer[parseIndex + 4];
                hidDesc->bNumDescriptors = buffer[parseIndex + 5];
                hidDesc->bReportDescriptorType = buffer[parseIndex + 6];
                hidDesc->wDescriptorLength[0] = buffer[parseIndex + 7];
                hidDesc->wDescriptorLength[1] = buffer[parseIndex + 8];
                break;
            }

            parseIndex += subLength;

            if ((totalLength - parseIndex) < STD_CFG_DESC_SIZE)
            {
                break;
            }
        }
    }

    return USBH_OK;
}

/*********************** USB host HID class handler ***************************/

/*!
 * @brief       USB host HID configuration handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_HID_ClassInitHandler(USBH_INFO_T* usbInfo)
{
    uint8_t itfNum = 0;
    uint8_t epNumMax;
    uint16_t epMPS;
    uint8_t subClass;
    uint8_t itfClass;
    uint8_t itfProtocol;
    uint32_t i;
    USBH_HID_INFO_T *hidInfo;

    /* Find HID class interface */
    while (itfNum < INTERFACE_DESC_MAX_NUM)
    {
        itfClass = USBH_ReadInterfaceClass(usbInfo, itfNum);
        subClass = USBH_ReadInterfaceSubClass(usbInfo, itfNum);
        itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, itfNum);

        if ((itfClass == USBH_CLASS_HID) && \
            (subClass == USBH_HID_BOOT_CODE))
            {
                break;
            }

        itfNum++;
    }

    if (itfNum >= INTERFACE_DESC_MAX_NUM)
    {
        USBH_USR_DBG("USB host HID class is not found");
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
        USBH_USR_DBG("USB host HID interface not found");
        return USBH_FAIL;
    }

    /* Link class data */
    usbInfo->activeClass->classData = (USBH_HID_INFO_T*)malloc(sizeof(USBH_HID_INFO_T));
    hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;

    if (hidInfo == NULL)
    {
        USBH_USR_DBG("USB host HID class malloc failed");
        return USBH_FAIL;
    }
    else
    {
        (void)memset(hidInfo, 0, sizeof(USBH_HID_INFO_T));
    }

    /* Set HID state */
    hidInfo->state = USBH_HID_ERR;

    /* Decode class protocol */
    itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, itfNum);
    switch (itfProtocol)
    {
        case USBH_HID_MOUSE_BOOT_CODE:
            USBH_USR_DBG("Register USB host HID mouse class");
            hidInfo->callback = &USBH_HID_MOUSE_Handler;
            break;

        case USBH_HID_KEYBOARD_BOOT_CODE:
            USBH_USR_DBG("Register USB host HID keyboard class");
            hidInfo->callback = &USBH_HID_KEYBOARD_Handler;
            break;

        default:
            USBH_USR_DBG("Protocol is not supported");
            return USBH_FAIL;
    }

    /* Set HID parameters with interface descriptor */
    hidInfo->epSize = USBH_ReadEndpointMPS(usbInfo, itfNum, 0);
    hidInfo->epAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, 0);
    hidInfo->pollInterval = USBH_ReadEndpointInterval(usbInfo, itfNum, 0);

    if (hidInfo->pollInterval < USBH_HID_POLL_MIN_NUM)
    {
        hidInfo->pollInterval = USBH_HID_POLL_MIN_NUM;
    }

    /* Set HID state */
    hidInfo->state = USBH_HID_INIT;
    hidInfo->classReqState = USBH_HID_REQ_INIT;

    /* Find HID class endpoint */
    epNumMax = USBH_ReadInterfaceEpNum(usbInfo, itfNum);

    if (epNumMax > ENDPOINT_DESC_MAX_NUM)
    {
        epNumMax = ENDPOINT_DESC_MAX_NUM;
    }

    for (i = 0; i < epNumMax; i++)
    {
        if (USBH_ReadEndpointAddress(usbInfo, itfNum, i) & 0x80)
        {
            hidInfo->inEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            hidInfo->inChNum = USBH_CH_AllocChannel(usbInfo, hidInfo->inEpAddr);
            epMPS = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open channel for IN endpoint */
            USBH_OpenChannelCallback(usbInfo, hidInfo->inChNum, \
                                     hidInfo->inEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_INTERRUPT, \
                                     epMPS);

            USBH_ConfigToggleCallback(usbInfo, hidInfo->inChNum, 0);
        }
        else
        {
            hidInfo->outEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            hidInfo->outChNum = USBH_CH_AllocChannel(usbInfo, hidInfo->outEpAddr);
            epMPS = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open channel for OUT endpoint */
            USBH_OpenChannelCallback(usbInfo, hidInfo->outChNum, \
                                     hidInfo->outEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_INTERRUPT, \
                                     epMPS);

            USBH_ConfigToggleCallback(usbInfo, hidInfo->outChNum, 0);
        }
    }

    return USBH_OK;
}

/*!
 * @brief       USB host HID class reset handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_HID_ClassDeInitHandler(USBH_INFO_T* usbInfo)
{
    USBH_HID_INFO_T *hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;

    if (hidInfo->inChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, hidInfo->inChNum);
        USBH_CH_FreeChannel(usbInfo, hidInfo->inChNum);
        hidInfo->inChNum = 0;
    }

    if (hidInfo->outChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, hidInfo->outChNum);
        USBH_CH_FreeChannel(usbInfo, hidInfo->outChNum);
        hidInfo->outChNum = 0;
    }

    if (usbInfo->activeClass->classData != NULL)
    {
        free(usbInfo->activeClass->classData);
        usbInfo->activeClass->classData = 0;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host HID class reguest handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_HID_ClassReqHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus = USBH_BUSY;
    USBH_HID_INFO_T *hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;
    static uint16_t descLength;

    switch (hidInfo->classReqState)
    {
        case USBH_HID_REQ_IDLE:
            break;

        case USBH_HID_REQ_INIT:
        case USBH_HID_REQ_GET_HID_DESC:
            (void)USBH_HidDescParse(&hidInfo->desc, usbInfo->devInfo.cfgDescBuf);
            hidInfo->classReqState = USBH_HID_REQ_GET_REP_DESC;
            break;

        case USBH_HID_REQ_GET_REP_DESC:
            descLength = hidInfo->desc.wDescriptorLength[0] | (hidInfo->desc.wDescriptorLength[1] << 8);
            reqStatus = USBH_HID_GetHIDReportDesc(usbInfo, descLength);
            switch (reqStatus)
            {
                case USBH_OK:
                    hidInfo->classReqState = USBH_HID_REQ_SET_IDLE;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_DBG("Class request error: Get HID report descriptor failed");
                    usbStatus = USBH_FAIL;
                    break;

                default:
                    break;
            }
            break;

        case USBH_HID_REQ_SET_IDLE:
            reqStatus = USBH_HID_SetIdle(usbInfo, 0, 0);
            switch (reqStatus)
            {
                case USBH_OK:
                    hidInfo->classReqState = USBH_HID_REQ_SET_PROTOCOL;
                    break;

                case USBH_ERR_NOT_SUP:
                    hidInfo->classReqState = USBH_HID_REQ_SET_PROTOCOL;
                    break;

                default:
                    break;
            }
            break;

        case USBH_HID_REQ_SET_PROTOCOL:
            reqStatus = USBH_HID_SetProtocol(usbInfo, 0);
            switch (reqStatus)
            {
                case USBH_OK:
                    hidInfo->classReqState = USBH_HID_REQ_IDLE;
                    usbStatus = USBH_OK;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_DBG("Class request error: Set protocol failed");
                    usbStatus = USBH_FAIL;
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
 * @brief       USB host HID SOF handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_HID_SOFHandler(USBH_INFO_T* usbInfo)
{
    USBH_HID_INFO_T *hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;

    if (hidInfo->state == USBH_HID_POLL)
    {
        if ((usbInfo->timer - hidInfo->timer) >= hidInfo->pollInterval)
        {
            hidInfo->state = USBH_HID_IN_DATA;
        }
    }

    return USBH_OK;
}

/*!
 * @brief       USB host HID handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_HID_CoreHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_OK;
    USBH_HID_INFO_T *hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;
    static uint32_t xferSize;

    switch (hidInfo->state)
    {
        case USBH_HID_INIT:
            if (hidInfo->callback->InitHandler != NULL)
            {
                usbStatus = hidInfo->callback->InitHandler(usbInfo);
            }

            /* Notify user class launched */
            if (usbInfo->userCallback != NULL)
            {
                usbInfo->userCallback(usbInfo, USBH_USER_CLASS_LAUNCHED);
            }

            /* Set HID state to idle */
            hidInfo->state = USBH_HID_IDLE;
            break;

        case USBH_HID_IDLE:
            usbStatus = USBH_HID_GetReport(usbInfo, 
                                           HID_INPUT_REPORT,
                                           0,
                                           hidInfo->buffer,
                                           (uint8_t)hidInfo->epSize);
            switch (usbStatus)
            {
                case USBH_OK:
                    /* Set HID state to sync */
                    hidInfo->state = USBH_HID_SYNC;
                    break;

                case USBH_BUSY:
                    /* Set HID state to idle */
                    hidInfo->state = USBH_HID_IDLE;
                    usbStatus = USBH_OK;
                    break;

                case USBH_ERR_NOT_SUP:
                    /* Set HID state to sync */
                    hidInfo->state = USBH_HID_SYNC;
                    usbStatus = USBH_OK;
                    break;

                default:
                    /* Set HID state to error */
                    hidInfo->state = USBH_HID_ERR;
                    usbStatus = USBH_FAIL;
                    break;
            }
            break;

        case USBH_HID_SYNC:
            if ((usbInfo->timer & 1) != 0)
            {
                /* Set HID state to IN data */
                hidInfo->state = USBH_HID_IN_DATA;
            }
            break;

        case USBH_HID_IN_DATA:
            (void)USBH_IntReceiveDataReq(usbInfo, \
                                         hidInfo->inChNum, \
                                         hidInfo->buffer, \
                                         (uint8_t)hidInfo->epSize);

            /* Set HID state to polling */
            hidInfo->state = USBH_HID_POLL;
            hidInfo->timer = usbInfo->timer;
            hidInfo->dataFlag = DISABLE;
            break;

        case USBH_HID_POLL:
            if (USBH_ReadUrbStatusCallback(usbInfo, hidInfo->inChNum) == USB_URB_OK)
            {
                xferSize = USBH_ReadLastXferSizeCallback(usbInfo, hidInfo->inChNum);

                if ((xferSize != 0) && (hidInfo->dataFlag == DISABLE))
                {
                    hidInfo->dataFlag = ENABLE;
                    
                    /* HID event callback */
                    USBH_HID_PollCallback(usbInfo);
                }
            }
            else
            {
                 if (USBH_ReadUrbStatusCallback(usbInfo, hidInfo->inChNum) == USB_URB_STALL)
                 {
                    if (USBH_ClearFeature(usbInfo, hidInfo->epAddr) == USBH_OK)
                    {
                        /* Set HID state IN data */
                        hidInfo->state = USBH_HID_IN_DATA;
                    }
                 }
            }
            break;

        case USBH_HID_OUT_DATA:

            break;

        case USBH_HID_BUSY:
            break;

        case USBH_HID_ERR:
            break;

        default:
            break;
    }

    return usbStatus;
}

/*********************** USB host HID poll callback ***************************/

/*!
 * @brief       USB host HID poll callback
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
__weak void USBH_HID_PollCallback(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);
}

/**@} end of group USBH_HID_Functions */
/**@} end of group USBH_HID_Class */
/**@} end of group APM32_USB_Library */
