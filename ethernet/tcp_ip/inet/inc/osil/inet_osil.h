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

/** @defgroup grp_inetlib_il Integration Guide
  @ingroup grp_inetlib

  @addtogroup grp_inetlib_il
  @{
  @section sec_inetlib_integration Integration Guide
  This page describes functions needed by an integrator.

  @section sec_comp_il_seq Sequence Diagram
  <BR>
  @image html inetlib_il_udptx_seq.png
  Above figure depicts the typical flow of UDP transmission
  @image html inetlib_il_udprx_seq.png
  Above figure depicts the typical flow of UDP reception
  @image html inetlib_il_arprx_seq.png
  Above figure depicts the typical flow of ARP reception

  @file inet_osil.h
  @brief Integration APIs of Internet Stack subsystem
  This file provides the integration interface/APIs for the Internet stack
  @version 0.86 Imported from MS Word document
*/

#ifndef INET_OSIL_H
#define INET_OSIL_H

#include <nif.h>

/**
    @name INET IL API IDs
    @{
    @brief API IDs for INET IL
*/
#define BRCM_ARCH_INETCOMP_CONVERTIPADDR_MACRO     (0x40U) /**< @brief #INET_CONVERT_IP_ADDRESS */
#define BRCM_ARCH_INETCOMP_MACADDRLEN_MACRO        (0x41U) /**< @brief #INET_CONVERT_IP_ADDRESS */
#define BRCM_ARCH_INETCOMP_ENDPOINTID_TYPE         (0x42U) /**< @brief #INET_EndPointId */
#define BRCM_ARCH_INETCOMP_L4PROTOCOL_TYPE         (0x43U) /**< @brief #INET_L4Protocol */
#define BRCM_ARCH_INETCOMP_ARPFLAG_TYPE            (0x44U) /**< @brief #INET_ARPFlag */
#define BRCM_ARCH_INETCOMP_INTERFACEID_TYPE        (0x45U) /**< @brief #INET_InterfaceId */
#define BRCM_ARCH_INETCOMP_NIFCTRLIDX_TYPE         (0x46U) /**< @brief #INET_NIFCtrlIdx */
#define BRCM_ARCH_INETCOMP_DSCP_TYPE               (0x47U) /**< @brief #INET_DSCP */
#define BRCM_ARCH_INETCOMP_NETMASK_TYPE            (0x48U) /**< @brief #INET_Netmask */
#define BRCM_ARCH_INETCOMP_IPADDRPOL_TYPE          (0x49U) /**< @brief #INET_IPAddressAssignmentType */
#define BRCM_ARCH_INETCOMP_REASSBUFSTATE_TYPE      (0x4AU) /**< @brief #INET_ReassemblyBufferState */
#define BRCM_ARCH_INETCOMP_ARPENTRY_TYPE           (0x4BU) /**< @brief #INET_ARPEntry */
#define BRCM_ARCH_INETCOMP_INTFVLANTAGCONFIG_TYPE  (0x4CU) /**< @brief #INET_InterfaceVlanTagConfig */
#define BRCM_ARCH_INETCOMP_INTFSTATS_TYPE          (0x4DU) /**< @brief #INET_InterfaceStats */
#define BRCM_ARCH_INETCOMP_INTFCONFIG_TYPE         (0x4EU) /**< @brief #INET_InterfaceConfig */
#define BRCM_ARCH_INETCOMP_INTFOBJ_TYPE            (0x4FU) /**< @brief #INET_InterfaceObject */
#define BRCM_ARCH_INETCOMP_SYSINTF_TYPE            (0x50U) /**< @brief #INET_SystemInterface */
#define BRCM_ARCH_INETCOMP_ENDPTSTATE_TYPE         (0x51U) /**< @brief #INET_EndPointState */
#define BRCM_ARCH_INETCOMP_ENDPTIPPORT_TYPE        (0x52U) /**< @brief #INET_EndPointIPPort */
#define BRCM_ARCH_INETCOMP_ENDPTOBJ_TYPE           (0x53U) /**< @brief #INET_EndPointObject */
#define BRCM_ARCH_INETCOMP_SYSENDPT_TYPE           (0x54U) /**< @brief #INET_SystemEndPoint */
#define BRCM_ARCH_INETCOMP_REASSBUF_TYPE           (0x55U) /**< @brief #INET_ReassemblyBuffer */
#define BRCM_ARCH_INETCOMP_RXPKT_TYPE              (0x56U) /**< @brief #INET_RxPkt */

#define BRCM_ARCH_INETCOMP_INIT_PROC               (0x60U) /**< @brief #INET_Init */
#define BRCM_ARCH_INETCOMP_PROCESSTIMERS_PROC      (0x61U) /**< @brief #INET_ProcessTimers */
#define BRCM_ARCH_INETCOMP_GETINTFFORENDPT_PROC    (0x62U) /**< @brief #INET_GetInterfaceForEndPoint */
#define BRCM_ARCH_INETCOMP_GETINTF_PROC            (0x63U) /**< @brief #INET_GetInterface */
#define BRCM_ARCH_INETCOMP_GETINTFCNT_PROC         (0x64U) /**< @brief #INET_GetInterfaceCount */
#define BRCM_ARCH_INETCOMP_GETENDPT_PROC           (0x65U) /**< @brief #INET_GetEndPoint */
#define BRCM_ARCH_INETCOMP_SRCHINTFBYPARAMS_PROC   (0x66U) /**< @brief #INET_SearchInterface */
#define BRCM_ARCH_INETCOMP_ADDARPENTRY_PROC        (0x68U) /**< @brief #INET_AddARPEntry */
#define BRCM_ARCH_INETCOMP_REMOVEARPENTRY_PROC     (0x69U) /**< @brief #INET_RemoveARPEntry */
#define BRCM_ARCH_INETCOMP_FINDARPENTRY_PROC       (0x6AU) /**< @brief #INET_FindARPEntry */
#define BRCM_ARCH_INETCOMP_ARPRSLVADDR_PROC        (0x6BU) /**< @brief #INET_ARPResolveAddress */
#define BRCM_ARCH_INETCOMP_ARPSENDREQ_PROC         (0x6CU) /**< @brief #INET_ARPSendRequest */
#define BRCM_ARCH_INETCOMP_GETREASSBUF_PROC        (0x6DU) /**< @brief #INET_GetReassemblyBuffer */
#define BRCM_ARCH_INETCOMP_UPDTREASSBUF_PROC       (0x6EU) /**< @brief #INET_UpdateReassemblyBuffer */
#define BRCM_ARCH_INETCOMP_FREEREASSBUF_PROC       (0x6FU) /**< @brief #INET_FreeReassemblyBuffer */
#define BRCM_ARCH_INETCOMP_UDPRECVIND_PROC         (0x70U) /**< @brief #INET_UDPReceiveIndication */
#define BRCM_ARCH_INETCOMP_RXIPV4PKT_PROC          (0x71U) /**< @brief #INET_RxIPv4Pkt */
#define BRCM_ARCH_INETCOMP_RXARPPKT_PROC           (0x72U) /**< @brief #INET_RxARPPkt */
#define BRCM_ARCH_INETCOMP_TXPKTCONF_PROC          (0x73U) /**< @brief #INET_TxPktConfirmation */
#define BRCM_ARCH_INETCOMP_BUFAVAIL_PROC           (0x74U) /**< @brief #INET_BufferAvailableIndication */
#define BRCM_ARCH_INETCOMP_SHELL_PROC              (0x75U) /**< @brief #INET_Shell */
#define BRCM_ARCH_INETCOMP_NETIFGETMTU_PROC        (0x76U) /**< @brief #INET_NetIfGetMTU */
#define BRCM_ARCH_INETCOMP_NETIFGETTXBUF_PROC      (0x77U) /**< @brief #INET_NetIfGetTxBuffer*/
#define BRCM_ARCH_INETCOMP_NETIFSEND_PROC          (0x78U) /**< @brief #INET_NetIfSend */
#define BRCM_ARCH_INETCOMP_NETIFGETMACADDR_PROC    (0x79U) /**< @brief #INET_NetIfGetMACAddr */
#define BRCM_ARCH_INETCOMP_NETIFPARSEHDR_PROC      (0x7AU) /**< @brief #INET_NetIfParseHdr */
/** @} */

/**
 * @brief Initialize the internet stack
 *
 * @pre None
 *
 * Performs initialization of the internet stack
 *
 * @behavior Sync, Non re-entrant
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_INIT_PROC  #BRCM_REQ_INETCOMP_INIT
 *
 * @limitations None
 */
void INET_Init(void);

/**
 * @brief Process internet stack timers
 *
 * @pre 3.125ms alarm expires
 *
 * Invoked every 3.125ms to provide the notion of elapsed time to the internet
 * stack and used to implement various timeouts.
 *
 * @behavior Sync, Non re-entrant
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_PROCESSTIMERS_PROC  #BRCM_REQ_INETCOMP_PROCESSTIMERS
 *
 * @limitations None
 */
void INET_ProcessTimers(void);

/**
 * @brief Receive IPv4 Packet
 *
 * @pre None
 *
 * Invoked by the network interface to handover a received packet containing
 * IPv4 payload to the internet stack for processing
 *
 * @param[in]   aCtrlIdx        NIF controller index
 * @param[in]   aBuf            Pointer to the packet buffer
 * @param[in]   aLen            Length of the received packet
 * @param[in]   aSrcMacAddr     Pointer to source MAC address
 * @param[in]   aDestMacAddr    Pointer to destination MAC address
 * @param[in]   aPriority       PCP priority of the packet
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_RXIPV4PKT_PROC    #BRCM_REQ_INETCOMP_RXIPV4PKT
 *
 * @limitations None
 */
void INET_RxIPv4Pkt(NIF_CntrlIDType aCtrlIdx,
                    const uint8_t *const aBuf,
                    uint16_t aLen,
                    const uint8_t *const aSrcMacAddr,
                    const uint8_t *const aDestMacAddr,
                    uint32_t aPriority);

/**
 * @brief Receive ARP Packet
 *
 * @pre None
 *
 * Invoked by the network interface to handover a received packet containing
 * ARP payload to the internet stack for processing
 *
 * @behavior Sync, Non-reentrant
 *
 * @param[in]   aCtrlIdx        NIF controller index
 * @param[in]   aBuf            Pointer to the packet buffer
 * @param[in]   aLen            Length of the received packet
 * @param[in]   aSrcMacAddr     Pointer to source MAC address
 * @param[in]   aDestMacAddr    Pointer to destination MAC address
 * @param[in]   aPriority       PCP priority of the packet
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_RXARPPKT_PROC    #BRCM_REQ_INETCOMP_RXARPPKT
 *
 * @limitations None
 */
void INET_RxARPPkt(NIF_CntrlIDType aCtrlIdx,
                   const uint8_t *const aBuf,
                   uint16_t aLen,
                   const uint8_t *const aSrcMacAddr,
                   const uint8_t *const aDestMacAddr,
                   uint32_t aPriority);

/**
 * @brief IP/ARP packet transmit confirmation
 *
 * @pre None
 *
 * Invoked by network interface to inform that an IP/ARP packet was
 * successfully transmitted
 *
 * @behavior Sync, Non-reentrant
 *
 * @param[in]   aCtrlIdx        NIF controller index
 * @param[in]   aBufIdx         Buffer identifier for the packet
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_TXPKTCONF_PROC    #BRCM_REQ_INETCOMP_TXPKTCONF
 *
 * @limitations None
 */
void INET_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                             uint32_t aBufIdx);

/**
 * @brief L2 buffer available indication
 *
 * @pre None
 *
 * Invoked by the network interface to inform that free L2 buffer is
 * available
 *
 * @behavior Sync, Non-reentrant
 *
 * @param[in]   aCtrlIdx           NIF controller index
 *
 * @return      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_BUFAVAIL_PROC    #BRCM_REQ_INETCOMP_BUFAVAIL
 *
 * @limitations Even though callback is received, any free buffers might have
 * been already acquired by higher priority clients. INET stack clients might
 * not find a free buffer on waking up. It can retry on a subsequent
 * invocations of the same callback
 */
void INET_BufferAvailableIndication (NIF_CntrlIDType aCtrlIdx);

/**
 * @brief Handle shell command
 *
 * @pre None
 *
 * This interface is invoked by the Console command handler to handle input
 * INET commands
 *
 * @behavior Sync, Reentrant
 *
 * @param[in]   aConsoleID      Console identifier
 * @param[in]   aArgString      String containing input shell command
 * @param[in]   aArgStringLen   Length of aArgString
 *
 * @retval      void
 *
 * @post None
 *
 * @trace #BRCM_ARCH_INETCOMP_SHELL_PROC    #BRCM_REQ_INETCOMP_SHELL
 *
 * @limitations None
 */
void INET_Shell(uint32_t aConsoleID,
                char* aArgString,
                uint32_t aArgStringLen);

#endif /* INET_OSIL_H */
/** @} */
