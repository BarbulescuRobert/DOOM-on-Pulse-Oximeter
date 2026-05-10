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

This example shows how to use SPI read and send data in polling mode
in two line full duplex mode. They will transfer data per 1 seconds.
Master and slave will output received data in USART1.

           Master      Slave
 CLK       PA5         PA5
 NSS       PA4         PA4
 MISO      PA6         PA6
 MOSI      PA7         PA7

&par Hardware Description

using USART1(TX:PA9, RX:PA10) to Master.
  - USART1 configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

using SPI1(MISO:PA6, MOSI:PA7, CLK:PA5, NSS: PA4) to Master.
  - Word Length    = 9 bit
  - Clock polarity = Low
  - Clock phase    = odd edge
  - baudrateDiv    = 64 (32MHz / 64 = 512Kbps)
  - firstBit       = MSB
  - Slave select   = Software slave select
  - direction      = 2 line full duplex

using SPI1(MISO:PA6, MOSI:PA7, CLK:PA5, NSS: PA4) to Slave.
  - Word Length    = 9 bit
  - Clock polarity = Low
  - Clock phase    = odd edge
  - baudrateDiv    = 64 (32MHz / 64 = 512Kbps)
  - firstBit       = MSB
  - Slave select   = Hardware slave select
  - direction      = 2 line full duplex

&par Directory contents

  - SPI/SPI_FullDuplexPolling/slave/Source/apm32f0xx_int.c      Slave  Interrupt handlers
  - SPI/SPI_FullDuplexPolling/slave/Source/main.c               Slave  Main program
  - SPI/SPI_FullDuplexPolling/master/Source/apm32f0xx_int.c     Master Interrupt handlers
  - SPI/SPI_FullDuplexPolling/master/Source/main.c              Master Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
