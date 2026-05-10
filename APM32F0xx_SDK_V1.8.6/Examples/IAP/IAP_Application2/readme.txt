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

This example shows how to generate a APP firmware to IAP.

LED3 are toggled with a timing defined by the Delay function.

note:

Before using IAP function, you must download the bootloader program to the target board.
Then compile the "Application1" or "Application2" project, and update the generated bin 
firmware file to the target board with the function of IAP through the HyperTerminal tool.

&par Hardware Description

Delays exactly one second, and the LED3 on-off state changes.

&par Directory contents

  - IAP/IAP_Application2/Source/apm32f0xx_int.c     Interrupt handlers
  - IAP/IAP_Application2/Source/main.c              Main program


&par Hardware and Software environment

  - This example runs on APM32F030 or APM32F072 MINI Devices.