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

/**
    @defgroup grp_pplib_qt Qualification Tests
    @ingroup grp_pplib

    @addtogroup grp_pplib_qt
    @{

    @file pplib_rt.c
    @brief PP Library Qualification Test
    This source file contains the requirement tests for pplib
    @version 1.4 Import from docx
*/

#include <stdlib.h>
#include <string.h>
#include <ee.h>
#include <pp.h>
#include <sp.h>
#include <ulog.h>
#include <console.h>
#include <bcm_test.h>
#include <imgl.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <bcm_time.h>
#include <mcu_ext.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_INFO

static volatile int32_t ppdma_qt_result = BCM_AT_NOT_STARTED;

#define PP_DMA_TEST_MAX_BUF_SIZE             (5UL * 30UL*1024UL)

uint8_t COMP_SECTION(".pp.uncached.test") inImage[PP_DMA_TEST_MAX_BUF_SIZE];
uint8_t COMP_SECTION(".pp.uncached.test") outImage[PP_DMA_TEST_MAX_BUF_SIZE];

int32_t PP_RunCameraDMATest()
{
    uint32_t i;
    uint32_t testCase;
    uint32_t ispID;
    int32_t ret;
    PP_DMACopyMsgType dmaSendMsg;
    PP_DMACopyMsgType dmaRecvMsg;
    BCM_EventMaskType mask;
    uint32_t jobIdx;
    BCM_TimeType startTime,endTime,totalTime;
    uint32_t width[3UL] = {1920,1920/2,1920/2};
    uint32_t height[3UL] = {16UL,8UL,8UL};
    uint8_t *inY = &inImage[0UL];
    uint8_t *inU = &inImage[1920UL*16UL* 3UL];
    uint8_t *inV = &inImage[(1920UL*16UL* 3UL) + (1920UL*4UL*3UL)];
    uint8_t *outY = &outImage[0];
    uint8_t *outU = &outImage[1920UL*16UL];
    uint8_t *outV = &outImage[(1920UL*16UL) + (1920UL*4UL)];
    uint8_t *inAddr[3UL] = {inY,inU,inV};
    uint8_t *inAddr3[3UL][3UL] = {{inY,inU,inV},
                                  {inY + (1920UL*16UL),inU + (1920UL*4UL),inV + (1920UL*4UL)},
                                  {inY + (1920UL*16UL*2UL),inU + (1920UL*4UL*2UL),inV + (1920UL*4UL*2UL)}};
    uint8_t *outAddr[3UL] = {outY,outU,outV};
    uint32_t size[3UL] = {1920UL*16UL,1920UL*4UL,1920UL*4UL};

    memset(inY,1,1920UL*16UL);
    memset(&inY[1920UL*16UL],2,1920UL*16UL);
    memset(&inY[1920UL*16UL*2UL],3,1920UL*16UL);

    memset(inU,1,1920UL*4UL);
    memset(&inU[1920UL*4UL],2,1920UL*4UL);
    memset(&inU[1920UL*4UL*2UL],3,1920UL*4UL);

    memset(inV,1,1920UL*4UL);
    memset(&inV[1920UL*4UL],2,1920UL*4UL);
    memset(&inV[1920UL*4UL*2UL],3,1920UL*4UL);

    memset(outImage,0,PP_DMA_TEST_MAX_BUF_SIZE);

    /* configure the DMA job */
    for (i=0;i<3UL;i++) {
        dmaSendMsg.chnInfo[i].inAddr = (uint32_t)inAddr[i];
        dmaSendMsg.chnInfo[i].outAddr = (uint32_t)outAddr[i];
        dmaSendMsg.chnInfo[i].width = width[i];
        dmaSendMsg.chnInfo[i].height = height[i];
        dmaSendMsg.chnInfo[i].inStride = width[i];
        dmaSendMsg.chnInfo[i].outStride = width[i];
    }

    dmaSendMsg.numCh = 3U;

    for (ispID = 0UL; ispID <= 1UL; ispID++) {
        /* configure HS to CPU using give ISP*/
        if (0UL == ispID) {
            MCU_SetDispHsSrc(MCU_CPUDHSCH_0, MCU_CPUDHSID_CPU);
        } else {
            MCU_SetDispHsSrc(MCU_CPUDHSCH_1, MCU_CPUDHSID_CPU);
        }

        /* send custom command to enable this */
        ret = PP_CustomCmd(0UL, PP_CUSTOM_CMD5 , (ispID << 16UL) | 3UL);
        if (BCM_ERR_OK != ret) {
            goto exit;
        }

        for (testCase = 0UL; testCase < 3UL; testCase++) {

            dmaSendMsg.mbNum = testCase;
            dmaSendMsg.bufIdx = testCase % 3UL;

            jobIdx = 0xFFFFFFFFUL;
            BCM_GetTime(&startTime);
            /* send prioroty DMA job */
            ret = PP_CameraDMAStart(&dmaSendMsg,&jobIdx,1U);
            if (BCM_ERR_OK != ret) {
                goto exit;
            }
            if (0UL == ispID) {
                if (0UL == testCase) {
                    MCU_DispHsTrigger(MCU_CPUDHSCH_0, 1UL);
                } else {
                    MCU_DispHsTrigger(MCU_CPUDHSCH_0, 0UL);
                }
            } else {
                if (0UL == testCase) {
                    MCU_DispHsTrigger(MCU_CPUDHSCH_1, 1UL);
                } else {
                    MCU_DispHsTrigger(MCU_CPUDHSCH_1, 0UL);
                }
            }

            do {
                BCM_WaitEvent(PP_GPEvent);
                BCM_GetEvent(PPDMATestTask,&mask);
                BCM_ClearEvent(mask);
                ret = PP_CameraDMAStatus(&dmaRecvMsg,jobIdx);
            } while (BCM_ERR_BUSY == ret);

            BCM_GetTime(&endTime);
            totalTime = BCM_GetTimeAbsDiff(startTime,endTime);
            ULOG_INFO("\n Time: (%d,%d) \n",totalTime.s,totalTime.ns);

            ret = BCM_ERR_OK;
            for (i=0UL;i<3UL;i++) {
                if (0UL != memcmp(inAddr3[testCase][i],outAddr[i],size[i])) {
                    ret = BCM_ERR_DATA_INTEG;
                    ULOG_INFO("\n First Test Failed: (%d,%d,%d) \n",ispID,testCase,i);
                    goto exit;
                }
            }
        }
        if (0UL == ispID) {
            MCU_DispHsTrigger(MCU_CPUDHSCH_0, 0UL);
            MCU_DispHsTrigger(MCU_CPUDHSCH_0, 0UL);
        } else {
            MCU_DispHsTrigger(MCU_CPUDHSCH_1, 0UL);
            MCU_DispHsTrigger(MCU_CPUDHSCH_1, 0UL);
        }
        dmaSendMsg.mbNum = testCase; /* This should overwrite */
        dmaSendMsg.bufIdx = testCase % 3UL;
        dmaSendMsg.numCh = 3U;

        jobIdx = 0xFFFFFFFFUL;
        BCM_GetTime(&startTime);
        /* send prioroty DMA job */
        ret = PP_CameraDMAStart(&dmaSendMsg,&jobIdx,1U);
        if (BCM_ERR_OK != ret) {
            goto exit;
        }

        /* Negative testing */
        ret = PP_CustomCmd(0UL, PP_CUSTOM_CMD5 , (ispID << 16UL) | 3UL);
        if (BCM_ERR_INVAL_STATE != ret) {
            goto exit;
        }

        do {
            BCM_WaitEvent(PP_GPEvent);
            BCM_GetEvent(PPDMATestTask,&mask);
            BCM_ClearEvent(mask);
            ret = PP_CameraDMAStatus(&dmaRecvMsg,jobIdx);
        } while (BCM_ERR_BUSY == ret);

        BCM_GetTime(&endTime);
        totalTime = BCM_GetTimeAbsDiff(startTime,endTime);
        ULOG_INFO("\n Time: (%d,%d) \n",totalTime.s,totalTime.ns);

        ret = BCM_ERR_OK;

        if (((testCase + 1UL) != (uint32_t)dmaRecvMsg.mbNum) ||
            (((uint8_t)(testCase + 1UL) % 3UL) != dmaRecvMsg.bufIdx)) {
            ULOG_INFO("\n Second Test BA Failed: (%d,%d) \n",ispID,testCase);
            goto exit;
        }

        for (i=0UL; i<3UL; i++) {
            if (0UL != memcmp(inAddr3[(testCase + 1UL) % 3UL][i],outAddr[i],size[i])) {
                ret = BCM_ERR_DATA_INTEG;
                ULOG_INFO("\n Second Test Failed: (%d,%d,%d) \n",ispID,testCase,i);
                goto exit;
            }
        }
    }
exit:
    return ret;
}
TASK(PPDMATestTask)
{
    int32_t ret;
    if (BCM_AT_EXECUTING == ppdma_qt_result) {
        ret = PP_RunCameraDMATest();
        if (BCM_ERR_OK != ret) {
            ppdma_qt_result = BCM_ERR_DATA_INTEG;
        }
    }

    /* Test1: Configure and check for 3 buffers */
    if (BCM_AT_EXECUTING == ppdma_qt_result) {
        ppdma_qt_result = BCM_ERR_OK;
    }

    ULOG_INFO("**********%s copy completed (%d)***********\n",__func__, ppdma_qt_result);

    BCM_TerminateTask();
}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ppdma_qt_result = BCM_AT_EXECUTING;
        ULOG_INFO("Test started\n");
        BCM_ActivateTask(PPDMATestTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = ppdma_qt_result;
    }
    return ret;
}

void ApplicationInit()
{
}
/** @} */
