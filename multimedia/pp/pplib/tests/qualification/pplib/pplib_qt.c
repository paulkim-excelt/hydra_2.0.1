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
static volatile uint32_t PPLogLevel = ULOG_LVL_INFO;
#define GetModuleLogLevel()     PPLogLevel
#define CMD_STR_LEN         (5UL)

#define CHECK_ERR(ret,line) {                               \
    if (BCM_ERR_OK != (ret)) {                              \
        ULOG_ERR("\nError: %d \t line %d",(ret),(line));    \
        (pplib_qt1_result) = (ret);                         \
    }                                                       \
}
static uint8_t COMP_SECTION(".pp.uncached.test") PPLibTestBin[0x1000UL];

static volatile int32_t pplib_qt1_result = BCM_AT_NOT_STARTED;
static uint32_t CoreID = 0UL;
static char Payload[PP_MSG_PAYLOAD_SIZE];

/**
    @name PP Library Qualification Test Sequence IDs
    @{
    @brief PP Library Qualification Test Sequence IDs
*/
#define BRCM_QTDSGN_PPLIB_SEQ0_PROC      (0xF0UL)   /**< @brief #PPLibTestTask1  */
#define BRCM_QTDSGN_PPLIB_SEQ1_PROC      (0xF1UL)   /**< @brief #PPDMATest  */
/** @} */

/**
    @brief Allocate a v-MP core

    @trace #BRCM_QTST_PPLIB_VMP_ALLOC    #BRCM_REQ_PPLIB_VMP_ALLOC
*/
#define BRCM_QTST_PPLIB_VMP_ALLOC   (0UL)

/**
    @brief Free an allocated v-MP core

    @trace #BRCM_QTST_PPLIB_VMP_FREE    #BRCM_REQ_PPLIB_VMP_ALLOC
*/
#define BRCM_QTST_PPLIB_VMP_FREE   (1UL)

/**
    @brief Change the client to be triggered for IPC message

    @trace #BRCM_QTST_PPLIB_VMP_IPC_CLIENT    #BRCM_REQ_PPLIB_VMP_IPC
*/
#define BRCM_QTST_PPLIB_VMP_IPC_CLIENT   (2UL)

/**
    @brief Send a message to v-MP

    @trace #BRCM_QTST_PPLIB_VMP_IPC_SEND    #BRCM_REQ_PPLIB_VMP_IPC
*/
#define BRCM_QTST_PPLIB_VMP_IPC_SEND   (3UL)

/**
    @brief Read a message to v-MP

    @trace #BRCM_QTST_PPLIB_VMP_IPC_RECV    #BRCM_REQ_PPLIB_VMP_IPC
*/
#define BRCM_QTST_PPLIB_VMP_IPC_RECV   (4UL)

/**
    @brief Boot v-MP core

    @trace #BRCM_QTST_PPLIB_VMP_BOOT_START    #BRCM_REQ_PPLIB_VMP_BOOT
*/
#define BRCM_QTST_PPLIB_VMP_BOOT_START   (5UL)

/**
    @brief Stop v-MP core

    @trace #BRCM_QTST_PPLIB_VMP_BOOT_STOP    #BRCM_REQ_PPLIB_VMP_BOOT
*/
#define BRCM_QTST_PPLIB_VMP_BOOT_STOP   (6UL)

/**
    @brief Read GP Data

    @trace #BRCM_QTST_PPLIB_VMP_IPC_GPREAD    #BRCM_REQ_PPLIB_VMP_IPC
*/
#define BRCM_QTST_PPLIB_VMP_IPC_GPREAD   (7UL)

/**
    @brief Write GP Data

    @trace #BRCM_QTST_PPLIB_VMP_IPC_GPWRITE    #BRCM_REQ_PPLIB_VMP_IPC
*/
#define BRCM_QTST_PPLIB_VMP_IPC_GPWRITE   (8UL)

/** @brief Brief description of BRCM_QTSEQ_COMP_SEQ0

    Detailed description of BRCM_QTSEQ_COMP_SEQ0

    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_ALLOC
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_FREE
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_IPC_CLIENT
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_IPC_SEND
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_IPC_RECV
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_BOOT_START
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_BOOT_STOP
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_IPC_GPREAD
    @testproc  #BRCM_QTDSGN_PPLIB_SEQ0  #BRCM_QTST_PPLIB_VMP_IPC_GPWRITE

    @code{.c}
    Create Task1 and Wait for a Signal from Task2
    Create Task2
    Task2: Allocate a core using PP_AllocCore API (BRCM_QTST_PPLIB_VMP_ALLOC)
        API shall return ERR_OK and give a non-zero Core ID
    Task2: Update the Client ID of the core to Task1 using PP_UpdateClient API (BRCM_QTST_PPLIB_VMP_IPC_CLIENT)
        API shall return ERR_OK
    Task2: Terminate
    Task1: Boot the core with binary attached using PP_BootCore API[binary attached as a C file] (BRCM_QTST_PPLIB_VMP_BOOT_START)
        API shall return ERR_OK
    Task1: Send a mailbox message[PP_SendMsg] with msg_type = 0x01234567 and payload with a pattern 0xAA (BRCM_QTST_PPLIB_VMP_IPC_SEND)
        API shall return ERR_OK
    Task1: Wait for Event notification from PP Driver for incoming message
    Task1: Read mailbox message using PP_RecvMsg (BRCM_QTST_PPLIB_VMP_IPC_RECV)
        API shall return ERR_OK
        Received payload shall have pattern 0xBB and Message type = 0x89ABCDEF
    Task1: Write GPData1 with 0x33335555 using PP_WriteGPData (BRCM_QTST_PPLIB_VMP_IPC_GPWRITE)
        API shall return ERR_OK
    Task1: Read back GPData1 using PP_ReadGPData (BRCM_QTST_PPLIB_VMP_IPC_GPREAD)
        API shall return ERR_OK and the value shall be 0x33335555
    Task1: Read back GPData0 using PP_ReadGPData (BRCM_QTST_PPLIB_VMP_IPC_GPREAD)
        API shall return ERR_OK and the value shall be 0x01234567
    Task1: Stop the core using PP_StopCore (BRCM_QTST_PPLIB_VMP_BOOT_STOP)
        API shall return ERR_OK
    Task1: Free the core using PP_FreeCore (BRCM_QTST_PPLIB_VMP_FREE)
        API shall return ERR_OK
    Task1: Terminate
    @endcode

*/
#define BRCM_QTDSGN_PPLIB_SEQ0   (1UL)


TASK(PPLibTestTask1)
{
    uint32_t i;
    uint32_t msgType = 0UL;
    uint32_t gpdataVal = 0UL;
    int32_t ret;
    BCM_EventMaskType mask = 0UL;
    uint32_t ppLibTestBinSize = 0UL;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;


    /* Load pp binary */
     ret = IMGL_LoadImg((uint16_t)0xEE20, PPLibTestBin, 0UL,
                        0x1000UL, SystemEvent1, &imglStatus);
    CHECK_ERR(ret, __LINE__);

    (void)BCM_WaitEvent(SystemEvent1);

    (void)BCM_ClearEvent(SystemEvent1);

    ptmStatus = IMGL_GetStatus(&imglStatus);
    if (ptmStatus != BCM_ERR_OK) {
        ULOG_ERR("\n PTM Error: %d \tfile: %s \t line %d",ptmStatus,__FILE__,__LINE__);
    } else {
        ppLibTestBinSize = imglStatus.size;
    }
    ULOG_INFO("\n PPLibTestTask1 Activated");
    ULOG_INFO("\n Waiting for event from PPLibTestTask2");
    (void)BCM_WaitEvent(TriggerEvent0);
    (void)BCM_GetEvent(PPLibTestTask1, &mask);
    if (0UL != (mask & TriggerEvent0)) {
       (void)BCM_ClearEvent(TriggerEvent0);
    }

    ULOG_INFO("\n Event received from PPLibTestTask2");
    ret = PP_BootCore(CoreID,PPLibTestBin,ppLibTestBinSize);
    CHECK_ERR(ret,__LINE__);

    for (i=0UL; i<56UL; i++) {
        Payload[i] = (char)0xAA;
    }
    ULOG_INFO("\n Core Booted ");
    ret = PP_SendMsg(CoreID,0x01234567,Payload);
    CHECK_ERR(ret,__LINE__);

    ULOG_INFO("\n Message sent");
    (void)BCM_WaitEvent(PP_GPEvent);
    (void)BCM_GetEvent(PPLibTestTask1, &mask);
    if (0UL != (mask & PP_GPEvent)) {
        (void)BCM_ClearEvent(PP_GPEvent);
    }
    ULOG_INFO("\n Mailbox event received ");
    ret = PP_RecvMsg(CoreID,&msgType,Payload);
    CHECK_ERR(ret,__LINE__);
    /* check message type */
    if (0x89ABCDEFUL != msgType) {
        ULOG_ERR("\nPP_RecvMsg msgType 0x%x Error\t line %d",
            msgType,__LINE__);
    }
    /* check payload */
    for (i=0UL; i<56UL; i++) {
        if ((char)0xBB != Payload[i]) {
            ULOG_ERR("\nPP_RecvMsg payload 0x%x Error\t line %d",
                Payload[i],__LINE__);
            break;
        }
    }

    ret = PP_WriteGPData(CoreID,1UL,0x33335555);
    CHECK_ERR(ret,__LINE__);

    ret = PP_ReadGPData(CoreID,1UL,&gpdataVal);
    CHECK_ERR(ret,__LINE__);

    if (0x33335555UL != gpdataVal) {
        ULOG_ERR("\nPP_ReadGPData gpdata1 0x%x Error\t line %d",
            gpdataVal,__LINE__);
    } else {
        ULOG_INFO("\n PP_WriteGPData Successful");
    }

    ret = PP_ReadGPData(CoreID,0UL,&gpdataVal);
    CHECK_ERR(ret,__LINE__);

    if (0x01234567UL != gpdataVal) {
        ULOG_ERR("\nPP_ReadGPData gpdata0 0x%x Error\t line %d",
            gpdataVal,__LINE__);
    } else {
        ULOG_INFO("\n PP_ReadGPData Successful");
    }

    ret = PP_StopCore(CoreID);
    CHECK_ERR(ret,__LINE__);

    ret = PP_FreeCore(CoreID);
    CHECK_ERR(ret,__LINE__);

    ULOG_INFO("\n Terminating PPLibTestTask1");

    ULOG_INFO("\n Test completed");
    if (BCM_AT_EXECUTING == pplib_qt1_result) {
        pplib_qt1_result = BCM_ERR_OK;
    }
    (void)BCM_TerminateTask();

}

TASK(PPLibTestTask2)
{
    int32_t ret;

    ULOG_INFO("\n PPLibTestTask2 Activated");
    ret = PP_AllocCore(&CoreID);
    CHECK_ERR(ret,__LINE__);
    if (0UL == CoreID) {
        ULOG_ERR("\nCoreID Error\t line %d",__LINE__);
    }

    ret = PP_UpdateClient(CoreID, PPLibTestTask1, PP_CLIENT_ALL);
    CHECK_ERR(ret,__LINE__);

    ULOG_INFO("\n Posting event to PPLibTestTask1");
    (void)BCM_SetEvent(PPLibTestTask1, TriggerEvent0);

    ULOG_INFO("\n Terminating PPLibTestTask2");
    (void)BCM_TerminateTask();
}



static volatile int32_t ppdma_qt2_result = BCM_AT_NOT_STARTED;

#define PP_DMA_TEST_MAX_BUF_SIZE             (5UL * 30UL*1024UL)
#define PP_DMA_NUM_TEST_CASES                (5UL)

static uint8_t COMP_SECTION(".pp.uncached.test") inImage[PP_DMA_TEST_MAX_BUF_SIZE];
static uint8_t COMP_SECTION(".pp.uncached.test") outImage[PP_DMA_TEST_MAX_BUF_SIZE];


#define CHECK_ERR_CP(ret,file,line)  \
    if ((BCM_ERR_OK != ret) && (BCM_ERR_EAGAIN != ret)) { \
        ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line);\
        ppdma_qt2_result = ret;\
        }
static int32_t PP_RunDMATest(uint32_t aChnCnt,uint32_t *aChnWidth, uint32_t *aChnHeight)
{
    uint32_t i;
    int32_t ret;
    PP_DMABufferType inBuffer;
    BCM_EventMaskType mask;
    PP_DMABufferType outBuffer;
    uint32_t copiedSize = 0UL;

    uint32_t jobIdx;
    BCM_TimeType startTime,endTime,totalTime;

    inBuffer.numCH = aChnCnt;
    outBuffer.numCH = aChnCnt;
    for (i=0UL; i<aChnCnt; i++) {
        inBuffer.channel[i].addr = &inImage[copiedSize];
        inBuffer.channel[i].width = aChnWidth[i];
        inBuffer.channel[i].height = aChnHeight[i];
        inBuffer.channel[i].stride = aChnWidth[i];

        outBuffer.channel[i].addr = &outImage[copiedSize];

        outBuffer.channel[i].width = aChnWidth[i];
        outBuffer.channel[i].height = aChnHeight[i];
        outBuffer.channel[i].stride = aChnWidth[i];

        copiedSize += (aChnWidth[i] * aChnHeight[i]);
    }

    if ((0UL < copiedSize) && (PP_DMA_TEST_MAX_BUF_SIZE >= copiedSize)) {
        (void)memset(inImage,2,copiedSize);
        (void)memset(outImage,3,copiedSize);
    }

    jobIdx = 0xFFFFFFFFUL;
    BCM_GetTime(&startTime);
    ret = PP_DMAStart(&inBuffer, &outBuffer, &jobIdx);
    if (BCM_ERR_OK != ret) {
        goto exit;
    }

    do {
        (void)BCM_WaitEvent(PP_GPEvent);
        (void)BCM_GetEvent(PPDMATest,&mask);
        (void)BCM_ClearEvent(mask);
        ret = PP_DMAStatus(jobIdx);
    } while (BCM_ERR_BUSY == ret);

    BCM_GetTime(&endTime);
    totalTime = BCM_GetTimeAbsDiff(startTime,endTime);
    ULOG_INFO("\n Time: (%d,%d) \n",totalTime.s,totalTime.ns);

    if ((0UL < copiedSize) && (PP_DMA_TEST_MAX_BUF_SIZE >= copiedSize)) {
        if (0L != memcmp(inImage,outImage,copiedSize)) {
            ret = BCM_ERR_DATA_INTEG;
        } else {
            ret = BCM_ERR_OK;
        }
    }

exit:
    return ret;
}

TASK(PPDMATest)
{
    uint32_t i;
    int32_t ret;
    uint32_t yWidth[PP_DMA_NUM_TEST_CASES] = {128,1280,1920,1920*2,1920*4};
    uint32_t yHeight[PP_DMA_NUM_TEST_CASES] = {16,16,8,4,2};
    for (i = 0; i < PP_DMA_NUM_TEST_CASES; i++) {
        uint32_t tempChnWidth[3UL] = {yWidth[i],yWidth[i]/2UL,yWidth[i]/2UL};
        uint32_t tempChnHeight[3UL] = {yHeight[i],yHeight[i]/2UL,yHeight[i]/2UL};
        ret = PP_RunDMATest(3UL,tempChnWidth,tempChnHeight);
        if (BCM_ERR_OK != ret) {
            ppdma_qt2_result = BCM_ERR_DATA_INTEG;
        }
    }

    /* Test1: Configure and check for 3 buffers */
    if (BCM_AT_EXECUTING == ppdma_qt2_result) {
        ppdma_qt2_result = BCM_ERR_OK;
    }

    ULOG_INFO("**********%s copy completed (%d)***********\n",__func__, ppdma_qt2_result);

    (void)BCM_TerminateTask();
}

static volatile int32_t ppdma_qt3_result = BCM_AT_NOT_STARTED;
TASK(PPWDTest)
{
    int32_t ret;
    BCM_TimeType startTime,endTime,totalTime,requestTime,diffTime;

    /* Allocate core */
    ULOG_INFO("\n PPWDTest Activated \n");
    ret = PP_AllocCore(&CoreID);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("\n Error: %d \t line %d",ret,__LINE__);
        ppdma_qt3_result = ret;
    }
    if (0UL == CoreID) {
        ULOG_ERR("\nCoreID Error\t line %d",__LINE__);
        ppdma_qt3_result = BCM_ERR_DATA_INTEG;
    } else {
        ret = PP_UpdateClient(CoreID, PPWDHandler, PP_CLIENT_WD);
        if(BCM_ERR_OK != ret) {
            ULOG_ERR("\n Error: %d \t line %d",ret,__LINE__);
            ppdma_qt3_result = ret;
        }
    }

    /* Enable watch dog */
    if (BCM_AT_EXECUTING == ppdma_qt3_result) {
        requestTime.s = 0;
        requestTime.ns = 1000UL*1000UL;
        BCM_GetTime(&startTime);
        ret = PP_EnableWD(CoreID, 1000UL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("\n Error: %d \t line %d",ret,__LINE__);
            ppdma_qt3_result = ret;
        } else {
            (void)BCM_WaitEvent(TriggerEvent0);
            BCM_GetTime(&endTime);
            (void)BCM_ClearEvent(TriggerEvent0);
            totalTime = BCM_GetTimeAbsDiff(startTime,endTime);
            ULOG_INFO("\n Requested: 1000us Time: (%ds,%dns) \n",totalTime.s,totalTime.ns);
            diffTime = BCM_GetTimeAbsDiff(totalTime,requestTime);
            if ((0UL != diffTime.s) || (20000UL < diffTime.ns)) {
                ppdma_qt3_result = BCM_ERR_DATA_INTEG;
            } else {
                ppdma_qt3_result = BCM_ERR_OK;
            }
            {
                PP_ErrorStatusType errorStatus;
                ret = PP_GetErrorStatus(CoreID, &errorStatus);
                if (BCM_ERR_OK != ret) {
                    ppdma_qt3_result = ret;
                } else {
                    if (1UL != errorStatus.wdCount) {
                        ppdma_qt3_result = BCM_ERR_DATA_INTEG;
                    }
                }
            }

        }
    }
    ULOG_INFO("**********%s test completed (%d)***********\n",__func__, ppdma_qt3_result);

    (void)BCM_TerminateTask();
}
TASK(PPWDHandler)
{
    (void)BCM_WaitEvent(PP_WDEvent);
    (void)BCM_SetEvent(PPWDTest, TriggerEvent0);
    (void)BCM_ClearEvent(PP_WDEvent);
    (void)BCM_TerminateTask();
}
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;
    if (1UL == aIndex) {
        pplib_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("Test started\n");
        (void)BCM_ActivateTask(PPLibTestTask1);
        (void)BCM_ActivateTask(PPLibTestTask2);
    } else if (2UL == aIndex) {
        ppdma_qt2_result = BCM_AT_EXECUTING;
        (void)BCM_ActivateTask(PPDMATest);
    } else if (3UL == aIndex) {
        ppdma_qt3_result = BCM_AT_EXECUTING;
        (void)BCM_ActivateTask(PPWDTest);
        (void)BCM_ActivateTask(PPWDHandler);
    } else {
        ret = BCM_AT_NOT_AVAILABLE;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret;
    if (1UL == aIndex) {
        ret = pplib_qt1_result;
    } else if (2UL == aIndex) {
        ret = ppdma_qt2_result;
    } else if (3UL == aIndex) {
        ret = ppdma_qt3_result;
    } else {
        ret = BCM_AT_NOT_AVAILABLE;
    }
    return ret;
}

void ApplicationInit(void)
{
}
/** @} */
