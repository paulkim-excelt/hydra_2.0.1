/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
  @addtogroup grp_inetlib_il
  @{
  @file inet_system.h
  @brief System integration interface of Internet Stack subsystem

  This file provides the system integration APIs

  @version 0.86 Imported from MS Word document
*/

#ifndef INET_SYSTEM_H
#define INET_SYSTEM_H

#include "inet.h"
#include <circ_queue.h>
#include <nif.h>

/**
    @name Port number for auto allocation

    @brief Minimum value that shall be used for auto allocation in case
    the INET client does not specify a value for the local port.
 */
#define INET_EPHERMERAL_PORT_START       (40000UL)

/**
    @name Maximum number of auto allocated local ports

    @brief Maximum number of port values assigned by the INET when the
    client does not specify a value for the local port.
 */
#define INET_MAX_NUM_EPHERMERAL_PORTS    (1000UL)

/**
    @name Bit mask for extracting the index of the end point

    @brief Bit mask for extracting the index of the end point from the
    local port number. This value is based on the number of bits used to
    represent the value of #INET_MAX_NUM_EPHERMERAL_PORTS.
 */
#define INET_AUTO_PORT_BIT_MASK          (0xFFFUL)

/**
  @name INET_EndPointId
  @{
  @brief End point identifier

  @trace #BRCM_ARCH_INETCOMP_ENDPOINTID_TYPE   #BRCM_REQ_INETCOMP_ENDPOINTID
*/
typedef uint16_t INET_EndPointId;         /**< @brief typedef for endpoint
                                               identifier */
#define INET_ENDPOINTID_INVALID (0xFFFFU) /**< @brief Invalid value of end
                                                point identifier */
/** @} */

/**
  @name INET_L4Protocol
  @{
  @brief OSI Layer 4 (Transport) protocol as defined by IANA (RFC790)

  @trace #BRCM_ARCH_INETCOMP_L4PROTOCOL_TYPE   #BRCM_REQ_INETCOMP_L4PROTOCOL
*/
typedef uint16_t INET_L4Protocol;   /**< @brief typedef for L4 protocol */
#define INET_L4PROTO_ICMP    (1U)   /**< @brief Internet Control Message
                                         Protocol */
#define INET_L4PROTO_TCP     (6U)   /**< @brief Transmission Control Protocol */
#define INET_L4PROTO_UDP     (17U)  /**< @brief User Datagram Protocol */
#define INET_L4PROTO_UDPLITE (136U) /**< @brief Lightweight User Datagram
                                          Protocol */
/** @} */

/**
  @name INET_ARPFlag
  @{
  @brief Flag denoting ARP entry properties

  @trace #BRCM_ARCH_INETCOMP_ARPFLAG_TYPE   #BRCM_REQ_INETCOMP_ARPFLAG
*/
typedef uint32_t INET_ARPFlag;             /**< @brief typedef for ARP flag */
#define INET_ARP_ENTRY_FLAG_WAIT     (1UL) /**< @brief ARP request has been sent
                                                 and a response is awaited */
#define INET_ARP_ENTRY_FLAG_RESOLVED (2UL) /**< @brief Valid response was
                                                 received for an ARP request */
#define INET_ARP_ENTRY_FLAG_LEARNT   (3UL) /**< @brief ARP entry learnt from a
                                                 gratuitous ARP or an incoming
                                                 request */
#define INET_ARP_ENTRY_FLAG_STATIC   (4UL) /**< @brief Statically configured ARP
                                                 entry */
/** @} */

/**
  @name INET_InterfaceId
  @{
  @brief Interface identifier

  @trace #BRCM_ARCH_INETCOMP_INTERFACEID_TYPE   #BRCM_REQ_INETCOMP_INTERFACEID
*/
typedef uint32_t INET_InterfaceId;              /**< @brief typedef for
                                                      interface identifier */
#define INET_INTERFACEID_INVALID (0xFFFFFFFFUL) /**< @brief Invalid interface */
/** @} */

#if 0
/**
  @name INET_NIFCtrlIdx
  @{
  @brief Network Interface Controller Index

  @trace #BRCM_ARCH_INETCOMP_NIFCTRLIDX_TYPE   #BRCM_REQ_INETCOMP_NIFCTRLIDX
*/
typedef NIF_CntrlIDType INET_NIFCtrlIdx;                /**< @brief typedef for
                                                              NIF controller
                                                              index */
#define INET_NIFCTRLIDX_INVALID (0xFFFFFFFFFFFFFFFFULL) /**< @brief Invalid NIF
                                                              controller index*/
/** @} */
#endif

/**
  @name INET_DSCP
  @{
  @brief IPv4 6 bit Differentiated Services (DS) field

  @trace #BRCM_ARCH_INETCOMP_DSCP_TYPE   #BRCM_REQ_INETCOMP_DSCP
*/
typedef uint8_t INET_DSCP;      /**< @brief typedef for IPv4 DSCP */
#define INET_DSCP_CS0   (0x00U) /**< @brief Class 0 Selector */
#define INET_DSCP_CS1   (0x08U) /**< @brief Class 1 Selector */
#define INET_DSCP_CS2   (0x10U) /**< @brief Class 2 Selector */
#define INET_DSCP_CS3   (0x18U) /**< @brief Class 3 Selector */
#define INET_DSCP_CS4   (0x20U) /**< @brief Class 4 Selector */
#define INET_DSCP_CS5   (0x28U) /**< @brief Class 5 Selector */
#define INET_DSCP_CS6   (0x30U) /**< @brief Class 6 Selector */
#define INET_DSCP_CS7   (0x38U) /**< @brief Class 7 Selector */
#define INET_DSCP_AF11  (0x0AU) /**< @brief Assured Forwarding Class 1, Low
                                      Drop Probability */
#define INET_DSCP_AF12  (0x0CU) /**< @brief Assured Forwarding Class 1, Medium
                                      Drop Probability */
#define INET_DSCP_AF13  (0x0EU) /**< @brief Assured Forwarding Class 1, High
                                      Drop Probability */
#define INET_DSCP_AF21  (0x12U) /**< @brief Assured Forwarding Class 2, Low
                                      Drop Probability */
#define INET_DSCP_AF22  (0x14U) /**< @brief Assured Forwarding Class 2, Medium
                                      Drop Probability */
#define INET_DSCP_AF23  (0x16U) /**< @brief Assured Forwarding Class 2, High
                                      Drop Probability */
#define INET_DSCP_AF31  (0x1AU) /**< @brief Assured Forwarding Class 3, Low
                                      Drop Probability */
#define INET_DSCP_AF32  (0x1CU) /**< @brief Assured Forwarding Class 3, Medium
                                      Drop Probability */
#define INET_DSCP_AF33  (0x1EU) /**< @brief Assured Forwarding Class 3, High
                                      Drop Probability */
#define INET_DSCP_AF41  (0x22U) /**< @brief Assured Forwarding Class 4, Low
                                      Drop Probability */
#define INET_DSCP_AF42  (0x24U) /**< @brief Assured Forwarding Class 4, Medium
                                      Drop Probability */
#define INET_DSCP_AF43  (0x26U) /**< @brief Assured Forwarding Class 4, High
                                      Drop Probability */
#define INET_DSCP_EF    (0x2EU) /**< @brief Expedited Forwarding */
#define INET_DSCP_VA    (0x2CU) /**< @brief Voice Admit */
/** @} */

/**
  @name INET_Netmask
  @{
  @brief Netmask expressed in CIDR notation

  @trace #BRCM_ARCH_INETCOMP_NETMASK_TYPE   #BRCM_REQ_INETCOMP_NETMASK
*/
typedef uint8_t INET_Netmask; /**< @brief typedef for netmask */
/** @} */

/**
  @name INET_IPAddressAssignmentType
  @{
  @brief IP address assignment policy

  @trace #BRCM_ARCH_INETCOMP_IPADDRPOL_TYPE   #BRCM_REQ_INETCOMP_IPADDRPOL
*/
typedef uint32_t INET_IPAddressAssignmentType;   /**< @brief typedef for IP
                                                      address assignment policy
                                                      */
#define INET_IPADDRESSASSIGNMENT_STATIC    (1UL) /**< @brief Statically
                                                      configured IP address */
#define INET_IPADDRESSASSIGNMENT_DYNAMIC   (2UL) /**< @brief IP address
                                                      configured using DHCP */
#define INET_IPADDRESSASSIGNMENT_LINKLOCAL (3UL) /**< @brief Link-local
                                                      assignment of IP address*/
/** @} */

/**
  @name INET_ReassemblyBufferState
  @{
  @brief Reassembly buffer state machine values

  @trace #BRCM_ARCH_INETCOMP_REASSBUFSTATE_TYPE #BRCM_REQ_INETCOMP_REASSBUFSTATE
*/
typedef uint32_t INET_ReassemblyBufferState; /**< @brief typedef for reassembly
                                                  buffer state */
#define INET_REASSEMBLYBUF_FREE      (1UL) /**< @brief Reassembly buffer is
                                                 unused/free */

#define INET_REASSEMBLYBUF_ALLOCATED (2UL) /**< @brief Reassembly buffer is
                                                allocated */

#define INET_REASSEMBLYBUF_QUEUED    (3UL) /**< @brief Reassembly buffer is
                                                queued to a client */

#define INET_REASSEMBLYBUF_RCVD      (4UL) /**< @brief Reassembly buffer is
                                                being read by the client */
/** @} */


/** @def INET_CONVERT_IP_ADDRESS
    @brief Converts an IP address from the a.b.c.d notation to INET_IPAddress
    @trace #BRCM_ARCH_INETCOMP_CONVERTIPADDR_MACRO #BRCM_REQ_INETCOMP_CONVERTIPADDR
*/
#define INET_CONVERT_IP_ADDRESS(A, B, C, D) (A & 0xFFU) << 24U|(B & 0xFFU) << 16U\
                                            |(C & 0xFFU) << 8U | (D & 0xFFU)

/**
    @brief ARP table entry

    @trace #BRCM_ARCH_INETCOMP_ARPENTRY_TYPE  #BRCM_REQ_INETCOMP_ARPENTRY
*/
typedef struct _INET_ARPEntry {
    uint32_t         valid;       /**< @brief Indicates whether the entry is
                                       populated or not */
    INET_IPAddress   ipAddr;      /**< @brief IP Address - used as the key for
                                       the lookup */
    uint8_t          macAddr[6U]; /**< @brief MAC Address - used as the result
                                       for the lookup */
    INET_ARPFlag     flags;       /**< @brief Flags to indicate the type of
                                       entry */
    uint32_t         timer;       /**< @brief Time in seconds for the entry to
                                       timeout */
    uint32_t         exclVal;     /**< @brief Exclusive - For critical section
                                       protection using Ldrex/Strex */
    INET_InterfaceId ifID;        /**< @brief Interface on which the ARP request
                                       was initiated. This is used to refresh the
                                       entry on timeout. */
} INET_ARPEntry;

/** @struct INET_InterfaceVlanTagConfig
    @brief VLAN tag configuration for a L2 interface

    @trace #BRCM_ARCH_INETCOMP_INTFVLANTAGCONFIG_TYPE  #BRCM_REQ_INETCOMP_INTFVLANTAGCONFIG
*/
typedef struct _INET_InterfaceVlanTagConfig {
    uint16_t   pcp; /**< @brief Priority code point value. Valid values are
                          0-7 */
    uint16_t  vlan; /**< @brief VLAN number. Valid values are 1-4094.
                          0 indicates untagged interface */
} INET_InterfaceVlanTagConfig;

/** @struct INET_InterfaceStats
    @brief Structure representing statistics for an interface

    @trace #BRCM_ARCH_INETCOMP_INTFSTATS_TYPE   #BRCM_REQ_INETCOMP_INTFSTATS
*/
typedef struct _INET_InterfaceStats {
    uint32_t txPkts;       /**< @brief Number of packets transmitted */
    uint32_t txBytes;      /**< @brief Number of bytes transmitted */
    uint32_t txARP;        /**< @brief Number of ARP messages transmitted */
    uint32_t txICMP;       /**< @brief Number of ICMP messages transmitted */
    uint32_t txDropPkts;   /**< @brief Number of transmit packets dropped */
    uint32_t txARPFailure; /**< @brief Number of ARP resolution failures */
    uint32_t txCancelPkts; /**< @brief Number of transmit packets cancelled */
    uint32_t txL2PktCount; /**< @brief Number of L2 packets transmitted */
    uint32_t txL2BufNA;    /**< @brief Number of L2 buffer unavailable */
    uint64_t rxPkts;       /**< @brief Number of packets received */
    uint64_t rxBytes;      /**< @brief Number of bytes received */
    uint64_t rxL2PktCount; /**< @brief Number of all incoming packets received*/
    uint32_t rxDropPkts;   /**< @brief Number of received packets dropped */
    uint32_t rxICMP;       /**< @brief Number of ICMP messages received */
    uint32_t rxICMPErr;    /**< @brief Number of ICMP messages not supported */
    uint32_t rxARP;        /**< @brief Number of ARP messages received */
} INET_InterfaceStats;

/** @struct INET_InterfaceConfig
    @brief Configuration for an interface

    @trace #BRCM_ARCH_INETCOMP_INTFCONFIG_TYPE   #BRCM_REQ_INETCOMP_INTFCONFIG
*/
typedef struct _INET_InterfaceConfig {
    INET_InterfaceId             hwIndex;       /**< @brief Identifier for the
                                                     L2 controller */
    INET_InterfaceVlanTagConfig  vlanConfig;    /**< @brief VLAN configuration
                                                     for the interface */
    INET_IPAddressAssignmentType addressType;   /**< @brief Type of IP address
                                                     assigned to the
                                                     interface */
    INET_IPAddress               staticAddress; /**< @brief IP address assigned
                                                     to the interface (valid
                                                     only for static
                                                     assignment) */
    INET_Netmask                 netmask;       /**< @brief Netmask (valid only
                                                     for static assigment) */
    INET_IPAddress               defaultRoute;  /**< @brief Default
                                                     route/gateway (valid only
                                                     for static assignment) */
} INET_InterfaceConfig;

/** @struct INET_InterfaceObject
    @brief Interface Object Structure

    @trace #BRCM_ARCH_INETCOMP_INTFOBJ_TYPE   #BRCM_REQ_INETCOMP_INTFOBJ
*/
typedef struct _INET_InterfaceObject {
    INET_InterfaceId           ifID;       /**< @brief Unique interface
                                                identifier */
    const INET_InterfaceConfig *ifCfg;     /**< @brief Pointer to the
                                                configuration data for this
                                                interface */
    INET_InterfaceStats        *ifStats;   /**< @brief Pointer to the statistics
                                                memory for this interface */
    NIF_CntrlIDType            nifCtrlIdx; /**< @brief Network Interface
                                                Controller Index */
} INET_InterfaceObject;

/** @struct INET_SystemInterface
    @brief Structure representing an interface

    @trace #BRCM_ARCH_INETCOMP_SYSINTF_TYPE   #BRCM_REQ_INETCOMP_SYSINTF
*/
typedef struct _INET_SystemInterface {
    INET_InterfaceObject ifObj;     /**< @brief Interface details with
                                          configuration */
    INET_ARPEntry        *arpCache; /**< @brief Pointer to the ARP cache for
                                          the interface */
} INET_SystemInterface;

/** @struct INET_EndPointState
    @brief Context/Scratch-memory for an endpoint

    @trace #BRCM_ARCH_INETCOMP_ENDPTSTATE_TYPE   #BRCM_REQ_INETCOMP_ENDPTSTATE
*/
typedef struct _INET_EndPointState {
    uint16_t           ident;         /**< @brief Identification field for IP
                                            header */
    uint16_t           sentBytes;     /**< @brief Bytes sent for an ongoing
                                            transmission */
    const uint8_t*     txBuf;         /**< @brief Client buffer containing the
                                            payload for transmission */
    uint16_t           txLength;      /**< @brief Length of the client buffer
                                            containing payload for
                                            transmission */
    const CIRCQ_Type* const rxPktQ;   /**< @brief List of received packets
                                            waiting for client to read */
    uint64_t           txPkts;        /**< @brief Number of packets
                                            transmitted */
    uint64_t           txBytes;       /**< @brief Number of bytes transmitted */
    uint64_t           rxGoodPkts;    /**< @brief Number of packets received */
    uint64_t           rxTruncPkts;   /**< @brief Number of packets truncated
                                           due to insufficient reassembly buffer
                                           size*/
    uint64_t           rxDropPkts;    /**< @brief Number of packets dropped due
                                            to no waiting client task */
    uint64_t           rxBytes;       /**< @brief Number of bytes received */
    uint32_t           arpCacheIndex; /**< @brief Index of ARP cache where the
                                            remote entry was last resolved */
} INET_EndPointState;

/** @struct INET_EndPointObject
    @brief Endpoint object details

    @trace #BRCM_ARCH_INETCOMP_ENDPTOBJ_TYPE     #BRCM_REQ_INETCOMP_ENDPTOBJ
*/
typedef struct _INET_EndPointObject {
    INET_L4Protocol            l4Proto;    /**< @brief OSI L4 protocol */
    INET_DSCP                  dscp;       /**< @brief Value of the differentiated services
                                                (DS) field in the IP header */
    INET_EndPointIPPort*       cfg;        /**< @brief IP/Port Configuration for the
                                                end point */
    INET_EndPointState*        ctxt;       /**< @brief Context/Scratch memory
                                                for the end point */
} INET_EndPointObject;

/** @struct INET_SystemEndPoint
    @brief Structure representing an end point

    @trace #BRCM_ARCH_INETCOMP_SYSENDPT_TYPE     #BRCM_REQ_INETCOMP_SYSENDPT
*/
typedef struct _INET_SystemEndPoint {
    uint64_t            compId;        /**< @brief Component identifier used to
                                            identify the client */
    uint32_t            payloadId;     /**< @brief Instance identifier used to
                                            identify the client */
    INET_InterfaceId    intfId;        /**< @brief Interface identifier */
    INET_EndPointObject endPtObj;      /**< @brief End point details */
    int32_t             txTaskId;      /**< @brief Task to notify on packet
                                            transmission confirmation */
    uint32_t            txTaskEventId; /**< @brief Event of the Task to notify
                                            on free buffer being available or
                                            when the ARP gets resolved */
    int32_t             rxTaskId;      /**< @brief Task to notify on packet
                                            reception */
    uint32_t            rxTaskEventId; /**< @brief Event of the Task to notify
                                            on packet reception */
} INET_SystemEndPoint;

/** @struct INET_ReassemblyBuffer
    @brief State structure for reassembling a buffer

    @trace #BRCM_ARCH_INETCOMP_REASSBUF_TYPE     #BRCM_REQ_INETCOMP_REASSBUF
*/
typedef struct _INET_ReassemblyBuffer {
    INET_IPAddress             ipAddr;        /**< @brief Source IP address of
                                                   incoming datagram */
    uint16_t                   ident;         /**< @brief Identification field
                                                   of incoming datagram */
    INET_ReassemblyBufferState bufState;      /**< @brief State of the
                                                   reassembly buffer */
    const CIRCQ_Type* const    fragmentQ;     /**< @brief Circular queue to hold

                                                   the fragment offsets.Used to
                                                   detect duplicates */
    uint8_t* const             buffer;        /**< @brief Buffer to reassemble
                                                   the fragments into */
    uint16_t                   curLen;        /**< @brief Total length of
                                                   fragments currently
                                                   reassembled */
    uint16_t                   totalLen;      /**< @brief Total length of the IP
                                                   datagram */
    uint32_t                   timer;         /**< @brief Reassembly and client
                                                   read timeout */
    uint16_t                   clientEndPtID; /**< @brief End point ID for this
                                                   reassembly buffer */
} INET_ReassemblyBuffer;

/**
    @brief Cache of received packets till client reads them

    @trace #BRCM_ARCH_INETCOMP_RXPKT_TYPE     #BRCM_REQ_INETCOMP_RXPKT
*/
typedef struct _INET_RxPkt {
    const uint8_t* buffer;    /**< @brief Reassembly buffer containing received
                                   payload */
    uint16_t       length;    /**< @brief Length of received payload in
                                   reassembly buffer */
    INET_IPAddress srcIPAddr; /**< @brief Remote host from which the payload
                                   was received */
    INET_Port      srcPort;   /**< @brief Remote port from which the payload
                                   was received */
} INET_RxPkt;

/**
 * @brief  Get L2 interface for an end point
 *
 * @pre None
 *
 *  Used to retrieve the L2 interface object for an end point
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]   aEndPointId           End point identifier
 * @param[out]  aIntfObj              L2 Interface Object
 *
 * @retval   #BCM_ERR_OK              L2 interface successfully
 *                                    retrieved
 * @retval   #BCM_ERR_INVAL_PARAMS    aIntfId is NULL or end point not found
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_GETINTFFORENDPT_PROC #BRCM_REQ_INETCOMP_GETINTFFORENDPT
 *
 * @limitations None
 */
int32_t INET_GetInterfaceForEndPoint(const INET_EndPointId aEndPointId,
                                     INET_InterfaceObject** const aIntfObj);

/**
 * @brief  Get L2 interface
 *
 * @pre None
 *
 *  Used to retrieve the L2 interface object from the identifier
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]   aIntfId               L2 interface identifier
 * @param[out]  aIntfObj              L2 Interface Object
 *
 * @retval   #BCM_ERR_OK              L2 interface successfully
 *                                    retrieved
 * @retval   #BCM_ERR_INVAL_PARAMS    Interface not found
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_GETINTF_PROC #BRCM_REQ_INETCOMP_GETINTF
 *
 * @limitations None
 */
int32_t INET_GetInterface(const INET_InterfaceId aIntfId,
                          INET_InterfaceObject** const aIntfObj);

/**
 * @brief  Get L2 interface count
 *
 * @pre None
 *
 *  Used to retrieve the count of L2 interfaces created
 *
 * @behavior Sync, Re-entrant
 *
 * @param[out]  aIntfCount            L2 interface count
 *
 * @retval      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_GETINTFCNT_PROC #BRCM_REQ_INETCOMP_GETINTFCNT
 *
 * @limitations None
 */
void INET_GetInterfaceCount(uint32_t* const aIntfCount);

/**
 * @brief  Get end point information
 *
 * @pre None
 *
 *  Used to retrieve the end point for a client along with its configuration and
 *  context
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]  aCompId                Component identifier used to identify the
 *                                    client of the end point
 * @param[in]  aPayloadId             Payload identifier used to identify the
 *                                    client of the end point
 * @param[out] aEndPtIndex            Index of the end point in the array
 * @param[out] aEndPointObj           Configuration for the end point and
 *                                    context/scratch memory for the end point
 *
 * @retval  #BCM_ERR_OK               End point successfully retrieved
 * @retval  #BCM_ERR_INVAL_PARAMS     aEndPointId is NULL
 * @retval  #BCM_ERR_NOT_FOUND        End point not found
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_GETENDPT_PROC #BRCM_REQ_INETCOMP_GETENDPT
 *
 * @limitations None
 */
int32_t INET_GetEndPoint(const uint64_t aCompId,
                         const uint32_t aPayloadId,
                         INET_EndPointId* const aEndPtIndex,
                         const INET_EndPointObject** const aEndPointObj);

/**
 * @brief Finds the H/W address for an IP address
 *
 * @pre None
 *
 *  Retrieves the H/W address from the cache (if it exists) for unicast entry.
 *  On ARP cache lookup failure, sends an ARP request. For broadcast and
 *  multicast IP address returns the mapped H/W address.
 *
 * @behavior Sync, Non Re-entrant
 *
 * @param[in]  aIntfObj               Interface configuration and stats
 * @param[in]  aIPAddr                IP address that needs to be resolved
 * @param[out] aMACAddr               MAC address resolved by ARP
 * @param[inout] arpIndex             Index of ARP cache where it was last found
 *
 * @retval  #BCM_ERR_OK               HW address resolved successfully
 * @retval  #BCM_ERR_INVAL_PARAMS     aIPAddr is invalid
 * @retval  #BCM_ERR_NOT_FOUND        HW address not resolved
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_ARPRSLVADDR_PROC #BRCM_REQ_INETCOMP_ARPRSLVADDR
 *
 * @limitations None
 */
int32_t INET_ARPResolveAddress(INET_InterfaceObject* aIntfObj,
                               const INET_IPAddress aIPAddr,
                               uint8_t* const aMACAddr,
                               uint32_t* const arpIndex);

/**
 * @brief Sends an ARP request
 *
 * @pre None
 *
 *  Constructs an ARP request message and sends it on the network interface
 *
 * @param[in]  aIntfId                Interface identifier
 * @param[in]  aIPAddr                IP Address to be resolved
 * @param[in]  aFlags                 Current flags of the ARP entry
 *
 * @retval     void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_ARPSENDREQ_PROC #BRCM_REQ_INETCOMP_ARPSENDREQ
 *
 * @limitations None
 */
void INET_ARPSendRequest(const INET_InterfaceId aIntfId,
                         const INET_IPAddress aIPAddr,
                         const uint32_t aFlags);

/**
 * @brief Adds/updates an entry in the ARP cache
 *
 * @pre None
 *
 *  Adds/updates an entry in the ARP cache for the provided interface
 *
 * @param[in]  aIntfId                Interface identifier
 * @param[in]  aIPAddr                IP Address
 * @param[in]  aMACAddr               MAC Address
 * @param[in]  aFlags                 Flags
 * @param[in]  aUpdateAddr            Flag to indicate if MAC address
 *                                    needs to be updated
 *
 * @retval     BCM_ERR_OK             Entry added/update successfully
 * @retval     BCM_ERR_NOMEM          No free space in ARP cache
 * @retval     BCM_ERR_INVAL_PARAMS   aIntfId is invalid or aMACAddr is NULL
 * @retval     BCM_ERR_NOPERM         Entry conflicts with a static entry
 *                                    already present in the cache
 */
int32_t INET_AddARPEntry(const INET_InterfaceId aIntfId,
                         const INET_IPAddress  aIPAddress,
                         const uint8_t* const aMACAddr,
                         const uint32_t aFlags,
                         const uint8_t aUpdateAddr);

/**
 * @brief Finds an entry in the ARP cache
 *
 * @pre None
 *
 *  Finds an entry in the ARP cache for the provided interface
 *
 * @param[in]  aIntfId                Interface identifier
 * @param[in]  aIPAddr                IP Address
 * @param[out] aMACAddr               MAC Address to be retrieved
 * @param[out] aFlags                 Flags for the entry retrieved
 * @param[inout]arpIndex              ARP cache index
 *
 * @retval     BCM_ERR_OK             Entry found
 * @retval     BCM_ERR_INVAL_PARAMS   aIntfId is invalid or aMACAddr/aFlags is
 *                                    NULL
 * @retval     BCM_ERR_NOT_FOUND      Entry does not exist in cache
 */
int32_t INET_FindARPEntry(const INET_InterfaceId aIntfId,
                          const INET_IPAddress  aIPAddress,
                          uint8_t* const aMACAddr,
                          uint32_t* const aFlags,
                          uint32_t* const arpIndex);

/**
 * @brief Removes an entry from the ARP cache
 *
 * @pre None
 *
 *  Removes an entry in the ARP cache for the provided interface
 *
 * @param[in]  aIntfId                Interface identifier
 * @param[in]  aIPAddr                IP Address - key for the entry
 *
 * @retval     BCM_ERR_OK             Entry removed successfully
 * @retval     BCM_ERR_INVAL_PARAMS   aIntfId is invalid
 * @retval     BCM_ERR_NOT_FOUND      Entry does not exist in cache
 * @retval     BCM_ERR_NOSUPPORT      Entry is static
 */
int32_t INET_RemoveARPEntry(const INET_InterfaceId aIntfId,
                            const INET_IPAddress  aIPAddress);

/**
 * @brief Searches for an interface using IP Address as key
 *
 * @pre None
 *
 *  Used to search for the L2 interface identifier from the system configuration
 *  The key is <aHwId , aIPAddr>
 *
 * @param[in]  aHwId                  HW index of the interface
 * @param[in]  aIPAddress             IP Address
 * @param[in]  aVlan                  Vlan tag value
 * @param[out] aIntfId                L2 interface identifier
 *
 * @retval     BCM_ERR_OK             L2 interface identifier retrieved
 * @retval     BCM_ERR_INVAL_PARAMS   aIntfId is NULL
 * @retval     BCM_ERR_NOT_FOUND      No such valid interface exists
 */
int32_t INET_SearchInterface(const uint32_t aHwId,
                             const INET_IPAddress aIPAddress,
                             const uint16_t aVlan,
                             INET_InterfaceId* const aIntfId);

/**
 * @brief Get a reassembly buffer
 *
 * @pre None
 *
 *  Used by the stack to allocate/fetch a reassembly buffer for received IP
 *  datagrams. The key for the search is <aSrcAddr, aIdent> which can be used to
 *  fetch a previously allocated buffer
 *
 * @param[in]  aSrcAddr               Source address in IP datagram being
 *                                    reassembled
 * @param[in]  aIdent                 Identification field from the IP datagram
 * @param[out] aBuf                   Reassembly buffer
 * @param[out] aLen                   Length of the reassembly buffer
 * @param[out] aCurLen                Length of data in reassembly buffer
 * @param[out] aTotalLen              Total length of reassembled datagram
 * @param[out] aFragQ                 Pointer to the fragment queue
 *
 * @retval     BCM_ERR_OK             Reassembly buffer allocated successfully
 * @retval     BCM_ERR_INVAL_PARAMS   aBuf or aCurLen is NULL
 * @retval     BCM_ERR_NOMEM          Out of reassembly buffers
 */
int32_t INET_GetReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                 const uint16_t aIdent,
                                 uint8_t** aBuf,
                                 uint16_t* aLen,
                                 uint16_t* const aCurLen,
                                 uint16_t* const aTotalLen,
                                 const CIRCQ_Type** aFragQ);

/**
 * @brief Update a reassembly buffer
 *
 * @pre None
 *
 *  Used by the stack to update a reassembly buffer for received IP
 *  datagrams. The key for the search is <aSrcAddr, aIdent>
 *
 * @param[in] aSrcAddr               Source address in IP datagram being
 *                                   reassembled
 * @param[in] aIdent                 Identification field from the IP datagram
 * @param[in] aCurLen                Length of data in reassembly buffer. Value
 *                                   0U frees the buffer
 * @param[in] aTotalLen              Total length of reassembled datagram
 *
 * @retval    BCM_ERR_OK             Reassembly buffer updated
 * @retval    BCM_ERR_NOT_FOUND      Unable to find reassembly buffer for
 *                                   provided parameters
 */
int32_t INET_UpdateReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                    const uint16_t aIdent,
                                    const uint16_t aCurLen,
                                    const uint16_t aTotalLen);

/**
 * @brief Free reassembly buffer
 *
 * @pre None
 *
 *  Used by the stack to free a reassembly buffer. The key for the search is
 *  <aBuf>
 *
 * @param[in] aBuf                   Reassembly buffer
 *
 * @retval    BCM_ERR_OK             Reassembly buffer freed
 * @retval    BCM_ERR_NOT_FOUND      Buffer could not be found or was
 *                                   unallocated
 */
int32_t INET_FreeReassemblyBuffer(const uint8_t* const aBuf);

/**
 * @brief Check end point ID and update reassembly buffer state
 *
 * @pre None
 *
 *  Atomically sets the state of the reassembly buffer to INET_REASSEMBLYBUF_RCVD
 *  after validation of the aEndPtID
 *
 * @param[in] aBuf                   Reassembly data buffer pointer
 * @param[in] aEndPtID               End point ID of the client
 *
 * @retval    BCM_ERR_OK             Reassembly buffer State successfully set
 * @retval    BCM_ERR_INVAL_STATE    Reassembly buffer State set failure as
 *                                   buffer state was not INET_REASSEMBLYBUF_QUEUED
 * @retval    BCM_ERR_NOT_FOUND      The reassembly buffer pointer is invalid
 *                                   or the buffer has timed out and
 *                                   not intended for current client
 */
int32_t INET_ClientAckReassBuf(const uint8_t* const aBuf,
                               INET_EndPointId const aEndPtID);

/**
 * @brief Inform client of received UDP data
 *
 * @pre None
 *
 *  Invoked by the stack to indicate that UDP payload is available. System can
 *  identify the exact client from the provided information and notify it.
 *
 * @param[in] aSrcAddr               Source IP Address in a received IP datagram
 * @param[in] aSrcPort               Source L4 port
 * @param[in] aDestPort              Destination IP Address in a received IP
 *                                   datagram
 * @param[in] aDestPort              Destination L4 port
 * @param[in] aL4Protocol            L4 protocol of received data
 * @param[in] aBuf                   Buffer containing payload
 * @param[in] aLen                   Length of payload in buffer
 *
 * @retval    BCM_ERR_OK             Client notified successfully
 * @retval    BCM_ERR_NOT_FOUND      No client found for the received packet
 * @retval    BCM_ERR_NOMEM          No space in the receive packet queue
 * @retval    BCM_ERR_UNKNOWN        Client task coud not be notified
 */
int32_t INET_UDPReceiveIndication(const INET_IPAddress aSrcAddr,
                                  const INET_Port aSrcPort,
                                  const INET_IPAddress aDestAddr,
                                  const INET_Port aDestPort,
                                  const INET_L4Protocol aL4Protocol,
                                  const uint8_t* const aBuf,
                                  const uint16_t aLen);

#endif /* INET_SYSTEM_H */
/** @} */

