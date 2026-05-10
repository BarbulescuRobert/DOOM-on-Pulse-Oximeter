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
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_rcm.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup RCM_ClockSwitch
  @{
  */

/** @defgroup RCM_ClockSwitch_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group RCM_ClockSwitch_Macros */

/** @defgroup RCM_ClockSwitch_Enumerations Enumerations
  @{
*/

/**@} end of group RCM_ClockSwitch_Enumerations */

/** @defgroup RCM_ClockSwitch_Structures Structures
  @{
*/

/**@} end of group RCM_ClockSwitch_Structures */

/** @defgroup RCM_ClockSwitch_Variables Variables
  @{
*/

/**@} end of group RCM_ClockSwitch_Variables */

/** @defgroup RCM_ClockSwitch_Functions Functions
  @{
  */

/* Delay */
void Delay(void);
/* COC Init */
void ClockOutputInit(void);
/* Clock Select */
const char *RCM_SYSCLK_SEL_TAB[4] =
{
    "RCM_SYSCLK_SEL_HSI",
    "RCM_SYSCLK_SEL_HSE",
    "RCM_SYSCLK_SEL_PLL",
    "RCM_SYSCLK_SEL_HSI48"
};

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    APM_MINI_PBInit(BUTTON_KEY1, BUTTON_MODE_EINT);
    APM_MINI_PBInit(BUTTON_KEY2, BUTTON_MODE_EINT);
    APM_MINI_COMInit(COM1);

    ClockOutputInit();
    printf("sysSource = %s ", RCM_SYSCLK_SEL_TAB[RCM_ReadSYSCLKSource()]);
    printf("sysClock = %d\r\n", RCM_ReadSYSCLKFreq());

    for (;;)
    {
        Delay();
        APM_MINI_LEDToggle(LED2);
    }
}

/*!
 * @brief       HSE as System Clock
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SystemClock_HSE_Init()
{
    /* Open HSE */
    RCM_ConfigHSE(RCM_HSE_OPEN);

    /* Wait until HSE is ready */
    while (RCM_WaitHSEReady() == ERROR);

    /* select HSE as SYSCLK */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSE);

    /* Initiatate the usart */
    APM_MINI_COMInit(COM1);

    printf("sysSource = %s ", RCM_SYSCLK_SEL_TAB[RCM_ReadSYSCLKSource()]);
    printf("sysClock = %d\r\n", RCM_ReadSYSCLKFreq());
}


/*!
 * @brief       HSI as System Clock
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SystemClock_HSI_Init()
{
    /* set HSI trimming value */
    RCM_SetHSITrim(16);

    /* Enable HSI */
    RCM_EnableHSI();

    /* select HSI as SYSCLK */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSI);

    /* Initiatate the usart */
    APM_MINI_COMInit(COM1);

    printf("sysSource = %s ", RCM_SYSCLK_SEL_TAB[RCM_ReadSYSCLKSource()]);
    printf("sysClock = %d\r\n", RCM_ReadSYSCLKFreq());
}

/*!
 * @brief       PLL as System Clock
 *
 * @param       None
 *
 * @retval      None
 *
 */
void SystemClock_PLL_Init()
{
    /* Select HSI as System Clock at first */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_HSI);

    /* Disable PLL */
    RCM_DisablePLL();

    /* Wait until Pll is ready */
    while (RCM->CTRL1_B.PLLRDYFLG == SET);

    /* Config PLL source and multiplication factor
        SYSCLKFreq = (HSE * 6) / 4 */
    RCM_ConfigPLL(RCM_PLL_SEL_HSE, RCM_PLLMF_6);
    RCM_ConfigCLKDIV(RCM_CLK_DIV_2);

    /* Enable PLL */
    RCM_EnablePLL();

    /* Selct PLL as Sysclk */
    RCM_ConfigSYSCLK(RCM_SYSCLK_SEL_PLL);

    /* Initiatate the usart */
    APM_MINI_COMInit(COM1);

    printf("sysSource = %s ", RCM_SYSCLK_SEL_TAB[RCM_ReadSYSCLKSource()]);
    printf("sysClock = %d\r\n", RCM_ReadSYSCLKFreq());
}

/*!
 * @brief       Clock output init
 *
 * @param       None
 *
 * @retval      None
 *
 */
void ClockOutputInit()
{
    GPIO_Config_T gpioconfig;

    /* Connect RCM Clock output */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN0);
    gpioconfig.mode    =  GPIO_MODE_AF;
    gpioconfig.outtype =  GPIO_OUT_TYPE_PP;
    gpioconfig.pin     =  GPIO_PIN_8;
    gpioconfig.pupd    =  GPIO_PUPD_NO;
    gpioconfig.speed   =  GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &gpioconfig);

    /* set SYSCLK as COC source */
#if defined (APM32F030) || defined (APM32F051)
    RCM_ConfigCOC(RCM_COC_SYSCLK);
#else
    RCM_ConfigCOC(RCM_COC_SYSCLK, RCM_COC_DIV_128);
#endif
}

/*!
 * @brief       Switch sysclk with Key press
 *
 * @param       None
 *
 * @retval      None
 *
 */
void KEY_SWITCH_SYSCLK_Isr()
{
    if (EINT_ReadStatusFlag(EINT_LINE1) == SET)
    {
        SystemClock_PLL_Init();
        EINT_ClearStatusFlag(EINT_LINE1);
    }

    if (EINT_ReadStatusFlag(EINT_LINE0) == SET)
    {
        SystemClock_HSI_Init();
        EINT_ClearStatusFlag(EINT_LINE0);
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
void Delay(void)
{
    volatile uint32_t delay = 0x7ffff;

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
/**@} end of group RCM_ClockSwitch_Functions */
/**@} end of group RCM_ClockSwitch */
/**@} end of group Examples */
