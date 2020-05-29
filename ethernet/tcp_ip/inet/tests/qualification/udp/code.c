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

/**
  @defgroup grp_inet_qt Qualification Tests
  @ingroup grp_inetlib

  @addtogroup grp_inet_qt
  @{

  @file code.c
  @brief INET Library Integration Test
  This source file contains the qualification tests for INET library
  @version 0.86 Import from MS Word document
*/

#include <stdlib.h>
#include <string.h>
#include <ee.h>
#include <bcm_test.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <eth_osil.h>
#include <inet_cfg.h>
#include <osil/bcm_osil.h>

#include "inet_system.h"
#include "udp.h"

#define TEST1_PKT_SIZE      (INET_MAX_REASS_BUF_SIZE - 500U)
#define TEST2_PKT_SIZE      (INET_MAX_REASS_BUF_SIZE)
#define TEST3_PKT_SIZE       7500U
#define TEST1_NUM_TX_PKTS    1000UL
#define TEST3_NUM_TX_PKTS    100UL

#define INET_PRINT_IP_ADDRESS(X) (X >> 24U) & 0xFFU, (X >> 16U) & 0xFFU,\
                                 (X >> 8U) & 0xFFU, (X & 0xFFU)

#define GetModuleLogLevel()  ULOG_LVL_ERROR

static uint8_t          txBuf[TEST1_PKT_SIZE];
static uint8_t          rxBuf[TEST2_PKT_SIZE];
#ifdef DISABLE_INET_LOOPBACK_TEST
static uint8_t          tx1Buf[TEST3_PKT_SIZE];
static uint8_t          tx2Buf[TEST3_PKT_SIZE];
#endif
static volatile int32_t inet_qt1_result = BCM_AT_NOT_STARTED;
static volatile int32_t inet_qt2_result = BCM_AT_NOT_STARTED;
static uint32_t timer = 0UL;

void Systick_Callback(void)
{
    timer++;
    SetEvent(Test1Task, SystemEvent0);
    SetEvent(Test2Task, SystemEvent0);
#ifdef DISABLE_INET_LOOPBACK_TEST
    SetEvent(Test3Client1Task, SystemEvent0);
    SetEvent(Test3Client2Task, SystemEvent0);
#endif
}

/** @brief Brief description of COMP_IT1

  This test simulates UDP transmission, UDP reception and UDP cancellation.

  @code{.unparsed}

     # Add a static entry to the ARP cache to enable UDP_SendTo as ethernet
       loopback is enabled
     # Send a buffer of size INET_MAX_REASS_BUF_SIZE bytes filled with 0xAA
       pattern in all bytes using the UDP_SendTo API to a destination in unicast
       mode.
     # When BCM_ERR_BUSY is returned, wait for the UDPTxEvent to execute
       UDP_Process API.
     # Invoke UDP_Process API repeatedly until it returns BCM_ERR_OK
     # As loopback test is enabled, this packet should be received by the same
       DUT and INET should set the event UDPRxEvent to the client task
     # In a separate task, call the UDP_Recv API whenever UDPRxEvent is got
     # Check for the return code from UDP_Recv and also the more data flag of
       UDP_Recv API
     # Invoke UDP_Recv API as long as more data flag is TRUE
     # Validate the Rx data with 0xAA value filled by the sender
     # Repeat steps 1-9 for ‘n’ times and validate
     # Repeat steps 1-10 for sending data to multicast and broadcast
       address/end points
     # Send a buffer of size 14500 bytes using the UDP_SendTo API
     # If BCM_ERR_BUSY is returned, execute UDP_CancelSend API and verify that
       this returns BCM_ERR_OK
     # In the task receiving data, check the throughput displayed.
@endcode
*/

TASK(Test1Task)
{
    int32_t       ret;
    BCM_EventMaskType mask = 0UL;
    uint32_t      txInProgress = 0UL;
    uint32_t      txPktCnt = 0UL;
    INET_EndPointIPPort entPtCfg;

#ifndef DISABLE_INET_LOOPBACK_TEST
    INET_SystemInterface *sysIntfObj = NULL;
    INET_ARPEntry* cache;

    /* Get the interface structure for ARP cache access */
    ret = SysCfg_GetInterfaceByIndex(0UL, &sysIntfObj);
    if (BCM_ERR_OK != ret) {
        inet_qt1_result = ret;
    }
    cache = sysIntfObj->arpCache;

    /* Adding a dummy entry into ARP cache for loopback tests */
    cache->valid = 1UL;
    cache->ipAddr = (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (3U)));
    cache->flags = INET_ARP_ENTRY_FLAG_STATIC;

    entPtCfg.localPort = 5000U;
    /* For loopback to same IP */
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (3U));
    entPtCfg.remotePort = 5000U;
#else
    entPtCfg.localPort = 5000U;
    /* For loopback to same IP */
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U));
    entPtCfg.remotePort = 5000U;
#endif

    memset(txBuf, 0xAAU, TEST1_PKT_SIZE);

    /* Test UDP_SendTo and UDP_Process */
    while (txPktCnt < TEST1_NUM_TX_PKTS) {

        /* By default INET reassembly buffers are configured  */
        /* with 15 KB size. Hence we test for total Tx packet */
        /* TEST1_PKT_SIZE less than 15 KB                     */
        ret = UDP_SendTo(0x1234, INET_UDP_PAYLOAD_ID0, txBuf, TEST1_PKT_SIZE, &entPtCfg);
        if (ret == BCM_ERR_BUSY) {
            txInProgress = 1UL;
        } else if (ret == BCM_ERR_OK) {
            txPktCnt++;
        } else {
            ULOG_ERR("\nError: %d \t returned by UDP_SendTo",ret);
            inet_qt1_result = ret;
            break;
        }

        /* Send out any pending packets */
        while (1UL == txInProgress) {
            /* Wait for data or for retry event */
            BCM_WaitEvent(UDPTxEvent);
            BCM_GetEvent(Test1Task, &mask);

            if (mask & UDPTxEvent) {
                BCM_ClearEvent(UDPTxEvent);
                if (txInProgress == 1UL) {
                    ret = UDP_Process(0x1234, INET_UDP_PAYLOAD_ID0);
                    if (ret == BCM_ERR_OK) {
                        txInProgress = 0UL;
                        txPktCnt++;
                    } else if (ret != BCM_ERR_BUSY) {
                        ULOG_ERR("\nError: %d \t returned by UDP_Process",ret);
                        inet_qt1_result = ret;
                        break;
                    }
                }
            }
        }
    }

    ULOG_ERR("Tx Test for Unicast streaming - sent %d Tx Packets \n",txPktCnt);
    txPktCnt = 0UL;

    entPtCfg.localPort = 5003U;
    /* Multicast IP address */
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((224U), (10U), (10U), (10U));
    entPtCfg.remotePort = 5003U;

    /* Send packets on multicast address */
    while (txPktCnt < TEST3_NUM_TX_PKTS){

        ret = UDP_SendTo(0x1234, INET_UDP_PAYLOAD_ID3, txBuf, TEST1_PKT_SIZE, &entPtCfg);
        if (ret == BCM_ERR_BUSY) {
            txInProgress = 1UL;
        } else if (ret == BCM_ERR_OK) {
            txPktCnt++;
        } else {
            ULOG_ERR("\nError: %d \t returned by UDP_SendTo",ret);
            inet_qt1_result = ret;
            break;
        }

        /* Send out any pending packets */
        while (1UL == txInProgress) {
            /* Wait for data or for retry event */
            BCM_WaitEvent(UDPTxEvent);
            BCM_GetEvent(Test1Task, &mask);

            if (mask & UDPTxEvent) {
                BCM_ClearEvent(UDPTxEvent);
                if (txInProgress == 1UL) {
                    ret = UDP_Process(0x1234, INET_UDP_PAYLOAD_ID3);
                    if (ret == BCM_ERR_OK) {
                        txInProgress = 0UL;
                        txPktCnt++;
                    } else if (ret != BCM_ERR_BUSY) {
                        ULOG_ERR("\nError: %d \t returned by UDP_Process",ret);
                        inet_qt1_result = ret;
                        break;
                    }
                }
            }
        }
    }

    ULOG_ERR("Tx Test for Multicast streaming - sent %d Tx Packets \n",txPktCnt);
    txPktCnt = 0UL;

    entPtCfg.localPort = 5004U;
    /* Broadcast IP address */
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((255U), (255U), (255U), (255U));
    entPtCfg.remotePort = 5004U;

    /* Send packets on broadcast address */
    /* Test UDP_SendTo and UDP_Process     */
    while (txPktCnt < TEST3_NUM_TX_PKTS) {

        ret = UDP_SendTo(0x1234, INET_UDP_PAYLOAD_ID4, txBuf, TEST1_PKT_SIZE, &entPtCfg);
        if (ret == BCM_ERR_BUSY) {
            txInProgress = 1UL;
        } else if (ret == BCM_ERR_OK) {
            txPktCnt++;
        } else {
            ULOG_ERR("\nError: %d \t returned by UDP_SendTo",ret);
            inet_qt1_result = ret;
            break;
        }

        /* Send out any pending packets */
        while (1UL == txInProgress) {
            /* Wait for data or for retry event */
            BCM_WaitEvent(UDPTxEvent);
            BCM_GetEvent(Test1Task, &mask);

            if (mask & UDPTxEvent) {
                BCM_ClearEvent(UDPTxEvent);
                if (txInProgress == 1UL) {
                    ret = UDP_Process(0x1234, INET_UDP_PAYLOAD_ID4);
                    if (ret == BCM_ERR_OK) {
                        txInProgress = 0UL;
                        txPktCnt++;
                    } else if (ret != BCM_ERR_BUSY) {
                        ULOG_ERR("\nError: %d \t returned by UDP_Process",ret);
                        inet_qt1_result = ret;
                        break;
                    }
                }
            }
        }
    }

    ULOG_ERR("Tx Test for Broadcast streaming - sent %d Tx Packets \n",txPktCnt);

    entPtCfg.localPort = 5000U;
    /* For loopback to same IP */
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (3U));
    entPtCfg.remotePort = 5000U;
    /* Test UDP_SendTo and UDP_CancelSend */
    ret = UDP_SendTo(0x1234, 0UL, txBuf, TEST1_PKT_SIZE, &entPtCfg);
    if (ret == BCM_ERR_BUSY) {
        /* Try and cancel the transmission */
        ret = UDP_CancelSend(0x1234, 0UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("\nError: %d \t returned by UDP_CancelSend",ret);
            inet_qt1_result = ret;
        }
    } else {
        ULOG_INFO("\nUDP_CancelSend not tested %d", ret);
    }

#ifdef DISABLE_INET_LOOPBACK_TEST
    if (BCM_AT_EXECUTING == inet_qt1_result) {
        inet_qt1_result = BCM_ERR_OK;
    }
#endif
    BCM_TerminateTask();
}

TASK(Test2Task)
{
    int32_t        ret = BCM_ERR_OK;
    BCM_EventMaskType  mask = 0UL;
    uint16_t       rxBufLen;
    uint32_t       rxPktCnt = 0UL;
    uint32_t       rxBytes  = 0UL;
    INET_IPAddress srcIPAddr;
    INET_Port      srcPort;
    BCM_TimeType       start, end, diff;
    uint32_t       firstTime = 1UL;
    uint64_t       timeElapsedNs;
    uint64_t       bitrate = 0ULL;
    timer = 0UL;

    /* Wait till all packets are received or till timeout */
    while (timer < 500UL) {
        uint8_t        moreData = 0U;

        /* Wait for data */
        BCM_WaitEvent(UDPRxEvent|SystemEvent0|SystemEvent4|SystemEvent5);
        BCM_GetEvent(Test2Task, &mask);

        if (mask & UDPRxEvent) {
            BCM_ClearEvent(UDPRxEvent);
            do {
                rxBufLen = TEST2_PKT_SIZE;
                moreData = 0U;

                ret = UDP_Recv(0x1234, INET_UDP_PAYLOAD_ID0, rxBuf, &rxBufLen,
                               &srcIPAddr, &srcPort, &moreData);
                if ((BCM_ERR_OK == ret ) && (0U < rxBufLen)) {
                    if (1UL == firstTime) {
                        BCM_GetTime(&start);
                        firstTime = 0UL;
                    } else {
                        BCM_GetTime(&end);
                    }
                    rxPktCnt++;
                    rxBytes += rxBufLen;
#ifndef DISABLE_INET_LOOPBACK_TEST
                    if (rxBuf[0U] == (0xAAU & 0xFF)) {
                        ULOG_INFO("%u)Received %u bytes\n", rxPktCnt, rxBufLen);
                    } else {
                        ULOG_ERR("%u) Corruption %u bytes with byte 0x%x\n",
                                  rxPktCnt, rxBufLen, rxBuf[0U]);
                    }
#endif
                } else {
                    ULOG_ERR("\nError: %d \t returned by UDP_Recv",ret);
                }
            } while(1U == moreData);
        }

        if (mask & SystemEvent4) {
            BCM_ClearEvent(SystemEvent4);
            do {
                rxBufLen = TEST2_PKT_SIZE;
                moreData = 0U;

                ret = UDP_Recv(0x1234, INET_UDP_PAYLOAD_ID3, rxBuf, &rxBufLen,
                               &srcIPAddr, &srcPort, &moreData);
                if ((BCM_ERR_OK == ret ) && (0U < rxBufLen)) {
                    if (1UL == firstTime) {
                        BCM_GetTime(&start);
                        firstTime = 0UL;
                    } else {
                        BCM_GetTime(&end);
                    }
                    rxPktCnt++;
                    rxBytes += rxBufLen;
#ifndef DISABLE_INET_LOOPBACK_TEST
                    if (rxBuf[0U] == (0xAAU & 0xFF)) {
                        ULOG_INFO("%u)Received %u bytes\n", rxPktCnt, rxBufLen);
                    } else {
                        ULOG_ERR("%u) Corruption %u bytes with byte 0x%x\n",
                                  rxPktCnt, rxBufLen, rxBuf[0U]);
                    }
#endif
                } else {
                    ULOG_ERR("\nError: %d \t returned by UDP_Recv",ret);
                }
            } while(1U == moreData);
        }

        if (mask & SystemEvent5) {
            BCM_ClearEvent(SystemEvent5);
            do {
                rxBufLen = TEST2_PKT_SIZE;
                moreData = 0U;

                ret = UDP_Recv(0x1234, INET_UDP_PAYLOAD_ID4, rxBuf, &rxBufLen,
                               &srcIPAddr, &srcPort, &moreData);
                if ((BCM_ERR_OK == ret ) && (0U < rxBufLen)) {
                    if (1UL == firstTime) {
                        BCM_GetTime(&start);
                        firstTime = 0UL;
                    } else {
                        BCM_GetTime(&end);
                    }
                    rxPktCnt++;
                    rxBytes += rxBufLen;
#ifndef DISABLE_INET_LOOPBACK_TEST
                    if (rxBuf[0U] == (0xAAU & 0xFF)) {
                        ULOG_INFO("%u)Received %u bytes\n", rxPktCnt, rxBufLen);
                    } else {
                        ULOG_ERR("%u) Corruption %u bytes with byte 0x%x\n",
                                  rxPktCnt, rxBufLen, rxBuf[0U]);
                    }
#endif
                } else {
                    ULOG_ERR("\nError: %d \t returned by UDP_Recv",ret);
                }
            } while(1U == moreData);
        }

        if (mask & SystemEvent0) {
            BCM_ClearEvent(SystemEvent0);
        }

    }

    diff = BCM_GetTimeAbsDiff(start, end);
    timeElapsedNs = diff.s * 1000000000ULL + diff.ns;
    if (0ULL != timeElapsedNs){
        bitrate = (rxBytes * 8000000000ULL)/ timeElapsedNs;
    }
    ULOG_ERR("\nThroughput Test : %lu pkts %lu bytes recvd %u (bps) bitrate\n",
                    rxPktCnt, rxBytes, (uint32_t)bitrate );

#ifndef DISABLE_INET_LOOPBACK_TEST
    /* TEST1_NUM_TX_PKTS - Unicast packets   */
    /* TEST3_NUM_TX_PKTS - Multicast packets */
    /* TEST3_NUM_TX_PKTS - Broadcast packets */
    if ((TEST1_NUM_TX_PKTS + TEST3_NUM_TX_PKTS + TEST3_NUM_TX_PKTS) > rxPktCnt) {
        inet_qt1_result = BCM_ERR_UNKNOWN;
    } else {
        inet_qt1_result = BCM_ERR_OK;
    }
#endif

    BCM_TerminateTask();
}

TASK(Test3Client1Task)
{
#ifdef DISABLE_INET_LOOPBACK_TEST
    int32_t       ret;
    BCM_EventMaskType mask = 0UL;
    uint32_t      txInProgress = 0UL;
    uint32_t      txPktCnt = 0UL;
    INET_EndPointIPPort entPtCfg;

    memset(tx1Buf, 0x11U, TEST3_PKT_SIZE);

    entPtCfg.localPort = 5001U;
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U));
    entPtCfg.remotePort = 5001U;

    /* Test UDP_SendTo and UDP_Process */
    while (txPktCnt < TEST3_NUM_TX_PKTS) {
        /* Wait for data or for retry event */
        BCM_WaitEvent(UDPTxEvent | SystemEvent0);
        BCM_GetEvent(Test3Client1Task, &mask);
        if (mask & SystemEvent0) {
            BCM_ClearEvent(SystemEvent0);

            if(txInProgress == 1UL) {
                ULOG_ERR("\nOverflow");
                inet_qt2_result = BCM_ERR_TIME_OUT;
                break;
            } else {
                ret = UDP_SendTo(0x1234, 1UL, tx1Buf, TEST3_PKT_SIZE, &entPtCfg);
                if (ret == BCM_ERR_BUSY) {
                    txInProgress = 1UL;
                } else if (ret == BCM_ERR_OK) {
                    txPktCnt++;
                } else {
                    ULOG_ERR("\nError: %d \t returned by UDP_SendTo",ret);
                    inet_qt2_result = ret;
                    break;
                }
            }
        }

        if (mask & UDPTxEvent) {
            BCM_ClearEvent(UDPTxEvent);
            if (txInProgress == 1UL) {
                ret = UDP_Process(0x1234, 1UL);
                if (ret == BCM_ERR_OK) {
                    txInProgress = 0UL;
                    txPktCnt++;
                } else if (ret != BCM_ERR_BUSY) {
                    ULOG_ERR("\nError: %d \t returned by UDP_Process",ret);
                    inet_qt2_result = ret;
                    break;
                }
            }
        }
    }
#endif
    BCM_TerminateTask();
}

TASK(Test3Client2Task)
{
#ifdef DISABLE_INET_LOOPBACK_TEST
    int32_t       ret;
    BCM_EventMaskType mask = 0UL;
    uint32_t      txInProgress = 0UL;
    uint32_t      txPktCnt = 0UL;
    INET_EndPointIPPort entPtCfg;

    memset(tx2Buf, 0x11U, TEST3_PKT_SIZE);

    entPtCfg.localPort = 5002U;
    entPtCfg.remoteAddr = INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U));
    entPtCfg.remotePort = 5002U;

    /* Test UDP_SendTo and UDP_Process */
    while (txPktCnt < TEST3_NUM_TX_PKTS) {
        /* Wait for data or for retry event */
        BCM_WaitEvent(UDPTxEvent | SystemEvent0);
        BCM_GetEvent(Test3Client2Task, &mask);
        if (mask & SystemEvent0) {
            BCM_ClearEvent(SystemEvent0);

            if(txInProgress == 1UL) {
                ULOG_ERR("\nOverflow");
                inet_qt2_result = BCM_ERR_TIME_OUT;
                break;
            } else {
                ret = UDP_SendTo(0x1234, 2UL, tx1Buf, TEST3_PKT_SIZE, &entPtCfg);
                if (ret == BCM_ERR_BUSY) {
                    txInProgress = 1UL;
                } else if (ret == BCM_ERR_OK) {
                    txPktCnt++;
                } else {
                    ULOG_ERR("\nError: %d \t returned by UDP_SendTo",ret);
                    inet_qt2_result = ret;
                    break;
                }
            }
        }

        if (mask & UDPTxEvent) {
            BCM_ClearEvent(UDPTxEvent);
            if (txInProgress == 1UL) {
                ret = UDP_Process(0x1234, 2UL);
                if (ret == BCM_ERR_OK) {
                    txInProgress = 0UL;
                    txPktCnt++;
                } else if (ret != BCM_ERR_BUSY) {
                    ULOG_ERR("\nError: %d \t returned by UDP_Process",ret);
                    inet_qt2_result = ret;
                    break;
                }
            }
        }
    }
#endif
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex && inet_qt1_result != BCM_AT_EXECUTING) {
        inet_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test2Task);
        BCM_ActivateTask(Test1Task);
        ret = BCM_ERR_OK;
    }
#ifdef DISABLE_INET_LOOPBACK_TEST
        else if (2UL == aIndex && inet_qt2_result != BCM_AT_EXECUTING) {

        inet_qt2_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test3Client1Task);
        BCM_ActivateTask(Test3Client2Task);
        ULOG_INFO("\nTest exited\n");
        if (BCM_AT_EXECUTING == inet_qt2_result) {
            inet_qt2_result = BCM_ERR_OK;
        }
        ret = BCM_ERR_OK;
    } else {
        /* Nothing */
    }
#endif

    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    switch (aIndex)
    {
        case 1UL:
            ret = inet_qt1_result;
            break;
#ifdef DISABLE_INET_LOOPBACK_TEST
        case 2UL:
            ret = inet_qt2_result;
            break;
#endif
        default:
            break;
    }
    return ret;
}

void ApplicationInit()
{
#ifndef DISABLE_INET_LOOPBACK_TEST
    ETHER_EnableLoopBack(ETHER_HW_ID_0);
#endif
}
