/*!
 * @file        apm32f0xx.h
 *
 * @brief       CMSIS Cortex-M0 Device Peripheral Access Layer Header File.
 *
 * @details     This file contains all the peripheral register's definitions, bits definitions and memory mapping
 *
 * @version     V1.0.4
 *
 * @date        2025-07-04
 *
 * @attention
 *
 *  Copyright (C) 2020-2025 Geehy Semiconductor
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
#ifndef __APM32F0XX_H
#define __APM32F0XX_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @addtogroup CMSIS
  @{
*/

/** @addtogroup APM32F0xx
  * @brief Peripheral Access Layer
  @{
*/

/** @defgroup HSE_Macros
  @{
*/

/**
 * @brief Define Value of the External oscillator in Hz
 */
#ifndef  HSE_VALUE
#define  HSE_VALUE              ((uint32_t)8000000)
#endif

/* Time out for HSE start up */
#define HSE_STARTUP_TIMEOUT     ((uint32_t)0x10000)

/* Time out for HSI start up */
#define HSI_STARTUP_TIMEOUT    ((uint32_t)0x0500)

/* Value of the Internal oscillator in Hz */
#define HSI_VALUE              ((uint32_t)8000000)
#define HSI14_VALUE            ((uint32_t)14000000)
#define HSI48_VALUE            ((uint32_t)48000000)

#define LSE_VALUE              ((uint32_t)32768)
#define LSI_VALUE              ((uint32_t)40000)

/**@} end of group HSE_Macros */

/** @defgroup APM32F0xx_StdPeripheral_Library_Version
  @{
*/

/*!< [31:16] APM32F0XX Standard Peripheral Library main version V1.0.4*/
#define __APM32F0_DEVICE_VERSION_MAIN   (0x01) /*!< [31:24] main version */
#define __APM32F0_DEVICE_VERSION_SUB1   (0x00) /*!< [23:16] sub1 version */
#define __APM32F0_DEVICE_VERSION_SUB2   (0x04) /*!< [15:8]  sub2 version */
#define __APM32F0_DEVICE_VERSION_RC     (0x00) /*!< [7:0]  release candidate */
#define __APM32F0_DEVICE_VERSION        ((__APM32F0_DEVICE_VERSION_MAIN << 24)\
                                        |(__APM32F0_DEVICE_VERSION_SUB1 << 16)\
                                        |(__APM32F0_DEVICE_VERSION_SUB2 << 8 )\
                                        |(__APM32F0_DEVICE_VERSION_RC))

/**@} end of group APM32F0xx_StdPeripheral_Library_Version */

#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050) /* ARM Compiler V6 */
  #ifndef __weak
    #define __weak  __attribute__((weak))
  #endif
  #ifndef __packed
    #define __packed  __attribute__((packed))
  #endif
#elif defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
  #ifndef __packed
    #define __packed __attribute__((__packed__))
  #endif /* __packed */
#endif /* __GNUC__ */

/** @addtogroup Device_Included
  * @{
  */
#if defined (APM32F030x6) || defined (APM32F030x8) || defined (APM32F030xC)
#define    APM32F030
#endif /*!< APM32F030x6 or APM32F030x8 or APM32F030xC */

#if defined (APM32F051x6) || defined (APM32F051x8)
#define    APM32F051
#endif /*!< APM32F051x6 or APM32F051x8 */

#if defined (APM32F070xB)
#define    APM32F070
#endif /*!< APM32F070xB */

#if defined (APM32F071x8) || defined (APM32F071xB)
#define    APM32F071
#endif /*!< APM32F071x8 or  APM32F071xB */

#if defined (APM32F072x8) || defined (APM32F072xB)
#define    APM32F072
#endif /*!< APM32F072x8 or  APM32F072xB */

#if defined (APM32F091xB) || defined (APM32F091xC)
#define    APM32F091
#endif /*!< APM32F091xB or APM32F091xC */

#if !defined (APM32F030) && !defined (APM32F051) && !defined (APM32F070) && \
    !defined (APM32F071) && !defined (APM32F072) && !defined (APM32F091)
#error "Please select first the target APM32F0xx device used in your application (in apm32f0xx.h file)"
#endif

#if defined(APM32F030)
  #include "apm32f030xx.h"
#elif defined(APM32F051)
  #include "apm32f051xx.h"
#elif defined(APM32F072)
  #include "apm32f072xx.h"
#elif defined(APM32F091)
  #include "apm32f091xx.h"
#else
  #error "No valid APM32F0xx device is defined, please check your device selection in apm32f0xx.h file"
#endif

/**
  * @}
  */

/** @defgroup Exported_types
  * @{
*/

typedef enum {FALSE, TRUE} BOOL;

enum {BIT_RESET, BIT_SET};

enum {RESET = 0, SET = !RESET};

enum {DISABLE = 0, ENABLE = !DISABLE};

enum {ERROR = 0, SUCCESS = !ERROR};

#ifndef __IM
#define __IM   __I
#endif
#ifndef __OM
#define __OM   __O
#endif
#ifndef __IOM
#define __IOM  __IO
#endif

#ifndef NULL
#define NULL   ((void *)0)
#endif

#if defined (__CC_ARM )
#pragma anon_unions
#endif

/**@} end of group Exported_types */

/** @defgroup Exported_Macros
  @{
*/

/* Define one bit mask */
#define BIT0    0x00000001
#define BIT1    0x00000002
#define BIT2    0x00000004
#define BIT3    0x00000008
#define BIT4    0x00000010
#define BIT5    0x00000020
#define BIT6    0x00000040
#define BIT7    0x00000080
#define BIT8    0x00000100
#define BIT9    0x00000200
#define BIT10   0x00000400
#define BIT11   0x00000800
#define BIT12   0x00001000
#define BIT13   0x00002000
#define BIT14   0x00004000
#define BIT15   0x00008000
#define BIT16   0x00010000
#define BIT17   0x00020000
#define BIT18   0x00040000
#define BIT19   0x00080000
#define BIT20   0x00100000
#define BIT21   0x00200000
#define BIT22   0x00400000
#define BIT23   0x00800000
#define BIT24   0x01000000
#define BIT25   0x02000000
#define BIT26   0x04000000
#define BIT27   0x08000000
#define BIT28   0x10000000
#define BIT29   0x20000000
#define BIT30   0x40000000
#define BIT31   0x80000000

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

/**@} end of group Exported_Macros*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __APM32F0xx_H */

/**@} end of group APM32F0xx */
/**@} end of group CMSIS */
