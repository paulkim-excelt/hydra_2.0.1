/*****************************************************************************
Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_avbt_ifc Interface
    @ingroup grp_avbt

    @addtogroup grp_avbt_ifc
    @{
    @section sec_avbt_overview Overview
    @image html avbt_overview.jpg "AVB Talker Overview"

    The above figure depicts the block diagram of Audio Video Bridging
    (AVB) Talker component, also referred as AVBT.
    The component interfaces with the multimedia framework for control
    and configuration. AVBT uses the AVB library which has interfaces
    for 1722 packetization functionality.

    The AVBT interfaces with the Ethernet driver via NIF for transmission
    of packets. The system timer from AVT block is used for time-stamping
    the AVTP packets along with the network time derived from the
    ethernet driver.

    @section sec_avbt_fsm State Machine
    @image html avbt_state_machine.jpg "AVB Talker State Machine"

    The AVB Talker component at power up is in Reset state.
    Once it has been initialized using COMP_INIT_CMD, it moves to the
    'Initialized' state. It transitions to the 'Ready' state once it
    receives the predefined configuration via COMP_CONFIG_CMD.
    The transition to the 'Start' state happens when the AVBT completes
    processing the COMP_START_CMD command. The COMP_STOP_CMD command
    moves the component back to 'Ready' state. The COMP_DEINIT_CMD
    transitions the component from the 'Ready' or 'Initialized'
    state to 'Reset' state.

    @section sec_avbtp_seq Sequence Diagrams
    @image html avbt_operation_sequence.jpg "AVB Talker Sequence Diagram"

    @limitations
     -# Only H264 Video and PCM Audio payloads are supported
     -# A maximum of 4 streams of either video or audio payload
        is supported by AVB Talker component

    @section sec_avbt_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc avbt.dep

    @file avb_talker.h
    @brief Data structures and interface APIs of the AVBT Component
    @version 1.0 Design of AVBT component
*/

#ifndef AVB_TALKER_H
#define AVB_TALKER_H

 /* Includes */
#include <component.h>
#include <nif.h>

/**
    @name AVB Talker Architecture IDs
    @{
    @brief Architecture IDs for AVBT Component Interface
*/
#define BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC        (0x81U) /**< @brief #AVBT_GetCompFnTable */
#define BRCM_SWARCH_AVBT_RXPKTINDICATION_PROC       (0x82U) /**< @brief #AVBT_RxPktIndication */
#define BRCM_SWARCH_AVBT_TXPKTCONFIRMATION_PROC     (0x83U) /**< @brief #AVBT_TxPktConfirmation */
#define BRCM_SWARCH_AVBT_BUFAVAILINDICATION_PROC    (0x84U) /**< @brief #AVBT_BufAvailIndication */
/** @} */

/**
    @brief Get the interface functions of AVBT component

    @behavior Sync, Non-reentrant

    @pre None

    API used by framework to get the interface functions of the
    AVBT Component

    @param None

    @retval Pointer to the MSYS_CompFnTblType structure

    @post None

    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

    @limitations None
 */
const MSYS_CompFnTblType* AVBT_GetCompFnTable(void);

/**
    @brief Receive ethernet L2 Packet

    @behavior Sync, Non-reentrant

    @pre None

    Invoked by the network interface to handover L2 packet to the module
    for processing. The unprivileged context of Ethernet driver invokes
    this callback. Incoming Data needs to be copied by the component
    and control returned as soon as possible.

    @param[in]   aCtrlIdx       NIF controller identifier
    @param[in]   aBuf           Pointer to payload
    @param[in]   aLen           Length of payload
    @param[in]   aSrcMacAddr    Source MAC address
    @param[in]   aDestMacAddr   Destination MAC address
    @param[in]   aPriority      Packet priority

    @retval None

    @post None

    @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

    @limitations None
 */
void AVBT_RxPktIndication (NIF_CntrlIDType aCtrlIdx,
                           const uint8_t *const aBuf,
                           uint16_t aLen,
                           const uint8_t *const aSrcMacAddr,
                           const uint8_t *const aDestMacAddr,
                           uint32_t aPriority);

/**
    @brief L2 packet transmit confirmation

    @behavior Sync, Non-reentrant

    @pre None

    Invoked by the network interface to inform that an outbound Tx Ethernet
    packet was transmitted out on network.

    @param[in]   aCtrlIdx       NIF controller identifier
    @param[in]   aBufIdx        Buffer identifier for the L2 packet

    @retval None

    @post None

    @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

    @limitations None
 */
void AVBT_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                             uint32_t aBufIdx);

/**
    @brief Buffer available indication

    @behavior Sync, Non-reentrant

    @pre None

    Invoked by the network interface to inform that a buffer is available
    based on the output transmit credits available for the stream

    @param[in]   aCtrlIdx       NIF controller identifier

    @retval None

    @post None

    @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

    @limitations None
 */
void AVBT_BufAvailIndication(NIF_CntrlIDType aCtrlIdx);

#endif /* AVB_TALKER_H */
/** @} */
