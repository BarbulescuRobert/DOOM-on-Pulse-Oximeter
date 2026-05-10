/*!
 * @file        apm32f0xx_int.c
 *
 * @brief       Main Interrupt Service Routines
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
#include "apm32f0xx_int.h"
#include "main.h"
#include "apm32f0xx_usb_device.h"
#include "bsp_delay.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup USBD_CDC
  @{
  */

/** @defgroup USBD_CDC_INT_Variables INT_Variables
  @{
  */

extern USBD_HANDLE_T usbDeviceHandler;

/**@} end of group USBD_CDC_INT_Variables */

/** @defgroup USBD_CDC_INT_Functions INT_Functions
  @{
  */

/*!
 * @brief        This function handles NMI exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void NMI_Handler(void)
{
}

/*!
 * @brief        This function handles Hard Fault exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void HardFault_Handler(void)
{
}

/*!
 * @brief        This function handles SVCall exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SVC_Handler(void)
{
}

/*!
 * @brief        This function handles PendSV_Handler exception
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief        This function handles SysTick Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void SysTick_Handler(void)
{
    APM_DelayTickDec();
}

/*!
 * @brief        This function handles USBD Handler
 *
 * @param        None
 *
 * @retval       None
 *
 * @note
 */
void USBD_IRQHandler(void)
{
    USBD_IsrHandler(&usbDeviceHandler);
}

/**@} end of group USBD_CDC_INT_Functions */
/**@} end of group USBD_CDC */
/**@} end of group Examples */
