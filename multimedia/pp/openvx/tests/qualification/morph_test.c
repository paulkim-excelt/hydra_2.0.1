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
#include "morph_test_internal.h"

OpenVXLib_StorageInfo MorphTestImageInfo[] = {
[MORPH_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[MORPH_TEST_TEMP_STORAGE_IDX].storageSize = MORPH_TEST_TEMP_STORAGE_SIZE,

[MORPH_TEST_INU8_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_INU8_IMG_IDX].storageSize = MORPH_TEST_INU8_IMG_SIZE,

[MORPH_TEST_OUTU8_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_OUTU8_IMG_IDX].storageSize = MORPH_TEST_OUTU8_IMG_SIZE,

[MORPH_TEST_INU16_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_INU16_IMG_IDX].storageSize = MORPH_TEST_INU16_IMG_SIZE,

[MORPH_TEST_OUTU16_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_OUTU16_IMG_IDX].storageSize = MORPH_TEST_OUTU16_IMG_SIZE,

[MORPH_TEST_IN1S32_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_IN1S32_IMG_IDX].storageSize = MORPH_TEST_IN1S32_IMG_SIZE,

[MORPH_TEST_IN2S32_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_IN2S32_IMG_IDX].storageSize = MORPH_TEST_IN2S32_IMG_SIZE,

[MORPH_TEST_OUTS32_IMG_IDX].storageAddr = NULL,
[MORPH_TEST_OUTS32_IMG_IDX].storageSize = MORPH_TEST_OUTS32_IMG_SIZE,
};

int32_t MorphTest_SetMemory()
{
    uint32_t i;
    uint8_t *tmpPtr;

    for (i=0;i<MORPH_TEST_MAX_DATA_ENTRIES;i++){
            tmpPtr = OpenVXTest_PoolMemAlloc(MorphTestImageInfo[i].storageSize);
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            MorphTestImageInfo[i].storageAddr = tmpPtr;
        }
    }

    return BCM_ERR_OK;
}
void MorphTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}
const OpenVXLib_StorageInfo* MorphTest_GetImageInfo()
{
    return MorphTestImageInfo;
}
uint32_t MorphTest_GetNumImages()
{
    return MORPH_TEST_MAX_DATA_ENTRIES;
}

int32_t MorphTest_PrepareInput(uint32_t aGraphID)
{
    int32_t ret;
    uint32_t i,j;
    uint16_t *u16In;
    int32_t *s32In;
    uint32_t imgSz = 0UL;

    uint8_t *morphTest_InputImageU8 = MorphTestImageInfo[MORPH_TEST_INU8_IMG_IDX].storageAddr;
    uint8_t *morphTest_InputImageU16 = MorphTestImageInfo[MORPH_TEST_INU16_IMG_IDX].storageAddr;
    uint8_t *morphTest_In1ImageS32 = MorphTestImageInfo[MORPH_TEST_IN1S32_IMG_IDX].storageAddr;
    uint8_t *morphTest_OutImageU8 = MorphTestImageInfo[MORPH_TEST_OUTU8_IMG_IDX].storageAddr;
    uint8_t *morphTest_OutImageU16 = MorphTestImageInfo[MORPH_TEST_OUTU16_IMG_IDX].storageAddr;
    uint8_t *morphTest_OutImageS32 = MorphTestImageInfo[MORPH_TEST_OUTS32_IMG_IDX].storageAddr;

    u16In = (uint16_t*)morphTest_InputImageU16;
    s32In = (int32_t*)morphTest_In1ImageS32;

    ret = OpenVXTest_PTMLoadData((uint16_t)0xFF0C, morphTest_InputImageU8, MORPH_TEST_INU8_IMG_SIZE, &imgSz);

    if (MORPH_TEST_INU8_IMG_SIZE != imgSz) {
        ret = BCM_ERR_UNKNOWN;
    }

    if (0UL == aGraphID) {
        /* BCM_MemSet output images */
        BCM_MemSet(morphTest_OutImageU8, 0, MORPH_TEST_OUTU8_IMG_SIZE);
    }

    if (1UL == aGraphID) {
        /* BCM_MemSet output images */
        BCM_MemSet(morphTest_OutImageU16, 0, MORPH_TEST_OUTU16_IMG_SIZE);

        /* load the images */
        for (i=0;i<256;i++) {
            for (j=0;j<256;j++) {
                u16In[i*256+j] = morphTest_InputImageU8[i*256+j] * (uint16_t)(200U);
            }
        }
    }

    if (2UL == aGraphID) {

        /* BCM_MemSet output images */
        BCM_MemSet(morphTest_OutImageS32, 0, MORPH_TEST_OUTS32_IMG_SIZE);

        /* load the images */
        for (i=0;i<256;i++) {
            for (j=0;j<256;j++) {
                u16In[i*256+j] = morphTest_InputImageU8[i*256+j] * (uint16_t)(200U);
                s32In[i*256+j] = u16In[i*256+j] * (int32_t)(100UL);
            }
        }
    }

    if (2UL < aGraphID) {
        ret = BCM_ERR_INVAL_PARAMS;
    }

    return ret;
}

int32_t MorphTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t *morphTest_OutImageU8 = MorphTestImageInfo[MORPH_TEST_OUTU8_IMG_IDX].storageAddr;
    uint8_t *morphTest_OutImageU16 = MorphTestImageInfo[MORPH_TEST_OUTU16_IMG_IDX].storageAddr;
    uint8_t *morphTest_OutImageS32 = MorphTestImageInfo[MORPH_TEST_OUTS32_IMG_IDX].storageAddr;

    if (0UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(morphTest_OutImageU8,MORPH_TEST_OUTU8_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x9e28d810UL != crc) {
            ULOG_ERR(" Morph Test Failed in  morphTest_OutImageU8 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    if (1UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(morphTest_OutImageU16,MORPH_TEST_OUTU16_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x360afb9cUL != crc) {
            ULOG_ERR(" Morph Test Failed in morphTest_OutImageU16 for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    if (2UL == aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(morphTest_OutImageS32,MORPH_TEST_OUTS32_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (0x61ba12a5UL != crc) {
            ULOG_ERR(" Morph Test Failed in morphTest_OutImageS32 for Graph : %d\t %x\n",aGraphID, crc);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    /* check for CRC */
    return ret;
}

OpenVXTestConfigType MorphTestConfig = {MorphTest_GetImageInfo, MorphTest_GetNumImages, MorphTest_PrepareInput, MorphTest_ValidateOutput, MorphTest_SetMemory, MorphTest_FreeMemory, 0xFF09, 3, MORPH_TEST_UNIQUE_ID};

OpenVXTestConfigType* MorphTest_GetTestConfig()
{
    return &MorphTestConfig;
}

