/***************************************************************************//**
 * @file btl_second_stage.c
 * @brief Main file for Main Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * @section License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "config/btl_config.h"
#include "api/btl_interface.h"

#include "core/btl_core.h"
#include "core/btl_reset.h"
#include "core/btl_parse.h"
#include "core/btl_bootload.h"
#include "core/btl_upgrade.h"

#include "plugin/debug/btl_debug.h"
#include "plugin/gpio-activation/btl_gpio_activation.h"

#ifdef BOOTLOADER_SUPPORT_STORAGE
#include "plugin/storage/btl_storage.h"
#include "plugin/storage/bootloadinfo/btl_storage_bootloadinfo.h"
#endif

#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
#include "plugin/communication/btl_communication.h"
#endif

#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_chip.h"

#if defined(__GNUC__)
#define ROM_END_SIZE 0
extern const size_t __rom_end__;
#elif defined(__ICCARM__)
#define ROM_END_SIZE 4
const size_t __rom_end__ @ "ROM_SIZE";
#endif

// --------------------------------
// Local function declarations
__STATIC_INLINE bool enterBootloader(void);
SL_NORETURN static void bootToApp(uint32_t);

void HardFault_Handler(void)
{
  BTL_DEBUG_PRINTLN("Fault          ");
  reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
}

// Main Bootloader implementation

int main(void)
{
  int32_t ret;

  // Coming out of reset...
  SystemCoreClock = SystemHfrcoFreq;

  CHIP_Init();
  /*
  // Enabling HFXO will add a hefty code size penalty (~1k)!
  CMU_HFXOInit_TypeDef hfxoInit = CMU_HFXOINIT_DEFAULT;
  CMU_HFXOInit(&hfxoInit);
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
  */

  BTL_DEBUG_PRINTLN("BTL entry");

#if defined(EMU_CMD_EM01VSCALE2)
  // Device supports voltage scaling, and the bootloader may have been entered
  // with a downscaled voltage. Scale voltage up to allow flash programming.
  EMU->CMD = EMU_CMD_EM01VSCALE2;
  while (EMU->STATUS & EMU_STATUS_VSCALEBUSY);
#endif

  btl_init();
  reset_invalidateResetReason();

#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
  communication_init();

  if((ret = communication_start()) != BOOTLOADER_OK) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }

  ret = communication_main();
  BTL_DEBUG_PRINT("Protocol returned ");
  BTL_DEBUG_PRINT_WORD_HEX(ret);
  BTL_DEBUG_PRINT_LF();

  communication_shutdown();

  if (ret == BOOTLOADER_OK) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  } else if (ret == BOOTLOADER_ERROR_COMMUNICATION_IMAGE_ERROR) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
  } else if (ret == BOOTLOADER_ERROR_COMMUNICATION_DONE) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  } else {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }
#endif // BOOTLOADER_SUPPORT_COMMUNICATION

#ifdef BOOTLOADER_SUPPORT_STORAGE

  ret = storage_main();

  if (ret == BOOTLOADER_OK) {
    reset_resetWithReason(BOOTLOADER_RESET_REASON_GO);
  } else if (ret == BOOTLOADER_ERROR_STORAGE_BOOTLOAD) {
    // Bootload attempt failed. Wait a short while before rebooting.
    // This prevents the reset loop from being so tight that a debugger is
    // unable to reattach to flash a new app when neither the app nor the
    // contents of storage are valid.
    for (volatile int i = 10000000; i > 0; i--);
    reset_resetWithReason(BOOTLOADER_RESET_REASON_BADIMAGE);
  } else {
    for (volatile int i = 10000000; i > 0; i--);
    reset_resetWithReason(BOOTLOADER_RESET_REASON_FATAL);
  }
#endif

}

#ifdef BOOTLOADER_SUPPORT_STORAGE
extern const BootloaderStorageFunctions_t storageFunctions;
#endif

const MainBootloaderTable_t mainStageTable = {
  {
    .type = BOOTLOADER_MAGIC_MAIN,
    .layout = BOOTLOADER_HEADER_VERSION_MAIN,
    .version = BOOTLOADER_VERSION_MAIN
  },
  // Bootloader size is the relative address of the end variable plus 4 for the CRC
  .size = ((uint32_t)&__rom_end__) - BTL_MAIN_STAGE_BASE + ROM_END_SIZE + 4,
  .startOfAppSpace = (BareBootTable_t *)(BTL_APPLICATION_BASE),
  .endOfAppSpace = (void *)(BTL_APPLICATION_BASE + BTL_APP_SPACE_SIZE),
  .capabilities = (0
#ifdef BOOTLOADER_ENFORCE_SIGNED_UPGRADE
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_SIGNATURE
#endif
#ifdef BOOTLOADER_ENFORCE_ENCRYPTED_UPGRADE
                   | BOOTLOADER_CAPABILITY_ENFORCE_UPGRADE_ENCRYPTION
#endif
#ifdef BOOTLOADER_ENFORCE_SECURE_BOOT
                   | BOOTLOADER_CAPABILITY_ENFORCE_SECURE_BOOT
#endif
                   | BOOTLOADER_CAPABILITY_BOOTLOADER_UPGRADE
                   | BOOTLOADER_CAPABILITY_EBL
                   | BOOTLOADER_CAPABILITY_EBL_SIGNATURE
                   | BOOTLOADER_CAPABILITY_EBL_ENCRYPTION
#ifdef BOOTLOADER_SUPPORT_STORAGE
                   | BOOTLOADER_CAPABILITY_STORAGE
#endif
#ifdef BOOTLOADER_SUPPORT_COMMUNICATION
                   | BOOTLOADER_CAPABILITY_COMMUNICATION
#endif
                   ),
  .init = &btl_init,
  .deinit = &btl_deinit,
  .verifyApplication = &bootload_verifyApplication,
  .initParser = &core_initParser,
  .parseBuffer = &core_parseBuffer,
#ifdef BOOTLOADER_SUPPORT_STORAGE
  .storage = &storageFunctions
#else
  .storage = NULL
#endif
};

/**
* This function gets executed before ANYTHING got initialized.
* So, no using global variables here!
*/
void SystemInit2(void)
{
  // Initialize debug before first debug print
  BTL_DEBUG_INIT();

  // Assumption: We should enter the app
  bool enterApp = true;
  bool verifyApp;
  // Assumption: The app should be verified if
#ifdef BOOTLOADER_ENFORCE_SECURE_BOOT
  verifyApp = true;
#else
  verifyApp = false;
#endif

  // Check if we came from EM4. If any other bit than the EM4 bit it set, we
  // can't know whether this was really an EM4 reset, and we need to do further
  // checking.
  if(RMU->RSTCAUSE == RMU_RSTCAUSE_EM4RST) {
    // We came from EM4, app doesn't need to be verified
    verifyApp = false;
  } else if (enterBootloader()) {
    // We want to enter the bootloader, app doesn't need to be verified
    enterApp = false;
    verifyApp = false;
  }

  uint32_t startOfAppSpace = (uint32_t)mainStageTable.startOfAppSpace;

  // Sanity check application program counter
  uint32_t pc = *(uint32_t *)(startOfAppSpace + 4);
  if (pc == 0xFFFFFFFF) {
    // Sanity check failed; enter the bootloader
    reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
    enterApp = false;
    verifyApp = false;
  }

  // App should be verified
  if (verifyApp) {
    // If app verification fails, enter bootloader instead
    enterApp = bootload_verifyApplication(startOfAppSpace);
    if (!enterApp) {
      reset_setResetReason(BOOTLOADER_RESET_REASON_BADAPP);
    }
  }

  if (enterApp) {
    BTL_DEBUG_PRINTLN("Enter app");
    BTL_DEBUG_PRINT_LF();

#if defined(MSC_BOOTLOADERCTRL_BLWDIS) && defined(BOOTLOADER_WRITE_DISABLE)
    // Disable write access to bootloader. Prevents application from touching
    // the bootloader.
    MSC->BOOTLOADERCTRL |= MSC_BOOTLOADERCTRL_BLWDIS;
#endif

    // Set vector table to application's table
    SCB->VTOR = startOfAppSpace;

    bootToApp(startOfAppSpace);
  }
  // Enter bootloader
}

/**
 * Jump to app
 */
SL_NORETURN static void bootToApp(uint32_t startOfAppSpace)
{
  (void)startOfAppSpace;

  // Load SP and PC of application
  __ASM("mov r0, %0       \n" // Load address of SP into R0
        "ldr r1, [r0]     \n" // Load SP into R1
        "msr msp, r1      \n" // Set MSP
        "msr psp, r1      \n" // Set PSP
        "ldr r0, [r0, #4] \n" // Load PC into R0
        "mov pc, r0       \n" // Set PC
        :: "r"(startOfAppSpace) : "r0","r1");

  while (1);
}

/**
 * Check whether we should enter the bootloader
 *
 * @return True if the bootloader should be entered
 */
__STATIC_INLINE bool enterBootloader(void)
{
  if (RMU->RSTCAUSE & RMU_RSTCAUSE_SYSREQRST) {
    // Check if we were asked to run the bootloader...
    switch (reset_classifyReset()) {
      case BOOTLOADER_RESET_REASON_BOOTLOAD:
      case BOOTLOADER_RESET_REASON_FORCE:
      case BOOTLOADER_RESET_REASON_UPGRADE:
      case BOOTLOADER_RESET_REASON_BADAPP:
        // Asked to go into bootload mode
        return true;
      default:
        break;
    }
  }

#ifdef BTL_PLUGIN_GPIO_ACTIVATION
  if (gpio_enterBootloader()) {
    // GPIO pin state signals bootloader entry
    return true;
  }
#endif

  return false;
}
