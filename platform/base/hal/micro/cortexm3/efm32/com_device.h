#if (defined CORTEXM3_EZR32LG)
  #include "ezr32lg/com_device.h"
#elif (defined CORTEXM3_EZR32WG)
  #include "ezr32wg/com_device.h"
#elif (defined CORTEXM3_EZR32HG)
  #include "ezr32hg/com_device.h"
#elif defined(CORTEXM3_EFR32_MICRO)
  #include "efr32/com_device.h"
#endif
