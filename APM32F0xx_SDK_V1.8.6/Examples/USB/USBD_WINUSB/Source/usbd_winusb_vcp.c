/*!
 * @file        usbd_winusb_vcp.c
 *
 * @brief       usb device WINUSB class Virtual Com Port handler
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

/* Includes */
#include "usbd_winusb_vcp.h"
#include "usb_device_user.h"

/** @addtogroup Examples
  * @brief USBD WINUSB examples
  @{
  */

/** @addtogroup USBD_WINUSB
  @{
  */

/** @defgroup USBD_WINUSB_Variables Variables
  @{
  */

uint8_t winusbTxBuffer[USBD_WINUSB_TX_BUF_LEN];
uint8_t winusbRxBuffer[USBD_WINUSB_RX_BUF_LEN];

/**@} end of group USBD_WINUSB_Variables*/

/** @defgroup USBD_WINUSB_Structures Structures
  @{
  */

USBD_WINUSB_VCP_INFO_T gUsbVCP;

/* USB FS CDC interface handler */
USBD_WINUSB_INTERFACE_T USBD_WINUSB_INTERFACE_FS =
{
    "WINUSB Interface FS",
    USBD_FS_WINUSB_ItfInit,
    USBD_FS_WINUSB_ItfDeInit,
    USBD_FS_WINUSB_ItfCtrl,
    USBD_FS_WINUSB_ItfSend,
    USBD_FS_WINUSB_ItfSendEnd,
    USBD_FS_WINUSB_ItfReceive,
};

/**@} end of group USBD_WINUSB_Structures*/

/** @defgroup USBD_WINUSB_Functions Functions
  @{
  */

/*!
 * @brief       USB device initializes WINUSB media handler
 *
 * @param       None
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfInit(void)
{
    USBD_STA_T usbStatus = USBD_OK;
    
    USBD_WINUSB_ConfigRxBuffer(&gUsbDeviceFS, winusbRxBuffer);
    USBD_WINUSB_ConfigTxBuffer(&gUsbDeviceFS, winusbTxBuffer, 0);
    
    return usbStatus;
}

/*!
 * @brief       USB device deinitializes WINUSB media handler
 *
 * @param       None
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfDeInit(void)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device WINUSB interface control request handler
 *
 * @param       command: Command code
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfCtrl(uint8_t command, uint8_t *buffer, uint16_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device WINUSB interface send handler
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfSend(uint8_t *buffer, uint16_t length)
{
    USBD_STA_T usbStatus = USBD_OK;

    USBD_WINUSB_INFO_T *usbDevCDC = (USBD_WINUSB_INFO_T*)gUsbDeviceFS.devClass[gUsbDeviceFS.classID]->classData;
    
    if(usbDevCDC->winusbTx.state != USBD_WINUSB_XFER_IDLE)
    {
        return USBD_BUSY;
    }
    
    USBD_WINUSB_ConfigTxBuffer(&gUsbDeviceFS, buffer, length);
    
    usbStatus = USBD_WINUSB_TxPacket(&gUsbDeviceFS);
    
    return usbStatus;
}

/*!
 * @brief       USB device WINUSB interface send end event handler
 *
 * @param       epNum: endpoint number
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfSendEnd(uint8_t epNum, uint8_t *buffer, uint32_t *length)
{
    USBD_STA_T usbStatus = USBD_OK;

    return usbStatus;
}

/*!
 * @brief       USB device WINUSB interface receive handler
 *
 * @param       buffer: Command data buffer
 *
 * @param       length: Command data length
 *
 * @retval      USB device operation status
 */
USBD_STA_T USBD_FS_WINUSB_ItfReceive(uint8_t *buffer, uint32_t *length)
{
    USBD_STA_T usbStatus = USBD_OK;
    
    USBD_WINUSB_ConfigRxBuffer(&gUsbDeviceFS, &buffer[0]);
    
    USBD_WINUSB_RxPacket(&gUsbDeviceFS);
    
    gUsbVCP.state = USBD_WINUSB_VCP_REV_UPDATE;
    gUsbVCP.rxUpdateLen = *length;
    
    return usbStatus;
}

/**@} end of group USBD_WINUSB_Functions */
/**@} end of group USBD_WINUSB */
/**@} end of group Examples */
