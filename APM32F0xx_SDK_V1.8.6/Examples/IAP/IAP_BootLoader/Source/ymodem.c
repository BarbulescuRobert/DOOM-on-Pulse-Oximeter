/*!
 * @file        ymodem.c
 *
 * @brief       Include Ymodem operation to HyperTerminal
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
#include "ymodem.h"
#include <string.h>

/** @addtogroup Examples
  @{
  */

/** @addtogroup IAP_BootLoader
  @{
  */

/** @addtogroup YMODEM
  @{
  */

/** @defgroup YMODEM_Macros Macros
  @{
  */

/**@} end of group YMODEM_Macros */

/** @defgroup YMODEM_Enumerations Enumerations
  @{
  */


/**@} end of group YMODEM_Enumerations */

/** @defgroup YMODEM_Structures Structures
  @{
  */

/**@} end of group YMODEM_Structures */

/** @defgroup YMODEM_Variables Variables
  @{
  */

/**@} end of group YMODEM_Variables */

/** @defgroup YMODEM_Functions Functions
  @{
  */

/*!
 * @brief       Receive byte(s) from sender
 *
 * @param       character: input character
 *
 * @param       revTimeout: receive timeout
 *
 * @retval      SUCCESS : Byte be received
 *              ERROR: receive timeout
 * @note
 */
static int32_t Receive_Byte(uint8_t* character, uint32_t revTimeout)
{
    while (revTimeout-- > 0)
    {
        if (GetKeyPressed(character) == SUCCESS)
        {
            return SUCCESS;
        }
    }

    return ERROR;
}

/*!
 * @brief       Send a byte to receiver
 *
 * @param       character: character to be sent
 *
 * @retval      reture value
 *              @arg 0: Byte sent
 *
 * @note
 */
static uint32_t Send_Byte(uint8_t character)
{
    Send_Char(character);

    return 0;
}

/*!
 * @brief       Send 2 byte to receiver
 *
 * @param       Character: character to be sent
 *
 * @retval      reture value
 *              @arg 0: Byte sent
 *
 * @note
 */
static uint32_t Send_2Byte(uint16_t Character)
{
    Send_Char(Character >> 0x08);
    Send_Char((uint8_t) Character);

    return 0;
}

/*!
 * @brief       Update CRC16 for input byte
 *
 * @param       value: CRC input value
 *
 * @param       b: input byte
 *
 * @retval      returnTemp: Updated CRC value
 *
 * @note
 */
uint16_t CRC16_Update(uint16_t value, uint8_t b)
{
    uint32_t value_crc = value;
    uint32_t value_int = b | 0x100;
    uint16_t returnTemp;

    do
    {
        value_crc <<= 1;
        value_int <<= 1;

        if (value_int & 0x100)
        {
            ++ value_crc;
        }

        if (value_crc & 0x10000)
        {
            value_crc ^= 0x1021;
        }
    }
    while (!(value_int & 0x10000));

    returnTemp = (uint16_t)(value_crc & 0xffff);

    return returnTemp;
}

/*!
 * @brief       Cal CRC16
 *
 * @param       data
 *
 * @param       len
 *
 * @retval      CRC16 sum
 *
 * @note
 */
uint16_t CRC16_Sum(const uint8_t* data, uint32_t len)
{
    uint32_t crc_sum = 0;
    const uint8_t* data_len = data + len;

    do
    {
        crc_sum = CRC16_Update(crc_sum, *data++);
    }
    while (data < data_len);

    crc_sum = CRC16_Update(CRC16_Update(crc_sum, 0), 0);

    crc_sum &= (uint16_t)0xffff;

    return crc_sum;
}

/*!
 * @brief       Cal Check sum for YModem Packet
 *
 * @param       data
 *
 * @param       len
 *
 * @retval      checksum
 *
 * @note
 */
uint8_t Check_sum(const uint8_t* data, uint32_t len)
{
    uint32_t checksum = 0;
    const uint8_t* data_len = data + len;

    do
    {
        checksum += *data++;
    }
    while (data < data_len);

    checksum &= (uint16_t)0xff;

    return checksum;
}

/*!
 * @brief       Receive a Ymodem packet from sender
 *
 * @param       data
 *
 * @param       len
 *
 * @param       timeout
 *              @arg 0 : end
 *              @arg -1: abort
 *              @arg >0: pack length
 *
 * @retval      receive status
 *              @arg 0 : normally return
 *              @arg -1: timeout or pack error
 *              @arg 1 : abort
 *
 * @note
 */
static int32_t Receive_YmodePacket(uint8_t* data, int32_t* len, uint32_t timeout)
{
    uint16_t i, pack_len, pack_crc;
    uint8_t character;
    *len = 0;

    if (Receive_Byte(&character, timeout) != SUCCESS)
    {
        return -1;
    }

    if (character == FRAME_SOH)
    {
        pack_len = PACKET_SIZE;
    }
    else if (character == FRAME_STX)
    {
        pack_len = PACKET_1K_SIZE;
    }
    else if (character == FRAME_EOT)
    {
        return 0;
    }
    else if (character == FRAME_CAN)
    {
        if ((Receive_Byte(&character, timeout) == SUCCESS) && (character == FRAME_CAN))
        {
            *len = -1;
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else if ((character == ABORT1) | (character == ABORT2))
    {
        return 1;
    }
    else
    {
        return -1;
    }

    *data = character;

    for (i = 1; i < (pack_len + PACK_FRAME_OVERHEAD); i ++)
    {
        if (Receive_Byte(data + i, timeout) != SUCCESS)
        {
            return -1;
        }
    }

    if (data[PACK_FRAMENO_INDEX] != ((data[PACK_FRAMENO_COMP_INDEX] ^ 0xff) & 0xff))
    {
        return -1;
    }

    /* Calculate the CRC16 */
    pack_crc = CRC16_Sum(&data[PACK_FRAME_HEADER], (uint32_t)pack_len);

    if (pack_crc != (uint16_t)((data[pack_len + 3] << 8) | data[pack_len + 4]))
    {
        /* CRC16 error */
        return -1;
    }

    *len = pack_len;

    return 0;
}

/*!
 * @brief       Receive a file from sender using the Ymodem
 *
 * @param       buffer: file buffer
 *
 * @param       Application : APP1 or APP2
 *
 * @retval      The lenght of the file
 *
 * @note
 */
int32_t recceiveFile(uint8_t* buffer, uint8_t Application)
{
    uint8_t pack_data[PACK_1K_FRAME_OVERHEAD_SIZE];
    uint8_t file_len[FILE_SIZE_MAX];
    uint8_t* file_p;
    uint8_t* buf_p;

    int32_t i, pack_len;
    int32_t session_finish;
    int32_t file_finish;
    int32_t packs_received;
    int32_t err;
    int32_t session_begin;
    int32_t pack_ref;
    int32_t len = 0;
    uint32_t flashaddress;
    uint32_t code;

    /* Initialize flashaddress variable */
    if (Application == APP1)
    {
        flashaddress = USER_APP1_START_ADDRESS;
    }
    else
    {
        flashaddress = USER_APP2_START_ADDRESS;
    }

    for (session_finish = 0, err = 0, session_begin = 0; ;)
    {
        for (packs_received = 0, file_finish = 0, buf_p = buffer; ;)
        {
            pack_ref = Receive_YmodePacket(pack_data, &pack_len, NAK_TIMEOUT);

            if (pack_ref == 0x01)
            {
                Send_Byte(FRAME_CAN);
                Send_Byte(FRAME_CAN);

                return -3;
            }
            else if (pack_ref == 0)
            {
                err = 0;

                if (pack_len == -1)
                {
                    Send_Byte(FRAME_ACK);
                    return 0;
                }
                else if (pack_len == 0)
                {
                    Send_Byte(FRAME_ACK);
                    file_finish = 1;
                }
                else
                {
                    if ((pack_data[PACK_FRAMENO_INDEX] & 0xff) != (packs_received & 0xff))
                    {
                        Send_Byte(FRAME_NACK);
                    }
                    else
                    {
                        if (packs_received == 0)
                        {
                            /* Ymodem Filename Pack */
                            if (pack_data[PACK_FRAME_HEADER] != 0)
                            {
                                /* Filename pack has valid data */
                                for (i = 0, file_p = pack_data + PACK_FRAME_HEADER; (*file_p != 0) && (i < FILE_NAME_MAX);)
                                {
                                    FileN[i++] = *file_p++;
                                }

                                FileN[i++] = '\0';

                                for (i = 0, file_p ++; (*file_p != ' ') && (i < (FILE_SIZE_MAX - 1));)
                                {
                                    file_len[i++] = *file_p++;
                                }

                                file_len[i++] = '\0';
                                StrConInt(file_len, &len);

                                /* File len is greater than Flash len */
                                if (len > (USER_FLASH_SIZE + 1))
                                {
                                    /** End section */
                                    Send_Byte(FRAME_CAN);
                                    Send_Byte(FRAME_CAN);

                                    return -1;
                                }

                                /* erase user application area */
                                FLASH_IAP_Erase((APP_TypeDef)Application);
                                Send_Byte(FRAME_ACK);
                                Send_Byte(FRAME_CRC16);
                            }
                            /* Filename pack is empty, end section */
                            else
                            {
                                Send_Byte(FRAME_ACK);
                                file_finish = 1;
                                session_finish = 1;
                                break;
                            }
                        }
                        /* Ymodem Data Pack */
                        else
                        {
                            memcpy(buf_p, pack_data + PACK_FRAME_HEADER, pack_len);
                            code = (uint32_t)buffer;

                            /* Write data in Flash */
                            if (FLASH_IAP_Write(&flashaddress, (uint32_t*) code, (uint16_t) pack_len / 4, (APP_TypeDef)Application)  == SUCCESS)
                            {
                                Send_Byte(FRAME_ACK);
                            }
                            else
                            {
                                /* End section */
                                Send_Byte(FRAME_CAN);
                                Send_Byte(FRAME_CAN);

                                return -2;
                            }
                        }
                        packs_received++;
                        session_begin = 1;
                    }
                }
            }
            else
            {
                if (session_begin > 0)
                {
                    err ++;
                }

                if (err > ERRORS)
                {
                    Send_Byte(FRAME_CAN);
                    Send_Byte(FRAME_CAN);

                    return 0;
                }
                Send_Byte(FRAME_CRC16);
                break;
            }

            if (file_finish != 0)
            {
                break;
            }
        }

        if (session_finish != 0)
        {
            break;
        }
    }

    return (int32_t)len;
}

/**!
 * @brief       Prepare the Ymodem first packet
 *
 * @param       data: packet data
 *
 * @param       fileName: file name
 *
 * @param       length: file lenght
 *
 * @retval      None
 *
 * @note
 */
void PreFirstPack(uint8_t* data, const uint8_t* fileName, uint32_t* length)
{
    uint16_t i, j;
    uint8_t file_p[10];

    /* Make frame three pack */
    data[0] = FRAME_SOH;
    data[1] = 0x00;
    data[2] = 0xff;

    i = 0;

    do
    {
        data[i + PACK_FRAME_HEADER] = fileName[i];
        i++;
    }
    while ((fileName[i] != '\0') && (i < FILE_NAME_MAX));

    data[i + PACK_FRAME_HEADER] = 0x00;

    IntConStr(file_p, *length);

    i = i + PACK_FRAME_HEADER + 1;
    j = 0;

    do
    {
        data[i++] = file_p[j++];
    }
    while (file_p[j] != '\0');

    j = i;

    do
    {
        data[j] = 0;
        j++;
    }
    while (j < PACKET_SIZE + PACK_FRAME_HEADER);
}

/*!
 * @brief       Prepare the Ymodem data pack
 *
 * @param       sourceBuf:source buffer
 *
 * @param       data:packet data
 *
 * @param       packNo:packet block number
 *
 * @param       lenBlk: packet block lenght
 *
 * @retval      None
 *
 * @note
 */
void PrePack(uint8_t* sourceBuf, uint8_t* data, uint8_t packNo, uint32_t lenBlk)
{
    uint16_t i, len, packSize;
    uint8_t* file_p;

    /* Make frame three pack */
    if (lenBlk < PACKET_1K_SIZE)
    {
        packSize = PACKET_SIZE;
    }
    else
    {
        packSize = PACKET_1K_SIZE;
    }

    if (lenBlk >= packSize)
    {
        len = packSize;
    }
    else
    {
        len = lenBlk;
    }

    if (packSize != PACKET_1K_SIZE)
    {

        data[0] = FRAME_SOH;
    }
    else
    {
        data[0] = FRAME_STX;
    }

    data[1] = packNo;
    data[2] = (~packNo);
    file_p = sourceBuf;

    for (i = PACK_FRAME_HEADER; i < len + PACK_FRAME_HEADER; i++)
    {
        data[i] = *file_p++;
    }

    if (len  <= packSize)
    {
        for (i = len + PACK_FRAME_HEADER; i < packSize + PACK_FRAME_HEADER; i++)
        {
            data[i] = FILE_FILL_VAL;
        }
    }
}

/*!
 * @brief       Transmit a data pack using the ymodem protocol
 *
 * @param       data: packet data
 *
 * @param       length : The length of packet data
 *
 * @retval      None
 *
 * @note
 */
void Send_Packet(uint8_t* data, uint16_t length)
{
    for (uint16_t i = 0; i < length; i++)
    {
        Send_Byte(data[i]);
    }
}

/*!
 * @brief       Transmit a file using the Ymodem
 *
 * @param       buf: File address of the first byte
 *
 * @param       name: File name
 *
 * @param       lenFile: The lenght of the file
 *
 * @retval      The lenght of the file
 *
 * @note
 */
uint8_t transmitFile(uint8_t* buf, const uint8_t* name, uint32_t lenFile)
{
    uint8_t pack_data[PACKET_1K_SIZE + PACK_FRAME_OVERHEAD];
    uint8_t FileN[FILE_NAME_MAX];
    uint8_t* buf_p;
    uint8_t checkSumTemp;
    uint16_t crcTemp;
    uint16_t blockNum;
    uint8_t revC[2];
    uint8_t crc16F = 0;
    uint8_t i = 0;
    uint32_t err;
    uint32_t len = 0;
    uint32_t packSize;

    while (i < (FILE_NAME_MAX - 1))
    {
        FileN[i] = name[i];
        i++;
    }

    crc16F = 1;

    /* Prepare the first packet */
    PreFirstPack(&pack_data[0], FileN, &lenFile);

    for (err = 0; err < ERRORS;)
    {
        /* Send Packet to receiver*/
        Send_Packet(pack_data, PACKET_SIZE + PACK_FRAME_HEADER);

        /* Send CRC or checksum based on crc16F */
        if (crc16F)
        {
            crcTemp = CRC16_Sum(&pack_data[3], PACKET_SIZE);
            Send_2Byte(crcTemp);
        }
        else
        {
            checkSumTemp = Check_sum(&pack_data[3], PACKET_SIZE);
            Send_Byte(checkSumTemp);
        }

        /* Wait for Ack of 'C' */
        if (Receive_Byte(&revC[0], 1000000) == SUCCESS)
        {
            if (revC[0] == FRAME_ACK)
            {
                /* correctly */
                break;
            }
        }
        else
        {
            err++;
        }
    }

    if (err >=  ERRORS)
    {
        return err;
    }

    buf_p = buf;
    len = lenFile;
    blockNum = 0x01;

    /* 1024 bytes package */
    while (len)
    {
        /* Prepare pack */
        PrePack(buf_p, &pack_data[0], blockNum, len);
        revC[0] = 0;
        err = 0;

        for (err = 0; err < ERRORS;)
        {
            /* Send pack */
            if (len >= PACKET_1K_SIZE)
            {
                packSize = PACKET_1K_SIZE;

            }
            else
            {
                packSize = PACKET_SIZE;
            }

            Send_Packet(pack_data, packSize + PACK_FRAME_HEADER);

            /* Send CRC or checksum based on crc16F */
            if (crc16F)
            {
                crcTemp = CRC16_Sum(&pack_data[3], packSize);
                Send_2Byte(crcTemp);
            }
            else
            {
                checkSumTemp = Check_sum(&pack_data[3], packSize);
                Send_Byte(checkSumTemp);
            }

            /* Wait for Ack */
            if (Receive_Byte(&revC[0], 1000000) == SUCCESS)
            {
                if (revC[0] == FRAME_ACK)
                {
                    if (len > packSize)
                    {
                        buf_p += packSize;
                        len -= packSize;

                        if (blockNum == (USER_FLASH_SIZE / 1024))
                        {
                            /* error */
                            return 0xFF;
                        }
                        else
                        {
                            blockNum++;
                        }
                    }
                    else
                    {
                        buf_p += packSize;
                        len = 0;
                    }

                    break;
                }
            }
            else
            {
                err++;
            }
        }

        /* Resend pack */
        if (err >=  ERRORS)
        {
            return err;
        }

    }

    revC[0] = 0x00;
    revC[1] = 0x00;

    for (err = 0; err < ERRORS;)
    {
        /* Send EOT and wait for ack*/
        Send_Byte(FRAME_EOT);
        revC[0] = USART_COM_RXDATA;

        if (revC[0] == FRAME_ACK)
        {
            USART_COM_CLEAR_OVREE;
            break;
        }

        else
        {
            err++;
        }

        USART_COM_CLEAR_OVREE;
    }

    if (err >=  ERRORS)
    {
        return err;
    }

    /* Last pack preparation */
    revC[0] = 0x00;
    revC[1] = 0x00;

    pack_data[0] = FRAME_SOH;
    pack_data[1] = 0;
    pack_data[2] = 0xFF;

    for (i = PACK_FRAME_HEADER; i < (PACKET_SIZE + PACK_FRAME_HEADER); i++)
    {
        pack_data[i] = 0x00;
    }

    for (err = 0; err < ERRORS;)
    {
        /* Send pack */
        Send_Packet(pack_data, PACKET_SIZE + PACK_FRAME_HEADER);

        /* Send CRC or checksum based on crc16_F */
        crcTemp = CRC16_Sum(&pack_data[3], PACKET_SIZE);
        Send_2Byte(crcTemp);

        /* Wait for ack of 'C' */
        if (Receive_Byte(&revC[1], 1000000) == SUCCESS)
        {
            if (revC[1] == FRAME_ACK)
            {
                /* transfered correctly */
                break;
            }
        }
        else
        {
            err++;
        }
    }

    /* Resend pack */
    if (err >=  ERRORS)
    {
        return err;
    }

    revC[0] = 0x00;

    for (err = 0; err < ERRORS;)
    {
        /* Send EOT and wait for ack*/
        Send_Byte(FRAME_EOT);
        revC[0] = USART_COM_RXDATA;

        if (revC[0] == FRAME_ACK)
        {
            break;
        }
        else
        {
            err++;
        }

        USART_COM_CLEAR_OVREE;
    }

    if (err >=  ERRORS)
    {
        return err;
    }

    /* file trasmitted finish */
    return 0;
}

/**@} end of group YMODEM_Functions */
/**@} end of group YMODEM */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
