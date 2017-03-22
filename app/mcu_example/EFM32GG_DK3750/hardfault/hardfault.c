/**************************************************************************//**
 * @file
 * @brief Hardfault handler for Cortex-M3
 * @author Joseph Yiu, Frank Van Hooft, Silicon Labs
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdio.h>
#include "hardfault.h"
#include "em_device.h"

/**************************************************************************//**
 * @brief Enable trapping of divison by zero
 *****************************************************************************/
void HardFault_TrapDivByZero(void)
{
  volatile uint32_t *confctrl = (uint32_t *) 0xE000ED14;

  *confctrl |= (1<<4);
}


/**************************************************************************//**
 * @brief  Enable trapping of unaligned memory accesses
 *****************************************************************************/
void HardFault_TrapUnaligned(void)
{
  volatile uint32_t *confctrl = (uint32_t *) 0xE000ED14;

  *confctrl |= (1<<3);
}

/**************************************************************************//**
 * @brief  Exception handler for Cortex-M3 hard faults
 * @note This code is from http://blog.frankvh.com/, based on Joseph Yiu's
 *       hardfault code. For Keil MDKARM, this assembly code needs to be added
 *       as a separate assembly function, as the RVDS compiler cannot do inline
 *       assembly of thumb instructions required for Cortex-M3.
 *****************************************************************************/
#if defined(__GNUC__)
void HardFault_Handler(void) __attribute__ (( naked ));
#endif
void HardFault_Handler(void)
{
  /*
   * Get the appropriate stack pointer, depending on our mode,
   * and use it as the parameter to the C handler. This function
   * will never return
   */
  __asm("TST   LR, #4          \n"
        "ITE   EQ              \n"
        "MRSEQ R0, MSP         \n"
        "MRSNE R0, PSP         \n"
        "B HardFault_HandlerC  \n");
}

/**************************************************************************//**
 * @brief Exception handler for Cortex-M3 hard faults
 * @param[in] hardfault_args Stack frame location
 * @note  From Joseph Yiu, minor edits by FVH and Silicon Labs AS.
 *        Hard fault handler in C
 *****************************************************************************/
void HardFault_HandlerC(uint32_t *stack_pointer)
{
  uint32_t stacked_r0;
  uint32_t stacked_r1;
  uint32_t stacked_r2;
  uint32_t stacked_r3;
  uint32_t stacked_r12;
  uint32_t stacked_lr;
  uint32_t stacked_pc;
  uint32_t stacked_psr;

  stacked_r0 = ((uint32_t) stack_pointer[0]);
  stacked_r1 = ((uint32_t) stack_pointer[1]);
  stacked_r2 = ((uint32_t) stack_pointer[2]);
  stacked_r3 = ((uint32_t) stack_pointer[3]);

  stacked_r12 = ((uint32_t) stack_pointer[4]);
  stacked_lr =  ((uint32_t) stack_pointer[5]);
  stacked_pc =  ((uint32_t) stack_pointer[6]);
  stacked_psr = ((uint32_t) stack_pointer[7]);

  printf("\n\n[HardFault]\n");
  printf("R0        = %08x\n", (unsigned int)stacked_r0);
  printf("R1        = %08x\n", (unsigned int)stacked_r1);
  printf("R2        = %08x\n", (unsigned int)stacked_r2);
  printf("R3        = %08x\n", (unsigned int)stacked_r3);
  printf("R12       = %08x\n", (unsigned int)stacked_r12);
  printf("LR [R14]  = %08x - Subroutine Call return address\n", (unsigned int)stacked_lr);
  printf("PC [R15]  = %08x - Program Counter\n", (unsigned int)stacked_pc);
  printf("PSR       = %08x\n", (unsigned int)stacked_psr);
  printf("BFAR      = %08x - Bus Fault SR/Address causing bus fault\n",
         (unsigned int) (*((volatile uint32_t *)(0xE000ED38))));
  printf("CFSR      = %08x - Config. Fault SR\n",
         (unsigned int) (*((volatile uint32_t *)(0xE000ED28))));
  if((*((volatile uint32_t *)(0xE000ED28)))&(1<<25))
  {
    printf("  :UsageFault->DivByZero\n");
  }
  if((*((volatile uint32_t *)(0xE000ED28)))&(1<<24))
  {
    printf("  :UsageFault->Unaligned access\n");
  }
  if((*((volatile uint32_t *)(0xE000ED28)))&(1<<18))
  {
    printf("  :UsageFault->Integrity check error\n");
  }
  if((*((volatile uint32_t *)(0xE000ED28)))&(1<<0))
  {
    printf("  :MemFault->Data access violation\n");
  }
  if((*((volatile uint32_t *)(0xE000ED28)))&(1<<0))
  {
    printf("  :MemFault->Instruction access violation\n");
  }
  printf("HFSR      = %08x - Hard Fault SR\n",
         (unsigned int)(*((volatile uint32_t *)(0xE000ED2C))));
  if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<1))
  {
    printf("  :VECTBL, Failed vector fetch\n");
  }
  if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<30))
  {
    printf("  :FORCED, Bus fault/Memory management fault/usage fault\n");
  }
  if((*((volatile uint32_t *)(0xE000ED2C)))&(1UL<<31))
  {
    printf("  :DEBUGEVT, Hard fault triggered by debug event\n");
  }
  printf("DFSR      = %08x - Debug Fault SR\n", (unsigned int)(*((volatile uint32_t *)(0xE000ED30))));
  printf("MMAR      = %08x - Memory Manage Address R\n", (unsigned int)(*((volatile uint32_t *)(0xE000ED34))));
  printf("AFSR      = %08x - Auxilirary Fault SR\n", (unsigned int)(*((volatile uint32_t *)(0xE000ED3C))));
  printf("SCB->SHCSR= %08x - System Handler Control and State R (exception)\n", (unsigned int)SCB->SHCSR);

  while(1);
}
