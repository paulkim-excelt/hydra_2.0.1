/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
  @file netif.h
  @brief Network interface of the Internet Stack

  This file provides the integration interface for a network driver

  @version 0.86 Imported from MS Word document
*/

#ifndef NETIF_H
#define NETIF_H


/** @def INET_MACADDR_LEN
    @brief Length of MAC Address in bytes

    @trace #BRCM_ARCH_INETCOMP_MACADDRLEN_MACRO #BRCM_REQ_INETCOMP_MACADDRLEN
*/
#define INET_MACADDR_LEN                  (6U)

/**
 * @brief Get MTU for an interface
 *
 * @pre None
 *
 * Retrieve the maximum transmission unit (MTU) for a network interface
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in] aIntfId                L2 Interface index
 *
 * @retval    uint16_t               Length of the maximum sized packet allowed
 *                                   to be transmitted on the network interface
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_NETIFGETMTU_PROC  #BRCM_REQ_INETCOMP_NETIFGETMTU
 *
 * @limitations None
 */
uint16_t INET_NetIfGetMTU(const INET_InterfaceId aIntfId);

/**
 * @Get transmit buffer for an interface
 *
 * @pre None
 *
 *  Allocate an fetch an L2 buffer for transmission
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[in]  aEtherType             Ethernet ethtype field in ETHER II packet
 * @param[out] aBufIdx                L2 buffer identifier
 * @param[out] aBuf                   L2 buffer
 * @param[out] aLen                   Length of the retrieved L2 buffer in bytes
 *
 * @retval  #BCM_ERR_OK               L2 buffer allocated successfully
 * @retval  #BCM_ERR_INVAL_PARAMS     Returned in the following cases:
 *                                      1. aBuf or aLen or aBufIdx is NULL
 *                                      2. aIntfId  is invalid
 * @retval  #BCM_ERR_NOMEM            Network interface could not allocate a
 *                                    transmit buffer
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_NETIFGETTXBUF_PROC  #BRCM_REQ_INETCOMP_NETIFGETTXBUF
 *
 * @limitations None
 */
int32_t INET_NetIfGetTxBuffer(const INET_InterfaceId aIntfId,
                              uint16_t               aEtherType,
                              uint32_t* const        aBufIdx,
                              uint8_t**              aBuf,
                              uint32_t* const        aLen);

/**
 * @Send a packet on an interface
 *
 * @pre Buffer should have been allocated using INET_NetIfGetTxBuffer()
 *
 * Transmit an L2 frame on the given interface
 *
 * @behavior Async, Re-entrant
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[in]  aEtherType             Ethernet ethtype field in ETHER II packet
 * @param[in]  aDestMACAddr           Destination MAC address to which the
 *                                    packet needs to be destined
 * @param[in]  aBufIdx                L2 buffer identifier
 * @param[in]  aBuf                   L2 buffer
 * @param[in]  aLen                   Length of the L2 frame in bytes
 *
 * @retval  #BCM_ERR_OK               L2 frame enqueued by the network interface
 *                                    for transmission
 * @retval  #BCM_ERR_INVAL_PARAMS     aLen is 0U or aBuf is NULL
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_NETIFSEND_PROC  #BRCM_REQ_INETCOMP_NETIFSEND
 *
 * @limitations None
 */
int32_t INET_NetIfSend(const INET_InterfaceId aIntfId,
                       uint16_t               aEtherType,
                       const uint8_t* const   aDestMACAddr,
                       const uint32_t         aBufIdx,
                       uint8_t* const         aBuf,
                       const uint32_t         aLen);

/**
 * @Retrieve MAC address of an interface
 *
 * @pre None
 *
 * Retrieve the MAC address of the given interface
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]  aIntfId                L2 Interface index
 * @param[out] aBuf                   Buffer start address where the interface
 *                                    MAC address needs to be written to
 *
 * @retval  #BCM_ERR_OK               MAC address retrieved successfully
 * @retval  #BCM_ERR_INVAL_PARAMS     aBuf is NULL or aIntfId is invalid
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_NETIFGETMAC_PROC  #BRCM_REQ_INETCOMP_NETIFGETMAC
 *
 * @limitations None
 */
int32_t INET_NetIfGetMACAddr(const INET_InterfaceId aIntfId,
                             uint8_t* const         aBuf);

/**
 * @Retrieve Parse the data from L2
 *
 * @pre None
 *
 * Parse the received buffer from L2
 *
 * @behavior Sync, Re-entrant
 *
 * @param[in]  aCtrlIdx               NIF controller index
 * @param[out] aVlan                  Parsed VLAN Id
 * @param[out] aHwIdx                 Parsed Ethertype Idx
 *
 * @retval  #BCM_ERR_OK               L2 header parsed successfully
 * @retval  #BCM_ERR_INVAL_PARAMS     Buffer too short
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_NETIFPARSEHDR_PROC  #BRCM_REQ_INETCOMP_NETIFPARSEHDR
 *
 * @limitations None
 */
int32_t INET_NetIfParseHdr(NIF_CntrlIDType aCtrlIdx,
                           uint16_t* aVlan,
                           uint32_t* aHwIdx);

#endif /* NETIF_H */
/** @} */
