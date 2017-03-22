/*
 * File: micro.c
 * Description: EM3XX micro specific full HAL functions
 *
 *
 * Copyright 2008, 2009 by Ember Corporation. All rights reserved.          *80*
 */
//[[ Author(s): Brooks Barrett, Lee Taylor ]]


#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "stack/include/ember-types.h"
#include "include/error.h"

#include "hal/hal.h"

#ifdef RTOS
  #include "rtos/rtos.h"
#endif

#ifndef DEFAULT_PTA_OPTIONS
  #define DEFAULT_PTA_OPTIONS PTA_OPT_DISABLED
#endif
#ifndef CONST_PTA_OPTIONS
  #define CONST_PTA_OPTIONS (~PTA_OPT_DISABLED)
#endif

HalPtaOptions halPtaOptions = DEFAULT_PTA_OPTIONS;

extern void emPhyCancelTransmit (void);
void halStackRadioHoldOffPowerDown(void); // fwd ref
void halStackRadioHoldOffPowerUp(void);   // fwd ref

static inline void setBitMask(uint32_t * src, uint32_t mask, bool value) {
  if (src != NULL) {
    *src = value ? ((*src) | mask) : ((*src) & (~mask));
  }
}

EmberStatus halPtaSetBool(HalPtaOptions option, bool value)
{
  HalPtaOptions ptaOptions = halPtaGetOptions();

  setBitMask(&ptaOptions, option, value);

  return halPtaSetOptions(ptaOptions);
}

HalPtaOptions halPtaGetOptions(void)
{
  return halPtaOptions;
}

EmberStatus halPtaSetOptions(HalPtaOptions options)
{
  EmberStatus status = EMBER_SUCCESS;

  HalPtaOptions oldOptions = halPtaOptions;
  if ( (options & CONST_PTA_OPTIONS) != (DEFAULT_PTA_OPTIONS & CONST_PTA_OPTIONS)) {
    //Return error if any options argument is unsupported or constant
    status = EMBER_BAD_ARGUMENT;
  } else {
    //Only modify public options
    halPtaOptions = options;
  }
  if ((~oldOptions) & options & PTA_OPT_FORCE_HOLDOFF) {
    //Cancel all requests if force holdoff option is enabled
    (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
    (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
  }
  if ( (status == EMBER_SUCCESS)
     && (halPtaOptions & PTA_OPT_RHO_ENABLED) != (oldOptions & PTA_OPT_RHO_ENABLED) ) {
    status = halSetRadioHoldOff(options & PTA_OPT_RHO_ENABLED);
  }
#ifdef PTA_OPT_PTA_ENABLED
  if ( (status == EMBER_SUCCESS)
     && (options & PTA_OPT_PTA_ENABLED) != (oldOptions & PTA_OPT_PTA_ENABLED) ) {
    status = halPtaSetEnable(options & PTA_OPT_RHO_ENABLED);
  }
#endif //PTA_OPT_PTA_ENABLED
  return status;
}

  // halPta Implementation:

  // Board header is expected to define:
  // PTA REQUEST signal (OUT or OUT_OD): [optional]
  // #define PTA_REQ_GPIO       // PORTx_PIN(y) (x=A/B/C, y=0-7)
  // #define PTA_REQ_GPIOCFG_NORMAL // GPIOCFG_OUT
  // #define PTA_REQ_GPIOCFG_SHARED // GPIOCFG_OUT_OD (_ASSERTED must be 0)
  // #define PTA_REQ_GPIOCFG    // PTA_REQ_GPIOCFG_NORMAL or _SHARED as above
  // #define PTA_REQ_GPIOCFG    // GPIOCFG_OUT or GPIOCFG_OUT_OD if shared
  // #define PTA_REQ_ASSERTED   // 0 if negated logic; 1 if positive logic
  // #define PTA_REQ_FLAG_BIT   // INT_IRQnFLAG (n=A/B/C/D) [only if shared]
  // #define PTA_REQ_MISS_BIT   // INT_MISSIRQn (n=A/B/C/D) [only if shared]
  // #define PTA_REQ_INTCFG     // GPIO_INTCFGn (n=A/B/C/D) [only if shared]
  // #define PTA_REQ_INT_EN_BIT // INT_IRQn     (n=A/B/C/D) [only if shared]
  // #define PTA_REQ_SEL()      // do { GPIO_IRQnSEL = PTA_REQ_GPIO; } while (0)
  //                            // (n=C/D or empty if n=A/B)[only if shared]
  //
  // PTA GRANT signal (IN): [optional]
  // #define PTA_GNT_GPIO       // PORTx_PIN(y) (x=A/B/C, y=0-7)
  // #define PTA_GNT_GPIOCFG    // GPIOCFG_IN_PUD[ASSERTED] or GPIOCFG_IN
  // #define PTA_GNT_ASSERTED   // 0 if negated logic; 1 if positive logic
  // #define PTA_GNT_FLAG_BIT   // INT_IRQnFLAG (n=A/B/C/D)
  // #define PTA_GNT_MISS_BIT   // INT_MISSIRQn (n=A/B/C/D)
  // #define PTA_GNT_INTCFG     // GPIO_INTCFGn (n=A/B/C/D)
  // #define PTA_GNT_INT_EN_BIT // INT_IRQn     (n=A/B/C/D)
  // #define PTA_GNT_SEL()      // do { GPIO_IRQnSEL = PTA_GNT_GPIO; } while (0)
  //                            // (n=C/D or empty if n=A/B)
  // Note that REQ and GNT can share the same IRQn if necessary
  //
  // PTA PRIORITY signal (OUT): [optional]
  // #define PTA_PRI_GPIO       // PORTx_PIN(y) (x=A/B/C, y=0-7)
  // #define PTA_PRI_GPIOCFG    // GPIOCFG_OUT or GPIOCFG_OUT_OD if shared
  // #define PTA_PRI_ASSERTED   // 0 if negated logic; 1 if positive logic

 #ifdef  PTA_REQ_GPIO

  #ifdef EMBER_STACK_IP
    #define halPtaCounter(type,data) \
      emberCounterHandler(halPtaCounterType(type), data)
  #elif !defined(EMBER_STACK_CONNECT)//!EMBER_STACK_IP and !EMBER_STACK_CONNECT
    void emCallCounterHandler(EmberCounterType type, uint8_t data);

    #define halPtaCounter(type,data) \
      emCallCounterHandler(halPtaCounterType(type), data)
  #endif //EMBER_STACK_IP

  static bool ptaReqAsserted = PTA_REQ_ASSERTED;

  static inline void ptaReqGpioSet(bool enable)
  {
    if (enable == ptaReqAsserted) {
      halGpioSet(PTA_REQ_GPIO);
    } else {
      halGpioClear(PTA_REQ_GPIO);
    }
  }

  static inline void ptaReqGpioCfg(void)
  {
    // GPIO must have been set up prior.
    // Here we sense asserted state is opposite of its current output state.
    ptaReqAsserted = !halGpioReadOutput(PTA_REQ_GPIO);
  }

  #define ptaReqGpioCfgIsShared() \
            (halGpioGetConfig(PTA_REQ_GPIO) != PTA_REQ_GPIOCFG_NORMAL)

  #define ptaReqGpioInAsserted() \
            (!!halGpioRead(PTA_REQ_GPIO) == !!ptaReqAsserted)

  #define ptaReqGpioOutAsserted() \
            (!!halGpioReadOutput(PTA_REQ_GPIO) == !!ptaReqAsserted)

  #define ptaReqAndGntIrqShared() \
            ( (defined(PTA_GNT_GPIO))                          /*Have GNT*/ \
            &&(PTA_REQ_INT_EN_BIT == PTA_GNT_INT_EN_BIT) )     /*Shared IRQ*/

  static inline void ptaReqGpioIntAcknowledge(void)
  {
    INT_MISS = PTA_REQ_MISS_BIT;
    INT_GPIOFLAG = PTA_REQ_FLAG_BIT;
  }

  static inline void ptaReqGpioIntDisable(void)
  {
    INT_CFGCLR = PTA_REQ_INT_EN_BIT;  //clear top level int enable
    INT_PENDCLR = PTA_REQ_INT_EN_BIT; //clear any pended event
    ptaReqGpioIntAcknowledge();
  }

  static inline void ptaReqGpioIntEnable(void)
  {
    PTA_REQ_INTCFG = 0;               //disable triggering
    ptaReqGpioIntDisable();           //clear any stale events
    PTA_REQ_INTCFG  = (0 << GPIO_INTFILT_BIT) /* 0 = no filter  */
                    | ((GPIOINTMOD_RISING_EDGE+ptaReqAsserted) << GPIO_INTMOD_BIT); /* deassert edge */
    PTA_REQ_SEL(); //point IRQ to the desired pin
    INT_CFGSET = PTA_REQ_INT_EN_BIT;  //set top level interrupt enable
  }

 #else//!PTA_REQ_GPIO

  #define ptaReqGpioSet(enable)      /* no-op */
  #define ptaReqGpioCfg()            /* no-op */
  #define ptaReqGpioCfgIsShared()    0
  #define ptaReqGpioInAsserted()     0
  #define ptaReqGpioIntAcknowledge() /* no-op */
  #define ptaReqGpioIntDisable()     /* no-op */
  #define ptaReqGpioIntEnable()      /* no-op */
  #define ptaReqGpioOutAsserted()    1
  #define ptaReqAndGntIrqShared()    0
  #define halPtaCounter(type,data)   /* no-op */

 #endif//PTA_REQ_GPIO

 #ifdef  PTA_GNT_GPIO

  void PTA_GNT_ISR(void);

  static bool ptaGntAsserted = PTA_GNT_ASSERTED;
  static bool gntWasAsserted = false;

  static inline void ptaGntGpioCfg(void)
  {
    // GPIO must have been set up prior.
    // Here we sense asserted state is same as its current output state.
    ptaGntAsserted = !!halGpioReadOutput(PTA_GNT_GPIO);
  }

  #define ptaGntGpioInAsserted() \
            (!!halGpioRead(PTA_GNT_GPIO) == !!ptaGntAsserted)

  static inline void ptaGntGpioIntAcknowledge(void)
  {
    INT_MISS = PTA_GNT_MISS_BIT;
    INT_GPIOFLAG = PTA_GNT_FLAG_BIT;
  }

  static inline void ptaGntGpioIntDisable(void)
  {
    INT_CFGCLR = PTA_GNT_INT_EN_BIT;  //clear top level int enable
    INT_PENDCLR = PTA_GNT_INT_EN_BIT; //clear any pended event
    ptaGntGpioIntAcknowledge();
  }

  static inline void ptaGntGpioIntEnable(void)
  {
    PTA_GNT_INTCFG = 0;               //disable triggering
    ptaGntGpioIntDisable();           //clear any stale events
    gntWasAsserted = false; // Ensures we won't miss GNT assertion
    PTA_GNT_INTCFG  = (0 << GPIO_INTFILT_BIT) /* 0 = no filter  */
                    | (GPIOINTMOD_BOTH_EDGES << GPIO_INTMOD_BIT);
    PTA_GNT_SEL(); //point IRQ to the desired pin
    INT_CFGSET = PTA_GNT_INT_EN_BIT;  //set top level interrupt enable
  }

  static inline void ptaGntGpioIntPend(void)
  {
    INT_PENDSET = PTA_GNT_INT_EN_BIT;
  }

 #else//!PTA_GNT_GPIO

  #define ptaGntGpioCfg()            /* no-op */
  #define ptaGntGpioInAsserted()     1
  #define ptaGntGpioIntAcknowledge() /* no-op */
  #define ptaReqGpioIntDisable()     /* no-op */
  #define ptaGntGpioIntDisable()     /* no-op */
  #define ptaGntGpioIntEnable()      /* no-op */
  #define ptaGntGpioIntPend()        (halInternalPtaOrRhoNotifyRadio())

 #endif//PTA_GNT_GPIO

 #ifdef  PTA_PRI_GPIO

  #define ptaPriGpioOutAsserted() \
            (!!halGpioReadOutput(PTA_PRI_GPIO) == !!ptaPriAsserted)

  static bool ptaPriAsserted = PTA_PRI_ASSERTED;

  static inline void ptaPriGpioSet(bool enable)
  {
    if (enable != ptaPriAsserted) {
      halGpioClear(PTA_PRI_GPIO);
    } else {
      halGpioSet(PTA_PRI_GPIO);
    }
  }

  static inline void ptaPriGpioCfg(void)
  {
    // GPIO must have been set up prior.
    // Here we sense asserted state is opposite of its current output state.
    ptaPriAsserted = !halGpioReadOutput(PTA_PRI_GPIO);
  }

 #else//!PTA_PRI_GPIO

  #define ptaPriGpioOutAsserted()    false
  #define ptaPriGpioSet(enable)      /* no-op */
  #define ptaPriGpioCfg()            /* no-op */

 #endif//PTA_PRI_GPIO

  static void halInternalPtaOrRhoNotifyRadio(void);
  static volatile bool ptaEnabled;
  static volatile bool requestDenied;
  static volatile halPtaReq_t txReq, rxReq;
  static volatile halPtaCb_t  txCb,  rxCb;

  // Must be called with interrupts disabled
  static void ptaUpdateReqIsr(void)
  {
    halPtaReq_t txReqL = txReq; // Local non-volatile flavor avoids warnings
    halPtaReq_t rxReqL = rxReq; // Local non-volatile flavor avoids warnings
    bool myReq = !!((txReqL | rxReqL) & PTA_REQ_ON);     // I need to REQUEST
    bool force = !!((txReqL | rxReqL) & PTA_REQ_FORCE);  // (ignoring others)
    bool exReq;                                          // external requestor?
    if (ptaReqGpioOutAsserted()) {  // in GRANT phase
      exReq = false;                // ignore external requestors
    } else {                        // in REQUEST phase
      ptaReqGpioIntAcknowledge();   // Before sampling REQUEST, avoids race
      exReq = ptaReqGpioCfgIsShared() && ptaReqGpioInAsserted();
    }
    if (halPtaOptions & PTA_OPT_FORCE_HOLDOFF) {
      myReq = false;
    }
    if (myReq) {                    // want to assert REQUEST
      if (force || !exReq) {        // can assert REQUEST
        if (!ptaReqGpioOutAsserted()) {
          // Assume request denied until request is granted
          requestDenied = true;
        }
        ptaGntGpioIntAcknowledge();
        ptaGntGpioIntEnable();
        ptaReqGpioSet(true);
        ptaPriGpioSet(!!((txReqL | rxReqL) & PTA_REQ_HIPRI));
        // Issue callbacks on REQUEST assertion
        // These are one-shot callbacks
        if ((rxCb != NULL) && (rxReq & PTA_REQCB_REQUESTED)) {
          (*rxCb)(PTA_REQCB_REQUESTED);
          rxReq &= ~PTA_REQCB_REQUESTED;
        }
        if ((txCb != NULL) && (txReq & PTA_REQCB_REQUESTED)) {
          (*txCb)(PTA_REQCB_REQUESTED);
          txReq &= ~PTA_REQCB_REQUESTED;
        }
        ptaGntGpioIntPend(); // Manually force GRANT check if missed/no edge
      } else {                      // must wait for REQUEST
        ptaReqGpioIntEnable();
      }
    } else {                        // negate REQUEST
      if (ptaReqGpioOutAsserted()) {
        halPtaCounter(REQUESTED, 1);
        if (requestDenied) {
          requestDenied = false;
          halPtaCounter(DENIED, 1);
        }
      }
      ptaPriGpioSet(false);
      ptaReqGpioSet(false);
      ptaGntGpioIntDisable();
      ptaReqGpioIntDisable();
      halInternalPtaOrRhoNotifyRadio(); // Reassess (assert) RHO
    }
  }

 #ifdef  PTA_GNT_GPIO
  // Triggered on both GRANT edges
  void PTA_GNT_ISR(void)
  {
    ptaGntGpioIntAcknowledge();
    if (ptaReqGpioOutAsserted()) {  // GRANT phase
      bool newGnt = ptaGntGpioInAsserted(); // Sample GPIO once, now
      if (newGnt != gntWasAsserted) {
        gntWasAsserted = newGnt;
        halInternalPtaOrRhoNotifyRadio();
        // Issue callbacks on GRANT assert or negate
        // These are not one-shot callbacks
        halPtaReq_t newState = (newGnt ? PTA_REQCB_GRANTED : PTA_REQCB_NEGATED);
        if ((rxCb != NULL) && (rxReq & newState)) {
          (*rxCb)(newState);
        }
        if ((txCb != NULL) && (txReq & newState)) {
          (*txCb)(newState);
        }
        //Do we need this to meet GRANT -> REQUEST timing?
        // On GNT deassertion, pulse REQUEST to keep us going.
        // Don't want to revert to REQUEST phase here but stay in GRANT phase.
        // This seems dangerous in that it could allow a peer to assert their
        // REQUEST causing a conflict/race.

        if (!newGnt) {
          if (halPtaOptions & PTA_OPT_ABORT_TX) {
            halPtaCounter(TX_ABORTED, 1);
            emPhyCancelTransmit();
          }
       #ifdef ENABLE_PTA_REQ_PULSE
          ptaReqGpioSet(false);
          ptaReqGpioSet(true);
       #endif
        }
      }
    } else {                        // REQUEST phase
     #if     (ptaReqAndGntIrqShared())
      if (ptaReqGpioCfgIsShared()) {
        // External REQUEST deasserted so we can assert ours
        ptaReqGpioIntDisable(); // This is a one-shot event
        //TODO: Perform some random backoff before claiming REQUEST??
        ptaUpdateReqIsr();
      } else {
        // Ignore GRANT changes unless we are REQUESTing
      }
     #endif//(ptaReqAndGntIrqShared())
    }
  }
 #endif//PTA_GNT_GPIO

 #if     ( defined(PTA_REQ_GPIO) && (!ptaReqAndGntIrqShared()) )
  // This IRQ is triggered on the negate REQUEST edge,
  // needed only when REQUEST signal is shared,
  // and not piggybacking GNT and REQ on same IRQ.
  void PTA_REQ_ISR(void)
  {
    // External REQUEST deasserted so we can assert ours
    //ptaReqGpioIntAcknowledge(); // Covered within ptaReqGpioIntDisable()
    ptaReqGpioIntDisable(); // This is a one-shot event
    //TODO: Perform some random backoff before claiming REQUEST??
    ptaUpdateReqIsr();
  }
 #endif//( defined(PTA_REQ_GPIO) && (!ptaReqAndGntIrqShared()) )

  // Public API

  EmberStatus halPtaSetTxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)
  {
    EmberStatus status = EMBER_ERR_FATAL;
    ATOMIC(
      if (ptaEnabled) {
        // Signal old OFF callback when unrequesting
        // in case PTA is disabled whilst in the midst of a request,
        // so the requestor's state machine doesn't lock up.
        if ( (ptaReq == PTA_REQ_OFF)
           &&(txReq != PTA_REQ_OFF)
           &&(txCb != NULL)
           &&(txReq & PTA_REQCB_OFF) ) {
            (*txCb)(PTA_REQCB_OFF);
        }
        txCb  = ptaCb;
        if (txReq == ptaReq) {
          // Save a little time if redundant request
        } else {
          txReq = ptaReq;
          ptaUpdateReqIsr();
        }
        status = EMBER_SUCCESS;
      }
    )//ATOMIC
    return status;
  }

  EmberStatus halPtaSetRxRequest(halPtaReq_t ptaReq, halPtaCb_t ptaCb)
  {
    EmberStatus status = EMBER_ERR_FATAL;
    ATOMIC(
      if (ptaEnabled) {
        // Signal old OFF callback when unrequesting
        // in case PTA is disabled whilst in the midst of a request,
        // so the requestor's state machine doesn't lock up.
        if ( (ptaReq == PTA_REQ_OFF)
           &&(rxReq != PTA_REQ_OFF)
           &&(rxCb != NULL)
           &&(rxReq & PTA_REQCB_OFF) ) {
            (*rxCb)(PTA_REQCB_OFF);
        }
        rxCb  = ptaCb;
        if (rxReq == ptaReq) {
          // Save a little time if redundant request
        } else {
          rxReq = ptaReq;
          ptaUpdateReqIsr();
        }
        status = EMBER_SUCCESS;
      }
    )//ATOMIC
    return status;
  }

  halPtaReq_t halPtaFrameDetectReq(void)
  {
    HalPtaOptions options = halPtaGetOptions();
    halPtaReq_t syncDet = PTA_REQ_OFF;
    if (halPtaIsEnabled() && !(options & PTA_OPT_REQ_FILTER_PASS)) {
      syncDet |= PTA_REQ_ON;
      if (options & PTA_OPT_RX_HIPRI) {
        syncDet |= PTA_REQ_HIPRI;
      }
    }
    return syncDet;
  }

  halPtaReq_t halPtaFilterPassReq(void)
  {
    HalPtaOptions options = halPtaGetOptions();
    halPtaReq_t filterPass = PTA_REQ_OFF;
    if (halPtaIsEnabled()) {
      filterPass |= PTA_REQ_ON;
      if (options & (PTA_OPT_RX_HIPRI | PTA_OPT_HIPRI_FILTER_PASS)) {
        filterPass |= PTA_REQ_HIPRI;
      }
    }
    return filterPass;
  }

  // Certain radios may want to override this with their own
  WEAK(void emRadioEnablePta(bool enable))
  {
  }

  EmberStatus halPtaSetEnable(bool enabled)
  {
    if (enabled != ptaEnabled) {
      if (enabled) {
        ptaReqGpioCfg();
        ptaPriGpioCfg();
        ptaGntGpioCfg();
      } else {
        // Shut any pending PTA operation down
        (void) halPtaSetRxRequest(PTA_REQ_OFF, NULL);
        (void) halPtaSetTxRequest(PTA_REQ_OFF, NULL);
      }
      ptaEnabled = enabled;
      // Inform the radio in case PTA requires radio state manipulations
      emRadioEnablePta(enabled);
      halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of new config
#ifdef PTA_OPT_PTA_ENABLED
      setBitMask(&halPtaOptions, PTA_OPT_PTA_ENABLED, ptaEnabled);
#endif// PTA_OPT_PTA_ENABLED
    }
    return EMBER_SUCCESS;
  }

  bool halPtaIsEnabled(void)
  {
    return ptaEnabled;
  }

#ifdef  RHO_GPIO // BOARD_HEADER supports Radio HoldOff

#ifdef  WAKE_ON_DFL_RHO_VAR // Only define this if needed per board header
uint8_t WAKE_ON_DFL_RHO_VAR = WAKE_ON_DFL_RHO;
#endif//WAKE_ON_DFL_RHO_VAR

#define RHO_ENABLED_MASK  0x01u // RHO is enabled
#define RHO_RADIO_ON_MASK 0x02u // Radio is on (not sleeping)
static uint8_t rhoState;
static bool rhoAsserted = !!RHO_ASSERTED;

bool halGetRadioHoldOff(void)
{
  return (!!(rhoState & RHO_ENABLED_MASK));
}

// Return active state of Radio HoldOff GPIO pin
static bool halInternalRhoPinIsActive(void)
{
  return ( (rhoState & RHO_ENABLED_MASK)
         &&(((bool)halGpioRead(RHO_GPIO)) == rhoAsserted) );
}

void RHO_ISR(void)
{
  if (rhoState & RHO_ENABLED_MASK) {
    // Ack interrupt before reading GPIO to avoid potential of missing int
    INT_MISS = RHO_MISS_BIT;
    INT_GPIOFLAG = RHO_FLAG_BIT; // acknowledge the interrupt
    // Notify Radio land of state change
    halInternalPtaOrRhoNotifyRadio();
  } else {
   #ifdef  RHO_ISR_FOR_DFL
    // Defer to default GPIO config's ISR
    extern void RHO_ISR_FOR_DFL(void);
    RHO_ISR_FOR_DFL(); // This ISR is expected to acknowledge the interrupt
   #else//!RHO_ISR_FOR_DFL
    INT_GPIOFLAG = RHO_FLAG_BIT; // acknowledge the interrupt
   #endif//RHO_ISR_FOR_DFL
  }
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  // If enabling afresh or disabling after having been enabled
  // restart from a fresh state just in case.
  // Otherwise don't touch a setup that might already have been
  // put into place by the default 'DFL' use (e.g. a button).
  // When disabling after having been enabled, it is up to the
  // board header caller to reinit the default 'DFL' use if needed.
  if (enabled || (rhoState & RHO_ENABLED_MASK)) {
    RHO_INTCFG = 0;              //disable RHO triggering
    INT_CFGCLR = RHO_INT_EN_BIT; //clear RHO top level int enable
    INT_GPIOFLAG = RHO_FLAG_BIT; //clear stale RHO interrupt
    INT_MISS = RHO_MISS_BIT;     //clear stale missed RHO interrupt
  }

  rhoState = (rhoState & ~RHO_ENABLED_MASK) | (enabled ? RHO_ENABLED_MASK : 0);

  // Reconfigure GPIOs for desired state
  ADJUST_GPIO_CONFIG_DFL_RHO(enabled);

  if (enabled) {
    // Here we sense asserted state is opposite of its current output state.
    rhoAsserted = !halGpioReadOutput(RHO_GPIO);
    // Only update radio if it's on, otherwise defer to when it gets turned on
    if (rhoState & RHO_RADIO_ON_MASK) {
      halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of current state
      INT_CFGSET = RHO_INT_EN_BIT; //set top level interrupt enable
      // Interrupt on now, ISR will maintain proper state
    }
  } else {
    halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of configured state
    // Leave interrupt state untouched (probably turned off above)
  }
  setBitMask(&halPtaOptions, PTA_OPT_RHO_ENABLED, rhoState & RHO_ENABLED_MASK);
  return EMBER_SUCCESS;
}

void halStackRadioHoldOffPowerDown(void)
{
  rhoState &= ~RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When sleeping radio, no need to monitor RHO anymore
    INT_CFGCLR = RHO_INT_EN_BIT; //clear RHO top level int enable
  }
}

void halStackRadioHoldOffPowerUp(void)
{
  rhoState |= RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When waking radio, set up initial state and resume monitoring
    INT_CFGCLR = RHO_INT_EN_BIT; //ensure RHO interrupt is off
    RHO_ISR(); // Manually call ISR to assess current state
    INT_CFGSET = RHO_INT_EN_BIT; //enable RHO interrupt
  }
}

#else//!RHO_GPIO

// Stubs in case someone insists on referencing them or for PTA w/o RHO

bool halGetRadioHoldOff(void)
{
  return false;
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  return (enabled ? EMBER_BAD_ARGUMENT : EMBER_SUCCESS);
}

void halStackRadioHoldOffPowerDown(void)
{
}

void halStackRadioHoldOffPowerUp(void)
{
  halInternalPtaOrRhoNotifyRadio();
}

#define halInternalRhoPinIsActive() (false)

#endif//RHO_GPIO // Board header supports Radio HoldOff

extern void emRadioHoldOffIsr(bool active);

static void halInternalPtaOrRhoNotifyRadio(void)
{
  bool ptaRho = ( ptaEnabled                       // PTA is enabled and:
                &&( (!ptaReqGpioOutAsserted())     // not REQUESTing or
                  ||(!ptaGntGpioInAsserted()) ) ); // REQUEST not GRANTed
  ptaRho = ptaRho || halInternalRhoPinIsActive();
  if (!ptaRho) {
    requestDenied = false;
  }
  emRadioHoldOffIsr(ptaRho);
}
