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
File Name: sensor_image_util.c
Descritpion: This file dumps sensor image.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sensor_image_util.h"
#include "image_to_array.h"

/* Max of */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Externs */
extern RegDataConfig gBlockConfig;
extern RegDataConfig gModeConfig[MAX_NUM_MODES];
extern RegDataConfig gStartConfig;
extern RegDataConfig gStopConfig;
extern CSN_ImageHdrType gImgHdr;
extern CamSensorParams gSensorParams;
extern char modeString[MAX_NUM_MODES][MAX_LINE];

/******************************************************************************
FUNCTION NAME: UpdateSensorCfg

ARGUMENTS: aRegDataCfg, acfgType

DESCRIPTION: This function forms sensor packet and calls update.

RETURNS: BCM_ERR_OK
         BCM_ERR_INVAL_PARAMS
         BCM_ERR_NOT_FOUND
******************************************************************************/
static int32_t UpdateSensorCfg(RegDataConfig *aRegDataCfg, uint32_t acfgType,
        CSN_ImageType *aSensorImage)
{
    int32_t ret = BCM_ERR_OK;
    int32_t continuePkt = 0L;
    uint32_t curIndex = 0UL;
    uint32_t pktStartIdx = 0UL;
    uint32_t cfgDataIndex = 0UL;

    if (0UL != aSensorImage->imgHdr.numConfigs) {
        cfgDataIndex = (aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs - 1].dataOffset
                + aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs - 1].length);
    }

    /* Check for the count */
    if (0UL == aRegDataCfg->count) {
        ret = BCM_ERR_NOT_FOUND;
        goto exit;
    }

    for (curIndex = 0UL; curIndex < aRegDataCfg->count; curIndex++) {
        if (aSensorImage->imgHdr.numConfigs > CSN_MAX_CFGS) {
            printf("%s: Reached max configs: curIndex: %d \n", __FUNCTION__, curIndex);
            ret = BCM_ERR_INVAL_PARAMS;
            goto exit;
        }

        if (cfgDataIndex > CSN_MAX_CFG_SIZE) {
            printf("%s: Reached max sensor data config size\n", __FUNCTION__);
            ret = BCM_ERR_INVAL_PARAMS;
            goto exit;
        }

        if (0UL != curIndex) {
            /* check for continuity */
            continuePkt = ((aSensorImage->imgHdr.dataWidth >> 3UL) ==
                    (aRegDataCfg->cfg[curIndex].addr - aRegDataCfg->cfg[curIndex - 1].addr));
            if (0L != continuePkt) {
                aSensorImage->imgHdr.numConfigs--;
            }
        }

        /* Check for empety data */
        if (0x0UL == aRegDataCfg->cfg[curIndex].addr) {
            ret = BCM_ERR_NOT_FOUND;
            goto exit;
        }

        if (0L == continuePkt){
            pktStartIdx = cfgDataIndex;
            aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs].cfgType = acfgType;
            aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs].dataOffset =
                cfgDataIndex;

            if (0xffffUL == aRegDataCfg->cfg[curIndex].addr) {
                aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs].cmdType =
                    CSN_CMD_DELAY;
            } else {
                aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs].cmdType =
                    CSN_CMD_REG_WRITE;
            }

            if (CSN_ENDIAN_BIG == gImgHdr.endianness) {
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    ((aRegDataCfg->cfg[curIndex].addr >> 8) & 0xff);
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    (aRegDataCfg->cfg[curIndex].addr & 0xff);
            } else if (CSN_ENDIAN_LITTLE == gImgHdr.endianness) {
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    (aRegDataCfg->cfg[curIndex].addr & 0xff);
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    ((aRegDataCfg->cfg[curIndex].addr >> 8) & 0xff);
            } else {
                printf("%s: Wrong Endianness \n", __FUNCTION__);
                ret = BCM_ERR_INVAL_PARAMS;
                goto exit;
            }
        }

        if (16UL == aSensorImage->imgHdr.dataWidth) {
            if (CSN_ENDIAN_BIG == gImgHdr.endianness) {
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    ((aRegDataCfg->cfg[curIndex].data >> 8) & 0xff);
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    (aRegDataCfg->cfg[curIndex].data & 0xff);
            } else if (CSN_ENDIAN_LITTLE == gImgHdr.endianness) {
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    (aRegDataCfg->cfg[curIndex].data & 0xff);
                aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                    ((aRegDataCfg->cfg[curIndex].data >> 8) & 0xff);
            }
        } else {
            /*  Reg data */
            aSensorImage->cameraSensorCfgData[cfgDataIndex++] =
                aRegDataCfg->cfg[curIndex].data;
        }

        /* Update Length */
        aSensorImage->cfgs[aSensorImage->imgHdr.numConfigs].length =
            (cfgDataIndex - pktStartIdx);

        /* Increment Num packtes */
        aSensorImage->imgHdr.numConfigs++;
    }

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: GetSensorInterfaceSyntax

ARGUMENTS: aInterface

DESCRIPTION: This function gets interface syntax.

RETURNS: SyntaxEntry *

******************************************************************************/
static const SyntaxEntry *GetSensorInterfaceSyntax(int32_t aInterface)
{
    const SyntaxEntry *ret = NULL;

    switch(aInterface) {
        case eCsi2:
            ret= GetUnicamCsi2Syntax();
            break;
        case eTg:
            ret = GetUnicamTgSyntax();
            break;
        default:
            printf("%s: Error no syntax present for this mode = %d\n",
             __FUNCTION__, aInterface);
            break;
    }

    return ret;
}

/******************************************************************************
FUNCTION NAME: SensorParseSyntax

ARGUMENTS: aFileName

DESCRIPTION: This function parse sensor configs.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
static int32_t SensorParseSyntax(char *aFileName)
{
    int32_t ret = BCM_ERR_OK;

    /* Sensor details syntax */
    const SyntaxEntry *sensorDetailSyntax[] = {
        GetSensorDetailsSyntax(),
    };

    ret = ParseConfig(aFileName, sensorDetailSyntax,
            (sizeof(sensorDetailSyntax) / sizeof(SyntaxEntry *)));
    if (ret) {
        printf("%s:Error: parser failed\n", __func__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Sensor remaining syntax */
    const SyntaxEntry *sensorSyntax[] = {
        GetSensorModeDetailsSyntax(),
        GetSensorInterfaceSyntax(gSensorParams.ifMode),
        GetSensorModeSyntax()
    };

    ret = ParseConfig(aFileName, sensorSyntax,
            (sizeof(sensorSyntax) / sizeof(SyntaxEntry *)));
    if (ret) {
        printf("%s:Error: parser failed\n", __func__);
        ret = BCM_ERR_INVAL_PARAMS;
    }

exit:
    return ret;
}
/******************************************************************************
FUNCTION NAME: SensorImageWriter

ARGUMENTS: None

DESCRIPTION: This function dumps sensor image.

RETURNS: BCM_ERR_OK
         BCM_ERR_INVAL_PARAMS
         BCM_ERR_NOT_FOUND
******************************************************************************/
int32_t SensorImageWriter(char *aFileName)
{
    int32_t ret = BCM_ERR_OK;
    FILE *fp =  NULL;
    CSN_ImageType sensorImage;
    char fileName[ISYS_MAX_NAME_CHAR] = {0};
    int32_t i;
    char *modeStringPtr = (char *)modeString;
    char modeStr[MAX_LINE] = {0};
    uint32_t longBPP = 0UL;
    uint32_t shortBPP = 0UL;
    uint32_t bpp = 0UL;

    /* Parse Syntax */
    ret = SensorParseSyntax(aFileName);
    if (BCM_ERR_OK != ret) {
        printf("%s: Failed to parse sensor syntax\n", __FUNCTION__);
        goto exit;
    }

    if (((16UL != gImgHdr.addrWidth) ||
        ((8UL != gImgHdr.dataWidth) &&
         (16UL != gImgHdr.dataWidth))) &&
        (eTg != gSensorParams.ifMode)) {
        printf("%s: Not supported addr_width/data_width!\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Update fps for TG */
    gSensorParams.fps = gImgHdr.fpsHigh;

    ret = UnicamGetFormatInfo(gSensorParams.format,
                              &longBPP,
                              NULL);
    if (BCM_ERR_OK != ret) {
        printf("%s: UnicamGetFormatInfo(Long) failed\n", __FUNCTION__);
        goto exit;
    }
    bpp = longBPP;

    if (0UL != gSensorParams.isHDREnabled) {
        ret = UnicamGetFormatInfo(gSensorParams.shortFormat,
                                  &shortBPP,
                                  NULL);
        if (BCM_ERR_OK != ret) {
            printf("%s: UnicamGetFormatInfo(Short) failed\n", __FUNCTION__);
            goto exit;
        }
        if (longBPP != shortBPP) {
            bpp = MAX(longBPP, shortBPP);
        }
    }
    gImgHdr.bpp = bpp;
    gImgHdr.isHDR = (uint32_t) gSensorParams.isHDREnabled;
    gImgHdr.imgWidth = (uint32_t) gSensorParams.imgWidth;

    /* Clear sensorImage */
    memset(&sensorImage, 0, sizeof(CSN_ImageType));

    /* Main header */
    memcpy(&sensorImage.imgHdr, &gImgHdr, sizeof(CSN_ImageHdrType));
    sensorImage.imgHdr.commonHdr.cfgType = CSN_CONFIG;
    sensorImage.imgHdr.commonHdr.cfgMagic   = CSN_MAGIC;
    if (eTg != gSensorParams.ifMode) {
        /* Fill Block reg data */
        /* Commom Config */
        ret = UpdateSensorCfg(&gBlockConfig, CSN_CFG_COMMON, &sensorImage);
        if (BCM_ERR_OK != ret) {
            printf("%s: UpdateSensorCfg failed\n", __FUNCTION__);
            goto exit;
        }

        /* Start Config */
        ret = UpdateSensorCfg(&gStartConfig, CSN_CFG_START, &sensorImage);
        if (BCM_ERR_OK != ret) {
            printf("%s: UpdateSensorCfg failed\n", __FUNCTION__);
            goto exit;
        }

        /* Stop Config */
        ret = UpdateSensorCfg(&gStopConfig, CSN_CFG_STOP, &sensorImage);
        if (BCM_ERR_OK != ret) {
            printf("%s: UpdateSensorCfg failed\n", __FUNCTION__);
            goto exit;
        }
    }
    /* Common config settings */
    fp = fopen("camera_sensor_common.img", "wb+");
    if (NULL == fp) {
        printf("%s: Error in opening file\n", __FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    /* Write to file */
    if ((sizeof(sensorImage)) != fwrite(&sensorImage, 1, sizeof(sensorImage), fp)) {
        printf("%s: Error in file write\n",__FUNCTION__);
        ret = BCM_ERR_UNKNOWN;
        goto exit;
    }

    /* Write to C file */
    ret = ImageToArray("sensor_common_image", (unsigned char *)&sensorImage, sizeof(sensorImage));
    if (BCM_ERR_OK != ret) {
        printf("%s: ImageToArray failed\n", __FUNCTION__);
        goto exit;
    }

    /* Close File */
    fp = (FILE *)fclose(fp);

    for (i = 0; i < MAX_NUM_MODES; i++) {
        /* Initialize */
        memset(&sensorImage, 0, sizeof(CSN_ImageType));

        /* Main header */
        memcpy(&sensorImage.imgHdr, &gImgHdr, sizeof(CSN_ImageHdrType));
        sensorImage.imgHdr.commonHdr.cfgType = CSN_CONFIG;
        sensorImage.imgHdr.commonHdr.cfgMagic   = CSN_MAGIC;

        /* Fill Modereg data */
        ret = UpdateSensorCfg(&gModeConfig[i], CSN_CFG_MODE, &sensorImage);
        if (BCM_ERR_NOT_FOUND == ret) {
            continue;
        } else if (BCM_ERR_OK != ret) {
            printf("%s: UpdateSensorCfg failed\n", __FUNCTION__);
            goto exit;
        }
        /* Copy mode string */
        strcpy(modeStr, (modeStringPtr + (i * MAX_LINE)));

        sprintf(fileName, "camera_sensor_%s.img", modeStr);
        /* Mode Config settings */
        fp = fopen(fileName, "wb+");
        if (NULL == fp) {
            printf("%s: Error in opening file\n", __FUNCTION__);
            return ret;
        }

        /* Writes to file */
        if ((sizeof(sensorImage)) != fwrite(&sensorImage, 1, sizeof(sensorImage), fp)) {
            printf("%s: Error in file write\n",__FUNCTION__);
            ret = BCM_ERR_UNKNOWN;
            goto exit;

        }

        /* Write to C file */
        sprintf(fileName, "sensor_%s_image", modeStr);
        /* ImageToArray */
        ret = ImageToArray(fileName, (unsigned char *)&sensorImage, sizeof(sensorImage));
        if (BCM_ERR_OK != ret) {
            printf("%s: ImageToArray failed\n", __FUNCTION__);
            goto exit;
        }
        fp = (FILE *)fclose(fp);
    }
    ret = BCM_ERR_OK;
exit:
    if (NULL != fp) {
        fclose(fp);
    }
    return ret;
}

/******************************************************************************
FUNCTION NAME: SensorImageReader

ARGUMENTS: aCfg

DESCRIPTION: This function reads sensor image and dumps to a file.

RETURNS: BCM_ERR_OK or BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t SensorImageReader(const CSN_ImageType *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t index = 0UL;
    int32_t i;
    int32_t j;
    FILE *fp = NULL;
    CSN_ImageType *sensorImg;
    uint32_t regAddr = 0UL;
    char cfgType[20] = {0};
    char prevCfgType[20] = {0};
    uint32_t sameCfg = 0UL;
    uint32_t inc = 1UL;

    if (NULL == aCfg) {
        printf("%s: Null Config pointer\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }
    if (CSN_CONFIG != aCfg->imgHdr.commonHdr.cfgType) {
        printf("%s: Wrong Config!\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }
    if (CSN_MAGIC != aCfg->imgHdr.commonHdr.cfgMagic)
    {
        printf("%s: Wrong Magic in config\n", __FUNCTION__);
        ret = BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Have it in local pointer */
    sensorImg = (CSN_ImageType *)aCfg;

    fp = fopen("sensor_config.txt", "w");
    if (NULL == fp) {
        printf("%s: Failed to open file\n", __FUNCTION__);
        goto exit;
    }

    fprintf(fp, "Sensor_Details:\n");
    fprintf(fp, "cfgType = 0x%X\n", sensorImg->imgHdr.commonHdr.cfgType);
    fprintf(fp, "cfgMagic = 0x%X\n", sensorImg->imgHdr.commonHdr.cfgMagic);
    fprintf(fp, "slaveAddr = 0x%X\n", sensorImg->imgHdr.slaveAddr);
    fprintf(fp, "i2cPort = %d\n", sensorImg->imgHdr.i2cPort);
    fprintf(fp, "i2cFrequency = %d\n", sensorImg->imgHdr.i2cFrequency);
    fprintf(fp, "sensorFrequency = %d\n", sensorImg->imgHdr.sensorFrequency);
    fprintf(fp, "is_10bitSlave = %d\n", sensorImg->imgHdr.is_10bitSlave);
    fprintf(fp, "is_RestartRequired = %d\n", sensorImg->imgHdr.is_RestartRequired);
    fprintf(fp, "addrWidth = %d\n", sensorImg->imgHdr.addrWidth);
    fprintf(fp, "dataWidth = %d\n", sensorImg->imgHdr.dataWidth);
    if (CSN_ENDIAN_LITTLE == sensorImg->imgHdr.endianness) {
        fprintf(fp, "endianness = %s\n", "LE");
    } else {
        fprintf(fp, "endianness = %s\n", "BE");
    }
    fprintf(fp, "\nMode_Details:\n");
    fprintf(fp, "fps_low = %u\n", sensorImg->imgHdr.fpsLow);
    fprintf(fp, "fps_high = %d\n", sensorImg->imgHdr.fpsHigh);
    fprintf(fp, "line_length = %u\n", sensorImg->imgHdr.lineLength);
    fprintf(fp, "integration_min = %u\n", sensorImg->imgHdr.integrationMin);
    fprintf(fp, "coarse_integration_Offset = %u\n", sensorImg->imgHdr.coarseIntegrationOffset);
    fprintf(fp, "min_frame_integration_diff = %u\n", sensorImg->imgHdr.minFrameIntegrationDiff);
    fprintf(fp, "bpp = %u\n", sensorImg->imgHdr.bpp);
    fprintf(fp, "isHDR = %u\n", sensorImg->imgHdr.isHDR);
    fprintf(fp, "imgWidth = %u\n", sensorImg->imgHdr.imgWidth);

    for (i = 0; i< ISYS_SENSOR_EXP_IDX_MAX; i++) {
        fprintf(fp, "meta_%d_offset = %u\n", i, sensorImg->imgHdr.metaOffset[i]);
        fprintf(fp, "meta_%d_size = %u\n", i, sensorImg->imgHdr.metaSize[i]);
        fprintf(fp, "stats_%d_offset = %u\n", i, sensorImg->imgHdr.statsOffset[i]);
        fprintf(fp, "stats_%d_size = %u\n", i, sensorImg->imgHdr.statsSize[i]);
    }
    for (i = 0; i< sensorImg->imgHdr.numConfigs; i++) {
        switch(sensorImg->cfgs[i].cfgType) {
            case CSN_CFG_COMMON:
                strcpy(cfgType, "Common_Config:");
                break;
            case CSN_CFG_START:
                strcpy(cfgType, "Start_Config:");
                break;
            case CSN_CFG_STOP:
                strcpy(cfgType, "Stop_Config:");
                break;
            default:
                break;
        }

        index = sensorImg->cfgs[i].dataOffset;
        /* Get regAddr */
        regAddr = sensorImg->cameraSensorCfgData[index++];
        regAddr = (regAddr << 8) | (sensorImg->cameraSensorCfgData[index++]);
        sensorImg->cfgs[i].length -= 2;

        if (strcmp(cfgType, prevCfgType)) {
            sameCfg = 0;
            fprintf(fp, "\n");
        }
        if (0 == sameCfg) {
            fprintf(fp, "%s\n", cfgType);
            strcpy(prevCfgType, cfgType);
            sameCfg = 1;
        }
        inc = (16UL == sensorImg->imgHdr.dataWidth) ? 2UL : 1UL;
        for (j = 0 ; j < sensorImg->cfgs[i].length; j += inc) {
            if (16UL == sensorImg->imgHdr.dataWidth) {
                uint16_t data = 0UL;
                if (CSN_ENDIAN_BIG == sensorImg->imgHdr.endianness) {
                    data = (sensorImg->cameraSensorCfgData[index] << 8) +
                              (sensorImg->cameraSensorCfgData[index+1UL]);
                } else if (CSN_ENDIAN_LITTLE == sensorImg->imgHdr.endianness) {
                    data = (sensorImg->cameraSensorCfgData[index+1UL] << 8) +
                              (sensorImg->cameraSensorCfgData[index]);
                }
                fprintf(fp, "0x%x, 0x%.4x \n", regAddr, data);
                index += 2UL;
            } else {
                fprintf(fp, "0x%x, 0x%.2x \n", regAddr, sensorImg->cameraSensorCfgData[index++]);
            }
            regAddr += inc;
        }
    }

exit:
    if (NULL != fp) {
        fclose(fp);
    }
    return ret;
}
