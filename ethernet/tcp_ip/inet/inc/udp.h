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
  @defgroup grp_inetlib INET Library
  @ingroup grp_system_sw

  @addtogroup grp_inetlib
  @{
  @section sec_comp_overview Overview
  @image html inetlib_overview.png

  The Internet stack enables UDP & UDP lite based communication with a remote
  host. A client can receive or transmit data using the EndPoint abstraction
  which is used to identify the 5 tuple <transport protocol, local IP address,
  local port, remote IP address, remote port> to be used for packet creation.
      - Only Internet Protocol version 4 (IPv4) is supported at the network
        layer.
          - Support for fragmentation and reassembly
          - Ability to configure the DSCP field to ensure preference for packets
            in the network
      - ARP is supported for resolving H/W (MAC) addresses
      - ICMPv4 – Support for receiving Redirect, Echo Request,
                 Destination Unreachable & Time Exceeded and for transmitting
                 Echo Reply, Destination Unreachable & Time Exceeded messages.
      - The stack implements a true Internet ‘host’ as defined by RFC 1122 with
        no support for routing between multiple interfaces and limited support
        for multi-homing.
      - The entire set of end points and network interfaces are defined in the
        system. INET accesses these end points and interfaces via the APIs
        defined by the system layer.

  @section sec_comp_fsm State Machine
  The Internet stack library is modeled as a memory/state less module.

  @section sec_comp_seq Sequence Diagrams
  @image html inetlib_udptx_frag_seq.png
  Above figure depicts a typical flow of a client using UDP transmission with
  fragmentation
  @image html inetlib_udptx_cancel_seq.png
  Above figure depicts a typical flow of a client canceling UDP transmission
  @image html inetlib_udprx_seq.png
  Above figure depicts a typical flow of a client receiving UDP data

  @section sec_comp_perf Performance & Memory

  | Performance       | Description                                       |
  |:-----------------:|:-------------------------------------------------:|
  | CPU Load          | Throughput test for Transmit traffic: 100 Mbps    |
  |                   | (37500 byte payload), 12% CPU used by the client  |
  |                   | 25 Mbps traffic(10000 byte payload), 3% CPU used  |
  |                   | by the client                                     |
  | Memory Bandwidth  |   NA                                              |
  | HW Utilization    |   NA                                              |

@includedoc etslib_all.dep

@limitations
   - The Internet stack only supports Ethernet or Ethernet like interfaces
   - All endpoints, interfaces and reassembly buffers for Rx are supposed to be
     statically allocated by the system and provided to INET via a defined
     interface
   - No support for loopback interface
   - No support for connection oriented transport (TCP)
   - No support for IPv6 or related protocols
   - No support for dynamic IP assignment or Auto-IP
   - No support for IGMP
   - Only a single task handles transmission for an end point


@file udp.h
@brief UDP API of Internet Stack

This file provides the interface/APIs which can be used to communicate
with a remote peer using the UDP protocol
@version 0.86 Imported from MS Word document
*/

#ifndef UDP_H
#define UDP_H

#include <inet.h>

/**
    @name INET Library API IDs
    @{
    @brief API IDs for INET Library
 */
#define BRCM_ARCH_INETLIB_IPADDRESS_TYPE  (0x1U) /**< @brief INET_IPAddress */
#define BRCM_ARCH_INETLIB_PORT_TYPE       (0x2U) /**< @brief INET_Port */
#define BRCM_ARCH_INETLIB_UDPSEND_PROC    (0x3U) /**< @brief #UDP_Send */
#define BRCM_ARCH_INETLIB_UDPPROCESS_PROC (0x4U) /**< @brief #UDP_Process */
#define BRCM_ARCH_INETLIB_UDPCANCEL_PROC  (0x5U) /**< @brief #UDP_CancelSend */
#define BRCM_ARCH_INETLIB_UDPRECV_PROC    (0x6U) /**< @brief #UDP_Recv */
/** @} */

/**
 * @brief  Send data using UDP
 *
 * @pre None
 *
 *  This API is used to make a request to the Internet stack to transmit
 *  the data provided in the buffer using UDP protocol (as configured in
 *  the system configuration) to the remote end. In case the provided buffer
 *  is larger than the L2 MTU, it might be fragmented and sent. In case enough
 *  network buffers are not available, the API will return BCM_ERR_BUSY and the
 *  client is expected to resume transmission with the UDP_Process API.
 *
 * @behavior Async, Non re-entrant for same end point (identified by aCompId,
 *           aPayloadId)
 *
 * @param[in]   aCompId               Component identifier used to identify the
 *                                    client of the end point
 * @param[in]   aPayloadId            Payload identifier used to identify the
 *                                    client of the end point
 * @param[in]   aBuf                  Buffer containing the UDP payload
 * @param[in]   aLen                  Length of the UDP payload
 * @param[in]   aConfig               End point configuration such as
 *                                    1. Remote IP address (destination for packet)
 *                                    2. Remote UDP port
 *                                    3. Local port to be used for UDP transmission
 *
 * @retval  #BCM_ERR_OK               The complete data has been transmitted.
 *                                    The client is free to reuse the buffer
 * @retval  #BCM_ERR_INVAL_PARAMS     Returned in the following cases:
 *                                     1. aBuf is NULL or aLen is 0
 *                                     2. no UDP end point allocated for the
 *                                       client
 *                                     3. end point does not belong to a valid
 *                                       interface
 *                                     4. end point does not support UDP
 *                                     5. if valid IP addr/port not provided to end point
 * @retval  #BCM_ERR_BUSY             Transmission has not completed (due to
 *                                    unavailability of network buffers) OR if
 *                                    the ARP resolution is in progress. The client
 *                                    should not free/reuse the provided buffer.
 *                                    It should wait for indication of free buffers
 *                                    and execute UDP_Process
 * @retval  #BCM_ERR_INVAL_STATE      Transmission of a previous payload is in
 *                                    progress. The client can cancel the
 *                                    previous transmission using the
 *                                    UDP_CancelSend API
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETLIB_UDPSEND_PROC    #BRCM_REQ_ETSLIB_UDP
 *
 * @limitations When this call returns, the complete payload might not have been
 * transmitted, subject to availability of network buffers.
 */
int32_t UDP_SendTo(const uint64_t aCompId,
                   const uint32_t aPayloadId,
                   const uint8_t* const aBuf,
                   const uint16_t aLen,
                   const INET_EndPointIPPort* const aConfig);

/**
 * @brief  Resume UDP transmission
 *
 * @pre Previously invoked UDP_Send should have returned BCM_ERR_BUSY
 *
 *  This API is used to resume an ongoing UDP transmission on availability of
 *  network buffers.
 *
 * @behavior Async, Non re-entrant for same end point (identified by aCompId,
 *           aPayloadId)
 *
 * @param[in]   aCompId               Component identifier used to identify the
 *                                    client of the end point
 * @param[in]   aPayloadId            Instance identifier used to identify the
 *                                    client of the end point
 *
 * @retval  #BCM_ERR_OK               The complete data has been transmitted.
 *                                    The client is free to reuse the buffer
 * @retval  #BCM_ERR_INVAL_PARAMS     Returned if no UDP end point allocated for
 *                                    the client or if valid ip addr/port is not
 *                                    yet assigned to the end point
 * @retval  #BCM_ERR_BUSY             Transmission has not completed (due to
 *                                    unavailability of network buffers) OR if the
 *                                    ARP resolution is still in progress. The client
 *                                    should not free/reuse the provided buffer.
 *                                    It should wait for indication of free buffers and
 *                                    re-execute UDP_Process
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETLIB_UDPPROCESS_PROC    #BRCM_REQ_ETSLIB_UDP
 *
 * @limitations When this call returns, the complete payload might not have been
 * transmitted, subject to availability of network buffers.
 */
int32_t UDP_Process(const uint64_t aCompId,
                    const uint32_t aPayloadId);

/**
 * @brief  Cancel UDP transmission
 *
 * @pre Previously invoked UDP_Send should have returned BCM_ERR_BUSY
 *
 *  This API is used to cancel the current UDP transmission which is waiting for
 *  availability of network buffers.
 *
 * @behavior Sync, Non re-entrant for same end point (identified by aCompId,
 *           aPayloadId)
 *
 * @param[in]   aCompId               Component identifier used to identify the
 *                                    client of the end point
 * @param[in]   aPayloadId            Instance identifier used to identify the
 *                                    client of the end point
 *
 * @retval  #BCM_ERR_OK               The cancellation request succeeded. The
 *                                    client can now free the payload buffer.
 * @retval  #BCM_ERR_INVAL_PARAMS     Returned if no UDP end point allocated for
 *                                    the client
 * @post None
 *
 * @trace #BRCM_ARCH_INETLIB_UDPCANCEL_PROC    #BRCM_REQ_ETSLIB_UDP
 *
 * @limitations None
 */
int32_t UDP_CancelSend(const uint64_t aCompId,
                       const uint32_t aPayloadId);

/**
 * @brief  Receive data from UDP end point
 *
 * @pre UDPRxEvent was received by the task registered for the end point
 *
 *  This API is used to fetch data for a UDP end point from the Internet stack
 *  received from a remote end point.
 *
 * @behavior Sync, Non-Reentrant for same end point (identified by aCompId,
 *           aPayloadId)
 *
 * @param[in]    aCompId              Component identifier used to identify the
 *                                    client of the end point
 * @param[in]    aPayloadId           Payload identifier used to identify the
 *                                    client of the end point
 * @param[in]    aBuf                 Pointer to client buffer to copy the
 *                                    received data to
 * @param[inout] aLen                 Length of the client buffer. When the call
 *                                    returns this contains the number of bytes
 *                                    of received data copied to the client
 *                                    buffer
 * @param[out]   aSrcIPAddr           IP address of the remote end from which
 *                                    the data was received
 * @param[out]   aSrcPort             Port of the remote end from which the data
 *                                    was received
 * @param[out]   aMoreData            Boolean flag which shall be set to 1 in
 *                                    case there is more data that is to be read
 *                                    by the caller
 *
 * @retval  #BCM_ERR_OK               Received data has been copied to the input
 *                                    buffer. The aLen field is updated with the
 *                                    number of bytes of data copied. The
 *                                    aMoreData field is set to 1 in case there
 *                                    is more data that can be read by the
 *                                    client
 * @retval  #BCM_ERR_INVAL_PARAMS     Returned in the following cases:
 *                                      1. aBuf is NULL or aLen is 0
 *                                      2. no UDP end point allocated for this
 *                                         client
 *                                      3. end point does not belong to a valid
 *                                          interface
 * @post None
 *
 * @trace #BRCM_ARCH_INETLIB_UDPRECV_PROC    #BRCM_REQ_ETSLIB_UDP
 *
 * @limitations If the client provides a buffer which is insufficient to hold
 * all the received bytes, the stack discards the leftover data
 */
int32_t UDP_Recv(const uint64_t aCompId,
                 const uint32_t aPayloadId,
                 uint8_t* const aBuf,
                 uint16_t* const  aLen,
                 INET_IPAddress* const aSrcIPAddr,
                 INET_Port* const aSrcPort,
                 uint8_t* const aMoreData);
#endif /* UDP_H */
/** @} */
