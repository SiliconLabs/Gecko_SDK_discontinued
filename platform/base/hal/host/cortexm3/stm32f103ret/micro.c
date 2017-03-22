/** @file hal/host/cortexm3/micro.c
 *  @brief General STM32F103RET host HAL functions.
 *
 * GPIO Allocation for STM32F103RET host HAL, showing primary usage
 * and powerdown configuration (IPU=input-pullup, IPD=input-pulldown,
 * AIN=analog)
 *
 * NOTE: Regarding GPIO config, the lowest power consumption is achieved
 * by using analog mode (AIN) since this disables the Schmitt trigger.  AIN
 * is recommended whenever a specific level is not required.
 *
 * PA0  - IPU - nCTS2
 * PA1  - AIN - nRTS2
 * PA2  - IPU - TXD2
 * PA3  - AIN - RXD2
 * PA4  - IPU - nSS
 * PA5  - AIN - SCK
 * PA6  - AIN - MISO
 * PA7  - AIN - MOSI
 * PA8  - IPD - TEMP_ENABLE
 * PA9  - IPU - TXD1
 * PA10 - AIN - RXD1
 * PA11 - IPU - nCTS1
 * PA12 - AIN - nRTS1
 * PA13 - AIN - JTMS-SWDIO
 * PA14 - AIN - JCLK-SWCLK
 * PA15 - AIN - JTDI
 * PB0  - IPU - NCP_nRESET
 * PB1  - AIN - BTL
 * PB2  - AIN - BOOT1
 * PB3  - AIN - JTDO
 * PB4  - AIN - nJTRST
 * PB5  - AIN - unused/testpoint10
 * PB6  - AIN - spare/HGPIO10
 * PB7  - IPD - DF_nSD (shutdown)
 * PB8  - IPU - LED0
 * PB9  - IPU - LED1
 * PB10 - IPU - BUTTON0
 * PB11 - IPU - BUTTON1
 * PB12 - IPU - DF_nCS
 * PB13 - IPU - DF_SCK
 * PB14 - AIN - DF_SO
 * PB15 - IPU - DF_SI
 * PC0  - AIN - TEMP_SENSOR
 * PC1  - AIN - spare/HGPIO9
 * PC2  - AIN - unused/testpoint2
 * PC3  - AIN - unused/testpoint3
 * PC4  - IPU - nHOST_INT
 * PC5  - IPU - nWAKE
 * PC6  - AIN - PIEZO
 * PC7  - AIN - spare/HGPIO5
 * PC8  - AIN - spare/HGPIO6
 * PC9  - AIN - unused/testpoint5
 * PC10 - AIN - unused/testpoint6
 * PC11 - AIN - unused/testpoint7
 * PC12 - AIN - unused/testpoint8
 * PC13 - AIN - TAMPER-RTC
 * PC14 - AIN - OSC32-IN
 * PC15 - AIN - OSC32-OUT
 * PD0  - AIN - OSC-IN
 * PD1  - AIN - OSC-OUT
 * PD2  - AIN - unused/testpoint9
 * BOOT0 - (none) - System memory bootloader
 *
 *
 * GPIO/Peripheral Grouping by module/file:
 *
 * - emberSerialInit()::  USART1
 *     - app/util/serial/serial.c
 *
 * - halNcpSerialInit()::  USART2/SPI1/EXTI4/nHOST_INT/nWAKE/nRESET
 *     - hal/host/cortexm3/stm32f103ret/spi-protocol.c
 *
 * - halInternalInitAdc()::  ADC1/TEMP_SENSE/TEMP_ENABLE
 *     - hal/plugin/adc/adc-stm32f103ret.c
 *
 * - halInternalInitButton()::  EXTI15_10/BUTTON0/BUTTON1
 *     - hal/host/cortexm3/stm32f103ret/button.c
 *
 * - halInternalInitLed()::  LED0/LED1
 *     - hal/host/cortexm3/stm32f103ret/led.c
 *
 * - halPlayTune_P()::  TIM3/PIEZO
 *     - hal/plugin/buzzer/buzzer-stm32f103ret.c
 *
 * - halInternalStartSystemTimer()::  RTC/OSC32
 *     - hal/host/cortexm3/stm32f103ret/system-timer.c
 *
 * - halInternalInitSysTick()::  SysTick
 *     - hal/host/cortexm3/stm32f103ret/micro.c
 *
 * - halEepromInit()::  SPI2/DataFlash
 *     - hal/host/cortexm3/stm32f103ret/bootloader/at45db021d.c
 *     - hal/host/cortexm3/stm32f103ret/bootloader/m45pe20.c
 *
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

#include PLATFORM_HEADER
#include <stdlib.h>
#include "stack/include/error.h"
#include "hal/hal.h"
#include "hal/host/spi-protocol-common.h"
#include "serial/serial.h"


void halInit(void)
{
  //At this stage the microcontroller clock setting is already configured
  //via the SystemInit() function which lives in system_stm32f10x.c and
  //is called from startup_stm32f10x_hd.s. (Both these files are in ST's
  //library directory.)
  
  //cstartup has not disabled interrupts so disable them here to allow
  //HAL initialization to run undisturbed.
  INTERRUPTS_OFF();
  
  //Define all of the periphals we use.
  uint32_t apb1Peripherals = (RCC_APB1Periph_BKP  |
                            RCC_APB1Periph_PWR  |
                            RCC_APB1Periph_SPI2 |
                            RCC_APB1Periph_TIM2 |
                            RCC_APB1Periph_TIM3 |
                            RCC_APB1Periph_TIM5 );
  uint32_t apb2Peripherals = (RCC_APB2Periph_ADC1   |
                            RCC_APB2Periph_AFIO   |
                            RCC_APB2Periph_GPIOA  |
                            RCC_APB2Periph_GPIOB  |
                            RCC_APB2Periph_GPIOC  |
                            RCC_APB2Periph_GPIOD  |
                            RCC_APB2Periph_SPI1   |
                            RCC_APB2Periph_USART1 );
  //Enable clocks to all of our peripherals.
  RCC_APB1PeriphClockCmd(apb1Peripherals, ENABLE);
  RCC_APB2PeriphClockCmd(apb2Peripherals, ENABLE);
  
  //Allow access to BKP Domain
  PWR_BackupAccessCmd(ENABLE);
  
  //De-initialize our peripherals here so each module only has to
  //worry about initializing them as needed.  DeInit works by toggling
  //PeriphResetCmd enable/disable.
  ADC_DeInit(ADC1);
  BKP_DeInit();
  EXTI_DeInit();
  GPIO_AFIODeInit();
  GPIO_DeInit(GPIOA);
  GPIO_DeInit(GPIOB);
  GPIO_DeInit(GPIOC);
  GPIO_DeInit(GPIOD);
  //Do not DeInit PWR
  SPI_I2S_DeInit(SPI1);
  TIM_DeInit(TIM2);
  TIM_DeInit(TIM3);
  TIM_DeInit(TIM5);
  USART_DeInit(USART1);
  
  //Since there is no board file, each piece of the HAL is responsible for
  //configuring it's own GPIO.  Refer to the top of this file for a listing
  //of GPIO allocation and what is responsible for each group of GPIO.
  
  //WARNING: ONCE STARTED THE WATCHDOG CANNOT BE STOPPED.  THIS HAS THE
  //         POTENTIAL TO DISRUPT SLEEP OPERATIONS!  ONLY ENABLE THE
  //         WATCHDOG IF YOU ARE NOT SLEEPING OR YOU CAN GUARANTEE YOUR
  //         SLEEP CYCLES ARE SHORTER THAN THE WATCHDOG.  SEE THE
  //         IMPLEMENTATION FOR EXACT WATCHDOG TIMEOUT TIME.
  //halInternalEnableWatchDog();
  halInternalStartSystemTimer();
  halInternalInitSysTick();
  halInternalInitAdc();
  halInternalInitButton();
  halInternalInitLed();
#if (! defined(EMBER_STACK_IP))
  halNcpSerialInit();
#endif
}

void halPowerUp(void)
{
  //TODO: need a proper powerup function
  halNcpSerialPowerup();
}

void halPowerDown(void)
{
  //TODO: need a proper powerdown function
  halNcpSerialPowerdown();
}

void halInternalEnableWatchDog(void)
{
  //WARNING: ONCE STARTED THE IWDG CANNOT BE STOPPED.  THIS HAS THE
  //         POTENTIAL TO DISRUPT SLEEP OPERATIONS!
  
  //Enable write access to IWDG_PR and IWDG_RLR registers
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  
  //IWDG counter clock: 40KHz(LSI) / 256 = 156.25 Hz
  IWDG_SetPrescaler(IWDG_Prescaler_32);
  
  //Set counter reload value to 1562 which is a reset about every 9.99s
  //(maximum possible IWDG time: 25s)
  IWDG_SetReload(1562);
  
  //Reload IWDG counter
  IWDG_ReloadCounter();

  //Enable IWDG (the LSI oscillator will be enabled by hardware)
  IWDG_Enable();
}

void halInternalResetWatchDog(void)
{
  IWDG_ReloadCounter();
}

void halInternalDisableWatchDog(uint8_t magicKey)
{
  //Once the IWDG is started, it's not possible to turn it off!
  assert(false);
}

void halReboot(void)
{
  NVIC_SystemReset();
}

void halInternalAssertFailed(PGM_P filename, int linenumber)
{
  halResetWatchdog();  // In case we're close to running out.
  
  INTERRUPTS_OFF();
  
  #if !defined(EMBER_ASSERT_OUTPUT_DISABLED)
    //The host doesn't have a concept of multiple serial ports so instead of
    //using EMBER_ASSERT_SERIAL_PORT, just use a dummy value of 0.
    emberSerialGuaranteedPrintf(0, 
                                "\r\n[ASSERT:%p:%d]\r\n",
                                filename, 
                                linenumber);
  #endif
  
  halReboot();
}

uint8_t resetCause = RESET_UNSET;
uint8_t halGetResetInfo(void)
{
  //Since the reset flags in hardware need to be reset after they are read,
  //the flag needs to be stored in a global variable.  That global variable
  //is initialized to RESET_UNSET to know that it is unset and needs to be
  //saved off.
  if(resetCause != RESET_UNSET) {
    return resetCause;
  }
  
  //Since other resets feed the pin reset and there is no prioritization on
  //the reset bits, just return the first non-pin reset we can find.  There
  //is a chance this might not be entirely accurate.
  if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST) == SET) {
    resetCause = RESET_LOW_POWER;
  } else if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST) == SET) {
    resetCause = RESET_WINDOW_WATCHDOG;
  } else if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST) == SET) {
    resetCause = RESET_INDEPENDENT_WATCHDOG;
  } else if(RCC_GetFlagStatus(RCC_FLAG_SFTRST) == SET) {
    resetCause = RESET_SOFTWARE;
  } else if(RCC_GetFlagStatus(RCC_FLAG_PORRST) == SET) {
    resetCause = RESET_POR_PDR;
  } else if(RCC_GetFlagStatus(RCC_FLAG_PINRST) == SET) {
    resetCause = RESET_PIN;
  } else {
    resetCause = RESET_UNKNOWN;
  }
  
  RCC_ClearFlag();
  
  return resetCause;
}

PGM_P halGetResetString(void)
{
  static PGM_P resetString[] = { "unknown",
                                 "low-power",
                                 "window watchdog",
                                 "independent watchdog",
                                 "software",
                                 "POR/PDR",
                                 "pin" };
  
  return resetString[halGetResetInfo()];
}

//Use stdlib random numbers.
void halStackSeedRandom(uint32_t seed)
{
  srand(seed);
}

//Use stdlib random numbers.
uint16_t halCommonGetRandom(void)
{
  return (uint16_t)rand();
}

void halInternalInitSysTick(void)
{
  // Select AHB clock as SysTick Clock Source
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

//NOTE: This implmentation of halCommonDelayMicroseconds is based on the
//SysTick timer.  If you require the SysTick timer for another purpose, you 
//will need to implement halCommonDelayMicroseconds in another fashion.
void halCommonDelayMicroseconds(uint16_t us)
{
  if(us < 3) {
    //Don't try to delay if the duration is less than our overhead.
    return;
  }
  
  //Configure SysTick to count down to zero every microsecond using the
  //core clock but no interrupt. (The -1 is necessary to satisfy SysTick
  //behavior when using the timer as multishot.)
  SysTick->LOAD = (72-1);
  SysTick->VAL = 0;
  (uint32_t)SysTick->CTRL; //ensure COUNTFLAG is cleared
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE | 
                   SysTick_CTRL_ENABLE;
  
  while(us-->0) {
    while((SysTick->CTRL & SysTick_CTRL_COUNTFLAG) !=
          SysTick_CTRL_COUNTFLAG) {}
  }
  
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE;
}

//Burning cycles for milliseconds is generally a bad idea, but it is
//necessary in some situations.  If you have to burn more than 65ms of time,
//the halCommonDelayMicroseconds function becomes cumbersome, so this
//function gives you millisecond granularity.
void halCommonDelayMilliseconds(uint16_t ms)
{
  if(ms==0) {
    return;
  }
  
  while(ms-->0) {
    halCommonDelayMicroseconds(1000);
  }
}

//NOTE: The STM32F103RET host does not currently support any sleep modes other
//      than SLEEPMODE_MAINTAINTIMER.  Since RUNNING and IDLE closest resemble
//      simply returning, that is what they do.  WAKETIMER and NOTIMER
//      more closely match MAINTAINTIMER so they redirect to that mode.
void halSleep(SleepModes sleepMode)
{
  //SLEEPMODE_POWERDOWN and SLEEPMODE_POWERSAVE are deprecated.  Remap them
  //to their appropriate, new mode name.
  if(sleepMode == SLEEPMODE_POWERDOWN) {
    sleepMode = SLEEPMODE_MAINTAINTIMER;
  } else if(sleepMode == SLEEPMODE_POWERSAVE) {
    sleepMode = SLEEPMODE_WAKETIMER;
  }

  switch(sleepMode) {
    case SLEEPMODE_RUNNING:
    case SLEEPMODE_IDLE:
    default:
      return;
    case SLEEPMODE_WAKETIMER:
    case SLEEPMODE_NOTIMER:
    case SLEEPMODE_MAINTAINTIMER:
      //The STM32 datasheet states:
      //  "In Stop mode, all clocks in the 1.8 V domain are stopped, the PLL,
      //   the HSI and the HSE RC oscillators are disabled. SRAM and register
      //   contents are preserved."
      //This means halSleep is only responsible for restoring the PLL, HSI,
      //and HSE RC.
      
      //Put the STM32 into deepsleep.
      PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
      
      //SystemInit() is the function called before main() to configure the clocks.
      //It is the simplest means of restoring our PLL/HSI/HSE/SYSCLK state.  If
      //any conde changes this state, it should manually restore it here instead
      //of calling SystemInit();
      SystemInit();
  };
}

