/*****************************************************************************
Copyright 2016-2019 Broadcom Limited.  All rights reserved.

This program is the proprietary software of Broadcom Corporation and/or its
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
  @defgroup grp_avbl_ifc AVB Listener Interface
  @ingroup grp_avbl

  @addtogroup grp_avbl_ifc
  @{
  @file avb_listener.h
  @brief Data structures and interface APIs of the AVB Listener Component
  The system framework or the media application can use these
  interfaces to work with the AVBL Component.
  To be updated:
    1. Sequence diagrams.
    2. State machine.
    3. Other information regarding component design.
*/

#ifndef AVB_LISTENER_H
#define AVB_LISTENER_H

 /* Includes */
#include <component.h>
#include <nif.h>

/**
    @name AVB Listener API IDs
    @{
    @brief API IDs for AVB Listener Component
*/
#define BRCM_ARCH_AVBL_AVBL_GETCOMPFNTABLE_PROC     (0x01U) /**< @brief #AVBL_GetCompFnTable */
#define BRCM_ARCH_AVBL_COMP_RX_PKT_INDICATION_PROC  (0x02U) /**< @brief #AVBL_RxPktIndication */
#define BRCM_ARCH_AVBL_COMP_TX_PKT_CONF_PROC        (0x03U) /**< @brief #AVBL_TxPktConfirmation */
/** @} */

/**
 * @brief Get the interface functions of AVBL component
 *
 * @behavior Sync, Non-reentrant
 *
 * @pre None
 *
 * API used by framework to get the interface functions of the
 * AVBL Component
 *
 * @param None
 *
 * @retval Pointer to the MSYS_CompFnTblType structure
 *
 * @post None
 *
 * @trace #TBD
 *
 * @limitations None
 */
const MSYS_CompFnTblType* AVBL_GetCompFnTable(void);

/**
 * @brief Receive Ethernet L2 Packet
 *
 * @behavior Sync, Non-reentrant
 *
 * @pre None
 *
 * Invoked by the network interface to handover L2 packet to the module
 * for processing. The unprivileged context of Ethernet driver invokes
 * this callback
 *
 * @param[in]   ctrlIdx       NIF controller identifier
 * @param[in]   pktBuf        Pointer to payload
 * @param[in]   pktLen        Length of payload
 * @param[in]   srcMacAddr    Source MAC address
 * @param[in]   destMacAddr   Destination MAC address
 * @param[in]   priority      Packet priority
 *
 * @retval None
 *
 * @post None
 *
 * @trace #BRCM_ARCH_AVBL_COMP_RX_PKT_INDICATION_PROC #BRCM_REQ_AVBL_MEDIA_COMPONENT
 *
 * @limitations None
 */
void AVBL_RxPktIndication (NIF_CntrlIDType ctrlIdx,
                           const uint8_t *const pktBuf,
                           uint16_t pktLen,
                           const uint8_t *const srcMacAddr,
                           const uint8_t *const destMacAddr,
                           uint32_t priority);

/**
 * @brief Transmit L2 Packet transmit confirmation
 *
 * @behavior Sync, Non-reentrant
 *
 * @pre None
 *
 * Invoked by the network interface to inform that an outbound Tx Ethernet
 * packet was transmitted out on network.
 *
 * @param[in]   ctrlIdx       NIF controller identifier
 * @param[in]   bufIdx        Buffer identifier for the L2 packet
 *
 * @retval None
 *
 * @post None
 *
 * @trace #BRCM_ARCH_AVBL_COMP_TX_PKT_CONF_PROC #BRCM_REQ_AVBL_MEDIA_COMPONENT
 *
 * @limitations None
 */
void AVBL_TxPktConfirmation (NIF_CntrlIDType ctrlIdx,
                             uint32_t bufIdx);

#endif /* AVB_LISTENER_H */
/** @} */
