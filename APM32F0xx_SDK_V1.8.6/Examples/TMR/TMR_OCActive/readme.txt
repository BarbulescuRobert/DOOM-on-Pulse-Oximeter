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

The program to show how to configure the TMR3 peripheral to generate 4 different
signals with four different delays.

  - SystemCoreClock = 48MHz.
  - TMR3CLK = SystemCoreClock = 48 MHz.Prescaler = 47, TMR counter clock = 1 MHz
  - TMR3_Period    = (1000000 / 1000) - 1
  - TMR3 CH1 pulse = 800
  - TMR3 CH1 delay = 800/1MHz = 800us
  - TMR3 CH2 pulse = 600
  - TMR3 CH2 delay = 600/1MHz = 600us
  - TMR3 CH3 pulse = 400
  - TMR3 CH3 delay = 400/1MHz = 400us
  - TMR3 CH4 pulse = 200
  - TMR3 CH4 delay = 200/1MHz = 200us

The CHx delay correspond to the time difference between PE6 falling edge and  TMR3_CHx signal
rising edges. Reset system and display TMR3 waveform by oscilloscope.

&par Hardware Description

  - using TMR3 Channel 1 (PB4) to output signal
  - using TMR3 Channel 2 (PB5) to output signal
  - using TMR3 Channel 3 (PB0) to output signal
  - using TMR3 Channel 4 (PB1) to output signal

&par Directory contents

  - TMR/TMR_OCActive/Source/apm32f0xx_int.c        Interrupt handlers
  - TMR/TMR_OCActive/Source/main.c                 Main program

&par Hardware and Software environment

  - This example runs on APM32F030 & APM32F072 MINI Devices.
