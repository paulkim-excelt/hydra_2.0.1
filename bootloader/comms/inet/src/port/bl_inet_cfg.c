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
 File Name: bl_inet_cfg.c
 Description: This file implements the end points and interfaces for INET stack.
******************************************************************************/

#include <stdint.h>
#include <bl_utils.h>
#include <bl_bcm_err.h>
#include <bl_log.h>
#include <bl_ethernet.h>
#include <bl_inet_cfg.h>
#include <bl_inet_port.h>
#include <bl_tftp.h>
#include <bl_command.h>
#include <bcm_comp.h>
#include <bcm_err.h>

#include "bl_log.h"

#define BRCM_DSGN_BL_INET_REPORT_ERROR_PROC           (0x90U) /**< @brief #BL_INET_ReportError */

#define BL_GetModuleLogLevel() BL_LOG_LVL_INFO

/* The structure used by INET OSIL for maintaining */
/* its internal variables and stats                */
typedef struct _BL_INET_COMPIL {
    /** Variable to track the number of timer events */
    uint32_t numTimerEvents;
} BL_INET_COMPIL;

/* The time interval at which ARP cache entries need to be refreshed */
#define BL_INET_ARP_HANDLE_INTERVAL  (1000000UL/BL_INET_ALARM_EVENT_INTERVAL)

static BL_INET_COMPIL BL_INET_OSILHandle = {0UL};

extern BL_INET_SystemInterface **BL_inet_interface_table;
extern const uint16_t BL_intf_table_sz;
extern BL_INET_SystemEndPoint **BL_inet_endpoint_table;
extern const uint16_t BL_endpoint_table_sz;

/***************** Configure  Interfaces *********************/
#define BL_ETHER_HW_ID_0 (0UL)
#define BL_BOARD_IP_ADDR   \
    (BL_INET_CONVERT_IP_ADDRESS((192U), (168U), (BL_BOARD_IPADDR_TAG), (1U)))
#define BL_HOST_IP_ADDR    \
    (BL_INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)))
BL_DEFINE_INET_INTERFACE((BL_EthIf), (BL_EthIfCache), (BL_EthIfCfg), (BL_EthIfStats),
                      (BL_ETHER_HW_ID_0), (0U), (0U),
                      (BL_BOARD_IP_ADDR),
                      (16U),
                      (BL_HOST_IP_ADDR));
static BL_INET_SystemInterface* BL_interfaces[] = {
    &BL_EthIf,
};

BL_INET_SystemInterface **BL_inet_interface_table = BL_interfaces;

const uint16_t BL_intf_table_sz = sizeof(BL_interfaces)/sizeof(BL_INET_SystemInterface*);

/****************** Configure End Points *********************/
/* End point for loopback test where destination and source IP is same */
BL_DEFINE_INET_ENDPOINT((BL_CmdClient), (BL_CmdClientCfg), (BL_CmdClientCtxt),
                     (0x345UL), (0UL), BL_CmdRecvCb,
                     (3UL), (58900U),
                     (BL_HOST_IP_ADDR),
                     (58900U), (BL_INET_L4PROTO_UDP), (BL_INET_DSCP_CS4));

BL_DEFINE_INET_ENDPOINT((BL_TFTPSrvrRecvReq), (BL_TFTPServerCfg), (BL_TFTPServerCtxt),
                     (BL_TFTP_PAYLOAD_RCV_REQ), (0UL), BL_TFTP_RecvRequestCb,
                     (3UL), (BL_TFTP_LISTEN_PORT),
                     (BL_HOST_IP_ADDR),
                     (58901U), (BL_INET_L4PROTO_UDP), (BL_INET_DSCP_CS4));

BL_DEFINE_INET_ENDPOINT((BL_TFTPSrvrRecvFile), (BL_TFTPServerRFCfg), (BL_TFTPServerRFCtxt),
                     (BL_TFTP_PAYLOAD_RCV_FILE), (0UL), BL_TFTP_RecvFileCb,
                     (3UL), (2000U),
                     (BL_HOST_IP_ADDR),
                     (58901U), (BL_INET_L4PROTO_UDP), (BL_INET_DSCP_CS4));

static BL_INET_SystemEndPoint* BL_endPoints[] = {
    &BL_CmdClient,
    &BL_TFTPSrvrRecvReq,
    &BL_TFTPSrvrRecvFile,
};

BL_INET_SystemEndPoint **BL_inet_endpoint_table = BL_endPoints;

const uint16_t BL_endpoint_table_sz = sizeof(BL_endPoints)/sizeof(BL_INET_SystemEndPoint*);


static BL_COMP_NEVER_INLINE void BL_INET_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_NET_ID, aInstanceID, BRCM_DSGN_BL_INET_REPORT_ERROR_PROC, aErr, 4UL, values);
}

/***********************************************************************
 FUNCTION NAME: BL_INETCfg_GetInterfaceForEndPoint

 ARGUMENTS:
             aEndPointId     [in]  -> End point identifier
             aSysIntfObj     [out] -> L2 system Interface Object

 DESCRIPTION: Used to retrieve the L2 interface for an end point

 RETURNS: BL_BCM_ERR_OK           L2 interface successfully retrieved
          BL_BCM_ERR_INVAL_PARAMS aIntfId is NULL or end point not found
***********************************************************************/

static int32_t BL_INETCfg_GetInterfaceForEndPoint(
                            const BL_INET_EndPointId aEndPointId,
                            BL_INET_SystemInterface** const aSysIntfObj)
{
    int32_t retVal = BL_BCM_ERR_OK;

    if ((aEndPointId >= BL_endpoint_table_sz) || (NULL == aSysIntfObj)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        BL_INET_InterfaceId intfID;
        intfID = BL_endPoints[aEndPointId]->intfId;
        BL_interfaces[intfID]->ifObj.ifID = intfID;
        *aSysIntfObj = BL_interfaces[intfID];
        retVal = BL_BCM_ERR_OK;
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INETCfg_GetInterfaceByIndex

 ARGUMENTS:
             aIntfId      [in]  -> L2 Interface Identifier
             aSysIntfObj  [out] -> L2 Interface system object

 DESCRIPTION: Retrieves the L2 system interface for an end point

 RETURNS: BL_BCM_ERR_OK           interface identifier retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfId is NULL or aEndPointId is invalid
******************************************************************************/
static int32_t BL_INETCfg_GetInterfaceByIndex(
                            const BL_INET_InterfaceId aIntfId,
                            BL_INET_SystemInterface** const aSysIntfObj)
{
    int32_t          retVal;

    if (aIntfId >= BL_intf_table_sz) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_OK;
        if (NULL != aSysIntfObj) {
            *aSysIntfObj = BL_interfaces[aIntfId];
        }
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: BL_INETCfg_SearchInterfaceByVlan

 ARGUMENTS:
             aHwId       [in]  -> HW index of the interface
             aVlan       [in]  -> VLAN tag, if the interface is tagged.
                                  Otherwise 0U
             aSysIntfObj [out] -> L2 system Interface Object

 DESCRIPTION: Used to search for the L2 interface identifier from the
              system configuration The key is <aHwId , aVlan> for the
              tagged case and <aHwId> otherwise

 RETURNS: BL_BCM_ERR_OK           L2 interface successfully retrieved
          BL_BCM_ERR_INVAL_PARAMS aSysIntfObj is NULL or end point not found
          BL_BCM_ERR_NOT_FOUND    No such valid interface exists
***********************************************************************/
static int32_t BL_INETCfg_SearchInterfaceByVlan(
                                const uint32_t aHwId,
                                const uint16_t aVlan,
                                BL_INET_SystemInterface** const aSysIntfObj)
{
    int32_t  retVal;
    uint32_t i;

    if (NULL == aSysIntfObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < BL_intf_table_sz; ++i) {
            if(0UL != BL_interfaces[i]->ifObj.ifCfg->vlanConfig.vlan) {
                /* check for both vlan and hwIndex */
                if ((aVlan == BL_interfaces[i]->ifObj.ifCfg->vlanConfig.vlan) &&
                    (aHwId == BL_interfaces[i]->ifObj.ifCfg->hwIndex)) {
                    *aSysIntfObj = BL_interfaces[i];
                    retVal = BL_BCM_ERR_OK;
                    break;
                }
            } else {
                /* check only hwIndex */
                if (aHwId == BL_interfaces[i]->ifObj.ifCfg->hwIndex) {
                    *aSysIntfObj = BL_interfaces[i];
                    retVal = BL_BCM_ERR_OK;
                    break;
                }
            }
        }
    }

    return retVal;
}


/***********************************************************************
 FUNCTION NAME: BL_INETCfg_SearchInterfaceByIPAddr

 ARGUMENTS:
             aHwId       [in]  -> HW index of the interface
             aIPAddress  [in]  -> IP Address
             aSysIntfObj [out] -> L2 system Interface Object

 DESCRIPTION: Used to search for the L2 interface identifier from the
              system configuration The key is <aHwId , Ip addr>

 RETURNS: BL_BCM_ERR_OK           L2 interface successfully retrieved
          BL_BCM_ERR_INVAL_PARAMS aSysIntfObj is NULL
          BL_BCM_ERR_NOT_FOUND    No such valid interface exists
***********************************************************************/
static int32_t BL_INETCfg_SearchInterfaceByIPAddr(
                                const uint32_t aHwId,
                                const BL_INET_IPAddress aIPAddress,
                                BL_INET_SystemInterface** const aSysIntfObj)
{
    int32_t  retVal;
    uint32_t i;

    if (NULL == aSysIntfObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < BL_intf_table_sz; ++i) {
            if((aIPAddress == BL_interfaces[i]->ifObj.ifCfg->staticAddress) &&
               (aHwId == BL_interfaces[i]->ifObj.ifCfg->hwIndex)){
                BL_interfaces[i]->ifObj.ifID = i;
                *aSysIntfObj = BL_interfaces[i];
                retVal = BL_BCM_ERR_OK;
                break;
            }
        }
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: BL_INETCfg_GetEndPointByPayID

 ARGUMENTS:
             aPayloadId      [in]  -> Payload identifier
             aBL_endPointsysObj [out] -> L2 system end point Object

 DESCRIPTION: Used to retrieve the system end point for a client along
              with its configuration and context using
              <Payload ID>

 RETURNS: BL_BCM_ERR_OK           End point successfully retrieved
          BL_BCM_ERR_INVAL_PARAMS aEndPointId is NULL
          BL_BCM_ERR_NOT_FOUND    End point not found
***********************************************************************/
static int32_t BL_INETCfg_GetEndPointByPayID(
                           const uint32_t aPayloadId,
                           BL_INET_SystemEndPoint** const aBL_endPointsysObj)
{
    int32_t  retVal;
    uint32_t i;

    if (NULL == aBL_endPointsysObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < BL_endpoint_table_sz; ++i) {
            if (BL_endPoints[i]->payloadId == aPayloadId) {
                BL_endPoints[i]->endPtObj.endPointID = (BL_INET_EndPointId)i;
                *aBL_endPointsysObj  = BL_endPoints[i];
                retVal = BL_BCM_ERR_OK;
                break;
            }
        }
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: BL_INETCfg_GetEndPointByProtoPort

 ARGUMENTS:
             aDestPort       [in]  -> The port ID for which the end point
                                      is registered
             aL4Protocol     [in]  -> The protocol for which the end point
                                      is registered
             aBL_endPointsysObj [out] -> Configuration for the end point and
                                      context/scratch memory for the end point

 DESCRIPTION: Used to retrieve the system end point for a client along
              with its configuration and context using
              <aDestPort, aL4Protocol>

 RETURNS: BL_BCM_ERR_OK           End point successfully retrieved
          BL_BCM_ERR_INVAL_PARAMS aBL_endPointsysObj is NULL
          BL_BCM_ERR_NOT_FOUND    End point not found
***********************************************************************/
static int32_t BL_INETCfg_GetEndPointByProtoPort(
                           const BL_INET_Port aDestPort,
                           const BL_INET_L4Protocol aL4Protocol,
                           BL_INET_SystemEndPoint** const aBL_endPointsysObj)
{
    int32_t  retVal;
    uint32_t i;

    if (NULL == aBL_endPointsysObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_NOT_FOUND;
        for (i = 0UL; i < BL_endpoint_table_sz; ++i) {
            /* Search for the end point which matches the protocol and local port */
            /* We do not match for the remote IP address and port for server kind */
            /* of application where an end point can receive data from multiple   */
            /* remote clients. The local address is not matched here as it is     */
            /* assumed to have been checked at reception.                         */
            if ((BL_endPoints[i]->endPtObj.cfg->l4Proto == aL4Protocol) &&
                (BL_endPoints[i]->endPtObj.cfg->localPort == aDestPort)) {
                BL_endPoints[i]->endPtObj.endPointID = (BL_INET_EndPointId)i;
                *aBL_endPointsysObj  = BL_endPoints[i];
                retVal = BL_BCM_ERR_OK;
                break;
            }
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INETCfg_GetEndPointByIndex

 ARGUMENTS:
             aEndPtId      [in]  -> End point index
             aSysEndptObj  [out] -> End point system object

 DESCRIPTION: Retrieves the end point object

 RETURNS: BL_BCM_ERR_OK           End point identifier retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aSysEndptObj is NULL or aEndPtId is invalid
******************************************************************************/
int32_t BL_INETCfg_GetEndPointByIndex(
                            const BL_INET_EndPointId aEndPtId,
                            BL_INET_SystemEndPoint** const aSysEndptObj)
{
    int32_t          retVal;

    if ((aEndPtId >= BL_endpoint_table_sz) || (NULL == aSysEndptObj)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_BCM_ERR_OK;
        *aSysEndptObj = BL_endPoints[aEndPtId];
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: BL_INETCfg_GetTotalNumendPoints

 ARGUMENTS: void

 DESCRIPTION: Used to return the number of end points
              registered in system

 RETURNS: uint32_t           Count of the number of end points
***********************************************************************/
uint32_t BL_INETCfg_GetTotalNumendPoints(void)
{
    return BL_endpoint_table_sz;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_Init

 ARGUMENTS: void

 DESCRIPTION: Initializes the INET stack by resolving the ARP
              addresses for all the registered end points

 RETURNS: void
******************************************************************************/
int32_t BL_INET_Init(void)
{
    int32_t  retVal;
    static uint16_t i = 0U;
    BL_INET_EndPointId             endPtId;
    BL_INET_EndPointState          *endPtCtxt = NULL;
    const BL_INET_EndPointConfig   *endPtCfg  = NULL;
    BL_INET_InterfaceObject*       intfObj    = NULL;
    uint8_t                     destMacAddr[6UL];

    i %= BL_endpoint_table_sz;

    for (; i < BL_endpoint_table_sz; ++i) {
        BL_endPoints[i]->endPtObj.endPointID = (BL_INET_EndPointId)i;
        endPtCtxt = BL_endPoints[i]->endPtObj.ctxt;
        endPtCfg = BL_endPoints[i]->endPtObj.cfg;
        endPtId = BL_endPoints[i]->endPtObj.endPointID;
        /* Find the interface on which this end point is created */
        retVal = BL_INET_GetInterfaceForEndPoint(endPtId, &intfObj);
        if (retVal != BL_BCM_ERR_OK) {
            retVal = BL_BCM_ERR_INVAL_PARAMS;
            break;
        } else {
            /* ARP resolution */
            retVal = BL_INET_ARPResolveAddress(intfObj, endPtCfg->remoteAddr,
                    destMacAddr,
                    &endPtCtxt->arpCacheIndex);
            if (BL_BCM_ERR_NOT_FOUND == retVal) {
                retVal = BL_BCM_ERR_EAGAIN;
                break;
            } else if (BL_BCM_ERR_OK != retVal) {
                break;
            }
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_GetInterfaceCount

 ARGUMENTS: void

 DESCRIPTION: Retrieves the count of L2 BL_interfaces

 RETURNS: Count of BL_interfaces
******************************************************************************/
uint32_t BL_INET_GetInterfaceCount(void)
{
    return BL_intf_table_sz;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_GetInterface

 ARGUMENTS:
             aIntfId      [in]  -> L2 Interface Identifier
             aIntfObj     [out] -> L2 Interface Object

 DESCRIPTION: Retrieves the L2 interface for an end point

 RETURNS: BL_BCM_ERR_OK           interface identifier retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfId is NULL or aEndPointId is invalid
******************************************************************************/
int32_t BL_INET_GetInterface(const BL_INET_InterfaceId aIntfId,
                             BL_INET_InterfaceObject** const aIntfObj)
{
    int32_t retVal;

    if (NULL == aIntfObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        BL_INET_ReportError(retVal, 0UL, aIntfId, 0UL, 0UL, __LINE__);
    } else {
        BL_INET_SystemInterface *sysIntfObj = NULL;
        retVal = BL_INETCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BL_BCM_ERR_OK == retVal) {
            *aIntfObj = &sysIntfObj->ifObj;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_GetInterfaceForEndPoint

 ARGUMENTS:
             aEndPointId  [in]  -> End Point Identifier
             aIntfObj     [out] -> L2 Interface Object

 DESCRIPTION: Retrieves the L2 interface for an end point

 RETURNS: BL_BCM_ERR_OK           Interface object retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfObj is NULL or aEndPointId is invalid
******************************************************************************/
int32_t BL_INET_GetInterfaceForEndPoint(const BL_INET_EndPointId aEndPointId,
                                        BL_INET_InterfaceObject** const aIntfObj)
{
    int32_t retVal = BL_BCM_ERR_INVAL_PARAMS;

    if (NULL == aIntfObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        BL_INET_ReportError(retVal, 0UL, (uint32_t)aEndPointId, 0UL, 0UL, __LINE__);
    } else {
        BL_INET_SystemInterface *sysIntfObj = NULL;
        retVal = BL_INETCfg_GetInterfaceForEndPoint(aEndPointId, &sysIntfObj);
        if (BL_BCM_ERR_OK == retVal) {
            *aIntfObj = &sysIntfObj->ifObj;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_GetEndPoint

 ARGUMENTS:
             aCompId       [in]  -> Component Identifier
             aPayloadId    [in]  -> Component Payload Identifier
             aEndPointObj  [out] -> End Point Object with cfg and stats

 DESCRIPTION: Retrieves the end point for a client

 RETURNS: BL_BCM_ERR_OK           end point identifier retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aEndPointId is NULL
          BL_BCM_ERR_NOT_FOUND    no end point corresponding to aClientId was found
******************************************************************************/
int32_t BL_INET_GetEndPoint(const uint32_t aPayloadId,
                            BL_INET_EndPointObject** const aEndPointObj)
{
    int32_t  retVal;

    if (NULL == aEndPointObj) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        BL_INET_SystemEndPoint *sysIntfObj = NULL;
        retVal = BL_INETCfg_GetEndPointByPayID(aPayloadId, &sysIntfObj);
        if (BL_BCM_ERR_OK == retVal) {
            *aEndPointObj = &sysIntfObj->endPtObj;
        }
    }

    if (BL_BCM_ERR_OK != retVal) {
        BL_INET_ReportError(retVal, 0UL, aPayloadId, (uint32_t)aEndPointObj, 0UL, __LINE__);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_UDPReceiveIndication

 ARGUMENTS:
             aSrcAddr     [in] -> Source IP Address
             aSrcPort     [in] -> Source Port
             aDestAddr    [in] -> Destination IP Address
             aDestPort    [in] -> Destination Port
             aL4Protocol  [in] -> Received protocol (UDP/UDP-lite)
             aBuf         [in] -> Buffer containing received UDP packet
             aLen         [in] -> Length of received UDP packet

 DESCRIPTION: Informs the end point owner that the buffer can be freed

 RETURNS: BL_BCM_ERR_OK             Client notified successfully
          BL_BCM_ERR_NOT_FOUND      No client found for the received packet
          BL_BCM_ERR_NOMEM          No space in the receive packet queue
          BL_BCM_ERR_UNKNOWN        Client task coud not be notified

******************************************************************************/
int32_t BL_INET_UDPReceiveIndication(const BL_INET_IPAddress aSrcAddr,
                                     const BL_INET_Port aSrcPort,
                                     const BL_INET_IPAddress aDestAddr,
                                     const BL_INET_Port aDestPort,
                                     const BL_INET_L4Protocol aL4Protocol,
                                     const uint8_t* const aBuf,
                                     const uint16_t aLen)
{
    int32_t retVal;
    BL_INET_SystemEndPoint *sysEndPtObj = NULL;

    retVal = BL_INETCfg_GetEndPointByProtoPort(
                        aDestPort, aL4Protocol, &sysEndPtObj);
    if (BL_BCM_ERR_OK == retVal) {

        sysEndPtObj->endPtObj.ctxt->rxGoodPkts++;
        sysEndPtObj->endPtObj.ctxt->rxBytes += aLen;

        /* Call packet reception callback for client */
        sysEndPtObj->rxCb(aBuf, aLen, aSrcAddr, aSrcPort);

    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_SearchInterfaceByVlan

 ARGUMENTS:
             aHwId        [in]  -> Interface H/W index
             aVlan        [in]  -> VLAN to search for
             aIntfId      [in]  -> Interface identifier corresponding to the
                                   interface which has the VLAN assigned

 DESCRIPTION: Retrieves the interface which has the input VLAN assigned

 RETURNS: BL_BCM_ERR_OK           interface retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfId is NULL
          BL_BCM_ERR_NOT_FOUND    no interface was found
******************************************************************************/
int32_t BL_INET_SearchInterfaceByVlan(const uint32_t aHwId,
                                      const uint16_t aVlan,
                                      BL_INET_InterfaceId* const aIntfId)
{
    int32_t  retVal;
    BL_INET_SystemInterface *sysIntfObj = NULL;

    if (NULL == aIntfId) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BL_INETCfg_SearchInterfaceByVlan(aHwId, aVlan, &sysIntfObj);
        if (BL_BCM_ERR_OK == retVal) {
            *aIntfId = sysIntfObj->ifObj.ifID;
        }
    }

    if (BL_BCM_ERR_OK != retVal) {
        BL_INET_ReportError(retVal, aHwId, (uint32_t)aVlan, (uint32_t)aIntfId, 0UL, __LINE__);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_SearchInterfaceByIPAddr

 ARGUMENTS:
             aHwId        [in]  -> Interface H/W index
             aIPAddress   [in]  -> IP Address to search for
             aIntfId      [in]  -> Interface identifier corresponding to the
                                   interface which has the IP address assigned

 DESCRIPTION: Retrieves the interface which has the input IP address assigned

 RETURNS: BL_BCM_ERR_OK           interface retrieved successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfId is NULL
          BL_BCM_ERR_NOT_FOUND    no interface was found
******************************************************************************/
int32_t BL_INET_SearchInterfaceByIPAddr(const uint32_t aHwId,
                                        const BL_INET_IPAddress aIPAddress,
                                        BL_INET_InterfaceId* const aIntfId)
{
    int32_t  retVal;

    if (NULL == aIntfId) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        BL_INET_SystemInterface *sysIntfObj = NULL;
        retVal = BL_INETCfg_SearchInterfaceByIPAddr(
                                aHwId, aIPAddress, &sysIntfObj);
        if (BL_BCM_ERR_OK == retVal) {
            *aIntfId = sysIntfObj->ifObj.ifID;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_AddARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface to whose ARP
                                   cache the entry needs to be added
             aIPAddress   [in]  -> IP Address of the entry
             aMACAddr     [in]  -> MAC Address of the entry
             aFlags       [in]  -> Flags corresponding to the entry
             aUpdateAddr  [in]  -> Flag to indicate if MAC address needs
                                   to be updated

 DESCRIPTION: Adds an entry to an interface's ARP cache

 RETURNS: BL_BCM_ERR_OK           entry added successfully
          BL_BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BL_BCM_ERR_NOSUPPORT    static entry already exists
          BL_BCM_ERR_NOMEM        cache has no space left to add the entry
******************************************************************************/
int32_t BL_INET_AddARPEntry(const BL_INET_InterfaceId aIntfId,
                            const BL_INET_IPAddress aIPAddress,
                            const uint8_t* const aMACAddr,
                            uint32_t aFlags,
                            const uint8_t aUpdateAddr)
{
    int32_t        retVal = BL_BCM_ERR_NOMEM;
    BL_INET_ARPEntry* cache;
    uint32_t       emptySlot = INET_MAX_ENTRIES_ARPCACHE;
    uint32_t       entryFound= 0UL;
    uint32_t       i;
    uint32_t       val;
    uint32_t lineNo = __LINE__;

    if (aIntfId >= BL_INET_GetInterfaceCount()) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        int32_t ret = BL_BCM_ERR_OK;
        BL_INET_SystemInterface *sysIntfObj = NULL;
        ret = BL_INETCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BL_BCM_ERR_OK != ret) {
            lineNo = __LINE__;
            goto error;
        }
        cache = sysIntfObj->arpCache;

        /* There could be an entry already allocated for this IP address */
        /* So traverse the complete table but keep an eye out for empty  */
        /* slot just in case we do not find the entry                    */
        for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
            entryFound = 0UL;
            retVal = BL_BCM_ERR_NOMEM;
            val = cache[i].exclVal;

            if (1UL == cache[i].valid) {
                /* Occupied slot */
                if (aIPAddress == cache[i].ipAddr) {
                    entryFound = 1UL;
                    switch (cache[i].flags) {
                        case BL_INET_ARP_ENTRY_FLAG_STATIC:
                            retVal = BL_BCM_ERR_NOSUPPORT;
                            break;
                        case BL_INET_ARP_ENTRY_FLAG_WAIT:
                        case BL_INET_ARP_ENTRY_FLAG_RESOLVED:
                        case BL_INET_ARP_ENTRY_FLAG_LEARNT:
                            /* For resolved and learnt entries, we are */
                            /* probably just refreshing them, still it */
                            /* is better to update the address         */
                            if (1U == aUpdateAddr){
                                BL_BCM_MemCpy(cache[i].macAddr, aMACAddr, 6UL);
                            }
                            cache[i].flags  = aFlags;
                            if (aFlags != BL_INET_ARP_ENTRY_FLAG_WAIT){
                                cache[i].timer  = INET_ARP_COMPLETE_ENTRY_LIFETIME;
                            }
                            retVal = BL_BCM_ERR_OK;
                            break;
                        default:
                            retVal = BL_BCM_ERR_INVAL_PARAMS;
                            break;
                    }
                }
            }
            cache[i].exclVal = ++val;

            /* Trigger to stop searching */
            if (1UL == entryFound){
                break;
            }
        }

        /* If an entry is not found in cache, then find an empty slot */
        if(0UL == entryFound) {
            for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
                retVal = BL_BCM_ERR_NOMEM;
                emptySlot = INET_MAX_ENTRIES_ARPCACHE;

                val = cache[i].exclVal;

                if (0UL == cache[i].valid) {
                    emptySlot = i;
                    /* Add entry into ARP cache */
                    cache[emptySlot].valid  = 1UL;
                    cache[emptySlot].ipAddr = aIPAddress;
                    cache[emptySlot].flags  = aFlags;
                    if (BL_INET_ARP_ENTRY_FLAG_WAIT == aFlags) {
                        cache[emptySlot].timer = INET_ARP_INCOMPLETE_ENTRY_LIFETIME;
                    } else {
                        cache[emptySlot].timer = INET_ARP_COMPLETE_ENTRY_LIFETIME;
                        BL_BCM_MemCpy(cache[emptySlot].macAddr, aMACAddr, 6UL);
                    }
                    retVal = BL_BCM_ERR_OK;
                }
                cache[i].exclVal = ++val;

                /* Trigger to stop searching */
                if (emptySlot != INET_MAX_ENTRIES_ARPCACHE){
                    break;
                }
            }
        }
    }

error:
    if (BL_BCM_ERR_OK != retVal) {
        BL_INET_ReportError(retVal, aIntfId, aIPAddress, (uint32_t)aFlags, emptySlot, lineNo);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_FindARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface whose cache needs
                                   to be searched for the entry
             aIPAddress   [in]  -> IP Address of the entry
             aMACAddr     [out] -> MAC Address of the entry to be retrieved
             aFlags       [out] -> Flags corresponding to the entry

 DESCRIPTION: Finds an ARP entry from an interface's ARP cache

 RETURNS: BL_BCM_ERR_OK           entry found
          BL_BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BL_BCM_ERR_NOT_FOUND    entry not found
******************************************************************************/
int32_t BL_INET_FindARPEntry(const BL_INET_InterfaceId aIntfId,
                             const BL_INET_IPAddress  aIPAddress,
                             uint8_t* const aMACAddr,
                             BL_INET_ARPFlag* const aFlags,
                             uint32_t* const arpIndex)
{
    int32_t        retVal = BL_BCM_ERR_NOT_FOUND;
    BL_INET_ARPEntry* cache;
    uint32_t       i;
    uint32_t       val;

    if ((aIntfId >= BL_INET_GetInterfaceCount()) ||
        (INET_MAX_ENTRIES_ARPCACHE <= *arpIndex)) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
        BL_INET_ReportError(retVal, 0UL, aIntfId, aIPAddress, 0UL, __LINE__);
    } else {
        int32_t ret = BL_BCM_ERR_OK;
        BL_INET_SystemInterface *sysIntfObj = NULL;
        ret = BL_INETCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BL_BCM_ERR_OK != ret) {
            goto error;
        }
        cache = sysIntfObj->arpCache;

        /* Check if entry is available at cached index */
        /* Reset value to avoid using old value when strex fails */
        /* and this loop has to run afresh to pick the new value */
        *aFlags = 0UL;
        retVal = BL_BCM_ERR_NOT_FOUND;

        val = cache[*arpIndex].exclVal;

        if ((1UL == cache[*arpIndex].valid) &&
                (aIPAddress == cache[*arpIndex].ipAddr)) {

            *aFlags = cache[*arpIndex].flags;
            if (*aFlags != BL_INET_ARP_ENTRY_FLAG_WAIT) {
                BL_BCM_MemCpy(aMACAddr, cache[*arpIndex].macAddr, 6UL);
            }
            retVal = BL_BCM_ERR_OK;

        }
        cache[*arpIndex].exclVal = ++val;

        /* If entry not found at cached index, then loop through */
        /* the ARP cache and identify it.                        */
        if (BL_BCM_ERR_NOT_FOUND == retVal){
            for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
                /* Reset value to avoid using old value when strex fails */
                /* and this loop has to run afresh to pick the new value */
                *aFlags = 0UL;
                retVal = BL_BCM_ERR_NOT_FOUND;

                val = cache[i].exclVal;

                if ((1UL == cache[i].valid) &&
                        (aIPAddress == cache[i].ipAddr)) {

                    *aFlags = cache[i].flags;
                    if (*aFlags != BL_INET_ARP_ENTRY_FLAG_WAIT) {
                        BL_BCM_MemCpy(aMACAddr, cache[i].macAddr, 6UL);
                    }
                    retVal = BL_BCM_ERR_OK;
                    *arpIndex = i;
                }
                cache[i].exclVal = ++val;

                /* In case an entry is found, return it */
                if (BL_BCM_ERR_OK == retVal) {
                    break;
                }
            }
        }
    }

error:
    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_RemoveARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface whose cache needs
                                   to be searched for the entry
             aIPAddress   [in]  -> IP Address of the entry to be deleted

 DESCRIPTION: Finds an ARP entry from an interface's ARP cache

 RETURNS: BL_BCM_ERR_OK           entry found and deleted
          BL_BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BL_BCM_ERR_NOT_FOUND    entry not found
          BL_BCM_ERR_NOSUPPORT    trying to remove static entry
******************************************************************************/
int32_t BL_INET_RemoveARPEntry(const BL_INET_InterfaceId aIntfId,
                               const BL_INET_IPAddress  aIPAddress)
{
    int32_t        retVal = BL_BCM_ERR_NOT_FOUND;
    BL_INET_ARPEntry* cache;
    uint32_t       i;
    uint32_t       val;
    uint32_t lineNo = __LINE__;

    if (aIntfId >= BL_INET_GetInterfaceCount()) {
        retVal = BL_BCM_ERR_INVAL_PARAMS;
    } else {
        int32_t ret = BL_BCM_ERR_OK;
        BL_INET_SystemInterface *sysIntfObj = NULL;
        ret = BL_INETCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BL_BCM_ERR_OK != ret) {
            lineNo = __LINE__;
            goto error;
        }
        cache = sysIntfObj->arpCache;

        for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
            val = cache[i].exclVal;
            /* Check and flush the cache entry */
            if ((1UL == cache[i].valid) && (aIPAddress == cache[i].ipAddr)) {
                if (cache[i].flags != BL_INET_ARP_ENTRY_FLAG_STATIC) {
                    cache[i].valid = 0UL;
                    cache[i].ipAddr = 0UL;
                    retVal = BL_BCM_ERR_OK;
                } else {
                    retVal = BL_BCM_ERR_NOSUPPORT;
                }
            }
            cache[i].exclVal = ++val;

            if (BL_BCM_ERR_NOT_FOUND != retVal){
                break;
            }
        }
    }
error:
    if (BL_BCM_ERR_OK != retVal) {
        BL_INET_ReportError(retVal, 0UL, aIntfId, aIPAddress, 0UL, lineNo);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: BL_INET_ProcessTimers

 DESCRIPTION: Invoked once every second. Used to timeout and flush ARP entries
              in the BL_interfaces' caches

 RETURNS: void
******************************************************************************/
void BL_INET_ProcessTimers(void)
{
    BL_INET_ARPEntry*        cache;
    uint32_t              idx;
    uint32_t              entry;
    int32_t               retVal = BL_BCM_ERR_OK;
    uint32_t              intfCount = 0UL;
    uint32_t              val;

    /* Increment the timer event count maintained by IL */
    BL_INET_OSILHandle.numTimerEvents++;

    /* ARP cache timer handling is needed once every 1 second */
    /* This handler is called once every 1 second */
    if ((BL_INET_OSILHandle.numTimerEvents % BL_INET_ARP_HANDLE_INTERVAL) == 0UL) {
        intfCount = BL_INET_GetInterfaceCount();

        /* Flush out entries from all the ARP caches */
        for (idx = 0UL; idx < intfCount; ++idx) {
            BL_INET_SystemInterface *sysIntfObj = NULL;

            retVal = BL_INETCfg_GetInterfaceByIndex(idx, &sysIntfObj);
            if (BL_BCM_ERR_OK == retVal) {
                cache = sysIntfObj->arpCache;

                for (entry = 0UL; entry < INET_MAX_ENTRIES_ARPCACHE; ++entry) {
                    val = cache[entry].exclVal;
                    if ((1UL == cache[entry].valid) &&
                            (BL_INET_ARP_ENTRY_FLAG_STATIC != cache[entry].flags)) {

                        if (0UL == cache[entry].timer) {
                            /* Flush the entry */
                            cache[entry].timer  = 0UL;
                            cache[entry].valid  = 0UL;
                            cache[entry].ipAddr = 0UL;
                        } else {
                            /* Decrement life of the entry */
                            cache[entry].timer--;
                            /* If the entry is about to timeout, try to refresh */
                            if (1UL == cache[entry].timer) {
                                BL_INET_ARPSendRequest(idx,
                                        cache[entry].ipAddr,
                                        cache[entry].flags);
                            }
                        }
                    }
                    cache[entry].exclVal = ++val;
                }
            }
        }
    }

    return;
}
