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

This example describes how to configure the TMR1 peripheral to Encoder mode.

&par Hardware Description

using USART2(TX:PA2, RX:PA3).
  - USART2 configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

using Input Capture Channel TMR1_CH1(PA8),TMR1_CH2(PA9).

&par Directory contents

  - TMR/TMR_EncoderInterface/Source/apm32f0xx_int.c     Interrupt handlers
  - TMR/TMR_EncoderInterface/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F030 or APM32F072 MINI Devices.
