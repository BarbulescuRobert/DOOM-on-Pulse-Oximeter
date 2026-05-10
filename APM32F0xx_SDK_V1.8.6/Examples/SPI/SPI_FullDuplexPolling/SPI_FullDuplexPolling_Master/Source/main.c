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

/** @addtogroup SPI_FullDuplexPolling_Master
  @{
  */

/** @defgroup SPI_FullDuplexPolling_Master_Macros Macros
  @{
*/

/* printf function configs to USART1 */
#define DEBUG_USART  USART1

/**@} end of group SPI_FullDuplexPolling_Master_Macros */

/** @defgroup SPI_FullDuplexPolling_Master_Variables Variables
  @{
*/

volatile static uint32_t g_delayCount = 0;

/* SPI TX Buffer*/
static const uint16_t g_spiTxBuf[] =
{
    0x101, 0x102, 0x103, 0x104, 0x105, 0x106, 0x107,
    0x108, 0x109, 0x109, 0x10a, 0x10b, 0x10c, 0x10d,
    0x10e, 0x10f, 0x110, 0x111, '\r', '\n', '\0'
};

static GPIO_Config_T g_gpioConfig =
{
    .mode = GPIO_MODE_AF,
    .outtype = GPIO_OUT_TYPE_PP,
    .speed = GPIO_SPEED_50MHz,
    .pupd = GPIO_PUPD_PU
};

/* SystemClock = 32M */
static SPI_Config_T g_spiConfig =
{
    .mode = SPI_MODE_MASTER,             /* Master mode */
    .length = SPI_DATA_LENGTH_9B,        /* 9-bit data length */
    .polarity = SPI_CLKPOL_LOW,          /* Clock polarity is Low */
    .phase = SPI_CLKPHA_1EDGE,           /* Clock Phase is 1EDGE */
    .baudrateDiv = SPI_BAUDRATE_DIV_64,  /* BaudRate divider is 64 */
    .firstBit = SPI_FIRST_BIT_MSB,       /* MSB */
    .slaveSelect = SPI_SSC_ENABLE,       /* Enable Software slave control */
    .direction = SPI_DIRECTION_2LINES_FULLDUPLEX /* Two line full duplex */
};

/**@} end of group SPI_FullDuplexPolling_Master_Variables */

/** @defgroup SPI_FullDuplexPolling_Master_Functions Functions
  @{
  */

void GPIO_Initialize(void);
void SPI_Initialize(void);
void SPI_Polling(void);
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
    /* COM1 initialize */
    APM_MINI_COMInit(COM1);
    /* Delay initialize */
    DelayInit();

    GPIO_Initialize();
    SPI_Initialize();

    for (;;)
    {
        /* Polling send data */
        SPI_Polling();
        APM_MINI_LEDToggle(LED2);
        /* Delay 1 second */
        Delay(1000);
    }
}

void SPI_Polling(void)
{
    int i = 0;

    GPIO_ClearBit(GPIOA, GPIO_PIN_4);

    for(i = 0; g_spiTxBuf[i] != '\0'; )
    {
        if(SPI_ReadStatusFlag(SPI1, SPI_FLAG_TXBE))
        {
            SPI_I2S_TxData16(SPI1, g_spiTxBuf[i++]);
        }

        if(SPI_ReadStatusFlag(SPI1, SPI_FLAG_RXBNE))
        {
            printf("0x%X ", SPI_I2S_RxData16(SPI1));
        }
    }
    printf("\r\n");

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
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_6, GPIO_AF_PIN0);
    GPIO_ConfigPinAF(GPIOA, GPIO_PIN_SOURCE_7, GPIO_AF_PIN0);

    /* config PA5->SCK , PA7->MOSI , PA6->MISO */
    g_gpioConfig.pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Config(GPIOA, &g_gpioConfig);

    /* config PA4 to out mode as NSS */
    g_gpioConfig.pin = GPIO_PIN_4;
    g_gpioConfig.mode = GPIO_MODE_OUT;
    GPIO_Config(GPIOA, &g_gpioConfig);

    /* Pause selection */
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
 * @brief Redirect C Library function printf to serial port.
 *        After Redirection, you can use printf function.
 *
 * @param ch: The characters that need to be send.
 * @param f: pointer to a FILE that can recording all information
 *        needed to control a stream
 *
 * @retval The characters that need to be send.
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
 * @brief Redirect C Library function printf to serial port.
 *        After Redirection, you can use printf function.
 *
 * @param ch: The characters that need to be send.
 *
 * @retval The characters that need to be send.
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
 * @brief Redirect C Library function printf to serial port.
 *        After Redirection, you can use printf function.
 *
 * @param file: Meaningless in this function.
 * @param ptr: Buffer pointer for data to be sent.
 * @param len: Length of data to be sent.
 *
 * @retval The characters that need to be send.
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

/**@} end of group SPI_FullDuplexPolling_Master_Functions */
/**@} end of group SPI_FullDuplexPolling_Master */
/**@} end of group Examples */
