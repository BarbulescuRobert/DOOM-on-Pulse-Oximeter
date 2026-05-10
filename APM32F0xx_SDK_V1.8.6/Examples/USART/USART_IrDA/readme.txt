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

The program shows how to  using USART IrDA mode, in this case,
USART1 sends data to upper computer. You can check the data in a Serial Port Utility

  - USART1(TX:PA9, RX:PA10) configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Transmit enabled

USARTx configured as follow:
  - BaudRate    =   115200 baud
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

To use this example, the user needs to prepare two IrDA modules.
   VCC             ---> 1# IrDA module VCC
   GND             ---> 1# IrDA module GND
   PA2(USART2_TX)  ---> 1# IrDA module TX
   PA3(USART2_RX)  ---> 1# IrDA module TX

   VCC             ---> 2# IrDA module VCC
   GND             ---> 2# IrDA module GND
   PB10(USART3_TX) ---> 2# IrDA module TX
   PB11(USART3_RX) ---> 2# IrDA module TX


&par Directory contents

  - USART/USART_Interrupt/Source/apm32f0xx_int.c     Interrupt handlers
  - USART/USART_Interrupt/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
