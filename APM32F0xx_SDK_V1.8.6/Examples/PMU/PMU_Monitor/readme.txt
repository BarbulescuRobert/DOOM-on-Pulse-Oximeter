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

This example shows how to use the PVD to monitor the voltage.
After initialize, LED2 is on, when the voltage drop to about 2.20V,
system will trig the PVD interrupt, and turn off LED2 and turn
on LED3.

&par Hardware Description

&par Directory contents

  - PMU/PMU_Monitor/Source/apm32f0xx_int.c      Interrupt handlers
  - PMU/PMU_Monitor/Source/main.c               Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
