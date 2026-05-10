/*!
 * @file        usbh_hid.h
 *
 * @brief       USB HID core function head file
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

/* Define to prevent recursive inclusion */
#ifndef USBH_HID_H
#define USBH_HID_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "usbh_core.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_HID_Class
  @{
  */

/** @defgroup USBH_HID_Macros Macros
  @{
*/

#define USBH_HID_CLASS_CODE             0x03
#define USBH_HID_BOOT_CODE              0x01
#define USBH_HID_MOUSE_BOOT_CODE        0x02
#define USBH_HID_KEYBOARD_BOOT_CODE     0x01

#define USBH_HID_QUEUE_MAX_SIZE         10
#define USBH_HID_POLL_MIN_NUM           10

/**@} end of group USBH_HID_Macros*/

/** @defgroup USBH_HID_Enumerates Enumerates
  @{
  */

/**
 * @brief   USB HID device type
 */
typedef enum
{
    HID_MOUSE = 1,
    HID_KEYBOARD,
    HID_JOYSTICK,
    HID_UNKNOWN = 0xFF,
} USBH_HID_DEV_T;

/**
 * @brief    USB HID report type
 */
typedef enum
{
    HID_INPUT_REPORT = 1,
    HID_OUTPUT_REPORT,
    HID_FEATURE_REPORT,
} USBH_HID_REPORT_T;

/**
 * @brief    USB HID request state table
 */
typedef enum
{
    USBH_HID_REQ_INIT,
    USBH_HID_REQ_IDLE,
    USBH_HID_REQ_GET_REP_DESC,
    USBH_HID_REQ_GET_HID_DESC,
    USBH_HID_REQ_SET_IDLE,
    USBH_HID_REQ_SET_PROTOCOL,
    USBH_HID_REQ_SET_REPORT,
} USBH_HID_REQ_STA_T;

/**
 * @brief    USB HID state table
 */
typedef enum
{
    USBH_HID_INIT = 0,
    USBH_HID_IDLE = 1,
    USBH_HID_SYNC,
    USBH_HID_IN_DATA,
    USBH_HID_POLL,
    USBH_HID_OUT_DATA,
    USBH_HID_BUSY,
    USBH_HID_ERR,
} USBH_HID_STATE_T;

/**
 * @brief   USB HID device class requests type
 */
typedef enum
{
    USBH_HID_GET_REPORT         = 1,
    USBH_HID_GET_IDLE,
    USBH_HID_GET_PROTOCOL,
    USBH_HID_SET_REPORT         = 9,
    USBH_HID_SET_IDLE,
    USBH_HID_SET_PROTOCOL,
} USBH_HID_REQ_TYPE_T;

/**@} end of group USBH_HID_Enumerates*/

/** @defgroup USBH_HID_Structures Structures
  @{
  */

/* Host HID class state handler function */
typedef USBH_STA_T(*USBH_HIDStateHandler_T)(USBH_INFO_T* usbInfo);

/**
 * @brief   USB host HID class handler
 */
typedef struct
{
    USBH_STA_T(*InitHandler)(USBH_INFO_T* usbInfo);
    USBH_STA_T(*DecodeHandler)(USBH_INFO_T* usbInfo);
} USBH_HID_CLASS_T;

/**
 * @brief   Reserved structure
 */
typedef struct
{
    uint8_t     *buffer;
    uint16_t    reserved1;
    uint16_t    reserved2;
    uint16_t    reserved3;
    uint8_t     reserved4;
} USBH_HID_RESERVED_T;

/**
 * @brief    USB HID descriptor
 */
typedef struct
{
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bcdHID[2];
    uint8_t     bCountryCode;
    uint8_t     bNumDescriptors;
    uint8_t     bReportDescriptorType;
    uint8_t     wDescriptorLength[2];

} USBH_HID_DESC_T;

/**
 * @brief    HID information management
 */
typedef struct
{
    uint8_t                 outEpAddr;
    uint8_t                 inEpAddr;
    USBH_HID_STATE_T        state;
    uint8_t                 outChNum;
    uint8_t                 inChNum;
    uint8_t                 classReqState;
    USBH_HID_RESERVED_T     reserved;
    uint8_t                 *buffer;
    uint16_t                epSize;
    uint8_t                 epAddr;
    uint16_t                pollInterval;
    uint32_t                timer;
    uint8_t                 dataFlag;
    USBH_HID_DESC_T         desc;
    USBH_HID_CLASS_T        *callback;
} USBH_HID_INFO_T;

extern USBH_CLASS_T USBH_HID_CLASS;

/**@} end of group USBH_HID_Structures*/

/** @defgroup USBH_HID_Functions Functions
  @{
  */

/* HID class control functions */
USBH_STA_T USBH_HID_SetProtocol(USBH_INFO_T* usbInfo, uint8_t protocol);
USBH_STA_T USBH_HID_SetIdle(USBH_INFO_T* usbInfo, uint8_t reportID, uint8_t duration);
USBH_STA_T USBH_HID_SetReport(USBH_INFO_T* usbInfo, uint8_t reportType, \
        uint8_t reportID, uint8_t* buffer, uint8_t length);
USBH_STA_T USBH_HID_GetReport(USBH_INFO_T* usbInfo, uint8_t reportType, \
        uint8_t reportID, uint8_t* buffer, uint8_t length);
USBH_STA_T USBH_HID_GetHIDReportDesc(USBH_INFO_T* usbInfo, uint16_t desLength);
USBH_STA_T USBH_HID_GetHIDDesc(USBH_INFO_T* usbInfo, uint16_t desLength);

uint8_t USBH_HID_ReadPollInterval(USBH_INFO_T* usbInfo);
USBH_HID_DEV_T USBH_HID_ReadDevType(USBH_INFO_T* usbInfo);

void USBH_HID_PollCallback(USBH_INFO_T* usbInfo);

/**@} end of group USBH_HID_Functions */
/**@} end of group USBH_HID_Class */
/**@} end of group APM32_USB_Library */

#ifdef __cplusplus
}
#endif

#endif /* USBH_HID_H */
