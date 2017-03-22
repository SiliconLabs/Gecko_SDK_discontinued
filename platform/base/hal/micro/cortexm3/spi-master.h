/** @file hal/micro/cortexm3/spi-master.h
 *  @brief EM3XX SPI Master Driver API
 *
 * <!-- Copyright 2013 Silicon Laboratories, Inc.                        *80*-->
 */

#ifndef __SPI_MASTER_H__
#define __SPI_MASTER_H__

//== DEFINITIONS ==

#define SPI_IDLE_BYTE       0xFF  // Byte sent/received as dummy pad

#define SPI_NSEL_EXTERNAL   0xFF  // SCx_NSEL set to this tells driver
                                  // that NSEL will be managed externally
                                  // and not within the SPI driver itself

#ifdef  SC4_RXBEGA // Existence of this #define indicates SC3 and SC4 exist
 #define SC3_AND_SC4_EXIST 1
#endif//SC4_RXBEGA


//== DATA TYPES ==

// SPI Ports:
// 0 = BitBang (BOARD_HEADER defines SC0_MOSI, SC0_MISO, SC0_SCLK, SC0_NSEL)
// 1 = SC1 @0x4000C800 (MOSI=PB1, MISO=PB2, SCLK=PB3, NSEL=PB4)
// 2 = SC2 @0x4000C000 (MOSI=PA0, MISO=PA1, SCLK=PA2, NSEL=PA3)
// 3 = SC3 @0x4000D800 (MOSI=PD1, MISO=PD2, SCLK=PD3, NSEL=PD4)
// 4 = SC4 @0x4000D000 (MOSI=PE0, MISO=PE1, SCLK=PE2, NSEL=PE3)
// BOARD_HEADER can override default choice of SCx_NSEL.

/** @brief SPI port choices
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef uint8_t EmberSpiPort;
enum
#else//!DOXYGEN_SHOULD_SKIP_THIS
enum EmberSpiPort
#endif//DOXYGEN_SHOULD_SKIP_THIS
{
  EMBER_SPI_PORT_BITBANG = 0,
  EMBER_SPI_PORT_SC1,
  EMBER_SPI_PORT_SC2,
 #if     SC3_AND_SC4_EXIST
  EMBER_SPI_PORT_SC3,
  EMBER_SPI_PORT_SC4,
 #endif//SC3_AND_SC4_EXIST
  EMBER_SPI_PORT_MAX  // Must be last
};

/** @brief SPI clock speed choices
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef uint8_t EmberSpiClkRate;
enum
#else//!DOXYGEN_SHOULD_SKIP_THIS
enum EmberSpiClkRate
#endif//DOXYGEN_SHOULD_SKIP_THIS
{ // Encodes SCx_RATELIN in upper nibble, SCx_RATEEXP in lower nibble
  EMBER_SPI_CLK_12_MHZ  = 0x00,
  EMBER_SPI_CLK_6_MHZ   = 0x10,
  EMBER_SPI_CLK_4_MHZ   = 0x20,
  EMBER_SPI_CLK_3_MHZ   = 0x30,
  EMBER_SPI_CLK_2_MHZ   = 0x21,
  EMBER_SPI_CLK_1_MHZ   = 0x22,
  EMBER_SPI_CLK_750_KHZ = 0x13,
  EMBER_SPI_CLK_500_KHZ = 0x23,
  EMBER_SPI_CLK_250_KHZ = 0x53,
  EMBER_SPI_CLK_100_KHZ = 0xE3,
  EMBER_SPI_CLK_50_KHZ  = 0xE4,
  EMBER_SPI_CLK_CUSTOM  = 0xFF, // EMBER_SPI_CLK_CUSTOM_SETTING in BOARD_HEADER
};

/** @brief SPI clocking moded choices (encoding polarity and phase)
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef uint8_t EmberSpiClkMode;
enum
#else//!DOXYGEN_SHOULD_SKIP_THIS
enum EmberSpiClkMode
#endif//DOXYGEN_SHOULD_SKIP_THIS
{ //                      Polarity       Phase
  EMBER_SPI_CLK_MODE_0 = (0             |0             ), // 0
  EMBER_SPI_CLK_MODE_1 = (0             |SC_SPIPHA_MASK), // 2
  EMBER_SPI_CLK_MODE_2 = (SC_SPIPOL_MASK|0             ), // 1
  EMBER_SPI_CLK_MODE_3 = (SC_SPIPOL_MASK|SC_SPIPHA_MASK), // 3
};

/** @brief SPI transfer operation flags
 */
#ifndef DOXYGEN_SHOULD_SKIP_THIS
typedef uint8_t EmberSpiOpFlags;
enum
#else//!DOXYGEN_SHOULD_SKIP_THIS
enum EmberSpiOpFlags
#endif//DOXYGEN_SHOULD_SKIP_THIS
{ // Operation flags
  EMBER_SPI_OP_NO_FLAGS  = 0x00, // Normal SPI operation
  EMBER_SPI_OP_SPLIT_LEN = 0x0F, // Split transaction length field mask
  EMBER_SPI_OP_SPLIT_TRX = 0x10, // Split transaction rxBuf really for Tx only
  EMBER_SPI_OP_KEEP_SEL  = 0x20, // Keep slave selected after operation
  EMBER_SPI_OP_ASYNC_IO  = 0x40, // Async IO -- do not wait for completion
  EMBER_SPI_OP_NO_DMA    = 0x80, // Avoid using DMA (e.g. if txBuf isn't in RAM)
};

/** @brief SPI transfer complete callback
 * @param  port     serial controller port that completed
 * @param  trLen    length of the transfer, as passed to halSpiMasterTransferBuf()
 * @param  rxBuf    data received, as passed into halSpiMasterTransferBuf()
 */
typedef void (*EmberSpiTransferCallback)(EmberSpiPort port,
                                         uint16_t trLen,
                                         uint8_t* rxBuf);

//== API FUNCTIONS ==

/** @brief Initialize SPI Master port
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @param  clkRate  SPI clock frequency to use per EmberSpiClkRate choices
 * @param  clkMode  SPI clocking mode to use per Emb erSpiClkMode choices
 * @param  lsbFirst true to send bytes LSB-first, false for MSB-first
 * @return EMBER_SUCCESS if everything goes well, something else if not
 */
extern EmberStatus halSpiMasterInit(EmberSpiPort port,
                                    EmberSpiClkRate clkRate,
                                    EmberSpiClkMode clkMode,
                                    bool lsbFirst);

/** @brief Returns SPI port busy (transfer in progress)
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @return true if SPI port is busy, false if idle (and will complete prior
 *         transaction per its opFlags.
 */
extern bool halSpiMasterIsBusy(EmberSpiPort port);

/** @brief Transfer buffer over SPI
 * @param  port     serial controller port to init (0=bitbang, 1=SC1, ...)
 * @param  trLen    number of bytes to transfer
 * @param  txBuf    buffer to transmit; may be NULL for Rx-only
 * @param  rxBuf    buffer for receive; may be NULL for Tx-only, or same
 *                  as txBuf to (safely) overwrite Tx data with Rx data
 * @param  opFlags  flags (bitmask) for the transfer per EmberSpiOpFlags choices
 * @param  opDoneCB operation complete callback (NULL if don't want callback)
 * @return EMBER_SUCCESS if everything goes well, something else if not (and
 *                  callback will not be called)
 * @note A zero-len transfer will still manipulate nSEL per opFlags, even
 * though no data will be moved.
 */
extern EmberStatus halSpiMasterTransferBuf(EmberSpiPort port,
                                           uint16_t trLen,
                                           uint8_t* txBuf,
                                           uint8_t* rxBuf,
                                           EmberSpiOpFlags opFlags,
                                           const EmberSpiTransferCallback opDoneCB);

/** @brief Some convenience macros for Tx-only and Rx-only transfers
 */
#define halSpiMasterTxBuf(port, len, txBuf, opFlags, opDoneCB) \
  halSpiMasterTransferBuf((port), (len), (txBuf), NULL, (opFlags), (opDoneCB))

#define halSpiMasterRxBuf(port, len, rxBuf, opFlags, opDoneCB) \
  halSpiMasterTransferBuf((port), (len), NULL, (rxBuf), (opFlags), (opDoneCB))

#define halSpiMasterSelectSlave(port) \
  halSpiMasterTransferBuf((port), 0, NULL, NULL, EMBER_SPI_OP_KEEP_SEL, NULL)

#define halSpiMasterDeselectSlave(port) \
  halSpiMasterTransferBuf((port), 0, NULL, NULL, EMBER_SPI_OP_NO_FLAGS, NULL)

#endif//__SPI_MASTER_H__
