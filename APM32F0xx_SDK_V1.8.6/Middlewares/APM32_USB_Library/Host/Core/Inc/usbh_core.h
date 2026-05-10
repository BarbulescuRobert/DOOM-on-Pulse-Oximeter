/*!
 * @file        usbh_core.h
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

/* Define to prevent recursive inclusion */
#ifndef USBH_CORE_H
#define USBH_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "usbh_config.h"
#include "usbh_channel.h"
#include "usbh_stdReq.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_Core
  @{
  */

/** @defgroup USBH_Core_Enumerates Enumerates
  @{
  */

/**
 * @brief   USB host user status
 */
typedef enum
{
    USBH_USER_CONNECTION = 1,
    USBH_USER_DISCONNECTION,
    USBH_USER_DETECTED_SPEED,
    USBH_USER_ENUMERATION,
    USBH_USER_SELECT_CONFIGURATION,
    USBH_USER_CLASS_LAUNCHED,
    USBH_USER_NOT_SUPPORT,
    USBH_USER_ERROR,
} USBH_USER_STATUS;

/**@} end of group USBH_Core_Enumerates*/

/** @defgroup USBH_Core_Functions Functions
  @{
  */

void USBH_Start(USBH_INFO_T* usbInfo);
void USBH_Stop(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_Disconnect(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_Connect(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_ReEnumerate(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_PortEnable(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_PortDisable(USBH_INFO_T* usbInfo);
uint8_t USBH_PortIsEnabled(USBH_INFO_T* usbInfo);
void USBH_IncTimer(USBH_INFO_T* usbInfo);
void USBH_ConfigTimer(USBH_INFO_T* usbInfo, uint32_t tick);

void USBH_HardwareInit(USBH_INFO_T* usbInfo);
void USBH_HardwareReset(USBH_INFO_T* usbInfo);

void USBH_StartCallback(USBH_INFO_T* usbInfo);
void USBH_StopCallback(USBH_INFO_T* usbInfo);
void USBH_StopHostCallback(USBH_INFO_T* usbInfo);
void USBH_ConfigVbusCallback(USBH_INFO_T* usbInfo, uint8_t status);
void USBH_ResetCallback(USBH_INFO_T* usbInfo);
uint8_t USBH_ReadSpeedCallback(USBH_INFO_T* usbInfo);
void USBH_OpenChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, \
                              uint8_t endPointNum, uint8_t devAddr, \
                              uint8_t devSpeed, uint8_t epType, uint16_t packetMaxSize);
void USBH_CloseChannelCallback(USBH_INFO_T* usbInfo, uint8_t channelNum);
void USBH_UrbSubmitCallback(USBH_INFO_T* usbInfo, uint8_t chNum, uint8_t dir, \
                            uint8_t epType, uint8_t tokenType, uint8_t* buffer, \
                            uint16_t length, uint8_t pingStatus);

uint8_t USBH_ReadUrbStatusCallback(USBH_INFO_T* usbInfo, uint8_t channelNum);
uint32_t USBH_ReadLastXferSizeCallback(USBH_INFO_T* usbInfo, uint8_t channelNum);
uint8_t USBH_ReadToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum);
void USBH_ConfigToggleCallback(USBH_INFO_T* usbInfo, uint8_t channelNum, uint8_t toggle);
void USBH_DelayCallback(uint32_t tick);

USBH_STA_T USBH_Init(USBH_INFO_T* usbInfo, USBH_SPEED_T usbHostSpeed, USBH_CLASS_T* usbHostClass,
                     void (*userCallbackFunc)(struct _USBH_INFO_T*, uint8_t));
USBH_STA_T USBH_DeInit(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_RegisterClass(USBH_INFO_T* usbInfo, USBH_CLASS_T* usbHostClass);
uint8_t USBH_ReadActiveClass(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_PollingProcess(USBH_INFO_T* usbInfo);

/**@} end of group USBH_Core_Functions */
/**@} end of group USBH_Core */
/**@} end of group APM32_USB_Library */

#ifdef __cplusplus
}
#endif

#endif /* USBH_CORE_H */
