/** @file hal/micro/cortexm3/ext-device.c
 *  @brief External Device GPIO driver
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#include PLATFORM_HEADER
#include "stack/include/ember-types.h"
#include "stack/include/error.h"
#include "hal/hal.h"

#include "ext-device.h"         // Implement this API


#ifdef  ENABLE_EXT_DEVICE       // Driver is enabled

//== DRIVER CONFIGURATION ==

/** @brief API to access External Device GPIOs configured by BOARD_HEADER:
 *  - EXT_DEVICE_PWR  Power signal
 *  - EXT_DEVICE_RDY  Ready signal
 *  - EXT_DEVICE_SEL  Select signal
 *  - EXT_DEVICE_INT  Interrupt signal
 *  It is not necessary to provide any which don't apply to the design.
 *  Each one requires 4 definitions: itself, two helpers (_PORT and _BIT
 *  which make for efficient code), and its true-asserted value (polarity).
 *  E.g.:
 *      #define EXT_DEVICE_INT          PORTB_PIN(6)
 *      #define EXT_DEVICE_INT_PORT     B
 *      #define EXT_DEVICE_INT_BIT      6
 *      #define EXT_DEVICE_INT_TRUE     0 // 0=low true, 1=high true
 *  For EXT_DEVICE_INT, and optionally EXT_DEVICE_RDY if want a distinct
 *  ready interrupt, must additionally provide the IRQx selection and
 *  the settings for its GPIO_INTCFGx register (GPIO_INTFILT and
 *  GPIO_INT_MOD settings):
 *      #define EXT_DEVICE_INT_IRQ      B   // A,B,C,D are the valid IRQs
 *      #define EXT_DEVICE_INT_IRQCFG   (GPIOINTMOD_FALLING_EDGE)
 */


//== DEFINITIONS ==

// Some handy-dandy preprocessor magic
#define PASTE(a,b,c)       a##b##c
#define EVAL3(a,b,c)       PASTE(a,b,c)

//-- GPIO access for signals
#define GPIO_INP(port,bit)  ( !!(EVAL3(GPIO_P,port,IN)  & EVAL3(PA,bit,_MASK)) )
#define GPIO_CLR(port,bit)  (    EVAL3(GPIO_P,port,CLR) = EVAL3(PA,bit,_MASK)  )
#define GPIO_SET(port,bit)  (    EVAL3(GPIO_P,port,SET) = EVAL3(PA,bit,_MASK)  )

//-- IRQx access for interrupts
#define halIrqxIsr(irq)     EVAL3(halIrq,irq,Isr)       // IRQ routine name
#define GPIO_INTCFGx(irq)   EVAL3(GPIO_INTCFG,irq,)
#define INT_IRQx(irq)       (EVAL3(INT_IRQ,irq,))       // For INT_CFGCLR/SET
#define INT_IRQxFLAG(irq)   (EVAL3(INT_IRQ,irq,FLAG))   // For INT_GPIOFLAG
#define INT_MISSIRQx(irq)   (EVAL3(INT_MISSIRQ,irq,))   // For INT_MISS
#define IRQx_PRIORITY_REGISTER(irq) \
  (* ( ((uint8_t *)NVIC_IPR_3to0_ADDR) + EVAL3(IRQ,irq,_VECTOR_INDEX) - 16) )


enum { A, B, C, D, MAXIRQ }; // Handy for runtime GPIO_IRQxSEL determination


//== LOCAL STATE ==

static HalExtDeviceIrqCB halExtDeviceIntCB = NULL;
static HalExtDeviceIrqCB halExtDeviceRdyCB = NULL;
static bool halExtDevicePowered = false;
static bool halExtDeviceSelected = false;
static uint8_t halExtDeviceIntLevel = EXT_DEVICE_INT_UNCONFIGURED;


//== HELPER FUNCTIONS ==

/** @brief Configure and enable/disable the device ready IRQ
 */
static void halExtDeviceRdyCfgIrq(void)
{
  #ifdef  EXT_DEVICE_RDY_IRQ
    // The logic here is basically:
    // - If device isn't powered (e.g. at boot), just disable and configure IRQ.
    // - If device is powered then clear out anything stale and enable or
    //   disable IRQ as indicated by its callback.
    if (! halExtDevicePowered) {
      // Start from a fresh state just in case
      GPIO_INTCFGx(EXT_DEVICE_RDY_IRQ) = 0;                 // Disable GPIO IRQx
      INT_CFGCLR   = INT_IRQx(EXT_DEVICE_RDY_IRQ);          // Disable top level
      // Configure interrupt mode
      GPIO_INTCFGx(EXT_DEVICE_RDY_IRQ) = EXT_DEVICE_RDY_IRQCFG;
      // Point IRQC/D at the right pin -- a bit kludgy but avoids warnings
      switch (EXT_DEVICE_RDY_IRQ) {
      case C:
        GPIO_IRQCSEL = EXT_DEVICE_RDY;
        break;
      case D:
        GPIO_IRQDSEL = EXT_DEVICE_RDY;
        break;
      default:
        break;
      }
     #ifdef  EXT_DEVICE_RDY_IRQPRI
      // Need to change the interrupt's priority in the NVIC
      IRQx_PRIORITY_REGISTER(EXT_DEVICE_RDY_IRQ) = EXT_DEVICE_RDY_IRQPRI;
     #endif//EXT_DEVICE_RDY_IRQPRI
    } else {
      // Clear out any stale state
      INT_CFGCLR   = INT_IRQx(EXT_DEVICE_RDY_IRQ);          // Disable top level
      INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_RDY_IRQ);      // Clear 2nd level
      INT_MISS     = INT_MISSIRQx(EXT_DEVICE_RDY_IRQ);      // Clear any missed
      INT_PENDCLR  = INT_IRQx(EXT_DEVICE_RDY_IRQ);          // Clear any pended
      if (halExtDeviceRdyCB != NULL) {
        INT_CFGSET = INT_IRQx(EXT_DEVICE_RDY_IRQ);          // Enable top level
      }
    }
  #endif//EXT_DEVICE_RDY_IRQ
}

/** @brief Configure and enable/disable the device interrupt IRQ
 */
static void halExtDeviceIntCfgIrq(void)
{
  #ifdef  EXT_DEVICE_INT_IRQ
    // The logic here is basically:
    // - If device isn't powered (e.g. at boot), just disable and configure IRQ.
    // - If device is powered then clear out anything stale and enable or
    //   disable IRQ as indicated by its callback.
    if (! halExtDevicePowered) {
      // Start from a fresh state just in case
      GPIO_INTCFGx(EXT_DEVICE_INT_IRQ) = 0;                 // Disable GPIO IRQx
      INT_CFGCLR   = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Disable top level
      // Configure interrupt mode
      GPIO_INTCFGx(EXT_DEVICE_INT_IRQ) = EXT_DEVICE_INT_IRQCFG;
      // Point IRQC/D at the right pin -- a bit kludgy but avoids warnings
      switch (EXT_DEVICE_INT_IRQ) {
      case C:
        GPIO_IRQCSEL = EXT_DEVICE_INT;
        break;
      case D:
        GPIO_IRQDSEL = EXT_DEVICE_INT;
        break;
      default:
        break;
      }
     #ifdef  EXT_DEVICE_INT_IRQPRI
      // Need to change the interrupt's priority in the NVIC
      IRQx_PRIORITY_REGISTER(EXT_DEVICE_INT_IRQ) = EXT_DEVICE_INT_IRQPRI;
     #endif//EXT_DEVICE_INT_IRQPRI
    } else {
      // Clear out any stale state
      INT_CFGCLR   = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Disable top level
      INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_INT_IRQ);      // Clear 2nd level
      INT_MISS     = INT_MISSIRQx(EXT_DEVICE_INT_IRQ);      // Clear any missed
      INT_PENDCLR  = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Clear any pended
      if (halExtDeviceIntCB == NULL) {
        halExtDeviceIntLevel = EXT_DEVICE_INT_UNCONFIGURED;
      } else {
        halExtDeviceIntLevel = EXT_DEVICE_INT_LEVEL_OFF;
        // Callers need to use halExtDeviceIntEnable() to enable top level
      }
    }
  #endif//EXT_DEVICE_INT_IRQ
}


//== INTERNAL ISRS ==

#ifdef  EXT_DEVICE_RDY_IRQ
  void halIrqxIsr(EXT_DEVICE_RDY_IRQ)(void)
  {
   #if     (EXT_DEVICE_RDY_IRQCFG <  (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Acknowledge edge-triggered interrupt before callback, so don't miss edge
    INT_MISS     = INT_MISSIRQx(EXT_DEVICE_RDY_IRQ);        // Clear any missed
    INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_RDY_IRQ);        // Clear 2nd level
   #endif//(EXT_DEVICE_RDY_IRQCFG <  (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Issue callback -- in ISR context
    if (halExtDeviceRdyCB != NULL) {
      (*halExtDeviceRdyCB)();
    }
   #if     (EXT_DEVICE_RDY_IRQCFG >= (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Acknowledge level-triggered interrupt after callback
    INT_MISS     = INT_MISSIRQx(EXT_DEVICE_RDY_IRQ);        // Clear any missed
    INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_RDY_IRQ);        // Clear 2nd level
   #endif//(EXT_DEVICE_RDY_IRQCFG >= (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
  }
#endif//EXT_DEVICE_RDY_IRQ

#ifdef  EXT_DEVICE_INT_IRQ
  void halIrqxIsr(EXT_DEVICE_INT_IRQ)(void)
  {
   #if     (EXT_DEVICE_INT_IRQCFG <  (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Acknowledge edge-triggered interrupt before callback, so don't miss edge
    INT_MISS     = INT_MISSIRQx(EXT_DEVICE_INT_IRQ);        // Clear any missed
    INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_INT_IRQ);        // Clear 2nd level
   #endif//(EXT_DEVICE_INT_IRQCFG <  (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Issue callback -- in ISR context
    if (halExtDeviceIntCB != NULL) {
      (*halExtDeviceIntCB)();
    }
   #if     (EXT_DEVICE_INT_IRQCFG >= (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
    // Acknowledge level-triggered interrupt after callback
    INT_MISS     = INT_MISSIRQx(EXT_DEVICE_INT_IRQ);        // Clear any missed
    INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_INT_IRQ);        // Clear 2nd level
   #endif//(EXT_DEVICE_INT_IRQCFG >= (GPIOINTMOD_HIGH_LEVEL << GPIO_INTMOD_BIT))
  }
#endif//EXT_DEVICE_INT_IRQ


//== API FUNCTIONS ==

//-- External Device Initialization --

/** @brief Initialize External Device GPIOs
 * @param  deviceIntCB The callback routine for device general interrupt
 *                     (NULL to disable)
 * @param  deviceRdyCB The callback routine for device ready interrupt
 *                     (NULL to disable)
 * @return bitmask of configured features for this device
 * @note First initialization after bootup leaves the device powered down
 *       and unselected.  Subsequent inits don't touch the device powered
 *       or selected states and can be used to reconfigure callback(s),
 *       which always clears any stale/pending events.  For deviceRdyCB,
 *       its interrupt is enabled upon configuration; for deviceIntCB,
 *       halExtDeviceIntEnable() must subsequently be called to enable it.
 */
HalExtDeviceConfig halExtDeviceInit(HalExtDeviceIrqCB deviceIntCB,
                                    HalExtDeviceIrqCB deviceRdyCB)
{
  // BOARD_HEADER should already have specified proper GPIO configuration

  // Record (new) ISR information
  UNUSED_VAR(halExtDeviceRdyCB); // Work around potential compiler warnings
  UNUSED_VAR(halExtDeviceIntCB); // Work around potential compiler warnings
  halExtDeviceRdyCB = deviceRdyCB;
  halExtDeviceIntCB = deviceIntCB;
  // Inits while not powered simply ensure device is indeed not powered and
  // not selected.
  // Inits while powered just update the IRQ callbacks.
  if (! halExtDevicePowered) {
    halExtDeviceDeselect();
    halExtDevicePowerDown();
  } else {
    halExtDeviceRdyCfgIrq();
    halExtDeviceIntCfgIrq();
  }

  return ( 0
        #ifdef  EXT_DEVICE_PWR
          | EXT_DEVICE_HAS_PWR
        #endif//EXT_DEVICE_PWR
        #ifdef  EXT_DEVICE_RDY
          | EXT_DEVICE_HAS_RDY
        #ifdef  EXT_DEVICE_RDY_IRQ
          | EXT_DEVICE_HAS_RDY_IRQ
        #endif//EXT_DEVICE_RDY_IRQ
        #endif//EXT_DEVICE_RDY
        #ifdef  EXT_DEVICE_SEL
          | EXT_DEVICE_HAS_SEL
        #endif//EXT_DEVICE_SEL
        #ifdef  EXT_DEVICE_INT
          | EXT_DEVICE_HAS_INT
        #ifdef  EXT_DEVICE_INT_IRQ
          | EXT_DEVICE_HAS_INT_IRQ
        #endif//EXT_DEVICE_INT_IRQ
        #endif//EXT_DEVICE_INT
         );
}

//-- External Device Power --

/** @brief Power down the external device per GPIO
 */
void halExtDevicePowerDown(void)
{
  // Shut off ISRs before removing power, in case of glitches
  halExtDevicePowered = false;
  halExtDeviceRdyCfgIrq();
  halExtDeviceIntCfgIrq();

  #ifdef  EXT_DEVICE_PWR
    // Deassert Power
    #if     (EXT_DEVICE_PWR_TRUE)
      GPIO_CLR(EXT_DEVICE_PWR_PORT, EXT_DEVICE_PWR_BIT);
    #else//!(EXT_DEVICE_PWR_TRUE)
      GPIO_SET(EXT_DEVICE_PWR_PORT, EXT_DEVICE_PWR_BIT);
    #endif//(EXT_DEVICE_PWR_TRUE)
  #endif//EXT_DEVICE_PWR
}

/** @brief Power up the external device per GPIO
 */
void halExtDevicePowerUp(void)
{
  // Enable ISRs before powering so they can notify when device is ready
  halExtDevicePowered = true;
  halExtDeviceRdyCfgIrq();
  halExtDeviceIntCfgIrq();

  #ifdef  EXT_DEVICE_PWR
    // Assert power
    #if     (EXT_DEVICE_PWR_TRUE)
      GPIO_SET(EXT_DEVICE_PWR_PORT, EXT_DEVICE_PWR_BIT);
    #else//!(EXT_DEVICE_PWR_TRUE)
      GPIO_CLR(EXT_DEVICE_PWR_PORT, EXT_DEVICE_PWR_BIT);
    #endif//(EXT_DEVICE_PWR_TRUE)
  #endif//EXT_DEVICE_PWR
}

//-- External Device Ready --

/** @brief Indicate if the device is ready per GPIO
 * @return true if device is ready, false otherwise
 */
bool halExtDeviceIsReady(void)
{
  #ifdef  EXT_DEVICE_RDY
    return (halExtDevicePowered
           && GPIO_INP(EXT_DEVICE_RDY_PORT, EXT_DEVICE_RDY_BIT)
              == EXT_DEVICE_RDY_TRUE);
  #else//!EXT_DEVICE_RDY
    return halExtDevicePowered; // Assume ready only when powered
  #endif//EXT_DEVICE_RDY
}

/** @brief Wait for the device to become ready per GPIO
 */
void halExtDeviceWaitReady(void)
{
  halResetWatchdog();
  while (! halExtDeviceIsReady()) {
    // spin
  }
  halResetWatchdog();
}

//-- External Device Selection --

/** @brief Select the external device
 */
void halExtDeviceSelect(void)
{
  halExtDeviceSelected = true;
  #ifdef  EXT_DEVICE_SEL
    // Assert select
    #if     (EXT_DEVICE_SEL_TRUE)
      GPIO_SET(EXT_DEVICE_SEL_PORT, EXT_DEVICE_SEL_BIT);
    #else//!(EXT_DEVICE_SEL_TRUE)
      GPIO_CLR(EXT_DEVICE_SEL_PORT, EXT_DEVICE_SEL_BIT);
    #endif//(EXT_DEVICE_SEL_TRUE)
  #endif//EXT_DEVICE_SEL
}

/** @brief Unselect the external device
 */
void halExtDeviceDeselect(void)
{
  halExtDeviceSelected = false;
  #ifdef  EXT_DEVICE_SEL
    // Deassert select
    #if     (EXT_DEVICE_SEL_TRUE)
      GPIO_CLR(EXT_DEVICE_SEL_PORT, EXT_DEVICE_SEL_BIT);
    #else//!(EXT_DEVICE_SEL_TRUE)
      GPIO_SET(EXT_DEVICE_SEL_PORT, EXT_DEVICE_SEL_BIT);
    #endif//(EXT_DEVICE_SEL_TRUE)
  #endif//EXT_DEVICE_SEL
}

/** @brief Indicate if the device is selected
 */
bool halExtDeviceIsSelected(void)
{
  return halExtDeviceSelected;
}

//-- External Device Interrupts --

/** @brief Indicate if device interrupt GPIO is pending
 * @return true if device interrupt is pending, false otherwise
 */
bool halExtDeviceIntPending(void)
{
  #ifdef  EXT_DEVICE_INT
    //FIXME: This is pure GPIO level -- should it check INT_PENDING instead?
    return (halExtDevicePowered
            && GPIO_INP(EXT_DEVICE_INT_PORT, EXT_DEVICE_INT_BIT)
               == EXT_DEVICE_INT_TRUE);
  #else//!EXT_DEVICE_INT
    return false; // Assume never pending
  #endif//EXT_DEVICE_INT
}

/** @brief Disable device interrupt and increment interrupt nesting level.
 * @return interrupt level prior to disabling (0=interrupts were enabled)
 */
HalExtDeviceIntLevel halExtDeviceIntDisable(void)
{
  #ifdef  EXT_DEVICE_INT
    uint8_t origLevel;
    INT_CFGCLR = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Disable top level
    // We don't bother with 2nd-level here
    ATOMIC( // ATOMIC because these routines might be called from other ISRs
      origLevel = halExtDeviceIntLevel;
      if (origLevel != EXT_DEVICE_INT_UNCONFIGURED) {
        halExtDeviceIntLevel += 1;
      }
    );
    return origLevel;
  #else//!EXT_DEVICE_INT
    return EXT_DEVICE_INT_UNCONFIGURED;
  #endif//EXT_DEVICE_INT
}

/** @brief Decrement interrupt nesting level and, if 0, enable device
 * interrupt.
 * @param  clearPending if true clears any "stale" pending interrupt just
 *         prior to enabling device interrupt.
 * @return interrupt level prior to enabling (0=interrupts were enabled)
 */
HalExtDeviceIntLevel halExtDeviceIntEnable(bool clearPending)
{
  #ifdef  EXT_DEVICE_INT
    uint8_t origLevel;
    bool justEnabled = false;
    ATOMIC( // ATOMIC because these routines might be called from other ISRs
      origLevel = halExtDeviceIntLevel;
      if (origLevel != EXT_DEVICE_INT_UNCONFIGURED) {
        if (origLevel > EXT_DEVICE_INT_LEVEL_ON) { // Peg at LEVEL_ON
          halExtDeviceIntLevel -= 1;
          justEnabled = (halExtDeviceIntLevel == EXT_DEVICE_INT_LEVEL_ON);
        }
      }
    );
    if (clearPending) {
      // Clear out any stale state
      INT_GPIOFLAG = INT_IRQxFLAG(EXT_DEVICE_INT_IRQ);      // Clear 2nd level
      INT_MISS     = INT_MISSIRQx(EXT_DEVICE_INT_IRQ);      // Clear any missed
      INT_PENDCLR  = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Clear any pended
    }
    if (justEnabled) {
      INT_CFGSET = INT_IRQx(EXT_DEVICE_INT_IRQ);          // Enable top level
    }
    return origLevel;
  #else//!EXT_DEVICE_INT
    UNUSED_VAR(clearPending);
    return EXT_DEVICE_INT_UNCONFIGURED;
  #endif//EXT_DEVICE_INT
}

#endif//ENABLE_EXT_DEVICE       // Driver is enabled
