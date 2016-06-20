/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

#ifndef _LWIP_OPTS_H_
#define _LWIP_OPTS_H_

#include "em_device.h"

/* opt.h defines fall-back values -  this file is included at the beginning of opt.h to overwrite defaults */
#define LWIP4EFM32                      1

/* CPU alignment */
#define MEM_ALIGNMENT                   4
#define ETH_PAD_SIZE                    0

/* DHCP / AUTOIP options */
#define LWIP_DHCP                       1
#define LWIP_AUTOIP                     0
#define LWIP_DHCP_AUTOIP_COOP           0

#if (!LWIP_DHCP)
/* static IP address of the host */
#define STATIC_IPADDR0                  192
#define STATIC_IPADDR1                  168
#define STATIC_IPADDR2                  1
#define STATIC_IPADDR3                  5
/* static network mask */
#define STATIC_NET_MASK0                255
#define STATIC_NET_MASK1                255
#define STATIC_NET_MASK2                255
#define STATIC_NET_MASK3                0
/* static gateway address */
#define STATIC_GW_IPADDR0               192
#define STATIC_GW_IPADDR1               168
#define STATIC_GW_IPADDR2               1
#define STATIC_GW_IPADDR3               2
#endif

/* Use simple non-threaded mode */
#define NO_SYS                          1

/* Adapting buffer sizes by the available ram */
#if SRAM_SIZE >= 0x20000
#define PBUF_POOL_SIZE                  48
#define MEM_SIZE                        (32 * 1024)
#elif SRAM_SIZE >= 0x8000
#define PBUF_POOL_SIZE                  4
#define MEM_SIZE                        (8 * 1024)
#else
#error Not enough SRAM to run the lwip stack
#endif

#define IP_REASS_MAX_PBUFS              32

 /* Payload (1500) + MAC(18) + optional 802.1Q (4) */
#define PBUF_POOL_BUFSIZE               1522
#define PBUF_LINK_HLEN                  (14 + ETH_PAD_SIZE)

#define MEMP_OVERFLOW_CHECK             2
#define MEMP_SANITY_CHECK               1
#define MEMP_NUM_SYS_TIMEOUT            6
#define MEMP_NUM_PBUF                   64

/* ARP options */
#define LWIP_ARP                        1
#if LWIP_ARP
#define ARP_TABLE_SIZE                  40
#define ETHARP_SUPPORT_VLAN             0
#endif

/* IP options */
#define ETHARP_TRUST_IP_MAC             1
#define IP_FORWARD                      0
#define IP_OPTIONS_ALLOWED              1
#define IP_REASSEMBLY                   1
#define IP_FRAG                         1
#define IP_SOF_BROADCAST                0
#define IP_SOF_BROADCAST_RECV           0

/* ICMP options */
#define LWIP_ICMP                       1
#define LWIP_BROADCAST_PING             0
#define LWIP_MULTICAST_PING             0

/* RAW options */
#define LWIP_RAW                        1

/* SNMP options */
#define LWIP_SNMP                       0

/* IGMP options */
#define LWIP_IGMP                       0

/* DNS options */
#define LWIP_DNS                        0

/* TCP options */
#define LWIP_TCP                        1

/* UDP options */
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    0

#define TCP_MSS                         1460              /* Standard for Ethernet */

/* Network Interfaces options */
#define LWIP_NETIF_HOSTNAME             0

/* LOOPIF options */
#define LWIP_HAVE_LOOPIF                0

/* Sequential layer options */
#define LWIP_NETCONN                    0

/* Socket options */
#define LWIP_SOCKET                     0

/* Statistics options */
#define LWIP_STATS                      1
#define LWIP_STATS_LARGE                1       /* 32-bit counters */
#if LWIP_STATS
#define LINK_STATS                      1
#define IP_STATS                        0
#define IPFRAG_STATS                    0
#define ICMP_STATS                      0
#define IGMP_STATS                      0
#define UDP_STATS                       0
#define TCP_STATS                       0
#define MEM_STATS                       0
#define MEMP_STAT                       0
#define SYS_STATS                       0
#define LWIP_STATS_DISPLAY              0
#endif /* STATS */

/* Checksum options */
#define CHECKSUM_GEN_IP                 0
#define CHECKSUM_GEN_UDP                0
#define CHECKSUM_GEN_TCP                0
#define CHECKSUM_CHECK_IP               0
#define CHECKSUM_CHECK_UDP              0
#define CHECKSUM_CHECK_TCP              0
#define LWIP_CHECKSUM_ON_COPY           0

/* Http options */
#define HTTPD_USE_CUSTOM_FSDATA         1

#if defined(__CROSSWORKS_ARM)
/* Rowley Crossworks provides a strnstr function */
#define LWIP_HTTPD_STRNSTR_PRIVATE      0
#endif

/* Netif options */
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_STATUS_CALLBACK      1

/* Debug options */
#define LWIP_DEBUG
/*
 * Only messages that have a level that is greater than or equal to
 * LWIP_DBG_MIN_LEVEL are printed to the console.
 *
 * These are the possible debug levels:
 *   0 - LWIP_DBG_LEVEL_ALL     (Print all messages)
 *   1 - LWIP_DBG_LEVEL_WARNING (Print warning or more severe messages)
 *   2 - LWIP_DBG_LEVEL_SERIOUS (Print serious or more severe messages)
 *   3 - LWIP_DBG_LEVEL_SEVERE  (Only print the most severe messages)
 */
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_WARNING


#define LWIP_GLOB_DEBUG                 LWIP_DBG_ON
/**
 * LWIP_DBG_TYPES_ON can be used to globally enable debug messages
 * of a certain type. There are 3 types that are defined:
 *   LWIP_DBG_TRACE - to follow program flow
 *   LWIP_DBG_STATE - to follow module states
 *   LWIP_DBG_FRESH - newly added code (not thoroughly tested yet)
 * When we set this to LWIP_DBG_ON it leads to enabling all debug messages
 * even the once that have no type set.
 */
#define LWIP_DBG_TYPES_ON               LWIP_DBG_ON /* LWIP_DBG_TRACE | LWIP_DBG_STATE | LWIP_DBG_FRESH */

/* modify for a module using other options (LWIP_DBG_ON || LWIP_DBG_OFF) ||( |DBG_1|DBG_2|DBG_3) */
#define ETHARP_DEBUG        LWIP_GLOB_DEBUG
#define NETIF_DEBUG         LWIP_GLOB_DEBUG
#define PBUF_DEBUG          LWIP_GLOB_DEBUG
#define API_LIB_DEBUG       LWIP_GLOB_DEBUG
#define API_MSG_DEBUG       LWIP_GLOB_DEBUG
#define SOCKETS_DEBUG       LWIP_GLOB_DEBUG
#define ICMP_DEBUG          LWIP_GLOB_DEBUG
#define IGMP_DEBUG          LWIP_GLOB_DEBUG
#define INET_DEBUG          LWIP_GLOB_DEBUG
#define IP_DEBUG            LWIP_GLOB_DEBUG
#define IP_REASS_DEBUG      LWIP_GLOB_DEBUG
#define RAW_DEBUG           LWIP_GLOB_DEBUG
#define MEM_DEBUG           LWIP_GLOB_DEBUG
#define MEMP_DEBUG          LWIP_GLOB_DEBUG
#define SYS_DEBUG           LWIP_GLOB_DEBUG
#define TIMERS_DEBUG        LWIP_GLOB_DEBUG
#define TCP_DEBUG           LWIP_GLOB_DEBUG
#define TCP_INPUT_DEBUG     LWIP_GLOB_DEBUG
#define TCP_FR_DEBUG        LWIP_GLOB_DEBUG
#define TCP_RTO_DEBUG       LWIP_GLOB_DEBUG
#define TCP_CWND_DEBUG      LWIP_GLOB_DEBUG
#define TCP_WND_DEBUG       LWIP_GLOB_DEBUG
#define TCP_OUTPUT_DEBUG    LWIP_GLOB_DEBUG
#define TCP_RST_DEBUG       LWIP_GLOB_DEBUG
#define TCP_QLEN_DEBUG      LWIP_GLOB_DEBUG
#define UDP_DEBUG           LWIP_GLOB_DEBUG
#define TCPIP_DEBUG         LWIP_GLOB_DEBUG
#define HTTPD_DEBUG         LWIP_GLOB_DEBUG
#define PPP_DEBUG           LWIP_GLOB_DEBUG
#define SLIP_DEBUG          LWIP_GLOB_DEBUG
#define DHCP_DEBUG          LWIP_GLOB_DEBUG
#define AUTOIP_DEBUG        LWIP_GLOB_DEBUG
#define SNMP_MSG_DEBUG      LWIP_GLOB_DEBUG
#define SNMP_MIB_DEBUG      LWIP_GLOB_DEBUG
#define DNS_DEBUG           LWIP_GLOB_DEBUG
#define MAIN_DEBUG          LWIP_GLOB_DEBUG
#define NETIO_DEBUG         LWIP_GLOB_DEBUG

/* to avoid warnings of unused variables */
#if defined (__ICCARM__)
#ifdef LWIP_DEBUG
#define LWIP_DBGVAR(debug, var, val)    do {                             \
    if (                                                                 \
      ((debug) & LWIP_DBG_ON) &&                                         \
      ((debug) & LWIP_DBG_TYPES_ON) &&                                   \
      ((s16_t) ((debug) & LWIP_DBG_MASK_LEVEL) >= LWIP_DBG_MIN_LEVEL)) { \
      var = val;                                                         \
    } else {                                                             \
      _Pragma("diag_suppress=Pe549")                                     \
      (void) var; }                                                      \
} while (0)
#else
#define LWIP_DBGVAR(debug, var, val) \
  _Pragma("diag_suppress=Pe549")     \
  (void) var
#endif /* LWIP_DEBUG */
#else
#ifdef LWIP_DEBUG
#define LWIP_DBGVAR(debug, var, val)    do {                             \
    if (                                                                 \
      ((debug) & LWIP_DBG_ON) &&                                         \
      ((debug) & LWIP_DBG_TYPES_ON) &&                                   \
      ((s16_t) ((debug) & LWIP_DBG_MASK_LEVEL) >= LWIP_DBG_MIN_LEVEL)) { \
      var = val;                                                         \
    } else {                                                             \
      (void) var; }                                                      \
} while (0)
#else
#define LWIP_DBGVAR(debug, var, val)    (void) var
#endif /* LWIP_DEBUG */
#endif

#endif /* _LWIP_OPTS_H_ */

