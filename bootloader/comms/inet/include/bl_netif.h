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

#ifndef BL_NETIF_H
#define BL_NETIF_H

/** @file bl_netif.h
    @brief Network interface of the Internet Stack

    This file provides the integration interface for a network driver
*/

/** @def BL_INET_MACADDR_LEN
    @brief Length of MAC Address in bytes
*/
#define BL_INET_MACADDR_LEN                  (6U)

/** @Get MTU for an interface
 *
 *  Retrieve the maximum transmission unit (MTU) for a network interface
 *
 * @param[in] aIntfId                L2 Interface index
 *
 * @retval    uint16_t               Length of the maximum sized packet allowed
 *                                   to be transmitted on the network interface
 */
uint16_t BL_INET_NetIfGetMTU(const BL_INET_InterfaceId aIntfId);

/** @Get transmit buffer for an interface
 *
 *  Allocate an fetch an L2 buffer for transmission
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[in]  aEtherType             Ethernet ethtype field in ETHER II packet
 * @param[out] aBufIdx                L2 buffer identifier
 * @param[out] aBuf                   L2 buffer
 * @param[out] aLen                   Length of the retrieved L2 buffer in bytes
 *
 * @retval     #BL_BCM_ERR_OK             L2 buffer allocated successfully
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   Returned in the following cases:
 *                                    1. aBuf or aLen or aBufIdx is NULL
 *                                    2. aIntfId  is invalid
 * @retval     #BL_BCM_ERR_NOMEM          L2 driver could not allocate a transmit
 *                                    buffer
 */
int32_t BL_INET_NetIfGetTxBuffer(const BL_INET_InterfaceId aIntfId,
                                 uint16_t aEtherType,
                                 uint32_t* const aBufIdx,
                                 uint8_t** aBuf,
                                 uint32_t* const aLen);

/** @Send a packet on an interface
 *
 *  Transmit an L2 frame on the given interface
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[in]  aEtherType             Ethernet ethtype field in ETHER II packet
 * @param[in]  aDestMACAddr           Destination MAC address to which the
 *                                    packet needs to be destined
 * @param[in]  aBufIdx                L2 buffer identifier
 * @param[in]  aBuf                   L2 buffer
 * @param[in]  aLen                   Length of the L2 frame in bytes
 *
 * @retval     #BL_BCM_ERR_OK             L2 frame enqueued by the interface driver
 *                                    for transmission
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   aLen is 0U or aBuf is NULL
 */
int32_t BL_INET_NetIfSend(const BL_INET_InterfaceId aIntfId,
                          uint16_t aEtherType,
                          const uint8_t* const aDestMACAddr,
                          const uint32_t aBufIdx,
                          uint8_t* const aBuf,
                          const uint32_t aLen);

/** @Retrieve MAC address of an interface
 *
 *  Retrieve the MAC address of the given interface
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[out] aBuf                   Buffer start address where the interface
 *                                    MAC address needs to be written to
 *
 * @retval     #BL_BCM_ERR_OK             MAC address retrieved successfully
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   aBuf is NULL or aIntfId is invalid
 */
int32_t BL_INET_NetIfGetMACAddr(const BL_INET_InterfaceId aIntfId,
                                uint8_t* const aBuf);

/** @Retrieve Parse the data from L2
 *
 *  Parse the received buffer from L2
 *
 * @param[in]  aCtrlIdx               system ethernet control index
 * @param[out] aVlan                  Parsed VLAN Id
 * @param[out] aHwIdx                 Parsed Ethertype Idx
 *
 * @retval     #BL_BCM_ERR_OK             L2 header parsed successfully
 * @retval     #BL_BCM_ERR_INVAL_PARAMS   Buffer too short
 */
int32_t BL_INET_NetIfParseHdr(uint32_t aCtrlIdx,
                              uint16_t* aVlan,
                              uint32_t* aHwIdx);

#endif /* BL_NETIF_H */