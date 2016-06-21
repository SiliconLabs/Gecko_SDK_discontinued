/***************************************************************************//**
 * @file    iec60335_class_b_ram_test.c
 * @brief   c based RAM test
 *             POST and BIST RAM test for all compiler
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "em_device.h"
#include "iec60335_class_b.h"

/*! @addtogroup IEC60335_RAM_TEST
 * @{
 */

/* local definitions */
#if     (defined (__CC_ARM))
/*! @cond RealView */
/*------------------RealView Compiler -----------------*/
#define getSP()    __current_sp()
/*! @endcond */

#elif   (defined (__ICCARM__))
/*! @cond ICC */
/*------------------ ICC Compiler -------------------*/
#define getSP()    __get_MSP()
/*! @endcond */

#elif   (defined (__GNUC__))
/*! @cond GCC */
/*------------------ GNU Compiler ---------------------*/
#define getSP()    __get_MSP()
/*! @endcond */
#else
#error "iec60355_class_b_interrupt_test.c: undefined compiler"
#endif


/* 32-bit word access macro */
#define WA(ptr)    * ((volatile uint32_t *) ptr)


#ifdef TESSY
/*! @cond TESSY */
void TS_TessyDummy(volatile uint8_t* ts_ptr);
#define __st(x)      {x;}
#define MOP_R0()     __st(TS_TessyDummy(ptr); if (WA(ptr) != r0_exp) goto R0_FAULT_DETECTED;)
#define MOP_R1()     __st(TS_TessyDummy(ptr); if (WA(ptr) != r1_exp) goto R1_FAULT_DETECTED;)
/*! @endcond */
#else
/* local macros */
/* March memory operations */
#define __st(x)      do { x; } while (0)
#define MOP_R0()     __st(if (WA(ptr) != r0_exp) goto R0_FAULT_DETECTED;)
#define MOP_R1()     __st(if (WA(ptr) != r1_exp) goto R1_FAULT_DETECTED;)
#endif  /* TESSY */

#define MOP_W0()     __st(WA(ptr) = r0_exp;)
#define MOP_W1()     __st(WA(ptr) = r1_exp;)
#define MOP_DEL()    __NOP()

testResult_t IEC60335_ClassB_RAMtest(uint32_t startAddrs, uint32_t size, uint8_t block_sel)
{
  testResult_t result = IEC60335_testFailed;

  /* Note that function calls cannot be used from this function as the */
  /* stack is destroyed. */

  /* Use a byte pointer to increment through the RAM blocks. */
  /* The byte pointer is cast into 32-bit word accesses to generate both */
  /* aligned and unaligned accesses. The unaligned accesses are split into */
  /* multiple 8- and 16-bit accesses by the CM3 bus matrix. That way, we are */
  /* also testing for individual byte WE faults in the RAM IP and address */
  /* decoder faults. */
  register volatile uint8_t *ptr;
  register uint32_t         offset;
  volatile uint8_t          *ram_base_init;
  volatile uint8_t          *ram_end_init;
  register volatile uint8_t *ram_base;
  register volatile uint8_t *ram_end;
  register uint32_t         r0_exp;
  register uint32_t         r1_exp;
  uint32_t                  currentSP;
  register uint32_t         segment_size2;
  uint32_t                  stackSize;

  /* Set RAM block boundary pointers */
  switch (block_sel)
  {
  case BLOCK_SEL_CORE:
    ram_base_init    = (volatile uint8_t *) startAddrs;
    ram_end_init     = ram_base_init + size;
    r0_exp           = CORE_RAM_R0_EXP;
    r1_exp           = CORE_RAM_R1_EXP;
    break;

  case BLOCK_SEL_PLINK:
    ram_base_init    = PLINK_RAM_BASE_PTR;
    ram_end_init     = (volatile uint8_t *)(PLINK_RAM_BASE_ADDR + PLINK_RAM_MAX_BC);
    r0_exp           = PLINK_RAM_R0_EXP;
    r1_exp           = PLINK_RAM_R1_EXP;
    CMU->HFPERCLKEN0 = ~0;
    break;

  default:
    goto BOUNDARY_FAULT_DETECT;                               /* error condition */
    /*break;*/
  }
  do {
    segment_size2    = 0;
    if (block_sel == BLOCK_SEL_CORE)
    {
      currentSP = getSP();
      stackSize = initial_sp - currentSP;

      /* test boundaries */
      if (((uint32_t)ram_base_init < RAM_MEM_BASE)||((uint32_t)ram_end_init > (RAM_MEM_BASE + RAM_MEM_SIZE)))
      {
        goto BOUNDARY_FAULT_DETECT;
      }
      if (ram_base_init > ram_end_init)
      {
        goto BOUNDARY_FAULT_DETECT;
      }
      if ((currentSP < RAM_MEM_BASE)||(currentSP > initial_sp)||(stackSize > RAM_MEM_SIZE))
      {
        goto BOUNDARY_FAULT_DETECT;
      }
      if ((size%4 != 0) || (size == 0))
      {
        goto BOUNDARY_FAULT_DETECT;
      }

      /* check stack end in tested RAM area */
      if ((currentSP > (uint32_t) ram_base_init) && (currentSP < (uint32_t) ram_end_init))
      {
        /* check SP at end of tested RAM section */
        if (initial_sp >= ((uint32_t)ram_end_init - 0x10))
        {
          /* stack is at the end of the tested area */
          ram_end_init = (uint8_t*)(currentSP - IEC60335_RAM_SP_Offset);
          segment_size2 = 0;
          /* check sufficient size of tested memory */
          if (ram_base_init >= ram_end_init)
          {
            goto BOUNDARY_FAULT_DETECT;
          }
        }
        else /* there is a gap behind the stack */
        {
          segment_size2 = (uint32_t)ram_end_init - initial_sp;    /* next segment to check */
          ram_end_init = (uint8_t*)(currentSP - IEC60335_RAM_SP_Offset);
        }
      }
      /* check stack start in tested RAM area */
      else if ((initial_sp > (uint32_t) ram_base_init) && (initial_sp < (uint32_t) ram_end_init))
      {
        /* stack is at the start of the tested area */
        ram_base_init = (uint8_t*)initial_sp + 4;
        segment_size2 = 0;
        /* check sufficient size of tested memory */
        if (ram_base_init >= ram_end_init)
        {
          goto BOUNDARY_FAULT_DETECT;
        }
      }
    }
    for (offset = 0; offset < sizeof(uint32_t); offset++)
    {
      /* Offset the byte boundary pointer to generate all combinations of aligned and unaligned access to the Core RAM. */
      /* The PLINK RAM is arranged as bytes aligned to 32-bit word addresses. Hence, skip offset 1, 2 and 3 for the PLINK RAM. */
      if (offset != 0)
      {
        if (block_sel != BLOCK_SEL_CORE)
          break;
        ram_base = (volatile uint8_t *)((uint32_t) ram_base_init + offset);
        ram_end  = (volatile uint8_t *)((uint32_t) ram_end_init + offset - sizeof(uint32_t));       /* do not exceed the last byte on unaligned access */
      }
      else
      {
        ram_base = ram_base_init;
        ram_end  = ram_end_init;
      }

      /* March element M0: <->(w0), using -> address order */
      ptr = ram_base;
      while (ptr < ram_end)
      {
        MOP_W0();
        ptr = ptr + sizeof(uint32_t);
      }

      /* March element M1: ->(r0,w1) */
      ptr = ram_base;
      while (ptr < ram_end)
      {
        MOP_R0();
        MOP_W1();
        ptr = ptr + sizeof(uint32_t);
      }

      /* March element M2: <-(r1,w0,r0,w1) */
      ptr = (ram_end - sizeof(uint32_t));
      while (ptr >= ram_base)
      {
        MOP_R1();
        MOP_W0();
        MOP_R0();
        MOP_W1();
        ptr = ptr - sizeof(uint32_t);
      }

      /* March element M3: <-(r1,w0) */
      ptr = (ram_end - sizeof(uint32_t));
      while (ptr >= ram_base)
      {
        MOP_R1();
        MOP_W0();
        ptr = ptr - sizeof(uint32_t);
      }

      /* March element M4: <-(r0,w1,r1,w0) */
      ptr = (ram_end - sizeof(uint32_t));
      while (ptr >= ram_base)
      {
        MOP_R0();
        MOP_W1();
        MOP_R1();
        MOP_W0();
        ptr = ptr - sizeof(uint32_t);
      }

      /* March element M5: <-(r0) */
      ptr = (ram_end - sizeof(uint32_t));
      while (ptr >= ram_base)
      {
        MOP_R0();
        ptr = ptr - sizeof(uint32_t);
      }

      /* March element M6: Del */
      MOP_DEL();

      /* March element M7: <->(r0,w1,r1), using -> address order */
      ptr = ram_base;
      while (ptr < ram_end)
      {
        MOP_R0();
        MOP_W1();
        MOP_R1();
        ptr = ptr + sizeof(uint32_t);
      }

      /* March element M8: Del */
      MOP_DEL();

      /* March element M9: <->(r1), using <- address order */
      ptr = (ram_end - sizeof(uint32_t));
      while (ptr >= ram_base)
      {
        MOP_R1();
        ptr = ptr - sizeof(uint32_t);
      }
    }
    if (segment_size2 != 0)
    {
      ram_base_init = (volatile uint8_t *) initial_sp + 4;
      ram_end_init  = (volatile uint8_t *) initial_sp + segment_size2;
    }
  } while (segment_size2);
  /* No faults found, report OK */
  result = IEC60335_testPassed;
  goto RAM_TEST_END;

 /* Fault return */
 BOUNDARY_FAULT_DETECT:
  result = IEC60335_testFailed;
  goto RAM_TEST_END;

 R0_FAULT_DETECTED:
  result = IEC60335_testFailed;
  goto RAM_TEST_END;

 R1_FAULT_DETECTED:
  result = IEC60335_testFailed;

 RAM_TEST_END:
  return(result);
}

testResult_t IEC60335_ClassB_RAMtest_POST(void)
{
  return ((testResult_t)IEC60335_ClassB_RAMtest(CORE_RAM_BASE_ADDR, IEC60335_RAM_SIZE, BLOCK_SEL_CORE));
}

testResult_t IEC60335_ClassB_RAMtest_BIST(uint32_t startAddr, uint32_t length)
{
  uint8_t      buffer[IEC60335_RAM_buffersize];
  testResult_t result = IEC60335_testFailed;

/* check StartAddr in RAM space */
  if (startAddr > (IEC60335_RAM_START + IEC60335_RAM_SIZE - 4))
  {
    return(result);
  }
  if (startAddr < IEC60335_RAM_START)
  {
    return(result);
  }
/* limit size to buffer size */
  if (length > IEC60335_RAM_buffersize)
  {
    length = IEC60335_RAM_buffersize;
  }
  if (length == 0)
  {
    return(result);
  }
/* limit endaddress to RAM space */
  if ((startAddr + length) > (IEC60335_RAM_START + IEC60335_RAM_SIZE - 4))
  {
    length = IEC60335_RAM_START + IEC60335_RAM_SIZE - 4 - startAddr;
  }
/* check buffer overlaps test area low */
  if (((uint32_t) &buffer[0] > startAddr) && ((uint32_t) &buffer[0] < (startAddr + length)))
  {
    return(result);
  }
/* check buffer overlaps test area high */
  if (((uint32_t) &buffer[IEC60335_RAM_buffersize - 1] > startAddr) &&
      ((uint32_t) &buffer[IEC60335_RAM_buffersize - 1] < (startAddr + length)))
  {
    return(result);
  }
/* save memory content */
  util_memcpy(&buffer[0], &startAddr, length);

  result = IEC60335_ClassB_RAMtest(startAddr, length, BLOCK_SEL_CORE);
/* restore memory content */
  util_memcpy(&startAddr, &buffer[0], length);
  return(result);
}

/*!
 * @}
 */

/************************************** EOF *********************************/
