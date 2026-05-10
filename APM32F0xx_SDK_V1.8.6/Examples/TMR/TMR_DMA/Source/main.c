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
#include "apm32f0xx_gpio.h"
#include "apm32f0xx_tmr.h"
#include "apm32f0xx_dma.h"
#include "apm32f0xx_misc.h"
#include "apm32f0xx_eint.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup TMR_DMA
  @{
*/

/** @defgroup TMR_DMA_Macros Macros
  @{
*/

/**@} end of group TMR_DMA_Macros */

/** @defgroup TMR_DMA_Enumerations Enumerations
  @{
*/

/**@} end of group TMR_DMA_Enumerations */

/** @defgroup TMR_DMA_Structures Structures
  @{
*/

/**@} end of group TMR_DMA_Structures */

/** @defgroup TMR_DMA_Variables Variables
  @{
*/

/**@} end of group TMR_DMA_Variables */

/** @defgroup TMR_DMA_Functions
  @{
*/

/* TMR1 Init */
void TMR_Init(void);
void GPIO_Init(void);
void DMA_Init(uint32_t* Buf);

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

    uint16_t ConfigBuf[3] = {800, 400, 200};

    GPIO_Init();
    DMA_Init((uint32_t*)ConfigBuf);
    TMR_Init();

    while (1)
    {
    }
}

/*!
 * @brief       GPIO Init
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void GPIO_Init()
{
    GPIO_Config_T GPIO_ConfigStruct;

    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);

    /* TMR1_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);
    /* TMR1_CH1N */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN2);

    GPIO_ConfigStruct.mode    = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed   = GPIO_SPEED_50MHz;
    GPIO_ConfigStruct.pupd    = GPIO_PUPD_NO;
    GPIO_ConfigStruct.pin     = GPIO_PIN_8;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin     = GPIO_PIN_7;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);
}

/*!
 * @brief       DMA Init
 *
* @param       Buf : PWM duty cycle
 *
 * @retval      None
 *
 * @note
 */
void DMA_Init(uint32_t* Buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);

    /* size of buffer */
    dmaConfig.bufferSize = 3;
    /* set memory Data Size */
    dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
    /* Set peripheral Data Size */
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
    /* Enable Memory Address increase */
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase */
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode */
    dmaConfig.circular = DMA_CIRCULAR_ENABLE;
    /* set priority */
    dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;
    /* read from peripheral */
    dmaConfig.direction = DMA_DIR_MEMORY;
    /* Set memory Address */
    dmaConfig.memoryAddress = (uint32_t)Buf;
    /* Set Peripheral Address */
    dmaConfig.peripheralAddress = (uint32_t)&TMR1->CC1;

    DMA_Config(DMA1_CHANNEL_2, &dmaConfig);

    DMA_Enable(DMA1_CHANNEL_2);
}

/*!
 * @brief       TMR Init
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void TMR_Init()
{
    TMR_TimeBase_T timeBaseConfig;
    TMR_OCConfig_T OCconfigStruct;

    /* Enable Clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);

    /* Set clockDivision = 1 */
    timeBaseConfig.clockDivision = TMR_CKD_DIV1;
    /* Up-counter */
    timeBaseConfig.counterMode = TMR_COUNTER_MODE_UP;
    /* TMR1 clock freq ~= 48/(47 + 1) = 1MHZ */
    timeBaseConfig.div = 47 ;
    /* Set counter = 1000 */
    timeBaseConfig.period = 999;
    /* Repetition counter = 0x06 */
    timeBaseConfig.repetitionCounter = 6;

    TMR_ConfigTimeBase(TMR1, &timeBaseConfig);

    /* Configure channel */
    OCconfigStruct.OC_Mode         = TMR_OC_MODE_PWM1;
    OCconfigStruct.OC_Idlestate    = TMR_OCIDLESTATE_RESET;
    OCconfigStruct.OC_NIdlestate   = TMR_OCNIDLESTATE_RESET;
    OCconfigStruct.OC_Polarity     = TMR_OC_POLARITY_HIGH;
    OCconfigStruct.OC_NPolarity    = TMR_OC_NPOLARITY_HIGH;
    OCconfigStruct.OC_OutputState  = TMR_OUTPUT_STATE_ENABLE;
    OCconfigStruct.OC_OutputNState = TMR_OUTPUT_NSTATE_ENABLE;
    OCconfigStruct.Pulse = 500;

    TMR_OC1Config(TMR1, &OCconfigStruct);
    TMR_OC1PreloadConfig(TMR1, TMR_OC_PRELOAD_ENABLE);
    TMR_EnableAUTOReload(TMR1);

    TMR_EnableDMASoure(TMR1, TMR_DMA_CH1);

    /*  Enable TMR1  */
    TMR_Enable(TMR1);

    /* Enable TMRx PWM output */
    TMR_EnablePWMOutputs(TMR1);
}

/**@} end of group TMR_DMA_Functions */
/**@} end of group TMR_DMA */
/**@} end of group Examples */
