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
#include "main.h"
#include "stdio.h"
#include "Board.h"
#include "apm32f0xx_dma.h"
#include "apm32f0xx_adc.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup ADC_VBAT
  @{
  */

/** @defgroup ADC_VBAT_Macros Macros
  @{
  */

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/* define Tramismit Buf Size */
#define BufSize 0x100

/**@} end of group ADC_VBAT_Macros */

/** @defgroup ADC_VBAT_Enumerations Enumerations
  @{
  */

/**@} end of group ADC_VBAT_Enumerations */

/** @defgroup ADC_VBAT_Structures Structures
  @{
  */

/**@} end of group ADC_VBAT_Structures */

/** @defgroup ADC_VBAT_Variables Variables
  @{
  */

/**@} end of group ADC_VBAT_Variables */

/** @defgroup ADC_VBAT_Functions Functions
  @{
  */

/* Delay */
void Delay(uint32_t count);
/* ADC Init */
void ADC_Init(void);
/* DMA Init */
void DMA_Init(uint32_t* Buf);

/*!
 * @brief     Main program
 *
 * @param     None
 *
 * @retval    None
 */
int main(void)
{
    /* DMA value from ADC Dual Interleaved Mode*/
    uint32_t DMA_ConvertedValue = 0;

    /* ADC1 convert to volatage*/
    uint16_t ADC1_ConvertedValue = 0;

    /* VBAT volatage*/
    uint16_t VBatValue = 0;

    /* COM1 init*/
    APM_MINI_COMInit(COM1);

    DMA_Init(&DMA_ConvertedValue);

    ADC_Init();
    ADC_StartConversion();

    while (1)
    {

        if (DMA_ReadStatusFlag(DMA1_FLAG_TF1))
        {
            ADC1_ConvertedValue = (uint16_t)(((float)DMA_ConvertedValue / 4095 * 3.3) * 1000);

            /* The VBAT pin is internally connected to a bridge divider by 2 */
            VBatValue = ADC1_ConvertedValue * 2;

            printf("\r\n");
            printf("ADC REGDATA = 0x%04X \r\n", DMA_ConvertedValue);
            printf("ADC CH18: Volatage    = %d mV \r\n", ADC1_ConvertedValue);
            printf("VBAT: Volatage    = %d mV \r\n", VBatValue);

            Delay(0xFFFFFF);
            DMA_ClearStatusFlag(DMA1_FLAG_TF1);
        }
    }
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
    ADC_Config_T  adcConfig;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_ADC1);

    /* ADC Configuration */
    ADC_Reset();

    ADC_ConfigStructInit(&adcConfig);
    /* Set resolution*/
    adcConfig.resolution = ADC_RESOLUTION_12B;
    /* Set dataAlign*/
    adcConfig.dataAlign = ADC_DATA_ALIGN_RIGHT;
    /* Set scanDir*/
    adcConfig.scanDir = ADC_SCAN_DIR_UPWARD;
    /* Set convMode continous*/
    adcConfig.convMode = ADC_CONVERSION_CONTINUOUS;
    /* Set extTrigEdge*/
    adcConfig.extTrigEdge = ADC_EXT_TRIG_EDGE_NONE;
    /* Set extTrigConv*/
    adcConfig.extTrigConv = ADC_EXT_TRIG_CONV_TRG0;

    ADC_Config(&adcConfig);

    ADC_ConfigChannel(ADC_CHANNEL_18, ADC_SAMPLE_TIME_239_5);

    ADC_EnableVbat();

    /* Calibration */
    ADC_ReadCalibrationFactor();
    ADC_EnableDMA();

    ADC_DMARequestMode(ADC_DMA_MODE_CIRCULAR);
    /* Enable ADC */
    ADC_Enable();
}

/*!
 * @brief     DMA Init
 *
 * @param     None
 *
 * @retval    None
 */
void DMA_Init(uint32_t* Buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);

    /* size of buffer*/
    dmaConfig.bufferSize = 1;
    /* set memory Data Size */
    dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
    /* Set peripheral Data Size */
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
    /* Enable Memory Address increase */
    dmaConfig.memoryInc = DMA_MEMORY_INC_DISABLE;
    /* Disable Peripheral Address increase */
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode */
    dmaConfig.circular = DMA_CIRCULAR_ENABLE;
    /* set priority */
    dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;
    /* read from peripheral */
    dmaConfig.direction = DMA_DIR_PERIPHERAL;
    /* Set memory Address */
    dmaConfig.memoryAddress = (uint32_t)Buf;
    /* Set Peripheral Address */
    dmaConfig.peripheralAddress = (uint32_t)&ADC->DATA;
    /* Disable M2M */
    dmaConfig.memoryTomemory = DMA_M2M_DISABLE;

    DMA_Config(DMA1_CHANNEL_1, &dmaConfig);

    DMA_Enable(DMA1_CHANNEL_1);
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

/**@} end of group ADC_VBAT_Functions */
/**@} end of group ADC_VBAT */
/**@} end of group Examples */
