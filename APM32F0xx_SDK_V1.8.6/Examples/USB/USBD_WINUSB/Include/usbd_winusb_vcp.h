/*!
 * @file        usbd_winusb_vcp.h
 *
 * @brief       usb device WINUSB class VCP handler header file
 *
 * @version     V1.0.0
 *
 * @date        2023-01-16
 *
 * @attention
 *
 *  Copyright (C) 2023 Geehy Semiconductor
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
#ifndef _USBD_WINUSB_VCP_H_
#define _USBD_WINUSB_VCP_H_

/* Includes */
#include "usbd_winusb.h"

/** @addtogroup Examples
  * @brief USBD WINUSB examples
  @{
  */

/** @addtogroup USBD_WINUSB
  @{
  */

/** @defgroup USBD_WINUSB_Macros Macros
  @{
*/

#define USBD_WINUSB_TX_BUF_LEN         1024
#define USBD_WINUSB_RX_BUF_LEN         1024

/**@} end of group USBD_WINUSB_Macros*/

/** @defgroup USBD_WINUSB_Enumerates Enumerates
  @{
  */

/**
 * @brief   USB device WINUSB VCP status
 */
typedef enum
{
    USBD_WINUSB_VCP_IDLE,
    USBD_WINUSB_VCP_REV_UPDATE,
} USBD_WINUSB_VCP_STA_T;

/**@} end of group USBD_WINUSB_Enumerates*/

/** @defgroup USBD_WINUSB_Structures Structures
  @{
  */

/**
 * @brief   USB device WINUSB VCP information
 */
typedef struct
{
    uint32_t    rxUpdateLen;
    uint8_t     state;
} USBD_WINUSB_VCP_INFO_T;

/**@} end of group USBD_WINUSB_Structures*/

/** @defgroup USBD_WINUSB_Variables Variables
  @{
  */

extern USBD_WINUSB_INTERFACE_T USBD_WINUSB_INTERFACE_FS;
extern USBD_WINUSB_VCP_INFO_T gUsbVCP;
extern uint8_t winusbTxBuffer[USBD_WINUSB_TX_BUF_LEN];
extern uint8_t winusbRxBuffer[USBD_WINUSB_RX_BUF_LEN];

/**@} end of group USBD_WINUSB_Variables*/

/** @defgroup USBD_WINUSB_Functions Functions
  @{
  */

USBD_STA_T USBD_FS_WINUSB_ItfInit(void);
USBD_STA_T USBD_FS_WINUSB_ItfDeInit(void);
USBD_STA_T USBD_FS_WINUSB_ItfCtrl(uint8_t command, uint8_t *buffer, uint16_t length);
USBD_STA_T USBD_FS_WINUSB_ItfSend(uint8_t *buffer, uint16_t length);
USBD_STA_T USBD_FS_WINUSB_ItfSendEnd(uint8_t epNum, uint8_t *buffer, uint32_t *length);
USBD_STA_T USBD_FS_WINUSB_ItfReceive(uint8_t *buffer, uint32_t *length);

/**@} end of group USBD_WINUSB_Functions */
/**@} end of group USBD_WINUSB */
/**@} end of group Examples */

#endif
