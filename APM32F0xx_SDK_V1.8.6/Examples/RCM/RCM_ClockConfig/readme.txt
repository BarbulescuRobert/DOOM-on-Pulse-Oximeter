/*!
 * @file        readme.txt
 *
 * @brief       This file is routine instruction
 *
 * @version     V1.0.0
 *
 * @date        2023-08-31
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

This example shows how to switch from HSE to HSI when the HSE fails.
After initializal, if the HSE is working properly, LED2 is on.
else the HSE fails, system will switch to HSI, and the LED3 on.
The HSE fails when the system is running, it will trig the CSS,
and enter the NMI interrupt handler, the LED2 and LED3 will on together.

&par Hardware Description

&par Directory contents

  - RCM/RCM_ClockConfig/Source/apm32f0xx_int.c     Interrupt handlers
  - RCM/RCM_ClockConfig/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
