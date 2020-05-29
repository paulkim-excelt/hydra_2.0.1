/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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

#define GetModuleLogLevel()     ULOG_LVL_VERBOSE

#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <pp.h>
#include <ulog.h>
#include <console.h>
#include <cache.h>
#include <bcm_utils.h>
#include <bcm_test.h>
#include <osil/bcm_osil.h>

#include "call_vmp_add42.h"


/* Test events mapping */
#define TEST_EVENT_START      SystemEvent0
#define TEST_EVENT_STOP       SystemEvent1

#define TESTALGO_MAGICID_READY  0x52454459UL
#define TESTALGO_MAGICID_START  0x53545254UL
#define TESTALGO_MAGICID_DONE   0x444F4E45UL


static volatile int32_t add_example_qt1_result = BCM_AT_NOT_STARTED;

uint8_t dmaBuf[640*8*2];
/** @brief Main task for pp driver test
 *
 * Main task for pp driver test
 *
 * @return  void
 */
TASK(AddExampeTestTask)
{
    uint32_t testCoreId = 0UL;
    int32_t ret;
    uint32_t payLoad[PP_MSG_PAYLOAD_SIZE/sizeof(uint32_t)];
    uint32_t msgType = 0UL;

    uint32_t inAddr;
    uint32_t outAddr;
    uint32_t width;
    uint32_t height;

    BCM_EventMaskType mask;

    inAddr = (uint32_t)dmaBuf; /* 8 lines of image */
    outAddr = (uint32_t)(dmaBuf + (640*8));
    width = 640;
    height = 8;

    /* Fill test value - just for 4 bytes location
	vMP test Algorithm would add 42 (0x2a) to each byte value */
    *((uint32_t *) inAddr) = 0x30405060;


    ULOG_INFO ("%s *inAddr (input data quick check): %x \n",__func__, *((uint32_t *)inAddr));

    ret = PP_AllocCore(&testCoreId);

    if( ret != BCM_ERR_OK) {
        ULOG_ERR("TestBootCore_1 Failed %d\n",ret );
        add_example_qt1_result = BCM_ERR_UNKNOWN;
    }
    ULOG_ERR("allocated %x\n", testCoreId);

    ret = PP_BootCore(testCoreId, (uint8_t*)call_vmp_add42, call_vmp_add42_size);
    if (ret == BCM_ERR_OK) {
        payLoad[0] = 0xA7A7A7A7; /*Message goes here*/

#if 0
        /* Need to wait for this, only if the algorithm is sending explicit ready message */
        ULOG_ERR("waiting for message TESTALGO_MAGICID_READY from vMP ...\n");
        BCM_WaitEvent(PP_GPEvent);
        BCM_GetEvent(PPLibTest,&mask);
        if (mask & PP_GPEvent) {
            BCM_ClearEvent(PP_GPEvent);
        }
        ULOG_ERR("Got event, Reading message  ...\n");
        ret = PP_RecvMsg(testCoreId, &msgType, (char*)payLoad);
        if (ret != BCM_ERR_OK) {
            ULOG_ERR ("%s Receive Message failed \n",__func__);
        }
        if (TESTALGO_MAGICID_READY != BE2Host32(msgType)) {
            ULOG_ERR ("%s TESTALGO_MAGICID_READY not received!!!  \n",__func__);
        }
#endif
        /* fill payload; consider endian aspect */
        payLoad[0UL] = Host2BE32((uint32_t)inAddr);
        payLoad[1UL] = Host2BE32((uint32_t)outAddr);
        payLoad[2UL] = Host2BE32((uint32_t)width);
        payLoad[3UL] = Host2BE32((uint32_t)height);

        /* flush this buffer - just to flush test pattern written */
        DCACHE_CleanRange((uint32_t)inAddr,
                ((640* sizeof(uint32_t))));

        ULOG_INFO ("%s Sending Message TESTALGO_MAGICID_START to vMP ... \n",__func__);
        ret = PP_SendMsg(testCoreId, Host2BE32(TESTALGO_MAGICID_START), (char*)payLoad);
        if (ret != BCM_ERR_OK) {
            ULOG_ERR ("%s Sending Message failed \n",__func__);
        }
        ULOG_INFO("waiting for message TESTALGO_MAGICID_DONE from vMP ...\n");
        BCM_WaitEvent(PP_GPEvent);
        BCM_GetEvent(AddExampeTestTask,&mask);
        ULOG_INFO("received mailbox even!t\n");
        if (mask & PP_GPEvent) {
            BCM_ClearEvent(PP_GPEvent);
        }
        ULOG_INFO("reading mailbox payload...\n");
        ret = PP_RecvMsg(testCoreId, &msgType, (char*)payLoad);
        if (ret != BCM_ERR_OK) {
            ULOG_ERR ("%s Receive Message failed \n",__func__);
        }

        ULOG_INFO("checking message...%x \n", msgType);
        if (TESTALGO_MAGICID_DONE == msgType){
            ULOG_ERR ("%s Receive Message TESTALGO_MAGICID_DONE \n",__func__);
            ULOG_ERR ("%s *outAddr (output quick check): %x \n",__func__, *((uint32_t *)outAddr));
        }

        ret = PP_StopCore(testCoreId);
        ret = PP_FreeCore(testCoreId);

        if (*((uint32_t *)outAddr) == (0x30405060 + 0x2a2a2a2a)) {
            add_example_qt1_result = BCM_ERR_OK;
        } else {
            add_example_qt1_result = BCM_ERR_UNKNOWN;
        }

    } else {
        ULOG_ERR("%s No Free Core \n",__func__);
        add_example_qt1_result = BCM_ERR_UNKNOWN;
    }
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        add_example_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("\n Test started");
        BCM_ActivateTask(AddExampeTestTask);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = add_example_qt1_result;
    }
    return ret;
}


void ApplicationInit()
{
}
