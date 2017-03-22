#********************************************************************************************************
#                                                uC/CPU
#                                    CPU CONFIGURATION & PORT LAYER
#
#                          (c) Copyright 2004-2016; Micrium, Inc.; Weston, FL
#
#               All rights reserved.  Protected by international copyright laws.
#
#               uC/CPU is provided in source form to registered licensees ONLY.  It is
#               illegal to distribute this source code to any third party unless you receive
#               written permission by an authorized Micrium representative.  Knowledge of
#               the source code may NOT be used to develop a similar product.
#
#               Please help us continue to provide the Embedded community with the finest
#               software available.  Your honesty is greatly appreciated.
#
#               You can find our product's user manual, API reference, release notes and
#               more information at https://doc.micrium.com.
#               You can contact us at www.micrium.com.
#
# File          : cpu_cache_powerpc_e200z4204n3_a.S
# Version       : V1.31.00
# For           : PowerPC e200z4204n3
# Toolchain     : GNU
# Programmer(s) : SB
#********************************************************************************************************


#********************************************************************************************************
#                                           PUBLIC FUNCTIONS
#********************************************************************************************************

    .global     CPU_DCache_LineSizeGet
    .global     CPU_DCache_RangeInv
    .global     CPU_DCache_RangeFlush


#********************************************************************************************************
#                                                EXTERNS
#********************************************************************************************************

    .extern     CPU_Cache_Linesize


#********************************************************************************************************
#                                           MACROS AND DEFINIITIONS
#********************************************************************************************************

    .equ        L1CFG0,     515


#********************************************************************************************************
#                                      CODE GENERATION DIRECTIVES
#********************************************************************************************************

    .section    .text


#********************************************************************************************************
#                                           CPU_DCache_LineSizeGet()
#
# Description : Returns the cache line size.
#
# Prototypes  : void  CPU_DCache_LineSizeGet (void)
#
# Argument(s) : none.
#********************************************************************************************************

CPU_DCache_LineSizeGet:
    mfspr       r0, L1CFG0                                      # CBSIZE is in L1CFG0[39-40]
    e_rlwinm    r0, r0, 9, 30, 31                               # Move CBSIZE into R0[62-63] and mask other bits to zero

    e_li        r3, 32                                          # Line size is 32 << CBSIZE, 0 <= CBSIZE <= 2
    e_rlw       r3, r3, r0                                      # ...

    se_blr                                                      # Return the line size in R3


#********************************************************************************************************
#                                      INVALIDATE DATA CACHE RANGE
#
# Description : Invalidate a range of data cache.
#
# Prototypes  : void  CPU_DCache_RangeInv  (void       *p_mem,
#                                           CPU_SIZE_T  range);
#
# Argument(s) : p_mem    Start address of the region to invalidate.
#
#               range    Size of the region to invalidate in bytes.
#
# Note(s)     : none.
#********************************************************************************************************

CPU_DCache_RangeInv:
    e_cmp16i    r4, 0                                           # If the range is 0, do nothing
    e_beq       InvReturn                                       # ...

    mfmsr       r0                                              # Save MSR and disable interrupts
    wrteei      0
    sync                                                        # Complete data operations before invalidation

    e_lis       r11, CPU_Cache_Linesize@ha                      # Store the line size into R11
    e_lwz       r11, CPU_Cache_Linesize@l(r11)                  # ...

    neg         r12, r11                                        # The line size is a power of 2; form a mask for the lower bits

    neg         r4, r4                                          # Pad the range so it is a multiple of the line size
    and         r4, r4, r12                                     # ...
    neg.        r4, r4                                          # Check if the result underflowed
    e_bne       InvRangeNotOV                                   # If it did not, we are finished with the range

    e_or2is     r4, 0xFFFF                                      # Otherwise, Set the maximum range, aligned to the line size.
    e_or2i      r4, 0xFFFF                                      # ...
    and         r4, r4, r12                                     # ...

InvRangeNotOV:
    and         r3, r3, r12                                     # Align the address to the start of the cache line

    xor         r12, r12, r12                                   # R12 will count how much memory has been invalidated
InvLine:
    cmpw        r12, r4                                         # Have we invalidated the entire range?
    e_beq       InvEnd                                          # If so, we can return

    dcbf        r3, r12                                         # Invalidate the cache line

    add         r12, r12, r11                                   # Move to the next cache line
    se_b        InvLine                                         # ...

InvEnd:
    sync
    wrtee       r0                                              # Restore MSR

InvReturn:
    se_blr


#********************************************************************************************************
#                                       FLUSH DATA CACHE RANGE
#
# Description : Flush (clean) a range of data cache.
#
# Prototypes  : void  CPU_DCache_RangeFlush  (void       *p_mem,
#                                             CPU_SIZE_T  range)
#
# Argument(s) : p_mem    Start address of the region to flush.
#
#               range    Size of the region to invalidate in bytes.
#
# Note(s)     : none.
#********************************************************************************************************

CPU_DCache_RangeFlush:
    e_cmp16i    r4, 0                                           # If the range is 0, do nothing
    e_beq       FlushReturn                                     # ...

    mfmsr       r0                                              # Save MSR and disable interrupts
    wrteei      0
    sync                                                        # Complete data operations before clean

    e_lis       r11, CPU_Cache_Linesize@ha                      # Store the line size into R11
    e_lwz       r11, CPU_Cache_Linesize@l(r11)                  # ...

    neg         r12, r11                                        # The line size is a power of 2; form a mask for the lower bits

    neg         r4, r4                                          # Pad the range so it is a multiple of the line size
    and         r4, r4, r12                                     # ...
    neg.        r4, r4                                          # Check if the result underflowed
    e_bne       FlushRangeNotOV                                 # If it did not, we are finished with the range

    e_or2is     r4, 0xFFFF                                      # Otherwise, Set the maximum range, aligned to the line size.
    e_or2i      r4, 0xFFFF                                      # ...
    and         r4, r4, r12                                     # ...

FlushRangeNotOV:
    and         r3, r3, r12                                     # Align the address to the start of the cache line

    xor         r12, r12, r12                                   # R12 will count how much memory has been cleaned
FlushLine:
    cmpw        r12, r4                                         # Have we cleaned the entire range?
    e_beq       FlushEnd                                        # If so, we can return

    dcbst       r3, r12                                         # Clean the cache line

    add         r12, r12, r11                                   # Move to the next cache line
    se_b        FlushLine                                       # ...

FlushEnd:
    sync
    wrtee       r0                                              # Restore MSR

FlushReturn:
    se_blr
