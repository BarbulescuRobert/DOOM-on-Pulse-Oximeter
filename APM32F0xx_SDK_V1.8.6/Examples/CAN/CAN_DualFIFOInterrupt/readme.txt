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

This example shows how to configurate CAN peripheral in normal mode, then
to send and receive CAN frames in interrupt mode and using FIFO0 and FIFO1.
The CAN send frames use two different standard identifier.
When two boards KEY1/KEY2 button is pressed, two boards send CAN frames to each other.
When two boards the CAN frames are received, the receive data will be printed.

You need to download this example to both boards.

&par Hardware Description

            Board1      Board2
  CAN_TX    PB9         PB9
  CAN_RX    PB8         PB8

     _____________                                     ________________
    |             |                                   |                |
    |             |-PB8(CAN1_RX)  <--------->  CAN_RX-| CAN Transceiver|-CANH  <--------
    |   Board1    |-PB9(CAN1_TX)  <--------->  CAN_TX-|                |-CANL  <--     |
    |             |                                   |________________|          |    |
    |-------------|                                    ________________           |    |
    |             |                                   |                |          |    |
    |   Board2    |-PB12(CAN2_RX) <--------->  CAN_RX-| CAN Transceiver|-CANL  <--     |
    |             |-PB13(CAN2_TX) <--------->  CAN_TX-|                |-CANH  <--------
    |_____________|                                   |________________|

using USART1(TX:PA9, RX:PA10) to Board1 and Board2.
  - USART1 configured as follow:
  - BaudRate    =  115200
  - Word Length =  USART_WordLength_8b
  - Stop Bit    =  USART_StopBits_1
  - Parity      =  USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - CAN/CAN_DualFIFOInterrupt/Source/apm32f0xx_int.c         Interrupt handlers
  - CAN/CAN_DualFIFOInterrupt/Source/main.c                  Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
