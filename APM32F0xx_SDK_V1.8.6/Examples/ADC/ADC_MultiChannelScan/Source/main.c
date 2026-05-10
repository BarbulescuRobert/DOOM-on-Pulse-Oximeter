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
#include "apm32f0xx_dma.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup ADC_MultiChannelScan
  @{
  */

/** @defgroup ADC_MultiChannelScan_Macros Macros
  @{
  */

/* printf function configs to USART1*/
#define DEBUG_USART  USART1

/**@} end of group ADC_MultiChannelScan_Macros*/

/** @defgroup ADC_MultiChannelScan_Functions Functions
  @{
  */

/* Delay */
void Delay(uint32_t count);
/* ADC init */
void ADC_Init(void);

void ADC_MultiChannelPolling(void);

/* save adc data*/
#define ADC_CH_SIZE         3
#define ADC_DR_ADDR         ((uint32_t)ADC_BASE + 0x40)

uint16_t adcData[ADC_CH_SIZE];

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    /* COM1 init*/
    APM_MINI_COMInit(COM1);

    /* ADC1 initialization */
    ADC_Init();
    ADC_StartConversion();
    while (1)
    {
        Delay(0x3FFFFF);
        ADC_MultiChannelPolling();
    }
}

/*!
 * @brief     DMA Init
 *
 * @param     None
 *
 * @retval    None
 */
void DMA_Init(void)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);

    /* Set Peripheral Address */
    dmaConfig.peripheralAddress = ADC_DR_ADDR;
    /* Set memory Address */
    dmaConfig.memoryAddress = (uint32_t)&adcData;
    /* read from peripheral */
    dmaConfig.direction = DMA_DIR_PERIPHERAL;
    /* size of buffer*/
    dmaConfig.bufferSize = ADC_CH_SIZE;
    /* Disable Peripheral Address increase */
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Enable Memory Address increase */
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Set peripheral Data Size */
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
    /* set memory Data Size */
    dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
    /* Reset Circular Mode */
    dmaConfig.circular = DMA_CIRCULAR_ENABLE;
    /* Disable M2M */
    dmaConfig.memoryTomemory = DMA_M2M_DISABLE;
    /* set priority */
    dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;

    DMA_Config(DMA1_CHANNEL_1, &dmaConfig);

    DMA_Enable(DMA1_CHANNEL_1);
}

/*!
 * @brief     ADC Init
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_Init(void)
{
    GPIO_Config_T           gpioConfig;
    ADC_Config_T            adcConfig;

    /* Enable GPIOA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* ADC channel 0 configuration */
    GPIO_ConfigStructInit(&gpioConfig);
    gpioConfig.mode    = GPIO_MODE_AN;
    gpioConfig.pupd    = GPIO_PUPD_NO;
    gpioConfig.pin     = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_Config(GPIOA, &gpioConfig);

    /* Enable ADC clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC configuration */
    ADC_Reset();

    ADC_ConfigStructInit(&adcConfig);
    /* Set resolution*/
    adcConfig.resolution            = ADC_RESOLUTION_12B;
    /* Set scanDir*/
    adcConfig.scanDir               = ADC_SCAN_DIR_UPWARD;
    /* Set convMode continous*/
    adcConfig.convMode              = ADC_CONVERSION_CONTINUOUS;
    /* Set dataAlign*/
    adcConfig.dataAlign             = ADC_DATA_ALIGN_RIGHT;
    /* Set extTrigEdge*/
    adcConfig.extTrigEdge           = ADC_EXT_TRIG_EDGE_NONE;
    /* Set extTrigConv*/
    adcConfig.extTrigConv           = ADC_EXT_TRIG_CONV_TRG0;
    ADC_Config(&adcConfig);

    /* ADC channel Convert configuration */
    ADC_ConfigChannel(ADC_CHANNEL_0, ADC_SAMPLE_TIME_239_5);
    ADC_ConfigChannel(ADC_CHANNEL_1, ADC_SAMPLE_TIME_239_5);
    ADC_ConfigChannel(ADC_CHANNEL_2, ADC_SAMPLE_TIME_239_5);

    /* Config DMA*/
    DMA_Init();

    /* Calibration */
    ADC_ReadCalibrationFactor();

    /* Enable ADC DMA*/
    ADC_EnableDMA();
    ADC_DMARequestMode(ADC_DMA_MODE_CIRCULAR);
    /* Enable ADC */
    ADC_Enable();
}

/*!
 * @brief     ADC multi channel polling
 *
 * @param     None
 *
 * @retval    None
 */
void ADC_MultiChannelPolling(void)
{
    float voltage;
    uint8_t index;

    for (index = 0; index < ADC_CH_SIZE; index++)
    {
        voltage = (adcData[index] * 3300.0) / 4095.0;

        printf("ADC CH[%d] voltage = %.3f V\r\n", index, voltage);
    }
    printf("\r\n");
}

/*!
 * @brief     Delay
 *
 * @param     count:  delay count
 *
 * @retval    None
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

/**@} end of group ADC_MultiChannelScan_Functions */
/**@} end of group ADC_MultiChannelScan */
/**@} end of group Examples */
