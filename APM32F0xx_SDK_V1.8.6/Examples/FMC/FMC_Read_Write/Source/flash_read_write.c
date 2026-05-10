/*!
 * @file        flash_read_write.c
 *
 * @brief       This file provides a flash read/write interface
 *
 * @version     V1.0.0
 *
 * @date        2023-08-31
 *
 * @attention
 *
 *  Copyright (C) 2022-2023 Geehy Semiconductor
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
#include "flash_read_write.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup FMC_Read_Write
  @{
  */

/** @defgroup FMC_Read_Write_Variables Variables
  @{
*/

/* The buffer that write or erase page data */
static uint8_t Flash_Buffer[APM32_FLASH_PAGE_SIZE];

/**@} end of group FMC_Read_Write_Variables */


/** @defgroup FMC_Read_Write_Functions Functions
  @{
*/

/*!
 * @brief     flash read byte.
 *
 * @param     readAddr:  flash address.
 *
 * @retval    the data of assign address.
 */
static uint8_t Flash_ReadByte(uint32_t readAddr)
{
    return (*(__IO uint8_t *)readAddr); 
}

/*!
 * @brief     Read the specified length of data from the specified address.
 *
 * @param     readAddr: read address.
 *
 * @param     pData: save the read data.
 *
 * @param     len: read data length.
 *
 * @retval    Return Success or error status. It can be one of value:
 *            @arg -1 : Read data error.
 *            @arg 0  : Read data successful.
 *
 */
int Flash_Read(uint32_t readAddr, uint8_t *pData, uint32_t len)
{
    /* illegal address direct return */
    if ((readAddr < FLASH_READ_WRITE_START_ADDR) || ((readAddr + len) > FLASH_READ_WRITE_END_ADDR))
        return -1;
    
    /* illegal pointer direct return */
    if (pData == NULL)
        return -1;
    
    /* read data */
    for (int i = 0; i < len; i++)
    {
        pData[i] = Flash_ReadByte(readAddr);
        readAddr += 1;
    }
    
    return 0;
}

/*!
 * @brief     In a page, write the specified length of data from the specified address.
 *
 * @param     writeAddr: write address.
 *
 * @param     pData: save the write data.
 *
 * @param     len: write data length.
 *
 * @retval    Return Success or error status. It can be one of value:
 *            @arg -1         : Write data error.
 *            @arg flashStatus: FMC program status. The value refer to FMC_STATE_T.
 *
 * @note      Address and length must be 4-bytes aligned.
 *
 */
static int Flash_WriteOnePage(uint32_t writeAddr, const uint8_t *pData, uint32_t len)
{
    uint32_t isErase = 0;
    uint32_t startAddr;
    uint32_t offsetAddr;
    uint32_t i = 0;
    uint8_t *pTemp = Flash_Buffer;
    FMC_STATE_T flashStatus = FMC_STATE_COMPLETE;

    startAddr = writeAddr / APM32_FLASH_PAGE_SIZE * APM32_FLASH_PAGE_SIZE;
    offsetAddr = writeAddr % APM32_FLASH_PAGE_SIZE;

    /* illegal address direct return */
    if ((writeAddr < FLASH_READ_WRITE_START_ADDR) || ((writeAddr + len) > FLASH_READ_WRITE_END_ADDR))
        return -1;
    
    /* illegal pointer direct return */
    if (pData == NULL)
        return -1;
    
    /* unlock flash for erase or write*/
    FMC_Unlock();
    
    /* check whether the page need to be erased */
    for (i = 0; i < len; i++)
    {
        if (Flash_ReadByte(writeAddr + i) != 0xFF)
        {
            isErase = 1;
            break;
        }
    }
    
    /* the page needs to be erase */
    if (isErase == 1)
    {
        /* read the entire page data to the buffer before write or erase */
        Flash_Read(startAddr, Flash_Buffer, APM32_FLASH_PAGE_SIZE);
        
        /* copy the data to the buffer */
        for (i = 0; i < len; i++)
        {
            Flash_Buffer[offsetAddr + i] = pData[i];
        }
        
        /* erase the page where the address is located */
        if ((flashStatus = FMC_ErasePage(startAddr)) != FMC_STATE_COMPLETE)
        {
            return flashStatus;
        }
        
        /* write the entire page data */
        for (i = 0; i < (APM32_FLASH_PAGE_SIZE / 4); i++)
        {
            if ((flashStatus = FMC_ProgramWord(startAddr, *(uint32_t *)pTemp)) != FMC_STATE_COMPLETE)
            {
                return flashStatus;
            }
            startAddr += 4;
            pTemp += 4;
        }
    }
    /* the page don't need to be erase */
    else
    {
        /* write n bytes of data to the page */
        for (i = 0; i < len; i += 4)
        {
            if ((flashStatus = FMC_ProgramWord(writeAddr, *(uint32_t *)pData)) != FMC_STATE_COMPLETE)
            {
                return flashStatus;
            }
            writeAddr += 4;
            pData += 4;
        }
    }
    
    /* lock flash */
    FMC_Lock();
    
    return FMC_STATE_COMPLETE;
}

/*!
 * @brief     Write the specified length of data from the specified address.
 *            Can be written across page.
 *
 * @param     writeAddr: write address.
 *
 * @param     pData: save the write data.
 *
 * @param     len: write data length.
 *
 * @retval    Return Success or error status. It can be one of value:
 *            @arg -1         : Write data error.
 *            @arg flashStatus: FMC program status. The value refer to FMC_STATE_T.
 *
 * @note      Address and length must be 4-bytes aligned.
 *
 */
int Flash_Write(uint32_t writeAddr, uint8_t *pData, uint32_t len)
{
    uint32_t numOfPage = 0, numOfByte = 0, offsetAddr = 0;
    uint32_t count = 0, temp = 0;
    int writeStatus = 0;
    
    /* address and len is not 4-bytes aligned */
    if ((writeAddr % 4 != 0) || (len % 4 != 0))
        return -1;
    
    /* offerset address in the page */
    offsetAddr = writeAddr % APM32_FLASH_PAGE_SIZE;
    
    /* The size of the remaining space inthe page from writeAddr */
    count = APM32_FLASH_PAGE_SIZE - offsetAddr;
    
    /* Calculate how many pages to write */
    numOfPage = len / APM32_FLASH_PAGE_SIZE;
    
    /* Calculate how many bytes are left less than one page */
    numOfByte = len % APM32_FLASH_PAGE_SIZE;
    
    /* offsetAddr = 0, writeAddr is page aligned */
    if (offsetAddr == 0)
    {
        /* len < APM32_FLASH_PAGE_SIZE */
        if (numOfPage == 0) 
        {
            if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, len)) != FMC_STATE_COMPLETE)
            {
                return writeStatus;
            }
        }
        /* len > APM32_FLASH_PAGE_SIZE */
        else 
        {
            /* write numOfPage page */
            while (numOfPage--)
            {
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, APM32_FLASH_PAGE_SIZE)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
                writeAddr +=  APM32_FLASH_PAGE_SIZE;
                pData += APM32_FLASH_PAGE_SIZE;
            }
            
            /* write remaining data */
            if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, numOfByte)) != FMC_STATE_COMPLETE)
            {
                return writeStatus;
            }
        }
    }
    /* offsetAddr != 0, writeAddr is not page aligned */
    else 
    {
        /* len < APM32_FLASH_PAGE_SIZE, the data length is less than one page */
        if (numOfPage == 0)
        {
            /* numOfByte > count,  need to write across the page */
            if (numOfByte > count) 
            {
                temp = numOfByte - count;
                /* fill the current page */
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, count)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
                
                writeAddr +=  count;
                pData += count;
                /* write remaining data */
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, temp)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
            }
            else 
            {
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, len)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
            }
        }
        /* len > APM32_FLASH_PAGE_SIZE */
        else 
        {
            len -= count;
            numOfPage = len / APM32_FLASH_PAGE_SIZE;
            numOfByte = len % APM32_FLASH_PAGE_SIZE;
                
            /* write count data */
            if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, count)) != FMC_STATE_COMPLETE)
            {
                return writeStatus;
            }
            
            writeAddr +=  count;
            pData += count;
            
            /* write numOfPage page */
            while (numOfPage--)
            {
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, APM32_FLASH_PAGE_SIZE)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
                writeAddr +=  APM32_FLASH_PAGE_SIZE;
                pData += APM32_FLASH_PAGE_SIZE;
            }
            
            if (numOfByte != 0)
            {
                if ((writeStatus = Flash_WriteOnePage(writeAddr, pData, numOfByte)) != FMC_STATE_COMPLETE)
                {
                    return writeStatus;
                }
            }
        }
    }
    
    return FMC_STATE_COMPLETE;
}

/**@} end of group FMC_Read_Write_Functions */
/**@} end of group FMC_Read_Write */
/**@} end of group Examples */
