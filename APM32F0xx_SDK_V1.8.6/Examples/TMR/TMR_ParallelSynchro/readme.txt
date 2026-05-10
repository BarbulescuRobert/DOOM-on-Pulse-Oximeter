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

This example shows how to synchronize TMR peripherals in parallel mode.

  - parallel mode:
  - TMR1 Master for TMR2 and TMR3, Config PWM Mode and TMR1 Update event is used as Trigger Output
  - TMR2 is slave for TMR1,
  - Config PWM Mode
  - The ITR1(TMR1) is used as input trigger
  - TMR3 is slave for TMR1,
  - Config PWM Mode
  - The ITR1(TMR1) is used as input trigger

  - The TMR1 peripherals clock is 48 MHz.

  - The Master Timer TMR1 frequency :
  - TMR1 frequency = (48M)/ (127 + 1) = 375 KHz
  - duty cycle = 64/(127 + 1) = 50%.

  - The TMR2 is running at:
  - (375  KHz)/ (1 + 1) = 187.5 KHz
  - duty cycle = 1/(1 + 1) = 50%

  - The TMR3 is running at:
  - (375 KHz)/ (3 + 1) = 93.75 KHz
  - duty cycle = 2/(3 + 1) = 50%

&par Hardware Description

  - using TMR1 CHANNEL1(PA8) to output PWM
  - using TMR2 CHANNEL1(PA15) to output PWM
  - using TMR3 CHANNEL1(PB4) to output PWM

&par Directory contents

  - TMR/TMR_ParallelSynchro/Source/apm32f0xx_int.c        Interrupt handlers
  - TMR/TMR_ParallelSynchro/Source/main.c                 Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
