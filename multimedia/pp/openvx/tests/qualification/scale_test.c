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

#include "test_main.h"
#include "scale_test_internal.h"

OpenVXLib_StorageInfo ScaleTestImageInfo[] = {
[SCALE_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[SCALE_TEST_TEMP_STORAGE_IDX].storageSize = SCALE_TEST_TEMP_STORAGE_SIZE,

[SCALE_TEST_IN_IMG_IDX].storageAddr = NULL,
[SCALE_TEST_IN_IMG_IDX].storageSize = SCALE_TEST_IN_IMG_SIZE,

[SCALE_TEST_OUT_IMG_IDX].storageAddr = NULL,
[SCALE_TEST_OUT_IMG_IDX].storageSize = SCALE_TEST_OUT_IMG_SIZE,

[SCALE_TEST_IN1_IMG_IDX].storageAddr = NULL,
[SCALE_TEST_IN1_IMG_IDX].storageSize = SCALE_TEST_IN1_IMG_SIZE,

[SCALE_TEST_OUT1_IMG_IDX].storageAddr = NULL,
[SCALE_TEST_OUT1_IMG_IDX].storageSize = SCALE_TEST_OUT1_IMG_SIZE,

};

int32_t ScaleTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;
    for (i=0;i<SCALE_TEST_MAX_DATA_ENTRIES;i++){
        tmpPtr = OpenVXTest_PoolMemAlloc(ScaleTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            ScaleTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;

}
void ScaleTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}
const OpenVXLib_StorageInfo* ScaleTest_GetImageInfo()
{
    return ScaleTestImageInfo;
}
uint32_t ScaleTest_GetNumImages()
{
    return SCALE_TEST_MAX_DATA_ENTRIES;
}

int32_t ScaleTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    uint32_t imgSz;
    int32_t ret = BCM_ERR_OK;
    uint8_t *scaleTest_InImage = ScaleTestImageInfo[SCALE_TEST_IN_IMG_IDX].storageAddr;
    uint8_t *scaleTest_OutImage = ScaleTestImageInfo[SCALE_TEST_OUT_IMG_IDX].storageAddr;
    uint8_t *scaleTest_In1Image = ScaleTestImageInfo[SCALE_TEST_IN1_IMG_IDX].storageAddr;
    uint8_t *scaleTest_Out1Image = ScaleTestImageInfo[SCALE_TEST_OUT1_IMG_IDX].storageAddr;
    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF0C, scaleTest_In1Image, SCALE_TEST_IN1_IMG_SIZE, &imgSz);
    if (SCALE_TEST_IN1_IMG_SIZE != imgSz) {
        ULOG_ERR(" loading image fails : %x\n",imgSz);
        ret = BCM_ERR_UNKNOWN;
    }

    if (0UL == aGraphID) {
        /* set first image */
        for (i=0;i<720;i++) {
            for (j=0;j<1280;j++) {
                scaleTest_InImage[i*1280+j] = i;
            }
        }
        /* clear output image to take care of stide */
        for (i=0;i<540;i++) {
            for (j=0;j<1024;j++) {
                scaleTest_OutImage[i*1024+j] = 0;
            }
        }
        for (i=0;i<480;i++) {
            for (j=0;j<500;j++) {
                scaleTest_Out1Image[i*500+j] = 0;
            }
        }
    }
    return ret;
}

int32_t ScaleTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t i,j;
    uint32_t crc;
    uint8_t *scaleTest_OutImage = ScaleTestImageInfo[SCALE_TEST_OUT_IMG_IDX].storageAddr;
    uint8_t *scaleTest_Out1Image = ScaleTestImageInfo[SCALE_TEST_OUT1_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        /* set the stride to zeros */
         for (i=0;i<540;i++) {
            for (j=960;j<1024;j++) {
                scaleTest_OutImage[i*1024+j] = 0;
            }
        }
        for (i=0;i<480;i++) {
            for (j=500;j<512;j++) {
                scaleTest_Out1Image[i*512+j] = 0;
            }
        }
        crc = BCM_CRC32(scaleTest_OutImage,SCALE_TEST_OUT_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x65E42DDFUL != crc) {
            ULOG_ERR(" Scale Test Failed for downscaling Graph : %x\n",crc );
            ret = BCM_ERR_DATA_INTEG;
        }
        crc = BCM_CRC32(scaleTest_Out1Image,SCALE_TEST_OUT1_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0xECEF7791UL != crc) {
            ULOG_ERR(" Scale Test Failed for upscaling Graph : %x\n",crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType ScaleTestConfig = {ScaleTest_GetImageInfo, ScaleTest_GetNumImages, ScaleTest_PrepareInput, ScaleTest_ValidateOutput, ScaleTest_SetMemory, ScaleTest_FreeMemory, 0xFF00, 1, SCALE_TEST_UNIQUE_ID};

OpenVXTestConfigType* ScaleTest_GetTestConfig()
{
    return &ScaleTestConfig;
}

