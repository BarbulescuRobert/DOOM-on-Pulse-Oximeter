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
#include "flash_eeprom.h"

/** @addtogroup Examples
  @{
*/

/** @addtogroup FMC_Flash_EEPROM
  @{
*/

/** @defgroup FMC_Flash_EEPROM_Macros Macros
  @{
*/

/* printf function configure to USART1 */
#define DEBUG_USART                USART1

/* Read/Write buffer size, The value must be less than the NUMBER_OF_VARIABLES macro */
#define BUFFER_SIZE                16

/**@} end of group FMC_Flash_EEPROM_Macros */

/** @defgroup FMC_Flash_EEPROM_Variables Variables
  @{
*/

/* read and write buffer */
uint32_t Write_Buffer[BUFFER_SIZE] = {0};
uint32_t Read_Buffer[BUFFER_SIZE] = {0};

/**@} end of group FMC_Flash_EEPROM_Variables */

/** @defgroup FMC_Flash_EEPROM_Functions Functions
  @{
*/

void Print_Buffer(uint32_t *pBuffer, uint32_t BufferLength);
BOOL BufferCompare(uint32_t *Buff1, uint32_t *Buff2, uint32_t Size);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 */
int main(void)
{
    uint32_t i, VirtAddress;


    APM_MINI_COMInit(COM1);

    /* Init LED2 */
    APM_MINI_LEDInit(LED2);

    /* fill Write_Buffer */
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        Write_Buffer[i] = i + 0x100;
    }

    /* Print write data */
    printf("\r\nwrite data:\r\n");
    Print_Buffer(Write_Buffer, BUFFER_SIZE);

    /* Flash EEPROM Init */
    Flash_EE_Init();

    /* Write the variable data and corresponding virtual address to Flash EEPROM*/
    /* Virtual address value must be between 0 ~ NUMBER_OF_VARIABLES */
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        VirtAddress = i;
        Flash_EE_WriteData(VirtAddress, Write_Buffer[i]);
    }

    /* Read the variable data corresponding to the virtual address */
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        VirtAddress = i;
        Flash_EE_ReadData(VirtAddress, &Read_Buffer[i]);
    }

    /* Print read data */
    printf("\r\nread data:\r\n");
    Print_Buffer(Read_Buffer, BUFFER_SIZE);

    /* Compare the values of two buffers for equality */
    if(BufferCompare(Write_Buffer, Read_Buffer, BUFFER_SIZE) == TRUE)
    {
        APM_MINI_LEDOn(LED2);
        printf("\r\nFlash EEPROM test successful!\r\n");
    }
    else
    {
        APM_MINI_LEDOff(LED2);
        printf("\r\nFlash EEPROM test fail!\r\n");
    }

    while (1)
    {
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
void Print_Buffer(uint32_t *pBuffer, uint32_t BufferLength)
{
    uint32_t i = 0;

    for (i = 0; i  < BufferLength; i++)
    {
        printf("0x%08x ", pBuffer[i]);

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
BOOL BufferCompare(uint32_t *Buff1, uint32_t *Buff2, uint32_t Size)
{
    uint32_t i = 0;

    for (i = 0; i < Size; i++)
    {
        if (Buff1[i] != Buff2[i])
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

/**@} end of group FMC_Flash_EEPROM_Functions */
/**@} end of group FMC_Flash_EEPROM */
/**@} end of group Examples */
