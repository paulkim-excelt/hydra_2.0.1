/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
 * @file bl_inet_port.h
 *
 * @brief Ethernet INET porting header
 */

#ifndef BL_INETPORT_H
#define BL_INETPORT_H

#include <bl_inet.h>
#include <bl_inet_cfg.h>

/** @brief Initialize the internet stack
 *
 * Performs initialization of the internet stack
 *
 * @retval      BCM_ERR_OK      BL_INET_Init successful
 * @retval      BCM_ERR_EAGAIN  BL_INET_Init in progress, call again
 * @retval      Any other error Error happened
 */
int32_t BL_INET_Init(void);

/** @brief Process internet stack timers
 *
 * Invoked every second to provide the notion of elapsed time to the internet
 * stack and used to implement various timeouts.
 *
 * @return      void
 */
void BL_INET_ProcessTimers(void);

/** @brief Receive IPv4 Packet
 *
 * Invoked by the system ethernet to handover a received packet containing
 * IPv4 payload to the internet stack for processing
 *
 * @param[in]   aCtrlIdx        Controller index in the context of Ethernet
 *                              interface
 * @param[in]   aBuf            Pointer to the packet buffer
 * @param[in]   aLen            Length of the received packet
 * @param[in]   aSrcMacAddr     Pointer to source MAC address
 * @param[in]   aDestMacAddr    Pointer to destination MAC address
 * @param[in]   aPriority       PCP priority of the packet
 *
 * @return      void
 */
void BL_INET_RxIPv4Pkt(uint32_t aCtrlIdx,
                       const uint8_t *const aBuf,
                       uint16_t aLen,
                       const uint8_t *const aSrcMacAddr,
                       const uint8_t *const aDestMacAddr,
                       uint32_t aPriority);

/** @brief Receive ARP Packet
 *
 * Invoked by the system ethernet to handover a received packet containing
 * ARP payload to the internet stack for processing
 *
 * @param[in]   aCtrlIdx        Controller index in the context of SysEth
 * @param[in]   aBuf            Pointer to the packet buffer
 * @param[in]   aLen            Length of the received packet
 * @param[in]   aSrcMacAddr     Pointer to source MAC address
 * @param[in]   aDestMacAddr    Pointer to destination MAC address
 * @param[in]   aPriority       PCP priority of the packet
 *
 * @return      void
 */
void BL_INET_RxARPPkt(uint32_t aCtrlIdx,
                      const uint8_t *const aBuf,
                      uint16_t aLen,
                      const uint8_t *const aSrcMacAddr,
                      const uint8_t *const aDestMacAddr,
                      uint32_t aPriority);

/** @brief  Get L2 interface for an end point
 *
 *  Used to retrieve the L2 interface for an end point
 *
 * @param[in]   aEndPointId           End point identifier
 * @param[out]  aIntfObj              L2 Interface Object
 *
 * @retval      BCM_ERR_OK            L2 interface successfully
 *                                    retrieved
 * @retval      BCM_ERR_INVAL_PARAMS  aIntfId is NULL or end point not found
 */
int32_t BL_INET_GetInterfaceForEndPoint(const BL_INET_EndPointId aEndPointId,
                                        BL_INET_InterfaceObject** const aIntfObj);

/** @brief  Get L2 interface for an end point
 *
 *  Used to retrieve the L2 interface for an end point
 *
 * @param[in]   aIntfId               L2 interface identifier
 * @param[out]  aIntfObj              L2 Interface Object
 *
 * @retval      BCM_ERR_OK            L2 interface successfully
 *                                    retrieved
 * @retval      BCM_ERR_INVAL_PARAMS  Interface not found
 */
int32_t BL_INET_GetInterface(const BL_INET_InterfaceId aIntfId,
                             BL_INET_InterfaceObject** const aIntfObj);

/** @brief  Get L2 interface count
 *
 *  Used to retrieve the count of L2 interfaces created
 *
 * @param[out]  aIntfCount            L2 interface count
 *
 * @return      Count of interfaces
 */
uint32_t BL_INET_GetInterfaceCount(void);

/** @brief  Get end point information
 *
 *  Used to retrieve the end point for a client along with its configuration and
 *  context
 *
 * @param[in]  aPayloadId             Payload identifier used to identify the
 *                                    client of the end point
 * @param[out] aEndPointId            End point identifier for the end point of
 *                                    the client
 * @param[out] aEndPointObj           Configuration for the end point and
 *                                    context/scratch memory for the end point
 *
 * @retval     BCM_ERR_OK             End point successfully retrieved
 * @retval     BCM_ERR_INVAL_PARAMS   aEndPointId is NULL
 * @retval     BCM_ERR_NOT_FOUND      End point not found
 */
int32_t BL_INET_GetEndPoint(const uint32_t aPayloadId,
                            BL_INET_EndPointObject** const aEndPointObj);

/** @brief Finds the H/W address for an IP address
 *
 *  Retrieves the H/W address from the cache (if it exists) for unicast entry.
 *  On ARP cache lookup failure, sends an ARP request. For broadcast and
 *  multicast IP address returns the mapped H/W address.
 *
 * @param[in]  aIntfObj               Interface configuration and stats
 * @param[in]  aIPAddr                IP address that needs to be resolved
 * @param[out] aMACAddr               MAC address resolved by ARP
 * @param[inout] arpIndex             Index of ARP cache where it was last found
 *
 * @retval     BCM_ERR_OK             HW address resolved successfully
 * @retval     BCM_ERR_INVAL_PARAMS   aIPAddr is invalid
 * @retval     BCM_ERR_NOT_FOUND      HW address not resolved
 */
int32_t BL_INET_ARPResolveAddress(BL_INET_InterfaceObject* aIntfObj,
                                  const BL_INET_IPAddress aIPAddr,
                                  uint8_t* const aMACAddr,
                                  uint32_t* const arpIndex);

/** @brief Sends an ARP request
 *
 *  Constructs an ARP request message and sends it on the network interface
 *
 * @param[in]  aIntfId                Interface identifier
 * @param[in]  aIPAddr                IP Address to be resolved
 * @param[in]  aFlags                 Current flags of the ARP entry
 *
 * @retval     void
 */
void BL_INET_ARPSendRequest(const BL_INET_InterfaceId aIntfId,
                            const BL_INET_IPAddress aIPAddr,
                            const uint32_t aFlags);

/** @brief Adds/updates an entry in the ARP cache
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
 * @retval     BCM_ERR_NOSUPPORT      Entry conflicts with a static entry
 *                                    already present in the cache
 */
int32_t BL_INET_AddARPEntry(const BL_INET_InterfaceId aIntfId,
                            const BL_INET_IPAddress aIPAddress,
                            const uint8_t* const aMACAddr,
                            const uint32_t aFlags,
                            const uint8_t aUpdateAddr);

/** @brief Finds an entry in the ARP cache
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
int32_t BL_INET_FindARPEntry(const BL_INET_InterfaceId aIntfId,
                             const BL_INET_IPAddress aIPAddress,
                             uint8_t* const aMACAddr,
                             uint32_t* const aFlags,
                             uint32_t* const arpIndex);

/** @brief Removes an entry from the ARP cache
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
int32_t BL_INET_RemoveARPEntry(const BL_INET_InterfaceId aIntfId,
                               const BL_INET_IPAddress aIPAddress);

/** @brief Searches for an interface using VLAN as key
 *
 *  Used to search for the L2 interface identifier from the system configuration
 *  The key is <aHwId , aVlan> for the tagged case and <aHwId> otherwise
 *
 * @param[in]  aHwId                  HW index of the interface
 * @param[in]  aVlan                  VLAN tag, if the interface is tagged.
 *                                    Otherwise 0U
 * @param[out] aIntfId                L2 interface identifier
 *
 * @retval     #BL_BCM_ERR_OK             L2 interface identifier retrieved
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   aIntfId is NULL
 * @retval     #BL_BCM_ERR_NOT_FOUND      No such valid interface exists
 */
int32_t BL_INET_SearchInterfaceByVlan(const uint32_t aHwId,
                                      const uint16_t aVlan,
                                      BL_INET_InterfaceId *const aIntfId);

/** @brief Searches for an interface using IP Address as key
 *
 *  Used to search for the L2 interface identifier from the system configuration
 *  The key is <aHwId , aIPAddr>
 *
 * @param[in]  aHwId                  HW index of the interface
 * @param[in]  aIPAddress             IP Address
 * @param[out] aIntfId                L2 interface identifier
 *
 * @retval     #BL_BCM_ERR_OK             L2 interface identifier retrieved
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   aIntfId is NULL
 * @retval     #BL_BCM_ERR_NOT_FOUND      No such valid interface exists
 */
int32_t BL_INET_SearchInterfaceByIPAddr(const uint32_t aHwId,
                                        const BL_INET_IPAddress aIPAddress,
                                        BL_INET_InterfaceId *const aIntfId);

/** @brief Inform client of received UDP data
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
 * @retval    #BL_BCM_ERR_OK             Client notified successfully
 * @retval    #BL_BCM_ERR_NOT_FOUND      No client found for the received packet
 * @retval    #BL_BCM_ERR_NOMEM          No space in the receive packet queue
 * @retval    #BL_BCM_ERR_UNKNOWN        Client task coud not be notified
 */
int32_t BL_INET_UDPReceiveIndication(const BL_INET_IPAddress aSrcAddr,
                                     const BL_INET_Port aSrcPort,
                                     const BL_INET_IPAddress aDestAddr,
                                     const BL_INET_Port aDestPort,
                                     const BL_INET_L4Protocol aL4Protocol,
                                     const uint8_t* const aBuf,
                                     const uint16_t aLen);

#endif /* BL_INETPORT_H */