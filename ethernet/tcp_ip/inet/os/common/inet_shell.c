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

/** @file inet_shell.c
    @brief Shell commands for Internet stack

    Shell commands to query status/statistics
*/

/* Includes */
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ee.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <console.h>
#include <bcm_utils.h>
#include <inet_cfg.h>
#include <inet_osil.h>
#include <inet_system.h>

#define GetModuleLogLevel() ULOG_LVL_INFO

#define INET_PRINT_IP_ADDRESS(X) (X >> 24U) & 0xFFU, (X >> 16U) & 0xFFU,\
                                 (X >> 8U) & 0xFFU, (X & 0xFFU)

static void INET_ShowARPCache(void);
static void INET_ShowEndPointStats(void);
static void INET_ShowInterfaceStats(void);
static void INET_ShowReassembly(void);

/** @brief Shell command handler
 *
 * Parses input string to identify the command and invokes the command handler
 *
 * @param[in]   aConsoleID               Console handle
 * @param[in]   aArgString               Command string
 * @param[in]   aArgStringLen            Command string length
 * @return      void
 */
void INET_Shell(uint32_t aConsoleID,
                char* aArgString,
                uint32_t aArgStringLen)
{
    if (NULL != aArgString) {

        if (0 == strncmp(aArgString, "arp", 4UL)) {
            INET_ShowARPCache();
        } else if (0 == strncmp(aArgString, "if", 3UL)) {
            INET_ShowInterfaceStats();
        } else if (0 == strncmp(aArgString, "endpt", 6UL)) {
            INET_ShowEndPointStats();
        } else if (0 == strncmp(aArgString, "rx", 3UL)) {
            INET_ShowReassembly();
        } else {
            CONSOLE_Write(aConsoleID,
                   "inet arp\n"
                   "inet rx\n"
                   "inet if\n"
                   "inet endpt\n");
        }
    } else {
            CONSOLE_Write(aConsoleID,
                   "inet arp\n"
                   "inet rx\n"
                   "inet if\n"
                   "inet endpt\n");
    }
}

static void INET_ShowARPCache(void)
{
    uint32_t intf;
    uint32_t row;
    INET_ARPEntry *entry;
    uint32_t intfCount;
    int32_t retVal;

    intfCount = SysCfg_GetTotalNumInterfaces();

    /* Traverse each interface's cache and print it */
    for (intf = 0UL; intf < intfCount; intf++) {
        INET_SystemInterface *sysIntfObj = NULL;
        retVal = SysCfg_GetInterfaceByIndex(intf, &sysIntfObj);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("Interface %u Fetch Failed\n", intf);
            goto error;
        }
        /* Print local HW ID and IP Address */
        ULOG_INFO("Intf Id: %lu\n", intf);
        ULOG_INFO("HwID: %lu IP Address: %u:%u:%u:%u\n",
                  sysIntfObj->ifObj.ifCfg->hwIndex,
                  INET_PRINT_IP_ADDRESS((sysIntfObj->ifObj.ifCfg->staticAddress)));
        for (row = 0; row < INET_MAX_ENTRIES_ARPCACHE; ++row) {
            entry = &sysIntfObj->arpCache[row];
            if (1UL == entry->valid) {
                ULOG_INFO("%u:%u:%u:%u  %x:%x:%x:%x:%x:%x  %lu  %lu\n",
                INET_PRINT_IP_ADDRESS((entry->ipAddr)), entry->macAddr[0],
                entry->macAddr[1], entry->macAddr[2], entry->macAddr[3],
                entry->macAddr[4], entry->macAddr[5], entry->flags,
                entry->timer);
            }
        }
        ULOG_INFO("-----------------------------------------------\n");
    }
error:
    return;
}

#define INET_MAX_RX_PKTS_FOR_SHELL_PRINT 32UL /* MISRA */

static void INET_ShowEndPointStats(void)
{
    uint16_t endPt;
    INET_EndPointState *ctxt;
    /* MISRA does not permit declaration of an array of variable size */
    INET_RxPkt rxPkts[INET_MAX_RX_PKTS_FOR_SHELL_PRINT];
    uint32_t numRxPkts;
    uint32_t pkt;
    uint32_t allocRxBufCnt = 0UL;

    BCM_MemSet(rxPkts, 0U, sizeof(rxPkts));

    /* Logic to cap and print the first 32 Rx buffers in pool */
    /* As MISRA does not permit declaration and usage of a    */
    /* variable length array - rxPkts                         */
    if (INET_MAX_RX_PKTS_FOR_SHELL_PRINT < SysCfg_GetTotalNumReassemblyBuffers()) {
        allocRxBufCnt = INET_MAX_RX_PKTS_FOR_SHELL_PRINT;
    } else {
        allocRxBufCnt = SysCfg_GetTotalNumReassemblyBuffers();
    }

    /* Traverse each endPoint's context and print stats */
    for (endPt = 0U; endPt < SysCfg_GetTotalNumEndPoints(); endPt++) {
        int retVal;
        const INET_SystemEndPoint *aSysEndptObj;
        retVal = SysCfg_GetEndPointByIndex((uint16_t)endPt,&aSysEndptObj);
        if (BCM_ERR_OK != retVal) {
            goto error;
        }
        ctxt = aSysEndptObj->endPtObj.ctxt;
        ULOG_INFO("EndPt Id: %lu\n", endPt);
        ULOG_INFO("Intf: %lu Remote: %u:%u:%u:%u/%u\n",
                  aSysEndptObj->intfId,
                  INET_PRINT_IP_ADDRESS((aSysEndptObj->endPtObj.cfg->remoteAddr)),
                  aSysEndptObj->endPtObj.cfg->remotePort);
        ULOG_INFO("TX PKTS          : %lu\n", (uint32_t)ctxt->txPkts);
        ULOG_INFO("TX OCTETS        : %lu\n", (uint32_t)ctxt->txBytes);
        ULOG_INFO("RX PKTS          : %lu\n", (uint32_t)ctxt->rxGoodPkts);
        ULOG_INFO("RX OCTETS        : %lu\n", (uint32_t)ctxt->rxBytes);
        ULOG_INFO("RX TRUNCATED PKTS: %lu\n", (uint32_t)ctxt->rxTruncPkts);
        ULOG_INFO("RX DROPPED PKTS  : %lu\n", (uint32_t)ctxt->rxDropPkts);
        numRxPkts = CIRCQ_Peek(ctxt->rxPktQ, (char*)rxPkts, allocRxBufCnt);
        if (numRxPkts != 0UL) {
            ULOG_INFO("Cached Rx Pkts   : ");
            for (pkt = 0UL; pkt < numRxPkts; ++pkt) {
                ULOG_INFO("[%u:%u:%u:%u/%u %u bytes] ",
                INET_PRINT_IP_ADDRESS((rxPkts[pkt].srcIPAddr)),
                rxPkts[pkt].srcPort,
                rxPkts[pkt].length);
            }
            ULOG_INFO("\n");
        }
        ULOG_INFO("-----------------------------------------------\n");
    }
error:
    return;
}

static void INET_ShowInterfaceStats(void)
{
    uint32_t intf;
    INET_InterfaceStats *stats;
    uint32_t intfCount = SysCfg_GetTotalNumInterfaces();
    int32_t retVal;

    /* Traverse each interface's stats and print it */
    for (intf = 0UL; intf < intfCount; intf++) {
        INET_SystemInterface *sysIntfObj = NULL;
        retVal = SysCfg_GetInterfaceByIndex(intf, &sysIntfObj);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("Interface %u Fetch Failed\n", intf);
            goto error;
        }

        stats = sysIntfObj->ifObj.ifStats;
        /* Print local HW ID and IP Address */
        ULOG_INFO("Intf Id: %lu\n", intf);
        ULOG_INFO("HwID: %lu IP Address: %u:%u:%u:%u\n",
                  sysIntfObj->ifObj.ifCfg->hwIndex,
                  INET_PRINT_IP_ADDRESS((sysIntfObj->ifObj.ifCfg->staticAddress)));

        ULOG_INFO("TX PKTS          : %lu\n", stats->txPkts);
        ULOG_INFO("TX OCTETS        : %lu\n", stats->txBytes);
        ULOG_INFO("TX ARP PKTS      : %lu\n", stats->txARP);
        ULOG_INFO("TX ICMP PKTS     : %lu\n", stats->txICMP);
        ULOG_INFO("TX DROP PKTS     : %lu\n", stats->txDropPkts);
        ULOG_INFO("TX ARP Failure   : %lu\n", stats->txARPFailure);
        ULOG_INFO("TX CANCEL PKTS   : %lu\n", stats->txCancelPkts);

        ULOG_INFO("RX PKTS          : %lu\n", stats->rxPkts);
        ULOG_INFO("RX OCTETS        : %lu\n", stats->rxBytes);
        ULOG_INFO("RX DROPPED PKTS  : %lu\n", stats->rxDropPkts);
        ULOG_INFO("RX ARP PKTS      : %lu\n", stats->rxARP);
        ULOG_INFO("RX ICMP PKTS     : %lu\n", stats->rxICMP);
        ULOG_INFO("-----------------------------------------------\n");
    }
error:
    return;
}

static void INET_ShowReassembly(void)
{
    uint32_t buf;
    uint32_t fragCount;
    uint32_t fragment;
    uint16_t fragments[2UL^INET_LOG2_MAX_REASS_FRAGS] = {0U};
    INET_ReassemblyBuffer *reassBuffer = NULL;
    int32_t retVal;

    /* Traverse each buffer and print its details */
    for (buf = 0UL; buf < SysCfg_GetTotalNumReassemblyBuffers(); buf++) {
        retVal = SysCfg_GetReassemblyBufferbyIndex(buf, &reassBuffer);
        if (BCM_ERR_OK != retVal) {
            ULOG_INFO("Getting reassembly buffer %d Failed\n", buf);
            goto error;
        }
        ULOG_INFO("Buffer: %lu   State: %lu\n", buf, reassBuffer->bufState);
        if (INET_REASSEMBLYBUF_FREE != reassBuffer->bufState) {

            ULOG_INFO("IP Addr: %u:%u:%u:%u   Ident: %u\n",
                    INET_PRINT_IP_ADDRESS((reassBuffer->ipAddr)),
                    reassBuffer->ident);

            ULOG_INFO("Buffer: 0x%x  CurLen: %u  TotalLen: %u  Timer: %lu  clientEndPtID: %u\n",
                    reassBuffer->buffer,
                    reassBuffer->curLen,
                    reassBuffer->totalLen,
                    reassBuffer->timer,
                    reassBuffer->clientEndPtID);

            fragCount = CIRCQ_Peek(reassBuffer->fragmentQ,
                                  (char*)fragments, 2UL^INET_LOG2_MAX_REASS_FRAGS);
            if (fragCount != 0UL) {
                ULOG_INFO("Fragments: ");
                for (fragment = 0UL; fragment < fragCount; ++fragment) {
                    ULOG_INFO("%u ", fragments[fragment]);
                }
                ULOG_INFO("\n");
            }
        }
        ULOG_INFO("-----------------------------------------------\n");
    }
error:
    return;
}
