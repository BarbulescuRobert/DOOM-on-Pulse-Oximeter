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


&par Example Description

The example aim to show how to configure a bootloader firmware to IAP.
When device connet to HyperTerminal right, a usart menu will show to user.
The menu as follow:

***************************************************************
*      apm32f0xx In-Application Programming Application       *
*                                              (Version 1.0.0)*
***************************************************************

** Please select an operation item
*  1.Download Flash application 1 -----------------------> 1  *
*  2.Upload Flash application 1   -----------------------> 2  *
*  3.Jump to user application 1   -----------------------> 3  *
*  4.Download Flash application 2 -----------------------> 4  *
*  5.Upload Flash application 2   -----------------------> 5  *
*  6.Jump to user application 2   -----------------------> 6  *
***************************************************************

note:

Before using IAP function, you must download the bootloader program to the target board.
Then compile the "Application1" or "Application2" project, and update the generated bin
firmware file to the target board with the function of IAP through the HyperTerminal tool.

&par Hardware Description

  - USART1(TX:PA9, RX:PA10)  configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - IAP/IAP_BootLoader/Source/apm32f0xx_int.c        Interrupt handlers
  - IAP/IAP_BootLoader/Source/main.c                 Main program
  - IAP/IAP_BootLoader/Source/bsp_usart.c            USART Configuration
  - IAP/IAP_BootLoader/Source/bsp_flash.c            Flash handlers
  - IAP/IAP_BootLoader/Source/common.c               Common functions
  - IAP/IAP_BootLoader/Source/menu.c                 UART Menu
  - IAP/IAP_BootLoader/Source/ymodem.c               Ymodem protocol handlers


&par Hardware and Software environment

  - This example runs on APM32F030 or APM32F072 MINI Devices.
