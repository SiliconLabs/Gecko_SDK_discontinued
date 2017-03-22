/***************************************************************************//**
 * @file btl_storage_spiflash.h
 * @brief Spiflash-backed storage plugin for Silicon Labs Bootloader.
 * @author Silicon Labs
 * @version 1.0.0
 *******************************************************************************
 * # License
 * <b>Copyright 2016 Silicon Laboratories, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef BTL_STORAGE_SPIFLASH_H
#define BTL_STORAGE_SPIFLASH_H

/***************************************************************************//**
 * @addtogroup Storage
 * @{
 * @page storage_spiflash SPI Flash
 *   The SPI Flash storage implementation supports a variety of SPI flash parts,
 *   including:
 *   - Spansion S25FL208K (8Mbit)
 *   - Winbond W25X20BV (2Mbit), W25Q80BV (8Mbit)
 *   - Macronix MX25L2006E (2Mbit), MX25L4006E (4Mbit), MX25L8006E (8Mbit),
 *     MX25R8035F (8Mbit low power), MX25L1606E (16Mbit),
 *     MX25U1635E (16Mbit 2Volt), MX25R6435F (64Mbit low power)
 *   - Atmel/Adesto AT25DF041A (4Mbit), AT25DF081A (8Mbit)
 *   - Numonyx/Micron M25P20 (2Mbit), M25P40 (4Mbit), M25P80 (8Mbit),
 *     M25P16 (16Mbit)
 *   - ISSI IS25LQ025B (256Kbit), IS25LQ512B (512Kbit), IS25LQ010B (1Mbit),
 *     IS25LQ020B (2Mbit), IS25LQ040B (4Mbit)
 *
 *   The subset of devices to include support for can be configured at compile
 *   time using the configuration defines given in @ref SpiflashConfigs.
 *   Including support for multiple devices requires more flash space in
 *   the bootloader.
 *
 *   The SPI Flash storage implementation does not support any write protection
 *   functionality.
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

// -----------------------------------------------------------------------------
// Typedefs

/// Unique identifiers for supported SPI flash parts
typedef enum {
  UNKNOWN_DEVICE,
  SPANSION_8M_DEVICE,
  WINBOND_2M_DEVICE,
  WINBOND_8M_DEVICE,
  MACRONIX_2M_DEVICE,
  MACRONIX_4M_DEVICE,
  MACRONIX_8M_DEVICE,
  MACRONIX_8M_LP_DEVICE,
  MACRONIX_16M_DEVICE,
  MACRONIX_16M_2V_DEVICE,
  MACRONIX_64M_LP_DEVICE,
  ATMEL_4M_DEVICE,
  ATMEL_8M_DEVICE,
  //N.B. If add more ATMEL_ devices, update storage_init() accordingly
  ADESTO_4M_DEVICE,
  NUMONYX_2M_DEVICE,
  NUMONYX_4M_DEVICE,
  NUMONYX_8M_DEVICE,
  NUMONYX_16M_DEVICE,
  //N.B. If add more NUMONYX_ devices, update storage_eraseSlot() accordingly
  ISSI_256K_DEVICE,
  ISSI_512K_DEVICE,
  ISSI_1M_DEVICE,
  ISSI_2M_DEVICE,
  ISSI_4M_DEVICE,
} StorageSpiflashDevice_t;

// -----------------------------------------------------------------------------
// Defines


// -----------------------------------------------------------------------------
// Functions

/** @endcond */

/** @} // addtogroup storage */

#endif // BTL_STORAGE_SPIFLASH_H
