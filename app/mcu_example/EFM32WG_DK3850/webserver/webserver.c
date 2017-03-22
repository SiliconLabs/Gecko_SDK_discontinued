/**************************************************************************//**
 * @file
 * @brief Micrel KSZ8851SNL Ethernet controller example
 *    This example demonstrates use of the Micrel KSZ8851SNL MAC+PHY available on
 *    DK3850. lwIP and a simple web server is implemented.
 * @version 5.1.2
 ******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdio.h>

#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_gpio.h"

#include "bsp.h"

#include "ksz8851snl.h"
#include "retargetserial.h"
#include "tftconsole.h"

#include "ksz8851snl_driver.h"
#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/mem.h"
#include "lwip/ip.h"
#include "lwip/stats.h"
#include "lwip/timers.h"

#include "httpd.h"
#include "echo.h"

#include "lwipopts.h"
#include "debug.h"

#ifndef LWIP4EFM32
#error "lwiopts.h for EFM32 are not included"
#endif

static volatile uint32_t msTicks = 0; /**< counts 1ms timeTicks */
static struct netif * en0;            /**< first ethernet interface */

/* time counters */
static uint32_t lastMIBTime  = 0;
static uint32_t lastStatTime = 0;

static TFT_Pos rxStatPos = {-1, -1};
static TFT_Pos txStatPos = {-1, -1};
static TFT_Pos infoPos   = {-1, -1};

/* timeout for MIB counters readout and counters accumulation */
#define MIB_UPDATE_INTERVAL     30000
#define STAT_UPDATE_INTERVAL    1000
#define LINK_TIMEOUT            10000
#define AN_DONE                 0x0060
#define LINKGOOD_MASK           0x0020
#define LINKSPEED_MASK          0x0400
#define LINKDUPLEX_MASK         0x0200
#define CLEAR_LINKUP_MASK       0x0008

#define LINK_LED_NO             0
#define TICK_LED_NO             1

#if DEBUG_EFM
#define DEBUG_Print(...)    printf(__VA_ARGS__)
#else
#define DEBUG_Print(...)
#endif

/* Event flags for ETH controller */
static volatile bool ethEvent = false;
/* Link status from P1SR register */
static uint16_t phyStatus;

/***************************************************************************//**
 * @brief
 *   Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;
  if (msTicks % 1000 == 0)
  {
    BSP_LedToggle(TICK_LED_NO);
  }
}

/***************************************************************************//**
 * @brief
 *   Handle GPIO interrupt on PE0
 *
 * @note
 *   The interrupt handler checks the GPIO interrupt flags from the Board
 *   Controller registers and if BC_INTFLAG_ETH is asserted we signal that
 *   there is an ethernet event ready to be processed.
 ******************************************************************************/
void GPIO_EVEN_IRQHandler(void)
{
  uint16_t bcFlags;

  /* Clear GPIO interrupt */
  GPIO_IntClear(1 << BSP_GPIO_INT_PIN);

  /* Get and clear board controller interrupt flags */
  bcFlags = BSP_InterruptFlagsGet();
  BSP_InterruptFlagsClear(bcFlags);

  if (bcFlags & BC_INTFLAG_ETH)
  {
    /* Signal to the application that an event has happened and the
     * ethernet controller needs to be serviced.
     */
    ethEvent = true;
  }
}

/***************************************************************************//**
 * @brief
 *   Returns the current timestamp.
 *
 * @note
 *   It is required for lwIP and is defined in lwip-1.4.1/src/include/lwip/sys.h
 *
 * @return
 *   current timestamp value
 ******************************************************************************/
uint32_t sys_now(void)
{
  return msTicks;
}

/***************************************************************************//**
 * @brief
 *   Delays number of milliseconds
 *
 * @param[in] ms
 *   Number of milliseconds to delay
 ******************************************************************************/
static void delay(uint32_t ms)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < ms);
}

/***************************************************************************//**
 * @brief
 *   Enable the BoardController to generate GPIO interrupt
 *
 * @details
 *   The ethernet controller has an interrupt pin which is routed through the
 *   board controller to the EFM32. In this function we configure the board
 *   controller to enable only the interrupt from the ethernet controller.
 *   This will make pin PE0 go low on the EFM32 when the ethernet controller
 *   has issued an interrupt.
 ******************************************************************************/
static void bspIrqInit(void)
{
  /* Enable interrupts on ETH events only */
  BSP_InterruptDisable(0xffff);
  BSP_InterruptFlagsClear(0xffff);
  BSP_InterruptEnable(BC_INTEN_ETH);
}

/***************************************************************************//**
 * @brief
 *   Initialize GPIO interrupt from Board Controller/Ethernet controller
 *
 * @details
 *   We enable interrupt generation on a falling edge on pin PE0. On the
 *   development kit this pin is connected to the board controller interrupt
 *   pin, and since we have only enabled the ethernet controller interrupt
 *   inside the board controller this will mean that we only get interrupt
 *   requests when the ethernet controller needs attention.
 ******************************************************************************/
static void gpioIrqInit(void)
{
  /* Configure interrupt pin as input with pull-up */
  GPIO_PinModeSet(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, gpioModeInputPull, 1);

  /* Set falling edge interrupt and clear/enable it */
  GPIO_IntConfig(BSP_GPIO_INT_PORT, BSP_GPIO_INT_PIN, false, true, true);

  NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
  NVIC_EnableIRQ(GPIO_EVEN_IRQn);
}

/***************************************************************************//**
 * @brief
 *   Hard reset the ethernet controller
 ******************************************************************************/
static void resetEthernetController(void)
{
  BSP_RegisterWrite(&BC_REGISTER->CLKRST, BC_CLKRST_ETH_SHIFT);
  /* Wait for set to be detected */
  delay(10);
  BSP_RegisterWrite(&BC_REGISTER->CLKRST, 0x00);
  delay(10);
}

/***************************************************************************//**
 * @brief
 *   User command line interface
 *
 * @detail
 *   Provides a command line interface to the user that can be accessed via
 *   the serial console. This function does not block if there are no
 *   characters in the input buffer
 ******************************************************************************/
static void consoleRead(void)
{
  int c;

  /* non blocking char read, -1 for no char in buffer */
  c = RETARGET_ReadChar();
  if (c != -1)
  {
    switch (c)
    {
    case 'a':
      KSZ8851SNL_AllRegistersDump();
      break;
    case 'd':
      KSZ8851SNL_RegistersDump();
      break;
    case 'm':
      KSZ8851SNL_MIBCountersDump();
      break;
    case 'p':
      DEBUG_Print("KSZ8851SNL_GetPHYStatus() = 0x%04X \n", KSZ8851SNL_PHYStatusGet());
      break;
    case '?':
    default:
      if (c != '?')
      {
        DEBUG_Print("%c - unknown Command \n", c);
        DEBUG_Print("? - help \n");
      }
      DEBUG_Print("a - KSZ8851SNL_AllRegistersDump \n");
      DEBUG_Print("d - KSZ8851SNL_RegistersDump \n");
      DEBUG_Print("m - KSZ8851SNL_MIBCountersDump \n");
      DEBUG_Print("p - KSZ8851SNL_PHYStatusGet \n");
      break;
    }
  }
}

/***************************************************************************//**
 * @brief
 *   Display ip address on the uart and the tft display
 *
 * @note
 *   This will only display the ip address of the network interface if an ip
 *   address is assigned.
 *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
static void ipDisplay(struct netif *netif)
{
  if (!netif_is_up(netif))
  {
    return;
  }

  /* Print IP status on UART */
  DEBUG_Print("IP address  :%s\n", ipaddr_ntoa(&netif->ip_addr));
  DEBUG_Print("Subnet mask :%s\n", ipaddr_ntoa(&netif->netmask));
  DEBUG_Print("Gateway     :%s\n", ipaddr_ntoa(&netif->gw));
  DEBUG_Print("Link @ %s %s DUPLEX \n",
         (phyStatus & LINKSPEED_MASK) ? "100M" : "10M",
         (phyStatus & LINKDUPLEX_MASK) ? "FULL" : "HALF");

  /* Show IP status on the TFT Display */
  TFT_PosSet(&infoPos);
  TFT_Print("\n\t IP address  :%s\n", ipaddr_ntoa(&netif->ip_addr));
  TFT_Print("\n\t Subnet mask :%s\n", ipaddr_ntoa(&netif->netmask));
  TFT_Print("\n\t Gateway     :%s\n", ipaddr_ntoa(&netif->gw));
  TFT_Print("\n\t Link        :%s %s DUPLEX \n",
            (phyStatus & LINKSPEED_MASK) ? "100M" : "10M",
            (phyStatus & LINKDUPLEX_MASK) ? "FULL" : "HALF");
}

/***************************************************************************//**
 * @brief
 *   Link change callback function
 *
 * @notes
 *   This function is called by the lwIP stack when a network interface changes
 *   it's link status.
 *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
static void linkCallback(struct netif *netif)
{
  DEBUG_Print("%c%c%d link is %s\n",
      netif->name[0], netif->name[1], netif->num,
      (netif_is_link_up(netif) ? "up" : "down"));

  if (netif_is_link_up(netif))
  {
    BSP_LedSet(LINK_LED_NO);
    phyStatus = KSZ8851SNL_PHYStatusGet();
  }
  else
  {
    BSP_LedClear(LINK_LED_NO);
  }
}

/***************************************************************************//**
 * @brief
 *   Status change callback function
 *
 * @notes
 *   This function is called by the lwIP stack when a network interface changes
 *   it's network status. When an interface is up it means that it has aquired
 *   an ip address and is ready to send and receive packets.
 *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
static void statusCallback(struct netif *netif)
{
  DEBUG_Print("%c%c%d interface is %s\n",
      netif->name[0], netif->name[1], netif->num,
      (netif_is_up(netif) ? "up" : "down"));

  ipDisplay(netif);
}

/***************************************************************************//**
 * @brief
 *   Initiates the network interface.
 *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
static void netifInit(struct netif *netif)
{
  /* Initialize the IP address fields */
  ip_addr_t ipaddr, netmask, gw;

#if LWIP_DHCP
  ip_addr_set_zero(&ipaddr);
  ip_addr_set_zero(&netmask);
  ip_addr_set_zero(&gw);
#else
  IP4_ADDR(&ipaddr, STATIC_IPADDR0, STATIC_IPADDR1, STATIC_IPADDR2, STATIC_IPADDR3);
  IP4_ADDR(&netmask, STATIC_NET_MASK0, STATIC_NET_MASK1, STATIC_NET_MASK2, STATIC_NET_MASK3);
  IP4_ADDR(&gw, STATIC_GW_IPADDR0, STATIC_GW_IPADDR1, STATIC_GW_IPADDR2, STATIC_GW_IPADDR3);
#endif

  /* Add the current network interface to the lwIP list */
  netif_add(netif, &ipaddr, &netmask, &gw, NULL, ksz8851snl_driver_init, ethernet_input);
  /* Set the current network interface as default */
  netif_set_default(netif);
  netif_set_link_callback(netif, linkCallback);
  netif_set_status_callback(netif, statusCallback);

#if LWIP_DHCP
  DEBUG_Print("Using DHCP configuration\n");
  dhcp_start(netif);
#else
  DEBUG_Print("Using static IP configuration\n");
  netif_set_up(netif);
#endif

  delay(5);
  KSZ8851SNL_Enable();
}

/***************************************************************************//**
 * @brief
 *   Update MIB counters from the ethernet controller and display the current
 *   rx/tx counters from the stack
 ******************************************************************************/
static void statisticUpdate(void)
{
  if (msTicks - lastMIBTime >= MIB_UPDATE_INTERVAL)
  {
    lastMIBTime = msTicks;
    KSZ8851SNL_MIBCountersUpdate();
  }
  if (msTicks - lastStatTime >= STAT_UPDATE_INTERVAL)
  {
    lastStatTime = msTicks;
    if ((rxStatPos.x == -1) && (rxStatPos.y == -1))
    {
      TFT_PosGet(&rxStatPos);
    }
    TFT_PosSet(&rxStatPos);
    TFT_Print("\n\t Packet Rx   : %09d \n", lwip_stats.link.recv);

    if ((txStatPos.x == -1) && (txStatPos.y == -1))
    {
      TFT_PosGet(&txStatPos);
    }
    TFT_PosSet(&txStatPos);
    TFT_Print("\n\t Packet Tx   : %09d \n", lwip_stats.link.xmit);
  }
}

/***************************************************************************//**
 * @brief
 *   Main loop services the ethernet events and service the stack
 *
 * @param[in] netif
 *   The network interface
 ******************************************************************************/
static void mainLoop(struct netif *netif)
{
  while (true)
  {
    if (ethEvent)
    {
      ethEvent = false;
      ksz8851snl_driver_isr(netif);
    }

    if (netif_is_link_up(netif))
    {
      sys_check_timeouts();

      if (netif_is_up(netif))
      {
        statisticUpdate();
      }
    }

    consoleRead();
  }
}

/***************************************************************************//**
 * @brief
 *   Main function, this is the entry point of the example
 ******************************************************************************/
int main(void)
{
  CHIP_Init();

  /* Enable usage, bus and memory faults for debug of hardfaults */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

  /* Configure for 48MHz HFXO operation of core clock */
  CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
  CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000))
  {
    while (1);
  }

  /* Initialize DK board register access over EBI */
  BSP_Init(BSP_INIT_DK_EBI);

  /* Enable SPI interface for ethernet control */
  BSP_PeripheralAccess(BSP_ETH, true);

  /* Initialize UART and map LF to CRLF */
  RETARGET_SerialInit();
  RETARGET_SerialCrLf(1);

  BSP_LedsInit();

  DEBUG_Print("EFM32WG DK3850 Webserver Example\n");

  /* Reset the ETH controller */
  resetEthernetController();

  /* Initialize GPIO interrupt */
  gpioIrqInit();

  /* Initialize DVK interrupt enable */
  bspIrqInit();

  /* Initialize TFT */
  TFT_Init();
  TFT_Print("\n\t EFM32WG DK3850 Webserver Example\n");
  TFT_PosGet(&infoPos);

  /* Initialise lwIP */
  lwip_init();

  /* Initialise http */
  httpd_init();

  /* Initialise echo */
  echo_init();

  /* Allocate memory for the network interface */
  en0 = (struct netif *) mem_malloc(sizeof(struct netif));
  EFM_ASSERT(en0 != NULL);

  /* Initialize the network interface*/
  netifInit(en0);

  /* Start lwIP operation */
  mainLoop(en0);

  return 0;
}
