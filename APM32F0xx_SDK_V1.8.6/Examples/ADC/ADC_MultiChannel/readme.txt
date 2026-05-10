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

This example describes how to use the ADC1 scan the input values of
all ADC external channels(ADC_CH0-ADC_CH15) one by one.
The converted voltage is displayed on serial assistant through USART1.

&par Hardware Description

using ADC_Channel0-7(PA0-PA7)
using ADC_Channel8-9(PB0-PB1)
using ADC_Channel10-15(PC0-PC5)

using USART1(TX:PA9、RX:PA10).
  - USART1 configured as follow:
  - BaudRate    =  115200
  - Word Length =  USART_WordLength_8b
  - Stop Bit    =  USART_StopBits_1
  - Parity      =  USART_Parity_No
  - Hardware flow control disabled (RTS and CTS signals)
  - Receive and transmit enabled

&par Directory contents

   - ADC/ADC_MultiChannel/Source/apm32f0xx_int.c        Interrupt handlers
   - ADC/ADC_MultiChannel/Source/main.c                 Main program

&par Hardware and Software environment

  - This example runs on APM32F072 MINI Devices.
