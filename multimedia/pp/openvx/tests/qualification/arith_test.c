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
#include "arith_test_internal.h"

ARITH_TEST_SCALAR_U8_SCALAR_TYPE ArithTest_scalarU8 = 2;
ARITH_TEST_SCALAR_S16_SCALAR_TYPE ArithTest_scalarS16 = 2;

ARITH_TEST_SCALAR_MEAN_SCALAR_TYPE ArithTest_mean = 0;
ARITH_TEST_SCALAR_STDDEV_SCALAR_TYPE ArithTest_stddev = 0;

OpenVXLib_StorageInfo ArithTestImageInfo[] = {
[ARITH_TEST_TEMP_STORAGE_IDX].storageAddr = NULL,
[ARITH_TEST_TEMP_STORAGE_IDX].storageSize = ARITH_TEST_TEMP_STORAGE_SIZE,

[ARITH_TEST_IN1_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_IN1_IMG_IDX].storageSize = ARITH_TEST_IN1_IMG_SIZE,

[ARITH_TEST_IN2_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_IN2_IMG_IDX].storageSize = ARITH_TEST_IN2_IMG_SIZE,

[ARITH_TEST_OUT_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_OUT_IMG_IDX].storageSize = ARITH_TEST_OUT_IMG_SIZE,

[ARITH_TEST_IN1_S16_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_IN1_S16_IMG_IDX].storageSize = ARITH_TEST_IN1_S16_IMG_SIZE,

[ARITH_TEST_IN2_S16_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_IN2_S16_IMG_IDX].storageSize = ARITH_TEST_IN2_S16_IMG_SIZE,

[ARITH_TEST_OUT_S16_IMG_IDX].storageAddr = NULL,
[ARITH_TEST_OUT_S16_IMG_IDX].storageSize = ARITH_TEST_OUT_S16_IMG_SIZE,

[ARITH_TEST_SCALAR_U8_SCALAR_IDX].storageAddr = (uint8_t*)&ArithTest_scalarU8,
[ARITH_TEST_SCALAR_U8_SCALAR_IDX].storageSize = sizeof(ArithTest_scalarU8),

[ARITH_TEST_SCALAR_S16_SCALAR_IDX].storageAddr = (uint8_t*)&ArithTest_scalarS16,
[ARITH_TEST_SCALAR_S16_SCALAR_IDX].storageSize = sizeof(ArithTest_scalarS16),

[ARITH_TEST_SCALAR_MEAN_SCALAR_IDX].storageAddr = (uint8_t*)&ArithTest_mean,
[ARITH_TEST_SCALAR_MEAN_SCALAR_IDX].storageSize = sizeof(ArithTest_mean),

[ARITH_TEST_SCALAR_STDDEV_SCALAR_IDX].storageAddr = (uint8_t*)&ArithTest_stddev,
[ARITH_TEST_SCALAR_STDDEV_SCALAR_IDX].storageSize = sizeof(ArithTest_stddev),

};
const OpenVXLib_StorageInfo* ArithTest_GetImageInfo()
{
    return ArithTestImageInfo;
}
uint32_t ArithTest_GetNumImages()
{
    return ARITH_TEST_MAX_DATA_ENTRIES;
}

int32_t  ArithTest_SetMemory()
{
    uint8_t *tmpPtr;
    uint32_t i;

    for (i=0;i<ARITH_TEST_MAX_DATA_ENTRIES-4;i++){ /*SCALARs are allocated out*/
        tmpPtr = OpenVXTest_PoolMemAlloc(ArithTestImageInfo[i].storageSize );
        if (tmpPtr == NULL){
            return BCM_ERR_NOMEM;
        } else {
            ArithTestImageInfo[i].storageAddr = tmpPtr;
        }
    }
    return BCM_ERR_OK;
}

void ArithTest_FreeMemory()
{
    OpenVXTest_PoolFreeMem();
}



int32_t ArithTest_PrepareInput(uint32_t aGraphID)
{
    uint32_t i,j;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    uint8_t* arithTest_InputImage0 = ArithTestImageInfo[ARITH_TEST_IN1_IMG_IDX].storageAddr;
    uint8_t* arithTest_InputImage1 = ArithTestImageInfo[ARITH_TEST_IN2_IMG_IDX].storageAddr;
    uint8_t* arithTest_OutImage = ArithTestImageInfo[ARITH_TEST_OUT_IMG_IDX].storageAddr;
    uint8_t* arithTest_InputImage0_s16 = ArithTestImageInfo[ARITH_TEST_IN1_S16_IMG_IDX].storageAddr;
    uint8_t* arithTest_InputImage1_s16= ArithTestImageInfo[ARITH_TEST_IN2_S16_IMG_IDX].storageAddr;
    /* set the output image*/
    BCM_MemSet(&arithTest_OutImage[0], 0xFF, ARITH_TEST_OUT_IMG_SIZE);

    if ((0UL == aGraphID) || (2UL == aGraphID)) {

        /* set first image */
        for (i=0;i<720;i++) {
            for (j=0;j<1280;j++) {
                arithTest_InputImage0[i*1280+j] = i;
                arithTest_InputImage1[i*1280+j] = j;
            }
        }
        ret = BCM_ERR_OK;
    }

    if ((1UL == aGraphID) || (3UL == aGraphID)) {
        int16_t *in1;
        int16_t *in2;

        in1 = (int16_t*)arithTest_InputImage0_s16;
        in2 = (int16_t*)arithTest_InputImage1_s16;
        /* set first image */
        for (i=0;i<720;i++) {
            for (j=0;j<1280;j++) {
                in1[i*1280+j] = i;
                in2[i*1280+j] = j;
            }
        }
        ret = BCM_ERR_OK;
    }

    /* check for CRC */
    return ret;
}

uint32_t ArithTest_RefCRC[] =
{
0xb9976c82,
0x53d8d785,
0xffffffff,
0xffffffff
};
int32_t ArithTest_ValidateOutput(uint32_t aGraphID)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    uint32_t crc;
    uint8_t* arithTest_OutImage = ArithTestImageInfo[ARITH_TEST_OUT_IMG_IDX].storageAddr;

    if (4UL > aGraphID) {
        ret = BCM_ERR_OK;
        crc = BCM_CRC32(arithTest_OutImage,ARITH_TEST_OUT_IMG_SIZE,OPENVX_TEST_CRC32_POLY);
        if (ArithTest_RefCRC[aGraphID] != crc) {
            ULOG_ERR(" Arith Test Failed for Graph : %d\n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }

    if (0UL == aGraphID) {
        if ((19UL != ArithTest_stddev) || (120UL != ArithTest_mean)) {
            ULOG_ERR(" Arith Test mean and stddev check failed for Graph : %d \n",aGraphID);
            ret = BCM_ERR_DATA_INTEG;
        }
    }
    /* check for CRC */
    return ret;
}

OpenVXTestConfigType ArithTestConfig = {ArithTest_GetImageInfo, ArithTest_GetNumImages, ArithTest_PrepareInput, ArithTest_ValidateOutput, ArithTest_SetMemory, ArithTest_FreeMemory, 0xFF01, 4, ARITH_TEST_UNIQUE_ID};

OpenVXTestConfigType* ArithTest_GetTestConfig()
{
    return &ArithTestConfig;
}
