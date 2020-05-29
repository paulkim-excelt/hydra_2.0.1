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
File Name:  mode_image_util.c
Descritpion: This file implements mode image dump.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "image_to_array.h"
#include "mode_image_util.h"
#include "isp_regdump_interface.h"
#include "unicam_user.h"
#include <unicam_osil.h>

/* Marcos */
#define DEBUG_ERR printf

/* Externs */
extern ISPConfigData gISPCfgData;
extern CamSensorParams gSensorParams;
extern uint32_t gUnicamFormat;
extern char gSensorName[100];

/******************************************************************************
FUNCTION NAME: ModeImageWriter

ARGUMENTS:
aPassIdx -> Pass Index.

DESCRIPTION: This function dumps mode image.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ModeImageWriter(uint32_t aPassIdx)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp = NULL;
    int32_t i, j;
    ISYS_ModeImageType camModeImg;

    if (ISYS_PASS_IDX_MAX <= aPassIdx) {
        printf("%s: Wrong aPassIdx = %d\n", __FUNCTION__, aPassIdx);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }
    memset(&camModeImg, 0, sizeof(ISYS_ModeImageType));

    /* Mode Image Header */
    camModeImg.imgHdr.cfgType = ISYS_CONFIG_MODE;
    camModeImg.imgHdr.cfgMagic = ISYS_MODE_MAGIC;

    fp = fopen("mode.img", "wb");
    if (NULL == fp) {
        printf("%s: Error opening file\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Hdr */
    camModeImg.mode.hdr = (uint8_t) gSensorParams.isHDREnabled;

    /* FPS */
    camModeImg.mode.fps = (uint8_t) gSensorParams.fps;

    camModeImg.mode.metaLines[ISYS_SENSOR_EXP_IDX_LONG] =
                    (uint8_t) ((gSensorParams.embeddedBefore <<
                               UNICAM_MODECFG_META_LINES_START_SHIFT) &
                               UNICAM_MODECFG_META_LINES_START_MASK);
    camModeImg.mode.metaLines[ISYS_SENSOR_EXP_IDX_LONG] |=
                    (uint8_t) ((gSensorParams.embeddedAfter <<
                               UNICAM_MODECFG_META_LINES_END_SHIFT) &
                               UNICAM_MODECFG_META_LINES_END_MASK);

    camModeImg.mode.metaLines[ISYS_SENSOR_EXP_IDX_SHORT] =
                    (uint8_t) ((gSensorParams.shortEmbeddedBefore <<
                               UNICAM_MODECFG_META_LINES_START_SHIFT) &
                               UNICAM_MODECFG_META_LINES_START_MASK);
    camModeImg.mode.metaLines[ISYS_SENSOR_EXP_IDX_SHORT] |=
                    (uint8_t) ((gSensorParams.shortEmbeddedAfter <<
                               UNICAM_MODECFG_META_LINES_END_SHIFT) &
                               UNICAM_MODECFG_META_LINES_END_MASK);

    /* HDR short line offset */
    camModeImg.mode.shortLinesOffset = (uint16_t) gSensorParams.shortLineOffset;

    /* Unicam Width */
    camModeImg.mode.inputWidth = (uint16_t) gSensorParams.imgWidth;

    /* Unicam Height */
    camModeImg.mode.inputHeight = (uint16_t) gSensorParams.imgHeight;

    /* Unicam Stride */
    camModeImg.mode.inputFormat = (uint8_t) ((0UL != gSensorParams.isHDREnabled) &&
                                   (gSensorParams.format <
                                    gSensorParams.shortFormat)) ?
                                    gSensorParams.shortFormat : gSensorParams.format;

    /* Unicam Mode */
    camModeImg.mode.unicamMode = (uint8_t) gSensorParams.ifMode;

    /* ISP version */
    camModeImg.mode.ispVersion = gISPCfgData.ispVersion;

    /* ISP output enable */
    camModeImg.mode.passProps[aPassIdx].outputEnable = gISPCfgData.outputEnable;

    for (i = 0; i < ISYS_OUT_IDX_MAX; i++ ) {
        camModeImg.mode.passProps[aPassIdx].outputProps[i].width = gISPCfgData.outputProps[i].width;
        camModeImg.mode.passProps[aPassIdx].outputProps[i].height = gISPCfgData.outputProps[i].height;
        camModeImg.mode.passProps[aPassIdx].outputProps[i].format = gISPCfgData.outputProps[i].format;
        camModeImg.mode.passProps[aPassIdx].outputProps[i].outWidth = gISPCfgData.outputProps[i].outWidth;
        for (j = 0; j < ISYS_MAX_IMAGE_PLANES; j++ ) {
            camModeImg.mode.passProps[aPassIdx].outputProps[i].stride[j] = gISPCfgData.outputProps[i].stride[j];
        }
    }

    /* Pass Enable */
    camModeImg.mode.passEnable = (ISYS_PASS_IDX_HV == aPassIdx) ? ISYS_PASS_ENABLE_HV : ISYS_PASS_ENABLE_CV;

    /* Sensor Name */
    strncpy((char*)camModeImg.mode.sensorName, (char*)gSensorName, ISYS_MAX_NAME_CHAR);

    /* Writes to file */
    fwrite(&camModeImg, 1, sizeof(ISYS_ModeImageType), fp);

    /* Write to C file */
    ImageToArray("cam_mode_image", (unsigned char *)&camModeImg, sizeof(camModeImg));

exit:
    if (NULL != fp) {
        fclose(fp);
    }
    return ret;
}

/******************************************************************************
FUNCTION NAME: ModeImageReader

ARGUMENTS: None

DESCRIPTION: This function reads mode image and dump to file.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ModeImageReader(const ISYS_ModeImageType *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    int32_t i, j, k;
    FILE *fp = NULL;

    /* OutPut file */
    fp = fopen("mode_config.txt", "w+");
    if (NULL == fp) {
        DEBUG_ERR("%s: Error: Failed to open file\n", __FUNCTION__);
        ret  = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

   /* Check Parameter */
    if (NULL == aCfg) {
        DEBUG_ERR("%s: Null Config Pointer\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (ISYS_CONFIG_MODE != aCfg->imgHdr.cfgType) {
        DEBUG_ERR("%s: Wrong Config!\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    if (ISYS_MODE_MAGIC != aCfg->imgHdr.cfgMagic) {
        DEBUG_ERR("%s: Wrong Magic in config!\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    fprintf(fp, "hdr = %d\n", aCfg->mode.hdr);
    fprintf(fp, "fps = %d\n", aCfg->mode.fps);
    fprintf(fp, "ED begin(Long) = %d\n",
                ((aCfg->mode.metaLines[ISYS_SENSOR_EXP_IDX_LONG] &
                UNICAM_MODECFG_META_LINES_START_MASK) >>
                UNICAM_MODECFG_META_LINES_START_SHIFT));
    fprintf(fp, "ED end(Long) = %d\n",
                ((aCfg->mode.metaLines[ISYS_SENSOR_EXP_IDX_LONG] &
                UNICAM_MODECFG_META_LINES_END_MASK) >>
                UNICAM_MODECFG_META_LINES_END_SHIFT));
    fprintf(fp, "ED begin(Short) = %d\n",
                ((aCfg->mode.metaLines[ISYS_SENSOR_EXP_IDX_SHORT] &
                UNICAM_MODECFG_META_LINES_START_MASK) >>
                UNICAM_MODECFG_META_LINES_START_SHIFT));
    fprintf(fp, "ED end(Short) = %d\n",
                ((aCfg->mode.metaLines[ISYS_SENSOR_EXP_IDX_SHORT] &
                UNICAM_MODECFG_META_LINES_END_MASK) >>
                UNICAM_MODECFG_META_LINES_END_SHIFT));
    fprintf(fp, "shortLinesOffset = %u\n", aCfg->mode.shortLinesOffset);
    fprintf(fp, "input Width = %d\n", aCfg->mode.inputWidth);
    fprintf(fp, "input Height = %d\n", aCfg->mode.inputHeight);
    fprintf(fp, "input Format = %d\n", aCfg->mode.inputFormat);
    fprintf(fp, "unicamMode = %u\n", aCfg->mode.unicamMode);

    fprintf(fp, "ispVersion = %d\n", aCfg->mode.ispVersion);
    fprintf(fp, "Pass Enable = 0x%x\n", aCfg->mode.passEnable);

    for (i=0; i<ISYS_PASS_IDX_MAX; i++) {
        fprintf(fp, "outProps[%d].outputEnable = %d\n", i, aCfg->mode.passProps[i].outputEnable);
        for (j = 0; j < ISYS_OUT_IDX_MAX; j++ ) {
            fprintf(fp, "outProps[%d].outputProps[%d].width = %d\n",
                        i, j, aCfg->mode.passProps[i].outputProps[j].width);
            fprintf(fp, "outProps[%d].outputProps[%d].height = %d\n",
                        i, j, aCfg->mode.passProps[i].outputProps[j].height);
            fprintf(fp, "outProps[%d].outputProps[%d].format = %d\n",
                        i, j, aCfg->mode.passProps[i].outputProps[j].format);
            fprintf(fp, "outProps[%d].outputProps[%d].outWidth = %d\n",
                        i, j, aCfg->mode.passProps[i].outputProps[j].outWidth);
            for (k = 0; k < ISYS_MAX_IMAGE_PLANES; k++ ) {
                fprintf(fp, "outProps[%d].outputProps[%d].stride[%d] = %d\n",
                        i, j, k, aCfg->mode.passProps[i].outputProps[j].stride[k]);
            }
        }
    }

exit:
    if (NULL != fp) {
        fclose(fp);
    }
    return ret;
}

/******************************************************************************
FUNCTION NAME: ModeImageCombinePass

ARGUMENTS:
aHV_Img     -> HV Mode Image.
aHV_Img     -> CV Mode Image.
aCombImage  -> Combined Image.

DESCRIPTION: This function combines mode images for HV and CV passes.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t ModeImageCombinePass(ISYS_ModeImageType* aHV_Img,
                             ISYS_ModeImageType* aCV_Img,
                             ISYS_ModeImageType* aCombImage)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i, j;

    if ((NULL == aHV_Img) || (NULL == aCV_Img) || (NULL == aCombImage)) {
        DEBUG_ERR("%s: Invalid arguments!\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Copy HV Image */
    *aCombImage = *aHV_Img;

    aCombImage->mode.passEnable = (ISYS_PASS_ENABLE_HV | ISYS_PASS_ENABLE_CV);

    aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputEnable =
                 aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputEnable;

    for (i = 0; i < ISYS_OUT_IDX_MAX; i++ ) {
        aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].width =
                 aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].width;
        aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].height =
                aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].height;
        aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].format =
                aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].format;
        aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].outWidth =
                aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].outWidth;
        for (j = 0; j < ISYS_MAX_IMAGE_PLANES; j++ ) {
            aCombImage->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].stride[j] =
                    aCV_Img->mode.passProps[ISYS_PASS_IDX_CV].outputProps[i].stride[j];
        }
    }

exit:
    return ret;
}
