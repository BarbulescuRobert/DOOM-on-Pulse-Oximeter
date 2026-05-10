/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2022-09-20
 *
 * @attention
 *
 *  Copyright (C)  2022 Geehy Semiconductor
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

&par Example Description 

This example shows how to wake up from standby mode and stop mode.
 - CPU will be wakeup from standby mode by RTC ALARM event.
 - And CPU will be wakeup from stop mode by RTC ALARM event.

&par Hardware Description

using USART1 (TX:PA9, RX:PA10).
  - USART1 configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - RTC/RTC_LPWR_Wakeup/Source/apm32f0xx_int.c            Interrupt handlers
  - RTC/RTC_LPWR_Wakeup/Source/main.c                     Main program

&par Hardware and Software environment

  - This example runs on APM32F030 or APM32F072 MINI Devices.
