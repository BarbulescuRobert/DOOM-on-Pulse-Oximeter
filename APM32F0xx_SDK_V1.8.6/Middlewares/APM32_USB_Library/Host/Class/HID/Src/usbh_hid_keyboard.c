/*!
 * @file        usbh_hid_keyboard.c
 *
 * @brief       USB host HID keyboard function
 *
 * @attention
 *
 *  Copyright (C) 2023-2024 Geehy Semiconductor
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
#include "usbh_hid_keyboard.h"

/** @addtogroup APM32_USB_Library
  @{
  */

/** @addtogroup USBH_HID_Class
  @{
  */

/** @defgroup USBH_HID_Structures Structures
  @{
  */

USBH_HID_KEYBOARD_INFO_T hidKbInfo;

/* HID keyboard class handler */
USBH_HID_CLASS_T USBH_HID_KEYBOARD_Handler =
{
    USBH_HID_KeyBoardInit,
    USBH_HID_KeyboardDecode,
};

/**@} end of group USBH_HID_Structures*/

/** @defgroup USBH_HID_Variables Variables
  @{
  */
/* Usage ID 0x00 - 0xE7 */
static const uint8_t keyboardCode[] = {
    0,      0,      0,      0,      31,     50,     48,     33,
    19,     34,     35,     36,     24,     37,     38,     39,
    52,     51,     25,     26,     17,     20,     32,     21,
    23,     49,     18,     47,     22,     46,     2,      3,
    4,      5,      6,      7,      8,      9,      10,     11,
    43,     110,    15,     16,     61,     12,     13,     27,
    28,     29,     42,     40,     41,     1,      53,     54,
    55,     30,     112,    113,    114,    115,    116,    117,
    118,    119,    120,    121,    122,    123,    124,    125,
    126,    75,     80,     85,     76,     81,     86,     89,
    79,     84,     83,     90,     95,     100,    105,    106,
    108,    93,     98,     103,    92,     97,     102,    91,
    96,     101,    99,     104,    45,     129,    0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      107,    0,      56,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    58,     44,     60,     127,    64,     57,     62,     128
};

/* HID keyboard key value */
static const uint8_t keyboardKey[] =
{
    '\0',   '`',    '1',    '2',    '3',    '4',    '5',    '6',
    '7',    '8',    '9',    '0',    '-',    '=',    '\0',   '\r',
    '\t',   'q',    'w',    'e',    'r',    't',    'y',    'u',
    'i',    'o',    'p',    '[',    ']',    '\\',   '\0',   'a',
    's',    'd',    'f',    'g',    'h',    'j',    'k',    'l',
    ';',    '\'',   '\0',   '\n',   '\0',   '\0',   'z',    'x',
    'c',    'v',    'b',    'n',    'm',    ',',    '.',    '/',
    '\0',   '\0',   '\0',   '\0',   '\0',   ' ',    '\0',   '\0',
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\r',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',   '\0',   '\0',   '7',    '4',    '1',    '\0',   '/',
    '8',    '5',    '2',    '0',    '*',    '9',    '6',    '3',
    '.',    '-',    '+',    '\0',   '\n',   '\0',   '\0',   '\0',
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',   '\0',
    '\0',   '\0'
};

/* HID keyboard shift key value */
static const uint8_t keyboardShiftKey[] =
{
  '\0', '~',  '!',  '@',  '#',  '$',  '%',  '^',  '&',  '*',  '(',  ')',
  '_',  '+',  '\0', '\0', '\0', 'Q',  'W',  'E',  'R',  'T',  'Y',  'U',
  'I',  'O',  'P',  '{',  '}',  '|',  '\0', 'A',  'S',  'D',  'F',  'G',
  'H',  'J',  'K',  'L',  ':',  '"',  '\0', '\n', '\0', '\0', 'Z',  'X',
  'C',  'V',  'B',  'N',  'M',  '<',  '>',  '?',  '\0', '\0',  '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};
/**@} end of group USBH_HID_Variables*/

/** @defgroup USBH_HID_Functions Functions
  @{
  */

/*!
 * @brief       USB host HID init keyboard
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_HID_KeyBoardInit(USBH_INFO_T* usbInfo)
{
    USBH_STA_T  usbStatus = USBH_OK;
    USBH_HID_INFO_T* hidInfo = (USBH_HID_INFO_T*)usbInfo->activeClass->classData;
    uint16_t i;

    hidKbInfo.ctrlLeft = 0;
    hidKbInfo.altLeft = 0;
    hidKbInfo.shiftLeft = 0;
    hidKbInfo.winLeft = 0;

    hidKbInfo.ctrlRight = 0;
    hidKbInfo.altRight = 0;
    hidKbInfo.shiftRight = 0;
    hidKbInfo.winRight = 0;

    for (i = 0; i < USBH_HID_KEY_PRESSED_NUM_MAX; i++)
    {
        hidKbInfo.key[i] = 0;
    }

    for (i = 0; i < (sizeof(hidKbInfo.report.data) \
                     / sizeof(hidKbInfo.report.data[0])); i++)
    {
        hidKbInfo.report.data[i] = 0;
        hidKbInfo.report.rxBuffer[i] = 0;
    }

    if (hidInfo->epSize > sizeof(hidKbInfo.report.data))
    {
        hidInfo->epSize = (uint16_t)sizeof(hidKbInfo.report.data);
    }

    /* Init buffer point */
    hidInfo->buffer = (uint8_t*)(void*)hidKbInfo.report.data;

    return usbStatus;
}

/*!
 * @brief       USB host HID decode keyboard
 *
 * @param       usbInfo: usb host information
 *
 * @retval      USB host operation status
 */
USBH_STA_T USBH_HID_KeyboardDecode(USBH_INFO_T* usbInfo)
{
    USBH_STA_T  usbStatus = USBH_OK;

    uint8_t funKeyTemp;
    uint8_t i;

    UNUSED(usbInfo);

    if ((hidKbInfo.report.data[1] == 0x01010101) && \
            (hidKbInfo.report.data[0] == 0x01010000))
    {
        //USBH_USR_DBG("Keyboard do not support this operation");

        return USBH_ERR_NOT_SUP;
    }
    else
    {
        for (i = 2; i < 2 + USBH_HID_KEY_PRESSED_NUM_MAX; i++)
        {
            funKeyTemp = *((uint8_t*)(void*)hidKbInfo.report.data + i);
            if ((funKeyTemp == KEYBOARD_ERROR_ROLL_OVER) || \
                    (funKeyTemp == KEYBOARD_POST_FAIL) || \
                    (funKeyTemp == KEYBOARD_ERROR_UNDEFINED))
            {
                return USBH_FAIL;
            }
        }

        funKeyTemp = *((uint8_t*)(void*)hidKbInfo.report.data + 0);

        hidKbInfo.ctrlLeft     = (funKeyTemp & KB_L_CTRL_MASK) > 0 ? 1 : 0;
        hidKbInfo.shiftLeft    = (funKeyTemp & KB_L_SHIFT_MASK) > 0 ? 1 : 0;
        hidKbInfo.altLeft      = (funKeyTemp & KB_L_ALT_MASK) > 0 ? 1 : 0;
        hidKbInfo.winLeft      = (funKeyTemp & KB_L_GUI_MASK) > 0 ? 1 : 0;

        hidKbInfo.ctrlRight    = (funKeyTemp & KB_R_CTRL_MASK) > 0 ? 1 : 0;
        hidKbInfo.shiftRight   = (funKeyTemp & KB_R_SHIFT_MASK) > 0 ? 1 : 0;
        hidKbInfo.altRight     = (funKeyTemp & KB_R_ALT_MASK) > 0 ? 1 : 0;
        hidKbInfo.winRight     = (funKeyTemp & KB_R_GUI_MASK) > 0 ? 1 : 0;

        for (i = 0; i < USBH_HID_KEY_PRESSED_NUM_MAX; i++)
        {
            hidKbInfo.key[i] = *((uint8_t*)(void*)hidKbInfo.report.data + i + 2);
        }
    }

    return usbStatus;
}

/*!
 * @brief       USB host HID keyboard value to ascii code
 *
 * @param       kbInfo: keyboard information
 *
 * @retval      ascii code
 */
uint8_t USBH_HID_KeyboardValueToAsciiCode(USBH_HID_KEYBOARD_INFO_T* kbInfo)
{
    uint8_t c;
    uint8_t key = keyboardCode[kbInfo->key[0]];
    
    if ((kbInfo->shiftLeft != 0U) || (kbInfo->shiftRight != 0U))
    {
        c = keyboardShiftKey[key];
    }
    else
    {
        c = keyboardKey[key];
    }

    return c;
}

/*!
 * @brief       USB host HID keyboard data process
 *
 * @param       usbInfo: usb host information
 *
 * @retval      keyboard information
 */
__weak USBH_HID_KEYBOARD_INFO_T *USBH_HID_KeyboardCallback(USBH_INFO_T* usbInfo)
{
    if (USBH_HID_KeyboardDecode(usbInfo) == USBH_OK)
    {
        return &hidKbInfo;
    }

    return NULL;
}

/**@} end of group USBH_HID_Functions */
/**@} end of group USBH_HID_Class */
/**@} end of group APM32_USB_Library */
