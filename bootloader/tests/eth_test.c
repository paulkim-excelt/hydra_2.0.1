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
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <log.h>
#include <bcm_err.h>
#include <ethernet.h>
#include <eth_xcvr.h>
#include <utils.h>

#define GetModuleLogLevel() LOG_LVL_INFO

#define ETHER_HW_ID_0       (0UL)
#define ETHER_CHAN_ID_0     (0UL)

#define TEST_PKT_COUNT      (2UL)

#define ETH_PKT_PATTERN_SZ  (46UL)

/* Unimac loopback is an internal API but to make ethernet testing
   independent of external network, it is required to do loopback.
*/
extern void BL_UNIMAC_EnableLoopback(uint32_t enable, uint32_t local);

static const uint8_t ethPkt[3][ETH_PKT_PATTERN_SZ] = {
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x9F, 0x18,
0x08, 0x00, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6E, 0x65, 0x74, 0x20, 0x54,
0x65, 0x73, 0x74, 0x20, 0x41, 0x50, 0x50, 0x3A, 0x20, 0x54, 0x65, 0x73,
0x74, 0x20, 0x50, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x74,
},
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x18,
0x80, 0x00, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x54,
0x65, 0x73, 0x74, 0x20, 0x41, 0x50, 0x50, 0x3a, 0x20, 0x53, 0x65, 0x63,
0x6f, 0x6e, 0x64, 0x20, 0x70, 0x61, 0x63, 0x6b, 0x65, 0x74,
},
{
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x9f, 0x18,
0x80, 0x00, 0x45, 0x74, 0x68, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x54,
0x65, 0x73, 0x74, 0x20, 0x41, 0x50, 0x50, 0x3a, 0x20, 0x54, 0x68, 0x69,
0x72, 0x64, 0x20, 0x20, 0x70, 0x61, 0x63, 0x6b, 0x65, 0x74,
},
};

static uint32_t RcvCbCnt = 0UL;
static uint32_t isRcvErr = FALSE;
static uint32_t XmtCbCnt[4UL] = {0UL, 0UL, 0UL, 0UL};
static uint32_t ethPktSize = 0UL;

/* Receive callback */
void ETHER_RxInd (ETHER_HwIDType aID,
            const uint8_t *const aBuf,
            uint16_t aLen)
{
    int32_t retVal;
    uint32_t i;

    LOG_INFO("ETHER_RxInd aBuf: %p aLen: %d\n", aBuf, aLen);

    if (aLen == ethPktSize) {
        for (i = 0UL; i < ethPktSize; i += ETH_PKT_PATTERN_SZ) {
            retVal =  memcmp(&aBuf[i], &ethPkt[RcvCbCnt/3][0],
                                            ETH_PKT_PATTERN_SZ);
            if (0UL != retVal) {
                LOG_ERR("Pkt[%d] Data mismatch\n", RcvCbCnt);
                isRcvErr = TRUE;
            }
        }
    } else if (aLen == 60UL) {
        if (ethPktSize > 60UL) {
            isRcvErr = TRUE;
            LOG_ERR("Pkt[%d] len[%d] != expectedLen[%d]\n",
                    RcvCbCnt, aLen, ethPktSize);
        } else {
            for (i = 0UL; i < ethPktSize; i += ETH_PKT_PATTERN_SZ) {
                retVal =  memcmp(&aBuf[i], &ethPkt[RcvCbCnt/3][0],
                                                ETH_PKT_PATTERN_SZ);
                if (0UL != retVal) {
                    LOG_ERR("Pkt[%d] Data mismatch\n", RcvCbCnt);
                    isRcvErr = TRUE;
                }
            }
        }
    } else {
        isRcvErr = TRUE;
        LOG_ERR("Pkt[%d] len[%d] != expectedLen[%d]\n",
                RcvCbCnt, aLen, ethPktSize);
    }
    RcvCbCnt++;
}

static int32_t TrasmitTest(ETHER_HwIDType aHwID,
            ETHER_ChanIDType aChannID,
            uint32_t aPktLen)
{
    int32_t retVal;
    uint32_t i, j;
    uint8_t *pktBuf = NULL;
    uint32_t bufIdx = 0UL;
    uint32_t pktLen;

    for (i = 0UL; i < TEST_PKT_COUNT; i++) {
        pktLen = aPktLen;
        retVal = ETHER_GetTxBuffer(aHwID, aChannID, &bufIdx,
                &pktBuf, &pktLen);
        if (BCM_ERR_NOMEM == retVal) {
            LOG_ERR("TrasmitTest: ETHER_GetTxBuffer() failed\n");
            break;
        }
        for (j = 0; j < pktLen; j += ETH_PKT_PATTERN_SZ) {
            memcpy((void *)&pktBuf[j], &ethPkt[i/3][0], ETH_PKT_PATTERN_SZ);
        }
        retVal = ETHER_Send(aHwID, aChannID, bufIdx, pktLen);
        if (BCM_ERR_OK != retVal) {
            LOG_ERR("ETHER_Send failure\n");
            break;
        }
        LOG_ERR("Send packet bufIdx: %d Len: %d\n", bufIdx, pktLen);
    }

    return retVal;
}

static int32_t ChannInitnLoopback()
{
#define RX_WAIT_TIME_CNT        (1000)
    int32_t retVal;
    uint32_t i;
    uint32_t pollCnt;

    ETHER_Init(ETHER_HW_ID_0, ETHER_CHAN_ID_0);

    for (i = 0UL; i < 4UL; i++) {
        pollCnt = 0UL;
        isRcvErr = FALSE;
        RcvCbCnt = 0UL;
        ethPktSize = ETH_PKT_PATTERN_SZ * (1 + (8 * i)) ;
        retVal = TrasmitTest(ETHER_HW_ID_0, ETHER_CHAN_ID_0, ethPktSize);
        if (retVal == BCM_ERR_OK) {
            do {
                ETHER_ProcessPkts();
                if (TRUE == isRcvErr) {
                    retVal = BCM_ERR_UNKNOWN;
                    break;
                }
            } while ((pollCnt++ < RX_WAIT_TIME_CNT) && (RcvCbCnt != TEST_PKT_COUNT));
            if (retVal != BCM_ERR_OK) {
                LOG_ERR("TX test failed!!\n");
                break;
            }
        }
    }
    return retVal;
}

static void XcvrInitNegativeTest(void)
{
    ETHER_XcvrCfgType xcvrConfig;
    const uint8_t macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    xcvrConfig.speed = ETHER_XCVR_SPEED_100MBPS;
    xcvrConfig.mode = ETHER_XCVR_DUPLEX_MODE_FULL;
    xcvrConfig.maxRxFrmLen = 1522UL;
    xcvrConfig.macAddr = macAddr;

    LOG_ERR("XcvrInitNegativeTest1 Test\n");
    ETHER_XcvrInit(1UL, &xcvrConfig);

    LOG_ERR("XcvrInitNegativeTest2 Test\n");
    ETHER_XcvrInit(ETHER_XCVR_HW_ID_0, NULL);
}

static void EtherInitPostiveTest(void)
{
    LOG_ERR("%s Test\n", __func__);

    ETHER_Init(ETHER_HW_ID_0, ETHER_CHAN_ID_0);
}

static void EtherInitNegativeTest(void)
{

    LOG_ERR("EtherInitNegativeTest1 Test\n");
    ETHER_Init(1UL, ETHER_CHAN_ID_0);

    LOG_ERR("EtherInitNegativeTest2 Test\n");
    ETHER_Init(ETHER_HW_ID_0, 4UL);
}

static void EtherGetTxBufNegativeTest(void)
{
    int32_t retVal;
    uint32_t bufIdx;
    uint8_t *buf = NULL;
    uint32_t len = 1518UL;

    retVal = ETHER_GetTxBuffer(1UL, ETHER_CHAN_ID_0, &bufIdx, &buf, &len);
    if (BCM_ERR_INVAL_PARAMS == retVal) {
        LOG_ERR("EtherGetTxBufNegativeTest1 Passed\n");
    } else {
        LOG_ERR("EtherGetTxBufNegativeTest1 failed\n");
    }

    retVal = ETHER_GetTxBuffer(ETHER_HW_ID_0, 4UL, &bufIdx, &buf, &len);
    if (BCM_ERR_INVAL_PARAMS == retVal) {
        LOG_ERR("EtherGetTxBufNegativeTest2 Passed\n");
    } else {
        LOG_ERR("EtherGetTxBufNegativeTest2 failed\n");
    }

    retVal = ETHER_GetTxBuffer(ETHER_HW_ID_0, ETHER_CHAN_ID_0,
                            NULL, &buf, &len);
    if (BCM_ERR_INVAL_PARAMS == retVal) {
        LOG_ERR("EtherGetTxBufNegativeTest3 Passed\n");
    } else {
        LOG_ERR("EtherGetTxBufNegativeTest3 failed\n");
    }

    retVal = ETHER_GetTxBuffer(ETHER_HW_ID_0, ETHER_CHAN_ID_0,
                            &bufIdx, NULL, &len);
    if (BCM_ERR_INVAL_PARAMS == retVal) {
        LOG_ERR("EtherGetTxBufNegativeTest4 Passed\n");
    } else {
        LOG_ERR("EtherGetTxBufNegativeTest4 failed\n");
    }
}

static void EtherSendNegativeTest(void)
{
    int32_t retVal;
    uint32_t j;
    uint32_t bufIdx;
    uint8_t *buff = NULL;
    uint32_t len = 1518UL;

    ethPktSize = len;
    retVal = ETHER_GetTxBuffer(ETHER_HW_ID_0, ETHER_CHAN_ID_0,
                            &bufIdx, &buff, &len);
    if (BCM_ERR_OK == retVal) {
        for (j = 0; j < 1518UL; j += ETH_PKT_PATTERN_SZ) {
            memcpy(&buff[j], &ethPkt[0][0], ETH_PKT_PATTERN_SZ);
        }
        retVal = ETHER_Send(1UL, ETHER_CHAN_ID_0, bufIdx, len);
        if (BCM_ERR_INVAL_PARAMS == retVal) {
            LOG_ERR("EtherSendNegativeTest1 passed\n");
        }
        retVal = ETHER_Send(ETHER_HW_ID_0, 4UL, bufIdx, len);
        if (BCM_ERR_INVAL_PARAMS == retVal) {
            LOG_ERR("EtherSendNegativeTest2 passed\n");
        }
        retVal = ETHER_Send(ETHER_HW_ID_0, 4UL, 20, len);
        if (BCM_ERR_INVAL_PARAMS == retVal) {
            LOG_ERR("EtherSendNegativeTest3 passed\n");
        }
        len = 2000UL;
        retVal = ETHER_Send(ETHER_HW_ID_0, ETHER_CHAN_ID_0, bufIdx, len);
        if (BCM_ERR_INVAL_PARAMS == retVal) {
            LOG_ERR("EtherSendNegativeTest4 passed\n");
        }
    } else {
        LOG_ERR("%s:ETHER_GetTxBuffer failure\n", __func__);
    }
}

static void TxRxTest(void)
{
    int32_t retVal;
    ETHER_XcvrCfgType xcvrConfig;
    const uint8_t macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

    xcvrConfig.speed = ETHER_XCVR_SPEED_100MBPS;
    xcvrConfig.mode = ETHER_XCVR_DUPLEX_MODE_FULL;
    xcvrConfig.maxRxFrmLen = 1522UL;
    xcvrConfig.macAddr = macAddr;

    LOG_ERR("TxRxTest++ \n");
    ETHER_XcvrInit(ETHER_XCVR_HW_ID_0, &xcvrConfig);
    BL_UNIMAC_EnableLoopback(1, 1);

    retVal = ChannInitnLoopback();
    if (retVal == BCM_ERR_OK) {
        LOG_INFO("TxRxTest Passed!!\n");
    } else {
        LOG_INFO("TxRxTest Failed!!\n");
    }
    LOG_ERR("TxRxTest--\n");
}

void EthernetTest(void)
{
    TxRxTest();
    XcvrInitNegativeTest();
    EtherInitNegativeTest();
    EtherInitPostiveTest();
    EtherGetTxBufNegativeTest();
    EtherSendNegativeTest();
}
