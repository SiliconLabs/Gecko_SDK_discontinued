// -----------------------------------------------------------------------------
// @file
// @brief EFM micro specific full HAL functions
//
// @author Silicon Laboratories Inc.
// @version 1.0.0
//
// @section License
// <b>(C) Copyright 2014 Silicon Laboratories, http://www.silabs.com</b>
//
// This file is licensed under the Silabs License Agreement. See the file
// "Silabs_License_Agreement.txt" for details. Before using this software for
// any purpose, you must agree to the terms of that agreement.
//
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "gpiointerrupt.h"

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

static void GPIOINT_InitSafe(void)
{
  // Enable GPIO clock for configuring interrupts
  CMU_ClockEnable(cmuClock_GPIO, true);

  // Turn on GPIO interrupts only if they weren't enabled elsewhere
  if (CORE_NvicIRQDisabled(GPIO_ODD_IRQn)
      || CORE_NvicIRQDisabled(GPIO_EVEN_IRQn)) {
    GPIOINT_Init();
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
    //Cancel all requests if request option is disabled
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

#ifdef PTA_REQ_MAX_BACKOFF_MASK
static void ptaReqRandomBackoff()
{
  halCommonDelayMicroseconds(halCommonGetRandom() & PTA_REQ_MAX_BACKOFF_MASK);
}
#else //PTA_REQ_MAX_BACKOFF_MASK
#define ptaReqRandomBackoff()
#endif //PTA_REQ_MAX_BACKOFF_MASK

  // halPta Implementation:

  // Board header is expected to define:
  // PTA REQUEST signal (OUT or OUT_OD): [optional]
  // #define PTA_REQ_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_REQ_GPIOCFG_NORMAL // gpioModePushPull
  // #define PTA_REQ_GPIOCFG_SHARED // gpioModeWiredAnd/Or
  // #define PTA_REQ_GPIOCFG    // PTA_REQ_GPIOCFG_NORMAL or _SHARED as above
  // #define PTA_REQ_ASSERTED   // 0 if negated logic; 1 if positive logic
  //
  // PTA GRANT signal (IN): [optional]
  // #define PTA_GNT_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_GNT_GPIOCFG    // gpioModeInputPull[ASSERTED] or gpioModeInput
  // #define PTA_GNT_ASSERTED   // 0 if negated logic; 1 if positive logic
  // Note that REQ and GNT can share the same IRQn if necessary
  //
  // PTA PRIORITY signal (OUT): [optional]
  // #define PTA_PRI_GPIO       // PORTx_PIN(y) (x=A/B/C/D/E/F/..., y=0-15)
  // #define PTA_PRI_GPIOCFG    // gpioModePushPull or gpioModeWiredOr/And if shared
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
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_REQ_GPIO) == gpioModeDisabled)
    {
     #if     PTA_REQ_ASSERTED
      halGpioClear(PTA_REQ_GPIO);
     #else//!PTA_REQ_ASSERTED
      halGpioSet(PTA_REQ_GPIO);
     #endif//PTA_REQ_ASSERTED
      halGpioSetConfig(PTA_REQ_GPIO, PTA_REQ_GPIOCFG);
    }
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
            &&(GPIO_PIN(PTA_REQ_GPIO) == GPIO_PIN(PTA_GNT_GPIO)) )/*Shared IRQ*/

 #if     (ptaReqAndGntIrqShared())
  #define PTA_REQ_ISR PTA_GNT_ISR // REQUEST and GRANT share same IRQ & ISR
 #endif//(ptaReqAndGntIrqShared())

  static void PTA_REQ_ISR(uint8_t pin);

  static inline void ptaReqGpioIntAcknowledge(void)
  {
    GPIO_IntClear(GPIO_FLAG(PTA_REQ_GPIO));
  }

  static inline void ptaReqGpioIntDisable(void)
  {
    GPIO_IntDisable(GPIO_FLAG(PTA_REQ_GPIO));
    ptaReqGpioIntAcknowledge();
  }

  static inline void ptaReqGpioIntEnable(void)
  {
    // Disable triggering and clear any stale events
    GPIO_IntConfig(GPIO_PORT(PTA_REQ_GPIO), GPIO_PIN(PTA_REQ_GPIO),
                   false, false, false);
    // Register callback before setting up and enabling pin interrupt
    GPIOINT_CallbackRegister(GPIO_PIN(PTA_REQ_GPIO), PTA_REQ_ISR);
    // Enable deasserting edge interrupt only
    GPIO_IntConfig(GPIO_PORT(PTA_REQ_GPIO), GPIO_PIN(PTA_REQ_GPIO),
                   !ptaReqAsserted, ptaReqAsserted, true);
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

  static void PTA_GNT_ISR(uint8_t pin);

  static bool ptaGntAsserted = PTA_GNT_ASSERTED;
  static bool gntWasAsserted = false;

  static inline void ptaGntGpioCfg(void)
  {
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_GNT_GPIO) == gpioModeDisabled)
    {
     #if     PTA_GNT_ASSERTED
      halGpioSet(PTA_GNT_GPIO); // pull up
     #else//!PTA_GNT_ASSERTED
      halGpioClear(PTA_GNT_GPIO); // pull down
     #endif//PTA_GNT_ASSERTED
      halGpioSetConfig(PTA_GNT_GPIO, PTA_GNT_GPIOCFG);
    }
    // Here we sense asserted state is same as its current output state.
    ptaGntAsserted = !!halGpioReadOutput(PTA_GNT_GPIO);
  }

  #define ptaGntGpioInAsserted() \
            (!!halGpioRead(PTA_GNT_GPIO) == !!ptaGntAsserted)

  static inline void ptaGntGpioIntAcknowledge(void)
  {
    GPIO_IntClear(GPIO_FLAG(PTA_GNT_GPIO));
  }

  static inline void ptaGntGpioIntDisable(void)
  {
    GPIO_IntDisable(GPIO_FLAG(PTA_GNT_GPIO));
    ptaGntGpioIntAcknowledge();
  }

  static inline void ptaGntGpioIntEnable(void)
  {
    // Disable triggering and clear any stale events
    GPIO_IntConfig(GPIO_PORT(PTA_GNT_GPIO), GPIO_PIN(PTA_GNT_GPIO),
                   false, false, false);
    gntWasAsserted = false; // Ensures we won't miss GNT assertion
    // Register callbacks before setting up and enabling pin interrupt
    GPIOINT_CallbackRegister(GPIO_PIN(PTA_GNT_GPIO), PTA_GNT_ISR);
    // Enable both edges' interrupt
    GPIO_IntConfig(GPIO_PORT(PTA_GNT_GPIO), GPIO_PIN(PTA_GNT_GPIO),
                   true, true, true);
  }

  static inline void ptaGntGpioIntPend(void)
  {
    GPIO_IntSet(GPIO_FLAG(PTA_GNT_GPIO));
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
    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(PTA_PRI_GPIO) == gpioModeDisabled)
    {
     #if     PTA_PRI_ASSERTED
      halGpioClear(PTA_PRI_GPIO);
     #else//!PTA_PRI_ASSERTED
      halGpioSet(PTA_PRI_GPIO);
     #endif//PTA_PRI_ASSERTED
      halGpioSetConfig(PTA_PRI_GPIO, PTA_PRI_GPIOCFG);
    }
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
  static void PTA_GNT_ISR(uint8_t pin)
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
        // Do we need this to meet GRANT -> REQUEST timing?
        // On GNT deassertion, pulse REQUEST to keep us going.
        // Don't want to revert to REQUEST phase here but stay in GRANT phase.
        // This seems dangerous in that it could allow a peer to assert their
        // REQUEST causing a conflict/race.

        if (!newGnt) {
          // If grant is lost mid transmit,
          // cancel request if we are transmitting
          // or if ack hold off is enabled and we are receiving
          if ((halPtaOptions & PTA_OPT_ABORT_TX)
             && ((txReq != PTA_REQ_OFF)
             || ((halPtaGetOptions() & PTA_OPT_ACK_HOLDOFF)
             && (rxReq != PTA_REQ_OFF)))) {
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
        ptaReqRandomBackoff();
        ptaUpdateReqIsr();
      } else {
        // Ignore GRANT changes unless we are REQUESTing
      }
     #endif//(ptaReqAndGntIrqShared())
    }
  }
  // Certain radios may want to override this with their own
  WEAK(void emPhyCancelTransmit(void))
  {
  }
 #endif//PTA_GNT_GPIO

 #if     ( defined(PTA_REQ_GPIO) && (!ptaReqAndGntIrqShared()) )
  // This IRQ is triggered on the negate REQUEST edge,
  // needed only when REQUEST signal is shared,
  // and not piggybacking GNT and REQ on same IRQ.
  static void PTA_REQ_ISR(uint8_t pin)
  {
    // External REQUEST deasserted so we can assert ours
    //ptaReqGpioIntAcknowledge(); // Covered within ptaReqGpioIntDisable()
    ptaReqGpioIntDisable(); // This is a one-shot event
    //TODO: Perform some random backoff before claiming REQUEST??
    ptaReqRandomBackoff();
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
        // Safely turn on GPIO interrupts
        GPIOINT_InitSafe();

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

#if defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)
static void halInternalTogglePtaReq()
{
  if( ptaEnabled                      // PTA is enabled and
      && ptaReqGpioOutAsserted()      // REQUESTing and
      && !halInternalRhoPinIsActive() // RHO not asserted
      && !ptaGntGpioInAsserted()) {   // GRANT not asserted
    ptaReqGpioSet(false);
    ptaReqGpioSet(true);
 }
}
#else //defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)
#define halInternalTogglePtaReq()
#endif //defined(PTA_REQ_PULSE_ON_RHO_DEASSERT) && defined(PTA_GNT_GPIO) && defined(PTA_REQ_GPIO)

static void RHO_ISR(uint8_t pin)
{
  // Ack interrupt before reading GPIO to avoid potential of missing int
  GPIO_IntClear(GPIO_FLAG(RHO_GPIO));
  if (rhoState & RHO_ENABLED_MASK) {
    // Notify Radio land of state change
    halInternalTogglePtaReq();
    halInternalPtaOrRhoNotifyRadio();
  }
}

EmberStatus halSetRadioHoldOff(bool enabled)
{
  // If enabling afresh or disabling after having been enabled
  // restart from a fresh state just in case.
  if (enabled || (rhoState & RHO_ENABLED_MASK)) {
    // Disable RHO interrupt & callback
    GPIO_IntConfig(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                   false, false, false);
    GPIOINT_CallbackUnRegister(GPIO_PIN(RHO_GPIO));
  }

  rhoState = (rhoState & ~RHO_ENABLED_MASK) | (enabled ? RHO_ENABLED_MASK : 0);

  if (enabled) {
    // Safely turn on GPIO interrupts
    GPIOINT_InitSafe();

    // Only configure GPIO if it was not set up prior
    if (halGpioGetConfig(RHO_GPIO) == gpioModeDisabled) {
      // Configure GPIO as input and if pulling, pull it toward deasserted state
      GPIO_PinModeSet(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                      RHO_GPIOCFG, !RHO_ASSERTED);
    }
    // Here we sense asserted state is opposite of its current output state.
    rhoAsserted = !halGpioReadOutput(RHO_GPIO);
    // Register callbacks before setting up and enabling pin interrupt.
    GPIOINT_CallbackRegister(GPIO_PIN(RHO_GPIO), RHO_ISR);
    // Set rising and falling edge interrupts; don't enable interrupt yet
    GPIO_IntConfig(GPIO_PORT(RHO_GPIO), GPIO_PIN(RHO_GPIO),
                   true, true, false);

    // Only update radio if it's on, otherwise defer to when it gets turned on
    if (rhoState & RHO_RADIO_ON_MASK) {
      halInternalPtaOrRhoNotifyRadio(); //Notify Radio land of current state
      GPIO_IntEnable(GPIO_FLAG(RHO_GPIO));
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
    GPIO_IntDisable(GPIO_FLAG(RHO_GPIO)); //clear RHO top level int enable
  }
}

void halStackRadioHoldOffPowerUp(void)
{
  rhoState |= RHO_RADIO_ON_MASK;
  if (rhoState & RHO_ENABLED_MASK) {
    // When waking radio, set up initial state and resume monitoring
    GPIO_IntDisable(GPIO_FLAG(RHO_GPIO)); //ensure RHO interrupt is off
    RHO_ISR(0); // Manually call ISR to assess current state
    GPIO_IntEnable(GPIO_FLAG(RHO_GPIO)); //enable RHO interrupt
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
