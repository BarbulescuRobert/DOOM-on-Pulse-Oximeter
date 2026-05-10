/*!
 * @file        apm32f0xx_int.c
 *
 * @brief       Main Interrupt Service Routines
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

#include "main.h"
#include "apm32f0xx_int.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_PWMInput
  @{
*/

/** @defgroup TMR_PWMInput_INT_Macros Macros
  @{
*/

/**@} end of group TMR_PWMInput_INT_Macros */

/** @defgroup TMR_PWMInput_INT_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_PWMInput_INT_Enumerations */

/** @defgroup TMR_PWMInput_INT_Structures Structures
  @{
*/

/**@} end of group TMR_PWMInput_INT_Structures */

/** @defgroup TMR_PWMInput_INT_Variables Variables
  @{
*/

/**@} end of group TMR_PWMInput_INT_Variables */

/** @defgroup TMR_PWMInput_INT_Functions INT_Functions
  @{
  */

/*!
 * @brief   This function handles NMI exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void NMI_Handler(void)
{
}

/*!
 * @brief   This function handles Hard Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Memory Manage exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Bus Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles Usage Fault exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

/*!
 * @brief   This function handles SVCall exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SVC_Handler(void)
{
}

/*!
 * @brief   This function handles Debug Monitor exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void DebugMon_Handler(void)
{
}

/*!
 * @brief   This function handles PendSV_Handler exception
 *
 * @param   None
 *
 * @retval  None
 *
 */
void PendSV_Handler(void)
{
}

/*!
 * @brief   This function handles SysTick Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void SysTick_Handler(void)
{
    tick ++ ;
}

/*!
 * @brief   This function handles TMR1 Update Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void TMR1_BRK_UP_TRG_COM_IRQHandler(void)
{
    if (TMR_ReadIntFlag(TMR1, TMR_INT_FLAG_UPDATE) == SET)
    {
        tick++;
        TMR_ClearIntFlag(TMR1, TMR_INT_UPDATE);
    }
}

/*!
 * @brief   This function handles TMR5 Handler
 *
 * @param   None
 *
 * @retval  None
 *
 */
void TMR3_IRQHandler(void)
{
    TMR_ClearIntFlag(TMR3, TMR_INT_FLAG_CH2);
    IC2Value = TMR_ReadCaputer2(TMR3);

    if (IC2Value != 0)
    {
        DutyCycle = (TMR_ReadCaputer1(TMR3) * 100) / IC2Value;
        Frequency = (RCM_ReadPCLKFreq()) / IC2Value;
    }
    else
    {
        DutyCycle = 0;
        Frequency = 0;
    }
    irqFlag++;
}

/**@} end of group TMR_PWMInput_INT_Functions */
/**@} end of group TMR_PWMInput */
/**@} end of group Examples */
