/*!
 * @file        usbd_custom_hid_if.h
 *
 * @brief       usb device Custom HID interface handler header file
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
#ifndef _USBD_CUSTOM_HID_IF_H_
#define _USBD_CUSTOM_HID_IF_H_

/* Includes */
#include "usbd_customhid.h"

/** @addtogroup Examples
  * @brief USBD CUSTOM HID Keyboard examples
  @{
  */

/** @addtogroup USBD_CUSTOM_HID_Keyboard
  @{
  */

/** @defgroup USBD_CUSTOM_HID_Keyboard_Variables Variables
  @{
*/

extern USBD_CUSTOM_HID_INTERFACE_T USBD_CUSTOM_HID_INTERFACE_FS;

/**@} end of group USBD_CUSTOM_HID_Keyboard_Variables*/

/** @defgroup USBD_CUSTOM_HID_Keyboard_Functions Functions
  @{
*/

USBD_STA_T USBD_FS_CUSTOM_HID_ItfInit(void);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfDeInit(void);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfSendReport(uint8_t *buffer, uint8_t length);
USBD_STA_T USBD_FS_CUSTOM_HID_ItfReceive(uint8_t *buffer, uint8_t *length);

/**@} end of group USBD_CUSTOM_HID_Keyboard_Functions */
/**@} end of group USBD_CUSTOM_HID_Keyboard */
/**@} end of group Examples */

#endif
