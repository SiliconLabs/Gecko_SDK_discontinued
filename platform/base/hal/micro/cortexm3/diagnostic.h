/** @file hal/micro/cortexm3/diagnostic.h
 * See @ref diagnostics for detailed documentation.
 *
 * <!-- Copyright 2009 by Ember Corporation. All rights reserved.-->
 */

/** @addtogroup diagnostics
 * @brief Crash and watchdog diagnostic functions.
 *
 * See diagnostic.h for source code.
 *@{
 */
 
#ifndef __EM3XX_DIAGNOSTIC_H__
#define __EM3XX_DIAGNOSTIC_H__

#ifndef DOXYGEN_SHOULD_SKIP_THIS

// Define the reset reasons that should print out detailed crash data.
#define RESET_CRASH_REASON_MASK ( (1 << RESET_UNKNOWN) |    \
                                  (1 << RESET_WATCHDOG) |   \
                                  (1 << RESET_CRASH) |      \
                                  (1 << RESET_FLASH) |      \
                                  (1 << RESET_FAULT) |      \
                                  (1 << RESET_FATAL) )        

typedef struct
{
  // first two fields must be the same as HalCrashInfoType
  uint16_t resetReason;       // reason written out just before forcing a reset
  uint16_t resetSignature; 
  uint16_t panId;         // PanId that the bootloader will use
  uint8_t  radioChannel;  // emberGetRadioChannel() - 11
  uint8_t  radioPower;    // emberGetRadioPower()
  uint8_t  radioLnaCal;   // Low Noise Amplifier calibration
  uint8_t  reserved[22]; // (reserved for future use)
} HalBootParamType;

typedef struct
{
  PGM_P file;
  int line;
} HalAssertInfoType;

// note that assertInfo and dmaProt are written just before a forced reboot
typedef union
{
  HalAssertInfoType assertInfo;
  struct { uint32_t channel; uint32_t address; } dmaProt;
} HalCrashSpecificDataType;

// Define crash registers as structs so a debugger can display their bit fields 
typedef union {
  struct
  {
    uint32_t EXCPT          : 9;  // B0-8
    uint32_t ICIIT_LOW      : 7;  // B9-15
    uint32_t                : 8;  // B16-23
    uint32_t T              : 1;  // B24
    uint32_t ICIIT_HIGH     : 2;  // B25-26
    uint32_t Q              : 1;  // B27
    uint32_t V              : 1;  // B28
    uint32_t C              : 1;  // B29
    uint32_t Z              : 1;  // B30
    uint32_t N              : 1;  // B31
  } bits;
  uint32_t word;
} HalCrashxPsrType;

typedef union {
  struct
  {
    uint32_t VECTACTIVE     : 9;  // B0-8
    uint32_t                : 2;  // B9-10
    uint32_t RETTOBASE      : 1;  // B11
    uint32_t VECTPENDING    : 9;  // B12-20
    uint32_t                : 1;  // B21
    uint32_t ISRPENDING     : 1;  // B22
    uint32_t ISRPREEMPT     : 1;  // B23
    uint32_t                : 1;  // B24
    uint32_t PENDSTCLR      : 1;  // B25
    uint32_t PENDSTSET      : 1;  // B26
    uint32_t PENDSVCLR      : 1;  // B27
    uint32_t PENDSVSET      : 1;  // B28
    uint32_t                : 2;  // B29-30
    uint32_t NMIPENDSET     : 1;  // B31
  } bits;
  uint32_t word;
} HalCrashIcsrType;

typedef union {
  struct
  {
#ifdef _EZR_DEVICE
    uint32_t DMA_IRQn       : 1;  // B0
    uint32_t GPIO_EVEN_IRQn : 1;  // B1
    uint32_t TIMER0_IRQn    : 1;  // B2
    uint32_t USART0_RX_IRQn : 1;  // B3
    uint32_t USART0_TX_IRQn : 1;  // B4
    uint32_t USB_IRQn       : 1;  // B5
    uint32_t ACMP0_IRQn     : 1;  // B6
    uint32_t ADC0_IRQn      : 1;  // B7
    uint32_t DAC0_IRQn      : 1;  // B8
    uint32_t I2C0_IRQn      : 1;  // B9
    uint32_t I2C1_IRQn      : 1;  // B10
    uint32_t GPIO_ODD_IRQn  : 1;  // B11
    uint32_t TIMER1_IRQn    : 1;  // B12
    uint32_t TIMER2_IRQn    : 1;  // B13
    uint32_t TIMER3_IRQn    : 1;  // B14
    uint32_t USART1_RX_IRQn : 1;  // B15
    uint32_t USART1_TX_IRQn : 1;  // B16
    uint32_t LESENSE_IRQn   : 1;  // B17
    uint32_t USART2_RX_IRQn : 1;  // B18
    uint32_t USART2_TX_IRQn : 1;  // B19
    uint32_t UART0_RX_IRQn  : 1;  // B20
    uint32_t UART0_TX_IRQn  : 1;  // B21
    uint32_t UART1_RX_IRQn  : 1;  // B22
    uint32_t UART1_TX_IRQn  : 1;  // B23
    uint32_t LEUART0_IRQn   : 1;  // B24
    uint32_t LEUART1_IRQn   : 1;  // B25
    uint32_t LETIMER0_IRQn  : 1;  // B26
    uint32_t PCNT0_IRQn     : 1;  // B27
    uint32_t PCNT1_IRQn     : 1;  // B28
    uint32_t PCNT2_IRQn     : 1;  // B29
    uint32_t RTC_IRQn       : 1;  // B30
    uint32_t BURTC_IRQn     : 1;  // B31
    uint32_t CMU_IRQn       : 1;  // B32
    uint32_t VCMP_IRQn      : 1;  // B33
    uint32_t LCD_IRQn       : 1;  // B34
    uint32_t MSC_IRQn       : 1;  // B35
    uint32_t AES_IRQn       : 1;  // B36
    uint32_t EBI_IRQn       : 1;  // B37
    uint32_t EMU_IRQn       : 1;  // B38
    uint32_t                : 25; // B39-63
  } bits;
  uint32_t word[2];
#elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_1)
    uint32_t EMU_IRQn         : 1;  // B0
    uint32_t FRC_PRI_IRQn     : 1;  // B1
    uint32_t WDOG0_IRQn       : 1;  // B2
    uint32_t FRC_IRQn         : 1;  // B3
    uint32_t MODEM_IRQn       : 1;  // B4
    uint32_t RAC_SEQ_IRQn     : 1;  // B5
    uint32_t RAC_RSM_IRQn     : 1;  // B6
    uint32_t BUFC_IRQn        : 1;  // B7
    uint32_t LDMA_IRQn        : 1;  // B8
    uint32_t GPIO_EVEN_IRQn   : 1;  // B9
    uint32_t TIMER0_IRQn      : 1;  // B10
    uint32_t USART0_RX_IRQn   : 1;  // B11
    uint32_t USART0_TX_IRQn   : 1;  // B12
    uint32_t ACMP0_IRQn       : 1;  // B13
    uint32_t ADC0_IRQn        : 1;  // B14
    uint32_t IDAC0_IRQn       : 1;  // B15
    uint32_t I2C0_IRQn        : 1;  // B16
    uint32_t GPIO_ODD_IRQn    : 1;  // B17
    uint32_t TIMER1_IRQn      : 1;  // B18
    uint32_t USART1_RX_IRQn   : 1;  // B19
    uint32_t USART1_TX_IRQn   : 1;  // B20
    uint32_t LEUART0_IRQn     : 1;  // B21
    uint32_t PCNT0_IRQn       : 1;  // B22
    uint32_t CMU_IRQn         : 1;  // B23
    uint32_t MSC_IRQn         : 1;  // B24
    uint32_t CRYPTO_IRQn      : 1;  // B25
    uint32_t LETIMER0_IRQn    : 1;  // B26
    uint32_t AGC_IRQn         : 1;  // B27
    uint32_t PROTIMER_IRQn    : 1;  // B28
    uint32_t RTCC_IRQn        : 1;  // B29
    uint32_t SYNTH_IRQn       : 1;  // B30
    uint32_t CRYOTIMER_IRQn   : 1;  // B31
    uint32_t RFSENSE_IRQn     : 1;  // B32
    uint32_t FPUEH_IRQn       : 1;  // B33
    uint32_t                : 30; // B34-63
  } bits;
  uint32_t word[2];
#elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_2)
    uint32_t EMU_IRQn         : 1;  // B0
    uint32_t FRC_PRI_IRQn     : 1;  // B1
    uint32_t WDOG0_IRQn       : 1;  // B2
    uint32_t WDOG1_IRQn       : 1;  // B3
    uint32_t FRC_IRQn         : 1;  // B4
    uint32_t MODEM_IRQn       : 1;  // B5
    uint32_t RAC_SEQ_IRQn     : 1;  // B6
    uint32_t RAC_RSM_IRQn     : 1;  // B7
    uint32_t BUFC_IRQn        : 1;  // B8
    uint32_t LDMA_IRQn        : 1;  // B9
    uint32_t GPIO_EVEN_IRQn   : 1;  // B10
    uint32_t TIMER0_IRQn      : 1;  // B11
    uint32_t USART0_RX_IRQn   : 1;  // B12
    uint32_t USART0_TX_IRQn   : 1;  // B13
    uint32_t ACMP0_IRQn       : 1;  // B14
    uint32_t ADC0_IRQn        : 1;  // B15
    uint32_t IDAC0_IRQn       : 1;  // B16
    uint32_t I2C0_IRQn        : 1;  // B17
    uint32_t GPIO_ODD_IRQn    : 1;  // B18
    uint32_t TIMER1_IRQn      : 1;  // B19
    uint32_t USART1_RX_IRQn   : 1;  // B20
    uint32_t USART1_TX_IRQn   : 1;  // B21
    uint32_t LEUART0_IRQn     : 1;  // B22
    uint32_t PCNT0_IRQn       : 1;  // B23
    uint32_t CMU_IRQn         : 1;  // B24
    uint32_t MSC_IRQn         : 1;  // B25
    uint32_t CRYPTO0_IRQn     : 1;  // B26
    uint32_t LETIMER0_IRQn    : 1;  // B27
    uint32_t AGC_IRQn         : 1;  // B28
    uint32_t PROTIMER_IRQn    : 1;  // B29
    uint32_t RTCC_IRQn        : 1;  // B30
    uint32_t SYNTH_IRQn       : 1;  // B31
    uint32_t CRYOTIMER_IRQn   : 1;  // B32
    uint32_t RFSENSE_IRQn     : 1;  // B33
    uint32_t FPUEH_IRQn       : 1;  // B34
    uint32_t SMU_IRQn         : 1;  // B35
    uint32_t WTIMER0_IRQn     : 1;  // B36
    uint32_t WTIMER1_IRQn     : 1;  // B37
    uint32_t PCNT1_IRQn       : 1;  // B38
    uint32_t PCNT2_IRQn       : 1;  // B39
    uint32_t USART2_RX_IRQn   : 1;  // B40
    uint32_t USART2_TX_IRQn   : 1;  // B41
    uint32_t I2C1_IRQn        : 1;  // B42
    uint32_t USART3_RX_IRQn   : 1;  // B43
    uint32_t USART3_TX_IRQn   : 1;  // B44
    uint32_t VDAC0_IRQn       : 1;  // B45
    uint32_t CSEN_IRQn        : 1;  // B46
    uint32_t LESENSE_IRQn     : 1;  // B47
    uint32_t CRYPTO1_IRQn     : 1;  // B48
    uint32_t TRNG0_IRQn       : 1;  // B49
    uint32_t                : 14; // B50-63
  } bits;
  uint32_t word[2];
#elif defined (_SILICON_LABS_32B_SERIES_1_CONFIG_3)
    uint32_t EMU_IRQn         : 1;  // B0
    uint32_t FRC_PRI_IRQn     : 1;  // B1
    uint32_t WDOG0_IRQn       : 1;  // B2
    uint32_t WDOG1_IRQn       : 1;  // B3
    uint32_t FRC_IRQn         : 1;  // B4
    uint32_t MODEM_IRQn       : 1;  // B5
    uint32_t RAC_SEQ_IRQn     : 1;  // B6
    uint32_t RAC_RSM_IRQn     : 1;  // B7
    uint32_t BUFC_IRQn        : 1;  // B8
    uint32_t LDMA_IRQn        : 1;  // B9
    uint32_t GPIO_EVEN_IRQn   : 1;  // B10
    uint32_t TIMER0_IRQn      : 1;  // B11
    uint32_t USART0_RX_IRQn   : 1;  // B12
    uint32_t USART0_TX_IRQn   : 1;  // B13
    uint32_t ACMP0_IRQn       : 1;  // B14
    uint32_t ADC0_IRQn        : 1;  // B15
    uint32_t IDAC0_IRQn       : 1;  // B16
    uint32_t I2C0_IRQn        : 1;  // B17
    uint32_t GPIO_ODD_IRQn    : 1;  // B18
    uint32_t TIMER1_IRQn      : 1;  // B19
    uint32_t USART1_RX_IRQn   : 1;  // B20
    uint32_t USART1_TX_IRQn   : 1;  // B21
    uint32_t LEUART0_IRQn     : 1;  // B22
    uint32_t PCNT0_IRQn       : 1;  // B23
    uint32_t CMU_IRQn         : 1;  // B24
    uint32_t MSC_IRQn         : 1;  // B25
    uint32_t CRYPTO0_IRQn     : 1;  // B26
    uint32_t LETIMER0_IRQn    : 1;  // B27
    uint32_t AGC_IRQn         : 1;  // B28
    uint32_t PROTIMER_IRQn    : 1;  // B29
    uint32_t PRORTC_IRQn      : 1;  // B30
    uint32_t RTCC_IRQn        : 1;  // B31
    uint32_t SYNTH_IRQn       : 1;  // B32
    uint32_t CRYOTIMER_IRQn   : 1;  // B33
    uint32_t RFSENSE_IRQn     : 1;  // B34
    uint32_t FPUEH_IRQn       : 1;  // B35
    uint32_t SMU_IRQn         : 1;  // B36
    uint32_t WTIMER0_IRQn     : 1;  // B37
    uint32_t USART2_RX_IRQn   : 1;  // B38
    uint32_t USART2_TX_IRQn   : 1;  // B39
    uint32_t I2C1_IRQn        : 1;  // B40
    uint32_t VDAC0_IRQn       : 1;  // B41
    uint32_t CSEN_IRQn        : 1;  // B42
    uint32_t LESENSE_IRQn     : 1;  // B43
    uint32_t CRYPTO1_IRQn     : 1;  // B44
    uint32_t TRNG0_IRQn       : 1;  // B45
    uint32_t                : 18; // B46-63
  } bits;
  uint32_t word[2];
#elif CORTEXM3_EMBER_MICRO
    uint32_t TIM1_IRQn      : 1;  // B0
    uint32_t TIM2_IRQn      : 1;  // B1
    uint32_t MGMT_IRQn      : 1;  // B2
    uint32_t BB_IRQn        : 1;  // B3
    uint32_t SLEEPTMR_IRQn  : 1;  // B4
    uint32_t SC1_IRQn       : 1;  // B5
    uint32_t SC2_IRQn       : 1;  // B6
    uint32_t SEC_IRQn       : 1;  // B7
    uint32_t MACTMR_IRQn    : 1;  // B8
    uint32_t MACTX_IRQn     : 1;  // B9
    uint32_t MACRX_IRQn     : 1;  // B10
    uint32_t ADC_IRQn       : 1;  // B11
    uint32_t IRQA_IRQn      : 1;  // B12
    uint32_t IRQB_IRQn      : 1;  // B13
    uint32_t IRQC_IRQn      : 1;  // B14
    uint32_t IRQD_IRQn      : 1;  // B15
    uint32_t DEBUG_IRQn     : 1;  // B16
    uint32_t                : 15; // B17-31
  } bits;
  uint32_t word;
#else
  #error micro not recognized
#endif
} HalCrashIntActiveType;

typedef union {
  struct
  {
    uint32_t MEMFAULTACT    : 1;  // B0   
    uint32_t BUSFAULTACT    : 1;  // B1 
    uint32_t                : 1;  // B2 
    uint32_t USGFAULTACT    : 1;  // B3 
    uint32_t                : 3;  // B4-6 
    uint32_t SVCALLACT      : 1;  // B7 
    uint32_t MONITORACT     : 1;  // B8 
    uint32_t                : 1;  // B9 
    uint32_t PENDSVACT      : 1;  // B10
    uint32_t SYSTICKACT     : 1;  // B11
    uint32_t USGFAULTPENDED : 1;  // B12
    uint32_t MEMFAULTPENDED : 1;  // B13
    uint32_t BUSFAULTPENDED : 1;  // B14
    uint32_t SVCALLPENDED   : 1;  // B15
    uint32_t MEMFAULTENA    : 1;  // B16
    uint32_t BUSFAULTENA    : 1;  // B17
    uint32_t USGFAULTENA    : 1;  // B18
    uint32_t                : 13; // B19-31  
  } bits;
  uint32_t word;
} HalCrashShcsrType;

typedef union {
  struct
  {
    uint32_t IACCVIOL       : 1;  // B0 
    uint32_t DACCVIOL       : 1;  // B1
    uint32_t                : 1;  // B2
    uint32_t MUNSTKERR      : 1;  // B3
    uint32_t MSTKERR        : 1;  // B4
    uint32_t                : 2;  // B5-6
    uint32_t MMARVALID      : 1;  // B7
    uint32_t IBUSERR        : 1;  // B8
    uint32_t PRECISERR      : 1;  // B9
    uint32_t IMPRECISERR    : 1;  // B10
    uint32_t UNSTKERR       : 1;  // B11
    uint32_t STKERR         : 1;  // B12
    uint32_t                : 2;  // B13-14
    uint32_t BFARVALID      : 1;  // B15
    uint32_t UNDEFINSTR     : 1;  // B16
    uint32_t INVSTATE       : 1;  // B17
    uint32_t INVPC          : 1;  // B18
    uint32_t NOCP           : 1;  // B19
    uint32_t                : 4;  // B20-23
    uint32_t UNALIGNED      : 1;  // B24
    uint32_t DIVBYZERO      : 1;  // B25
    uint32_t                : 6;  // B26-31
  } bits;
  uint32_t word;
} HalCrashCfsrType;

typedef union {
  struct
  {
    uint32_t                : 1;  // B0
    uint32_t VECTTBL        : 1;  // B1
    uint32_t                : 28; // B2-29
    uint32_t FORCED         : 1;  // B30
    uint32_t DEBUGEVT       : 1;  // B31
  } bits;
  uint32_t word;
} HalCrashHfsrType;

typedef union {
  struct
  {
    uint32_t HALTED         : 1;  // B0
    uint32_t BKPT           : 1;  // B1
    uint32_t DWTTRAP        : 1;  // B2
    uint32_t VCATCH         : 1;  // B3
    uint32_t EXTERNAL       : 1;  // B4
    uint32_t                : 27; // B5-31         
  } bits;
  uint32_t word;
} HalCrashDfsrType;

typedef union {
  struct
  {
    uint32_t MISSED         : 1;  // B0
    uint32_t RESERVED       : 1;  // B1
    uint32_t PROTECTED      : 1;  // B2
    uint32_t WRONGSIZE      : 1;  // B3
    uint32_t                : 28; // B4-31
  } bits;
  uint32_t word;
} HalCrashAfsrType;

#define NUM_RETURNS     6

// Define the crash data structure
typedef struct
{
  // ***************************************************************************
  // The components within this first block are written by the assembly
  // language common fault handler, and position and order is critical.
  // cstartup-iar-boot-entry.s79 also relies on the position/order here.
  // Do not edit without also modifying that code.
  // ***************************************************************************
  uint16_t resetReason;    // reason written out just before forcing a reset
  uint16_t resetSignature; 
  uint32_t R0;            // processor registers
  uint32_t R1;
  uint32_t R2;
  uint32_t R3;
  uint32_t R4;
  uint32_t R5;
  uint32_t R6;
  uint32_t R7;
  uint32_t R8;
  uint32_t R9;
  uint32_t R10;
  uint32_t R11;
  uint32_t R12;
  uint32_t LR;
  uint32_t mainSP;        // main and process stack pointers
  uint32_t processSP;
  // ***************************************************************************
  // End of the block written by the common fault handler.
  // ***************************************************************************
  
  uint32_t PC;              // stacked return value (if it could be read)
  HalCrashxPsrType xPSR;  // stacked processor status reg (if it could be read)
  uint32_t mainSPUsed;      // bytes used in main stack
  uint32_t processSPUsed;   // bytes used in process stack
  uint32_t mainStackBottom; // address of the bottom of the stack
  HalCrashIcsrType icsr;  // interrupt control state register
  HalCrashShcsrType shcsr;// system handlers control and state register
  HalCrashIntActiveType intActive;  // irq active bit register
  HalCrashCfsrType cfsr;  // configurable fault status register
  HalCrashHfsrType hfsr;  // hard fault status register
  HalCrashDfsrType dfsr;  // debug fault status register
  uint32_t faultAddress;    // fault address register (MMAR or BFAR)
  HalCrashAfsrType afsr;  // auxiliary fault status register
  uint32_t returns[NUM_RETURNS];  // probable return addresses found on the stack       
  HalCrashSpecificDataType data;  // additional data specific to the crash type
} HalCrashInfoType;

typedef union 
{
  HalCrashInfoType crash;
  HalBootParamType boot;
} HalResetInfoType;

#define RESETINFO_WORDS  ((sizeof(HalResetInfoType)+3)/4)

extern HalResetInfoType halResetInfo;

void halInternalSaveAssertInfo(void);

#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @brief Macro evaluating to true if the last reset was a crash, false
 * otherwise.
 */
#define halResetWasCrash() \
                ( ( (1 << halGetResetInfo()) & RESET_CRASH_REASON_MASK) != 0)

/** @brief Returns the number of bytes used in the main stack.
 *
 * @return The number of bytes used in the main stack.
 */
uint32_t halGetMainStackBytesUsed(void);

/** @brief Print a summary of crash details.
 *
 * @param port  Serial port number (0 or 1). 
 */
void halPrintCrashSummary(uint8_t port);

/** @brief Print the complete, decoded crash details.
 *
 * @param port  Serial port number (0 or 1). 
 */
void halPrintCrashDetails(uint8_t port);

/** @brief Print the complete crash data.
 *
 * @param port  Serial port number (0 or 1). 
 */
void halPrintCrashData(uint8_t port);

/** @brief If last reset was from an assert, return saved assert information.
 *
 * @return Pointer to struct containing assert filename and line.
 */
const HalAssertInfoType *halGetAssertInfo(void);

#endif  //__EM3XX_DIAGNOSTIC_H__

/** @} END addtogroup */

