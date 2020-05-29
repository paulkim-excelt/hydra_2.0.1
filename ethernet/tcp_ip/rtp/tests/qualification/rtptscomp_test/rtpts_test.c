/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
 File Name: rtpts_test.c
 Description: This file implements the IT application for RTP Sender and Receiver.
******************************************************************************/

/* Includes */
#include <stddef.h>
#include <compiler.h>
#include <string.h>

#include <media_system.h>
#include <component.h>
#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <atomic.h>
#include <eth_osil.h>
#include <inet_cfg.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>

#include "inet_system.h"
#include "rtp_sender.h"
#include "udp.h"

extern uint32_t GetAPPLogLevel();
#define GetModuleLogLevel   GetAPPLogLevel
#define CMD_STR_LEN         (5UL)
extern volatile int32_t rtpts_qt1_result;
volatile int32_t rtpts_qt1_result = BCM_AT_NOT_STARTED;

#define VIDEO_FPS 30UL
#define TS_TIMESTAMP_DIFF ((1000UL/VIDEO_FPS) * 90000UL)
#define RTP_SSRC_VALUE (0x00000001UL)

#define ES_PID (17U)
#define PMT_PID (16U)
#define PAT_PID (0U)
#define NULL_PID (0x1FFF)

uint8_t RTPTSRxBuffer[INET_MAX_REASS_BUF_SIZE];
uint16_t lastRTPseq = 0U;
uint32_t lastRTPts = 0UL;
uint32_t ccPAT;
uint32_t ccPMT;
uint32_t ccES;

/* Varaibles used to track the incoming TS packets */
uint32_t numESPacket = 0UL;
uint32_t numPATPacket = 0UL;
uint32_t numPMTPacket = 0UL;
uint32_t numNULLPacket = 0UL;
uint32_t numESVidFrames = 0UL;
uint32_t numESVidBytes = 0UL;

uint32_t GetAPPLogLevel()
{
    return ULOG_LVL_INFO;
}

const MSYS_Type mediaSys = {
    .magicID = MSYS_MAGIC,
    .count = 2UL,

    .component[0].compID = MSYS_COMP_ID_EX_SRC,
    .component[0].numConfigs = 1UL,
    .component[0].config[0] = 0UL,
    .component[0].portMap[0].srcCompIdx = 0U,
    .component[0].portMap[0].srcPortIdx = 0U,
    .component[0].portMap[1].srcCompIdx = 0U,
    .component[0].portMap[1].srcPortIdx = 0U,
    .component[0].portMap[2].srcCompIdx = 0U,
    .component[0].portMap[2].srcPortIdx = 0U,
    .component[0].portMap[3].srcCompIdx = 0U,
    .component[0].portMap[3].srcPortIdx = 0U,

    .component[1].compID = MSYS_COMP_ID_RTPS,
    .component[1].numConfigs = 2UL,
    .component[1].config[0] = MSYS_CONFIG_RTP_SENDER,
    .component[1].config[1] = MSYS_CONFIG_MPEGTS_LIB,
    .component[1].portMap[0].srcCompIdx = 0U,
    .component[1].portMap[0].srcPortIdx = 0U,
#if (1UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[1].srcCompIdx = 0U,
    .component[1].portMap[1].srcPortIdx = 1U,
#endif
#if (2UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[2].srcCompIdx = 0U,
    .component[1].portMap[2].srcPortIdx = 2U,
#endif
#if (3UL < RTP_MAX_NUM_INPUT_PORTS)
    .component[1].portMap[3].srcCompIdx = 0U,
    .component[1].portMap[3].srcPortIdx = 3U,
#endif
};

/*
 * This API is invoked by the framework to retrieve the media system connection
 * information.
 */
const MSYS_Type* MSYS_Get(uint32_t aIdx)
{
    const MSYS_Type* mSys = NULL;
    if(0UL == aIdx) {
        mSys = &mediaSys;
    }
    return mSys;
}


void MSYS_WdsErrorCb(const char* aName,
	             int32_t aError,
                     const WDS_MonitorType *aMonitor,
		     uint32_t aMaxAllowedErrors)
{
}

void RTPTSTest_Start(void)
{
    int32_t ret;

    ret = MSYS_Init(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Init Failed %d\n",ret);
        rtpts_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Configure(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Configure Failed %d\n",ret);
        rtpts_qt1_result = ret;
        goto err;
    }

    ret = MSYS_Start(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Start Failed %d\n",ret);
        rtpts_qt1_result = ret;
    }

err:
    return;
}

void RTPTSTest_Stop(void)
{
    int32_t ret;

    ret = MSYS_Stop(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_Stop Failed %d\n",ret);
        rtpts_qt1_result = ret;
        goto err;
    }

    ret = MSYS_DeInit(&mediaSys);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("MSYS_DeInit Failed %d\n",ret);
        rtpts_qt1_result = ret;
    }

    /* Reset the global variables */
    lastRTPseq = 0UL;
    lastRTPts = 0UL;
    ccPAT = 0UL;
    ccPMT = 0UL;
    ccES = 0UL;

    if (BCM_AT_EXECUTING == rtpts_qt1_result) {
        rtpts_qt1_result = BCM_ERR_OK;
    }
err:
    return;
}

volatile uint32_t RTP_TestAlarmVal = 0UL;
uint32_t gRTPSTestCount = 0UL;
void Test_RTPTSAlarm_cb()
{
    if(1UL == RTP_TestAlarmVal)
    {
        if(++gRTPSTestCount == 2)
        {
            ATOMIC_Set(&RTP_TestAlarmVal, 0UL);
            SetEvent(Test_RTPTSComp,SystemEvent0);
        }
    }
}

/* Function to validate the RTP header in payload */
static int32_t RTPTS_ValidateRTPHeader(uint8_t *buf)
{
    int32_t retVal = BCM_ERR_OK;
    uint16_t curRTPseq = 0U;
    uint32_t curRTPts = 0UL;
    uint32_t curSSRC = 0UL;

    /* Check RTP payload type */
    if((0x80U != buf[0]) && (0x21U != buf[1])){
        ULOG_ERR("ERR > RTP Hdr Incorrect \n");
        retVal = BCM_ERR_DATA_INTEG;
        goto error;
    }

    (void)BCM_MemCpy(&curRTPseq, buf+2, 2UL);
    (void)BCM_MemCpy(&curRTPts, buf+4, 4UL);
    (void)BCM_MemCpy(&curSSRC, buf+8, 4UL);

    /* Check sequence number, timestamp and SSRC */
    if((lastRTPseq != BE2Host16(curRTPseq)) ||
       ((BE2Host32(curRTPts) > lastRTPts)?
            ((BE2Host32(curRTPts) - lastRTPts) < (TS_TIMESTAMP_DIFF - 100UL)):0UL) ||
       (RTP_SSRC_VALUE != BE2Host32(curSSRC))){

        ULOG_ERR("ERROR > last_seq %u \t cur_seq %u last_ts %u cur_ts %u SSRC %u \n",
            lastRTPseq,BE2Host16(curRTPseq),lastRTPts,
            BE2Host32(curRTPts),BE2Host32(curSSRC));
        retVal = BCM_ERR_UNKNOWN;
        goto error;
    }

    lastRTPseq++;
    lastRTPts = BE2Host32(curRTPts);

error:
    return retVal;
}

/* Function to validate the TS packets in payload */
static int32_t RTPTS_ValidateTSPacket(uint8_t *buf, uint32_t bufLen)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t totalLenProc = 0UL;
    uint8_t payloadStartInd = 0U;
    uint16_t tsPID = 0U;
    uint8_t adapField = 0U;
    uint8_t cc = 0U;

    while(totalLenProc < bufLen){
        uint32_t tsPktStartLen = totalLenProc;

        /* Verify the TS sync byte */
        if (0x47U != buf[totalLenProc]) {
            ULOG_ERR("ERR > TS Sync byte Incorrect %u %u --- %x \n",
                                totalLenProc,bufLen,buf[totalLenProc]);
            retVal = BCM_ERR_DATA_INTEG;
            goto error;
        }

        /* Fetch the TS header fields */
        payloadStartInd = (buf[totalLenProc+1] >> 6U) & 0x1;
        BCM_MemCpy(&tsPID, &buf[totalLenProc+1], 2U);
        tsPID = BE2Host16(tsPID) & 0x1FFFU;
        adapField = (buf[totalLenProc+3] >> 4U) & 3U;
        cc = buf[totalLenProc+3] & 0xFU;
        totalLenProc += 4U;

        /* Check if its a PAT packet */
        if (PAT_PID == tsPID){
            numPATPacket++;
            totalLenProc += 184U;
            if((ccPAT % 0x10) != cc){
                ULOG_ERR("ERR > CC-PAT actual %u exp %u \n",cc,ccPAT%0x10);
                ccPAT = cc + 1;
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            ccPAT = cc + 1;
        /* Check if its a PMT packet */
        } else if (PMT_PID == tsPID){
            numPMTPacket++;
            totalLenProc += 184U;
            if((ccPMT % 0x10) != cc){
                ULOG_ERR("ERR > CC-PMT actual %u exp %u \n",cc,ccPMT%0x10);
                ccPMT = cc + 1;
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            ccPMT = cc + 1;
        /* Check if its a ES-PES packet */
        } else if (ES_PID == tsPID){
            numESPacket++;
            if((ccES % 0x10) != cc){
                ULOG_ERR("ERR > CC-ES actual %u exp %u \n",cc,ccES%0x10);
                ccES = cc + 1;
                retVal = BCM_ERR_UNKNOWN;
                goto error;
            }
            ccES = cc + 1;

            /* Parse Adaptation Control Field */
            if(1U == ((adapField >> 1U) & 1U)){
                uint8_t adapLen = buf[totalLenProc];
                totalLenProc += adapLen+1;
            }

            /* Parse Payload Field */
            if (1U == (adapField & 1U)){
                if((1U == payloadStartInd) &&
                   ((0U == buf[totalLenProc]) &&
                    (0U == buf[totalLenProc+1]) &&
                    (1U == buf[totalLenProc+2]))){
                    numESVidFrames++;
                }
                numESVidBytes += 188UL - (totalLenProc - tsPktStartLen);
                totalLenProc += 188UL - (totalLenProc - tsPktStartLen);
            }
        /* Check if its a NULL packet */
        } else if (NULL_PID == tsPID){
            numNULLPacket++;
            totalLenProc += 184U;
        /* In case of an unsupported packet */
        } else {
            ULOG_ERR("ERR > Unsupported packet PID %d \n",tsPID);
            retVal = BCM_ERR_NOSUPPORT;
            goto error;
        }

    }

error:
    return retVal;

}

/* Task to receive the ethernet packets and validate the TS packets */
TASK(Test_TSRxTask)
{
    BCM_EventMaskType mask = 0UL;
    uint16_t dataSize = 0U;
    INET_IPAddress ipAddr;
    INET_Port ipPort;
    int32_t  udpRet = BCM_ERR_OK;
    int32_t retVal;

    /* As long as the test is in running state */
    while(1){
        uint8_t  moreData     = 0U;

        /* Wait on events from INET UDP stack */
        (void)BCM_WaitEvent(UDPRxEvent | SystemEvent0);
        (void)BCM_GetEvent(Test_TSRxTask, &mask);
        (void)BCM_ClearEvent(mask);
        if (0U != (mask & SystemEvent0)) {
            break;
        }

        do {
            moreData = 0U;
            dataSize = (uint16_t)(INET_MAX_REASS_BUF_SIZE);

            /* Receive the UDP buffer */
            udpRet = UDP_Recv(BCM_MTL_ID, 0UL,
                              RTPTSRxBuffer, &dataSize,
                              &ipAddr, &ipPort, &moreData);
            if ((BCM_ERR_OK == udpRet) &&
                (0U < dataSize)) {

                /* Verify the RTP header in the packet */
                retVal = RTPTS_ValidateRTPHeader(RTPTSRxBuffer);
                if(BCM_ERR_OK != retVal){
                    ULOG_ERR("RTP Header Error %u\n",retVal);
                    rtpts_qt1_result = retVal;
                    continue;
                }

                /* Verify the 'n' TS packets in payload */
                retVal = RTPTS_ValidateTSPacket(RTPTSRxBuffer+12,
                                                dataSize-12);
                if(BCM_ERR_OK != retVal){
                    ULOG_ERR("TS Packet Error %u\n",retVal);
                    rtpts_qt1_result = retVal;
                    continue;
                }
            }
        }while(1U == moreData);
    }

    BCM_TerminateTask();
}

TASK(Test_RTPTSComp)
{
    INET_SystemInterface *sysIntfObj = NULL;
    INET_ARPEntry* cache;
    int32_t ret;

    /* Get the interface structure for ARP cache access */
    ret = SysCfg_GetInterfaceByIndex(0UL, &sysIntfObj);
    if (BCM_ERR_OK != ret) {
        rtpts_qt1_result = ret;
    }
    cache = sysIntfObj->arpCache;

    /* Adding a dummy entry into ARP cache for loopback tests */
    cache->valid = 1UL;
    cache->ipAddr = (INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)));
    cache->flags = INET_ARP_ENTRY_FLAG_STATIC;

    RTPTSTest_Start();
    gRTPSTestCount = 0UL;
    ATOMIC_Set(&RTP_TestAlarmVal, 1UL);
    BCM_WaitEvent(SystemEvent0);
    RTPTSTest_Stop();
    (void)BCM_SetEvent(Test_TSRxTask,SystemEvent0);

    ULOG_ERR("RTPTS TEST > PAT=%u PMT=%u NULL=%u ES-PES=%u VidFrames=%u VidBytes=%u\n",
                  numPATPacket, numPMTPacket, numNULLPacket,
                  numESPacket, numESVidFrames, numESVidBytes);

    /* Reset the logging counters */
    numPATPacket = 0UL;
    numPMTPacket = 0UL;
    numNULLPacket = 0UL;
    numESPacket = 0UL;
    numESVidFrames = 0UL;
    numESVidBytes = 0UL;

    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        rtpts_qt1_result = BCM_AT_EXECUTING;
        BCM_ActivateTask(Test_RTPTSComp);
        BCM_ActivateTask(Test_TSRxTask);
        ULOG_INFO("\n Test started");
        ret = BCM_ERR_OK;
    }
    return ret;
}
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = rtpts_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
    ETHER_EnableLoopBack(ETHER_HW_ID_0);
    BCM_ActivateTask(ExSrc_CmdProcessTask);
}
