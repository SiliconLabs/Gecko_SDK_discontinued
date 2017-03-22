/** @file hal/micro/cortexm3/em35x/regs.h
 *
 * @brief
 * This file pulls in the appropriate register
 * headers based on the specific Cortex-M3 being
 * compiled.
 *
 * THIS IS A GENERATED FILE.  DO NOT EDIT.
 *
 * <!-- Copyright 2016 Silicon Laboratories, Inc.                        *80*-->
 */

#if defined(CORTEXM3_EM317)
  #include "em317/regs.h"
#elif defined(CORTEXM3_EM341)
  #include "em341/regs.h"
#elif defined(CORTEXM3_EM342)
  #include "em342/regs.h"
#elif defined(CORTEXM3_EM346)
  #include "em346/regs.h"
#elif defined(CORTEXM3_EM351)
  #include "em351/regs.h"
#elif defined(CORTEXM3_EM355)
  #include "em355/regs.h"
#elif defined(CORTEXM3_EM3555)
  #include "em3555/regs.h"
#elif defined(CORTEXM3_EM356)
  #include "em356/regs.h"
#elif defined(CORTEXM3_EM357)
  #include "em357/regs.h"
#elif defined(CORTEXM3_EM3581)
  #include "em3581/regs.h"
#elif defined(CORTEXM3_EM3582)
  #include "em3582/regs.h"
#elif defined(CORTEXM3_EM3585)
  #include "em3585/regs.h"
#elif defined(CORTEXM3_EM3586)
  #include "em3586/regs.h"
#elif defined(CORTEXM3_EM3587)
  #include "em3587/regs.h"
#elif defined(CORTEXM3_EM3588)
  #include "em3588/regs.h"
#elif defined(CORTEXM3_EM359)
  #include "em359/regs.h"
#elif defined(CORTEXM3_EM3591)
  #include "em3591/regs.h"
#elif defined(CORTEXM3_EM3592)
  #include "em3592/regs.h"
#elif defined(CORTEXM3_EM3595)
  #include "em3595/regs.h"
#elif defined(CORTEXM3_EM3596)
  #include "em3596/regs.h"
#elif defined(CORTEXM3_EM3597)
  #include "em3597/regs.h"
#elif defined(CORTEXM3_EM3598)
  #include "em3598/regs.h"
#elif defined(CORTEXM3_SKY66107)
  #include "sky66107/regs.h"
#else
  #error Unknown Cortex-M3 micro
#endif
