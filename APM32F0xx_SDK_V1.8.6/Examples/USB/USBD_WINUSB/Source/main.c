/*!
 * @file        main.c
 *
 * @brief       Main program body
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
#include "board.h"
#include "bsp_delay.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_usart.h"
#include "usb_device_user.h"
#include "usbd_winusb.h"
#include "usbd_winusb_vcp.h"
#include <stdio.h>
#include <string.h>

/** @addtogroup Examples
  * @brief USBD WINUSB examples
  @{
  */

/** @addtogroup USBD_WINUSB
  @{
  */

/** @defgroup USBD_WINUSB_Macros Macros
  @{
*/

#define DEBUG_USART    USART1

/**@} end of group USBD_WINUSB_Macros*/

/** @defgroup USBD_WINUSB_Enumerates Enumerates
  @{
  */

/**
 * @brief User application state
 */
typedef enum
{
    USER_APP_INIT,
    USER_APP_RUN,
} USER_APP_STATE_T;

/**@} end of group USBD_WINUSB_Enumerates*/

/** @defgroup USBD_WINUSB_Functions Functions
  @{
  */

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      int
 */
int main(void)
{
    /* COM1 init*/
    APM_MINI_COMInit(COM1);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_GPIO);
    APM_MINI_PBInit(BUTTON_KEY2, BUTTON_MODE_GPIO);

    /* Init USB device */
    USB_DeviceInit();

    printf("USB Device WINUSB Application\r\n");

    while (1)
    {
        USB_DevUserApplication();
    }
}

/*!
 * @brief       User application
 *
 * @param       None
 *
 * @retval      None
 */
void USB_DevUserApplication(void)
{
    static uint8_t userAppState = USER_APP_INIT;
    static uint8_t interval = 0;
    static uint16_t i = 0;
    static char str[] = "Hello      \r\n";
    
    switch (userAppState)
    {
        case USER_APP_INIT:
            interval = USBD_WINUSB_ReadInterval(&gUsbDeviceFS);
            userAppState = USER_APP_RUN;
            break;

        case USER_APP_RUN:
            /* Loopback */
            if(gUsbVCP.state == USBD_WINUSB_VCP_REV_UPDATE)
            {
                gUsbVCP.state = USBD_WINUSB_VCP_IDLE;
                
                USBD_FS_WINUSB_ItfSend((uint8_t *)winusbRxBuffer, (uint16_t)gUsbVCP.rxUpdateLen);
                
                USBD_FS_WINUSB_ItfSend((uint8_t *)"\r\n", strlen("\r\n"));
            }
            else
            {
                i++;
                sprintf(str,"Hello %d\r\n",i);
                USBD_FS_WINUSB_ItfSend((uint8_t *)str, strlen(str));
            }
            
            APM_DelayMs(interval);
            break;
    }
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group USBD_WINUSB_Functions */
/**@} end of group USBD_WINUSB */
/**@} end of group Examples */
