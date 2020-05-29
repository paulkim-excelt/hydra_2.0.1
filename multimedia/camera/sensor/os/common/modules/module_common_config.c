/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

/* Includes */
#include <string.h>
#include <bcm_err.h>
#include <camera_sensor_module.h>

/******************************************************************************
  File Name:  module_common_config.c
Description: This file has module specific configuration.
 ******************************************************************************/

#ifdef ENABLE_CAM_MODULE_OX3A10
static CSN_MODULE_TunerCfgType gOX3A10TunerConfig;
static int32_t CSN_MODULE_MakeOX3A10Config(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    /* ---------------------------------------------------------------------- */
    /* Parsing configuration */
    /* Initialize FN config */
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.numBytes = 2U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[0].offset = 328U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[0].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[0].shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[1].offset = 296U;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[1].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseFnCfg.bytes[1].shift = 8U;

    /* Initialize Exposure config */
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.numBytes = 2U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[0].offset = 104U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[0].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[0].shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[1].offset = 72U;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[1].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseExpCfg.bytes[1].shift = 8U;

    /* Initialize RGain config */
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].numBytes = 2U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].shift = 0x82U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].offset = 392U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].offset = 360U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].shift = 8U;

    /* Initialize GGain config */
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].numBytes = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].numBytes = 0U;

    /* Initialize BGain config */
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].numBytes = 2U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].shift = 0x82U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].offset = 456U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].shift = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].offset = 424U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].shift = 8U;

    /* Initialize WBE config */
    gOX3A10TunerConfig.parseCfg.parseWBCfg.enByteBufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.enByte.offset = 488U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.enByte.mask = 0x81U;
    gOX3A10TunerConfig.parseCfg.parseWBCfg.enByte.shift = 0U;

    /* Initialize Analog Gain (Gain1) config */
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].numBytes = 2U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].useTable = 0U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].shift = 0x2U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[0].offset = 168U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[0].mask = 0xfcU;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[0].shift = 0x82U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[1].offset = 136U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[1].mask = 0x1fU;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[0].bytes[1].shift = 6U;

    /* Initialize Digital Gain config */
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].numBytes = 3U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].shift = 0x82U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[0].offset = 264U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[0].mask = 0xc0U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[0].shift = 0x86U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[1].offset = 232U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[1].mask = 0xffU;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[1].shift = 0x2U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[2].offset = 200U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[2].mask = 0x0fU;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[1].bytes[2].shift = 10U;

    /* Initialize CG config */
    gOX3A10TunerConfig.parseCfg.parseGainCfg[2].bufIdx = 0U;
    gOX3A10TunerConfig.parseCfg.parseGainCfg[2].numBytes = 0U;

    /* Initialize ED CRC config */
    if (1280U == aConfig->imgWidth) {
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edFormat = CSN_MODULE_ED_FORMAT_TAGGED;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.crcType = CSN_MODULE_ED_CRC_FORMAT_32C;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.numCRCs = 1U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].channelIdx = CSN_MODULE_EXP_IDX_LONG;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].edLayout.startOffset = 0x48U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].edLayout.jumpOffset = 0x20U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].edLayout.numBytes = 14U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.order = CSN_MODULE_CRC_ORDER_LE;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.startOffset = 0x9e0U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.jumpOffset = 0x8U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.numBytes = 4U;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.poly = 0x1EDC6F41UL;
        gOX3A10TunerConfig.parseCfg.parseEECRCCfg.edCrcLayout[0].crcLayout.crcInitValue = 0xffffffffUL;
    }

    /* ------------------------------------------------------------------------ */
    /* Get Ops configuration */
    /* Group hold start */
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[0] = 0x3187U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[0] = 0xffU;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[1] = 0x3208U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[1] = 0x0U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[2] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[2] = 0x0U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[3] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[3] = 0x0U;

    /* Group hold end */
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[0] = 0x3208U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[0] = 0x10U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[1] = 0x320DU;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[1] = 0x00U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[2] = 0x3208U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[2] = 0xe0U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[3] = 0x3187U;
    gOX3A10TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[3] = 0x0U;

    /* Gain R Config */
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0x51A7U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0X51A6U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0x5187U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0x5186U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0x51C7U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0x51C6U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;

    /* Gain GR Config */
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Gain GB Config */
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Gain B Config */
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0x51A1U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0X51A0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0x5181U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0x5180U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0x51C1U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0x51C0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOX3A10TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Shutter configuration */
    gOX3A10TunerConfig.getOpsCfg.expCfg[0].addr[0] = 0x3502U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[0].addr[1] = 0x3501U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[0].linesMin = 0U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[0].linesMax = 0xffffU;

    gOX3A10TunerConfig.getOpsCfg.expCfg[1].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.expCfg[1].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.expCfg[1].linesMin = 0U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[1].linesMax = 0xffffU;

    gOX3A10TunerConfig.getOpsCfg.expCfg[2].addr[0] = 0x3582U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[2].addr[1] = 0x3581U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[2].linesMin = 3U;
    gOX3A10TunerConfig.getOpsCfg.expCfg[2].linesMax = 16U;

    /* Define Gain Map */
    /* Gain map for AG(gain0) */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].max[0] = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].max[0] = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].max[0] = 4096U;

    /* Gain map for DG(gain1) */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][1].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][1].max[0] = 4095U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][1].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][1].max[0] = 4095U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][1].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][1].max[0] = 4095U;

    /* Gain map for gain2 */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][2].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][2].max[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][2].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][2].max[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][2].min[0] = 256U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][2].max[0] = 256U;

    /* Make the second stage to invalid */
    /* Gain map for AG(gain0) */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for DG(gain1) */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for gain2 */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain0 configuration */
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[0] = 0x3549U;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[1] = 0x3548U;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[0] = 0x3509U;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[1] = 0x3508U;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[0] = 0x3589U;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[1] = 0x3588U;

    /* Gain1 configuration */
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[0] = 0x354cU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[1] = 0x354bU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[2] = 0x354aU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[0] = 0x350cU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[1] = 0x350bU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[2] = 0x350aU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[0] = 0x358cU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[1] = 0x358bU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[2] = 0x358aU;

    /* Gain2 configuration */
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[1] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[0] = 0xffffU;
    gOX3A10TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[1] = 0xffffU;

    /* program multipliers for gain */
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][0].ratio = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][1].ratio = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[0][2].ratio = 4096U;

    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][0].ratio = 3855U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][1].ratio = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[1][2].ratio = 4096U;

    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][0].ratio = 36623U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][1].ratio = 4096U;
    gOX3A10TunerConfig.getOpsCfg.gainParamCfg[2][2].ratio = 4096U;

    return ret;
}
#endif /* ENABLE_CAM_MODULE_OX3A10 */
#ifdef ENABLE_CAM_MODULE_AR0143
static CSN_MODULE_TunerCfgType gAR143TunerConfig;

/* CG codes */
static const uint16_t gAR143_CGCodes[8] = {0U, 0x7U};
static const uint16_t gAR143_CGVals[8]  = {256U, 0x02E0U};
static const uint8_t gAR143_NumCGVals = 2U;

/* AG codes */
static const uint16_t gAR143_AGCodes[8] = {0x777U, 0x888U, 0x999U, 0xaaaU, 0xbbbU, 0xcccU, 0xdddU, 0xeeeU};
static const uint16_t gAR143_AGVals[8]  = {0x0100U, 0x0140U, 0x0180U, 0x0200U, 0x0255U, 0x0380U, 0x0400U, 0x0800U};
static const uint8_t gAR143_NumAGVals = 8U;

static int32_t CSN_MODULE_MakeAR0143Config(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    /* ---------------------------------------------------------------------- */
    /* Parsing configuration */
    /* Initialize FN config */
    gAR143TunerConfig.parseCfg.parseFnCfg.bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseFnCfg.numBytes = 4U;
    gAR143TunerConfig.parseCfg.parseFnCfg.shift = 0U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[0].offset = 0x18U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[0].shift = 0U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[1].offset = 0x14U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[1].shift = 8U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[2].offset = 0x10U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[2].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[2].shift = 16U;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[3].offset = 0x0cU;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[3].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseFnCfg.bytes[3].shift = 24U;

    /* Initialize Exposure config */
    gAR143TunerConfig.parseCfg.parseExpCfg.bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseExpCfg.numBytes = 2U;
    gAR143TunerConfig.parseCfg.parseExpCfg.shift = 0U;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[0].offset = 0x5f0U;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[0].shift = 0U;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[1].offset = 0x5ec;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseExpCfg.bytes[1].shift = 8U;

    /* Initialize RGain config */
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].numBytes = 2U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].shift = 0x1U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].offset = 0x0708U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].shift = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].offset = 0x0704U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].shift = 8U;

    /* Initialize GGain config */
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].numBytes = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].numBytes = 0U;

    /* Initialize BGain config */
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].numBytes = 2U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].shift = 0x1U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].offset = 0x0700U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].shift = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].offset = 0x06fcU;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].shift = 8U;

    /* WB offset configuration */
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.numBytes = 0U;

    /* Initialize WBE config */
    gAR143TunerConfig.parseCfg.parseWBCfg.enByteBufIdx = 0U;
    if (1280U == aConfig->imgWidth) {
        gAR143TunerConfig.parseCfg.parseWBCfg.enByte.offset = 0xbf8U;
    } else {
        /* 1344 */
        gAR143TunerConfig.parseCfg.parseWBCfg.enByte.offset = 0xc78U;
    }
    gAR143TunerConfig.parseCfg.parseWBCfg.enByte.mask = 0x10U;
    gAR143TunerConfig.parseCfg.parseWBCfg.enByte.shift = 0U;

    /* Initialize CG config */
    gAR143TunerConfig.parseCfg.parseGainCfg[0].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[0].numBytes = 1U;
    gAR143TunerConfig.parseCfg.parseGainCfg[0].shift = 0x0U;
    if(1280U == aConfig->imgWidth) {
        gAR143TunerConfig.parseCfg.parseGainCfg[0].bytes[0].offset = 0x1618U;
    } else {
        /* 1344 */
        gAR143TunerConfig.parseCfg.parseGainCfg[0].bytes[0].offset = 0x1718U;
    }
    gAR143TunerConfig.parseCfg.parseGainCfg[0].bytes[0].mask = 0x0fU;
    gAR143TunerConfig.parseCfg.parseGainCfg[0].bytes[0].shift = 0x0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[0].useTable = 1U;

    /* Initialize Analog Gain (Gain1) config */
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].numBytes = 2U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].shift = 0x0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].useTable = 2U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[0].offset = 0x0720U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[0].shift = 0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[1].offset = 0x071cU;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseGainCfg[1].bytes[1].shift = 8U;

    /* Initialize Digital Gain config */
    gAR143TunerConfig.parseCfg.parseGainCfg[2].bufIdx = 0U;
    gAR143TunerConfig.parseCfg.parseGainCfg[2].numBytes = 2U;
    gAR143TunerConfig.parseCfg.parseGainCfg[2].shift = 0x81U;
    if(1280U == aConfig->imgWidth) {
        gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[0].offset = 0x14b0U;
    } else {
        /* 1344 */
        gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[0].offset = 0x15b0U;
    }
    gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[0].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[0].shift = 0U;
    if(1280U == aConfig->imgWidth) {
        gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[1].offset = 0x14acU;
    } else {
        /* 1344 */
        gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[1].offset = 0x15acU;
    }
    gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[1].mask = 0xffU;
    gAR143TunerConfig.parseCfg.parseGainCfg[2].bytes[1].shift = 0x8U;

    /* CG Table config */
    if (NULL ==
        memcpy(&gAR143TunerConfig.parseCfg.gainTable[0].code,
               &gAR143_CGCodes,
               sizeof(gAR143TunerConfig.parseCfg.gainTable[0].code))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    if (NULL ==
        memcpy(&gAR143TunerConfig.parseCfg.gainTable[0].val,
               &gAR143_CGVals,
               sizeof(gAR143TunerConfig.parseCfg.gainTable[0].val))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;

    }
    gAR143TunerConfig.parseCfg.gainTable[0].mask = 0xfU;;
    gAR143TunerConfig.parseCfg.gainTable[0].numVals = gAR143_NumCGVals;

    /* AG Table config */
    if (NULL ==
        memcpy(&gAR143TunerConfig.parseCfg.gainTable[1].code,
               &gAR143_AGCodes,
               sizeof(gAR143TunerConfig.parseCfg.gainTable[1].code))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    if (NULL ==
        memcpy(&gAR143TunerConfig.parseCfg.gainTable[1].val,
               &gAR143_AGVals,
               sizeof(gAR143TunerConfig.parseCfg.gainTable[1].val))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    gAR143TunerConfig.parseCfg.gainTable[1].mask = 0xfU;;
    gAR143TunerConfig.parseCfg.gainTable[1].numVals = gAR143_NumAGVals;

    /* ------------------------------------------------------------------------ */
    /* Get Ops configuration */
    /* Group hold start */
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[0] = 0x3022U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[1] = 0x0U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[2] = 0x0U;

    /* Group hold end */
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[0] = 0x3022U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[0] = 0x0U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[1] = 0x0U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[2] = 0x0U;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[3] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[3] = 0x0U;

    /* Gain R Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0x305bU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0x305aU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;

    /* Gain GR Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;

    /* Gain GB Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;

    /* Gain B Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0x3059U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0x3058U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Gain R offset Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;

    /* Gain GR offset Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;

    /* Gain GB offset Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;

    /* Gain B offset Config */
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gAR143TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;

    /* Shutter configuration */
    gAR143TunerConfig.getOpsCfg.expCfg[0].addr[0] = 0x3013U;
    gAR143TunerConfig.getOpsCfg.expCfg[0].addr[1] = 0x3012U;
    gAR143TunerConfig.getOpsCfg.expCfg[0].linesMin = 0U;
    gAR143TunerConfig.getOpsCfg.expCfg[0].linesMax = 0xffffU;

    gAR143TunerConfig.getOpsCfg.expCfg[1].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.expCfg[1].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.expCfg[1].linesMin = 0U;
    gAR143TunerConfig.getOpsCfg.expCfg[1].linesMax = 0U;

    gAR143TunerConfig.getOpsCfg.expCfg[2].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.expCfg[2].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.expCfg[2].linesMin = 0U;
    gAR143TunerConfig.getOpsCfg.expCfg[2].linesMax = 0U;

    /* Define Gain Map */
    /* Gain map for CG(gain0) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][0].min[0] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][0].max[0] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][0].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][0].max[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][0].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][0].max[0] = 0U;

    /* Gain map for AG(gain1) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][1].min[0] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][1].max[0] = 0x0400U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][1].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][1].max[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][1].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][1].max[0] = 0U;

    /* Gain map for gain2(DG) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][2].min[0] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][2].max[0] = 512U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][2].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][2].max[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][2].min[0] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][2].max[0] = 0U;

    /* Make the second stage */
    /* Gain map for CG(gain0) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0x02E0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0x02E0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for AG(gain1) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][1].min[1] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][1].max[1] = 0x0800U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][1].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][1].max[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][1].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][1].max[1] = 0U;

    /* Gain map for gain2(DG) */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][2].min[1] = 256U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][2].max[1] = 1023U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][2].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][2].max[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][2].min[1] = 0U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][2].max[1] = 0U;

    /* Gain0 configuration */
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[0] = 0x3363U;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[1] = 0x3362U;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[1] = 0xffffU;

    /* Gain1 configuration */
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[0] = 0x3367U;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[1] = 0x3366U;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[1] = 0xffffU;

    /* Gain2 configuration */
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[0] = 0x3309u;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[1] = 0x3308u;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[1] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[0] = 0xffffU;
    gAR143TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[1] = 0xffffU;

    /* program multipliers for gain */
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][0].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][1].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[0][2].ratio = 4096U;

    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][0].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][1].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[1][2].ratio = 4096U;

    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][0].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][1].ratio = 4096U;
    gAR143TunerConfig.getOpsCfg.gainParamCfg[2][2].ratio = 4096U;

func_exit:
    return ret;
}
#endif /* ENABLE_CAM_MODULE_AR0143 */

#ifdef ENABLE_CAM_MODULE_OV10640
static CSN_MODULE_TunerCfgType gOV10640TunerConfig;

static const uint16_t gOV10640_AGCodes[8] = {0x0U, 0x1U, 0x2U, 0x4U};
static const uint16_t gOV10640_AGVals[8]  = {256U, 512U, 1024U, 2048U};
static const uint8_t gOV10640_NumAGVals = 4U;

static int32_t CSN_MODULE_MakeOV10640Config(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    /* ---------------------------------------------------------------------- */
    /* Parsing configuration */
    /* Initialize FN config */
    gOV10640TunerConfig.parseCfg.parseFnCfg.bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.numBytes = 4U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.shift = 0U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[0].offset = 478U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[1].offset = 475U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[1].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[1].shift = 8U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[2].offset = 472U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[2].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[2].shift = 16U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[3].offset = 469U;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[3].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseFnCfg.bytes[3].shift = 24U;

    /* Initialize Exposure config */
    gOV10640TunerConfig.parseCfg.parseExpCfg.bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.numBytes = 2U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.shift = 0U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[0].offset = 694U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[1].offset = 691U;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[1].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseExpCfg.bytes[1].shift = 8U;

    /* Initialize RGain config */
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].numBytes = 2U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].shift = 0x0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].offset = 1369U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].offset = 1366U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].shift = 8U;

    /* Initialize GGain config */
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].numBytes = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].numBytes = 0U;

    /* Initialize BGain config */
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].numBytes = 2U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].shift = 0x0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].offset = 1363U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].offset = 1360U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].shift = 8U;

    /* WB offset configuration */
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.numBytes = 0U;

    /* Initialize WBE config */
    gOV10640TunerConfig.parseCfg.parseWBCfg.enByteBufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.enByte.offset = 886U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.enByte.mask = 0x3U;
    gOV10640TunerConfig.parseCfg.parseWBCfg.enByte.shift = 0U;

    /* Initialize Analog Gain config */
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].numBytes = 1U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].shift = 0x0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].bytes[0].offset = 706U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].bytes[0].mask = 0x3U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[0].useTable = 1U;

    /* Initialize Digital Gain config */
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].numBytes = 2U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].shift = 0x0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[0].offset = 712U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[1].offset = 709U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[1].mask = 0x3fU;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].bytes[1].shift = 0x8U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[1].useTable = 0U;

    /* Initialize Digital Gain config */
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bufIdx = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].numBytes = 2U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].shift = 0x81U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[0].offset = 0x14b0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[0].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[0].shift = 0U;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[1].offset = 0x14acU;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[1].mask = 0xffU;
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].bytes[1].shift = 0x8U;

    /* Initialize CG config */
    gOV10640TunerConfig.parseCfg.parseGainCfg[2].numBytes = 0U;

    /* AG Table config */
    if (NULL ==
        memcpy(&gOV10640TunerConfig.parseCfg.gainTable[0].code,
               &gOV10640_AGCodes,
               sizeof(gOV10640TunerConfig.parseCfg.gainTable[0].code))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    if (NULL ==
        memcpy(&gOV10640TunerConfig.parseCfg.gainTable[0].val,
               &gOV10640_AGVals,
               sizeof(gOV10640TunerConfig.parseCfg.gainTable[0].val))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    gOV10640TunerConfig.parseCfg.gainTable[0].mask = 0xfU;;
    gOV10640TunerConfig.parseCfg.gainTable[0].numVals = gOV10640_NumAGVals;

    /* ------------------------------------------------------------------------ */
    /* Get Ops configuration */
    /* Group hold start */
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[0] = 0x3028U;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[0] = 0x40U;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[1] = 0x302fU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[1] = 0x0U;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[2] = 0x302cU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[2] = 0x0U;

    /* Group hold end */
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[0] = 0x302CU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[0] = 0x20UL;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[1] = 0x302FU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[1] = 0x1UL;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[2] = 0x0U;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[3] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[3] = 0x0U;

    /* Gain R Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xb1c8U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xb1c7U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xb1d0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xb1cfU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;

    /* Gain GR Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;

    /* Gain GB Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;

    /* Gain B Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xb1c6U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xb1c5U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xb1ceU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xb1cdU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Gain R offset Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 0U;

    /* Gain GR offset Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;

    /* Gain GB offset Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;

    /* Gain B offset Config */
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 0U;
    gOV10640TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 0U;

    /* Shutter configuration */
    gOV10640TunerConfig.getOpsCfg.expCfg[0].addr[0] = 0xB0e7U;
    gOV10640TunerConfig.getOpsCfg.expCfg[0].addr[1] = 0xB0e6U;
    gOV10640TunerConfig.getOpsCfg.expCfg[0].linesMin = 0U;
    gOV10640TunerConfig.getOpsCfg.expCfg[0].linesMax = 0xffffU;

    gOV10640TunerConfig.getOpsCfg.expCfg[1].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.expCfg[1].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.expCfg[1].linesMin = 0U;
    gOV10640TunerConfig.getOpsCfg.expCfg[1].linesMax = 0U;

    gOV10640TunerConfig.getOpsCfg.expCfg[2].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.expCfg[2].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.expCfg[2].linesMin = 0U;
    gOV10640TunerConfig.getOpsCfg.expCfg[2].linesMax = 0U;

    /* Define Gain Map */
    /* Gain map for AG(gain0) */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][0].min[0] = 256U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][0].max[0] = 2048U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][0].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][0].max[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][0].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][0].max[0] = 0U;

    /* Gain map for DG(gain1) */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][1].min[0] = 256U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][1].max[0] = 0x3fffU;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][1].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][1].max[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][1].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][1].max[0] = 0U;

    /* Gain map for gain2 */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][2].min[0] = 256U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][2].max[0] = 256U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][2].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][2].max[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][2].min[0] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][2].max[0] = 0U;

    /* Make the second stage invalid */
    /* Gain map for AG(gain0) */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for DG(gain1) */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][1].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][1].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][1].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][1].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][1].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][1].max[1] = 0U;

    /* Gain map for gain2 */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][2].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][2].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][2].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][2].max[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][2].min[1] = 0U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][2].max[1] = 0U;

    /* Gain0 configuration */
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[0] = 0xb0eb;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[1] = 0xffffU;

    /* Gain1 configuration */
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[0] = 0xb0edU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[1] = 0xb0ecU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[1] = 0xffffU;

    /* Gain2 configuration */
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[1] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[0] = 0xffffU;
    gOV10640TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[1] = 0xffffU;

    /* program multipliers for gain */
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][0].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][1].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[0][2].ratio = 4096U;

    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][0].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][1].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[1][2].ratio = 4096U;

    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][0].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][1].ratio = 4096U;
    gOV10640TunerConfig.getOpsCfg.gainParamCfg[2][2].ratio = 4096U;

func_exit:
    return ret;
}
#endif /* ENABLE_CAM_MODULE_OV10640 */

#ifdef ENABLE_CAM_MODULE_OV9716
static CSN_MODULE_TunerCfgType gOV9716TunerConfig;

static const uint16_t gOV9716_AGCodes[8] = {0x0U, 0x15U, 0x2aU, 0x3fU};
static const uint16_t gOV9716_AGVals[8]  = {256U, 512U, 1024U, 2048U};
static const uint8_t gOV9716_NumAGVals = 4U;

static int32_t CSN_MODULE_MakeOV9716Config(CSN_MODULE_ConfigType *aConfig)
{
    int32_t ret = BCM_ERR_OK;

    /* ---------------------------------------------------------------------- */
    /* Parsing configuration */
    /* Initialize FN config */
    gOV9716TunerConfig.parseCfg.parseFnCfg.bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.numBytes = 2U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.shift = 0U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[0].offset = 62U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[1].offset = 58U;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[1].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseFnCfg.bytes[1].shift = 8U;

    /* Initialize Exposure config */
    gOV9716TunerConfig.parseCfg.parseExpCfg.bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.numBytes = 2U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.shift = 0U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[0].offset = 6U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[1].offset = 2U;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[1].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseExpCfg.bytes[1].shift = 8U;

    /* Initialize RGain config */
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].numBytes = 2U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].offset = 2658U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].offset = 2654U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_R].bytes[1].shift = 8U;

    /* Initialize GGain config */
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].numBytes = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].numBytes = 0U;

    /* Initialize BGain config */
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].numBytes = 2U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].offset = 2682U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].offset = 2678U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_B].bytes[1].shift = 8U;

    /* WB offset configuration */
    /* Initialize rGain/bGain offset config */
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.numBytes = 3U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.shift = 0x0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[0].offset = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[0].shift = 0x0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[1].offset = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[1].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[1].shift = 8U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[2].offset = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[2].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseWBCfg.parseWBOffsetCfg.bytes[2].shift = 16U;

    /* Initialize WBE config */
    gOV9716TunerConfig.parseCfg.parseWBCfg.enByteBufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.enByte.offset = 1566U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.enByte.mask = 0x33U;
    gOV9716TunerConfig.parseCfg.parseWBCfg.enByte.shift = 0U;

    /* Initialize Analog Gain (Gain1) config */
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].numBytes = 1U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].useTable = 1U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].bytes[0].offset = 30U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseGainCfg[0].bytes[0].shift = 0U;

    /* Initialize Digital Gain config */
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].numBytes = 2U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[0].offset = 674U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[0].mask = 0xffU;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[0].shift = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[1].offset = 670U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[1].mask = 0x3fU;
    gOV9716TunerConfig.parseCfg.parseGainCfg[1].bytes[1].shift = 0x8U;

    /* Initialize CG config */
    gOV9716TunerConfig.parseCfg.parseGainCfg[2].bufIdx = 0U;
    gOV9716TunerConfig.parseCfg.parseGainCfg[2].numBytes = 0U;

    /* AG Table config */
    if (NULL ==
        memcpy(&gOV9716TunerConfig.parseCfg.gainTable[0].code,
               &gOV9716_AGCodes,
               sizeof(gOV9716TunerConfig.parseCfg.gainTable[0].code))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    if (NULL ==
        memcpy(&gOV9716TunerConfig.parseCfg.gainTable[0].val,
               &gOV9716_AGVals,
               sizeof(gOV9716TunerConfig.parseCfg.gainTable[0].val))) {
        ret = BCM_ERR_UNKNOWN;
        goto func_exit;
    }
    gOV9716TunerConfig.parseCfg.gainTable[0].mask = 0x3U;
    gOV9716TunerConfig.parseCfg.gainTable[0].numVals = gOV9716_NumAGVals;

  /* ------------------------------------------------------------------------ */
    /* Get Ops configuration */
    /* Group hold start */
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[0] =  0X3467U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[0] = 0x0U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[1] = 0X3464U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[1] = 0x0U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[2] = 0x0U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].addr[3] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_START].val[3] = 0x0U;

    /* Group hold end */
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[0] = 0X3464U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[0] = 64U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[1] = 0X3467U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[1] = 1U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[2] = 0x0U;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].addr[3] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.ghCfg[CSN_MODULE_GROUP_HOLD_IDX_END].val[3] = 0x0U;

    /* Gain R Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB369U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB368U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB361U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB360U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB371U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB370U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_R].max = 0xffffU;

    /* Gain GR Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GR].max = 0xffffU;

    /* Gain GB Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_GB].max = 0xffffU;

    /* Gain B Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB36FU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0XB36EU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB367U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xB366U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB377U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xB376U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].min = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbGainCfg[CSN_MODULE_WG_GAIN_IDX_B].max = 0xffffU;

    /* Gain R offset Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB386U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB385U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xB384U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 256U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB37AU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB379U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xB378U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 256U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[0] = 0xB392U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[1] = 0xB391U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].addr[2] = 0xB390U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_R].sub = 256U;

    /* Gain GR offset Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GR].sub = 0U;

    /* Gain GB offset Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].addr[2] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].mul = 0U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_GB].sub = 0U;

    /* Gain B offset Config */
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB38FU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xB38eU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xB38dU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[0].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 256U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB383U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xB382U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xB381U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[1].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 256U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[0] = 0xB39BU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[1] = 0xB39aU;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].addr[2] = 0xB399U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].mul = 64U;
    gOV9716TunerConfig.getOpsCfg.wbCfg[2].wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_B].sub = 256U;

    /* Shutter configuration */
    gOV9716TunerConfig.getOpsCfg.expCfg[0].addr[0] = 0xB0B7U;
    gOV9716TunerConfig.getOpsCfg.expCfg[0].addr[1] = 0xB0B6U;
    gOV9716TunerConfig.getOpsCfg.expCfg[0].linesMin = 0U;
    gOV9716TunerConfig.getOpsCfg.expCfg[0].linesMax = 0xffffU;

    gOV9716TunerConfig.getOpsCfg.expCfg[1].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.expCfg[1].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.expCfg[1].linesMin = 0U;
    gOV9716TunerConfig.getOpsCfg.expCfg[1].linesMax = 0xffffU;

    gOV9716TunerConfig.getOpsCfg.expCfg[2].addr[0] = 0xB0B9U;
    gOV9716TunerConfig.getOpsCfg.expCfg[2].addr[1] = 0xB0B8U;
    gOV9716TunerConfig.getOpsCfg.expCfg[2].linesMin = 1U;
    gOV9716TunerConfig.getOpsCfg.expCfg[2].linesMax = 20U;

    /* Define Gain Map */
    /* Gain map for AG(gain0) */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].max[0] = 2048U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].max[0] = 2048U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].max[0] = 2048U;

    /* Gain map for DG(gain1) */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][1].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][1].max[0] = 2047U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][1].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][1].max[0] = 2047U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][1].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][1].max[0] = 2047U;

    /* Gain map for gain2 */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][2].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][2].max[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][2].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][2].max[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][2].min[0] = 256U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][2].max[0] = 256U;

    /* Make the second stage to invalid */
    /* Gain map for AG(gain0) */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for DG(gain1) */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain map for gain2 */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].max[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].min[1] = 0U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].max[1] = 0U;

    /* Gain0 configuration */
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[0] = 0xB0BBU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][0].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][0].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][0].addr[1] = 0xffffU;

    /* Gain1 configuration */
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[0] = 0xB165U;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][1].addr[1] = 0xB164U;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[0] = 0xB163U;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][1].addr[1] = 0xB162U;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[0] = 0xB167U;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][1].addr[1] = 0xB166U;

    /* Gain2 configuration */
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[0][2].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[1][2].addr[1] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[0] = 0xffffU;
    gOV9716TunerConfig.getOpsCfg.gainRegCfg[2][2].addr[1] = 0xffffU;

    /* program multipliers for gain */
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][0].ratio = 4096U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][1].ratio = 4096U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[0][2].ratio = 4096U;

    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][0].ratio = 4096U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][1].ratio = 38720U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[1][2].ratio = 4096U;

    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][0].ratio = 4096U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][1].ratio = 4096U;
    gOV9716TunerConfig.getOpsCfg.gainParamCfg[2][2].ratio = 4096U;

func_exit:
    return ret;
}
#endif /* ENABLE_CAM_MODULE_OV9716 */

extern CSN_MODULE_TunerCfgType * CSN_MODULE_GetStaticConfig(CSN_MODULE_ConfigType *aConfig)
{
    CSN_MODULE_TunerCfgType *cfg = NULL;

#ifdef ENABLE_CAM_MODULE_OX3A10
    if (BCM_ERR_OK == CSN_MODULE_MakeOX3A10Config(aConfig)) {
        cfg = &gOX3A10TunerConfig;
    }
#endif /* ENABLE_CAM_MODULE_OX3A10 */

#ifdef ENABLE_CAM_MODULE_AR0143
    if (BCM_ERR_OK == CSN_MODULE_MakeAR0143Config(aConfig)) {
        cfg = &gAR143TunerConfig;
    }
#endif /* ENABLE_CAM_MODULE_AR0143 */

#ifdef ENABLE_CAM_MODULE_OV10640
    if (BCM_ERR_OK == CSN_MODULE_MakeOV10640Config(aConfig)) {
        cfg = &gOV10640TunerConfig;
    }
#endif /* ENABLE_CAM_MODULE_OV10640 */

#ifdef ENABLE_CAM_MODULE_OV9716
    if (BCM_ERR_OK == CSN_MODULE_MakeOV9716Config(aConfig)) {
        cfg = &gOV9716TunerConfig;
    }
#endif /* ENABLE_CAM_MODULE_OV9716 */

    return cfg;
}
