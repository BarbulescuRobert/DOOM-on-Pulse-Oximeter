/*!
 * @file        bsp_flash.c
 *
 * @brief       Include IAP flash operation
 *
 * @version     V1.0.0
 *
 * @date        2022-09-20
 *
 * @attention
 *
 *  Copyright (C) 2022 Geehy Semiconductor
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
#include "bsp_flash.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @addtogroup BSP_FLASH
  @{
  */

/** @defgroup BSP_FLASH_Macros Macros
  @{
  */

/**@} end of group BSP_FLASH_Macros */

/** @defgroup BSP_FLASH_Enumerations Enumerations
  @{
  */

/**@} end of group BSP_FLASH_Enumerations */

/** @defgroup BSP_FLASH_Structures Structures
  @{
  */

/**@} end of group BSP_FLASH_Structures */

/** @defgroup BSP_FLASH_Variables Variables
  @{
  */

volatile FMC_STATE_T FLASHStatus = FMC_STATE_COMPLETE;

/**@} end of group BSP_FLASH_Variables */

/** @defgroup BSP_FLASH_Functions Functions
  @{
  */

/*!
 * @brief       Unlocks Flash for write access
 *
 * @param       None
 *
 * @retval      None
 *
 */
void FLASH_IAP_Init(void)
{
    /* Unlock the flash memory */
    FMC_Unlock();

    /* Clear all FMC flags */
    FMC_ClearStatusFlag(FMC_FLAG_OC | FMC_FLAG_PE \
                        | FMC_FLAG_WPE);
}

/*!
 * @brief       Erase of all user flash area
 *
 * @param       Application : APP1 or APP2
 *
 * @retval      SUCCESS: user flash area successfully erased
 *              ERROR: error occurred
 */
uint32_t FLASH_IAP_Erase(APP_TypeDef Application)
{
    uint32_t numberOfPage = 0x0;
    uint32_t eraseCounter = 0x0;
    uint32_t flashDestination = 0x0;

    if (Application == APP1)
    {
        numberOfPage = (USER_APP1_END_ADDRESS - USER_APP1_START_ADDRESS) / FLASH_PAGE_SIZE;
        flashDestination = USER_APP1_START_ADDRESS;
    }
    else
    {
        numberOfPage = (USER_APP2_END_ADDRESS - USER_APP2_START_ADDRESS) / FLASH_PAGE_SIZE;
        flashDestination = USER_APP2_START_ADDRESS;
    }

    for (eraseCounter = 0; (eraseCounter < numberOfPage) && (FLASHStatus == FMC_STATE_COMPLETE); eraseCounter++)
    {
        FLASHStatus = FMC_ErasePage(flashDestination + (FLASH_PAGE_SIZE * eraseCounter));
    }

    return (SUCCESS);
}

/*!
 * @brief       Write the appoint data buffer in flash
 *
 * @param       address: start address for writing data buffer
 *
 * @param       data: pointer on data buffer
 *
 * @param       length: length of data buffer (unit is 32-bit word)
 *
 * @param       Application : APP1 or APP2
 *
 * @retval      SUCCESS: data successfully written to Flash memory
 *              ERROR: Error occurred while writing data in Flash memory
 *
 * @note        buffer data are 32-bit aligned
 */
uint32_t FLASH_IAP_Write(__IO uint32_t* address, uint32_t* data, uint16_t length, APP_TypeDef Application)
{
    uint32_t i = 0;
    uint32_t addressEnd;

    if (Application == APP1)
    {
        addressEnd = USER_APP1_END_ADDRESS - 4;
    }
    else
    {
        addressEnd = USER_APP2_END_ADDRESS - 4;
    }

    for (i = 0; i < length; i++)
    {
        if (*address > addressEnd)
        {
            return (ERROR);
        }

        /* The operation will be done by word and Check the written value*/
        if ((FMC_ProgramWord(*address, *(uint32_t*)(data + i)) != FMC_STATE_COMPLETE) \
                || (*(uint32_t*)*address != *(uint32_t*)(data + i)))
        {
            return (ERROR);
        }

        *address += 4;
    }

    return (SUCCESS);
}

/**@} end of group BSP_FLASH_Functions */
/**@} end of group BSP_FLASH */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
