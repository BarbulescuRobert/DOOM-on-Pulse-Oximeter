/*!
 * @file        flash_read_write.h
 *
 * @brief       Header for flash_read_write.c module
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
#include "apm32f0xx.h"
#include "apm32f0xx_fmc.h"

/* The following is the Flash region planning for this example. */

/*
 +-----------------------------------------------------------------------+
 |                                 Flash                                 |
 +-----------------------------------------------------------------------+
 |                Code               |          User Para area           |
 +--------+--------+--------+--------+--------+--------+--------+--------+
 |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |
 | page 0 | page 1 | page 2 | page N |page N+1|page N+2|page N+3|page N+N|
 |        |        |        |        |        |        |        |        |
 +--------+--------+--------+--------+--------+--------+--------+--------+
*/

/** @addtogroup Examples
  @{
  */

/** @addtogroup FMC_Read_Write
  @{
  */ 

/** @defgroup FMC_Read_Write_Macros Macros
  @{
*/

#if defined (APM32F072xB)
/* Only for APM32F072xB device */
#define APM32_FLASH_START_ADDR                    ((uint32_t)(0x08000000))
#define APM32_FLASH_SIZE                          (128 * 1024)
#define APM32_FLASH_END_ADDR                      ((uint32_t)(APM32_FLASH_START_ADDR + APM32_FLASH_SIZE))
#define APM32_FLASH_PAGE_SIZE                     ((uint32_t)(2 * 1024))
#elif defined (APM32F030x8)
/* Only for APM32F030x8 device */
#define APM32_FLASH_START_ADDR                    ((uint32_t)(0x08000000))
#define APM32_FLASH_SIZE                          (64 * 1024)
#define APM32_FLASH_END_ADDR                      ((uint32_t)(APM32_FLASH_START_ADDR + APM32_FLASH_SIZE))
#define APM32_FLASH_PAGE_SIZE                     ((uint32_t)(1 * 1024))
#elif defined (APM32F030xC)
/* Only for APM32F030xC device */
#define APM32_FLASH_START_ADDR                    ((uint32_t)(0x08000000))
#define APM32_FLASH_SIZE                          (64 * 1024)
#define APM32_FLASH_END_ADDR                      ((uint32_t)(APM32_FLASH_START_ADDR + APM32_FLASH_SIZE))
#define APM32_FLASH_PAGE_SIZE                     ((uint32_t)(2 * 1024))
#endif

/* flash read write total size. This value must be a multiple of the page size. */
#define FLASH_READ_WRITE_TOTAL_SIZE               ((uint32_t)(APM32_FLASH_PAGE_SIZE * 8))

/* flash read write page start address, it's must be page aligned */
#define FLASH_READ_WRITE_START_ADDR               (APM32_FLASH_END_ADDR - FLASH_READ_WRITE_TOTAL_SIZE)

/* flash read write page start address */
#define FLASH_READ_WRITE_END_ADDR                 APM32_FLASH_END_ADDR


/**@} end of group FMC_Read_Write_Macros*/

/** @defgroup FMC_Read_Write_Functions Functions
  @{
*/

/* Read the specified length of data from the specified address */
int Flash_Read(uint32_t readAddr, uint8_t *pData, uint32_t len);
/* Write the specified length of data from the specified address */
int Flash_Write(uint32_t writeAddr, uint8_t *pData, uint32_t len);

/**@} end of group FMC_Read_Write_Functions */
/**@} end of group FMC_Read_Write */
/**@} end of group Examples */
