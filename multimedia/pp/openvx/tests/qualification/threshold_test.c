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
#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <compiler.h>
#include <bcm_err.h>
#include <openvx_lib.h>
#include <stdlib.h>
#include <string.h>
#include <ulog.h>
#include <cache.h>
#include <crc.h>
#include <bcm_utils.h>

#include "test_main.h"
#include "threshold_test_internal.h"

OpenVXLib_StorageInfo ThresholdTestImageInfo[] = {
[THRESHOLD_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[THRESHOLD_TEST_TEMP_STORAGE_IDX].storageSize = THRESHOLD_TEST_TEMP_STORAGE_SIZE,

[THRESHOLD_TEST_INU8_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_INU8_IMG_IDX].storageSize = THRESHOLD_TEST_INU8_IMG_SIZE,

[THRESHOLD_TEST_OUTU8_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_OUTU8_IMG_IDX].storageSize = THRESHOLD_TEST_OUTU8_IMG_SIZE,

[THRESHOLD_TEST_INS16_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_INS16_IMG_IDX].storageSize = THRESHOLD_TEST_INS16_IMG_SIZE,

[THRESHOLD_TEST_OUTS16_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_OUTS16_IMG_IDX].storageSize = THRESHOLD_TEST_OUTS16_IMG_SIZE,

[THRESHOLD_TEST_INS32_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_INS32_IMG_IDX].storageSize = THRESHOLD_TEST_INS32_IMG_SIZE,

[THRESHOLD_TEST_OUTS32_IMG_IDX].storageAddr = NULL,
[THRESHOLD_TEST_OUTS32_IMG_IDX].storageSize = THRESHOLD_TEST_OUTS32_IMG_SIZE,
};
int32_t ThresholdTest_SetMemory()
{
    uint8_t *tmpPtr;
    uint32_t i;

    for (i=0;i<THRESHOLD_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(ThresholdTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            ThresholdTestImageInfo[i].storageAddr = tmpPtr;
        }

    }
    return BCM_ERR_OK;

}

void ThresholdTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}

const OpenVXLib_StorageInfo* ThresholdTest_GetImageInfo()
{
    return ThresholdTestImageInfo;
}
uint32_t ThresholdTest_GetNumImages()
{
    return THRESHOLD_TEST_MAX_DATA_ENTRIES;
}

int32_t ThresholdTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    int16_t *s16In;
    int32_t *s32In;
    uint8_t *thresholdTest_InputImageU8 = ThresholdTestImageInfo[THRESHOLD_TEST_INU8_IMG_IDX].storageAddr;
    uint8_t *thresholdTest_InputImageS16 = ThresholdTestImageInfo[THRESHOLD_TEST_INS16_IMG_IDX].storageAddr;
    uint8_t *thresholdTest_InputImageS32 = ThresholdTestImageInfo[THRESHOLD_TEST_INS32_IMG_IDX].storageAddr;
    uint8_t *thresholdTest_OutImageU8 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTU8_IMG_IDX].storageAddr ;
    uint8_t *thresholdTest_OutImageS16 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTS16_IMG_IDX].storageAddr ;
    uint8_t *thresholdTest_OutImageS32 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTS32_IMG_IDX].storageAddr;
    s16In = (int16_t*)thresholdTest_InputImageS16;
    s32In = (int32_t*)thresholdTest_InputImageS32;

    if (0UL == aGraphID) {
        /* set first image */
        /* set first image */
        for (i=0;i<720;i++) {
            for (j=0;j<1280;j++) {
                thresholdTest_InputImageU8[i*1280+j] = i*j;
                s16In[i*1280+j] = i*j;
                s32In[i*1280+j] = (i+j) + (i*j);
            }
        }
        BCM_MemSet(thresholdTest_OutImageU8, 0, THRESHOLD_TEST_OUTU8_IMG_SIZE);
        BCM_MemSet(thresholdTest_OutImageS16, 0, THRESHOLD_TEST_OUTS16_IMG_SIZE);
        BCM_MemSet(thresholdTest_OutImageS32, 0, THRESHOLD_TEST_OUTS32_IMG_SIZE);
    }
    return BCM_ERR_OK;
}

int32_t ThresholdTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t *thresholdTest_OutImageU8 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTU8_IMG_IDX].storageAddr ;
    uint8_t *thresholdTest_OutImageS16 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTS16_IMG_IDX].storageAddr ;
    uint8_t *thresholdTest_OutImageS32 = ThresholdTestImageInfo[THRESHOLD_TEST_OUTS32_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;

        crc = BCM_CRC32(thresholdTest_OutImageU8,THRESHOLD_TEST_OUTU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x35de2f88UL != crc) {
            ULOG_ERR(" Threshold Test Failed in  ThresholdTest_OutImageU8 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(thresholdTest_OutImageS16,THRESHOLD_TEST_OUTS16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xc1ea8b4aUL != crc) {
            ULOG_ERR(" Threshold Test Failed in ThresholdTest_OutImageS16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }

        crc = BCM_CRC32(thresholdTest_OutImageS32,THRESHOLD_TEST_OUTS32_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xe96162faUL != crc) {
            ULOG_ERR(" Threshold Test Failed in ThresholdTest_OutImageS32 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType ThresholdTestConfig = {ThresholdTest_GetImageInfo, ThresholdTest_GetNumImages, ThresholdTest_PrepareInput, ThresholdTest_ValidateOutput, ThresholdTest_SetMemory, ThresholdTest_FreeMemory, 0xFF08, 1, THRESHOLD_TEST_UNIQUE_ID};

OpenVXTestConfigType* ThresholdTest_GetTestConfig()
{
    return &ThresholdTestConfig;
}

