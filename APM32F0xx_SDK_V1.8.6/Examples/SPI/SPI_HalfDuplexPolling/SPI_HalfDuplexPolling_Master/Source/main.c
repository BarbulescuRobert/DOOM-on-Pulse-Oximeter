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
#include "apm32f0xx_spi.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup SPI_HalfDuplexPolling_Master
  @{
  */

/** @defgroup SPI_HalfDuplexPolling_Master_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group SPI_HalfDuplexPolling_Master_Macros */

/** @defgroup SPI_HalfDuplexPolling_Master_Variables Variables
  @{
*/

volatile uint32_t g_delayCount = 0;

/* SPI TX Buffer */
static uint16_t g_spiTxBuf[] =
{
    0x300, 0x301, 0x302, 0x303,
    0x304, 0x305, 0x306, 0x307,
    0x308, 0x309, 0x30A, 0x30B,
    0x30C, 0x30D, 0x30E, 0x30F,
    '\0'
};

static GPIO_Config_T g_gpioConfig =
{
    .mode = GPIO_MODE_AF,
    .outtype = GPIO_OUT_TYPE_PP,
    .speed = GPIO_SPEED_50MHz,
    .pupd = GPIO_PUPD_NO
};

/* SystemClock = 32M */
static SPI_Config_T g_spiConfig =
{
    .mode = SPI_MODE_MASTER,             /* Master mode */
    .length = SPI_DATA_LENGTH_11B,       /* 11-bit data length */
    .polarity = SPI_CLKPOL_LOW,          /* Clock polarity is Low */
    .phase = SPI_CLKPHA_1EDGE,           /* Clock Phase is 1EDGE */
    .baudrateDiv = SPI_BAUDRATE_DIV_64,  /* BaudRate divider is 64 */
    .slaveSelect = SPI_SSC_ENABLE,       /* Enable Software slave control */
    .firstBit = SPI_FIRST_BIT_MSB,       /* MSB */
    .direction = SPI_DIRECTION_1LINE_TX  /* One line send dierction */
};

/**@} end of group SPI_HalfDuplexPolling_Master_Variables */

/** @defgroup SPI_HalfDuplexPolling_Master_Functions Functions
  @{
  */

void GPIO_Initialize(void);
void SPI_Initialize(void);
void SPI_PollingSend(void);
void DelayInit(void);
void Delay(uint32_t milliseconds);

/*!
 * @brief Main program
 *
 * @param None
 *
 * @retval None
 */
int main(void)
{
    /* LED initialize */
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDOn(LED2);
    /* Delay initialize */
    DelayInit();

    GPIO_Initialize();
    SPI_Initialize();

    for (;;)
    {
        /* Polling send */
        SPI_PollingSend();
        /* Delay 1 second */
        Delay(1000);
    }
}

/*!
 * @brief SPI send in polling mode
 *
 * @param None
 *
 * @retval None
 */
void SPI_PollingSend(void)
{
    int i = 0;

    GPIO_ClearBit(GPIOA, GPIO_PIN_4);

    for(i = 0; g_spiTxBuf[i] != '\0';)
    {
        if(SPI_ReadStatusFlag(SPI1, SPI_FLAG_TXBE))
        {
            /* Send 11-bit data by SPI1 */
            SPI_I2S_TxData16(SPI1, g_spiTxBuf[i++]);
            /* Wait for transmit complete */
            while(SPI_ReadStatusFlag(SPI1, SPI_FLAG_BUSY));
        }
    }

    GPIO_SetBit(GPIOA, GPIO_PIN_4);
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
    /* Enable clock */
    RCM_EnableAHBPeriphClock(RCM_AHB_PERIPH_GPIOA);
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SYSCFG);

    /* Config alter function */
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_5, GPIO_AF_PIN0);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN0);

    /* config PA5->SCK , PA7->MOSI */
    g_gpioConfig.pin = GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_Config(GPIOA, &g_gpioConfig);

    /* config PA4 to out mode as soft NSS */
    g_gpioConfig.pin = GPIO_PIN_4;
    g_gpioConfig.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOA, &g_gpioConfig);

    /* Keep select the slave */
    GPIO_SetBit(GPIOA, GPIO_PIN_4);
}

/*!
 * @brief SPI Initialize
 *
 * @param None
 *
 * @retval None
 */
void SPI_Initialize(void)
{
    /* Enable clock */
    RCM_EnableAPB2PeriphClock(RCM_APB2_PERIPH_SPI1);

    /* SPI reset */
    SPI_Reset(SPI1);

    /* Set internal slave */
    SPI_EnableInternalSlave(SPI1);
    SPI_Config(SPI1, &g_spiConfig);
    SPI_ConfigFIFOThreshold(SPI1, SPI_RXFIFO_QUARTER);
    SPI_Enable(SPI1);
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
 * @brief Blocking milliseconds delay function
 *
 * @param None
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
    if(g_delayCount > 0)
    {
        g_delayCount--;
    }
}

/**@} end of group SPI_HalfDuplexPolling_Master_Functions */
/**@} end of group SPI_HalfDuplexPolling_Master */
/**@} end of group Examples */
