/*!
 * @file        usbh_msc_scsi.c
 *
 * @brief       USB host MSC scsi
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
#include "usbh_msc_scsi.h"
#include "usbh_msc_bot.h"
#include "usbh_msc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_MSC_Class
  @{
  */

/** @defgroup USBH_MSC_Functions Functions
  @{
  */

/*!
 * @brief       USB host MSC SCSI INQUIRY handler
 *
 * @param       usbInfo
 *
 * @param       lun: logical unit number
 *
 * @param       inquiry: inquiry response
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_Inquiry(USBH_INFO_T* usbInfo, uint8_t lun, \
                                 USBH_SCSI_INQUIRY_REQ_T* inquiry)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_IN;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = LEN_XFER_INQUIRY;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_INQUIRY;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[1] = lun << 5;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[2] = 0;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[3] = 0;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[4] = LEN_XFER_INQUIRY;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[5] = 0;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        /* Initialize buffer */
        mscInfo->botInfo.buffer = (uint8_t*)(void*)mscInfo->botInfo.data;
        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);

        switch (usbStatus)
        {
            case USBH_OK:
                (void)memset(inquiry, 0, sizeof(USBH_SCSI_INQUIRY_REQ_T));

                inquiry->devType = mscInfo->botInfo.buffer[0] & 0x1F;
                inquiry->peripheral = mscInfo->botInfo.buffer[0] >> 5;

                if ((mscInfo->botInfo.buffer[1] & 0x80) == 0x80)
                {
                    inquiry->media = 1;
                }
                else
                {
                    inquiry->media = 0;
                }

                (void)memcpy(inquiry->vendorID, &mscInfo->botInfo.buffer[8], 8);
                (void)memcpy(inquiry->productID, &mscInfo->botInfo.buffer[16], 16);
                (void)memcpy(inquiry->revID, &mscInfo->botInfo.buffer[32], 4);
                break;

            default:
                break;
        }
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SCSI TEST UNIT READY handler
 *
 * @param       usbInfo
 *
 * @param       lun: logical unit number
 *
 * @param       inquiry: inquiry response
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_TestUnitReady(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_OUT;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = LEN_XFER_TEST_UNIT_READY;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_TEST_UNIT_READY;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SCSI READ CAPACITY handler
 *
 * @param       usbInfo
 *
 * @param       lun: logical unit number
 *
 * @param       capacity: capacity response
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_ReadCapacity(USBH_INFO_T* usbInfo, uint8_t lun, \
                                      USBH_SCSI_READ_CAPACITY_REQ_T* capacity)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_IN;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = LEN_XFER_READ_CAPACITY;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_READ_CAPACITY;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        /* Initialize buffer */
        mscInfo->botInfo.buffer = (uint8_t*)(void*)mscInfo->botInfo.data;
        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);

        switch (usbStatus)
        {
            case USBH_OK:
                capacity->blockNum = (mscInfo->botInfo.buffer[0] << 24) | \
                                        (mscInfo->botInfo.buffer[1] << 16) | \
                                        (mscInfo->botInfo.buffer[2] << 8) | \
                                        mscInfo->botInfo.buffer[3];

                capacity->blockSize = (mscInfo->botInfo.buffer[4] << 24) | \
                                        (mscInfo->botInfo.buffer[5] << 16) | \
                                        (mscInfo->botInfo.buffer[6] << 8) | \
                                        mscInfo->botInfo.buffer[7];
                break;

            default:
                break;
        }
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SCSI request sense handler
 *
 * @param       usbInfo
 *
 * @param       lun: logical unit number
 *
 * @param       sense: sense response
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_RequestSense(USBH_INFO_T* usbInfo, uint8_t lun, \
                                      USBH_SCSI_SENSE_REQ_T* sense)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_IN;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = LEN_XFER_REQUEST_SENSE;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_REQUEST_SENSE;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[1] = lun << 5;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[2] = 0;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[3] = 0;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[4] = LEN_XFER_REQUEST_SENSE;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[5] = 0;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        /* Initialize buffer */
        mscInfo->botInfo.buffer = (uint8_t*)(void*)mscInfo->botInfo.data;
        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);

        switch (usbStatus)
        {
            case USBH_OK:
                sense->key = mscInfo->botInfo.buffer[2] & 0x0F;
                sense->asc = mscInfo->botInfo.buffer[12];
                sense->ascq = mscInfo->botInfo.buffer[13];
                break;

            default:
                break;
        }
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SCSI read10 handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @param       address: sector address
 *
 * @param       buffer: buffer point to data
 *
 * @param       cnt: count number of data
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_Read(USBH_INFO_T* usbInfo, uint8_t lun, uint32_t address, \
                              uint8_t* buffer, uint16_t cnt)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_IN;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = cnt * mscInfo->unit[0].capacity.blockSize;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_READ_10;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[2] = (address >> 24) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[3] = (address >> 16) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[4] = (address >> 8) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[5] = address & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[7] = (cnt >> 8) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[8] = cnt & 0xFF;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        /* Initialize buffer */
        mscInfo->botInfo.buffer = buffer;
        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SCSI write10 handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @param       address: sector address
 *
 * @param       buffer: buffer point to data
 *
 * @param       cnt: count number of data
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_SCSI_Write(USBH_INFO_T* usbInfo, uint8_t lun, uint32_t address, \
                               uint8_t* buffer, uint16_t cnt)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->botInfo.xferState == USBH_BOT_XFER_START)
    {
        mscInfo->botInfo.cbw.DATA_FIELD.bmFlags.CBW_FLAG_B.dir = USBH_REQ_DIR_OUT;
        mscInfo->botInfo.cbw.DATA_FIELD.dDataXferLen = cnt * mscInfo->unit[0].capacity.blockSize;
        mscInfo->botInfo.cbw.DATA_FIELD.bCBLen = USBH_LEN_CBW;

        (void)memset(mscInfo->botInfo.cbw.DATA_FIELD.CB, 0, USBH_BOT_CBW_CB_LEN);

        mscInfo->botInfo.cbw.DATA_FIELD.CB[0] = USBH_SCSI_CMD_WRITE_10;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[2] = (address >> 24) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[3] = (address >> 16) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[4] = (address >> 8) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[5] = address & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[7] = (cnt >> 8) & 0xFF;
        mscInfo->botInfo.cbw.DATA_FIELD.CB[8] = cnt & 0xFF;

        mscInfo->botInfo.xferState = USBH_BOT_XFER_WAITING;
        mscInfo->botInfo.state = USBH_BOT_SEND_CBW;

        /* Initialize buffer */
        mscInfo->botInfo.buffer = buffer;
        usbStatus = USBH_BUSY;
    }
    else if (mscInfo->botInfo.xferState == USBH_BOT_XFER_WAITING)
    {
        usbStatus = USBH_MSC_BOT_StateHandler(usbInfo, lun);
    }
    else
    {
        /* Do nothing */
    }

    return usbStatus;
}

/**@} end of group USBH_MSC_Functions */
/**@} end of group USBH_MSC_Class */
/**@} end of group APM32_USB_Library */
