/*!
 * @file        usbd_cdc_vcp.c
 *
 * @brief       usb device CDC class Virtual Com Port handler
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

/* Includes */
#include "usbd_cdc_vcp.h"
#include "usb_device_user.h"

/** @addtogroup Examples
  * @brief USBD CDC examples
  @{
  */

/** @addtogroup USBD_CDC
  @{
  */

/** @defgroup USBD_CDC_Variables Variables
  @{
  */

uint8_t cdcTxBuffer[USBD_CDC_TX_BUF_LEN];
uint8_t cdcRxBuffer[USBD_CDC_RX_BUF_LEN];

/**@} end of group USBD_CDC_Variables*/

/** @defgroup USBD_CDC_Structures Structures
  @{
  */

USBD_CDC_VCP_INFO_T gUsbVCP;

/* USB FS CDC interface handler */
USBD_CDC_INTERFACE_T USBD_CDC_INTERFACE_FS =
{
    "CDC Interface FS",
    USBD_FS_CDC_ItfInit,
    USBD_FS_CDC_ItfDeInit,
    USBD_FS_CDC_ItfCtrl,
    USBD_FS_CDC_ItfSend,
    USBD_FS_CDC_ItfSendEnd,
    USBD_FS_CDC_ItfReceive,
};

/**@} end of group USBD_CDC_Structures*/

/** @defgroup USBD_CDC_Functions Functions
  @{
  */

/*!
 * @brief       USB device initializes CDC media handler
 *
 * @param       None
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfInit(void)
{
    USBD_STA_T usbStatus = USBD_OK;
    
    USBD_CDC_ConfigRxBuffer(&gUsbDeviceFS, cdcRxBuffer);
    USBD_CDC_ConfigTxBuffer(&gUsbDeviceFS, cdcTxBuffer, 0);
    
    return usbStatus;
}

/*!
 * @brief       USB device deinitializes CDC media handler
 *
 * @param       None
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfDeInit(void)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device CDC interface control request handler
 *
 * @param       command: Command code
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfCtrl(uint8_t command, uint8_t *buffer, uint16_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    switch(command)
    {
        case USBD_CDC_SEND_ENCAPSULATED_COMMAND:

        break;
        case USBD_CDC_GET_ENCAPSULATED_RESPONSE:

        break;
        case USBD_CDC_SET_COMM_FEATURE:

        break;
        case USBD_CDC_GET_COMM_FEATURE:

        break;
        case USBD_CDC_CLEAR_COMM_FEATURE:

        break;
        case USBD_CDC_SET_LINE_CODING:

        break;
        case USBD_CDC_GET_LINE_CODING:

        break;
        case USBD_CDC_SET_CONTROL_LINE_STATE:

        break;
        case USBD_CDC_SEND_BREAK:

        break;
        default:

        break;
    }
    return usbStatus;
}

/*!
 * @brief       USB device CDC interface send handler
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfSend(uint8_t *buffer, uint16_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_CDC_INFO_T *usbDevCDC = (USBD_CDC_INFO_T*)gUsbDeviceFS.devClass[gUsbDeviceFS.classID]->classData;
    
    if(usbDevCDC->cdcTx.state != USBD_CDC_XFER_IDLE)
    {
        return USBD_BUSY;
    }
    
    USBD_CDC_ConfigTxBuffer(&gUsbDeviceFS, buffer, length);
    
    usbStatus = USBD_CDC_TxPacket(&gUsbDeviceFS);
    
    return usbStatus;
}

/*!
 * @brief       USB device CDC interface send end event handler
 *
 * @param       epNum: endpoint number
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfSendEnd(uint8_t epNum, uint8_t *buffer, uint32_t *length)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device CDC interface receive handler
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_CDC_ItfReceive(uint8_t *buffer, uint32_t *length)
{
    USBD_STA_T usbStatus = USBD_OK;
    
    USBD_CDC_ConfigRxBuffer(&gUsbDeviceFS, &buffer[0]);
    
    USBD_CDC_RxPacket(&gUsbDeviceFS);
    
    gUsbVCP.state = USBD_CDC_VCP_REV_UPDATE;
    gUsbVCP.rxUpdateLen = *length;
    
    return usbStatus;
}

/**@} end of group USBD_CDC_Functions */
/**@} end of group USBD_CDC */
/**@} end of group Examples */
