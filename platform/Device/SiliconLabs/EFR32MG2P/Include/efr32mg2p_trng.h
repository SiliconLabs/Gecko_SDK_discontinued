/**************************************************************************//**
 * @file efr32mg2p_trng.h
 * @brief EFR32MG2P_TRNG register and bit field definitions
 * @version 5.0.0
 ******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 ******************************************************************************
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software.@n
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.@n
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Silicon Laboratories, Inc.
 * has no obligation to support this Software. Silicon Laboratories, Inc. is
 * providing the Software "AS IS", with no express or implied warranties of any
 * kind, including, but not limited to, any implied warranties of
 * merchantability or fitness for any particular purpose or warranties against
 * infringement of any proprietary rights of a third party.
 *
 * Silicon Laboratories, Inc. will not be liable for any consequential,
 * incidental, or special damages, or any other relief, or for any claim by
 * any third party, arising from your use of this Software.
 *
 *****************************************************************************/
/**************************************************************************//**
* @addtogroup Parts
* @{
******************************************************************************/
/**************************************************************************//**
 * @defgroup EFR32MG2P_TRNG
 * @{
 * @brief EFR32MG2P_TRNG Register Declaration
 *****************************************************************************/
typedef struct
{
  __IOM uint32_t CONTROL;       /**< Main Control Register  */
  __IM uint32_t  FIFOLEVEL;     /**< FIFO level register  */
  uint32_t       RESERVED0[1];  /**< Reserved for future use **/
  __IM uint32_t  FIFODEPTH;     /**< FIFO depth register  */
  __IOM uint32_t KEY0;          /**< Key Register 0  */
  __IOM uint32_t KEY1;          /**< Key Register 1  */
  __IOM uint32_t KEY2;          /**< Key Register 2  */
  __IOM uint32_t KEY3;          /**< Key Register 3  */
  __IOM uint32_t TESTDATA;      /**< Test Data Register  */
  __IOM uint32_t REPTHRES;      /**< Threshold register (Rep)  */
  __IOM uint32_t PROP1THRES;    /**< Threshold register (Prop1)  */
  __IOM uint32_t PROP2THRES;    /**< Threshold register (Prop2)  */
  __IM uint32_t  STATUS;        /**< Status register  */
  __IOM uint32_t INITWAITVAL;   /**< Initial wait counter value  */
  uint32_t       RESERVED1[50]; /**< Reserved for future use **/
  __IM uint32_t  FIFO;          /**< FIFO data regsiter  */
} TRNG_TypeDef;                 /** @} */

/**************************************************************************//**
 * @defgroup EFR32MG2P_TRNG_BitFields
 * @{
 *****************************************************************************/

/* Bit fields for TRNG CONTROL */
#define _TRNG_CONTROL_RESETVALUE               0x00000000UL                                /**< Default value for TRNG_CONTROL */
#define _TRNG_CONTROL_MASK                     0x00003FFFUL                                /**< Mask for TRNG_CONTROL */
#define TRNG_CONTROL_ENABLE                    (0x1UL << 0)                                /**< Main enable bit */
#define _TRNG_CONTROL_ENABLE_SHIFT             0                                           /**< Shift value for TRNG_ENABLE */
#define _TRNG_CONTROL_ENABLE_MASK              0x1UL                                       /**< Bit mask for TRNG_ENABLE */
#define _TRNG_CONTROL_ENABLE_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_ENABLE_DEFAULT            (_TRNG_CONTROL_ENABLE_DEFAULT << 0)         /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_CONTROL                   (0x1UL << 1)                                /**< Control Mode */
#define _TRNG_CONTROL_CONTROL_SHIFT            1                                           /**< Shift value for TRNG_CONTROL */
#define _TRNG_CONTROL_CONTROL_MASK             0x2UL                                       /**< Bit mask for TRNG_CONTROL */
#define _TRNG_CONTROL_CONTROL_DEFAULT          0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_CONTROL_DEFAULT           (_TRNG_CONTROL_CONTROL_DEFAULT << 1)        /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_TESTEN                    (0x1UL << 2)                                /**< Select input for conditioning function and continuous tests */
#define _TRNG_CONTROL_TESTEN_SHIFT             2                                           /**< Shift value for TRNG_TESTEN */
#define _TRNG_CONTROL_TESTEN_MASK              0x4UL                                       /**< Bit mask for TRNG_TESTEN */
#define _TRNG_CONTROL_TESTEN_DEFAULT           0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_TESTEN_DEFAULT            (_TRNG_CONTROL_TESTEN_DEFAULT << 2)         /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_CONDBYPASS                (0x1UL << 3)                                /**< Conditioning bypass control */
#define _TRNG_CONTROL_CONDBYPASS_SHIFT         3                                           /**< Shift value for TRNG_CONDBYPASS */
#define _TRNG_CONTROL_CONDBYPASS_MASK          0x8UL                                       /**< Bit mask for TRNG_CONDBYPASS */
#define _TRNG_CONTROL_CONDBYPASS_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_CONDBYPASS_DEFAULT        (_TRNG_CONTROL_CONDBYPASS_DEFAULT << 3)     /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENREP                  (0x1UL << 4)                                /**< Interrupt enable for Repetition Count Test failure */
#define _TRNG_CONTROL_INTENREP_SHIFT           4                                           /**< Shift value for TRNG_INTENREP */
#define _TRNG_CONTROL_INTENREP_MASK            0x10UL                                      /**< Bit mask for TRNG_INTENREP */
#define _TRNG_CONTROL_INTENREP_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENREP_DEFAULT          (_TRNG_CONTROL_INTENREP_DEFAULT << 4)       /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPROP1                (0x1UL << 5)                                /**< Interrupt enable for Adaptive Proportion Test failure (64-sample window) */
#define _TRNG_CONTROL_INTENPROP1_SHIFT         5                                           /**< Shift value for TRNG_INTENPROP1 */
#define _TRNG_CONTROL_INTENPROP1_MASK          0x20UL                                      /**< Bit mask for TRNG_INTENPROP1 */
#define _TRNG_CONTROL_INTENPROP1_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPROP1_DEFAULT        (_TRNG_CONTROL_INTENPROP1_DEFAULT << 5)     /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPROP2                (0x1UL << 6)                                /**< Interrupt enable for Adaptive Proportion Test failure (4096-sample window) */
#define _TRNG_CONTROL_INTENPROP2_SHIFT         6                                           /**< Shift value for TRNG_INTENPROP2 */
#define _TRNG_CONTROL_INTENPROP2_MASK          0x40UL                                      /**< Bit mask for TRNG_INTENPROP2 */
#define _TRNG_CONTROL_INTENPROP2_DEFAULT       0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPROP2_DEFAULT        (_TRNG_CONTROL_INTENPROP2_DEFAULT << 6)     /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENFULL                 (0x1UL << 7)                                /**< Interrupt enable for FIFO full */
#define _TRNG_CONTROL_INTENFULL_SHIFT          7                                           /**< Shift value for TRNG_INTENFULL */
#define _TRNG_CONTROL_INTENFULL_MASK           0x80UL                                      /**< Bit mask for TRNG_INTENFULL */
#define _TRNG_CONTROL_INTENFULL_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENFULL_DEFAULT         (_TRNG_CONTROL_INTENFULL_DEFAULT << 7)      /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_SOFTRESET                 (0x1UL << 8)                                /**< Software reset */
#define _TRNG_CONTROL_SOFTRESET_SHIFT          8                                           /**< Shift value for TRNG_SOFTRESET */
#define _TRNG_CONTROL_SOFTRESET_MASK           0x100UL                                     /**< Bit mask for TRNG_SOFTRESET */
#define _TRNG_CONTROL_SOFTRESET_DEFAULT        0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_SOFTRESET_DEFAULT         (_TRNG_CONTROL_SOFTRESET_DEFAULT << 8)      /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPRE                  (0x1UL << 9)                                /**< Interrupt enable for preliminary noise alarm */
#define _TRNG_CONTROL_INTENPRE_SHIFT           9                                           /**< Shift value for TRNG_INTENPRE */
#define _TRNG_CONTROL_INTENPRE_MASK            0x200UL                                     /**< Bit mask for TRNG_INTENPRE */
#define _TRNG_CONTROL_INTENPRE_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENPRE_DEFAULT          (_TRNG_CONTROL_INTENPRE_DEFAULT << 9)       /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENALM                  (0x1UL << 10)                               /**< Interrupt enable for noise alarm */
#define _TRNG_CONTROL_INTENALM_SHIFT           10                                          /**< Shift value for TRNG_INTENALM */
#define _TRNG_CONTROL_INTENALM_MASK            0x400UL                                     /**< Bit mask for TRNG_INTENALM */
#define _TRNG_CONTROL_INTENALM_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_INTENALM_DEFAULT          (_TRNG_CONTROL_INTENALM_DEFAULT << 10)      /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_FORCERUN                  (0x1UL << 11)                               /**< Force oscillators to run when FIFO is full */
#define _TRNG_CONTROL_FORCERUN_SHIFT           11                                          /**< Shift value for TRNG_FORCERUN */
#define _TRNG_CONTROL_FORCERUN_MASK            0x800UL                                     /**< Bit mask for TRNG_FORCERUN */
#define _TRNG_CONTROL_FORCERUN_DEFAULT         0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_FORCERUN_DEFAULT          (_TRNG_CONTROL_FORCERUN_DEFAULT << 11)      /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_BYPASSSTARTUP             (0x1UL << 12)                               /**< Bypass NIST start-up test and write data to FIFO even if test fail. */
#define _TRNG_CONTROL_BYPASSSTARTUP_SHIFT      12                                          /**< Shift value for TRNG_BYPASSSTARTUP */
#define _TRNG_CONTROL_BYPASSSTARTUP_MASK       0x1000UL                                    /**< Bit mask for TRNG_BYPASSSTARTUP */
#define _TRNG_CONTROL_BYPASSSTARTUP_DEFAULT    0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_BYPASSSTARTUP_DEFAULT     (_TRNG_CONTROL_BYPASSSTARTUP_DEFAULT << 12) /**< Shifted mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_BYPASSAIS31               (0x1UL << 13)                               /**< Bypass AIS31 start-up test and write data to FIFO even if test fail. */
#define _TRNG_CONTROL_BYPASSAIS31_SHIFT        13                                          /**< Shift value for TRNG_BYPASSAIS31 */
#define _TRNG_CONTROL_BYPASSAIS31_MASK         0x2000UL                                    /**< Bit mask for TRNG_BYPASSAIS31 */
#define _TRNG_CONTROL_BYPASSAIS31_DEFAULT      0x00000000UL                                /**< Mode DEFAULT for TRNG_CONTROL */
#define TRNG_CONTROL_BYPASSAIS31_DEFAULT       (_TRNG_CONTROL_BYPASSAIS31_DEFAULT << 13)   /**< Shifted mode DEFAULT for TRNG_CONTROL */

/* Bit fields for TRNG FIFOLEVEL */
#define _TRNG_FIFOLEVEL_RESETVALUE             0x00000000UL                         /**< Default value for TRNG_FIFOLEVEL */
#define _TRNG_FIFOLEVEL_MASK                   0xFFFFFFFFUL                         /**< Mask for TRNG_FIFOLEVEL */
#define _TRNG_FIFOLEVEL_VALUE_SHIFT            0                                    /**< Shift value for TRNG_VALUE */
#define _TRNG_FIFOLEVEL_VALUE_MASK             0xFFFFFFFFUL                         /**< Bit mask for TRNG_VALUE */
#define _TRNG_FIFOLEVEL_VALUE_DEFAULT          0x00000000UL                         /**< Mode DEFAULT for TRNG_FIFOLEVEL */
#define TRNG_FIFOLEVEL_VALUE_DEFAULT           (_TRNG_FIFOLEVEL_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_FIFOLEVEL */

/* Bit fields for TRNG FIFODEPTH */
#define _TRNG_FIFODEPTH_RESETVALUE             0x00000040UL                         /**< Default value for TRNG_FIFODEPTH */
#define _TRNG_FIFODEPTH_MASK                   0xFFFFFFFFUL                         /**< Mask for TRNG_FIFODEPTH */
#define _TRNG_FIFODEPTH_VALUE_SHIFT            0                                    /**< Shift value for TRNG_VALUE */
#define _TRNG_FIFODEPTH_VALUE_MASK             0xFFFFFFFFUL                         /**< Bit mask for TRNG_VALUE */
#define _TRNG_FIFODEPTH_VALUE_DEFAULT          0x00000040UL                         /**< Mode DEFAULT for TRNG_FIFODEPTH */
#define TRNG_FIFODEPTH_VALUE_DEFAULT           (_TRNG_FIFODEPTH_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_FIFODEPTH */

/* Bit fields for TRNG KEY0 */
#define _TRNG_KEY0_RESETVALUE                  0x00000000UL                    /**< Default value for TRNG_KEY0 */
#define _TRNG_KEY0_MASK                        0xFFFFFFFFUL                    /**< Mask for TRNG_KEY0 */
#define _TRNG_KEY0_VALUE_SHIFT                 0                               /**< Shift value for TRNG_VALUE */
#define _TRNG_KEY0_VALUE_MASK                  0xFFFFFFFFUL                    /**< Bit mask for TRNG_VALUE */
#define _TRNG_KEY0_VALUE_DEFAULT               0x00000000UL                    /**< Mode DEFAULT for TRNG_KEY0 */
#define TRNG_KEY0_VALUE_DEFAULT                (_TRNG_KEY0_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_KEY0 */

/* Bit fields for TRNG KEY1 */
#define _TRNG_KEY1_RESETVALUE                  0x00000000UL                    /**< Default value for TRNG_KEY1 */
#define _TRNG_KEY1_MASK                        0xFFFFFFFFUL                    /**< Mask for TRNG_KEY1 */
#define _TRNG_KEY1_VALUE_SHIFT                 0                               /**< Shift value for TRNG_VALUE */
#define _TRNG_KEY1_VALUE_MASK                  0xFFFFFFFFUL                    /**< Bit mask for TRNG_VALUE */
#define _TRNG_KEY1_VALUE_DEFAULT               0x00000000UL                    /**< Mode DEFAULT for TRNG_KEY1 */
#define TRNG_KEY1_VALUE_DEFAULT                (_TRNG_KEY1_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_KEY1 */

/* Bit fields for TRNG KEY2 */
#define _TRNG_KEY2_RESETVALUE                  0x00000000UL                    /**< Default value for TRNG_KEY2 */
#define _TRNG_KEY2_MASK                        0xFFFFFFFFUL                    /**< Mask for TRNG_KEY2 */
#define _TRNG_KEY2_VALUE_SHIFT                 0                               /**< Shift value for TRNG_VALUE */
#define _TRNG_KEY2_VALUE_MASK                  0xFFFFFFFFUL                    /**< Bit mask for TRNG_VALUE */
#define _TRNG_KEY2_VALUE_DEFAULT               0x00000000UL                    /**< Mode DEFAULT for TRNG_KEY2 */
#define TRNG_KEY2_VALUE_DEFAULT                (_TRNG_KEY2_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_KEY2 */

/* Bit fields for TRNG KEY3 */
#define _TRNG_KEY3_RESETVALUE                  0x00000000UL                    /**< Default value for TRNG_KEY3 */
#define _TRNG_KEY3_MASK                        0xFFFFFFFFUL                    /**< Mask for TRNG_KEY3 */
#define _TRNG_KEY3_VALUE_SHIFT                 0                               /**< Shift value for TRNG_VALUE */
#define _TRNG_KEY3_VALUE_MASK                  0xFFFFFFFFUL                    /**< Bit mask for TRNG_VALUE */
#define _TRNG_KEY3_VALUE_DEFAULT               0x00000000UL                    /**< Mode DEFAULT for TRNG_KEY3 */
#define TRNG_KEY3_VALUE_DEFAULT                (_TRNG_KEY3_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_KEY3 */

/* Bit fields for TRNG TESTDATA */
#define _TRNG_TESTDATA_RESETVALUE              0x00000000UL                        /**< Default value for TRNG_TESTDATA */
#define _TRNG_TESTDATA_MASK                    0xFFFFFFFFUL                        /**< Mask for TRNG_TESTDATA */
#define _TRNG_TESTDATA_VALUE_SHIFT             0                                   /**< Shift value for TRNG_VALUE */
#define _TRNG_TESTDATA_VALUE_MASK              0xFFFFFFFFUL                        /**< Bit mask for TRNG_VALUE */
#define _TRNG_TESTDATA_VALUE_DEFAULT           0x00000000UL                        /**< Mode DEFAULT for TRNG_TESTDATA */
#define TRNG_TESTDATA_VALUE_DEFAULT            (_TRNG_TESTDATA_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_TESTDATA */

/* Bit fields for TRNG REPTHRES */
#define _TRNG_REPTHRES_RESETVALUE              0x0000003DUL                        /**< Default value for TRNG_REPTHRES */
#define _TRNG_REPTHRES_MASK                    0xFFFFFFFFUL                        /**< Mask for TRNG_REPTHRES */
#define _TRNG_REPTHRES_VALUE_SHIFT             0                                   /**< Shift value for TRNG_VALUE */
#define _TRNG_REPTHRES_VALUE_MASK              0xFFFFFFFFUL                        /**< Bit mask for TRNG_VALUE */
#define _TRNG_REPTHRES_VALUE_DEFAULT           0x0000003DUL                        /**< Mode DEFAULT for TRNG_REPTHRES */
#define TRNG_REPTHRES_VALUE_DEFAULT            (_TRNG_REPTHRES_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_REPTHRES */

/* Bit fields for TRNG PROP1THRES */
#define _TRNG_PROP1THRES_RESETVALUE            0x00000033UL                          /**< Default value for TRNG_PROP1THRES */
#define _TRNG_PROP1THRES_MASK                  0xFFFFFFFFUL                          /**< Mask for TRNG_PROP1THRES */
#define _TRNG_PROP1THRES_VALUE_SHIFT           0                                     /**< Shift value for TRNG_VALUE */
#define _TRNG_PROP1THRES_VALUE_MASK            0xFFFFFFFFUL                          /**< Bit mask for TRNG_VALUE */
#define _TRNG_PROP1THRES_VALUE_DEFAULT         0x00000033UL                          /**< Mode DEFAULT for TRNG_PROP1THRES */
#define TRNG_PROP1THRES_VALUE_DEFAULT          (_TRNG_PROP1THRES_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_PROP1THRES */

/* Bit fields for TRNG PROP2THRES */
#define _TRNG_PROP2THRES_RESETVALUE            0x000008C0UL                          /**< Default value for TRNG_PROP2THRES */
#define _TRNG_PROP2THRES_MASK                  0xFFFFFFFFUL                          /**< Mask for TRNG_PROP2THRES */
#define _TRNG_PROP2THRES_VALUE_SHIFT           0                                     /**< Shift value for TRNG_VALUE */
#define _TRNG_PROP2THRES_VALUE_MASK            0xFFFFFFFFUL                          /**< Bit mask for TRNG_VALUE */
#define _TRNG_PROP2THRES_VALUE_DEFAULT         0x000008C0UL                          /**< Mode DEFAULT for TRNG_PROP2THRES */
#define TRNG_PROP2THRES_VALUE_DEFAULT          (_TRNG_PROP2THRES_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_PROP2THRES */

/* Bit fields for TRNG STATUS */
#define _TRNG_STATUS_RESETVALUE                0x00000000UL                             /**< Default value for TRNG_STATUS */
#define _TRNG_STATUS_MASK                      0x000007F1UL                             /**< Mask for TRNG_STATUS */
#define TRNG_STATUS_TESTDATABUSY               (0x1UL << 0)                             /**< High when data written to TestData register is being processed */
#define _TRNG_STATUS_TESTDATABUSY_SHIFT        0                                        /**< Shift value for TRNG_TESTDATABUSY */
#define _TRNG_STATUS_TESTDATABUSY_MASK         0x1UL                                    /**< Bit mask for TRNG_TESTDATABUSY */
#define _TRNG_STATUS_TESTDATABUSY_DEFAULT      0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_TESTDATABUSY_DEFAULT       (_TRNG_STATUS_TESTDATABUSY_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_REPFAIL                    (0x1UL << 4)                             /**< Repetition Count Test interrupt status */
#define _TRNG_STATUS_REPFAIL_SHIFT             4                                        /**< Shift value for TRNG_REPFAIL */
#define _TRNG_STATUS_REPFAIL_MASK              0x10UL                                   /**< Bit mask for TRNG_REPFAIL */
#define _TRNG_STATUS_REPFAIL_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_REPFAIL_DEFAULT            (_TRNG_STATUS_REPFAIL_DEFAULT << 4)      /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PROP1FAIL                  (0x1UL << 5)                             /**< Adaptive Proportion Test (64-sample window) interrupt status */
#define _TRNG_STATUS_PROP1FAIL_SHIFT           5                                        /**< Shift value for TRNG_PROP1FAIL */
#define _TRNG_STATUS_PROP1FAIL_MASK            0x20UL                                   /**< Bit mask for TRNG_PROP1FAIL */
#define _TRNG_STATUS_PROP1FAIL_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PROP1FAIL_DEFAULT          (_TRNG_STATUS_PROP1FAIL_DEFAULT << 5)    /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PROP2FAIL                  (0x1UL << 6)                             /**< Adaptive Proportion Test (4096-sample window) interrupt status */
#define _TRNG_STATUS_PROP2FAIL_SHIFT           6                                        /**< Shift value for TRNG_PROP2FAIL */
#define _TRNG_STATUS_PROP2FAIL_MASK            0x40UL                                   /**< Bit mask for TRNG_PROP2FAIL */
#define _TRNG_STATUS_PROP2FAIL_DEFAULT         0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PROP2FAIL_DEFAULT          (_TRNG_STATUS_PROP2FAIL_DEFAULT << 6)    /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_FULLINT                    (0x1UL << 7)                             /**< FIFO full interrupt status */
#define _TRNG_STATUS_FULLINT_SHIFT             7                                        /**< Shift value for TRNG_FULLINT */
#define _TRNG_STATUS_FULLINT_MASK              0x80UL                                   /**< Bit mask for TRNG_FULLINT */
#define _TRNG_STATUS_FULLINT_DEFAULT           0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_FULLINT_DEFAULT            (_TRNG_STATUS_FULLINT_DEFAULT << 7)      /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PREINT                     (0x1UL << 8)                             /**< Preliminary noise alarm status */
#define _TRNG_STATUS_PREINT_SHIFT              8                                        /**< Shift value for TRNG_PREINT */
#define _TRNG_STATUS_PREINT_MASK               0x100UL                                  /**< Bit mask for TRNG_PREINT */
#define _TRNG_STATUS_PREINT_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_PREINT_DEFAULT             (_TRNG_STATUS_PREINT_DEFAULT << 8)       /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_ALMINT                     (0x1UL << 9)                             /**< Noise alarm status */
#define _TRNG_STATUS_ALMINT_SHIFT              9                                        /**< Shift value for TRNG_ALMINT */
#define _TRNG_STATUS_ALMINT_MASK               0x200UL                                  /**< Bit mask for TRNG_ALMINT */
#define _TRNG_STATUS_ALMINT_DEFAULT            0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_ALMINT_DEFAULT             (_TRNG_STATUS_ALMINT_DEFAULT << 9)       /**< Shifted mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_STARTUPPASS                (0x1UL << 10)                            /**< Start-up test pass status */
#define _TRNG_STATUS_STARTUPPASS_SHIFT         10                                       /**< Shift value for TRNG_STARTUPPASS */
#define _TRNG_STATUS_STARTUPPASS_MASK          0x400UL                                  /**< Bit mask for TRNG_STARTUPPASS */
#define _TRNG_STATUS_STARTUPPASS_DEFAULT       0x00000000UL                             /**< Mode DEFAULT for TRNG_STATUS */
#define TRNG_STATUS_STARTUPPASS_DEFAULT        (_TRNG_STATUS_STARTUPPASS_DEFAULT << 10) /**< Shifted mode DEFAULT for TRNG_STATUS */

/* Bit fields for TRNG INITWAITVAL */
#define _TRNG_INITWAITVAL_RESETVALUE           0x000000FFUL                           /**< Default value for TRNG_INITWAITVAL */
#define _TRNG_INITWAITVAL_MASK                 0x000000FFUL                           /**< Mask for TRNG_INITWAITVAL */
#define _TRNG_INITWAITVAL_VALUE_SHIFT          0                                      /**< Shift value for TRNG_VALUE */
#define _TRNG_INITWAITVAL_VALUE_MASK           0xFFUL                                 /**< Bit mask for TRNG_VALUE */
#define _TRNG_INITWAITVAL_VALUE_DEFAULT        0x000000FFUL                           /**< Mode DEFAULT for TRNG_INITWAITVAL */
#define TRNG_INITWAITVAL_VALUE_DEFAULT         (_TRNG_INITWAITVAL_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_INITWAITVAL */

/* Bit fields for TRNG FIFO */
#define _TRNG_FIFO_RESETVALUE                  0x00000000UL                    /**< Default value for TRNG_FIFO */
#define _TRNG_FIFO_MASK                        0xFFFFFFFFUL                    /**< Mask for TRNG_FIFO */
#define _TRNG_FIFO_VALUE_SHIFT                 0                               /**< Shift value for TRNG_VALUE */
#define _TRNG_FIFO_VALUE_MASK                  0xFFFFFFFFUL                    /**< Bit mask for TRNG_VALUE */
#define _TRNG_FIFO_VALUE_DEFAULT               0x00000000UL                    /**< Mode DEFAULT for TRNG_FIFO */
#define TRNG_FIFO_VALUE_DEFAULT                (_TRNG_FIFO_VALUE_DEFAULT << 0) /**< Shifted mode DEFAULT for TRNG_FIFO */

/** @} End of group EFR32MG2P_TRNG */
/** @} End of group Parts */

