/*!
 * @file        usbh_msc.c
 *
 * @brief       usb host msc class handler
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
#include "usbh_msc.h"
#include "usbh_msc_bot.h"
#include "usbh_msc_scsi.h"
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

static USBH_STA_T USBH_MSC_ClassInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_MSC_ClassDeInitHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_MSC_ClassReqHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_MSC_CoreHandler(USBH_INFO_T* usbInfo);
static USBH_STA_T USBH_MSC_SOFHandler(USBH_INFO_T* usbInfo);

static USBH_STA_T USBH_MSC_IdleHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_InitHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_InquiryHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_TestUnitReadyHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_RequestSenseHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_ReadCapacityHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_ErrorUnrecoveredHandler(USBH_INFO_T* usbInfo, uint8_t lun);

static USBH_STA_T USBH_MSC_ReadHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_WriteHandler(USBH_INFO_T* usbInfo, uint8_t lun);
static USBH_STA_T USBH_MSC_RWRequestSenseHandler(USBH_INFO_T* usbInfo, uint8_t lun);

/**@} end of group USBH_MSC_Functions */

/** @defgroup USBH_MSC_Structures Structures
  @{
  */

/* MSC class handler */
USBH_CLASS_T USBH_MSC_CLASS =
{
    "Class MSC",
    USBH_CLASS_MSC,
    USBH_MSC_ClassInitHandler,
    USBH_MSC_ClassDeInitHandler,
    USBH_MSC_ClassReqHandler,
    USBH_MSC_CoreHandler,
    USBH_MSC_SOFHandler,
    NULL,
};

/* USB host MSC state handler function */
USBH_MscStateHandler_T USBH_MSC_Handler[] =
{
    USBH_MSC_InitHandler,
    USBH_MSC_IdleHandler,
    USBH_MSC_InquiryHandler,
    USBH_MSC_TestUnitReadyHandler,
    USBH_MSC_RequestSenseHandler,
    USBH_MSC_ReadCapacityHandler,
    USBH_MSC_ErrorUnrecoveredHandler,
    USBH_MSC_ReadHandler,
    USBH_MSC_WriteHandler,
    USBH_MSC_RWRequestSenseHandler,
};

/**@} end of group USBH_MSC_Structures*/

/** @defgroup USBH_MSC_Functions Functions
  @{
  */

/*!
 * @brief       USB host MSC read unit information
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @param       device: device unit information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_MSC_ReadDevInfo(USBH_INFO_T* usbInfo, uint8_t lun, USBH_MSC_STORAGE_INFO_T* device)
{
    USBH_STA_T usbStatus = USBH_FAIL;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        (void)memcpy(device, &mscInfo->unit[lun], sizeof(USBH_MSC_STORAGE_INFO_T));
        usbStatus = USBH_OK;
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC read device ready status
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      Unit ready status
 *              @arg 0: not ready
 *              @arg 1: ready
 */
uint8_t USBH_MSC_DevStatus(USBH_INFO_T* usbInfo, uint8_t lun)
{
    uint8_t status = 0;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        if (mscInfo->unit[lun].errState == USBH_MSC_OK)
        {
            status = 1;
        }
    }

    return status;
}

/*!
 * @brief       USB host MSC read device WP status
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      WP status
 *              @arg DISABLE: not protect
 *              @arg 1: protect
 */
uint8_t USBH_MSC_ReadDevWP(USBH_INFO_T* usbInfo, uint8_t lun)
{
    uint8_t wpStatus = DISABLE;
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (usbInfo->hostState == USBH_HOST_CLASS)
    {
        if (mscInfo->unit[lun].sense.asc == USBH_SCSI_ASC_WRITE_PROTECTED)
        {
            wpStatus = ENABLE;
        }
    }

    return wpStatus;
}

/*!
 * @brief       USB host MSC read unit information
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
USBH_STA_T USBH_MSC_DevRead(USBH_INFO_T* usbInfo, uint8_t lun, uint32_t address, \
                            uint8_t* buffer, uint16_t cnt)
{
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    uint32_t timeout;

    if ((usbInfo->devInfo.connectedStatus == DISABLE) || \
        (usbInfo->hostState != USBH_HOST_CLASS) || \
        (mscInfo->unit[lun].state != USBH_MSC_IDLE))
        {
            return USBH_FAIL;
        }

    mscInfo->state = USBH_MSC_RW_READ;
    mscInfo->unit[lun].state = USBH_MSC_RW_READ;
    mscInfo->opLun = lun;

    (void)USBH_MSC_SCSI_Read(usbInfo, lun, address, buffer, cnt);

    timeout = usbInfo->timer;

    while (USBH_MSC_Handler[mscInfo->unit[lun].state](usbInfo, lun) == USBH_BUSY)
    {
        if (((usbInfo->timer - timeout) > (0x2FFF * cnt)) || \
            (usbInfo->devInfo.connectedStatus == DISABLE))
            {
                mscInfo->state = USBH_MSC_IDLE;
                return USBH_FAIL;
            }
    }

    mscInfo->state = USBH_MSC_IDLE;

    return USBH_OK;
}

/*!
 * @brief       USB host MSC write
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
USBH_STA_T USBH_MSC_DevWrite(USBH_INFO_T* usbInfo, uint8_t lun, uint32_t address, \
                             uint8_t* buffer, uint16_t cnt)
{
    USBH_MSC_INFO_T* mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    uint32_t timeout;

    if ((usbInfo->devInfo.connectedStatus == DISABLE) || \
        (usbInfo->hostState != USBH_HOST_CLASS) || \
        (mscInfo->unit[lun].state != USBH_MSC_IDLE))
        {
            return USBH_FAIL;
        }

    mscInfo->state = USBH_MSC_RW_WRITE;
    mscInfo->unit[lun].state = USBH_MSC_RW_WRITE;
    mscInfo->opLun = lun;

    (void)USBH_MSC_SCSI_Write(usbInfo, lun, address, buffer, cnt);

    timeout = usbInfo->timer;

    while (USBH_MSC_Handler[mscInfo->unit[lun].state](usbInfo, lun) == USBH_BUSY)
    {
        if (((usbInfo->timer - timeout) > (0x2FFF * cnt)) || \
            (usbInfo->devInfo.connectedStatus == DISABLE))
            {
                mscInfo->state = USBH_MSC_IDLE;
                return USBH_FAIL;
            }
    }

    mscInfo->state = USBH_MSC_IDLE;

    return USBH_OK;
}

/*********************** USB host MSC class handler ***************************/

/*!
 * @brief       USB host MSC logic unit init handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_InitHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    UNUSED(lun);

    USBH_USR_DBG("USB host MSC unit %d init", mscInfo->curLun);

    mscInfo->timer = usbInfo->timer;

    /* Set MSC unit state to INQUIRY */
    mscInfo->unit[mscInfo->curLun].state = USBH_MSC_INQUIRY;

    return USBH_OK;
}

/*!
 * @brief       USB host MSC logic unit read handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ReadHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_MSC_SCSI_Read(usbInfo, lun, 0, NULL, 0);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            usbStatus = USBH_OK;
            break;

        case USBH_FAIL:
            /* Set MSC unit state to REQUEST SENSE */
            mscInfo->unit[lun].state = USBH_MSC_RW_REQIEST_SENSE;
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_UNRECOVERED_STATE;
            usbStatus = USBH_FAIL;
            break;

        default:
            break;
    
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC logic unit write handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_WriteHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_MSC_SCSI_Write(usbInfo, lun, 0, NULL, 0);

    switch (reqStatus)
    {
        case USBH_OK:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            usbStatus = USBH_OK;
            break;

        case USBH_FAIL:
            /* Set MSC unit state to REQUEST SENSE */
            mscInfo->unit[lun].state = USBH_MSC_RW_REQIEST_SENSE;
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_UNRECOVERED_STATE;
            usbStatus = USBH_FAIL;
            break;

        default:
            break;
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC logic unit read and write request sense handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_RWRequestSenseHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    uint8_t reqStatus;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    reqStatus = USBH_MSC_SCSI_RequestSense(usbInfo, lun, &mscInfo->unit[lun].sense);

    switch (reqStatus)
    {
        case USBH_OK:
            USBH_USR_DBG("Sense key: 0x%02X, ASC: 0x%02X, ASCQ: 0x%02X", \
                        mscInfo->unit[lun].sense.key, \
                        mscInfo->unit[lun].sense.asc, \
                        mscInfo->unit[lun].sense.ascq);

            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_ERR;
            usbStatus = USBH_FAIL;
            break;

        case USBH_FAIL:
            USBH_USR_DBG("USB MSC device is not ready");
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_UNRECOVERED_STATE;
            usbStatus = USBH_FAIL;
            break;

        default:
            break;
    
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC logic unit idle handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_IdleHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    UNUSED(usbInfo);
    UNUSED(lun);

    return USBH_OK;
}

/*!
 * @brief       USB host MSC logic unit INQUIRY handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_InquiryHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    USBH_STA_T reqStatus;

    reqStatus = USBH_MSC_SCSI_Inquiry(usbInfo, lun, &mscInfo->unit[lun].inquiry);

    switch (reqStatus)
    {
        case USBH_OK:
            USBH_USR_DBG("Inquiry revision: %s", mscInfo->unit[lun].inquiry.revID);
            USBH_USR_DBG("Inquiry vendor: %s", mscInfo->unit[lun].inquiry.vendorID);
            USBH_USR_DBG("Inquiry product: %s", mscInfo->unit[lun].inquiry.productID);

            /* Set MSC unit state to TEST UNIT READY */
            mscInfo->unit[lun].state = USBH_MSC_TEST_UNIT_READY;
            break;

        case USBH_FAIL:
            /* Set MSC unit state to REQUEST SENSE */
            mscInfo->unit[lun].state = USBH_MSC_REQUEST_SENSE;
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_ERR;
            break;

        default:
            break;
    }

    return reqStatus;
}

/*!
 * @brief       USB host MSC logic unit test unit ready handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_TestUnitReadyHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    USBH_STA_T reqStatus;

    reqStatus = USBH_MSC_SCSI_TestUnitReady(usbInfo, lun);

    switch (reqStatus)
    {
        case USBH_OK:
            if (mscInfo->unit[lun].preReadyState == USBH_OK)
            {
                mscInfo->unit[lun].changeState = DISABLE;
            }
            else
            {
                mscInfo->unit[lun].changeState = ENABLE;
                USBH_USR_DBG("USB host MSC unit %d ready", lun);
            }

            /* Set MSC unit state to READ CAPACITY */
            mscInfo->unit[lun].state = USBH_MSC_READ_CAPACITY;
            mscInfo->unit[lun].preReadyState = USBH_OK;
            mscInfo->unit[lun].errState = USBH_MSC_OK;
            break;

        case USBH_FAIL:
            if (mscInfo->unit[lun].preReadyState == USBH_FAIL)
            {
                mscInfo->unit[lun].changeState = DISABLE;
            }
            else
            {
                mscInfo->unit[lun].changeState = ENABLE;
                USBH_USR_DBG("USB host MSC unit %d not ready", lun);
            }

            /* Set MSC unit state to REQUEST SENSE */
            mscInfo->unit[lun].state = USBH_MSC_REQUEST_SENSE;
            mscInfo->unit[lun].preReadyState = USBH_FAIL;
            mscInfo->unit[lun].errState = USBH_MSC_BUSY;
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_ERR;
            break;

        default:
            break;
    }

    return reqStatus;
}

/*!
 * @brief       USB host MSC logic unit read capacity handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ReadCapacityHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    USBH_STA_T reqStatus;
    uint32_t blockNum;
    uint32_t blockSize;

    reqStatus = USBH_MSC_SCSI_ReadCapacity(usbInfo, lun, &mscInfo->unit[lun].capacity);

    switch (reqStatus)
    {
        case USBH_OK:
            if (mscInfo->unit[lun].changeState == ENABLE)
            {
                blockNum = mscInfo->unit[lun].capacity.blockNum;
                blockSize = mscInfo->unit[lun].capacity.blockSize;

                USBH_USR_LOG("USB host MSC unit %d capacity: %lu blocks, %lu bytes", \
                            lun, \
                            blockNum, \
                            (uint32_t)(blockNum * blockSize));
            }

            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_OK;
            mscInfo->curLun++;
            break;

        case USBH_FAIL:
            /* Set MSC unit state to REQUEST SENSE */
            mscInfo->unit[lun].state = USBH_MSC_REQUEST_SENSE;
            break;

        case USBH_ERR_UNRECOVERED:
            /* Set MSC unit state to idle */
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_ERR;
            break;

        default:
            break;
    }

    return reqStatus;
}

/*!
 * @brief       USB host MSC logic unit request sense handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_RequestSenseHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    USBH_STA_T reqStatus;
    uint8_t senseKey;

    reqStatus = USBH_MSC_SCSI_RequestSense(usbInfo, lun, &mscInfo->unit[lun].sense);

    switch (reqStatus)
    {
        case USBH_OK:
            senseKey = mscInfo->unit[lun].sense.key;

            if ((senseKey == USBH_SCSI_SENSE_KEY_UNIT_ATTENTION) || \
                (senseKey == USBH_SCSI_SENSE_KEY_NOT_READY))
            {
                /* Retry to test unit ready 10s later */
                if ((usbInfo->timer - mscInfo->timer) < 10000)
                {
                    mscInfo->unit[lun].state = USBH_MSC_TEST_UNIT_READY;
                    break;
                }

                USBH_USR_DBG("Sense key: 0x%02X, ASC: 0x%02X, ASCQ: 0x%02X", \
                            mscInfo->unit[lun].sense.key, \
                            mscInfo->unit[lun].sense.asc, \
                            mscInfo->unit[lun].sense.ascq);

                /* Set MSC unit state to idle */
                mscInfo->unit[lun].state = USBH_MSC_IDLE;
                mscInfo->curLun++;
            }
            else
            {
                USBH_USR_DBG("USB MSC device sense key error");
            }
            break;

        case USBH_FAIL:
            USBH_USR_DBG("USB MSC device is not ready");
            mscInfo->unit[lun].state = USBH_MSC_UNRECOVERED_STATE;
            break;

        case USBH_ERR_UNRECOVERED:
            mscInfo->unit[lun].state = USBH_MSC_IDLE;
            mscInfo->unit[lun].errState = USBH_MSC_ERR;
            break;

        default:
            break;
    }

    return reqStatus;
}

/*!
 * @brief       USB host MSC logic unit unrecovered error handler
 *
 * @param       usbInfo: usb host information
 *
 * @param       lun: logical unit number
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ErrorUnrecoveredHandler(USBH_INFO_T* usbInfo, uint8_t lun)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    UNUSED(lun);

    USBH_USR_DBG("USB MSC device unrecovered error");

    mscInfo->curLun++;

    return USBH_ERR_UNRECOVERED;
}

/*!
 * @brief       USB host MSC configuration handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ClassInitHandler(USBH_INFO_T* usbInfo)
{
    uint8_t itfNum = 0;
    uint8_t epNumMax;
    uint8_t subClass;
    uint8_t itfClass;
    uint8_t itfProtocol;
    uint32_t i;
    USBH_MSC_INFO_T *mscInfo;

    /* Find MSC class interface */
    while (itfNum < INTERFACE_DESC_MAX_NUM)
    {
        itfClass = USBH_ReadInterfaceClass(usbInfo, itfNum);
        subClass = USBH_ReadInterfaceSubClass(usbInfo, itfNum);
        itfProtocol = USBH_ReadInterfaceProtocol(usbInfo, itfNum);

        if ((itfClass == USBH_CLASS_MSC) && \
            (subClass == USBH_MSC_SCSI_CLASS_CODE) && \
            (itfProtocol == USBH_MSC_PROTOCOL_BBB))
            {
                break;
            }

        itfNum++;
    }

    if (itfNum >= INTERFACE_DESC_MAX_NUM)
    {
        USBH_USR_DBG("USB host MSC interface is not found");
        return USBH_FAIL;
    }

    if (itfNum < USBH_ReadConfigurationItfNum(usbInfo))
    {
        usbInfo->devInfo.curItf = itfNum;
        USBH_USR_DBG("USB host switch to interface %d", itfNum);
        USBH_USR_DBG("USB Class: 0x%02X, SubClass: 0x%02X, Protocol: 0x%02X", \
                    USBH_ReadInterfaceClass(usbInfo, itfNum), \
                    USBH_ReadInterfaceSubClass(usbInfo, itfNum), \
                    USBH_ReadInterfaceProtocol(usbInfo, itfNum));
    }
    else
    {
        USBH_USR_DBG("USB host MSC class not found");
        return USBH_FAIL;
    }

    /* Link class data */
    usbInfo->activeClass->classData = (USBH_MSC_INFO_T*)malloc(sizeof(USBH_MSC_INFO_T));
    mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo == NULL)
    {
        USBH_USR_DBG("USB host MSC class data malloc failed");
        return USBH_FAIL;
    }
    else
    {
        (void)memset(mscInfo, 0, sizeof(USBH_MSC_INFO_T));
    }

    /* Find MSC class endpoint */
    epNumMax = USBH_ReadInterfaceEpNum(usbInfo, itfNum);

    if (epNumMax > ENDPOINT_DESC_MAX_NUM)
    {
        epNumMax = ENDPOINT_DESC_MAX_NUM;
    }

    for (i = 0; i < epNumMax; i++)
    {
        if (USBH_ReadEndpointAddress(usbInfo, itfNum, i) & 0x80)
        {
            mscInfo->bulkInEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            mscInfo->inChNum = USBH_CH_AllocChannel(usbInfo, mscInfo->bulkInEpAddr);
            mscInfo->bulkInEpSize = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open channel for IN endpoint */
            USBH_OpenChannelCallback(usbInfo, mscInfo->inChNum, \
                                     mscInfo->bulkInEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_BULK, \
                                     mscInfo->bulkInEpSize);

            USBH_ConfigToggleCallback(usbInfo, mscInfo->inChNum, 0);
        }
        else
        {
            mscInfo->bulkOutEpAddr = USBH_ReadEndpointAddress(usbInfo, itfNum, i);
            mscInfo->outChNum = USBH_CH_AllocChannel(usbInfo, mscInfo->bulkOutEpAddr);
            mscInfo->bulkOutEpSize = USBH_ReadEndpointMPS(usbInfo, itfNum, i);

            /* Open channel for OUT endpoint */
            USBH_OpenChannelCallback(usbInfo, mscInfo->outChNum, \
                                     mscInfo->bulkOutEpAddr, \
                                     usbInfo->devInfo.address, \
                                     usbInfo->devInfo.speed, \
                                     EP_TYPE_BULK, \
                                     mscInfo->bulkOutEpSize);

            USBH_ConfigToggleCallback(usbInfo, mscInfo->outChNum, 0);
        }
    }

    /* Set MSC state */
    mscInfo->state = USBH_MSC_INIT;
    mscInfo->errState = USBH_MSC_OK;
    mscInfo->classReqState = USBH_MSC_REQ_IDLE;

    /* Initialize BOT */
    (void)USBH_MSC_BOT_Init(usbInfo);

    return USBH_OK;
}

/*!
 * @brief       USB host MSC class reset handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ClassDeInitHandler(USBH_INFO_T* usbInfo)
{
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->inChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, mscInfo->inChNum);
        USBH_CH_FreeChannel(usbInfo, mscInfo->inChNum);
        mscInfo->inChNum = 0;
    }

    if (mscInfo->outChNum != 0)
    {
        USBH_CloseChannelCallback(usbInfo, mscInfo->outChNum);
        USBH_CH_FreeChannel(usbInfo, mscInfo->outChNum);
        mscInfo->outChNum = 0;
    }

    if (usbInfo->activeClass->classData != NULL)
    {
        free(usbInfo->activeClass->classData);
        usbInfo->activeClass->classData = 0;
    }

    return USBH_OK;
}

/*!
 * @brief       USB host MSC class reguest handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_ClassReqHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_STA_T reqStatus = USBH_BUSY;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;
    uint8_t i;

    switch (mscInfo->classReqState)
    {
        case USBH_MSC_REQ_IDLE:
        case USBH_MSC_REQ_GET_MAX_LUN:
            reqStatus = USBH_MSC_BOT_REQ_GetMaxLun(usbInfo, &mscInfo->maxLun);
            switch (reqStatus)
            {
                case USBH_OK:
                    if (mscInfo->maxLun > USBH_SUPPORTED_LUN_MAX)
                    {
                        mscInfo->maxLun = USBH_SUPPORTED_LUN_MAX;
                    }
                    else
                    {
                        mscInfo->maxLun++;
                    }

                    /* Initialize unit state */
                    for (i = 0; i < mscInfo->maxLun; i++)
                    {
                        mscInfo->unit[i].changeState = DISABLE;
                        mscInfo->unit[i].preReadyState = USBH_FAIL;
                    }
                    
                    usbStatus = USBH_OK;
                    break;

                case USBH_ERR_NOT_SUP:
                    mscInfo->maxLun = 0;
                    usbStatus = USBH_OK;
                    break;

                default:
                    break;
            }
            break;

        case USBH_MSC_REQ_CTRL_ERROR:
            reqStatus = USBH_ClearFeature(usbInfo, 0);
            if (reqStatus == USBH_OK)
            {
                mscInfo->classReqState = mscInfo->preClassReqState;
            }
            break;

        default:
            break;
    }

    return usbStatus;
}

/*!
 * @brief       USB host MSC SOF handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_SOFHandler(USBH_INFO_T* usbInfo)
{
    UNUSED(usbInfo);

    return USBH_OK;
}

/*!
 * @brief       USB host MSC handler
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
static USBH_STA_T USBH_MSC_CoreHandler(USBH_INFO_T* usbInfo)
{
    USBH_STA_T usbStatus = USBH_BUSY;
    USBH_MSC_INFO_T *mscInfo = (USBH_MSC_INFO_T*)usbInfo->activeClass->classData;

    if (mscInfo->state == USBH_MSC_INIT)
    {
        if (mscInfo->curLun >= mscInfo->maxLun)
        {
            mscInfo->curLun = 0;
            mscInfo->state = USBH_MSC_IDLE;

            /* Notify user class launched */
            if (usbInfo->userCallback != NULL)
            {
                usbInfo->userCallback(usbInfo, USBH_USER_CLASS_LAUNCHED);
            }
        }
        else
        {
            mscInfo->unit[mscInfo->curLun].errState = USBH_MSC_BUSY;

            /* MSC unit state handler */
            USBH_MSC_Handler[mscInfo->unit[mscInfo->curLun].state](usbInfo, mscInfo->curLun);
        }
    }
    else if (mscInfo->state == USBH_MSC_IDLE)
    {
        usbStatus = USBH_OK;
    }
    else
    {
        /* Nothing to do */
    }

    return usbStatus;
}

/**@} end of group USBH_MSC_Functions */
/**@} end of group USBH_MSC_Class */
/**@} end of group APM32_USB_Library */
