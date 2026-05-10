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

The program to show how to configure the TMR1 peripheral to generate 6 Steps.
In this example, a software COM event is generated each 100 ms.

The TMR1 is configured in Timing Mode, each time a COM event occurs, a new TMR1
configuration will be set in advance.

A  =  Channel 1
A' =  Channel 1N
B  =  Channel 2
B' =  Channel 2N
C  =  Channel 3
C' =  Channel 3N

  - Step1
  - A  &  B'
  - Step2
  - B' &  C
  - Step3
  - C  &  A'
  - Step4
  - A' &  B
  - Step5
  - B  &  C'
  - Step6
  - C' &  A

Display TMR1 waveform by oscilloscope.

&par Hardware Description

  - using TMR1 Channel 1  (PA8) to output PWM
  - using TMR1 Channel 2  (PA9) to output PWM
  - using TMR1 Channel 3 (PA10) to output PWM
  - using TMR1 Channel 1N (PA7) to output PWM
  - using TMR1 Channel 2N (PB0) to output PWM
  - using TMR1 Channel 3N (PB1) to output PWM

&par Directory contents

  - TMR/TMR_6Steps/Source/apm32f0xx_int.c        Interrupt handlers
  - TMR/TMR_6Steps/Source/main.c                 Main program

&par Hardware and Software environment

  - This example runs on APM32F030 or APM32F072 MINI Devices.
