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
/******************************************************************************
File Name  : ethif.c
Description:

 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "bcm_utils.h"
#include "bcm_err.h"
#include "nif.h"
#include "eth_osil.h"
#include "inet_system.h"
#include "netif.h"

#ifdef ULOG
#include "ulog.h"
#define GetModuleLogLevel() ULOG_LVL_ERROR
#else
#define ULOG_ERR(...)
#endif

#define INET_ETH_MTU           (1500U)
#define INET_VLAN_ETHTYPE      (0x8100U)
#define INET_IP_ETHTYPE        (0x800U)

/******************************************************************************
 FUNCTION NAME: INET_NetIfGetMTU

 ARGUMENTS:
             aIntfId  [in] -> L2 Interface Identifier

 DESCRIPTION: Retrieves the maximum transmit unit for an interface

 RETURNS: MTU for an L2 interface
******************************************************************************/
uint16_t INET_NetIfGetMTU(const INET_InterfaceId aIntfId)
{
    return INET_ETH_MTU;
}

/******************************************************************************
 FUNCTION NAME: INET_NetIfGetTxBuffer

 ARGUMENTS:
             aIntfId    [in]  -> Interface Identifier
             aEtherType [in]  -> Ethernet ethtype field in ETHER II packet
             aBufIdx    [in]  -> Network buffer identifier
             aBuf       [out] -> Network buffer
             aLen       [out] -> Length of the retrieved buffer in bytes

 DESCRIPTION: Allocate and fetch an network interface buffer for transmission

 RETURNS: BCM_ERR_OK             network buffer allocated successfully
          BCM_ERR_INVAL_PARAMS   Returned in the following cases:
                                 1. aBuf or aLen or aBufIdx is NULL
                                 2. aIntfId  is invalid
          BCM_ERR_NOMEM          Network interface could not allocate a
                                 transmit buffer
******************************************************************************/
int32_t INET_NetIfGetTxBuffer(const INET_InterfaceId aIntfId,
                              uint16_t aEtherType,
                              uint32_t* const aBufIdx,
                              uint8_t** aBuf,
                              uint32_t* const aLen)
{
    int32_t                    rc;
    INET_InterfaceObject       *ifObj = NULL;

    rc = INET_GetInterface(aIntfId, &ifObj);
    if (BCM_ERR_OK == rc) {
        if (NIF_CNTRLIDTYPE_INVALID == ifObj->nifCtrlIdx){
            rc = NIF_GetCtrlIdx(ifObj->ifCfg->vlanConfig.vlan,
                                BCM_NET_ID,
                                0U,
                                &ifObj->nifCtrlIdx);
        }
        if(BCM_ERR_OK == rc) {
            rc = NIF_GetTxBuffer(ifObj->nifCtrlIdx,
                                 aEtherType,
                                 ifObj->ifCfg->vlanConfig.pcp,
                                 aBufIdx,
                                 aBuf,
                                 aLen);
        }
    }

    return rc;
}

/******************************************************************************
 FUNCTION NAME: INET_NetIfSend

 ARGUMENTS:
             aIntfId      [in] -> L2 Interface Identifier
             aEtherType   [in] -> Ethernet ethtype field in ETHER II packet
             aDestMACAddr [in] -> Destination MAC address
             aBufIdx      [in] -> L2 buffer identifier
             aBuf         [in] -> L2 buffer
             aLen         [in] -> Length of the retrieved L2 buffer in bytes

 DESCRIPTION: Transmit an L2 frame on the given interface

 RETURNS: BCM_ERR_OK             L2 frame enqueued by the interface driver
                                 for transmission
          BCM_ERR_INVAL_PARAMS   aLen is 0U or aBuf is NULL
******************************************************************************/
int32_t INET_NetIfSend(const INET_InterfaceId aIntfId,
                       uint16_t               aEtherType,
                       const uint8_t *const   aDestMACAddr,
                       const uint32_t         aBufIdx,
                       uint8_t* const         aBuf,
                       const uint32_t         aLen)
{
    int32_t                    rc;
    INET_InterfaceObject*      ifObj = NULL;

    rc = INET_GetInterface(aIntfId, &ifObj);
    if (BCM_ERR_OK == rc) {
        if (NIF_CNTRLIDTYPE_INVALID == ifObj->nifCtrlIdx){
            rc = NIF_GetCtrlIdx(ifObj->ifCfg->vlanConfig.vlan,
                                BCM_NET_ID,
                                0U,
                                &ifObj->nifCtrlIdx);
        }
        if(BCM_ERR_OK == rc) {
            rc = NIF_Send(ifObj->nifCtrlIdx,
                          aEtherType,
                          1UL,
                          aBufIdx,
                          aLen,
                          aDestMACAddr);
            if (BCM_ERR_OK == rc) {
                /* Track the number of L2 packet transmitted */
                ifObj->ifStats->txL2PktCount++;
            }
        }
    }
    return rc;
}

/******************************************************************************
 FUNCTION NAME: INET_NetIfGetMACAddr

 ARGUMENTS:
           aIntfId  [in]  -> L2 Interface Identifier
           aBuf     [out] -> Buffer start address where the interface
                             MAC address needs to be written to

 DESCRIPTION: Retrieve the MAC address of the given interface

 RETURNS: BCM_ERR_OK             MAC address retrieved successfully
          BCM_ERR_INVAL_PARAMS   aBuf is NULL or aIntfId is invalid
******************************************************************************/
int32_t INET_NetIfGetMACAddr(const INET_InterfaceId aIntfId,
                             uint8_t* const         aBuf)
{
    int32_t rc = BCM_ERR_OK;
    INET_InterfaceObject *ifObj = NULL;

    rc = INET_GetInterface(aIntfId, &ifObj);
    if (BCM_ERR_OK == rc) {
        if (NIF_CNTRLIDTYPE_INVALID == ifObj->nifCtrlIdx){
            rc = NIF_GetCtrlIdx(ifObj->ifCfg->vlanConfig.vlan,
                                BCM_NET_ID,
                                0U,
                                &ifObj->nifCtrlIdx);
        }
        if(BCM_ERR_OK == rc) {
            rc = NIF_GetMacAddr(ifObj->nifCtrlIdx,aBuf);
        }
    }
    return rc;
}

/******************************************************************************
 FUNCTION NAME: INET_NetIfParseHdr

 ARGUMENTS:
             aCtrlIdx [in] -> system ethernet control index
             aVlan   [out] -> Parsed VLAN Id
             aHwIdx  [out] -> Parsed Ethertype Idx

 DESCRIPTION: Parse the received buffer from L2

 RETURNS:
           BCM_ERR_OK             L2 header parsed successfully
           BCM_ERR_INVAL_PARAMS   Buffer too short
******************************************************************************/
int32_t INET_NetIfParseHdr(NIF_CntrlIDType aCtrlIdx,
                           uint16_t* aVlan,
                           uint32_t* aHwIdx)
{
    int32_t retVal;

    *aHwIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    retVal = NIF_GetVlanID(aCtrlIdx, aVlan);

    return retVal;
}

