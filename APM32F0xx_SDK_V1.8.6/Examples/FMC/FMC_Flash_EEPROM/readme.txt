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

The example describes a software method, using the onchip Flash memory of the
APM32F072xB devices to emulate eeprom. After running this example, if the reading
and writing successful, LED2 is on, otherwise it is off. You also can view the
print data through the USART1.

notes:
/* The following is the Flash region planning for this example. */
Recommend that you use the last few pages.

/*
 +-----------------------------------------------------------------------+
 |                                 Flash                                 |
 +-----------------------------------------------------------------------+
 |                Code               |            Flash EEPROM           |
 +--------+--------+--------+--------+--------+--------+--------+--------+
 |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |  2KB   |
 | page 0 | page 1 | page 2 | page N |page N+1|page N+2|page N+3|page N+N|
 |        |        |        |        |        |        |        |        |
 +--------+--------+--------+--------+--------+--------+--------+--------+
*/

&par Hardware Description

using USART1(TX:PA9, RX:PA10).
  - USART1 configured as follow:
  - BaudRate    =  115200
  - Word Length =  USART_WordLength_8b
  - Stop Bit    =  USART_StopBits_1
  - Parity      =  USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

  - FMC/FMC_Write/Source/apm32f0xx_int.c     Interrupt handlers
  - FMC/FMC_Write/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
