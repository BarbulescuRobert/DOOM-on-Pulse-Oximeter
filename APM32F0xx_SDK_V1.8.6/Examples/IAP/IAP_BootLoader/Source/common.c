/*!
 * @file        common.c
 *
 * @brief       Include data convert and read and write operation to HyperTerminal
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
#include "common.h"

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @addtogroup COMMON
  @{
  */

/** @defgroup COMMON_Macros Macros
  @{
  */

/**@} end of group COMMON_Macros */

/** @defgroup COMMON_Enumerations Enumerations
  @{
  */


/**@} end of group COMMON_Enumerations */

/** @defgroup COMMON_Structures Structures
  @{
  */

/**@} end of group COMMON_Structures */

/** @defgroup COMMON_Variables Variables
  @{
  */

/**@} end of group COMMON_Variables */

/** @defgroup COMMON_Functions Functions
  @{
  */

/*!
 * @brief       Integer number convert to string
 *
 * @param       str: string buffer
 *
 * @param       conint: specific interger number
 *
 * @retval      None
 *
 * @note
 */
void IntConStr(uint8_t* str, int32_t conint)
{
    uint32_t i;
    uint32_t j;
    uint32_t div;
    uint32_t convStatus;

    j = 0;
    i = 0;
    div  = 1000000000;
    convStatus = 0;

    do
    {
        str[j++] = (conint / div) + 48;

        conint = conint % div;
        div /= 10;

        if ((str[j - 1] == '0') & (convStatus == 0))
        {
            j = 0;
        }
        else
        {
            convStatus++;
        }

        i++;
    }
    while (i < 10);
}

/*!
 * @brief       string convert to an integer number
 *
 * @param       hexstr: hex string type data
 *
 * @param       conint: specific interger number
 *
 * @retval      SUCCESS
 *              ERROR
 * @note
 */
uint32_t Str_Hex_ConInt(uint8_t* hexstr, int32_t* conint)
{
    uint32_t i = 0;
    uint32_t res = ERROR;
    uint32_t val = 0;

    if ((hexstr[2] != '\0'))
    {
        i = 2;

        do
        {
            if (hexstr[i] == '\0')
            {
                *conint = val;
                res = SUCCESS;
                break;
            }

            if (CHECK_HEX(hexstr[i]))
            {
                val = (val << 4) + CON_HEX(hexstr[i]);
            }
            else
            {
                res = ERROR;
                break;
            }

            i++;
        }
        while (i < 11);

        /* over 8 digit hex */
        if (i >= 11)
        {
            res = ERROR;
        }

    }
    else
    {
        return ERROR;
    }

    return res;
}

/*!
 * @brief       decimal string convert to an integer
 *
 * @param       decstr: dec string buffer
 *
 * @param       conint: specific interger number
 *
 * @retval      SUCCESS
 *              ERROR
 * @note
 */
uint32_t Str_Dec_ConInt(uint8_t* decstr, int32_t* conint)
{
    uint32_t i = 0;
    uint32_t res = ERROR;
    uint32_t val = 0;

    i = 0;
    do
    {
        if (decstr[i] == '\0')
        {
            *conint = val;
            res = SUCCESS;
            break;
        }
        else if ((decstr[i] == 'k' || decstr[i] == 'K') && (i > 0))
        {
            val = val << 10;
            *conint = val;
            res = SUCCESS;
            break;
        }
        else if ((decstr[i] == 'm' || decstr[i] == 'M') && (i > 0))
        {
            val = val << 20;
            *conint = val;
            res = SUCCESS;
            break;
        }
        else if (CHECK_DEC(decstr[i]))
        {
            val = val * 10 + CON_DEC(decstr[i]);
        }
        else
        {
            res = ERROR;
            break;
        }

        i++;
    }
    while (i < 11);

    /* Over 10 digit decimal */
    if (i >= 11)
    {
        res = ERROR;
    }

    return res;
}

/*!
 * @brief       string type convert to integer type
 *
 * @param       str: string buffer
 *
 * @param       conint: specific interger number
 *
 * @retval      SUCCESS
 *              ERROR
 * @note
 */
uint32_t StrConInt(uint8_t* str, int32_t* conint)
{
    uint32_t res = ERROR;

    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
    {
        res = Str_Hex_ConInt(str, conint);
    }
    else
    {
        res = Str_Dec_ConInt(str, conint);
    }

    return res;
}

/*!
 * @brief       Get a key from the HyperTerminal
 *
 * @param       inputKey: User input key pressed
 *
 * @retval      SUCCESS
 *              ERROR
 * @note
 */
uint32_t GetKeyPressed(uint8_t* inputKey)
{
    if (USART_COM_RXREADY)
    {
        *inputKey = USART_COM_RXDATA;
        return SUCCESS;
    }
    else
    {
        return ERROR;
    }
}

/*!
 * @brief       Read a key from the HyperTerminal
 *
 * @param       None
 *
 * @retval      The input key pressed
 *
 * @note
 */
uint8_t ReadKey(void)
{
    uint8_t inputKey = 0;

    /* Waiting for user input */
    while (1)
    {
        if (GetKeyPressed((uint8_t*) & inputKey) == SUCCESS)
        {
            break;
        }
    }

    return inputKey;

}

/*!
 * @brief       Read a key from the HyperTerminal
 *
 * @param       delay_timeOut£ºread input key timeout
 *
 * @retval      The input key pressed
 *
 * @note
 */
uint8_t ReadKey_TimeOut(uint32_t delay_timeOut)
{
    uint8_t inputKey = 0xFF;

    /* Waiting for user input */
    while (1)
    {
        if ((GetKeyPressed((uint8_t*) & inputKey) == SUCCESS) | (delay_timeOut < 10))
        {
            break;
        }

        delay_timeOut--;
    }

    return inputKey;
}
/*!
 * @brief       Send a character on the HyperTerminal
 *
 * @param       character: The character need to be sent
 *
 * @retval      None
 *
 * @note
 */
void Send_Char(uint8_t character)
{
    USART_COM_TXDATA(character);

    while (USART_COM_TXFINISH)
    {
    }
}

/*!
 * @brief       Send a string on the HyperTerminal
 *
 * @param       str: The string need to be sent
 *
 * @retval      None
 *
 * @note
 */
void Serial_SendString(uint8_t* str)
{
    while (*str != '\0')
    {
        Send_Char(*str);
        str++;
    }
}

/**@} end of group COMMON_Functions */
/**@} end of group COMMON */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
