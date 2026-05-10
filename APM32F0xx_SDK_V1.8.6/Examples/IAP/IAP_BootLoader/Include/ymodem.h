/*!
 * @file        ymodem.h
 *
 * @brief       Header for ymodem.c module
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

/* Define to prevent recursive inclusion */
#ifndef _YMODEM_H_
#define _YMODEM_H_

/* Includes */
#include "main.h"

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
#define PACK_FRAMENO_INDEX              (1)
#define PACK_FRAMENO_COMP_INDEX         (2)

#define PACK_FRAME_HEADER               (3)
#define PACK_FRAME_TRAILER              (2)
#define PACK_FRAME_OVERHEAD             (PACK_FRAME_HEADER + PACK_FRAME_TRAILER)
#define PACKET_SIZE                     (128)
#define PACKET_1K_SIZE                  (1024)
#define PACK_1K_FRAME_OVERHEAD_SIZE     (PACKET_1K_SIZE + PACK_FRAME_OVERHEAD)

#define FILE_NAME_MAX                   (256)
#define FILE_SIZE_MAX                   (16)

#define FILE_FILL_VAL                   (0x1A)
/* start of 128-byte data packet */
#define FRAME_SOH                       (0x01)
/* start of 1024-byte data packet */
#define FRAME_STX                       (0x02)
/* end of transmission */
#define FRAME_EOT                       (0x04)
/* acknowledge */
#define FRAME_ACK                       (0x06)
/* negative acknowledge */
#define FRAME_NACK                      (0x15)
/* cancel transmission */
#define FRAME_CAN                       (0x18)
/* 'C' == 0x43, request 16-bit CRC */
#define FRAME_CRC16                     (0x43)
/* 'A' == 0x41, abort by user */
#define ABORT1                          (0x41)
/* 'a' == 0x61, abort by user */
#define ABORT2                          (0x61)

#define NAK_TIMEOUT                     (0x200000)
#define ERRORS                          (10)

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

/* function declaration*/
int32_t recceiveFile(uint8_t* buffer, uint8_t Application);
uint8_t transmitFile(uint8_t* buf, const uint8_t* name, uint32_t lenFile);
uint16_t CRC16_Update(uint16_t value, uint8_t b);
uint16_t CRC16_Sum(const uint8_t* data, uint32_t len);
uint8_t Check_sum(const uint8_t* data, uint32_t len);
void PreFirstPack(uint8_t* data, const uint8_t* fileName, uint32_t* length);
void PrePack(uint8_t* sourceBuf, uint8_t* data, uint8_t packNo, uint32_t lenBlk);
void Send_Packet(uint8_t* data, uint16_t length);

/**@} end of group YMODEM_Functions */
/**@} end of group YMODEM */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
#endif
