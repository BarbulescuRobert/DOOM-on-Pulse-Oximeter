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

This example describes how to use TMR3 Channel_2 (PE4 for APM32F072 | PA7 for APM32F030) measure frequency and duty cycle of external signa. User can view the "DutyCycle" "Frequency "value through serial termainal.

TMR3_IRQHandler Function to calculate:
DutyCycle =  (TMR3_CC1*100)/(TMR3_CC2)  %.
Frequency = (RCM_ReadPCLKFreq()) / (TMR3_CC2) Hz.
The minimum frequency value to measure is 1 Hz.

&par Hardware Description

using USART1(TX:PA9, RX:PA10).
  - USART2 configured as follow:
  - BaudRate    =   115200
  - Word Length =   USART_WordLength_8b
  - Stop Bit    =   USART_StopBits_1
  - Parity      =   USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

  - using TMR3 CHANNEL2(PE4 for APM32F072 | PA7 for APM32F030) connect the external signal

&par Directory contents

  - TMR/TMR_PWMInput/Source/apm32f0xx_int.c     Interrupt handlers
  - TMR/TMR_PWMInput/Source/main.c              Main program

&par Hardware and Software environment

  - This example runs on APM32F030 & APM32F072 MINI Devices.
