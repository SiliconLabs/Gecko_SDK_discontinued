/*
 * File: hal/micro/cortexm3/diagnostic.c
 * Description: EM3XX-specific diagnostic HAL functions
 *
 * Author(s):
 *
 * Copyright 2009 by Ember Corporation. All rights reserved.                *80*
 */

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "serial/serial.h"
#include "diagnostic.h"
#include "hal/micro/cortexm3/mpu.h"

#ifdef RTOS
  #include "rtos/rtos.h"
  #define freeRTOS 1
#else  // RTOS
  #define freeRTOS 0
  // extern uint32_t* xTaskGetCurrentTaskStackTop(void);
  // extern uint32_t* xTaskGetCurrentTaskStackBottom(void);
#endif // RTOS

//------------------------------------------------------------------------------
// Preprocessor definitions

// Reserved instruction executed after a failed assert to cause a usage fault
#define ASSERT_USAGE_OPCODE 0xDE42

// Codes for DMA channels in DMA_PROT_CH
#define DMA_PROT_CH_SC1_RX    1
#define DMA_PROT_CH_GP_ADC    3
#define DMA_PROT_CH_MAC       4
#define DMA_PROT_CH_SC2_RX    5


//------------------------------------------------------------------------------
// Local Variables

static PGM_P PGM cfsrBits[] =
{

  // Memory management (MPU) faults
  "IACCVIOL: attempted instruction fetch from a no-execute address",  // B0
  "DACCVIOL: attempted load or store at an illegal address",          // B1
  "",                                                                 // B2
  "MUNSTKERR: unstack from exception return caused access violation", // B3
  "MSTKERR: stacking from exception caused access violation",         // B4
  "",                                                                 // B5
  "",                                                                 // B6
  "MMARVALID: MMAR contains valid fault address",                     // B7

  // Bus faults
  "IBUSERR: instruction prefetch caused bus fault",                   // B8
  "PRECISERR: precise data bus fault",                                // B9
  "IMPRECISERR: imprecise data bus fault",                            // B10
  "UNSTKERR: unstacking on exception return caused data bus fault",   // B11
  "STKERR: stacking on exception entry caused data bus fault",        // B12
  "",                                                                 // B13
  "",                                                                 // B14
  "BFARVALID: BFAR contains valid fault address",                     // B15

  // Usage faults
  "UNDEFINSTR: tried to execute an undefined instruction",            // B16
  "INVSTATE: invalid EPSR - e.g., tried to switch to ARM mode",       // B17
  "INVPC: exception return integrity checks failed",                  // B18
  "NOCP: attempted to execute a coprocessor instruction",             // B19
  "",                                                                 // B20
  "",                                                                 // B21
  "",                                                                 // B22
  "",                                                                 // B23
  "UNALIGNED: attempted an unaligned memory access",                  // B24
  "DIVBYZERO: attempted to execute SDIV or UDIV with divisor of 0"    // B25
};

static PGM_P PGM afsrBits[] =
{
  "",                                                                 // B0
  "RESERVED: attempted access past last peripheral register address", // B1
  "PROTECTED: attempted user mode write to privileged peripheral",    // B2
  "WRONGSIZE: attempted 8/16-bit access to peripheral register"       // B3
};

static PGM_P PGM intActiveBits[] =
{
  "Timer1",       // B0
  "Timer2",       // B1
  "Management",   // B2
  "Baseband",     // B3
  "Sleep_Timer",  // B4
  "SC1",          // B5
  "SC2",          // B6
  "Security",     // B7
  "MAC_Timer",    // B8
  "MAC_TX",       // B9
  "MAC_RX",       // B10
  "ADC",          // B11
  "IRQ_A",        // B12
  "IRQ_B",        // B13
  "IRQ_C",        // B14
  "IRQ_D",        // B15
  "Debug"         // B16
};

// Names of raw crash data items - each name is null terminated, and the
// end of the array is flagged by two null bytes in a row.
// NOTE: the order of these names must match HalCrashInfoType members.
static const char nameStrings[] = "R0\0R1\0R2\0R3\0"
                                  "R4\0R5\0R6\0R7\0"
                                  "R8\0R9\0R10\0R11\0"
                                  "R12\0R13(LR)\0MSP\0PSP\0"
                                  "PC\0xPSR\0MSP used\0PSP used\0"
                                  "CSTACK bottom\0ICSR\0SHCSR\0INT_ACTIVE\0"
                                  "CFSR\0HFSR\0DFSR\0MMAR/BFAR\0AFSR\0"
                                  "Ret0\0Ret1\0Ret2\0Ret3\0"
                                  "Ret4\0Ret5\0Dat0\0Dat1\0";

//------------------------------------------------------------------------------
// Forward Declarations

//------------------------------------------------------------------------------
// Functions

void halPrintCrashSummary(uint8_t port)
{
  HalCrashInfoType *c = &halResetInfo.crash;
  uint32_t sp, stackBegin, stackEnd, size, used;
  uint16_t pct;
  uint8_t *mode;
  const char *stack;
  uint8_t bit;

  if (c->LR & 4) {
    stack = "process";
    sp = c->processSP;
    used = c->processSPUsed;
    stackBegin = c->processSP;
    stackEnd = c->processSP;
  } else {
    stack = "main";
    sp = c->mainSP;
    used = c->mainSPUsed;
    stackBegin = (uint32_t)c->mainStackBottom;
    stackEnd = (uint32_t)_CSTACK_SEGMENT_END;
  }

  mode = (uint8_t *)((c->LR & 8) ? "Thread" : "Handler");
  size = stackEnd - stackBegin;
  pct = size ? (uint16_t)( ((100 * used) + (size / 2)) / size) : 0;
  emberSerialPrintfLine(port, "%p mode using %p stack (%4x to %4x), SP = %4x",
                        mode, stack, stackBegin, stackEnd, sp);
  emberSerialPrintfLine(port, "%u bytes used (%u%%) in %p stack"
                        " (out of %u bytes total)",
                        (uint16_t)used, pct, stack, (uint16_t)size);
  if ( !(c->LR & 4) && (used == size - 4*RESETINFO_WORDS)
       && (c->mainStackBottom < (uint32_t) _RESETINFO_SEGMENT_END) ) {
    // Here the stack overlaps the RESETINFO region and when we checked
    // stack usage we avoided checking that region because we'd already
    // started using it -- so if we found the stack almost full to that
    // point, warn that it might actually have been completely full.
    emberSerialPrintfLine(port, "Stack _may_ have used up to 100%% of total.");
  }
  // Valid SP range is [stackBegin, stackEnd] inclusive, but contents
  // of stack only go into [stackBegin, stackend).
  if ((sp > stackEnd) || (sp < stackBegin)) {
    emberSerialPrintfLine(port, "SP is outside %p stack range!", stack);
  }
  emberSerialWaitSend(port);
  if (c->intActive.word) {
    emberSerialPrintf(port, "Interrupts active (or pre-empted and stacked):");
    for (bit = INT_TIM1_BIT; bit <= INT_DEBUG_BIT; bit++) {
      if ( (c->intActive.word & (1 << bit)) && *intActiveBits[bit] ) {
        emberSerialPrintf(port, " %p", intActiveBits[bit]);
      }
    }
    emberSerialPrintCarriageReturn(port);
  } else {
    emberSerialPrintfLine(port, "No interrupts active");
  }
}

void halPrintCrashDetails(uint8_t port)
{
  HalCrashInfoType *c = &halResetInfo.crash;
  uint16_t reason = halGetExtendedResetInfo();
  uint8_t bit;
  const char *chan;

  switch (reason) {

  case RESET_WATCHDOG_EXPIRED:
    emberSerialPrintfLine(port, "Reset cause: Watchdog expired, no reliable extra information");
    emberSerialWaitSend(port);
    break;
  case RESET_WATCHDOG_CAUGHT:
    emberSerialPrintfLine(port, "Reset cause: Watchdog caught with enhanced info");
    emberSerialPrintfLine(port, "Instruction address: %4x", c->PC);
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_PROTDMA:
    switch (c->data.dmaProt.channel){
    case DMA_PROT_CH_SC1_RX:
      chan = "SC1 Rx";
      break;
    case DMA_PROT_CH_GP_ADC:
      chan = "ADC";
      break;
    case DMA_PROT_CH_MAC:
      chan = "MAC Rx";
      break;
    case DMA_PROT_CH_SC2_RX:
      chan = "SC2 Rx";
      break;
    default:
      chan = "??";
      break;
    }
    emberSerialPrintfLine(port, "Reset cause: DMA protection violation");
    emberSerialPrintfLine(port, "DMA: %p, address: %4x",
                          chan, c->data.dmaProt.address);
    emberSerialWaitSend(port);
    break;

  case RESET_CRASH_ASSERT:
    emberSerialPrintfLine(port, "Reset cause: Assert %p:%d",
          c->data.assertInfo.file, c->data.assertInfo.line);
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_HARD:
    emberSerialPrintfLine(port, "Reset cause: Hard Fault");
    if (c->hfsr.bits.VECTTBL) {
      emberSerialPrintfLine(port,
              "HFSR.VECTTBL: error reading vector table for an exception");
    }
    if (c->hfsr.bits.FORCED) {
      emberSerialPrintfLine(port,
              "HFSR.FORCED: configurable fault could not activate");
    }
    if (c->hfsr.bits.DEBUGEVT) {
      emberSerialPrintfLine(port,
              "HFSR.DEBUGEVT: fault related to debug - e.g., executed BKPT");
    }
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_MEM:
    emberSerialPrintfLine(port, "Reset cause: Memory Management Fault");
    if (c->cfsr.word & (SCS_CFSR_DACCVIOL_MASK | SCS_CFSR_IACCVIOL) ) {
      emberSerialPrintfLine(port, "Instruction address: %4x", c->PC);
    }
    if (c->cfsr.bits.MMARVALID) {
      emberSerialPrintfLine(port, "Illegal access address: %4x", c->faultAddress);
    }
    for (bit = SCS_CFSR_IACCVIOL_BIT; bit <= SCS_CFSR_MMARVALID_BIT; bit++) {
      if ( (c->cfsr.word & (1 << bit)) && *cfsrBits[bit] ) {
        emberSerialPrintfLine(port, "CFSR.%p", cfsrBits[bit]);
      }
    }
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_BUS:
    emberSerialPrintfLine(port, "Reset cause: Bus Fault");
    emberSerialPrintfLine(port, "Instruction address: %4x", c->PC);
    if (c->cfsr.bits.IMPRECISERR) {
      emberSerialPrintfLine(port,
        "Address is of an instruction after bus fault occurred, not the cause.");
    }
    if (c->cfsr.bits.BFARVALID) {
      emberSerialPrintfLine(port, "Illegal access address: %4x",
                            c->faultAddress);
    }
    for (bit = SCS_CFSR_IBUSERR_BIT; bit <= SCS_CFSR_BFARVALID_BIT; bit++) {
      if ( (c->cfsr.word & (1 << bit)) && *cfsrBits[bit] ) {
        emberSerialPrintfLine(port, "CFSR.%p", cfsrBits[bit]);
      }
    }
    if ( (c->cfsr.word & 0xFF) == 0) {
      emberSerialPrintfLine(port, "CFSR.(none) load or store at an illegal address");
    }
    for (bit = SCS_AFSR_RESERVED_BIT; bit <= SCS_AFSR_WRONGSIZE_BIT; bit++) {
      if ( (c->afsr.word & (1 << bit)) && *afsrBits[bit] ) {
        emberSerialPrintfLine(port, "AFSR.%p", afsrBits[bit]);
      }
    }
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_USAGE:
    emberSerialPrintfLine(port, "Reset cause: Usage Fault");
    emberSerialPrintfLine(port, "Instruction address: %4x", c->PC);
    for (bit = SCS_CFSR_UNDEFINSTR_BIT; bit <= SCS_CFSR_DIVBYZERO_BIT; bit++) {
      if ( (c->cfsr.word & (1 << bit)) && *cfsrBits[bit] ) {
        emberSerialPrintfLine(port, "CFSR.%p", cfsrBits[bit]);
      }
    }
    emberSerialWaitSend(port);
    break;

  case RESET_FAULT_DBGMON:
    emberSerialPrintfLine(port, "Reset cause: Debug Monitor Fault");
    emberSerialPrintfLine(port, "Instruction address: %4x", c->PC);
    emberSerialWaitSend(port);
    break;

  default:
    break;
  }
}

// Output an array of 32 bit values, 4 per line, each preceded by its name.
void halPrintCrashData(uint8_t port)
{
  uint32_t *data = (uint32_t *)&halResetInfo.crash.R0;
  char const *name = nameStrings;
  char const *separator;
  uint8_t i;

  for (i = 0; *name; i++) {
    emberSerialPrintf(port, "%p = %4x", name, *data++);
    while (*name++) {}  // intentionally empty while loop body
    /*lint -save -e448 */
    separator = (*name && ((i & 3) != 3) ) ? ", " : "\r\n";
    /*lint -restore */
    emberSerialPrintf(port, separator);
    emberSerialWaitSend(port);
  }
}

uint16_t halGetPCDiagnostics( void )
{
  return 0;
}

void halStartPCDiagnostics( void )
{
}

void halStopPCDiagnostics( void )
{
}
