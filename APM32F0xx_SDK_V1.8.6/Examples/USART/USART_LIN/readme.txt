/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.3
 *
 * @date        2022-09-20
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
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

Through The computer of serial debugging assistant, display the message sent
and received between the MCU and USART3.

The phenomenon of serial assistant can display information from USART3.

  - USART3(TX:PB10, RX:PB11) configured as follow:
  - BaudRate    = 115200
  - Word Length = USART_WordLength_8b
  - Stop Bit    = USART_StopBits_1
  - Parity      = USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

USART2(PA2, PA3) and USART1(PA9, PA10) configured as follow:
  - BaudRate    =   115200 baud
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

    USART2       USART1
      PA2  ---->  PA9
      PA3  ---->  PA10

&par Directory contents

  - USART/USART_Interrupt/Source/apm32f0xx_int.c     Interrupt handlers
  - USART/USART_Interrupt/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
