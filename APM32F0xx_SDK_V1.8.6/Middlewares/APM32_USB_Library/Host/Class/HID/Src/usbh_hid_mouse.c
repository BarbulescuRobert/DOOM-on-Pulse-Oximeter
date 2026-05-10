/*!
 * @file        usbh_hid_mouse.c
 *
 * @brief       USB host HID mouse function
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
#include "usbh_hid_mouse.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_HID_Class
  @{
  */

/** @defgroup USBH_HID_Structures Structures
  @{
  */

USBH_HID_MOUSE_INFO_T hidMouseInfo;

/* HID mouse class handler */
USBH_HID_CLASS_T USBH_HID_MOUSE_Handler =
{
    USBH_HID_MouseInit,
    USBH_HID_MouseDecode,
};

/**@} end of group USBH_HID_Structures*/

/** @defgroup USBH_HID_Functions Functions
  @{
  */

/*!
 * @brief       USB host HID init mouse
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_HID_MouseInit(USBH_INFO_T* usbInfo)
{
    USBH_STA_T  usbStatus = USBH_OK;
    USBH_HID_INFO_T* hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;
    uint16_t i;

    hidMouseInfo.x = 0;
    hidMouseInfo.y = 0;

    for (i = 0; i < USBH_HID_MOUSE_BUTTON_MAX_NUM; i++)
    {
        hidMouseInfo.button[i] = 0;
    }

    for (i = 0; i < (sizeof(hidMouseInfo.report.data) \
                     / sizeof(hidMouseInfo.report.data[0])); i++)
    {
        hidMouseInfo.report.data[i] = 0;
        hidMouseInfo.report.rxBuffer[i] = 0;
    }

    if (hidInfo->epSize > sizeof(hidMouseInfo.report.data))
    {
        hidInfo->epSize = (uint16_t)sizeof(hidMouseInfo.report.data);
    }

    /* Init buffer point */
    hidInfo->buffer = (uint8_t*)(void*)hidMouseInfo.report.data;

    return usbStatus;
}

/*!
 * @brief       USB host HID decode mouse
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_HID_MouseDecode(USBH_INFO_T* usbInfo)
{
    USBH_STA_T  usbStatus = USBH_OK;

    uint8_t buttonTemp;
    uint8_t coordinateX;
    uint8_t coordinateY;
    uint8_t coordinateZ;

    UNUSED(usbInfo);

    buttonTemp = *((uint8_t*)(void*)hidMouseInfo.report.data + 0);
    coordinateX = *((uint8_t*)(void*)hidMouseInfo.report.data + 1);
    coordinateY = *((uint8_t*)(void*)hidMouseInfo.report.data + 2);
    coordinateZ = *((uint8_t*)(void*)hidMouseInfo.report.data + 3);

    hidMouseInfo.button[0] = (buttonTemp & 0x01) > 0 ? 1 : 0;
    hidMouseInfo.button[1] = (buttonTemp & 0x02) > 0 ? 1 : 0;
    hidMouseInfo.button[2] = (buttonTemp & 0x04) > 0 ? 1 : 0;

    hidMouseInfo.x         = coordinateX;
    hidMouseInfo.y         = coordinateY;
    hidMouseInfo.z         = coordinateZ;

    return usbStatus;
}

/*!
 * @brief       USB host HID mouse data process
 *
 * @param       usbInfo: usb host information
 *
 * @retval      mouse information
 */
__weak USBH_HID_MOUSE_INFO_T *USBH_HID_MouseCallback(USBH_INFO_T* usbInfo)
{
    if (USBH_HID_MouseDecode(usbInfo) == USBH_OK)
    {
        return &hidMouseInfo;
    }

    return NULL;
}

/**@} end of group USBH_HID_Functions */
/**@} end of group USBH_HID_Class */
/**@} end of group APM32_USB_Library */
