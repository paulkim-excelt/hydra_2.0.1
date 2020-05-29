/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

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
File Name:  isp_hw_config_image.c
Description: This file implements isp hardware config image dump.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "cam_mode_image.h"
#include "isp_filter.h"
#include "isp_regdump_interface.h"
#include "image_to_array.h"
#include "isp_hw_config_image.h"
#include "isp_image.h"

/* Macros */
#define ISP_TCB_SIZE    (112UL)
#define ISP_APB_BASE    (0x40090000UL)
#define ISP_MAX_OFFSET  (0x00004D0CUL)

/* II_DPCM */
/* Byte swap for RAW16 CSI issue */
#define II_DPCM_OFFSET    (0x0000010cUL)
#define II_DPCM_BS_MASK   (0x00000002UL)
#define II_DPCM_BS_SHIFT  (1UL)

/* Error log */
#define DEBUG_ERR printf

/* Externs */
/* ISP data filled from offline utitlity */
extern ISPConfigData gISPCfgData;
/* byte swap enable as set from unicam driver */
extern uint32_t gISPByteSwapEnable;

/* Globals */
static ISP_ImageType gIspImageData;
static uint32_t gDataOfssetIndex = 0UL;

typedef struct _ISP_BlockEnable {
    uint32_t bayer;
    uint32_t ycbcr;
} ISP_BlockEnable;

/******************************************************************************
FUNCTION NAME: BlockConfigDump

ARGUMENTS: void

DESCRIPTION: This function dumps ISP block reg list.

RETURNS: BCM_ERR_OK

******************************************************************************/
static int32_t BlockConfigDump(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t addrRange = 0UL;
    uint32_t startOffset = 0UL;
    int32_t i;
    int32_t j;

    for (i = 0; i < (sizeof(gIspRegFilter)/sizeof(RegFilter)); i++) {
#if CHIP_VERSION_B0
        gIspImageData.cfgs[i].version = ISP_CFG_VERSION_B0;
#else
        gIspImageData.cfgs[i].version = ISP_CFG_VERSION_COMMON;
#endif /* CHIP_VERSION_B0 */
        gIspImageData.cfgs[i].pass = ISP_CFG_PASS_COMMON;
        gIspImageData.cfgs[i].type = ISP_CFG_BLK;
        gIspImageData.cfgs[i].deviceOffset = (gIspRegFilter[i].startAddr & OFFSET_MASK);
        gIspImageData.cfgs[i].dataOffset = gDataOfssetIndex;
        addrRange = ((gIspRegFilter[i].endAddr - gIspRegFilter[i].startAddr)/4) + 1;
        gIspImageData.cfgs[i].count = addrRange;

        startOffset = ((gIspRegFilter[i].startAddr & OFFSET_MASK) >> 2);

        for (j = 0; j < addrRange; j++) {
            gIspImageData.ispCfgData[gDataOfssetIndex] = gISPCfgData.IspBlockReg[startOffset + j].data;
            gDataOfssetIndex++;
        }
    }
    return ret;
}

int32_t ISP_HW_EnableInputByteSwap(ISP_ImageType *aImg)
{
    int32_t ret = BCM_ERR_UNKNOWN;
    uint32_t deviceOffset;
    uint32_t i, j;

    for (i = 0UL; (i < aImg->imgHdr.numConfigs); i++) {
        for (j = 0UL; (j < aImg->cfgs[i].count); j++) {
            deviceOffset = (aImg->cfgs[i].deviceOffset);
            deviceOffset += (j * 4UL);
            if (II_DPCM_OFFSET == deviceOffset) {
                aImg->ispCfgData[aImg->cfgs[i].dataOffset + j] |= II_DPCM_BS_MASK;
                ret = BCM_ERR_OK;
                break;
            }
        }
    }

    return ret;
}

/******************************************************************************
FUNCTION NAME: ISPHwConfigWriter

ARGUMENTS: None

DESCRIPTION: This function dumps ISP hardware config image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ISPHwConfigWriter(void)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp = NULL;
    int32_t i;
    uint32_t tcbIndex = 0UL;

    memset(&gIspImageData, 0, sizeof(ISP_ImageType));

    /* Open image file */
    fp = fopen("isp.img", "wb+");
    if (NULL == fp)
    {
        DEBUG_ERR("Error: opening file\n");
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Main Image Header */
    gIspImageData.imgHdr.commonHdr.cfgType = ISYS_CONFIG_ISP;
    gIspImageData.imgHdr.commonHdr.cfgMagic = ISP_MAGIC;
    gIspImageData.imgHdr.numConfigs = sizeof(gIspRegFilter)/sizeof(RegFilter) + 1;
    gIspImageData.imgHdr.passEnable = ISYS_PASS_ENABLE_HV;

    if (gIspImageData.imgHdr.numConfigs > ISP_MAX_CONFIGS) {
        DEBUG_ERR("%s: Error Reached max configs %d\n",
                        __func__, gIspImageData.imgHdr.numConfigs);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Dump ISP Block Config */
    ret = BlockConfigDump();
    if (BCM_ERR_OK != ret) {
        DEBUG_ERR("%s: Error: in block reg write\n", __func__);
        goto exit;
    }

    /* Tcb Index */
    tcbIndex = (sizeof(gIspRegFilter)/sizeof(RegFilter));

    /* Tcb config */
#if CHIP_VERSION_B0
    gIspImageData.cfgs[tcbIndex].version = ISP_CFG_VERSION_B0;
#else
    gIspImageData.cfgs[tcbIndex].version = ISP_CFG_VERSION_COMMON;
#endif /* CHIP_VERSION_B0 */
    gIspImageData.cfgs[tcbIndex].pass = ISP_CFG_PASS_COMMON;
    gIspImageData.cfgs[tcbIndex].type = ISP_CFG_TCB;
    gIspImageData.cfgs[tcbIndex].deviceOffset = 0x0;
    gIspImageData.cfgs[tcbIndex].dataOffset = gDataOfssetIndex;
    gIspImageData.cfgs[tcbIndex].count = TCB_MAX_DATA;

    /* Fills tcb data */
    for (i = 0; i < TCB_MAX_DATA; i++) {
        gIspImageData.ispCfgData[gDataOfssetIndex] = gISPCfgData.TcbData[i];
        gDataOfssetIndex++;
    }

    if (gDataOfssetIndex > ISP_MAX_CONFIG_SIZE) {
        DEBUG_ERR("%s: Error Reached max config size %d\n", __func__, gDataOfssetIndex);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Enable Byteswap if reqired */
    if (0UL != gISPByteSwapEnable) {
        ret = ISP_HW_EnableInputByteSwap(&gIspImageData);
        if (BCM_ERR_OK != ret) {
            DEBUG_ERR("%s: ISP_HW_EnableInputByteSwap failed\n", __func__);
            goto exit;
        }
    }

    /* Writes to file */
    fwrite(&gIspImageData, 1, sizeof(gIspImageData), fp);

    /* Write to C file */
    ImageToArray("isp_image", (unsigned char *)&gIspImageData, sizeof(gIspImageData));
exit:
    if (NULL != fp)
        fclose(fp);
    return ret;
}

/******************************************************************************
FUNCTION NAME: ISPHwConfigReader

ARGUMENTS: None

DESCRIPTION: This function reads ISP hardware config image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ISPHwConfigReader(void *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t addr = 0UL;
    uint32_t i = 0UL;
    uint32_t j = 0UL;
    FILE *fpHV = NULL;
    FILE *fpCV = NULL;
    ISP_ImageType *ispImg = NULL;
    uint8_t skipConfig = 0U;
    uint8_t haveCV;

    /* Check Parameter */
    if (NULL == aCfg) {
        DEBUG_ERR("%s: Null Config Pointer\n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    ispImg = (ISP_ImageType *)aCfg;

    haveCV = (0UL != (ispImg->imgHdr.passEnable & ISYS_PASS_ENABLE_CV)) ? 1U : 0U;

    /* OutPut file */
    fpHV = fopen("isp_config_b0_hv.txt", "w+");
    if (1U == haveCV) {
        fpCV = fopen("isp_config_b0_cv.txt", "w+");
    }

    if ((NULL == fpHV) || ((NULL == fpHV) && (1U == haveCV))) {
        DEBUG_ERR("%s: Error: Failed to open file \n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Check Config type */
    if (ISYS_CONFIG_ISP != ispImg->imgHdr.commonHdr.cfgType){
        DEBUG_ERR("%s: Wrong Config! \n", __FUNCTION__);
        ret  = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Check Magic */
    if (ISP_MAGIC != ispImg->imgHdr.commonHdr.cfgMagic){
        DEBUG_ERR("%s: Wrong Magic in config\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Check number of configs */
    if (ispImg->imgHdr.numConfigs > ISP_MAX_CONFIGS){
        DEBUG_ERR("%s: More ISP Configs! \n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Read config data */
    for (i = 0UL; (i < ispImg->imgHdr.numConfigs) && (BCM_ERR_OK == ret); i++) {
        switch(ispImg->cfgs[i].type){
            case ISP_CFG_BLK:
                if ((ISP_CFG_VERSION_B0 == ispImg->cfgs[i].version) ||
                   (ISP_CFG_VERSION_COMMON == ispImg->cfgs[i].version)) {
                    skipConfig = 0U;
                } else {
                    skipConfig = 1U;
                }
                /* Check if the offset is valid */
                if ((ispImg->cfgs[i].deviceOffset + (ispImg->cfgs[i].count >> 2UL)) > ISP_MAX_OFFSET) {
                    DEBUG_ERR("%s: Not a valid ISP offset\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                } else if (0UL == ispImg->cfgs[i].count) {
                    DEBUG_ERR("%s: Wrong cfg count\n", __FUNCTION__);
                    ret = BCM_ERR_INVAL_PARAMS;
                } else {
                    if (0U == skipConfig) {
                        /* Write Block config data */
                        for (j = 0UL; (j < ispImg->cfgs[i].count) && (BCM_ERR_OK == ret); j++) {
                            addr = ISP_APB_BASE;
                            addr += (ispImg->cfgs[i].deviceOffset);
                            addr += (j * 4UL);
                            if ((ISP_CFG_PASS_COMMON == ispImg->cfgs[i].pass) ||
                                (ISP_CFG_PASS_HV == ispImg->cfgs[i].pass)) {
                                fprintf(fpHV, "Addr = 0x%.8X Val = 0x%.8X\n", addr,
                                            ispImg->ispCfgData[ispImg->cfgs[i].dataOffset + j]);
                            }
                            if ((0UL != haveCV) &&
                                ((ISP_CFG_PASS_COMMON == ispImg->cfgs[i].pass) ||
                                 (ISP_CFG_PASS_CV == ispImg->cfgs[i].pass))) {
                                fprintf(fpCV, "Addr = 0x%.8X Val = 0x%.8X\n", addr,
                                            ispImg->ispCfgData[ispImg->cfgs[i].dataOffset + j]);
                            }
                        }
                    }
                }
                break;
            case ISP_CFG_TCB:
                /* Check if TCB size is valid */
                if (ispImg->cfgs[i].count != (ISP_TCB_SIZE >> 2UL)) {
                    DEBUG_ERR("%s: Mismatch in TCB Size\n", __FUNCTION__);
                    ret =  BCM_ERR_INVAL_PARAMS;
                } else {
                    uint32_t dataOffset = ispImg->cfgs[i].dataOffset;
                    for (j = 0UL; j < (ISP_TCB_SIZE >> 2UL); j++) {
                        if ((ISP_CFG_PASS_COMMON == ispImg->cfgs[i].pass) ||
                            (ISP_CFG_PASS_HV == ispImg->cfgs[i].pass)) {
                            fprintf(fpHV, "TCB_Data = 0x%.8X\n", ispImg->ispCfgData[dataOffset + (j)]);
                        }
                        if ((0UL != haveCV) &&
                            ((ISP_CFG_PASS_COMMON == ispImg->cfgs[i].pass) ||
                             (ISP_CFG_PASS_CV == ispImg->cfgs[i].pass))) {
                            fprintf(fpCV, "TCB_Data = 0x%.8X\n", ispImg->ispCfgData[dataOffset + (j)]);
                        }
                    }
                }
                break;
            default:
                DEBUG_ERR("%s: Unknown Configuration \n", __FUNCTION__);
                ret =  BCM_ERR_INVAL_PARAMS;
                break;
        }
    }

exit:
    if (NULL != fpHV) {
        fclose(fpHV);
    }
    if (NULL != fpCV) {
        fclose(fpCV);
    }

    return ret;
}

/******************************************************************************
FUNCTION NAME: IspImageCombineVersion

ARGUMENTS:
aA0Img  -> A0 ISP image.
aB0Img  -> B0 ISP image.
aImg    -> Combined ISP image.

DESCRIPTION: This function combines A0 and B0 images to a single image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t IspImageCombineVersion(ISP_ImageType* aA0Img, ISP_ImageType* aB0Img, ISP_ImageType* aImg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t cIdx = 0UL;
    uint32_t cfgDataIdx = 0UL;
    uint32_t i, j;

    /* Check Parameter */
    if (NULL == aA0Img || NULL == aB0Img || NULL == aImg) {
        DEBUG_ERR("%s: Null argument\n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    aImg->imgHdr = aA0Img->imgHdr;

    /* Read config data */
    for (i = 0UL; (i < aA0Img->imgHdr.numConfigs); i++) {
        uint32_t diff = 0UL;

        /* Compare data b/w A0 and B0 */
        for (j = 0UL; (j < aA0Img->cfgs[i].count); j++) {
            if (aA0Img->ispCfgData[aA0Img->cfgs[i].dataOffset + j] !=
                    aB0Img->ispCfgData[aB0Img->cfgs[i].dataOffset + j]) {
                diff = 1UL;
            }
        }

        if (1UL == diff) {
            /* Write config header for A0 */
            aImg->cfgs[cIdx].version = ISP_CFG_VERSION_A0;
            aImg->cfgs[cIdx].pass = aA0Img->cfgs[i].pass;
            aImg->cfgs[cIdx].type = aA0Img->cfgs[i].type;
            aImg->cfgs[cIdx].deviceOffset = aA0Img->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = aA0Img->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write Block config data for A0 */
            for (j = 0UL; (j < aA0Img->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    aA0Img->ispCfgData[aA0Img->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;

            /* Write config header for B0 */
            aImg->cfgs[cIdx].version = ISP_CFG_VERSION_B0;
            aImg->cfgs[cIdx].pass = aB0Img->cfgs[i].pass;
            aImg->cfgs[cIdx].type = aB0Img->cfgs[i].type;
            aImg->cfgs[cIdx].deviceOffset = aB0Img->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = aB0Img->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write Block config data for B0 */
            for (j = 0UL; (j < aA0Img->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    aB0Img->ispCfgData[aB0Img->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;
        } else {
            /* Write common config header */
            aImg->cfgs[cIdx].version = ISP_CFG_VERSION_COMMON;
            aImg->cfgs[cIdx].pass = aA0Img->cfgs[i].pass;
            aImg->cfgs[cIdx].type = aA0Img->cfgs[i].type;
            aImg->cfgs[cIdx].deviceOffset = aA0Img->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = aA0Img->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write common config data */
            for (j = 0UL; (j < aA0Img->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    aA0Img->ispCfgData[aA0Img->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;
        }
        if (ISP_MAX_CONFIGS < cIdx) {
            DEBUG_ERR("%s: Exceeded ISP_MAX_CONFIGS for ISP : %u \n", __FUNCTION__, cIdx);
            ret = BCM_ERR_NOMEM;
            goto exit;
        }
        if (ISP_MAX_CONFIG_SIZE < cfgDataIdx) {
            DEBUG_ERR("%s: Exceeded ISP_MAX_CONFIG_SIZE for ISP: %u\n", __FUNCTION__, cfgDataIdx);
            ret = BCM_ERR_NOMEM;
            goto exit;
        }
    }
    /* Update numConfig for combined image */
    aImg->imgHdr.numConfigs = cIdx;

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: IspImageCombinePass

ARGUMENTS:
aHV_Img  -> HV ISP image.
aCV_Img  -> CV ISP image.
aImg     -> Combined ISP image.

DESCRIPTION: This function combines HV and CV images to a single image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t IspImageCombinePass(ISP_ImageType* aHV_Img, ISP_ImageType* aCV_Img, ISP_ImageType* aImg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t cIdx = 0UL;
    uint32_t cfgDataIdx = 0UL;
    uint32_t i, j, k;
    ISP_BlockEnable ble[ISYS_PASS_IDX_MAX];
    uint32_t bayerEnableOffset = offsetof(ISP_RDBType, fr_bayer_en);
    uint32_t yCbCrEnableOffset = offsetof(ISP_RDBType, fr_ycbcr_en);

    /* Check Parameter */
    if (NULL == aHV_Img || NULL == aCV_Img || NULL == aImg) {
        DEBUG_ERR("%s: Null argument\n", __FUNCTION__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    aImg->imgHdr = aHV_Img->imgHdr;
    aImg->imgHdr.passEnable = ISYS_PASS_ENABLE_HV | ISYS_PASS_ENABLE_CV;

    /* Fill block enables */
    for (i = 0UL; (i < aHV_Img->imgHdr.numConfigs); i++) {
        for (j = 0UL; (j < aHV_Img->cfgs[i].count); j++) {
            if (ISP_CFG_BLK == aHV_Img->cfgs[i].type) {
                if ((aHV_Img->cfgs[i].deviceOffset + j*4) == bayerEnableOffset) {
                    ble[ISYS_PASS_IDX_HV].bayer = aHV_Img->ispCfgData[aHV_Img->cfgs[i].dataOffset + j];
                    ble[ISYS_PASS_IDX_CV].bayer = aCV_Img->ispCfgData[aCV_Img->cfgs[i].dataOffset + j];
                }
                if ((aHV_Img->cfgs[i].deviceOffset + j*4) == yCbCrEnableOffset) {
                    ble[ISYS_PASS_IDX_HV].ycbcr = aHV_Img->ispCfgData[aHV_Img->cfgs[i].dataOffset + j];
                    ble[ISYS_PASS_IDX_CV].ycbcr = aCV_Img->ispCfgData[aCV_Img->cfgs[i].dataOffset + j];
                }
            }
        }
    }

    /* Read config data */
    for (i = 0UL; (i < aHV_Img->imgHdr.numConfigs); i++) {
        uint32_t diff = 0UL;
        ISP_ImageType* defImage = aHV_Img;

        /* Compare data b/w HV and CV */
        for (j = 0UL; (j < aHV_Img->cfgs[i].count); j++) {
            if (aHV_Img->ispCfgData[aHV_Img->cfgs[i].dataOffset + j] !=
                    aCV_Img->ispCfgData[aCV_Img->cfgs[i].dataOffset + j]) {
                uint32_t filterIdx = 0UL;
                uint32_t hvEnable;
                uint32_t cvEnable;
                uint32_t configMask;

                /* Check if there is a difference in block enable */
                /* Find the corresponding filter index */
                for (k = 0; k < (sizeof(gIspRegFilter)/sizeof(RegFilter)); k++) {
                    if (aHV_Img->cfgs[i].deviceOffset == gIspRegFilter[k].startAddr) {
                        filterIdx = k;
                        break;
                    }
                }

                /* Get MASKs */
                configMask = gIspRegFilter[filterIdx].mask;
                if (ISP_BLOCK_TYPE_BAYER == gIspRegFilter[k].blockType) {
                    hvEnable = (configMask & ble[ISYS_PASS_IDX_HV].bayer);
                    cvEnable = (configMask & ble[ISYS_PASS_IDX_CV].bayer);
                } else if (ISP_BLOCK_TYPE_YCBCR == gIspRegFilter[k].blockType) {
                    hvEnable = (configMask & ble[ISYS_PASS_IDX_HV].ycbcr);
                    cvEnable = (configMask & ble[ISYS_PASS_IDX_CV].ycbcr);
                } else {
                    diff = 1UL;
                }

                /* Check enables */
                if ((0UL != hvEnable) && (0UL != cvEnable)) {
                    diff = 1UL;
                } else if ((0UL == hvEnable) && (0UL != cvEnable)) {
                    defImage = aCV_Img;
                } else {
                    defImage = aHV_Img;
                }
                break;
            }
        }

        if (1UL == diff) {
            /* Write config header for HV */
            aImg->cfgs[cIdx].version = aHV_Img->cfgs[i].version;
            aImg->cfgs[cIdx].pass = ISP_CFG_PASS_HV;
            aImg->cfgs[cIdx].type = aHV_Img->cfgs[i].type;
            aImg->cfgs[cIdx].deviceOffset = aHV_Img->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = aHV_Img->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write Block config data for HV */
            for (j = 0UL; (j < aHV_Img->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    aHV_Img->ispCfgData[aHV_Img->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;

            /* Write config header for CV */
            aImg->cfgs[cIdx].version = aCV_Img->cfgs[i].version;;
            aImg->cfgs[cIdx].pass = ISP_CFG_PASS_CV;
            aImg->cfgs[cIdx].type = aCV_Img->cfgs[i].type;;
            aImg->cfgs[cIdx].deviceOffset = aCV_Img->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = aCV_Img->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write Block config data for CV */
            for (j = 0UL; (j < aHV_Img->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    aCV_Img->ispCfgData[aCV_Img->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;
        } else {
            /* Write common config header */
            aImg->cfgs[cIdx].type = defImage->cfgs[i].type;
            aImg->cfgs[cIdx].version = defImage->cfgs[i].version;;
            aImg->cfgs[cIdx].pass = ISP_CFG_PASS_COMMON;
            aImg->cfgs[cIdx].deviceOffset = defImage->cfgs[i].deviceOffset;
            aImg->cfgs[cIdx].count = defImage->cfgs[i].count;
            aImg->cfgs[cIdx].dataOffset = cfgDataIdx;
            /* Write common config data */
            for (j = 0UL; (j < defImage->cfgs[i].count); j++) {
                aImg->ispCfgData[aImg->cfgs[cIdx].dataOffset + j] =
                    defImage->ispCfgData[defImage->cfgs[i].dataOffset + j];
            }
            cfgDataIdx += aImg->cfgs[cIdx].count;
            cIdx++;
        }
        if (ISP_MAX_CONFIGS < cIdx) {
            DEBUG_ERR("%s: Exceeded ISP_MAX_CONFIGS for ISP : %u \n", __FUNCTION__, cIdx);
            ret = BCM_ERR_NOMEM;
            goto exit;
        }
        if (ISP_MAX_CONFIG_SIZE < cfgDataIdx) {
            DEBUG_ERR("%s: Exceeded ISP_MAX_CONFIG_SIZE for ISP: %u\n", __FUNCTION__, cfgDataIdx);
            ret = BCM_ERR_NOMEM;
            goto exit;
        }
    }
    /* Update numConfig for combined image */
    aImg->imgHdr.numConfigs = cIdx;

exit:
    return ret;
}
