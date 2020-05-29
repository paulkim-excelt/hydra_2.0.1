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
File Name:  unicam_image_creater.c
Description: This file Unicam image dump.
******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unicam_rdb.h>
#include "unicam_driver.h"
#include "unicam_image_util.h"
#include "image_to_array.h"

/* Macros */
#define DEBUG_ERR printf
#define DEBUG
#ifdef DEBUG
#define DEBUG_INFO printf
#endif

/* TODO: Need to move */
#define UNICAM_MAX_DATA_LANES   (4UL)

/* Externs */
extern CamSensorParams gSensorParams;

/* Byte-swap enable for RAW16 CSI streams */
uint32_t gISPByteSwapEnable = 0UL;

/* UnicamGetFormatInfo */
int32_t UnicamGetFormatInfo(EInputFormat aFormat, uint32_t *aBPP, uint32_t *aID)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t bpp = 0UL;
    uint32_t id = 0UL;

    /* To get image id's and bpp */
    switch (aFormat) {
        case eRaw6:
            bpp = 6UL;
            id = 0x28UL;
            break;
        case eRaw7:
            bpp = 7UL;
            id = 0x29UL;
            break;
        case eRaw8:
            bpp = 8UL;
            id = 0x2aUL;
            break;
        case eRaw10:
            bpp = 10UL;
            id = 0x2bUL;
            break;
        case eRaw12:
            bpp = 12UL;
            id = 0x2cUL;
            break;
        case eRaw14:
            bpp = 14UL;
            id = 0x2dUL;
            break;
        case eRaw16:
            bpp = 16UL;
            id = 0x2eUL;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
    }

    if (NULL != aBPP) {
        *aBPP = bpp;
    }

    if (NULL != aID) {
        *aID = id;
    }

    return ret;
}

/******************************************************************************
FUNCTION NAME: Unicam_ImageWriter

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t Unicam_ImageWriter(void)
{
    UNICAM_ImageType img;
    int32_t status = BCM_ERR_INVAL_PARAMS;
    FILE *fp = NULL;
    CamSensorParams *sensorParams = &gSensorParams;

    memset(&img, 0, sizeof(UNICAM_ImageType));

    fp = fopen("unicam.img", "wb+");
    if (NULL == fp)
    {
        DEBUG_ERR("%s: Error opening file\n", __func__);
        status = BCM_ERR_UNKNOWN;
        goto func_exit;
    }

    if (eRaw16 == sensorParams->format) {
        gISPByteSwapEnable = 1UL;
    }

    /* Main header */
    img.imgHdr.cfgType = ISYS_CONFIG_UNICAM;
    img.imgHdr.cfgMagic = UNICAM_MAGIC;

    img.modeConfig.peripheralMode = (uint8_t)sensorParams->ifMode;
    img.modeConfig.imgPipelineCtl = (uint8_t) (((sensorParams->unPack <<
                                                 UNICAM_IPIPE_PUM_SHIFT) &
                                                 UNICAM_IPIPE_PUM_MASK) |
                                               ((sensorParams->dpcmDecode <<
                                                 UNICAM_IPIPE_DDM_SHIFT) &
                                                 UNICAM_IPIPE_DDM_MASK));
    img.modeConfig.format[UNICAM_OUTPUT_IDX_LONG] = (uint8_t) sensorParams->format;
    img.modeConfig.format[UNICAM_OUTPUT_IDX_SHORT] = (uint8_t) sensorParams->shortFormat;

    if (UNICAM_MODE_CSI == img.modeConfig.peripheralMode) {
        UNICAM_CSIConfigType *csiCfg = &img.InterfaceConfig.csiConfig;

        csiCfg->tclkTermEnable = (uint8_t) sensorParams->frontEnd.csiParams.clkTime.clkTermEn;
        csiCfg->tclkSettle = (uint8_t) sensorParams->frontEnd.csiParams.clkTime.clkSettle;
        csiCfg->tdTermEnable = (uint8_t) sensorParams->frontEnd.csiParams.dataTime.tdTermEn;
        csiCfg->thsSettle = (uint8_t) sensorParams->frontEnd.csiParams.dataTime.thsSettle;
        csiCfg->trxEnable = (uint8_t) sensorParams->frontEnd.csiParams.dataTime.trxEnable;
        csiCfg->dataLaneCtl = (uint8_t) (((sensorParams->numLanes <<
                                         UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT) &
                                         UNICAM_CSICFG_DATALANE_NUM_LINES_MASK));
        csiCfg->dataLaneCtl |= (uint8_t) ((sensorParams->frontEnd.csiParams.laneSync <<
                                         UNICAM_CSICFG_DATALANE_DLSMN_SHIFT) &
                                         UNICAM_CSICFG_DATALANE_DLSMN_MASK);
        csiCfg->dataLaneCtl |= (uint8_t) ((sensorParams->frontEnd.csiParams.laneTermination <<
                                         UNICAM_CSICFG_DATALANE_DLTREN_SHIFT) &
                                         UNICAM_CSICFG_DATALANE_DLTREN_MASK);
        csiCfg->passLineSync = (uint8_t) ((sensorParams->frontEnd.csiParams.lineStart <<
                                          UNICAM_LINCTL_PASS_LS_SHIFT) &
                                          UNICAM_LINCTL_PASS_LS_MASK);
        csiCfg->passLineSync |= (uint8_t) ((sensorParams->frontEnd.csiParams.lineEnd <<
                                           UNICAM_LINCTL_PASS_LE_SHIFT) &
                                           UNICAM_LINCTL_PASS_LE_MASK);
        csiCfg->packetFrameTimeout = (uint8_t) sensorParams->frontEnd.csiParams.packetFarmeTimeout;
        csiCfg->vcNum[UNICAM_OUTPUT_IDX_LONG] = (uint8_t) sensorParams->frontEnd.csiParams.longExpVc;
        csiCfg->vcNum[UNICAM_OUTPUT_IDX_SHORT] =(uint8_t) sensorParams->frontEnd.csiParams.shortExpVc;
        csiCfg->hsClkFrequency = sensorParams->hsClkFrequency;
    } else  if (UNICAM_MODE_TG == img.modeConfig.peripheralMode) {
        UNICAM_TGConfigType *tgCfg = &img.InterfaceConfig.tgConfig;

        tgCfg->blkSize = (uint8_t) ((sensorParams->frontEnd.tgParams.blockXSize <<
                                     UNICAM_VC5_TGBSZ_BXSIZE_SHIFT) &
                                     UNICAM_VC5_TGBSZ_BXSIZE_MASK);
        tgCfg->blkSize |= (uint8_t) ((sensorParams->frontEnd.tgParams.blockYSize <<
                                     UNICAM_VC5_TGBSZ_BYSIZE_SHIFT) &
                                     UNICAM_VC5_TGBSZ_BYSIZE_MASK);
        tgCfg->axiMinBurstLength = sensorParams->frontEnd.tgParams.min;
        tgCfg->axiMaxBurstLength = sensorParams->frontEnd.tgParams.max;
        tgCfg->tgCtl = (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.mode <<
                                   UNICAM_VC5_TGCTL_MODE_SHIFT) &
                                   UNICAM_VC5_TGCTL_MODE_MASK);
        tgCfg->tgCtl |= (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.cfgByPass <<
                                   UNICAM_VC5_TGCTL_CFG_BYPASS_SHIFT) &
                                   UNICAM_VC5_TGCTL_CFG_BYPASS_MASK);
        tgCfg->tgCtl |= (uint32_t)(((16UL - sensorParams->frontEnd.tgParams.ctrl.bpp) <<
                                   UNICAM_VC5_TGCTL_BPP_SHIFT) &
                                   UNICAM_VC5_TGCTL_BPP_MASK);
        tgCfg->tgCtl |= (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.chan <<
                                   UNICAM_VC5_TGCTL_CHAN_SHIFT) &
                                   UNICAM_VC5_TGCTL_CHAN_MASK);
        tgCfg->tgCtl |= (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.bayerOrder <<
                                   UNICAM_VC5_TGCTL_BAYER_SHIFT) &
                                   UNICAM_VC5_TGCTL_BAYER_MASK);
        tgCfg->tgCtl |= (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.distortEn <<
                                   UNICAM_VC5_TGCTL_DIS_EN_SHIFT) &
                                   UNICAM_VC5_TGCTL_DIS_EN_MASK);
        tgCfg->tgCtl |= (uint32_t)((sensorParams->frontEnd.tgParams.ctrl.translateEn <<
                                   UNICAM_VC5_TGCTL_TR_EN_SHIFT) &
                                   UNICAM_VC5_TGCTL_TR_EN_MASK);
        tgCfg->clkFrequency = sensorParams->frontEnd.tgParams.clkFreq;
    } else {
        status = BCM_ERR_INVAL_PARAMS;
        DEBUG_ERR("%s: Unsupported Mode\n", __func__);
        goto func_exit;
    }

    /* Write to the file */
    fwrite(&img, 1, sizeof(UNICAM_ImageType), fp);
    fclose(fp);

    /* Write to C file */
    ImageToArray("unicam_image", (unsigned char *)&img, sizeof(img));
    DEBUG_INFO("Unicam image creater done!!\n");

    status = BCM_ERR_OK;

func_exit:
    return status;
}

/******************************************************************************
FUNCTION NAME: Unicam_ImageReader

ARGUMENTS: None

DESCRIPTION: This function reads Unicam image.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t Unicam_ImageReader(const UNICAM_ImageType *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp = NULL;

    /* Check Parameter */
    if (NULL == aCfg) {
        DEBUG_ERR("%s: Null Config Pointer\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }
    if (ISYS_CONFIG_UNICAM != aCfg->imgHdr.cfgType) {
        DEBUG_ERR("%s: Wrong Config! \n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }
    if (UNICAM_MAGIC != aCfg->imgHdr.cfgMagic) {
        DEBUG_ERR("%s: Wrong Magic in config\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    fp = fopen("unicam_config.txt", "w+");
    if (NULL == fp) {
        DEBUG_ERR("%s: Failed to open file\n", __FUNCTION__);
        goto exit;
    }

    fprintf(fp, "interface = %u\n", aCfg->modeConfig.peripheralMode);
    fprintf(fp, "dpcm_decode = %lu\n",
                (aCfg->modeConfig.imgPipelineCtl & UNICAM_IPIPE_DDM_MASK) >>
                UNICAM_IPIPE_DDM_SHIFT);
    fprintf(fp, "unpack = %lu\n",
                (aCfg->modeConfig.imgPipelineCtl & UNICAM_IPIPE_PUM_MASK) >>
                UNICAM_IPIPE_PUM_SHIFT);
    fprintf(fp, "format = %d\n", aCfg->modeConfig.format[UNICAM_OUTPUT_IDX_LONG]);
    fprintf(fp, "short_format = %d\n", aCfg->modeConfig.format[UNICAM_OUTPUT_IDX_SHORT]);

    if (UNICAM_MODE_CSI == aCfg->modeConfig.peripheralMode) {
        fprintf(fp, "hs_clk_frequency = %d\n", aCfg->InterfaceConfig.csiConfig.hsClkFrequency);
        fprintf(fp, "tclk_settle = %d\n", aCfg->InterfaceConfig.csiConfig.tclkSettle);
        fprintf(fp, "tclk_term_enable = %d\n", aCfg->InterfaceConfig.csiConfig.tclkTermEnable);
        fprintf(fp, "td_term_enable = %d\n", aCfg->InterfaceConfig.csiConfig.tdTermEnable);
        fprintf(fp, "ths_settle = %d\n", aCfg->InterfaceConfig.csiConfig.thsSettle);
        fprintf(fp, "trx_enable = %d\n", aCfg->InterfaceConfig.csiConfig.trxEnable);
        fprintf(fp, "line_start_packet = %lu\n",
                (aCfg->InterfaceConfig.csiConfig.passLineSync & UNICAM_LINCTL_PASS_LS_MASK) >>
                UNICAM_LINCTL_PASS_LS_SHIFT);
        fprintf(fp, "line_end_packet = %lu\n",
                (aCfg->InterfaceConfig.csiConfig.passLineSync & UNICAM_LINCTL_PASS_LE_MASK) >>
                UNICAM_LINCTL_PASS_LE_SHIFT);
        fprintf(fp, "packet_farme_timeout = %d\n", aCfg->InterfaceConfig.csiConfig.packetFrameTimeout);
        fprintf(fp, "num_data_lanes = %d\n",
                (aCfg->InterfaceConfig.csiConfig.dataLaneCtl & UNICAM_CSICFG_DATALANE_NUM_LINES_MASK) >>
                UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT);
        fprintf(fp, "lane_sync = %d\n",
                (aCfg->InterfaceConfig.csiConfig.dataLaneCtl & UNICAM_CSICFG_DATALANE_DLSMN_MASK) >>
                UNICAM_CSICFG_DATALANE_DLSMN_SHIFT);
        fprintf(fp, "lane_termination = %d\n",
                (aCfg->InterfaceConfig.csiConfig.dataLaneCtl & UNICAM_CSICFG_DATALANE_DLTREN_MASK) >>
                UNICAM_CSICFG_DATALANE_DLTREN_SHIFT);
        fprintf(fp, "long_vc = %d\n", aCfg->InterfaceConfig.csiConfig.vcNum[UNICAM_OUTPUT_IDX_LONG]);
        fprintf(fp, "short_vc = %d\n", aCfg->InterfaceConfig.csiConfig.vcNum[UNICAM_OUTPUT_IDX_SHORT]);
    } else {
        fprintf(fp, "clock_frequency = %d\n", aCfg->InterfaceConfig.tgConfig.clkFrequency);
        fprintf(fp, "mode = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_MODE_MASK) >>
                    UNICAM_VC5_TGCTL_CFG_BYPASS_SHIFT);
        fprintf(fp, "cfg_by_pass = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_CFG_BYPASS_MASK) >>
                    UNICAM_VC5_TGCTL_MODE_SHIFT);
        fprintf(fp, "chan = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_CHAN_MASK) >>
                    UNICAM_VC5_TGCTL_CHAN_SHIFT);
        fprintf(fp, "bayer = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_BAYER_MASK) >>
                    UNICAM_VC5_TGCTL_BAYER_SHIFT);
        fprintf(fp, "distort_enable = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_DIS_EN_MASK) >>
                    UNICAM_VC5_TGCTL_DIS_EN_SHIFT);
        fprintf(fp, "translate_enable = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.tgCtl & UNICAM_VC5_TGCTL_TR_EN_MASK) >>
                    UNICAM_VC5_TGCTL_TR_EN_SHIFT);
        fprintf(fp, "block_y_size = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.blkSize & UNICAM_VC5_TGBSZ_BYSIZE_MASK) >>
                    UNICAM_VC5_TGBSZ_BYSIZE_SHIFT);
        fprintf(fp, "block_x_size = %lu\n",
                    (aCfg->InterfaceConfig.tgConfig.blkSize & UNICAM_VC5_TGBSZ_BXSIZE_MASK) >>
                    UNICAM_VC5_TGBSZ_BXSIZE_SHIFT);
        fprintf(fp, "max = %d\n", aCfg->InterfaceConfig.tgConfig.axiMaxBurstLength);
        fprintf(fp, "min = %d\n", aCfg->InterfaceConfig.tgConfig.axiMinBurstLength);
    }
exit:
    if (fp) {
        fclose(fp);
    }
    return ret;
}
