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

This example shows how to configure all TMR peripheral to generate PWM signals.
The PWM waveform can be displayed using an oscilloscope.

&par Hardware Description

using PE9  as TMR1 output channel1
using PE11 as TMR1 output channel2
using PE13 as TMR1 output channel3
using PE14 as TMR1 output channel4

using PA0  as TMR2 output channel1
using PA1  as TMR2 output channel2
using PA2  as TMR2 output channel3
using PA3  as TMR2 output channel4

using PC6  as TMR3 output channel1
using PC7  as TMR3 output channel2
using PC8  as TMR3 output channel3
using PC9  as TMR3 output channel4

using PA7  as TMR14 output channel1
using PB14 as TMR15 output channel1
using PB15 as TMR15 output channel2
using PB8  as TMR16 output channel1
using PB9  as TMR17 output channel1

&par Directory contents

  - TMR/TMR_MulitPwmOutput/Source/apm32f0xx_int.c            Interrupt handlers
  - TMR/TMR_MulitPwmOutput/Source/main.c                     Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
