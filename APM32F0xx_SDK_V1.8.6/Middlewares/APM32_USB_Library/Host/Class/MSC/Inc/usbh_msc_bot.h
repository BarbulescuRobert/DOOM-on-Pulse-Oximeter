/*!
 * @file        usbh_msc_bot.h
 *
 * @brief       USB host MSC bot
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
#ifndef USBH_MSC_BOT_H
#define USBH_MSC_BOT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "usbh_core.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_MSC_Class
  @{
  */

/** @defgroup USBH_MSC_Macros Macros
  @{
*/

/* CBW parameter */
#define USBH_MSC_BOT_CBW_SIGNATURE               (uint32_t)(0x43425355)
#define USBH_MSC_BOT_CBW_TAG                     (uint32_t)(0x20304050)
#define USBH_MSC_BOT_CBW_LEN                     31
#define USBH_BOT_CBW_CB_LEN                      16
#define USBH_LEN_CBW                             10

/* CSW parameter */
#define USBH_MSC_BOT_CSW_SIGNATURE               (uint32_t)(0x53425355)
#define USBH_MSC_BOT_CSW_LEN                     13
#define USBH_LEN_CSW_MAX                         63

/**@} end of group USBH_MSC_Macros*/

/** @defgroup USBH_MSC_Enumerates Enumerates
  @{
  */

/**
 * @brief    SCSI transmission state of BOT
 */
typedef enum
{
    USBH_BOT_SEND_CBW,
    USBH_BOT_SEND_CBW_WAIT,
    USBH_BOT_DATA_IN,
    USBH_BOT_DATA_IN_WAIT,
    USBH_BOT_DATA_OUT,
    USBH_BOT_DATA_OUT_WAIT,
    USBH_BOT_RECEIVE_CSW,
    USBH_BOT_RECEIVE_CSW_WAIT,
    USBH_BOT_ERROR_IN,
    USBH_BOT_ERROR_OUT,
    USBH_BOT_ERROR_UNRECOVERED,
} USBH_BOT_STATE_T;

/**
 * @brief    CSW status
 */
typedef enum
{
    USBH_BOT_CSW_OK,
    USBH_BOT_CSW_FAIL,
    USBH_BOT_CSW_ERR,
} USBH_BOT_CSW_STA_T;

/**
 * @brief    SCSI transmission command state of BOT
 */
typedef enum
{
    USBH_BOT_XFER_IDLE,
    USBH_BOT_XFER_START,
    USBH_BOT_XFER_WAITING,
} USBH_BOT_XFER_STA_T;

/**@} end of group USBH_MSC_Enumerates*/

/** @defgroup USBH_MSC_Structures Structures
  @{
  */

/**
 * @brief    Command Status Wrapper
 */
typedef union
{
    struct
    {
        uint32_t dSignature;
        uint32_t dTag;
        uint32_t dDataResidue;
        uint8_t  bStatus;
    } DATA_FIELD;

    uint8_t buffer[13];
} USBH_BOT_CSW_T;

/**
 * @brief    CBW flag
 */
typedef union
{
    uint8_t CBW_Flag;

    struct
    {
        uint8_t reserved : 7;
        uint8_t dir      : 1;
    } CBW_FLAG_B;

} USBH_BOT_CBW_FLAG_T;

/**
 * @brief    Command Block Wrapper
 */
typedef union
{
    struct
    {
        uint32_t            dSignature;
        uint32_t            dTag;
        uint32_t            dDataXferLen;
        USBH_BOT_CBW_FLAG_T bmFlags;
        uint8_t             bLUN;
        uint8_t             bCBLen;
        uint8_t             CB[16];
    } DATA_FIELD;

    uint8_t buffer[31];
} USBH_BOT_CBW_T;

/**
 * @brief    MSC BOT information
 */
typedef struct
{
    uint32_t            data[16];
    USBH_BOT_STATE_T    state;
    USBH_BOT_STATE_T    preState;
    uint8_t             xferState;
    USBH_BOT_CBW_T      cbw;
    uint8_t             reserved1;      /* Reserved for alignment */
    USBH_BOT_CSW_T      csw;
    uint8_t             reserved2[3];   /* Reserved for alignment */
    uint8_t             *buffer;
} USBH_BOT_INFO_T;

/**@} end of group USBH_MSC_Structures*/

/** @defgroup USBH_MSC_Functions Functions
  @{
  */

USBH_STA_T USBH_MSC_BOT_Init(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_MSC_BOT_REQ_Reset(USBH_INFO_T* usbInfo);
USBH_STA_T USBH_MSC_BOT_REQ_GetMaxLun(USBH_INFO_T* usbInfo, uint8_t* maxLun);
USBH_STA_T USBH_MSC_BOT_StateHandler(USBH_INFO_T* usbInfo, uint8_t lun);

/**@} end of group USBH_MSC_Functions */
/**@} end of group USBH_MSC_Class */
/**@} end of group APM32_USB_Library */

#ifdef __cplusplus
}
#endif

#endif /* USBH_MSC_BOT_H */
