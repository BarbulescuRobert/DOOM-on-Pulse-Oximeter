/*!
 * @file        usbh_enum.c
 *
 * @brief       USB host enum hander function
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
#include "usbh_enum.h"
#include "usbh_stdReq.h"
#include "usbh_core.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_Core
  @{
  */

/** @defgroup USBH_Core_Functions Functions
  @{
  */

/*!
 * @brief     USB host enum handler
 *
 * @param     usbInfo : usb handler information
 *
 * @retval    usb host status
 */
USBH_STA_T USBH_EnumHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_STA_T enumStatus = USBH_BUSY;
    uint16_t cfgDescTotalLen;

    switch (usbInfo->hostEnumState)
    {
        case USBH_ENUM_IDLE:
            /* Get 8 bytes device descriptor */
            enumStatus = USBH_GetDevDesc(usbInfo, 8);

            switch (enumStatus)
            {
                case USBH_OK:
                    /* Set control channel parameters */
                    usbInfo->ctrlInfo.channelSize = usbInfo->devInfo.device.bMaxPacketSize0;

                    USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelInNum, \
                                            0x80, usbInfo->devInfo.address, \
                                            usbInfo->devInfo.speed, EP_TYPE_CONTROL, \
                                            (uint16_t)usbInfo->ctrlInfo.channelSize);

                    USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelOutNum, \
                                            0x00, usbInfo->devInfo.address, \
                                            usbInfo->devInfo.speed, EP_TYPE_CONTROL, \
                                            (uint16_t)usbInfo->ctrlInfo.channelSize);

                    /* Set enum state to get full bytes device descriptor */
                    usbInfo->hostEnumState = USBH_ENUM_GET_DEV_DESC;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_LOG("Control request error, can not get device descriptor");
                    /* Count the number of enumeration */
                    usbInfo->devInfo.enumCnt++;
                    if (usbInfo->devInfo.enumCnt <= 3)
                    {
                        /* Free control channel */
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);

                        /* Set USB host state to idle */
                        usbInfo->hostState = USBH_HOST_IDLE;
                    }
                    else
                    {
                        USBH_USR_LOG("Control request error, device is not responding. Please unplug and re-plug the device");
                        /* Set USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                    break;

                default:
                    break;
            }
            break;

        case USBH_ENUM_GET_DEV_DESC:
            /* Get full bytes device descriptor */
            enumStatus = USBH_GetDevDesc(usbInfo, STD_DEV_DESC_SIZE);

            switch (enumStatus)
            {
                case USBH_OK:
                    /* Print device descriptor information */
                    USBH_USR_DBG("Device PID: 0x%04X", \
                                usbInfo->devInfo.device.idProduct[0] | \
                                (usbInfo->devInfo.device.idProduct[1] << 8));

                    USBH_USR_DBG("Device VID: 0x%04X", \
                                usbInfo->devInfo.device.idVendor[0] | \
                                (usbInfo->devInfo.device.idVendor[1] << 8));

                    USBH_USR_DBG("Device BCD: 0x%04X", \
                                usbInfo->devInfo.device.bcdUSB[0] | \
                                (usbInfo->devInfo.device.bcdUSB[1] << 8));

                    USBH_USR_DBG("Device endpoint 0 max packet size: %d", \
                                usbInfo->devInfo.device.bMaxPacketSize0);

                    USBH_USR_DBG("Device configuration number: %d", \
                                usbInfo->devInfo.device.bNumConfigurations);

                    /* Set enum state to set address */
                    usbInfo->hostEnumState = USBH_ENUM_SET_ADDRESS;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_LOG("Control request error, can not get full device descriptor");
                    /* Count the number of enumeration */
                    usbInfo->devInfo.enumCnt++;
                    if (usbInfo->devInfo.enumCnt <= 3)
                    {
                        /* Free control channel */
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);

                        /* Set USB host state to idle */
                        usbInfo->hostState = USBH_HOST_IDLE;

                        /* Set enum state to idle */
                        usbInfo->hostEnumState = USBH_ENUM_IDLE;
                    }
                    else
                    {
                        USBH_USR_LOG("Control request error, device is not responding. Please unplug and re-plug the device");
                        /* Set USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                    break;

                default:
                    break;
            }
            break;

        case USBH_ENUM_SET_ADDRESS:
            /* Set device address */
            enumStatus = USBH_SetAddress(usbInfo, USBH_DEVICE_CONFIGURED_ADDRESS);

            switch (enumStatus)
            {
                case USBH_OK:
                    usbInfo->devInfo.address = USBH_DEVICE_CONFIGURED_ADDRESS;
                    USBH_USR_DBG("Device address: %d", usbInfo->devInfo.address);

                    USBH_DelayCallback(2);

                    /* Update control channel parameters after set address */
                    USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelInNum, \
                                            0x80, usbInfo->devInfo.address, \
                                            usbInfo->devInfo.speed, EP_TYPE_CONTROL, \
                                            (uint16_t)usbInfo->ctrlInfo.channelSize);

                    USBH_OpenChannelCallback(usbInfo, usbInfo->ctrlInfo.channelOutNum, \
                                                0x00, usbInfo->devInfo.address, \
                                                usbInfo->devInfo.speed, EP_TYPE_CONTROL, \
                                                (uint16_t)usbInfo->ctrlInfo.channelSize);

                    /* Set enum state to get configuration descriptor */
                    usbInfo->hostEnumState = USBH_ENUM_GET_CFG_DESC;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_LOG("Control request error, can not set device address");

                    USBH_USR_LOG("Control request error, device is not responding. Please unplug and re-plug the device");
                    /* Set USB host state to abort */
                    usbInfo->hostState = USBH_HOST_ABORT;

                    /* Set enum state to idle */
                    usbInfo->hostEnumState = USBH_ENUM_IDLE;
                    break;

                default:
                    break;
            }
            break;

        case USBH_ENUM_GET_CFG_DESC:
            /* Get configuration descriptor */
            enumStatus = USBH_GetCfgDesc(usbInfo, STD_CFG_DESC_SIZE);

            switch (enumStatus)
            {
                case USBH_OK:
                    /* Set enum state to get full configuration descriptor */
                    usbInfo->hostEnumState = USBH_ENUM_GET_FULL_CFG_DESC;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_LOG("Control request error, can not get configuration descriptor");
                    /* Count the number of enumeration */
                    usbInfo->devInfo.enumCnt++;
                    if (usbInfo->devInfo.enumCnt <= 3)
                    {
                        /* Free control channel */
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);

                        /* Set USB host state to idle */
                        usbInfo->hostState = USBH_HOST_IDLE;

                        /* Set enum state to idle */
                        usbInfo->hostEnumState = USBH_ENUM_IDLE;
                    }
                    else
                    {
                        USBH_USR_LOG("Control request error, device is not responding. Please unplug and re-plug the device");
                        /* Set USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                    break;

                default:
                    break;
            }
            break;

        case USBH_ENUM_GET_FULL_CFG_DESC:
            cfgDescTotalLen = usbInfo->devInfo.configuration.wTotalLength[0] | \
                            (usbInfo->devInfo.configuration.wTotalLength[1] << 8);

            /* Get full configuration descriptor */
            enumStatus = USBH_GetCfgDesc(usbInfo, cfgDescTotalLen);

            switch (enumStatus)
            {
                case USBH_OK:
                    /* Print configuration descriptor information */
                    USBH_USR_DBG("Device num of interfaces: %d", usbInfo->devInfo.configuration.bNumInterfaces);
                    USBH_USR_DBG("Device configuration value: %d", usbInfo->devInfo.configuration.bConfigurationValue);
                    USBH_USR_DBG("Device attributes: 0x%02X", usbInfo->devInfo.configuration.bmAttributes);
                    USBH_USR_DBG("Device max power: %d mA", usbInfo->devInfo.configuration.bMaxPower * 2);

                    /* Set enum state to get manufacturer string descriptor */
                    usbInfo->hostEnumState = USBH_ENUM_GET_MFC_STRING_DESC;
                    break;

                case USBH_ERR_NOT_SUP:
                    USBH_USR_LOG("Control request error, can not get full configuration descriptor");
                    /* Count the number of enumeration */
                    usbInfo->devInfo.enumCnt++;
                    if (usbInfo->devInfo.enumCnt <= 3)
                    {
                        /* Free control channel */
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelInNum);
                        USBH_CH_FreeChannel(usbInfo, usbInfo->ctrlInfo.channelOutNum);

                        /* Set USB host state to idle */
                        usbInfo->hostState = USBH_HOST_IDLE;

                        /* Set enum state to idle */
                        usbInfo->hostEnumState = USBH_ENUM_IDLE;
                    }
                    else
                    {
                        USBH_USR_LOG("Control request error, device is not responding. Please unplug and re-plug the device");
                        /* Set USB host state to abort */
                        usbInfo->hostState = USBH_HOST_ABORT;
                    }
                    break;

                default:
                    break;
            }
            break;

        case USBH_ENUM_GET_MFC_STRING_DESC:
            if (usbInfo->devInfo.device.iManufacturer == 0)
            {
                USBH_USR_DBG("Manufacturer is N/A");
                
                /* Set enum state to get product string descriptor */
                usbInfo->hostEnumState = USBH_ENUM_GET_PRODUCT_STRING_DESC;
            }
            else
            {
                /* Get manufacturer string descriptor */
                enumStatus = USBH_GetStringDesc(usbInfo, \
                                            usbInfo->devInfo.device.iManufacturer, \
                                            usbInfo->devInfo.data, \
                                            0xFF);

                switch (enumStatus)
                {
                    case USBH_OK:
                        USBH_USR_DBG("Manufacturer: %s", (char *)(void *)usbInfo->devInfo.data);

                        /* Set enum state to get product string descriptor */
                        usbInfo->hostEnumState = USBH_ENUM_GET_PRODUCT_STRING_DESC;
                        break;

                    case USBH_ERR_NOT_SUP:
                        USBH_USR_DBG("Manufacturer is N/A");

                        /* Set enum state to get product string descriptor */
                        usbInfo->hostEnumState = USBH_ENUM_GET_PRODUCT_STRING_DESC;
                        break;

                    default:
                        break;
                
                }
            }
            break;

        case USBH_ENUM_GET_PRODUCT_STRING_DESC:
            if (usbInfo->devInfo.device.iProduct == 0)
            {
                USBH_USR_DBG("Product is N/A");
                
                /* Set enum state to get serial number string descriptor */
                usbInfo->hostEnumState = USBH_ENUM_GET_SERIAL_NUM_STRING_DESC;
            }
            else
            {
                /* Get product string descriptor */
                enumStatus = USBH_GetStringDesc(usbInfo, \
                                            usbInfo->devInfo.device.iProduct, \
                                            usbInfo->devInfo.data, \
                                            0xFF);

                switch (enumStatus)
                {
                    case USBH_OK:
                        USBH_USR_DBG("Product: %s", (char *)(void *)usbInfo->devInfo.data);

                        /* Set enum state to get serial number string descriptor */
                        usbInfo->hostEnumState = USBH_ENUM_GET_SERIAL_NUM_STRING_DESC;
                        break;

                    case USBH_ERR_NOT_SUP:
                        USBH_USR_DBG("Product is N/A");

                        /* Set enum state to get serial number string descriptor */
                        usbInfo->hostEnumState = USBH_ENUM_GET_SERIAL_NUM_STRING_DESC;
                        break;

                    default:
                        break;
                }
            }
            break;

        case USBH_ENUM_GET_SERIAL_NUM_STRING_DESC:
            if (usbInfo->devInfo.device.iSerialNumber == 0)
            {
                USBH_USR_DBG("Serial number is N/A");
                usbStatus = USBH_OK;
            }
            else
            {
                enumStatus = USBH_GetStringDesc(usbInfo, \
                                   usbInfo->devInfo.device.iSerialNumber, \
                                   usbInfo->devInfo.data, \
                                   0xFF);
                if (enumStatus == USBH_OK)
                {
                    usbStatus = USBH_OK;
                    USBH_USR_DBG("Serial number: %s", (char *)(void *)usbInfo->devInfo.data);
                }
                else if (enumStatus == USBH_ERR_NOT_SUP)
                {
                    usbStatus = USBH_OK;
                    USBH_USR_DBG("Serial number is N/A");
                }
                else
                {
                    
                }
            }
            break;
    }

    return usbStatus; 
}

/**@} end of group USBH_Core_Functions */
/**@} end of group USBH_Core */
/**@} end of group APM32_USB_Library */
