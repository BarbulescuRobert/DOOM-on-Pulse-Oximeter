/*!
 * @file        main.c
 *
 * @brief       Main program body
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

/* Includes */
#include "Board.h"
#include "stdio.h"
#include "apm32f0xx_pmu.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_misc.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup PMU_WakeUp
  @{
  */

/** @defgroup PMU_WakeUp_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group PMU_WakeUp_Macros */

/** @defgroup PMU_WakeUp_Enumerations Enumerations
  @{
*/

/**@} end of group PMU_WakeUp_Enumerations */

/** @defgroup PMU_WakeUp_Structures Structures
  @{
*/

/**@} end of group PMU_WakeUp_Structures */

/** @defgroup PMU_WakeUp_Variables Variables
  @{
*/

/**@} end of group PMU_WakeUp_Variables */

/** @defgroup PMU_WakeUp_Functions Functions
  @{
  */

/* Delay */
void Delay(uint32_t count);
/* Init WakeUp Line */
void EINT_WakeUp_Line_Init(void);

/*!
 * @brief    main program
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    APM_MINI_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);
    APM_MINI_COMInit(COM1);

    PMU_Reset();
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* Init PA6 as wakeup pin for sleep mode*/
    EINT_WakeUp_Line_Init();

    /* Enable PC13 as system WakeUp pin for standby mode */
    PMU_EnableWakeUpPin(PMU_WAKEUPPIN_2);

    printf("\r\nSystem is Init\r\n");
    printf("Press KEY1   : ENTER Sleep   Mode\r\n");
    printf("Press KEY2   : ENTER StandBy Mode\r\n");
    printf("Low down PA7 : ENTER Stop    Mode\r\n\r\n");

    for (;;)
    {
        Delay(0xfffff);
        APM_MINI_LEDToggle(LED3);
        printf("\r\nSystem is running\r\n");
    }
}

/*!
 * @brief    WakeUp line configure
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
void EINT_WakeUp_Line_Init()
{
    GPIO_Config_T gpioConfig;
    EINT_Config_T eintConfig;
    /* Enable the BUTTON Clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /* Configure Button pin as input floating */
    gpioConfig.mode = GPIO_MODE_IN;
    gpioConfig.pupd = GPIO_PUPD_PU;
    gpioConfig.pin = GPIO_PIN_6;
    GPIO_Config(GPIOA, &gpioConfig);

    gpioConfig.pin = GPIO_PIN_7;
    GPIO_Config(GPIOA, &gpioConfig);

    /* PA6,PA7 set Bits and wait for falling to enter interrupt*/
    GPIO_SetBit(GPIOA, GPIO_PIN_6);
    GPIO_SetBit(GPIOA, GPIO_PIN_7);

    /* Configure EINT1 line */
    eintConfig.line    =  EINT_LINE6;
    eintConfig.lineCmd =  ENABLE;
    eintConfig.mode    =  EINT_MODE_INTERRUPT;
    eintConfig.trigger =  EINT_TRIGGER_FALLING;
    EINT_Config(&eintConfig);

    eintConfig.line    =  EINT_LINE7;
    EINT_Config(&eintConfig);

    /* Enable and set EINTx Interrupt */
    NVIC_EnableIRQRequest(EINT4_15_IRQn, 0x0e);
}

/*!
 * @brief    enter STOP mode interrupt
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
void APM_MINI_PMU_Enter_STOP_Mode_Isr()
{
    if (EINT_ReadStatusFlag(EINT_LINE7) == SET)
    {
        printf("\r\nSystem have entered STOP mode\r\n");
        printf("now, LED2 and LED3 remain in the previous state\r\n");
        printf("and you can reset System to over STOP mode\r\n\r\n");

        /* Enter STOP Mode in WFI*/
        PMU_EnterSTOPMode(PMU_REGULATOR_ON, PMU_STOPENTRY_WFI);
        EINT_ClearStatusFlag(EINT_LINE7);
    }
}
/*!
 * @brief    WakeUp from Sleep mode Interrupt
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
void EINT_WakeUp_From_Sleep_Isr()
{
    if (EINT_ReadStatusFlag(EINT_LINE6) == SET)
    {
        printf("\r\nSystem have exit Sleep mode\r\n");
        EINT_ClearStatusFlag(EINT_LINE6);
    }
}

/*!
 * @brief    WakeUp from STOP mode Interrupt
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
void EINT_WakeUp_From_STOP_Isr()
{
    if (EINT_ReadStatusFlag(EINT_LINE8) == SET)
    {
        EINT_ClearStatusFlag(EINT_LINE8);
        printf("\r\nSystem have exit Stop mode\r\n");
        /* reset System*/
        NVIC_SystemReset();
    }
}

/*!
 * @brief    enter STANDBY mode or Sleep mode Interrupt
 *
 * @param    None
 *
 * @retval   None
 *
 * @note
 */
void APM_MINI_PB_PMU_Isr()
{

    if (EINT_ReadStatusFlag(EINT_LINE0) == SET)
    {
        printf("\r\nSystem have entered STANDBY mode\r\n");
        printf("now, LED2 and LED3 is off\r\n");
        printf("please give PC13 a rising edge or reset System to recover System\r\n\r\n");

        /* Clear flag bit to prevent repeated entry interrupt*/
        PMU_ClearStatusFlag(PMU_FLAG_STDBYF);
        PMU_ClearStatusFlag(PMU_FLAG_WUPF);

        /* Enter StandBy Mode*/
        PMU_EnterSTANDBYMode();
        EINT_ClearStatusFlag(EINT_LINE0);
    }

    if (EINT_ReadStatusFlag(EINT_LINE1) == SET)
    {
        printf("\r\nSystem have entered Sleep mode\r\n");
        printf("now, LED2 is on ,while LED3 LED3 remain in the previous state\r\n");
        printf("and you can low down PA6 or reset System to over Sleep mode\r\n\r\n");

        APM_MINI_LEDToggle(LED2);
        /* Enter Sleep Mode, and Wake up by interrupt*/
        PMU_EnterSleepMode(PMU_SLEEPENTRY_WFI);

        EINT_ClearStatusFlag(EINT_LINE1);
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Delay(uint32_t count)
{
    volatile uint32_t delay = count;

    while (delay--);
}

#if defined (__CC_ARM) || defined (__ICCARM__) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}

#elif defined (__GNUC__)

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @retval      The characters that need to be send.
*
* @note
*/
int __io_putchar(int ch)
{
    /* send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, ch);

    /* wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return ch;
}

/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       file:  Meaningless in this function.
*
* @param       *ptr:  Buffer pointer for data to be sent.
*
* @param       len:  Length of data to be sent.
*
* @retval      The characters that need to be send.
*
* @note
*/
int _write(int file, char* ptr, int len)
{
    int i;
    for (i = 0; i < len; i++)
    {
        __io_putchar(*ptr++);
    }

    return len;
}

#else
#warning Not supported compiler type
#endif

/**@} end of group PMU_WakeUp_Functions */
/**@} end of group PMU_WakeUp */
/**@} end of group Examples */
