/*!
 * @file        usbd_cdc_vcp.h
 *
 * @brief       usb device CDC class VCP handler header file
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
#ifndef _USBD_CDC_VCP_H_
#define _USBD_CDC_VCP_H_

/* Includes */
#include "usbd_cdc.h"

/** @addtogroup Examples
  * @brief USBD CDC examples
  @{
  */

/** @addtogroup USBD_CDC
  @{
  */

/** @defgroup USBD_CDC_Macros Macros
  @{
*/

#define USBD_CDC_TX_BUF_LEN         1024
#define USBD_CDC_RX_BUF_LEN         1024

/**@} end of group USBD_CDC_Macros*/

/** @defgroup USBD_CDC_Enumerates Enumerates
  @{
  */

/**
 * @brief   USB device CDC VCP status
 */
typedef enum
{
    USBD_CDC_VCP_IDLE,
    USBD_CDC_VCP_REV_UPDATE,
} USBD_CDC_VCP_STA_T;

/**@} end of group USBD_CDC_Enumerates*/

/** @defgroup USBD_CDC_Structures Structures
  @{
  */

/**
 * @brief   USB device CDC VCP information
 */
typedef struct
{
    uint32_t    rxUpdateLen;
    uint8_t     state;
} USBD_CDC_VCP_INFO_T;

/**@} end of group USBD_CDC_Structures*/

/** @defgroup USBD_CDC_Variables Variables
  @{
  */

extern USBD_CDC_INTERFACE_T USBD_CDC_INTERFACE_FS;
extern USBD_CDC_VCP_INFO_T gUsbVCP;
extern uint8_t cdcTxBuffer[USBD_CDC_TX_BUF_LEN];
extern uint8_t cdcRxBuffer[USBD_CDC_RX_BUF_LEN];

/**@} end of group USBD_CDC_Variables*/

/** @defgroup USBD_CDC_Functions Functions
  @{
  */

USBD_STA_T USBD_FS_CDC_ItfInit(void);
USBD_STA_T USBD_FS_CDC_ItfDeInit(void);
USBD_STA_T USBD_FS_CDC_ItfCtrl(uint8_t command, uint8_t *buffer, uint16_t length);
USBD_STA_T USBD_FS_CDC_ItfSend(uint8_t *buffer, uint16_t length);
USBD_STA_T USBD_FS_CDC_ItfSendEnd(uint8_t epNum, uint8_t *buffer, uint32_t *length);
USBD_STA_T USBD_FS_CDC_ItfReceive(uint8_t *buffer, uint32_t *length);

/**@} end of group USBD_CDC_Functions */
/**@} end of group USBD_CDC */
/**@} end of group Examples */

#endif
