/*!
 * @file        usbh_core.c
 *
 * @brief       USB host core function
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
#include "usbh_core.h"
#include "usbh_enum.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_Core
  @{
  */

/** @defgroup USBH_Core_Functions Functions
  @{
  */

static USBH_STA_T USBH_ResetStateMachine(USBH_INFO_T* usbInfo);
static void USBH_HandleSOF(USBH_INFO_T* usbInfo);

/**@} end of group USBH_Core_Functions */

/** @defgroup USBH_Core_Functions Functions
  @{
  */

/*!
 * @brief     USB host connection event
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
USBH_STA_T USBH_Connect(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_OK;

    usbInfo->devInfo.connectedStatus = ENABLE;
    usbInfo->devInfo.disconnectedStatus = DISABLE;
    usbInfo->devInfo.reEnumerateStatus = DISABLE;

    return usbStatus;
}

/*!
 * @brief     USB host disconnection event
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
USBH_STA_T USBH_Disconnect(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_OK;

    usbInfo->devInfo.connectedStatus = DISABLE;
    usbInfo->devInfo.disconnectedStatus = ENABLE;
    usbInfo->devInfo.portEnabledStatus = DISABLE;

    /* Stop the USB host */
    USBH_StopHostCallback(usbInfo);

    /* Free the channel */
    USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);
    USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);

    return usbStatus;
}

/*!
 * @brief     USB host port enable event
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
USBH_STA_T USBH_PortEnable(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_OK;

    usbInfo->devInfo.portEnabledStatus = ENABLE;

    return usbStatus;
}

/*!
 * @brief     USB host port disable event
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
USBH_STA_T USBH_PortDisable(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_OK;

    usbInfo->devInfo.portEnabledStatus = DISABLE;

    return usbStatus;
}

/*!
 * @brief     USB host port is enabled
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    USB port status
 */
uint8_t USBH_PortIsEnabled(USBH_INFO_T* usbInfo)
{
    return (usbInfo->devInfo.portEnabledStatus);
}

/*!
 * @brief     USB host configure timer
 *
 * @param     usbInfo : usb handler information
 *
 * @param     tick : time tick
 *
 * @retval    None
 */
void USBH_ConfigTimer(USBH_INFO_T* usbInfo, uint32_t tick)
{
    usbInfo->timer = tick;
}

/*!
 * @brief     USB host increase timer
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
void USBH_IncTimer(USBH_INFO_T* usbInfo)
{
    usbInfo->timer++;
    USBH_HandleSOF(usbInfo);
}

/*!
 * @brief     USB host read active class
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    class code
 */
uint8_t USBH_ReadActiveClass(USBH_INFO_T* usbInfo)
{
    return (usbInfo->devInfo.configuration.interface[0].bInterfaceClass);
}

/*!
 * @brief     USB host start
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
void USBH_Start(USBH_INFO_T* usbInfo)
{
    /* Start the USB host */
    USBH_StartCallback(usbInfo);

    /* Enable the VBUS */
    USBH_ConfigVbusCallback(usbInfo, ENABLE);
}

/*!
 * @brief     USB host stop
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
void USBH_Stop(USBH_INFO_T* usbInfo)
{
    /* Disable the VBUS */
    USBH_ConfigVbusCallback(usbInfo, DISABLE);

    /* Stop the USB host */
    USBH_StopHostCallback(usbInfo);

    /* Free the channel */
    USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
    USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);
}

/*!
 * @brief     USB host reenumeration
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_ReEnumerate(USBH_INFO_T* usbInfo)
{
    if (USBH_PortIsEnabled(usbInfo) != DISABLE)
    {
        usbInfo->devInfo.reEnumerateStatus = ENABLE;

        /* Stop the USB host */
        USBH_Stop(usbInfo);

        usbInfo->devInfo.disconnectedStatus = ENABLE;
    }

    return USBH_OK;
}

/*!
 * @brief     USB host core init
 *
 * @param     usbInfo : usb handler information
 *
 * @param     usbHostSpeed
 *
 * @param     usbHostClass
 *
 * @param     userCallback
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_Init(USBH_INFO_T* usbInfo, USBH_SPEED_T usbHostSpeed, USBH_CLASS_T* usbHostClass,
                     void (*userCallbackFunc)(struct _USBH_INFO_T*, uint8_t))
{
    /* Check the USB handler information */
    if (usbInfo == NULL)
    {
        return USBH_FAIL;
    }

    /* Reset device status */
    usbInfo->devInfo.connectedStatus = DISABLE;
    usbInfo->devInfo.disconnectedStatus = DISABLE;
    usbInfo->devInfo.reEnumerateStatus = DISABLE;
    usbInfo->devInfo.portEnabledStatus = DISABLE;

    /* Store USB host speed */
    usbInfo->hostSpeed = usbHostSpeed;

    /* Reset the USB host state machine */
    (void)USBH_ResetStateMachine(usbInfo);

    /* Register user callback function */
    if (userCallbackFunc != NULL)
    {
        usbInfo->userCallback = userCallbackFunc;
    }

    /* Reset class parameters */
    usbInfo->activeClass = NULL;
    usbInfo->classNum = 0;

    /* Register USB host class */
    if (usbHostClass != NULL)
    {
        (void)USBH_RegisterClass(usbInfo, usbHostClass);
    }

    /* Initialize the USB hardware */
    USBH_HardwareInit(usbInfo);

    return USBH_OK;
}

/*!
 * @brief     USB device core de-init
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_DeInit(USBH_INFO_T* usbInfo)
{
    /* Reset the USB host state machine */
    (void)USBH_ResetStateMachine(usbInfo);

    /* Reset device status */
    usbInfo->devInfo.connectedStatus = DISABLE;
    usbInfo->devInfo.disconnectedStatus = DISABLE;
    usbInfo->devInfo.reEnumerateStatus = DISABLE;
    usbInfo->devInfo.portEnabledStatus = DISABLE;
#if 0
    /* De-initialize the USB host class */
    for (i = 0; i < USBH_SUP_CLASS_MAX_NUM; i++)
    {
        if ((usbInfo->hostClass[i] != NULL) && \
            (usbInfo->activeClass != NULL) && \
            (usbInfo->activeClass->classData != NULL))
        {
            usbInfo->hostClass[i]->classDeInitHandler(usbInfo);
        }
    }
#endif
    /* De-initialize the USB host driver */
    if (usbInfo->dataPoint != NULL)
    {
        USBH_StopHostCallback(usbInfo);
    }

    /* De-initialize the USB hardware */
    USBH_HardwareReset(usbInfo);

    return USBH_OK;
}

/*!
 * @brief     Host register class
 *
 * @param     usbInfo : usb handler information
 * 
 * @param     usbHostClass : host class
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_RegisterClass(USBH_INFO_T* usbInfo, USBH_CLASS_T* usbHostClass)
{
    USBH_STA_T usbStatus = USBH_FAIL;

    if (usbInfo == NULL)
    {
        return usbStatus;
    }

    if (usbHostClass != NULL)
    {
        /* Register class function */
        if (usbInfo->classNum < USBH_SUP_CLASS_MAX_NUM)
        {
            usbInfo->hostClass[usbInfo->classNum++] = usbHostClass;
            usbStatus = USBH_OK;
        }
    }

    return usbStatus;
}

/*!
 * @brief     Host state machine polling Process
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_PollingProcess(USBH_INFO_T* usbInfo)
{
    __IO USBH_STA_T usbStatus = USBH_FAIL;
    uint8_t i;

    /* Device disconnect */
    if (usbInfo->devInfo.disconnectedStatus == ENABLE)
    {
        usbInfo->hostState = USBH_HOST_DEVICE_DISCONNECTED;
    }

    switch (usbInfo->hostState)
    {
        case USBH_HOST_IDLE:
            if (usbInfo->devInfo.connectedStatus != DISABLE)
            {
                USBH_USR_LOG("USB device inserted");

                if (usbInfo->userCallback != NULL)
                {
                    /* Notify the user that the USB device is connected */
                    usbInfo->userCallback(usbInfo, USBH_USER_CONNECTION);
                }

                /* Set the USB host state to device attached */
                usbInfo->hostState = USBH_HOST_WAIT_FOR_ATTACHMENT;

                /* Wait for 200ms after the device is inserted */
                USBH_DelayCallback(200);

                /* Reset USB host port */
                USBH_ResetCallback(usbInfo);

                usbInfo->timeout = 0;

                /* Set the USB device address */
                usbInfo->devInfo.address = USBH_DEVICE_DEFAULT_ADDRESS;
            }
            break;

        case USBH_HOST_WAIT_FOR_ATTACHMENT:
            if (usbInfo->devInfo.portEnabledStatus == ENABLE)
            {
                USBH_USR_LOG("USB device reset completed and port enabled");

                usbInfo->devInfo.rstCnt = 0;

                usbInfo->hostState = USBH_HOST_DEVICE_ATTACHED;
            }
            else
            {
                if (usbInfo->timeout <= USBH_DEVICE_RESET_TIMEOUT)
                {
                    usbInfo->timeout += 10;
                    USBH_DelayCallback(10);
                }
                else
                {
                    USBH_USR_DBG("USB device reset timeout");

                    usbInfo->devInfo.rstCnt++;

                    if (usbInfo->devInfo.rstCnt > USBH_DEVICE_RESET_MAX_CNT)
                    {
                        USBH_USR_LOG("USB device reset failed. Please unplug and re-plug the device");

                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                    else
                    {
                        USBH_USR_LOG("USB device reset retry");

                        usbInfo->hostState = USBH_HOST_IDLE;
                    }
                }
            }
            break;

        case USBH_HOST_DEVICE_ATTACHED:
            /* Wait for 100ms after the device is connected */
            USBH_DelayCallback(100);

            /* Get the USB device speed */
            usbInfo->devInfo.speed = (uint8_t)USBH_ReadSpeedCallback(usbInfo);

            if (usbInfo->userCallback != NULL)
            {
                /* Notify the user that the USB device speed is detected */
                usbInfo->userCallback(usbInfo, USBH_USER_DETECTED_SPEED);
            }

            /* Set control channel */
            usbInfo->ctrlInfo.channelOutNum = USBH_CH_AllocChannel(usbInfo, 0x00);
            usbInfo->ctrlInfo.channelInNum = USBH_CH_AllocChannel(usbInfo, 0x80);

            USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelOutNum, \
                                    0x00, usbInfo->devInfo.address, \
                                    usbInfo->devInfo.speed, EP_TYPE_CONTROL,
                                    (uint16_t)usbInfo->ctrlInfo.channelSize);

            USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelInNum, \
                                    0x80, usbInfo->devInfo.address, \
                                    usbInfo->devInfo.speed, EP_TYPE_CONTROL,
                                    (uint16_t)usbInfo->ctrlInfo.channelSize); 

            /* Set the USB host state to enumeration */
            usbInfo->hostState = USBH_HOST_ENUMERATION;
            break;

        case USBH_HOST_ENUMERATION:
            usbStatus = USBH_EnumHandler(usbInfo);

            if (usbStatus == USBH_OK)
            {
                USBH_USR_DBG("USB device enumeration completed");

                usbInfo->devInfo.curItf = 0;

                if (usbInfo->userCallback != NULL)
                {
                    /* Notify the user that the USB device is enumerated */
                    usbInfo->userCallback(usbInfo, USBH_USER_ENUMERATION);
                }

                if (usbInfo->devInfo.device.bNumConfigurations != 1)
                {
                    /* Set the USB host state to user input */
                    usbInfo->hostState = USBH_HOST_USER_INPUT;
                }
                else
                {
                    /* Set the USB host state to set configuration */
                    usbInfo->hostState = USBH_HOST_SET_CONFIGURATION;
                }
            }
            break;

        case USBH_HOST_USER_INPUT:
            if (usbInfo->userCallback != NULL)
            {
                USBH_USR_LOG("Please select the USB device configuration");
                /* Notify the user to select the USB device configuration */
                usbInfo->userCallback(usbInfo, USBH_USER_SELECT_CONFIGURATION);

                /* Set the USB host state to set configuration */
                usbInfo->hostState = USBH_HOST_SET_CONFIGURATION;
            }
            break;

        case USBH_HOST_SET_CONFIGURATION:
            if (USBH_SetConfiguration(usbInfo, (uint16_t)usbInfo->devInfo.configuration.bConfigurationValue) == USBH_OK)
            {
                USBH_USR_DBG("Set to the USB device default configuration");

                /* Set the USB host state to set feature */
                usbInfo->hostState = USBH_HOST_SET_FEATURE;
            }
            break;

        case USBH_HOST_SET_FEATURE:
            if ((usbInfo->devInfo.configuration.bmAttributes & 0x20) == 0)
            {
                USBH_USR_DBG("Device is not support remote wakeup");

                /* Set the USB host state to class active */
                usbInfo->hostState = USBH_HOST_CLASS_ACTIVE;
            }
            else
            {
                /* Set feature remote wakeup */
                usbStatus = USBH_SetFeature(usbInfo, USBH_FEATURE_REMOTE_WAKEUP);

                switch (usbStatus)
                {
                    case USBH_OK:
                        USBH_USR_DBG("Set feature remote wakeup completed");

                        /* Set the USB host state to class active */
                        usbInfo->hostState = USBH_HOST_CLASS_ACTIVE;
                        break;

                    case USBH_ERR_NOT_SUP:
                        USBH_USR_DBG("Device is not support remote wakeup");

                        /* Set the USB host state to class active */
                        usbInfo->hostState = USBH_HOST_CLASS_ACTIVE;
                        break;

                    default:
                        break;
                }
            }
            break;

        case USBH_HOST_CLASS_ACTIVE:
            if (usbInfo->classNum != 0)
            {
                usbInfo->activeClass = NULL;

                for (i = 0; i < USBH_SUP_CLASS_MAX_NUM; i++)
                {
                    if (USBH_ReadInterfaceClass(usbInfo, 0) == usbInfo->hostClass[i]->classCode)
                    {
                        usbInfo->activeClass = usbInfo->hostClass[i];
                        break;
                    }
                }

                if (usbInfo->activeClass != NULL)
                {
                    /* Initialize the USB host class */
                    if (usbInfo->activeClass->classInitHandler(usbInfo) == USBH_OK)
                    {
                        USBH_USR_DBG("USB host %s class actived", usbInfo->activeClass->className);

                        /* Set the USB host state to class request */
                        usbInfo->hostState = USBH_HOST_CLASS_REQ;
                    }
                    else
                    {
                        USBH_USR_DBG("USB host class is not support %s", usbInfo->activeClass->className);

                        /* Set the USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                }
                else
                {
                    USBH_USR_LOG("USB host class is not register for the USB device");

                    /* Set the USB host state to abort */
                    usbInfo->hostState = USBH_HOST_ABORT;
                }
            }
            else
            {
                USBH_USR_DBG("USB host class is not register for the USB device");
            }
            break;

        case USBH_HOST_CLASS_REQ:
            if (usbInfo->activeClass == NULL)
            {
                USBH_USR_DBG("USB host class driver is valid");

                /* Set the USB host state to abort */
                usbInfo->hostState = USBH_HOST_ABORT;
            }
            else
            {
                /* Class request handler */
                usbStatus = usbInfo->activeClass->classReqHandler(usbInfo);

                switch (usbStatus)
                {
                    case USBH_OK:
                        /* Set the USB host state to class handler */
                        usbInfo->hostState = USBH_HOST_CLASS;
                        break;

                    case USBH_FAIL:
                        USBH_USR_DBG("Device is not response to the class request. Please unplug and re-plug the device");

                        /* Set the USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                        break;

                    default:
                        break;
                }
            }
            break;

        case USBH_HOST_CLASS:
            if (usbInfo->activeClass != NULL)
            {
                /* Class handler */
                usbInfo->activeClass->classCoreHandler(usbInfo);
            }
            break;

        case USBH_HOST_DEVICE_DISCONNECTED:
            /* Disable the device connected status for the next enumeration */
            usbInfo->devInfo.disconnectedStatus = DISABLE;

            if (usbInfo->activeClass != NULL)
            {
                usbInfo->activeClass->classDeInitHandler(usbInfo);
                usbInfo->activeClass = NULL;
            }

            if (usbInfo->userCallback != NULL)
            {
                /* Notify the user that the USB device is disconnected */
                usbInfo->userCallback(usbInfo, USBH_USER_DISCONNECTION);
            }

            /* Reset the USB host state machine */
            (void)USBH_ResetStateMachine(usbInfo);

            USBH_USR_DBG("USB device disconnected");

            if (usbInfo->devInfo.reEnumerateStatus == DISABLE)
            {
                /* Wait for 200ms after the device is disconnected */
                USBH_DelayCallback(200);
                /* Start USB host */
                USBH_StartCallback(usbInfo);
            }
            else
            {
                USBH_USR_DBG("USB device re-enumeration");

                usbInfo->devInfo.reEnumerateStatus = DISABLE;

                /* Start USB host and enable the VBUS */
                USBH_Start(usbInfo);
            }
            break;

        case USBH_HOST_SUSPEND:
            break;

        case USBH_HOST_ABORT:
            break;

        default:
            break;
    }

    return USBH_OK;
}

/*!
 * @brief     Reset USB host state machine
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
static USBH_STA_T USBH_ResetStateMachine(USBH_INFO_T* usbInfo)
{
    uint32_t i;

    /* Clear all of the channel */
    for (i = 0; i < USBH_CHANNEL_MAX_NUM; i++)
    {
        usbInfo->xferChannel[i] = 0;
    }

    /* Clear all of the device data buffer */
    for (i = 0; i < USBH_DATA_BUF_MAX_NUM; i++)
    {
        usbInfo->devInfo.data[i] = 0;
    }

    /* Reset the USB host state machine */
    usbInfo->hostState      = USBH_HOST_IDLE;
    usbInfo->hostEnumState  = USBH_ENUM_IDLE;
    usbInfo->xferState      = USBH_XFER_START;
    usbInfo->timer          = 0;

    /* Reset the control information */
    usbInfo->ctrlInfo.channelSize   = USBH_EP0_PACKET_SIZE;
    usbInfo->ctrlInfo.length        = 0;
    usbInfo->ctrlInfo.state         = USBH_CTRL_SETUP;
    usbInfo->ctrlInfo.errCnt        = 0;

    /* Reset the device information */
    memset(&usbInfo->devInfo.cfgDescBuf, 0, sizeof(usbInfo->devInfo.cfgDescBuf));
    memset(&usbInfo->devInfo.data, 0, sizeof(usbInfo->devInfo.data));
    memset(&usbInfo->devInfo.device, 0, sizeof(usbInfo->devInfo.device));
    memset(&usbInfo->devInfo.configuration, 0, sizeof(usbInfo->devInfo.configuration));

    usbInfo->devInfo.address    = USBH_DEVICE_DEFAULT_ADDRESS;
    usbInfo->devInfo.speed      = USBH_DEVICE_SPEED_FS;
    usbInfo->devInfo.enumCnt    = 0;
    usbInfo->devInfo.rstCnt     = 0;

    return USBH_OK;
}

/*!
 * @brief     USB host SOF handler
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
static void USBH_HandleSOF(USBH_INFO_T* usbInfo)
{
    if ((usbInfo->hostState == USBH_HOST_CLASS) && \
        (usbInfo->activeClass != NULL))
    {
        usbInfo->activeClass->classSofHandler(usbInfo);
    }
}

/*********************** USB host event callback function ************************/

/*!
 * @brief     USB stop host event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
__weak void USBH_StopHostCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief     USB host start event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
__weak void USBH_StartCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief     USB host stop event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
__weak void USBH_StopCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief     USB host reset event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    None
 */
__weak void USBH_ResetCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);
}

/*!
 * @brief     USB host delay event callback function
 *
 * @param     ms : delay time
 *
 * @retval    None
 */
__weak void USBH_ConfigVbusCallback(USBH_INFO_T* usbInfo, uint8_t status)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(status);
}

/*!
 * @brief     USB host read speed event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    speed
 */
__weak uint8_t USBH_ReadSpeedCallback(USBH_INFO_T* usbInfo)
{
    /* callback interface */
    UNUSED(usbInfo);

    return 0;
}

/*!
 * @brief     USB host config the channel to transfer event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel numer
 *
 * @param     endPointNum : end point number
 *
 * @param     devAddr : USB device address
 *
 * @param     devSpeed : USB device speed
 *
 * @param     epType : end point type
 *
 * @param     packetMaxSize : max size of packet
 *
 * @retval    None
 */
__weak void USBH_OpenChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                                     uint8_t endPointNum, uint8_t devAddr, \
                                     uint8_t devSpeed, uint8_t epType, uint16_t packetMaxSize)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);
    UNUSED(endPointNum);
    UNUSED(devAddr);
    UNUSED(devSpeed);
    UNUSED(epType);
    UNUSED(packetMaxSize);
}

/*!
 * @brief     USB submit URB event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     chNum : channel number
 *
 * @param     dir : channel direction
 *
 * @param     epType : endpoint type
 *
 * @param     tokenType : tokenType
 *
 * @param     buffer : URB data
 *
 * @param     length : length of URB data
 *
 * @param     pingStatus : ping status
 *
 * @retval    None
 */
__weak void USBH_UrbSubmitCallback(USBH_INFO_T* usbInfo, uint8_t chNum, uint8_t dir, \
                                   uint8_t epType, uint8_t tokenType, uint8_t* buffer, \
                                   uint16_t length, uint8_t pingStatus)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(chNum);
    UNUSED(dir);
    UNUSED(epType);
    UNUSED(tokenType);
    UNUSED(buffer);
    UNUSED(length);
    UNUSED(pingStatus);
}

/*!
 * @brief     USB host read URB status event callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum : channel number
 *
 * @retval    URB status
 */
__weak uint8_t USBH_ReadUrbStatusCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);

    return 0;
}

/*!
 * @brief     USB host close channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    None
 */
__weak void USBH_CloseChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);
}

/*!
 * @brief     USB host read size of last xfer callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    xfer size
 */
__weak uint32_t USBH_ReadLastXferSizeCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);

    return 0;
}

/*!
 * @brief     USB host configure current toggle of channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @param     toggle: toggle
 *
 * @retval    xfer size
 */
__weak void USBH_ConfigToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, uint8_t toggle)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);
    UNUSED(toggle);
}

/*!
 * @brief     USB host read current toggle of channel callback function
 *
 * @param     usbInfo : usb handler information
 *
 * @param     channelNum: channel number
 *
 * @retval    xfer size
 */
__weak uint8_t USBH_ReadToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum)
{
    /* callback interface */
    UNUSED(usbInfo);
    UNUSED(channelNum);

    return 0;
}

/**@} end of group USBH_Core_Functions */
/**@} end of group USBH_Core */
/**@} end of group APM32_USB_Library */
