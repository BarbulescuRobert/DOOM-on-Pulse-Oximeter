/*!
 * @file       main.c
 *
 * @brief      Main program body
 *
 * @version    V1.0.1
 *
 * @date       2022-09-20
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
#include "main.h"
#include "Board.h"
#include "rtthread.h"

/** @addtogroup Examples
  * @{
  */

/** @addtogroup RT-thread
  * @{
  */

/** @defgroup RT-thread_Macros Macros
  * @{
  */

/** @defgroup RT-thread_Enumerations Enumerations
  * @{
  */

/**@} end of group RT-thread_Enumerations */

/** @defgroup RT-thread_Structures Structures
  * @{
  */

/**@} end of group RT-thread_Structures */

/** @defgroup RT-thread_Variables Variables
  * @{
  */

static rt_thread_t led1_thread = RT_NULL;

/**@} end of group RT-thread_Variables */

/** @defgroup RT-thread_Functions Functions
  * @{
  */

static void led1_thread_entry(void* parameter)
{
    while (1)
    {
        APM_MINI_LEDToggle(LED3);
        rt_thread_delay(1000);
        APM_MINI_LEDToggle(LED2);
    }
}

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 */
int main(void)
{
    APM_MINI_LEDInit(LED2);
    APM_MINI_LEDInit(LED3);

    led1_thread = rt_thread_create("led1",
                                   led1_thread_entry,
                                   RT_NULL,
                                   512,
                                   3,
                                   20);
    rt_thread_startup(led1_thread);
}

/**@} end of group RT-thread_Functions */
/**@} end of group RT-thread */
/**@} end of group Examples */
