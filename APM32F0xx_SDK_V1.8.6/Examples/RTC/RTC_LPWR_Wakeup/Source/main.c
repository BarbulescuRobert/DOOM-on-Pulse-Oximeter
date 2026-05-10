/*!
 * @file        main.c
 *
 * @brief       Main program body
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

/* Includes */
#include "Board.h"
#include "stdio.h"
#include "main.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"
#include "apm32f0xx_rtc.h"
#include "apm32f0xx_pmu.h"
#include "apm32f0xx_syscfg.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup RTC_LPWR_Wakeup
  @{
*/

/** @defgroup RTC_LPWR_Wakeup_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

#if defined (APM32F030xC) || defined (APM32F072) || defined (APM32F091)
#define RCM_PLL_SRC_SEL   2
#else
#define RCM_PLL_SRC_SEL   1
#endif /* APM32F030xC || APM32F072 || APM32F091 */

/**@} end of group RTC_LPWR_Wakeup_Macros */

/** @defgroup RTC_LPWR_Wakeup_Enumerations Enumerations
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_Enumerations */

/** @defgroup RTC_LPWR_Wakeup_Structures Structures
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_Structures */

/** @defgroup RTC_LPWR_Wakeup_Variables Variables
  @{
*/

/**@} end of group RTC_LPWR_Wakeup_Variables */

/** @defgroup RTC_LPWR_Wakeup_Functions Functions
  @{
*/

/* Delay */
void Delay(void);
/* RTC Init */
void RTC_Init(void);
/* RTC Alarm Init */
void RTC_AlarmsConfig(void);

void rtc_isr(void);
void wakeup_config(void);

#ifndef STANDBY_MODE
    static void SystemClock48M(void);
#endif

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
int main(void)
{
    uint8_t i;

    APM_MINI_LEDInit(LED2);
    APM_MINI_COMInit(COM1);

    wakeup_config();

    printf("startup!\r\n");

    while (1)
    {
        i = 6;
        while (i--)
        {
            APM_MINI_LEDToggle(LED2);
            Delay();
        }
        printf("enter lower power mode !\r\n");
        Delay();

#ifdef STANDBY_MODE

        if (PMU_ReadStatusFlag(PMU_FLAG_WUPF) == 1)
        {
            /* quit the standby mode should clear the WUEFLG flag */
            PMU_ClearStatusFlag(PMU_FLAG_WUPF);
        }
        PMU_EnterSTANDBYMode();
#else
        PMU_EnterSTOPMode(PMU_REGULATOR_ON, PMU_STOPENTRY_WFI);
        /* quit the stop mode should reconfigure frequency of master clock */
        SystemClock48M();
        printf("wakeup!\r\n");
#endif
    }
}

#ifndef STANDBY_MODE
static void SystemClock48M(void)
{
    uint32_t i;

    RCM->CTRL1_B.HSEEN = BIT_SET;

    for (i = 0; i < HSE_STARTUP_TIMEOUT; i++)
    {
        if (RCM->CTRL1_B.HSERDYFLG)
        {
            break;
        }
    }

    if (RCM->CTRL1_B.HSERDYFLG)
    {
        /* Enable Prefetch Buffer */
        FMC->CTRL1_B.PBEN = BIT_SET;
        /* Flash 1 wait state */
        FMC->CTRL1_B.WS = 1;

        /* HCLK = SYSCLK */
        RCM->CFG1_B.AHBPSC = 0X00;

        /* PCLK = HCLK */
        RCM->CFG1_B.APB1PSC = 0X00;

        /* PLL: HSE * 6 */
        RCM->CFG1_B.PLLSRCSEL = RCM_PLL_SRC_SEL;
        RCM->CFG1_B.PLLMULCFG = 4;

        /* Enable PLL */
        RCM->CTRL1_B.PLLEN = 1;

        /* Wait PLL Ready */
        while (RCM->CTRL1_B.PLLRDYFLG == BIT_RESET);

        /* Select PLL as system clock source */
        RCM->CFG1_B.SCLKSEL = 2;

        /* Wait till PLL is used as system clock source */
        while (RCM->CFG1_B.SCLKSWSTS != 0x02);
    }
}
#endif

void wakeup_config(void)
{
    /* Enable the PWR clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* Allow access to RTC */
    PMU_EnableBackupAccess();

    /* Reset RTC*/
    RTC_Reset();

    /* Config RTC */
    RTC_Init();

    /*Config Alarm*/
    RTC_AlarmsConfig();

    /* Alarm Enable */
    RTC_EnableAlarm();
}

/*!
 * @brief       RTC Init
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void RTC_Init(void)
{
    RTC_Config_T Struct;
    EINT_Config_T enit_t;

    enit_t.line = EINT_LINE17;
    enit_t.lineCmd = ENABLE;
    enit_t.mode = EINT_MODE_INTERRUPT;
    /* Must be RISING or ALL */
    enit_t.trigger = EINT_TRIGGER_RISING;
    EINT_Config(&enit_t);

    /* Enable the PWR clock */
    RCM_EnableAPB1PeriphClock(RCM_APB1_PERIPH_PMU);

    /* Allow access to RTC */
    PMU_EnableBackupAccess();
    /* software reset RTC */
    RCM->BDCTRL_B.BDRST = BIT_SET;
    RCM->BDCTRL_B.BDRST = BIT_RESET;
    /* Use LSE for RTC clock */
    RCM_ConfigLSE(RCM_LSE_OPEN);

    /* Wait till LSE is ready */
    while (RCM_ReadStatusFlag(RCM_FLAG_LSERDY) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCM_ConfigRTCCLK(RCM_RTCCLK_LSE);

    /* Enable RTC Clock*/
    RCM_EnableRTCCLK();

    RTC_EnableInterrupt(RTC_INT_ALR);
    /* Open RTC ALARM interrupt whether into standby mode or stop mode */
    NVIC_EnableIRQRequest(RTC_IRQn, 1);

    /* Disable Write Proctection */
    RTC_DisableWriteProtection();

    /* Wait Synchro*/
    RTC_WaitForSynchro();

    RTC_ConfigStructInit(&Struct);

    RTC_Config(&Struct);
}

#ifndef STANDBY_MODE
void rtc_isr(void)
{
    if (RTC_ReadIntFlag(RTC_INT_FLAG_ALR) == 1)
    {
        RTC_ClearIntFlag(RTC_INT_FLAG_ALR);
        /* Clear the EINT17 interrupt flag which is related to RTC alarm interrupt */
        EINT_ClearIntFlag(EINT_LINE17);

        RTC_TIME_T TimeStruct;
        /* reload the time of RTC, the alarm is 2 s */
        TimeStruct.H12 = RTC_H12_AM;
        TimeStruct.hours = 0;
        TimeStruct.minutes = 0;
        TimeStruct.seconds = 0;
        RTC_ConfigTime(RTC_FORMAT_BIN, &TimeStruct);
    }
}
#endif

/*!
 * @brief       Config Alarm
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void RTC_AlarmsConfig(void)
{
    RTC_TIME_T TimeStruct;
    RTC_ALARM_T AlarmStruct;

    /* Config Alarm */
    RTC_ConfigAlarmStructInit(&AlarmStruct);
    AlarmStruct.AlarmMask = 0x80808000;
    /* Cycle 4 s */
    AlarmStruct.time.seconds = 4;
    AlarmStruct.time.hours = 0;
    AlarmStruct.time.minutes = 0;
    RTC_ConfigAlarm(RTC_FORMAT_BIN, &AlarmStruct);

    /* Config Time */
    TimeStruct.H12 = RTC_H12_AM;
    TimeStruct.hours = 0;
    TimeStruct.minutes = 0;
    TimeStruct.seconds = 0;
    RTC_ConfigTime(RTC_FORMAT_BIN, &TimeStruct);
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
    volatile uint32_t delay = 0xFfFf5;

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

/**@} end of group RTC_LPWR_Wakeup_Functions */
/**@} end of group RTC_LPWR_Wakeup */
/**@} end of group Examples */
