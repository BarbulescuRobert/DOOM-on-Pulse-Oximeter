/*!
 * @file        usb_device_user.h
 *
 * @brief       usb device user function
 *
 * @version     V1.0.0
 *
 * @date        2025-07-04
 *
 * @attention
 *
 *  Copyright (C) 2025 Geehy Semiconductor
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
#ifndef _USB_DEVICE_USER_H_
#define _USB_DEVICE_USER_H_

/* Includes */
#include "apm32f0xx.h"
#include "usbd_core.h"

/** @addtogroup Examples
  * @brief USBD CUSTOM HID Keyboard examples
  @{
  */

/** @addtogroup USBD_CUSTOM_HID_Keyboard
  @{
  */

/** @defgroup USBD_CUSTOM_HID_Keyboard_Enumerates Enumerates
  @{
  */

/**
 * @brief    USB device application status
 */
typedef enum
{
    USBD_APP_IDLE,
    USBD_APP_SUSPEND,
    USBD_APP_READY,
} USBD_APP_STA_T;

/**@} end of group USBD_CUSTOM_HID_Keyboard_Enumerates*/

/** @defgroup USBD_CUSTOM_HID_Keyboard_Variables Variables
  @{
  */

extern USBD_APP_STA_T gUsbDevAppStatus;
extern USBD_INFO_T gUsbDeviceFS;

/**@} end of group USBD_CUSTOM_HID_Keyboard_Variables*/

/** @defgroup USBD_CUSTOM_HID_Keyboard_Functions Functions
  @{
  */

void USB_DeviceInit(void);
void USB_DeviceReset(void);
void USB_DevUserApplication(void);

/**@} end of group USBD_CUSTOM_HID_Keyboard_Functions */
/**@} end of group USBD_CUSTOM_HID_Keyboard */
/**@} end of group Examples */

#endif
