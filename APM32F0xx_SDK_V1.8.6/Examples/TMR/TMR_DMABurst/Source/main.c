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
#include "apm32f0xx_dma.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup TMR_DMABurst
  @{
  */

/** @defgroup TMR_DMABurst_Macros Macros
  @{
  */

/**@} end of group TMR_DMABurst_Macros */

/** @defgroup TMR_DMABurst_Enumerations Enumerations
  @{
  */

/**@} end of group TMR_DMABurst_Enumerations */

/** @defgroup TMR_DMABurst_Structures Structures
  @{
  */

/**@} end of group TMR_DMABurst_Structures */

/** @defgroup TMR_DMABurst_Variables Variables
  @{
  */

/**@} end of group TMR_DMABurst_Variables */

/** @defgroup TMR_DMABurst_Functions Functions
  @{
  */

/* DMA Init */
void DMA_Init(uint32_t* Buf);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    uint16_t ConfigBuf[6] = {999, 0, 800, 700, 600, 500};

    GPIO_Config_T GPIO_ConfigStruct;
    TMR_TimeBase_T TMR_TimeBaseStruct;
    TMR_OCConfig_T OCcongigStruct;

    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_TMR1);
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA | RCM_AHB_PERIPH_GPIOB);

    /* TMR1_CH1 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_8, GPIO_AF_PIN2);
    /* TMR1_CH2 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_9, GPIO_AF_PIN2);
    /* TMR1_CH3 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_10, GPIO_AF_PIN2);
    /* TMR1_CH1N */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN2);
    /* TMR1_CH2N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_14, GPIO_AF_PIN2);
    /* TMR1_CH3N */
    GPIO_ConfigPinAF(GPIOB, GPIO_PIN_SOURCE_15, GPIO_AF_PIN2);
    /* TMR1_CH4 */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_11, GPIO_AF_PIN2);

    /* Config TMR1 GPIO for complementary output PWM */
    GPIO_ConfigStruct.pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_ConfigStruct.mode = GPIO_MODE_AF;
    GPIO_ConfigStruct.outtype = GPIO_OUT_TYPE_PP;
    GPIO_ConfigStruct.speed = GPIO_SPEED_50MHz;
    GPIO_Config(GPIOA, &GPIO_ConfigStruct);

    GPIO_ConfigStruct.pin = GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Config(GPIOB, &GPIO_ConfigStruct);

    DMA_Init((uint32_t*)ConfigBuf);

    /* config TMR1 */
    TMR_TimeBaseStruct.clockDivision = TMR_CKD_DIV1;
    TMR_TimeBaseStruct.counterMode = TMR_COUNTER_MODE_UP;
    TMR_TimeBaseStruct.div = 47;
    TMR_TimeBaseStruct.period = 9999;
    TMR_TimeBaseStruct.repetitionCounter = 0;
    TMR_ConfigTimeBase(TMR1, &TMR_TimeBaseStruct);

    /* Configure channel */
    OCcongigStruct.OC_Mode = TMR_OC_MODE_PWM1;
    OCcongigStruct.Pulse = 8000;
    OCcongigStruct.OC_Idlestate = TMR_OCIDLESTATE_RESET;
    OCcongigStruct.OC_OutputState = TMR_OUTPUT_STATE_ENABLE;
    OCcongigStruct.OC_Polarity = TMR_OC_POLARITY_HIGH;
    OCcongigStruct.OC_NIdlestate = TMR_OCNIDLESTATE_RESET;
    OCcongigStruct.OC_OutputNState = TMR_OUTPUT_NSTATE_ENABLE;
    OCcongigStruct.OC_NPolarity = TMR_OC_NPOLARITY_HIGH;

    TMR_OC1Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 7000;
    TMR_OC2Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 6000;
    TMR_OC3Config(TMR1, &OCcongigStruct);

    OCcongigStruct.Pulse = 5000;
    TMR_OC4Config(TMR1, &OCcongigStruct);

    /* Configures the TMR1 DMA interface */
    TMR_ConfigDMA(TMR1, TMR_DMABASE_AUTORLD, TMR_DMA_BURSTLENGHT_6TRANSFERS);
    TMR_EnableDMASoure(TMR1, TMR_DMA_UPDATE);

    TMR_Enable(TMR1);

    /* Enable TMRx PWM output */
    TMR_EnablePWMOutputs(TMR1);

    while (1)
    {
    }
}

/*!
 * @brief       DMA Init
 *
 * @param       None
 *
 * @retval      None
 */
void DMA_Init(uint32_t* Buf)
{
    /* DMA Configure */
    DMA_Config_T dmaConfig;

    /* Enable DMA clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_DMA1);

    /* size of buffer*/
    dmaConfig.bufferSize = 6;
    /* set memory Data Size*/
    dmaConfig.memoryDataSize = DMA_MEMORY_DATASIZE_HALFWORD;
    /* Set peripheral Data Size*/
    dmaConfig.peripheralDataSize = DMA_PERIPHERAL_DATASIZE_HALFWORD;
    /* Enable Memory Address increase*/
    dmaConfig.memoryInc = DMA_MEMORY_INC_ENABLE;
    /* Disable Peripheral Address increase*/
    dmaConfig.peripheralInc = DMA_PERIPHERAL_INC_DISABLE;
    /* Reset Circular Mode*/
    dmaConfig.circular = DMA_CIRCULAR_DISABLE;
    /* set priority*/
    dmaConfig.priority = DMA_PRIORITY_LEVEL_HIGHT;
    /* read from peripheral*/
    dmaConfig.direction = DMA_DIR_PERIPHERAL;
    /* Set memory Address*/
    dmaConfig.memoryAddress = (uint32_t)Buf;
    /* Set Peripheral Address*/
    dmaConfig.peripheralAddress = (uint32_t)&TMR1->DMADDR;

    DMA_Config(DMA1_CHANNEL_5, &dmaConfig);

    DMA_Enable(DMA1_CHANNEL_5);
}

/**@} end of group TMR_DMABurst_Functions */
/**@} end of group TMR_DMABurst */
/**@} end of group Examples */
