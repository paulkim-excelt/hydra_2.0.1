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

/** @file inet_osil.c
    @brief ERIKA OSIL implementation of Internet Stack

    Operating System Integration Layer for Internet Stack. Contains OS specific
    routines for network packet handling, tasks, events etc.
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <ee.h>
#include <cortex.h>
#include <atomic.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <msg_queue.h>
#include <bcm_utils.h>
#include <system.h>
#include <ethernet.h>
#include <nif.h>
#include <inet_cfg.h>

#include "eecfg.h"
#include "inet_osil.h"
#include <inet_system.h>
#include <osil/bcm_osil.h>

#ifdef ULOG
#include "ulog.h"
#define GetModuleLogLevel() ULOG_LVL_ERROR
#else
#define ULOG_ERR(...)
#endif

/* The structure used by INET OSIL for maintaining */
/* its internal variables and stats                */
typedef struct _INET_COMPIL {
    /** Variable to track the number of timer events from COMMS Task */
    uint32_t numTimerEvents;

    /** Counter to track the number of Tx confirmations received */
    uint32_t inetTxConfCount;

    /** Counter to track the number of reassembly buffer timeouts */
    uint32_t inetBufTimeout;
} INET_COMPIL;

/* The time interval at which ARP cache entries need to be refreshed */
#define INET_ARP_HANDLE_INTERVAL  (1000000UL/INET_ALARM_EVENT_INTERVAL)

static INET_COMPIL COMP_SECTION(".data.inet")
INET_OSILHandle = { 0UL, 0UL, 0UL};

void INET_Timer_AlarmCb(void);

/*** Static Functions ***/
static void INET_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                             uint32_t aVal1, uint32_t aVal2, uint32_t aVal3);

void INET_Timer_AlarmCb(void)
{
    SetEvent(CommsTask, SystemEvent3);
}



/******************************************************************************
 FUNCTION NAME: INET_Init

 ARGUMENTS: void

 DESCRIPTION: Initializes the INET stack by resolving the ARP
              addresses for all the registered end points

 RETURNS: void
******************************************************************************/
void INET_Init(void)
{
    /* Will be implemented after ethernet driver adds callback to */
    /* notify link up notification                                */
    return;
}

/******************************************************************************
 FUNCTION NAME: INET_GetInterfaceCount

 ARGUMENTS: aIntfCount   [out] -> L2 Interface Count

 DESCRIPTION: Retrieves the count of L2 interfaces

 RETURNS: Void
******************************************************************************/
void INET_GetInterfaceCount(uint32_t* const aIntfCount)
{
    *aIntfCount = SysCfg_GetTotalNumInterfaces();
    return;
}

/******************************************************************************
 FUNCTION NAME: INET_GetInterface

 ARGUMENTS:
             aIntfId      [in]  -> L2 Interface Identifier
             aIntfObj     [out] -> L2 Interface Object

 DESCRIPTION: Retrieves the L2 interface for an end point

 RETURNS: BCM_ERR_OK           interface identifier retrieved successfully
          BCM_ERR_INVAL_PARAMS aIntfId is NULL or aEndPointId is invalid
******************************************************************************/
int32_t INET_GetInterface(const INET_InterfaceId aIntfId,
                          INET_InterfaceObject** const aIntfObj)
{
    int32_t retVal;

    if (NULL == aIntfObj) {
        retVal = BCM_ERR_INVAL_PARAMS;
        INET_ReportError(BRCM_ARCH_INETCOMP_GETINTF_PROC, retVal, aIntfId,
                         0UL, 0UL, 0UL);
    } else {
        INET_SystemInterface *sysIntfObj = NULL;
        retVal = SysCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BCM_ERR_OK == retVal) {
            *aIntfObj = &sysIntfObj->ifObj;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_GetInterfaceForEndPoint

 ARGUMENTS:
             aEndPointId  [in]  -> End Point Identifier
             aIntfObj     [out] -> L2 Interface Object

 DESCRIPTION: Retrieves the L2 interface for an end point

 RETURNS: BCM_ERR_OK           Interface object retrieved successfully
          BCM_ERR_INVAL_PARAMS aIntfObj is NULL or aEndPointId is invalid
******************************************************************************/
int32_t INET_GetInterfaceForEndPoint(const INET_EndPointId aEndPointId,
                                     INET_InterfaceObject** const aIntfObj)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL == aIntfObj) {
        retVal = BCM_ERR_INVAL_PARAMS;
        INET_ReportError(BRCM_ARCH_INETCOMP_GETINTFFORENDPT_PROC, retVal,
                        (uint32_t)aEndPointId, 0UL, 0UL, 0UL);
    } else {
        INET_SystemInterface *sysIntfObj = NULL;
        retVal = SysCfg_GetInterfaceForEndPoint(aEndPointId, &sysIntfObj);
        if (BCM_ERR_OK == retVal) {
            *aIntfObj = &sysIntfObj->ifObj;
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_GetEndPoint

 ARGUMENTS:
             aCompId       [in]  -> Component Identifier
             aPayloadId    [in]  -> Component Payload Identifier
             aEndPtIndex   [out] -> Index of the end point in the array
             aEndPointObj  [out] -> End Point Object with cfg and stats

 DESCRIPTION: Retrieves the end point for a client

 RETURNS: BCM_ERR_OK           end point identifier retrieved successfully
          BCM_ERR_INVAL_PARAMS aEndPointId is NULL
          BCM_ERR_NOT_FOUND    no end point corresponding to aClientId was found
******************************************************************************/
int32_t INET_GetEndPoint(const uint64_t aCompId,
                         const uint32_t aPayloadId,
                         INET_EndPointId* const aEndPtIndex,
                         const INET_EndPointObject** const aEndPointObj)
{
    int32_t  retVal;

    if ((NULL == aEndPointObj) || (NULL == aEndPtIndex)){
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        const INET_SystemEndPoint *sysIntfObj = NULL;
        retVal = SysCfg_GetEndPointByCompPayID(
                                aCompId, aPayloadId, aEndPtIndex, &sysIntfObj);
        if (BCM_ERR_OK == retVal) {
            *aEndPointObj = &sysIntfObj->endPtObj;
        }
    }

    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_GETENDPT_PROC, retVal,
                        (uint32_t)aCompId, aPayloadId, (uint32_t)aEndPointObj,
                        0UL);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_UDPReceiveIndication

 ARGUMENTS:
             aSrcAddr     [in] -> Source IP Address
             aSrcPort     [in] -> Source Port
             aDestAddr    [in] -> Destination IP Address
             aDestPort    [in] -> Destination Port
             aL4Protocol  [in] -> Received protocol (UDP/UDP-lite)
             aBuf         [in] -> Buffer containing received UDP packet
             aLen         [in] -> Length of received UDP packet

 DESCRIPTION: Informs the end point owner that the buffer can be freed

 RETURNS: BCM_ERR_OK             Client notified successfully
          BCM_ERR_NOT_FOUND      No client found for the received packet
          BCM_ERR_NOMEM          No space in the receive packet queue
          BCM_ERR_UNKNOWN        Client task coud not be notified

******************************************************************************/
int32_t INET_UDPReceiveIndication(const INET_IPAddress aSrcAddr,
                                  const INET_Port aSrcPort,
                                  const INET_IPAddress aDestAddr,
                                  const INET_Port aDestPort,
                                  const INET_L4Protocol aL4Protocol,
                                  const uint8_t* const aBuf,
                                  const uint16_t aLen)
{
    int32_t retVal;
    int32_t ret;
    const INET_SystemEndPoint *sysEndPtObj = NULL;
    INET_ReassemblyBuffer *reassbuffer = NULL;
    INET_EndPointId endPtIndex;

    retVal = SysCfg_GetEndPointByProtoPort(
                        aDestPort, aL4Protocol, &endPtIndex, &sysEndPtObj);
    if (BCM_ERR_OK == retVal) {
        INET_RxPkt pkt = {.buffer = aBuf,
                          .length = aLen,
                          .srcIPAddr = aSrcAddr,
                          .srcPort = aSrcPort};

        if (aLen > INET_MAX_REASS_BUF_SIZE) {
            /* The length parameter here is the length of the complete */
            /* payload. However there might be a limited reassembly    */
            /* buffer which is not sufficient to contain the complete  */
            /* payload in which case the payload gets truncated        */
            pkt.length = INET_MAX_REASS_BUF_SIZE;
            /* Update statistics */
            sysEndPtObj->endPtObj.ctxt->rxTruncPkts++;
            /* Report error */
            INET_ReportError(BRCM_ARCH_INETCOMP_UDPRECVIND_PROC, BCM_ERR_OK,
                            (uint32_t)aLen, aSrcAddr, (uint32_t)aSrcPort,
                            (uint32_t)aL4Protocol);
        }

        /* Store the buffer and length in the receive queue */
        retVal = CIRCQ_Push(sysEndPtObj->endPtObj.ctxt->rxPktQ,
                           (const char*) &pkt, 1UL);
        if (BCM_ERR_OK == retVal){
            retVal = SysCfg_GetReassemblyBufferbyPointer(aBuf, &reassbuffer);
            if ((BCM_ERR_OK == retVal) &&
                (NULL != reassbuffer)){
                /* Update the state and end point ID of the reassembly buffer */
                reassbuffer->clientEndPtID = endPtIndex;
                /* The client read timer is assigned here. It may happen that    */
                /* as soon as this value is assigned, a timer alarm may fire     */
                /* which will decrement the counter. In order to maintain the    */
                /* the minimum value of timeout time INET_MAX_CLIENT_READ_TIMEOUT*/
                /* we are adding the alarm interval to the timeout value         */
                reassbuffer->timer = INET_ALARM_EVENT_INTERVAL +
                                     INET_MAX_CLIENT_READ_TIMEOUT;
                reassbuffer->bufState = INET_REASSEMBLYBUF_QUEUED;
            }
        }

        /* Inform the receive task before trying to push into the queue */
        ret = BCM_SetEvent(sysEndPtObj->rxTaskId, sysEndPtObj->rxTaskEventId);
        if (BCM_ERR_OK != ret) {
            retVal = BCM_ERR_UNKNOWN;
        }

        /* Update statistics */
        if (BCM_ERR_OK == retVal) {
            sysEndPtObj->endPtObj.ctxt->rxGoodPkts++;
            sysEndPtObj->endPtObj.ctxt->rxBytes += pkt.length;
        } else {
            sysEndPtObj->endPtObj.ctxt->rxDropPkts++;
            INET_ReportError(BRCM_ARCH_INETCOMP_UDPRECVIND_PROC, retVal,
                            (uint32_t)ret, (uint32_t)sysEndPtObj->rxTaskId,
                            (uint32_t)endPtIndex,
                            (uint32_t)aDestPort);
        }
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_SearchInterface

 ARGUMENTS:
             aHwId        [in]  -> Interface H/W index
             aIPAddress   [in]  -> IP Address to search for
             aVlan        [in]  -> VLAN tag value
             aIntfId      [in]  -> Interface identifier corresponding to the
                                   interface which has the IP address assigned

 DESCRIPTION: Retrieves the interface which has the input IP address assigned

 RETURNS: BCM_ERR_OK           interface retrieved successfully
          BCM_ERR_INVAL_PARAMS aIntfId is NULL
          BCM_ERR_NOT_FOUND    no interface was found
******************************************************************************/
int32_t INET_SearchInterface(const uint32_t aHwId,
                             const INET_IPAddress aIPAddress,
                             const uint16_t aVlan,
                             INET_InterfaceId* const aIntfId)
{
    int32_t  retVal;

    if (NULL == aIntfId) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        INET_SystemInterface *sysIntfObj = NULL;
        retVal = SysCfg_SearchInterface(
                                aHwId, aIPAddress, aVlan, &sysIntfObj);
        if (BCM_ERR_OK == retVal) {
            *aIntfId = sysIntfObj->ifObj.ifID;
        }
    }

    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_SRCHINTFBYPARAMS_PROC, retVal,
                         aHwId, aIPAddress, (uint32_t)aVlan, (uint32_t)aIntfId);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_AddARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface to whose ARP
                                   cache the entry needs to be added
             aIPAddress   [in]  -> IP Address of the entry
             aMACAddr     [in]  -> MAC Address of the entry
             aFlags       [in]  -> Flags corresponding to the entry
             aUpdateAddr  [in]  -> Flag to indicate if MAC address needs
                                   to be updated

 DESCRIPTION: Adds an entry to an interface's ARP cache

 RETURNS: BCM_ERR_OK           entry added successfully
          BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BCM_ERR_NOPERM       static entry already exists
          BCM_ERR_NOMEM        cache has no space left to add the entry
******************************************************************************/
int32_t INET_AddARPEntry(const INET_InterfaceId aIntfId,
                         const INET_IPAddress  aIPAddress,
                         const uint8_t* const aMACAddr,
                         const INET_ARPFlag aFlags,
                         const uint8_t aUpdateAddr)
{
    int32_t        retVal = BCM_ERR_NOMEM;
    INET_ARPEntry* cache;
    uint32_t       emptySlot = INET_MAX_ENTRIES_ARPCACHE;
    uint32_t       entryFound= 0UL;
    uint32_t       i;
    uint32_t       notExclusive;
    uint32_t       val;
    INET_ARPFlag   cacheFlag;

    if (aIntfId >= SysCfg_GetTotalNumInterfaces()) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        int32_t ret = BCM_ERR_OK;
        INET_SystemInterface *sysIntfObj = NULL;
        ret = SysCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BCM_ERR_OK != ret) {
            goto error;
        }
        cache = sysIntfObj->arpCache;

        /* There could be an entry already allocated for this IP address */
        /* So traverse the complete table but keep an eye out for empty  */
        /* slot just in case we do not find the entry                    */
        for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
            do {
                notExclusive  = 0UL;
                entryFound = 0UL;
                retVal = BCM_ERR_NOMEM;
                val = CORTEX_LdrEx(&cache[i].exclVal);

                if (1UL == cache[i].valid) {
                    /* Occupied slot */
                    if (aIPAddress == cache[i].ipAddr) {
                        entryFound = 1UL;
                        switch (cache[i].flags) {
                            case INET_ARP_ENTRY_FLAG_STATIC:
                                retVal = BCM_ERR_NOPERM;
                                break;
                            case INET_ARP_ENTRY_FLAG_WAIT:
                            case INET_ARP_ENTRY_FLAG_RESOLVED:
                            case INET_ARP_ENTRY_FLAG_LEARNT:
                                /* For resolved and learnt entries, we are */
                                /* probably just refreshing them, still it */
                                /* is better to update the address         */
                                if (1U == aUpdateAddr){
                                    BCM_MemCpy(cache[i].macAddr, aMACAddr, 6UL);
                                }

                                /* Copy the flag value at index. This is done as we want */
                                /* the cache location to be updated before the event is  */
                                /* raised to the INET clients                            */
                                cacheFlag = cache[i].flags;
                                cache[i].flags  = aFlags;

                                if (aFlags != INET_ARP_ENTRY_FLAG_WAIT){
                                    cache[i].timer  = INET_ARP_COMPLETE_ENTRY_LIFETIME;
                                } else {
                                    cache[i].timer = INET_ARP_RETRY_PERIOD;
                                }

                                /* For a transition from WAIT to LEARNT/RESOLVED state */
                                if ((INET_ARP_ENTRY_FLAG_WAIT == cacheFlag) &&
                                    ((INET_ARP_ENTRY_FLAG_LEARNT == aFlags) ||
                                     (INET_ARP_ENTRY_FLAG_RESOLVED == aFlags))){
                                    uint16_t idx;
                                    uint16_t endPointCnt = SysCfg_GetTotalNumEndPoints();

                                    /* Whenever an ARP entry gets resolved, we check the list of end points */
                                    /* awaiting ARP resolution. If the resolved IP address matches with the */
                                    /* end point remote IP address, or if it matches the interface gateway  */
                                    /* IP address (in case when remote IP is on a different subnet) we      */
                                    /* notify the client. The client can then call the process API to       */
                                    /* continue data transmission.                                          */

                                    for (idx = 0U; idx < endPointCnt; idx++) {
                                        const INET_SystemEndPoint *sysEndPtObj = NULL;

                                        retVal = SysCfg_GetEndPointByIndex((uint16_t)idx, &sysEndPtObj);
                                        if (BCM_ERR_OK == retVal) {
                                            INET_SystemInterface *sysIntfCtxt = NULL;

                                            retVal = SysCfg_GetInterfaceForEndPoint(idx, &sysIntfCtxt);
                                            if (BCM_ERR_OK == retVal) {
                                                INET_IPAddress remoteIP = sysEndPtObj->endPtObj.cfg->remoteAddr;
                                                INET_IPAddress intfIP = sysIntfCtxt->ifObj.ifCfg->staticAddress;
                                                INET_IPAddress gatewayIP = sysIntfCtxt->ifObj.ifCfg->defaultRoute;
                                                INET_Netmask netMask = sysIntfCtxt->ifObj.ifCfg->netmask;
                                                uint32_t mask = ~((1UL << (32U - netMask)) - 1UL);

                                                /* Conditions on which an end point is notified                    */
                                                /* 1. When the IP address being added to ARP cache is same as the  */
                                                /*    remote IP of the end point                                   */
                                                /* 2. If the remote IP address of the end point is on a different  */
                                                /*    subnet and the current IP address being added matches the    */
                                                /*    interface gateway IP address of the end point                */
                                                if((aIPAddress == remoteIP) ||
                                                   (((remoteIP & mask) != (intfIP & mask)) &&
                                                     (gatewayIP == aIPAddress))) {
                                                    if ((sysEndPtObj->endPtObj.ctxt->sentBytes) <
                                                            (sysEndPtObj->endPtObj.ctxt->txLength)){
                                                        /* Update the ARP index for faster lookup */
                                                        sysEndPtObj->endPtObj.ctxt->arpCacheIndex = i;
                                                        (void)BCM_SetEvent(sysEndPtObj->txTaskId, sysEndPtObj->txTaskEventId);
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                retVal = BCM_ERR_OK;
                                break;
                            default:
                                retVal = BCM_ERR_INVAL_PARAMS;
                                break;
                        }
                    }
                }
                notExclusive = CORTEX_StrEx(&cache[i].exclVal, ++val);
            } while (1UL == notExclusive);

            /* Trigger to stop searching */
            if (1UL == entryFound){
                break;
            }
        }

        /* If an entry is not found in cache, then find an empty slot */
        if(0UL == entryFound) {
            for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
                do {
                    notExclusive  = 0UL;
                    retVal = BCM_ERR_NOMEM;
                    emptySlot = INET_MAX_ENTRIES_ARPCACHE;

                    val = CORTEX_LdrEx(&cache[i].exclVal);

                    if (0UL == cache[i].valid) {
                        emptySlot = i;
                        /* Add entry into ARP cache */
                        cache[emptySlot].valid  = 1UL;
                        cache[emptySlot].ipAddr = aIPAddress;
                        cache[emptySlot].flags  = aFlags;
                        /* Update the interface which is adding this entry */
                        cache[emptySlot].ifID   = aIntfId;
                        if (INET_ARP_ENTRY_FLAG_WAIT == aFlags) {
                            cache[emptySlot].timer = INET_ARP_RETRY_PERIOD;
                        } else {
                            cache[emptySlot].timer = INET_ARP_COMPLETE_ENTRY_LIFETIME;
                            BCM_MemCpy(cache[emptySlot].macAddr, aMACAddr, 6UL);
                        }
                        retVal = BCM_ERR_OK;
                    }
                    notExclusive = CORTEX_StrEx(&cache[i].exclVal, ++val);
                }while (1UL == notExclusive);

                /* Trigger to stop searching */
                if (emptySlot != INET_MAX_ENTRIES_ARPCACHE){
                    break;
                }
            }
        }
    }

error:
    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_ADDARPENTRY_PROC, retVal, aIntfId,
                         aIPAddress, (uint32_t)aFlags, emptySlot);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_FindARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface whose cache needs
                                   to be searched for the entry
             aIPAddress   [in]  -> IP Address of the entry
             aMACAddr     [out] -> MAC Address of the entry to be retrieved
             aFlags       [out] -> Flags corresponding to the entry

 DESCRIPTION: Finds an ARP entry from an interface's ARP cache

 RETURNS: BCM_ERR_OK           entry found
          BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BCM_ERR_NOT_FOUND    entry not found
******************************************************************************/
int32_t INET_FindARPEntry(const INET_InterfaceId aIntfId,
                          const INET_IPAddress  aIPAddress,
                          uint8_t* const aMACAddr,
                          INET_ARPFlag* const aFlags,
                          uint32_t* const arpIndex)
{
    int32_t        retVal = BCM_ERR_NOT_FOUND;
    INET_ARPEntry* cache;
    uint32_t       i;
    uint32_t       notExclusive;
    uint32_t       val;

    if ((aIntfId >= SysCfg_GetTotalNumInterfaces()) ||
        (INET_MAX_ENTRIES_ARPCACHE <= *arpIndex)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        INET_ReportError(BRCM_ARCH_INETCOMP_FINDARPENTRY_PROC, retVal, aIntfId,
                         aIPAddress, 0UL, 0UL);
    } else {
        int32_t ret = BCM_ERR_OK;
        INET_SystemInterface *sysIntfObj = NULL;
        ret = SysCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BCM_ERR_OK != ret) {
            goto error;
        }
        cache = sysIntfObj->arpCache;

        /* Check if entry is available at cached index */
        do {
            notExclusive = 0UL;
            /* Reset value to avoid using old value when strex fails */
            /* and this loop has to run afresh to pick the new value */
            *aFlags = 0UL;
            retVal = BCM_ERR_NOT_FOUND;

            val = CORTEX_LdrEx(&cache[*arpIndex].exclVal);

            if ((1UL == cache[*arpIndex].valid) &&
                (aIPAddress == cache[*arpIndex].ipAddr)) {

                *aFlags = cache[*arpIndex].flags;
                if (*aFlags != INET_ARP_ENTRY_FLAG_WAIT) {
                    BCM_MemCpy(aMACAddr, cache[*arpIndex].macAddr, 6UL);
                }
                retVal = BCM_ERR_OK;

            }
            notExclusive = CORTEX_StrEx(&cache[*arpIndex].exclVal, ++val);
        } while (1UL == notExclusive);

        /* If entry not found at cached index, then loop through */
        /* the ARP cache and identify it.                        */
        if (BCM_ERR_NOT_FOUND == retVal){
            for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
                do {
                    notExclusive = 0UL;
                    /* Reset value to avoid using old value when strex fails */
                    /* and this loop has to run afresh to pick the new value */
                    *aFlags = 0UL;
                    retVal = BCM_ERR_NOT_FOUND;

                    val = CORTEX_LdrEx(&cache[i].exclVal);

                    if ((1UL == cache[i].valid) &&
                        (aIPAddress == cache[i].ipAddr)) {

                        *aFlags = cache[i].flags;
                        if (*aFlags != INET_ARP_ENTRY_FLAG_WAIT) {
                            BCM_MemCpy(aMACAddr, cache[i].macAddr, 6UL);
                        }
                        retVal = BCM_ERR_OK;
                        *arpIndex = i;
                    }
                    notExclusive = CORTEX_StrEx(&cache[i].exclVal, ++val);
                } while (1UL == notExclusive);
                /* In case an entry is found, return it */
                if (BCM_ERR_OK == retVal) {
                    break;
                }
            }
        }
    }

error:
    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_RemoveARPEntry

 ARGUMENTS:
             aIntfId      [in]  -> Identifier of the interface whose cache needs
                                   to be searched for the entry
             aIPAddress   [in]  -> IP Address of the entry to be deleted

 DESCRIPTION: Finds an ARP entry from an interface's ARP cache

 RETURNS: BCM_ERR_OK           entry found and deleted
          BCM_ERR_INVAL_PARAMS aIntfId is invalid
          BCM_ERR_NOT_FOUND    entry not found
          BCM_ERR_NOSUPPORT    trying to remove static entry
******************************************************************************/
int32_t INET_RemoveARPEntry(const INET_InterfaceId aIntfId,
                            const INET_IPAddress  aIPAddress)
{
    int32_t        retVal = BCM_ERR_NOT_FOUND;
    INET_ARPEntry* cache;
    uint32_t       i;
    uint32_t       notExclusive;
    uint32_t       val;

    if (aIntfId >= SysCfg_GetTotalNumInterfaces()) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        int32_t ret = BCM_ERR_OK;
        INET_SystemInterface *sysIntfObj = NULL;
        ret = SysCfg_GetInterfaceByIndex(aIntfId, &sysIntfObj);
        if (BCM_ERR_OK != ret) {
            goto error;
        }
        cache = sysIntfObj->arpCache;

        for (i = 0UL; i < INET_MAX_ENTRIES_ARPCACHE; ++i) {
            do {
                notExclusive = 0UL;
                val = CORTEX_LdrEx(&cache[i].exclVal);
                /* Check and flush the cache entry */
                if ((1UL == cache[i].valid) && (aIPAddress == cache[i].ipAddr)) {
                    if (cache[i].flags != INET_ARP_ENTRY_FLAG_STATIC) {
                        cache[i].valid = 0UL;
                        cache[i].ipAddr = 0UL;
                        cache[i].ifID = INET_INTERFACEID_INVALID;
                        retVal = BCM_ERR_OK;
                    } else {
                        retVal = BCM_ERR_NOSUPPORT;
                    }
                }
                notExclusive = CORTEX_StrEx(&cache[i].exclVal, ++val);
            } while (1UL == notExclusive);

            if (BCM_ERR_NOT_FOUND != retVal){
                break;
            }
        }
    }
error:
    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_REMOVEARPENTRY_PROC, retVal,
                         aIntfId, aIPAddress, 0UL, 0UL);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_GetReassemblyBuffer

 ARGUMENTS:
             aSrcAddr     [in]  -> Source IP Address in IP header
             aIdent       [in]  -> 16 bit identifier received in IP header
             aBuf         [out] -> Reassembly buffer
             aLen         [out] -> Length of the reassembly buffer
             aCurLen      [out] -> Bytes reassembled so far in the buffer
             aTotalLen    [out] -> Total length of the IP datagram being
                                   reassembled
             aFragQ       [out] -> Pointer to the fragment queue

 DESCRIPTION: Gets the reassembly buffer for a connection from the free pool

 RETURNS: BCM_ERR_OK    reassembly buffer found/allocated
          BCM_ERR_NOMEM no buffer could be allocated
******************************************************************************/
int32_t INET_GetReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                 const uint16_t aIdent,
                                 uint8_t** aBuf,
                                 uint16_t* aLen,
                                 uint16_t* const aCurLen,
                                 uint16_t* const aTotalLen,
                                 const CIRCQ_Type** aFragQ)
{
    int32_t  retVal;
    INET_ReassemblyBuffer *reassBuffer = NULL;

    retVal = SysCfg_GetReassemblyBuffer(
                            aSrcAddr, aIdent, &reassBuffer);
    if (BCM_ERR_OK == retVal) {
        *aBuf      = reassBuffer->buffer;
        *aLen      = INET_MAX_REASS_BUF_SIZE;
        *aFragQ    = reassBuffer->fragmentQ;
        *aCurLen   = reassBuffer->curLen;
        *aTotalLen = reassBuffer->totalLen;
    }

    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_GETREASSBUF_PROC, retVal, aSrcAddr,
                        (uint32_t) aIdent,
                         SysCfg_GetTotalNumReassemblyBuffers(), 0UL);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_UpdateReassemblyBuffer

 ARGUMENTS:
             aSrcAddr     [in] -> Source IP Address in IP header
             aIdent       [in] -> 16 bit identifier received in IP header
             aCurLen      [in] -> Bytes reassembled so far in the buffer
             aTotalLen    [in] -> Total length of the IP datagram being
                                  reassembled

 DESCRIPTION: Updates/frees a previously allocated reassembly buffer

 RETURNS: BCM_ERR_OK        reassembly buffer updated
          BCM_ERR_NOT_FOUND the buffer could not be found
******************************************************************************/
int32_t INET_UpdateReassemblyBuffer(const INET_IPAddress aSrcAddr,
                                    const uint16_t aIdent,
                                    const uint16_t aCurLen,
                                    const uint16_t aTotalLen)
{
    int32_t  retVal;
    INET_ReassemblyBuffer *reassBuffer = NULL;

    retVal = SysCfg_GetReassemblyBuffer(
                            aSrcAddr, aIdent, &reassBuffer);
    if (BCM_ERR_OK == retVal) {
        if ((INET_REASSEMBLYBUF_ALLOCATED == reassBuffer->bufState) &&
            (aSrcAddr == reassBuffer->ipAddr) &&
            (aIdent == reassBuffer->ident)) {

            reassBuffer->curLen = aCurLen;
            if (aTotalLen != 0U) {
                reassBuffer->totalLen = aTotalLen;
            }

        } else {
            retVal = BCM_ERR_NOT_FOUND;
        }
    }

    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_UPDTREASSBUF_PROC, retVal, aSrcAddr,
                        (uint32_t)aIdent, (uint32_t)aCurLen,
                        (uint32_t)aTotalLen);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_FreeReassemblyBuffer

 ARGUMENTS:
             aBuf [in] -> Reassembly buffer to be freed

 DESCRIPTION: Frees a previously allocated reassembly buffer

 RETURNS: BCM_ERR_OK        reassembly buffer freed
          BCM_ERR_NOT_FOUND the buffer could not be found or was unallocated
******************************************************************************/

int32_t INET_FreeReassemblyBuffer(const uint8_t* const aBuf)
{
    int32_t  retVal;
    INET_ReassemblyBuffer *reassbuffer = NULL;

    retVal = SysCfg_GetReassemblyBufferbyPointer(aBuf, &reassbuffer);
    if ((BCM_ERR_OK == retVal) && (NULL != reassbuffer)) {
        reassbuffer->ident           = 0U;
        reassbuffer->ipAddr          = 0UL;
        reassbuffer->curLen          = 0U;
        reassbuffer->totalLen        = 0U;
        reassbuffer->timer           = 0U;
        reassbuffer->clientEndPtID   = INET_ENDPOINTID_INVALID;
        /* Whenever INET gets a Rx packet, it is the context of      */
        /* COMMS Task. The allocation is based on the value of       */
        /* 'bufState' flag of the reassembly buffer. This flag       */
        /* is reset here only after all the other members are reset. */
        /* Earlier, we were setting the 'bufState' flag to FREE first*/
        /* and if a context switch occured, COMMS Task would allocate*/
        /* this same buffer. Later, in this function, the other      */
        /* variables are reset. This race condition is avoided by    */
        /* setting the 'bufState' flag to zero at the end.           */
        CORTEX_DSB();
        reassbuffer->bufState = INET_REASSEMBLYBUF_FREE;
    } else {
        retVal = BCM_ERR_NOT_FOUND;
    }

    if (BCM_ERR_OK != retVal) {
        INET_ReportError(BRCM_ARCH_INETCOMP_FREEREASSBUF_PROC, retVal,
                         (uint32_t)aBuf, (uint32_t)reassbuffer, 0UL, 0UL);
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_ClientAckReassBuf

 ARGUMENTS:
             aBuf [in]    -> Reassembly data buffer pointer
             aEndPtID [in]-> End point ID of the client

 DESCRIPTION: Atomically sets the state of the reassembly buffer to the
              new value specified, only when its state matches the oldSate

 RETURNS: BCM_ERR_OK           State successfully set
          BCM_ERR_INVAL_STATE  State set failure as buffer state was not
                               INET_REASSEMBLYBUF_QUEUED
          BCM_ERR_INVAL_PARAMS The reassembly buffer pointer is invalid or
                               the buffer has timed out and not intended
                               for current client
******************************************************************************/
int32_t INET_ClientAckReassBuf(const uint8_t* const aBuf,
                               INET_EndPointId const aEndPtID)
{
    int32_t  retVal = BCM_ERR_OK;
    INET_ReassemblyBuffer *reassbuffer = NULL;
    INET_ReassemblyBufferState retState;

    retVal = SysCfg_GetReassemblyBufferbyPointer(aBuf, &reassbuffer);
    if ((BCM_ERR_OK == retVal) &&
        (NULL != reassbuffer)  &&
        /* Check if this buffer is for the mentioned client */
        (aEndPtID == reassbuffer->clientEndPtID) &&
        /* Check if buffer is in QUEUED state */
        (INET_REASSEMBLYBUF_QUEUED == reassbuffer->bufState)) {

        retState = ATOMIC_CheckUpdate(&reassbuffer->bufState,
                                     INET_REASSEMBLYBUF_QUEUED,
                                     INET_REASSEMBLYBUF_RCVD);
        /* A QUEUED buffer can either be  RCVD by the client task or */
        /* be timed out by the COMMS task. In case when the client   */
        /* is updating the state to RCVD, and it gets interrupted by */
        /* the COMMS task, it times out the buffer and sets its state*/
        /* to FREE, we are returning error in such case              */
        if (retState != INET_REASSEMBLYBUF_QUEUED){
            retVal = BCM_ERR_INVAL_STATE;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/******************************************************************************
 FUNCTION NAME: INET_ProcessTimers

 DESCRIPTION: Invoked once every second. Used to timeout and flush ARP entries
              in the interfaces' caches

 RETURNS: void
******************************************************************************/
void INET_ProcessTimers(void)
{
    INET_ARPEntry*        cache;
    uint32_t              idx;
    uint32_t              entry;
    int32_t               retVal = BCM_ERR_OK;
    uint32_t              bufCount = 0UL;
    INET_ReassemblyBuffer *reassBuf = NULL;
    uint32_t              notExclusive;
    uint32_t              val;
    uint32_t              isSendARP;
    INET_IPAddress        ipAddrRetry;
    uint32_t              count = 0UL;

    /* Increment the timer event count maintained by IL */
    INET_OSILHandle.numTimerEvents++;

    /* ARP cache timer handling is needed once every 1 second */
    /* This handler is called once every 3.125ms and          */
    /* 12.5ms * 320 = 1 sec                                   */
    if ((INET_OSILHandle.numTimerEvents % INET_ARP_HANDLE_INTERVAL) == 0UL) {

        count = SysCfg_GetTotalNumARPCaches();

        for (idx = 0UL; idx < count; idx++) {
            retVal = SysCfg_GetARPCacheByIndex(idx, &cache);
            if (BCM_ERR_OK == retVal) {
                for (entry = 0UL; entry < INET_MAX_ENTRIES_ARPCACHE; ++entry) {
                    isSendARP = 0UL;
                    do {
                        notExclusive = 0UL;
                        ipAddrRetry = 0UL;
                        val = CORTEX_LdrEx(&cache[entry].exclVal);
                        if ((1UL == cache[entry].valid) &&
                            (INET_ARP_ENTRY_FLAG_STATIC != cache[entry].flags)) {

                            if (0UL == cache[entry].timer) {
                                ipAddrRetry = cache[entry].ipAddr;
                                /* Flush the entry */
                                cache[entry].timer  = 0UL;
                                cache[entry].valid  = 0UL;
                                cache[entry].ipAddr = 0UL;
                                /* For a entry in WAIT state, mark for ARP resend */
                                if (INET_ARP_ENTRY_FLAG_WAIT == cache[entry].flags) {
                                    isSendARP = 1UL;
                                }
                            } else {
                                /* Decrement life of the entry */
                                cache[entry].timer--;
                                /* If the entry is about to timeout, try to refresh */
                                if ((INET_ARP_ENTRY_FLAG_WAIT != cache[entry].flags) &&
                                     (1UL == cache[entry].timer)) {
                                    isSendARP = 1UL;
                                }
                            }
                        }
                        notExclusive = CORTEX_StrEx(&cache[entry].exclVal, ++val);
                    } while (1UL == notExclusive);

                    /* Send ARP request when flag is set */
                    if ((1UL == isSendARP) && (0UL != ipAddrRetry)) {
                        INET_ARPSendRequest(cache[entry].ifID, ipAddrRetry, cache[entry].flags);
                    }
                }

            }
        }
    }

    /* Get the total number of reassembly buffers */
    bufCount = SysCfg_GetTotalNumReassemblyBuffers();

    /* Check all reassembly buffers to be timed out */
    for (idx = 0UL; idx < bufCount; idx++) {
        uint8_t bufTimeOut = 0U;
        retVal = SysCfg_GetReassemblyBufferbyIndex(idx, &reassBuf);

        if (BCM_ERR_OK == retVal){

            /* Case when a reassembly buffer is used and is not fully */
            /* filled as some IP fragment was not received OR         */
            /* Case when the reassembly buffer was queued to the Client */
            /* and if it fails to read within stipulated time           */
            if (((INET_REASSEMBLYBUF_ALLOCATED == reassBuf->bufState) &&
                 (reassBuf->curLen != reassBuf->totalLen)) ||
                 (INET_REASSEMBLYBUF_QUEUED == reassBuf->bufState)){

                /* Decrement timer and manage looparound */
                if (reassBuf->timer < INET_ALARM_EVENT_INTERVAL){
                    reassBuf->timer = 0UL;
                    bufTimeOut = 1U;
                } else {
                    reassBuf->timer -= INET_ALARM_EVENT_INTERVAL;
                }
            }

            /* Case when reassembly buffer timeout */
            if (1U == bufTimeOut){
                /* Increment stats for buffer timeout */
                INET_OSILHandle.inetBufTimeout++;
                (void)INET_FreeReassemblyBuffer(reassBuf->buffer);
            }
        }
    }
    return;
}

/** @brief Buffer available on controller
 *
 * Triggers all end points blocked in the middle of transmission due to
 * unavailability of NIF buffers
 *
 * @param[in]   aHwIdx             H/W controller index
 * @return      void
 */
static void INET_BufferAvailableOnController (const uint32_t aHwIdx)
{
    uint16_t i;
    uint16_t endPointCnt = SysCfg_GetTotalNumEndPoints();

    /* Send txTaskEventId to all tasks in the middle of transmission */
    /* on the network controller which raised the callback        */
    for (i = 0U; i < endPointCnt; ++i) {
        int32_t retVal;
        const INET_SystemEndPoint *sysEndPtObj = NULL;
        retVal = SysCfg_GetEndPointByIndex((uint16_t)i, &sysEndPtObj);

        if (BCM_ERR_OK == retVal) {
            INET_SystemInterface *sysIntfObj = NULL;
            retVal = SysCfg_GetInterfaceForEndPoint(i, &sysIntfObj);
            if (BCM_ERR_OK == retVal) {
                if (aHwIdx == sysIntfObj->ifObj.ifCfg->hwIndex) {
                    if ((sysEndPtObj->endPtObj.ctxt->sentBytes) <
                            (sysEndPtObj->endPtObj.ctxt->txLength)){
                        (void)BCM_SetEvent(sysEndPtObj->txTaskId, sysEndPtObj->txTaskEventId);
                    }
                }
            }
        }
    }
}

/** @brief L2 Packet transmit confirmation
 *
 * Invoked by the network interface to inform that an L2 packet was
 * successfully transmitted
 *
 * @param[in]   aCtrlIdx           NIF controller index
 * @param[in]   aBufIdx            Buffer identifier for the L2 packet
 * @return      void
 */
void INET_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                             uint32_t aBufIdx)
{
    uint32_t aHwIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    /* Track the number of INET Tx Confirmation received */
    INET_OSILHandle.inetTxConfCount++;

    INET_BufferAvailableOnController(aHwIdx);
}

/** @brief L2 buffer available indication
 *
 * Invoked by the network interface to inform that free L2 buffer is
 * available
 *
 * @param[in]   aCtrlIdx           NIF controller index
 * @return      void
 */
void INET_BufferAvailableIndication (NIF_CntrlIDType aCtrlIdx)
{
    uint32_t aHwIdx = NIF_GET_CONTROLLER_INDEX(aCtrlIdx);

    INET_BufferAvailableOnController(aHwIdx);
}

/** @brief Report error information to DET/DEM
 *
 * Report error information to DET/DEM
 *
 * @param[in]  aApiID                 API identifier
 * @param[in]  aErr                   Error number
 * @param[in]  aVal0                  Additional error information
 * @param[in]  aVal1                  Additional error information
 * @param[in]  aVal2                  Additional error information
 * @param[in]  aVal3                  Additional error information
 *
 * @return     void
 */
static void INET_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
#ifndef ULOG
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};

#define INET_COMP_INSTANCE_ID     (0x0U)

    BCM_ReportError(BCM_NET_ID, INET_COMP_INSTANCE_ID, aApiID,
                    aErr, 4UL, values);
#endif
}

