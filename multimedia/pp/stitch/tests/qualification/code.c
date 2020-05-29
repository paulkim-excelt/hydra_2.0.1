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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <stitch_lib.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_err.h>

#include <crc.h>

#include "call_stitch.h"

#include <ulog.h>
#include <osil/bcm_osil.h>

static volatile int32_t stitchlib_qt1_result = BCM_AT_NOT_STARTED;

#define FILLBUFFER_SIZE         (1488)
#define STITCH_TEST_CRC32_POLY  (0x04C11DB7UL)

extern uint8_t InImageY[];
extern uint8_t InImageU[];
extern uint8_t InImageV[];

uint8_t COMP_SECTION(".pp.uncached.test") OutImage[1280*16*3/2];

#define CHECK_ERR(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); stitchlib_qt1_result = ret;}

uint32_t RefCrc = 0x7fe1e0e1;

uint32_t OffsetY[4] = {0,640,(1280*8),(1280*8)+640};
uint32_t OffsetC[4] = {0,320,(640*4),(640*4)+320};
BCM_TimeType GStartTime,GEndTime;
BCM_TimeType GTimeDiff[4];
uint32_t GTotalTime;

TASK(STCHLibTest)
{
    uint32_t i;
    int32_t ret;
    uint32_t crc;

    STCHLib_Context context;
    STCHLib_YUVBuffer inYuvBuffer;
    STCHLib_YUVBuffer outYuvBuffer;

    BCM_EventMaskType mask;
    uint32_t flags;

    context.state = STCH_LIB_STATE_RESET;
    ret = STCHLib_Init(&context);
    CHECK_ERR(ret,__FILE__, __LINE__);

    ret = STCHLib_Boot(&context,
                       0xEE10,
                       call_stitch,
                       call_stitch_size);
    CHECK_ERR(ret,__FILE__, __LINE__);

    inYuvBuffer.lumaStride = 1280;
    inYuvBuffer.chromaStride = 640;
    inYuvBuffer.lumaWidth = 1280;
    inYuvBuffer.lumaHeight = 16;
    inYuvBuffer.chromaWidth = 640;
    inYuvBuffer.chromaHeight = 8;

    outYuvBuffer.lumaStride = 1280;
    outYuvBuffer.chromaStride = 640;

    for (i=0;i<4;i++) {
        inYuvBuffer.lumaAddr = &InImageY[0];
        inYuvBuffer.cbAddr = &InImageU[0];
        inYuvBuffer.crAddr = &InImageV[0];

        outYuvBuffer.lumaAddr = &OutImage[0 + OffsetY[i]];
        outYuvBuffer.cbAddr = &OutImage[(1280*16) + OffsetC[i]];
        outYuvBuffer.crAddr = &OutImage[(1280*16) + (640*8) + OffsetC[i]];

        ret = STCHLib_DecimateCopy(&context,&inYuvBuffer,&outYuvBuffer);
        BCM_GetTime(&GStartTime);
        CHECK_ERR(ret,__FILE__, __LINE__);
        BCM_WaitEvent(PP_GPEvent);
        BCM_GetEvent(STCHLibTest,&mask);
        BCM_ClearEvent(mask);
        do {
            flags = STCHLib_Process(&context);
            if (flags & STCH_LIB_DATA_PROCESSED) {
                BCM_GetTime(&GEndTime);
                GTimeDiff[i] = BCM_GetTimeAbsDiff(GStartTime,GEndTime);

                ULOG_INFO("Data copy done \n");
            }
           ULOG_INFO("STCHLib_Process: %x\n", flags);

        } while(flags != 0);
    }
    GTotalTime = 0;
    for(i=0;i<4;i++){
        ULOG_ERR("%d port MB Row Time (%ds, %dns)\n",i,GTimeDiff[i].s,GTimeDiff[i].ns);
        GTotalTime += GTimeDiff[i].ns;
    }
    ULOG_ERR("Averate time per MB row %dns\n",GTotalTime/(4));
    STCHLib_DeInit(&context);
    crc = BCM_CRC32(OutImage,((1280*16*3)/2),STITCH_TEST_CRC32_POLY);
    if (crc != RefCrc) {
        ULOG_ERR("Output CRC Check failed 0x%08x\n",crc);
        stitchlib_qt1_result = BCM_ERR_DATA_INTEG;
    }
    if (BCM_AT_EXECUTING == stitchlib_qt1_result) {
        stitchlib_qt1_result = BCM_ERR_OK;
    }
    ULOG_INFO("**********%s completed ***********\n",__func__);

    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        stitchlib_qt1_result = BCM_AT_EXECUTING;
        ULOG_INFO("\n Test started");
        BCM_ActivateTask(STCHLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = stitchlib_qt1_result;
    }
    return ret;
}

void ApplicationInit()
{
}
