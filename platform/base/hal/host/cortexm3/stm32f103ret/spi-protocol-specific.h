/** @file hal/host/cortexm3/stm32f103ret/spi-protocol-specific.h
 * See @ref spi_protocol and @ref stm32f103ret_spip for documentation.
 *
 * <!-- Copyright 2010 by Ember Corporation. All rights reserved.        *80*-->
 */

/** @addtogroup stm32f103ret_spip
 * @brief Example host specific SPI Protocol implementation for
 * interfacing with a NCP.
 *
 * For complete documentation of the SPI Protocol, refer to the NCP docs.
 *
 * See @ref spi_protocol for common documentation.
 *
 * The definitions in the micro specific header provide the necessary
 * pieces to link the common functionality to a specific micro.
 *
 * See spi-protocol-specific.h for source code.
 *@{
 */

#ifndef __SPI_PROTOCOL_SPECIFIC_H__
#define __SPI_PROTOCOL_SPECIFIC_H__

/**
 * @name SPI Protocol Interface
 */
//@{

/** @brief The actual port that nSSEL is connected to, PA4, which is
 * configured as a general purpose output.
 */
#define SPIP_nSSEL_PORT     GPIOA
/** @brief The actual pin that nSSEL is connected to, PA4, which is
 * configured as a general purpose output.
 */
#define SPIP_nSSEL_PIN      GPIO_Pin_4


/** @brief The actual port that MOSI is connected to, PA7, which is
 * configured as alternate function push-pull.
 */
#define SPIP_MOSI_PORT      GPIOA
/** @brief The actual pin that MOSI is connected to, PA7, which is
 * configured as alternate function push-pull.
 */
#define SPIP_MOSI_PIN       GPIO_Pin_7

/** @brief The actual port that MISO is connected to, PA6, which is
 * configured as input with pull-up.
 */
#define SPIP_MISO_PORT      GPIOA
/** @brief The actual pin that MISO is connected to, PA6, which is
 * configured as input with pull-up.
 */
#define SPIP_MISO_PIN       GPIO_Pin_6

/** @brief The actual port that SCLK is connected to, PA5, which is
 * configured as alternate functon push-pull.
 */
#define SPIP_SCLK_PORT      GPIOA
/** @brief The actual pin that SCLK is connected to, PA5, which is
 * configured as alternate functon push-pull.
 */
#define SPIP_SCLK_PIN       GPIO_Pin_5

/** @brief The actual port that nHOST_INT is connected to, PC4, which is
 * configured as input with pull-up; EXTI4 interrupt, falling edge.
 */
#define SPIP_nHOST_INT_PORT GPIOC
/** @brief The actual pin that nHOST_INT is connected to, PC4, which is
 * configured as input with pull-up; EXTI4 interrupt, falling edge.
 */
#define SPIP_nHOST_INT_PIN  GPIO_Pin_4

/** @brief The actual port that nWAKE is connected to, PC5, which is
 * configured as general purpose output.
 */
#define SPIP_nWAKE_PORT     GPIOC
/** @brief The actual pin that nWAKE is connected to, PC5, which is
 * configured as general purpose output.
 */
#define SPIP_nWAKE_PIN      GPIO_Pin_5

/** @brief The actual port that nRESET is connected to, PB0, which is
 * configured as general purpose output.
 */
#define SPIP_nRESET_PORT    GPIOB
/** @brief The actual pin that nRESET is connected to, PB0, which is
 * configured as general purpose output.
 */
#define SPIP_nRESET_PIN     GPIO_Pin_0

/** @} END SPI Protocol Interface */


/**
 * @name SPI Protocol timing parameters.
 *
 * @note Remember: TIM2 is configured to produce a 125us tick.
 *@{
 */

 /** @brief Wait section timeout is 300ms.
 */
#define WAIT_SECTION_TIMEOUT    (2400) //300ms //Increased per BugzId:14656

/** @brief Wait handshake timeout is 300ms.
 */
#define WAKE_HANDSHAKE_TIMEOUT  (2400) //300ms //Increased per BugzId:14655,6

/** @brief Startup timeout is 7500ms.
 */
#define STARTUP_TIMEOUT        (60000) //7500ms

/** @brief Intercommand spacing is 1ms.
 */
#define INTER_COMMAND_SPACING      (8) //1ms

/** @brief The time to assert nRESET is 26 microseconds.
 */
#define NCP_RESET_DELAY  (26)

/** @} END SPI Protocol timing parameters */


#endif // __SPI_PROTOCOL_SPECIFIC_H__

/** @} END addtogroup */

