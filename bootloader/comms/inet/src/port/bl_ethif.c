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
/******************************************************************************
File Name  : bl_ethif.c
Description:

 ******************************************************************************/
#include <stdint.h>
#include <string.h>
#include "bl_utils.h"
#include "bl_bcm_err.h"
#include "bl_ethernet.h"
#include "bl_eth_xcvr.h"
#include "bl_inet_port.h"
#include "bl_netif.h"

#include "bl_log.h"
#define BL_GetModuleLogLevel() BL_LOG_LVL_ERROR

#define BL_INET_ETH_CHAN_ID        (0UL)
#define BL_INET_ETH_XCVR_HW_ID     (0UL)

#define BL_INET_BL_ETH_MTU            (1500U)

#define BL_VLAN_TAG_SIZE           (4UL)

/******************************************************************************
 FUNCTION NAME: BL_INET_NetIfGetMTU

 ARGUMENTS:
             aIntfId  [in] -> L2 Interface Identifier

 DESCRIPTION: Retrieves the maximum transmit unit for an interface

 RETURNS: MTU for an L2 interface
******************************************************************************/
uint16_t BL_INET_NetIfGetMTU(const BL_INET_InterfaceId aIntfId)
{
    return BL_INET_BL_ETH_MTU;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_NetIfGetTxBuffer

 ARGUMENTS:
             aIntfId    [in]  -> Interface Identifier
             aEtherType [in]  -> Ethernet ethtype field in ETHER II packet
             aBufIdx    [in]  -> Network buffer identifier
             aBuf       [out] -> Network buffer
             aLen       [out] -> Length of the retrieved buffer in bytes

 DESCRIPTION: Allocate and fetch an network interface buffer for transmission

 RETURNS: BL_BCM_ERR_OK             network buffer allocated successfully
          BL_BCM_ERR_INVAL_PARAMS   Returned in the following cases:
                                 1. aBuf or aLen or aBufIdx is NULL
                                 2. aIntfId  is invalid
          BL_BCM_ERR_NOMEM          Network interface could not allocate a
                                 transmit buffer
******************************************************************************/
int32_t BL_INET_NetIfGetTxBuffer(const BL_INET_InterfaceId aIntfId,
                                 uint16_t aEtherType,
                                 uint32_t* const aBufIdx,
                                 uint8_t** aBuf,
                                 uint32_t* const aLen)
{
    int32_t                     rc;
    BL_ETHER_VLANIDType            vlanID;
    BL_ETHER_HwIDType              hwIdx;
    BL_ETHER_PrioType              priority;

    BL_INET_InterfaceObject       *ifObj = NULL;
    rc = BL_INET_GetInterface(aIntfId, &ifObj);
    if (BL_BCM_ERR_OK == rc) {
        if (BL_INET_NIFCTRLIDX_INVALID == ifObj->nifCtrlIdx){
            ifObj->nifCtrlIdx = ifObj->ifCfg->hwIndex
                | ((ifObj->ifCfg->vlanConfig.vlan << 8UL) & 0xFFF00UL);
        }
        if(BL_BCM_ERR_OK == rc) {
            hwIdx = ifObj->ifCfg->hwIndex;
            vlanID = ifObj->ifCfg->vlanConfig.vlan;
            priority = (ifObj->ifCfg->vlanConfig.pcp & 0x7U);

            /* Reserve space for source and destination MAC address */
            *aLen += (2UL * BL_ETHER_MAC_ADDR_SIZE);

            if ((vlanID >= BL_ETHER_VLANID_MIN)
                    && (vlanID <= BL_ETHER_VLANID_MAX)){
                /* Space for VLAN tag */
                *aLen += 4UL;
            }
            /* Space for Ether type */
            *aLen += 2UL;
            rc = BL_ETHER_GetTxBuffer(hwIdx, BL_INET_ETH_CHAN_ID, aBufIdx, aBuf, aLen);
            if (BL_BCM_ERR_OK != rc) {
                goto exit;
            }

            rc = BL_ETHER_XcvrGetMacAddr(BL_INET_ETH_XCVR_HW_ID,
                    &((*aBuf)[BL_ETHER_MAC_ADDR_SIZE]));
            if (BL_BCM_ERR_OK != rc) {
                goto exit;
            }

            *aLen -= (2UL * BL_ETHER_MAC_ADDR_SIZE);
            *aBuf += (2UL * BL_ETHER_MAC_ADDR_SIZE);

            if ((vlanID >= BL_ETHER_VLANID_MIN)
                    && (vlanID <= BL_ETHER_VLANID_MAX)){
                if (*aLen > BL_VLAN_TAG_SIZE) {
                    *aLen -= BL_VLAN_TAG_SIZE;
                    /* Insert VLAN tag */
                    *((BL_ETHER_EtherType *)(*aBuf)) = BL_Host2BE16(BL_ETHER_ETHERTYPE_VLANTAG);
                    *aBuf += 2UL;
                    *((BL_ETHER_VLANIDType *)(*aBuf)) =
                        BL_Host2BE16((0U << 12UL) | (priority << 13UL)
                                | (vlanID & 0xFFFU));
                    *aBuf += 2UL;
                } else {
                    rc = BL_ETHER_Send(hwIdx, BL_INET_ETH_CHAN_ID, *aBufIdx, 0UL);
                    if (BL_BCM_ERR_OK != rc) {
                        BL_LOG_ERR("%s: Line:0x%x, BL_ETHER_Send:0x%x\n",
                                __func__, __LINE__, rc);
                    }
                    rc = BL_BCM_ERR_NOMEM;
                    goto exit;
                }
            }

            if (*aLen > sizeof(BL_ETHER_EtherType)) {
                *aLen -= sizeof(BL_ETHER_EtherType);
                *((BL_ETHER_EtherType *)(*aBuf)) = BL_Host2BE16(aEtherType);
                *aBuf += sizeof(BL_ETHER_EtherType);
            } else {
                rc = BL_ETHER_Send(hwIdx, BL_INET_ETH_CHAN_ID, *aBufIdx, 0UL);
                if (BL_BCM_ERR_OK != rc) {
                    BL_LOG_ERR("%s: Line:0x%x, BL_ETHER_Send:0x%x\n",
                            __func__, __LINE__, rc);
                }
                rc = BL_BCM_ERR_NOMEM;
                goto exit;
            }
        }
    }
exit:
    return rc;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_NetIfSend

 ARGUMENTS:
             aIntfId      [in] -> L2 Interface Identifier
             aEtherType   [in] -> Ethernet ethtype field in ETHER II packet
             aDestMACAddr [in] -> Destination MAC address
             aBufIdx      [in] -> L2 buffer identifier
             aBuf         [in] -> L2 buffer
             aLen         [in] -> Length of the retrieved L2 buffer in bytes

 DESCRIPTION: Transmit an L2 frame on the given interface

 RETURNS: BL_BCM_ERR_OK             L2 frame enqueued by the interface driver
                                 for transmission
          BL_BCM_ERR_INVAL_PARAMS   aLen is 0U or aBuf is NULL
******************************************************************************/
int32_t BL_INET_NetIfSend(const BL_INET_InterfaceId aIntfId,
                          uint16_t aEtherType,
                          const uint8_t *const aDestMACAddr,
                          const uint32_t aBufIdx,
                          uint8_t* const aBuf,
                          const uint32_t aLen)
{
    int32_t                     rc;
    BL_INET_InterfaceObject*       ifObj = NULL;
    BL_ETHER_VLANIDType            vlanID;
    BL_ETHER_HwIDType              hwIdx;
    uint32_t                    len = aLen;
    uint8_t*                    buf = aBuf;


    rc = BL_INET_GetInterface(aIntfId, &ifObj);
    if (BL_BCM_ERR_OK == rc) {
        if (BL_INET_NIFCTRLIDX_INVALID == ifObj->nifCtrlIdx){
            ifObj->nifCtrlIdx = ifObj->ifCfg->hwIndex
                | ((ifObj->ifCfg->vlanConfig.vlan << 8UL) & 0xFFF00UL);
        }
        if(BL_BCM_ERR_OK == rc) {
            hwIdx = ifObj->ifCfg->hwIndex;
            vlanID = ifObj->ifCfg->vlanConfig.vlan;

            /* If len is zero free-up the packet */
            if (len != 0UL) {
                if (vlanID >= BL_ETHER_VLANID_MIN) {
                    len += 6UL; /* VLAN + EtherType */
                    buf -= 6UL;
                } else {
                    len += 2UL; /* EtherType */
                    buf -= 2UL;
                }
            }
            buf -= (2UL * BL_ETHER_MAC_ADDR_SIZE);
            len += (2UL * BL_ETHER_MAC_ADDR_SIZE);
            BL_BCM_MemCpy(buf, aDestMACAddr, BL_ETHER_MAC_ADDR_SIZE);

            rc = BL_ETHER_Send(hwIdx, BL_INET_ETH_CHAN_ID, aBufIdx, len);
            if (BL_BCM_ERR_OK == rc) {
                /* Track the number of L2 packet transmitted */
                ifObj->ifStats->txL2PktCount++;
            }
        }
    }

    return rc;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_NetIfGetMACAddr

 ARGUMENTS:
           aIntfId  [in]  -> L2 Interface Identifier
           aBuf     [out] -> Buffer start address where the interface
                             MAC address needs to be written to

 DESCRIPTION: Retrieve the MAC address of the given interface

 RETURNS: BL_BCM_ERR_OK             MAC address retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS   aBuf is NULL or aIntfId is invalid
******************************************************************************/
int32_t BL_INET_NetIfGetMACAddr(const BL_INET_InterfaceId aIntfId,
                                uint8_t *const aBuf)
{
    int32_t rc = BL_BCM_ERR_OK;

    rc = BL_ETHER_XcvrGetMacAddr(BL_INET_ETH_XCVR_HW_ID, aBuf);
    return rc;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_NetIfParseHdr

 ARGUMENTS:
             aCtrlIdx [in] -> system ethernet control index
             aVlan   [out] -> Parsed VLAN Id
             aHwIdx  [out] -> Parsed Ethertype Idx

 DESCRIPTION: Parse the received buffer from L2

 RETURNS:
           BL_BCM_ERR_OK             L2 header parsed successfully
           BL_BCM_ERR_INVAL_PARAMS   Buffer too short
******************************************************************************/
int32_t BL_INET_NetIfParseHdr(uint32_t aCtrlIdx,
                              uint16_t* aVlan,
                              uint32_t* aHwIdx)
{
    *aHwIdx = (aCtrlIdx & 0xFUL);
    *aVlan = (aCtrlIdx & 0xFFF00UL) >> 8UL;

    return BL_BCM_ERR_OK;
}

#if !defined(BL_ENABLE_ETH_TEST)
/**
 * Ethernet packet receive indication
 */
/**
    @code{.c}
    call client packet receive interface;
    @endcode
*/
void BL_ETHER_RxInd(BL_ETHER_HwIDType aID,
                    const uint8_t *const aBuf,
                    uint16_t aLen)
{
    BL_ETHER_EtherType etherType;
    uint32_t ctrlIdx;
    uint32_t len = aLen - (2UL * BL_ETHER_MAC_ADDR_SIZE);
    const uint8_t *buf = &aBuf[2UL * BL_ETHER_MAC_ADDR_SIZE];
    uint32_t priority = 0UL;
    BL_ETHER_VLANIDType vlanID = 0U;

    if ((aBuf != NULL) && (len >= BL_ETHER_HDR_SIZE)) {
        etherType = *((BL_ETHER_EtherType *)(buf));
        etherType = BL_BE2Host16(etherType);
        len -= 2UL;
        buf = &buf[2UL];
        /* Check if the packet is VLAN tagged */
        if(BL_ETHER_ETHERTYPE_VLANTAG == etherType) {
            vlanID = *((BL_ETHER_VLANIDType *)(buf));
            vlanID = BL_BE2Host16(vlanID);
            priority = (uint32_t)(vlanID >> 13UL);
            vlanID &= 0xFFFU;
            /* Update the packet etherType skipping the VLAN tag */
            etherType = *((BL_ETHER_EtherType *)(&buf[2UL]));
            etherType = BL_BE2Host16(etherType);
            len -= 4UL;
            buf = &buf[4UL];
        }
        /* Generate controller index */
        ctrlIdx = (aID & 0xFUL) | ((vlanID << 8UL) & 0xFF00UL);
        if (BL_ETHER_ETHERTYPE_IP == etherType) {
            BL_INET_RxIPv4Pkt(ctrlIdx, buf, len, &aBuf[BL_ETHER_MAC_ADDR_SIZE],
                    aBuf, priority);
        } else if (BL_ETHER_ETHERTYPE_ARP == etherType) {
            BL_INET_RxARPPkt(ctrlIdx, buf, len, &aBuf[BL_ETHER_MAC_ADDR_SIZE],
                    aBuf, priority);
        }
    }
}
#endif