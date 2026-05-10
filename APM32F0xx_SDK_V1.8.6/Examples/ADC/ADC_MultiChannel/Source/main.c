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
#include "main.h"
#include "Board.h"
#include "stdio.h"
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_adc.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_usart.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup ADC_MultiChannel
  @{
  */

/** @defgroup ADC_MultiChannel_Macros Macros
  @{
  */

/* save ADC data */
#define ADC_CH_SIZE 16
#define ADC_DR_ADDR ((uint32_t)ADC_BASE + 0x40)

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group ADC_MultiChannel_Macros*/

/** @defgroup ADC_MultiChannel_Variables Variables
  @{
  */

static GPIO_Config_T g_gpioConfig =
{
    .mode = GPIO_MODE_AN,
    .pupd = GPIO_PUPD_NO,
    .outtype = GPIO_OUT_TYPE_PP,
    .speed = GPIO_SPEED_10MHz
};

static ADC_Config_T g_adcConfig =
{
    .resolution = ADC_RESOLUTION_12B,
    .dataAlign = ADC_DATA_ALIGN_RIGHT,
    .scanDir = ADC_SCAN_DIR_UPWARD,
    .convMode = ADC_CONVERSION_SINGLE,
    .extTrigConv = ADC_EXT_TRIG_CONV_TRG0,
    .extTrigEdge = ADC_EXT_TRIG_EDGE_NONE
};

static uint32_t g_adcChannel[ADC_CH_SIZE] =
{
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_4,
    ADC_CHANNEL_5,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15
};

volatile static uint32_t g_delayCount = 0;

/**@} end of group ADC_MultiChannel_Variables*/

/** @defgroup ADC_MultiChannel_Functions Functions
  @{
  */

void DelayInit(void);
void Delay(uint32_t milliseconds);
void GPIO_Initialize(void);
void ADC_Initialize(void);
void ADC_PollingScan(void);
void ADC_DisableChannel(uint32_t channel);

/*!
 * @brief Main program
 *
 * @param None
 *
 * @retval None
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    /* COM1 initialize */
    APM_MINI_COMInit(COM1);
    /* GPIO initialize */
    GPIO_Initialize();
    /* ADC initialize */
    ADC_Initialize();
    DelayInit();

    while (1)
    {
        ADC_PollingScan();
        APM_MINI_LEDToggle(LED2);
        Delay(1000);
    }
}

/*!
 * @brief ADC multi channel polling
 *
 * @param None
 *
 * @retval None
 */
void ADC_PollingScan(void)
{
    float voltage = 0;
    uint8_t index = 0;
    uint16_t adcData = 0;

    for (index = 0; index < ADC_CH_SIZE; index++)
    {
        /* Wait for ADC conversion ready */
        while(ADC_ReadStatusFlag(ADC_FLAG_ADRDY) == RESET);
        /* Enable ADC conversion channel */
        ADC_ConfigChannel(g_adcChannel[index], ADC_SAMPLE_TIME_239_5);
        /* Start ADC conversion */
        ADC_StartConversion();
        /* Wait for ADC conversion finish */
        while(ADC_ReadStatusFlag(ADC_FLAG_CC) == RESET);
        /* Get ADC conversion value */
        adcData = ADC_ReadConversionValue();
        /* Conversion to real voltage */
        voltage = (adcData * 3300.0) / 4095.0;
        /* Output the conversion value */
        printf("ADC CH[%d] voltage = %.3f V\r\n", index, voltage);
        /* Wait for ADC conversion ready */
        while(ADC_ReadStatusFlag(ADC_FLAG_ADRDY) == RESET);
        /* Enable ADC conversion channel */
        ADC_DisableChannel(g_adcChannel[index]);
    }
    printf("\r\n");
}

/*!
 * @brief GPIO Initialize
 *
 * @param None
 *
 * @retval None
 */
void GPIO_Initialize(void)
{
    /* Enable GPIO clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOB);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOC);

    /* ADC channel0-7 configuration */
    g_gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 |
                       GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Config(GPIOA, &g_gpioConfig);

    /* ADC channel8-9 configuration */
    g_gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_Config(GPIOB, &g_gpioConfig);

    /* ADC channel10-15 configuration */
    g_gpioConfig.pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |
                       GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_Config(GPIOC, &g_gpioConfig);
}

/*!
 * @brief ADC Initialize
 *
 * @param None
 *
 * @retval None
 */
void ADC_Initialize(void)
{
    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset();
    ADC_Config(&g_adcConfig);

    /* Calibration */
    ADC_ReadCalibrationFactor();

    /* Enable ADC */
    ADC_Enable();
}

/*!
 * @brief Disable ADC channel
 *
 * @param channel: adc channel number
 *
 * @retval None
 */
void ADC_DisableChannel(uint32_t channel)
{
    ADC->CHSEL &= ~(channel);
}

/*!
 * @brief Delay function Initialize
 *
 * @param None
 *
 * @retval None
 */
void DelayInit(void)
{
    SysTick_Config(SystemCoreClock / 1000);
}

/*!
 * @brief Blocking delay milliseconds
 *
 * @param milliseconds: the delay time
 *
 * @retval None
 */
void Delay(uint32_t milliseconds)
{
    g_delayCount = milliseconds;
    while(g_delayCount);
}

/*!
 * @brief This function handles SysTick Handler
 *
 * @param None
 *
 * @retval None
 */
void SysTick_Handler(void)
{
    g_delayCount--;
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

/**@} end of group ADC_MultiChannel_Functions */
/**@} end of group ADC_MultiChannel */
/**@} end of group Examples */
