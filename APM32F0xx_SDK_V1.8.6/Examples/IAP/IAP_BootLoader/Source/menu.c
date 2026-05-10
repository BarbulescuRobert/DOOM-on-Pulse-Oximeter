/*!
 * @file        menu.c
 *
 * @brief       Display a usart menu to user and include IAP download and upload and area jump operation
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

/** @addtogroup MENU
  @{
  */

/** @defgroup MENU_Macros Macros
  @{
  */

/**@} end of group MENU_Macros */

/** @defgroup MENU_Enumerations Enumerations
  @{
  */


/**@} end of group MENU_Enumerations */

/** @defgroup MENU_Structures Structures
  @{
  */

/**@} end of group MENU_Structures */

/** @defgroup MENU_Variables Variables
  @{
  */

uint8_t filek[1024] = {0};
uint8_t FileN[FILE_NAME_MAX];

uint32_t FlashProtection = 0;

/**@} end of group MENU_Variables */

/** @defgroup MENU_Functions Functions
  @{
  */

/*!
 * @brief       Download a file via serial port
 *
 * @param       Application : APP1 or APP2
 *
 * @retval      None
 *
 * @note
 */
void Download(APP_TypeDef Application)
{
    uint8_t Number[10] = {0}, i = 0;
    uint32_t timeout = 0xFFFF;
    int32_t Size = 0;

    SendString(">> Waiting for the file to be sent ... (press 'a' to abort)\r\n");
    Size = recceiveFile(&filek[0], Application);
    while (timeout-- > 0);

    switch (Size)
    {
        case -3:
            SendString(">> Operation aborted!.\r\n");
            break;

        case -2:
            SendString(">> Verification failed!\r\n");
            break;

        case -1:
            SendString(">> The file is too large!\r\n");
            break;

        default:

            SendString(">> Programming Completed Successfully!\r\n");

            SendString("***********");

            for (i = 0; (i < 255) && (FileN[i] != 0x00); i++)
            {
                SendString("*");
            }

            SendString("\n\r   Name: ");
            SendString(FileN);
            IntConStr(Number, Size);
            SendString("\n\r   Size: ");
            SendString(Number);
            SendString(" Bytes\r\n");

            SendString("***********");

            for (i = 0; (i < 255) && (FileN[i] != 0x00); i++)
            {
                SendString("*");
            }

            break;
    }
}

/*!
 * @brief       Upload a file via serial port.
 *
 * @param       Application : APP1 or APP2
 *
 * @retval      None
 *
 * @note
 */
void Upload(APP_TypeDef Application)
{
    uint8_t Status;
    uint32_t timeout = 0xFFFF;

    SendString(">> Select Receive File\n\r");

    if (ReadKey() == FRAME_CRC16)
    {

        if (Application == APP1)
        {
            /* Transmit the flash image through ymodem protocol */
            Status = transmitFile((uint8_t*)USER_APP1_START_ADDRESS, \
                                  (const uint8_t*)"Up_apm32f0xx_App1_FlashImage.bin", USER_APP1_FLASH_SIZE);
        }
        else
        {
            /* Transmit the flash image through ymodem protocol */
            Status = transmitFile((uint8_t*)USER_APP2_START_ADDRESS, \
                                  (const uint8_t*)"Up_apm32f0xx_App2_FlashImage.bin", USER_APP2_FLASH_SIZE);
        }

        while (timeout-- > 0);

        if (Status == 0)
        {
            SendString(">> File uploaded successfully \n\r");
        }
        else
        {
            SendString(">> Error Occurred while Transmitting File\n\r");
        }
    }
}

/*!
 * @brief       Display the Main Menu on HyperTerminal
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Select_Menu(void)
{
    uint8_t key = 0;

    SendString("\r\n***************************************************************");
    SendString("\r\n*      apm32f0xx In-Application Programming Application       *");
    SendString("\r\n*                                              (Version 1.0.0)*");
    SendString("\r\n***************************************************************");
    SendString("\r\n");

    /* Checks whether the FLASH Write Protection Status is set or not */
    if (FMC_ReadOptionByteWriteProtection() != 0xFFFFFFFF)
    {
        SendString("Chip write protection£¬unable to download program !***\r\n");
        SendString(">> Jump to user application 1\r\n");
        Jump_to_App(APP1);
    }

    while (1)
    {
        SendString("\r\n** Please select an operation item \r\n");
        SendString("*  1.Download Flash application 1 -----------------------> 1  *\r\n");
        SendString("*  2.Upload Flash application 1   -----------------------> 2  *\r\n");
        SendString("*  3.Jump to user application 1   -----------------------> 3  *\r\n");
        SendString("*  4.Download Flash application 2 -----------------------> 4  *\r\n");
        SendString("*  5.Upload Flash application 2   -----------------------> 5  *\r\n");
        SendString("*  6.Jump to user application 2   -----------------------> 6  *\r\n");
        SendString("***************************************************************\r\n");

        /* Receive key */
        key = ReadKey_TimeOut(MENU_WAIT_TIMEOUT);

        switch (key)
        {

            case 0x31:/* Download user application in the Flash */
                Download(APP1);
                break;

            case 0x32:/* Upload user application from the Flash */
                Upload(APP1);
                break;

            case 0x33:/* execute the new program */
            case 0xFF:/* execute the new program */
                SendString(">> Jump to user application 1 \r\n");
                Jump_to_App(APP1);
                break;

            case 0x34:/* execute the new program */
                Download(APP2);
                break;

            case 0x35:/* execute the new program */
                Upload(APP2);
                break;

            case 0x36:/* execute the new program */
                SendString(">> Jump to user application 2 \r\n");
                Jump_to_App(APP2);
                break;

            default:
                SendString(">> Invalid Number ! ==> The number should be either 1, 2 3 4 5 or 6\r\n");
                break;
        }
    }
}

/**@} end of group MENU_Functions */
/**@} end of group MENU */
/**@} end of group IAP_BootLoader */
/**@} end of group Examples */
