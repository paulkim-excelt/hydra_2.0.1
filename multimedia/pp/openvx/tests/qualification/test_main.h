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

#define OPENVX_TEST_CRC32_POLY         (0x04C11DB7UL)


typedef struct _OpenVXTestConfigType {
    const OpenVXLib_StorageInfo* (*GetImageInfo)();
    uint32_t (*GetNumImages)();
    int32_t (*PrepareInput)(uint32_t);
    int32_t (*ValidateOutput)(uint32_t);
    int32_t (*setMemory)();
    void (*freeMemory)();
    uint16_t startGraphID;
    uint16_t numGraphs;
    uint32_t uniqueID;
} OpenVXTestConfigType;

OpenVXTestConfigType* ScaleTest_GetTestConfig();
OpenVXTestConfigType* ArithTest_GetTestConfig();
OpenVXTestConfigType* FilterTest_GetTestConfig();
OpenVXTestConfigType* HistTest_GetTestConfig();
OpenVXTestConfigType* ThresholdTest_GetTestConfig();
OpenVXTestConfigType* MorphTest_GetTestConfig();
OpenVXTestConfigType* ColorConvertTest_GetTestConfig();
OpenVXTestConfigType* BitdepthConvTest_GetTestConfig();
OpenVXTestConfigType* PedDetectTest_GetTestConfig();
OpenVXTestConfigType* OptFlowTest_GetTestConfig();
OpenVXTestConfigType* IntegralImgTest_GetTestConfig();
OpenVXTestConfigType* AccumulateImgTest_GetTestConfig();
OpenVXTestConfigType* GaussianPyramidTest_GetTestConfig();
OpenVXTestConfigType* HalfScaleGaussianTest_GetTestConfig();
OpenVXTestConfigType* LaplacianPyramidTest_GetTestConfig();
OpenVXTestConfigType* FaceDetectTest_GetTestConfig();
OpenVXTestConfigType* CropnScaleTest_GetTestConfig();

int32_t OpenVXTest_PTMLoadData(uint16_t id,uint8_t* aBuffer, uint32_t aMaxSize, uint32_t* aReadSize);

uint8_t* OpenVXTest_PoolMemAlloc();
void OpenVXTest_PoolFreeMem();

