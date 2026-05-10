/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.0
 *
 * @date        2023-02-28
 *
 * @attention
 *
 *  Copyright (C) 2022-2023 Geehy Semiconductor
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
#include "flash_read_write.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup FMC_Read_Write
  @{
*/

/** @defgroup FMC_Read_Write_Macros Macros
  @{
*/

/* printf function configure to USART1 */
#define DEBUG_USART                   USART1

/* Flash read write test address */
#define FLASH_READ_WRITE_TEST_ADDR    (FLASH_READ_WRITE_START_ADDR + APM32_FLASH_PAGE_SIZE)

/* test buffer size */
#define BUFFER_SIZE                   64

/**@} end of group FMC_Read_Write_Macros */

/** @defgroup FMC_Read_Write_Variables Variables
  @{
*/

/* test write buffer */
uint8_t Write_Buffer[BUFFER_SIZE] = {0};

/* test read buffer */
uint8_t Read_Buffer[BUFFER_SIZE] = {0};

/**@} end of group FMC_Read_Write_Variables */

/** @defgroup FMC_Read_Write_Functions Functions
  @{
*/

void InitBuffer(void);
void Print_Buffer(uint8_t* pBuffer, uint32_t BufferLength);
BOOL BufferCompare(uint8_t* buf1, uint8_t* buf2, uint32_t size);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    uint32_t addr = 0;
    
    /* Init USART */
    APM_MINI_COMInit(COM1);

    /* Init LED */
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);
    APM_MINI_LEDOff(LED2);
    APM_MINI_LEDOff(LED3);

    /* Page aligned write operation */
    addr = FLASH_READ_WRITE_TEST_ADDR;
    InitBuffer();
    FMC_Unlock();
    if(FMC_GetReadProtectionStatus())
    {
        APM_MINI_LEDOn(LED3);
    }
    else 
    {
        FMC_ConfigReadOutProtection(FMC_RDP_LEVEL_1);
    }
    Flash_Write(addr, Write_Buffer, BUFFER_SIZE);
    Flash_Read(addr, Read_Buffer, BUFFER_SIZE);
    if (BufferCompare(Write_Buffer, Read_Buffer, BUFFER_SIZE) == TRUE)
    {
        APM_MINI_LEDOn(LED2);
        printf("\r\nPage aligned write operation Successful!\r\n");
    }
    else
    {
        APM_MINI_LEDOff(LED2);
        printf("\r\nPage aligned write operation fail!\r\n");
    }

    /* Page non-aligned write operation */
    addr = FLASH_READ_WRITE_TEST_ADDR + 16;
    InitBuffer();
    Flash_Write(addr, Write_Buffer, BUFFER_SIZE);
    Flash_Read(addr, Read_Buffer, BUFFER_SIZE);
    if (BufferCompare(Write_Buffer, Read_Buffer, BUFFER_SIZE) == TRUE)
    {
        APM_MINI_LEDOn(LED2);
        printf("Page non-aligned write operation Successful!\r\n");
    }
    else
    {
        APM_MINI_LEDOff(LED2);
        printf("Page non-aligned write operation fail!\r\n");
    }

    /* Cross page write operation */
    addr = FLASH_READ_WRITE_TEST_ADDR - 8;
    InitBuffer();
    Flash_Write(addr, Write_Buffer, BUFFER_SIZE);
    Flash_Read(addr, Read_Buffer, BUFFER_SIZE);
    if (BufferCompare(Write_Buffer, Read_Buffer, BUFFER_SIZE) == TRUE)
    {
        APM_MINI_LEDOn(LED2);
        printf("Cross page write operation Successful!\r\n");
    }
    else
    {
        APM_MINI_LEDOff(LED2);
        printf("Cross page write operation fail!\r\n");
    }

    while (1)
    {
    }
}

/*!
 * @brief       Initialize Flash read write test buffers
 *
 * @param       None
 *
 * @retval      None
 */
void InitBuffer(void)
{
    for(uint32_t idx = 0U; idx < BUFFER_SIZE; idx++)
    {
        Write_Buffer[idx] = (uint8_t)idx;
        Read_Buffer[idx] = 0U;
    }
}

/*!
 * @brief       Print Buffer Data
 *
 * @param       pBuffer: buffer
 *
 * @param       length : length of the Buffer
 *
 * @retval      None
 */
void Print_Buffer(uint8_t* pBuffer, uint32_t BufferLength)
{
    uint32_t i = 0;

    for (i = 0; i  < BufferLength; i++)
    {
        printf("0x%02x ", pBuffer[i]);

        if ((i + 1) % 8 == 0)
        {
            printf("\r\n");
        }
    }
}

/*!
 * @brief       Compares two buffers
 *
 * @param       buf1:    First buffer to be compared
 *
 * @param       buf1:    Second buffer to be compared
 *
 * @param       size:    Buffer size
 *
 * @retval      Return TRUE if buf1 = buf2. If not then return FALSE
 */
BOOL BufferCompare(uint8_t* buf1, uint8_t* buf2, uint32_t size)
{
    uint32_t i = 0;

    for (i = 0; i < size; i++)
    {
        if (buf1[i] != buf2[i])
        {
            return FALSE;
        }
    }

    return TRUE;
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

    /* wait for the data to be send */
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

    /* wait for the data to be send */
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

/**@} end of group FMC_Read_Write_Functions */
/**@} end of group FMC_Read_Write */
/**@} end of group Examples */
