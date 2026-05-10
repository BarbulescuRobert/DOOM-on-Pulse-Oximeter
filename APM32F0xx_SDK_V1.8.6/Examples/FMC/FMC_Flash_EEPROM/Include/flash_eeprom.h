/*!
 * @file        flash_eeprom.h
 *
 * @brief       This file contains all the functions for the eeprom emulation.
 *
 * @version     V1.0.0
 *
 * @date        2023-02-28
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

#ifndef __FLASH_EEPROM_H
#define __FLASH_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes */
#include "apm32f0xx.h"
#include "apm32f0xx_fmc.h"

/* The following is the APM32F072 Flash region planning for this example. */

/*
 +-----------------------------------------------------------------------+
 |                                 Flash                                 |
 +-----------------------------------------------------------------------+
 |                Code               |            Flash EEPROM           |
 +--------+--------+--------+--------+--------+--------+--------+--------+
 |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |
 | page 0 | page 1 | page 2 | page N |page N+1|page N+2|page N+3|page N+N|
 |        |        |        |        |        |        |        |        |
 +--------+--------+--------+--------+--------+--------+--------+--------+
*/

/** @addtogroup Examples
  @{
*/

/** @addtogroup FMC_Flash_EEPROM
  @{
*/

/** @defgroup FMC_Flash_EEPROM_Macros Macros
  @{
*/

/* Flash EEPROM Page number, can use multiple MCU flash pages */
#define FLASH_EE_PAGE_NUM              ((uint32_t)1)

/* MCU Flash Page size */
#define APM32_FLASH_PAGE_SIZE          ((uint32_t)(2 * 1024))

/* MCU Flash size */
#define APM32_FLASH_SIZE               (128 * 1024)


/* The following macro definitions do not require user changes. */
/* Flash EEPROM Page size, can contain multiple MCU flash pages */
#define FLASH_EE_PAGE_SIZE             ((uint32_t)(FLASH_EE_PAGE_NUM * APM32_FLASH_PAGE_SIZE))

/* Flash EEPROM start address in Flash */
#define FLASH_EE_START_ADDRESS         ((uint32_t)(0x08000000 + APM32_FLASH_SIZE - FLASH_EE_PAGE_SIZE * 2))

/* Flash EEPROM Page0 and Page1 start address */
#define PAGE0_START_ADDRESS            ((uint32_t)(FLASH_EE_START_ADDRESS))
#define PAGE1_START_ADDRESS            ((uint32_t)(PAGE0_START_ADDRESS + FLASH_EE_PAGE_SIZE))

/* The maximum variables of number that can be stored */
#define NUMBER_OF_VARIABLES            ((uint32_t)(FLASH_EE_PAGE_SIZE / 8 - 1))

/* Flash EEPROM valid page definitions */
#define PAGE0_VALID                    ((uint32_t)0x00000000)  /*!< Page0 valid */
#define PAGE1_VALID                    ((uint32_t)0x00000001)  /*!< Page1 valid */
#define NO_VALID_PAGE                  ((uint32_t)0x00000002)  /*!< No valid page */

/* Flash EEPROM Page status definitions */
#define PAGE_STATUS_ERASED             ((uint32_t)0xFFFFFFFF)  /* PAGE is empty */
#define PAGE_STATUS_TRANSFER           ((uint32_t)0xEEEEEEEE)  /* PAGE is marked to transfer status */
#define PAGE_STATUS_VALID              ((uint32_t)0x00000000)  /* PAGE containing valid data */

/* Get valid pages in read and write mode defines */
#define GET_VALID_PAGE_READ            ((uint8_t)0x01)
#define GET_VALID_PAGE_WRITE           ((uint8_t)0x02)

/**@} end of group FMC_Flash_EEPROM_Macros */

/** @defgroup FMC_Flash_EEPROM_Functions Functions
  @{
*/

FMC_STATE_T Flash_EE_Init(void);
uint32_t Flash_EE_ReadData(uint32_t VirtAddress, uint32_t *pData);
FMC_STATE_T Flash_EE_WriteData(uint32_t VirtAddress, uint32_t Data);

/**@} end of group FMC_Flash_EEPROM_Functions */
/**@} end of group FMC_Flash_EEPROM */
/**@} end of group Examples */

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_EEPROM_H */
