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
File Name:  sensor_syntax_table.c
Descritpion: This file contains sensor syntax table.
******************************************************************************/
#include <stdio.h>
#include "parser.h"
#include "sensor_image_util.h"
#include "isys_osil.h"

/* Globals */
static RegData gBlockregData[MAX_BUF_SIZE];
static RegData gModeregData[MAX_NUM_MODES][MAX_BUF_SIZE];
static RegData gStartregData[MAX_BUF_SIZE];
static RegData gStopregData[MAX_BUF_SIZE];

RegDataConfig gBlockConfig = {(RegData *)&gBlockregData, 0x0UL};
/* Supports 10 sensor mode configs */
RegDataConfig gModeConfig[MAX_NUM_MODES] = {
                                            {(RegData *)&gModeregData[0], 0x0UL},
                                            {(RegData *)&gModeregData[1], 0x0UL},
                                            {(RegData *)&gModeregData[2], 0x0UL},
                                            {(RegData *)&gModeregData[3], 0x0UL},
                                            {(RegData *)&gModeregData[4], 0x0UL},
                                            {(RegData *)&gModeregData[5], 0x0UL},
                                            {(RegData *)&gModeregData[6], 0x0UL},
                                            {(RegData *)&gModeregData[7], 0x0UL},
                                            {(RegData *)&gModeregData[8], 0x0UL},
                                            {(RegData *)&gModeregData[9], 0x0UL}
                                           };
RegDataConfig gStartConfig = {(RegData *)&gStartregData, 0x0UL};
RegDataConfig gStopConfig = {(RegData *)&gStopregData, 0x0UL};

CSN_ImageHdrType gImgHdr;
/* For Unicam */
CamSensorParams gSensorParams = {0};
/* Sensor Name */
char gSensorName[100];

/* Enum table */
const char *EndianEnumName[] = {
    "LE",
    "BE"
};

int32_t EndianEnumValue[] = {
    eLitteEndian,
    eBigEndian
};

EnumReadInfo EndianEnum = {EndianEnumName, EndianEnumValue};

/* Enum table for interface */
const char *PeripheralModeEnumName[] = {
    "CPI",
    "CSI2",
    "TG",
    "HISPI",
};

int32_t PeripheralModeEnumValue[] = {
    eCpi,
    eCsi2,
    eTg,
    eHispi,
};

EnumReadInfo PeripheralMode = {PeripheralModeEnumName, PeripheralModeEnumValue};

/* Enable & disable */
typedef enum {
    eFalse = 0,
    eTrue = 1
} EEnableDisable;

const char *EnableDisableEnumName[] = {
    "FALSE",
    "TRUE"
};

int32_t EnableDisableEnumValue[] = {
    eFalse,
    eTrue
};

EnumReadInfo EnableDisable = {EnableDisableEnumName, EnableDisableEnumValue};

/* Input format */
const char *InputFormatEnumName[] = {
    "RAW6",
    "RAW7",
    "RAW8",
    "RAW10",
    "RAW12",
    "RAW14",
    "RAW16"
};

int32_t InputFormatEnumValue[] = {
    eRaw6,
    eRaw7,
    eRaw8,
    eRaw10,
    eRaw12,
    eRaw14,
    eRaw16
};

EnumReadInfo InputFormat = {InputFormatEnumName, InputFormatEnumValue};

const char *UnpackEnumName[] = {
    "NONE",
    "RAW6",
    "RAW7",
    "RAW8",
    "RAW12",
    "RAW10",
    "RAW14",
    "RAW16",
    "RAW20"
};

int32_t UnpackEnumValue[] = {
    eUnPackNone,
    eUnPackRaw6,
    eUnPackRaw7,
    eUnPackRaw8,
    eUnPackRaw10,
    eUnPackRaw12,
    eUnPackRaw14,
    eUnPackRaw16,
    eUnPackRaw20
};

EnumReadInfo Unpack = {UnpackEnumName, UnpackEnumValue};

const char *DpcmEnumName[] = {
    "NONE",
    "DPCM_8_10",
    "DPCM_7_10",
    "DPCM_6_10",
    "DPCM_8_12",
    "DPCM_7_12",
    "DPCM_6_12",
    "DPCM_10_14",
    "DPCM_8_14",
    "DPCM_12_16",
    "DPCM_10_16",
    "DPCM_8_16"
};

int32_t DpcmEnumValue[] = {
    eDpcmNone,
    eDpcm_8_10,
    eDpcm_7_10,
    eDpcm_6_10,
    eDpcm_8_12,
    eDpcm_7_12,
    eDpcm_6_12,
    eDpcm_10_14,
    eDpcm_8_14,
    eDpcm_12_16,
    eDpcm_10_16,
    eDpcm_8_16
};

EnumReadInfo Dpcm = {DpcmEnumName, DpcmEnumValue};

/* Syntax table */
SyntaxEntry SensorDetailsSyntax[] = {
    {"sensor_details", eChar, 0, 1, NULL, NULL, 0, 0},
    {"sensor_name", eChar, 0, 1, (void *)&gSensorName[0], NULL, 0, 0},
    {"slave_addr", eHex, 0, 1, (void *)&gImgHdr.slaveAddr, NULL, 0, 0},
    {"addr_width", eUint32, 0, 1, (void *)&gImgHdr.addrWidth, NULL, 8, 32},
    {"data_width", eUint32, 0, 1, (void *)&gImgHdr.dataWidth, NULL, 8, 32},
    {"endianness", eEnum, 0, 1, (void *)&gImgHdr.endianness, &EndianEnum, 0, 0},
    {"i2c_port", eUint32, 0, 1, (void *)&gImgHdr.i2cPort, NULL, 0, 4},
    {"i2c_frequency", eUint32, 0, 1, (void *)&gImgHdr.i2cFrequency, NULL, 0, 400},
    {"sensor_frequency", eUint32, 0, 1, (void *)&gImgHdr.sensorFrequency, NULL, 0, 400000000},
    {"is_10bit_slave", eEnum, 0, 1, (void *)&gImgHdr.is_10bitSlave, &EnableDisable, 0, 0},
    {"restart_required", eEnum, 0, 1, (void *)&gImgHdr.is_RestartRequired, &EnableDisable, 0, 0},
    {"interface", eEnum, 0, 1, (void *)&gSensorParams.ifMode, &PeripheralMode, 0, 0},
    { }
};

SyntaxEntry SensorModeSyntax[] = {
    {"common_config", eHexData, 0, 1, (void *)&gBlockConfig, NULL, 0, 0},
    {"start_config", eHexData, 0, 1, (void *)&gStartConfig, NULL, 0, 0},
    {"stop_config", eHexData, 0, 1, (void *)&gStopConfig, NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[0], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[1], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[2], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[3], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[4], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[5], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[6], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[7], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[8], NULL, 0, 0},
    {"mode_", eHexData, 0, 1, (void *)&gModeConfig[9], NULL, 0, 0},
    {}
};

/******************************************************************************
FUNCTION NAME: GetSensorDetailsSyntax

ARGUMENTS: None

DESCRIPTION: This function gets sensor details syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetSensorDetailsSyntax(void)
{
    return SensorDetailsSyntax;
}

/******************************************************************************
FUNCTION NAME: GetSensorModeSyntax

ARGUMENTS: None

DESCRIPTION: This function gets sensor mode syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetSensorModeSyntax(void)
{
    return SensorModeSyntax;
}

SyntaxEntry SensorModeDetailsSyntax[] = {
    {"common_mode_details", eChar, 0, 1, NULL, NULL, 0, 0},
    {"width", eUint32, 0, 1, (void *)&gSensorParams.imgWidth, NULL, 0, 1920},
    {"height", eUint32, 0, 1, (void *)&gSensorParams.imgHeight, NULL, 0, 1280},
    {"format", eEnum, 0, 1, (void *)&gSensorParams.format, &InputFormat, 0, 0},
    {"dpcm_decode", eEnum, 0, 1, (void *)&gSensorParams.dpcmDecode, &Dpcm, 0, 0},
    {"unpack", eEnum, 0, 1, (void *)&gSensorParams.unPack, &Unpack, 0, 0},
    {"fps_low", eUint32, 0, 1, (void *)&gImgHdr.fpsLow, NULL, 0, 60},
    {"fps_high", eUint32, 0, 1, (void *)&gImgHdr.fpsHigh, NULL, 0, 60},
    {"line_length", eUint32, 0, 1, (void *)&gImgHdr.lineLength, NULL, 0, 0xffffffffUL},
    {"integration_min", eUint32, 0, 1, (void *)&gImgHdr.integrationMin, NULL, 0, 10},
    {"coarse_integration_offset", eUint32, 0, 1, (void *)&gImgHdr.coarseIntegrationOffset, NULL, 0, 100},
    {"min_frame_integration_diff", eUint32, 0, 1, (void *)&gImgHdr.minFrameIntegrationDiff, NULL, 0, 50},
    {"hdr", eEnum, 0, 1, (void *)&gSensorParams.isHDREnabled, &EnableDisable, 0, 0},
    {"metadata_before_frame", eUint32, 0, 1, (void *)&gSensorParams.embeddedBefore, NULL, 0, 4},
    {"metadata_after_frame", eUint32, 0, 1, (void *)&gSensorParams.embeddedAfter, NULL, 0, 4},
    {"short_format", eEnum, 0, 1, (void *)&gSensorParams.shortFormat, &InputFormat, 0, 0},
    {"short_metadata_before_frame", eUint32, 0, 1, (void *)&gSensorParams.shortEmbeddedBefore, NULL, 0, 4},
    {"short_metadata_after_frame", eUint32, 0, 1, (void *)&gSensorParams.shortEmbeddedAfter, NULL, 0, 4},
    {"short_line_offset", eUint32, 0, 1, (void *)&gSensorParams.shortLineOffset, NULL, 0, 1280},
    {"long_meta_offset", eUint16, 0, 1, (void *)&gImgHdr.metaOffset[ISYS_SENSOR_EXP_IDX_LONG], NULL, 0, 0xffff},
    {"long_meta_size", eUint16, 0, 1, (void *)&gImgHdr.metaSize[ISYS_SENSOR_EXP_IDX_LONG], NULL, 0, 0xffff},
    {"short_meta_offset", eUint16, 0, 1, (void *)&gImgHdr.metaOffset[ISYS_SENSOR_EXP_IDX_SHORT], NULL, 0, 0xffff},
    {"short_meta_size", eUint16, 0, 1, (void *)&gImgHdr.metaSize[ISYS_SENSOR_EXP_IDX_SHORT], NULL, 0, 0xffff},
    {"long_stats_offset", eUint16, 0, 1, (void *)&gImgHdr.statsOffset[ISYS_SENSOR_EXP_IDX_LONG], NULL, 0, 0xffff},
    {"long_stats_size", eUint16, 0, 1, (void *)&gImgHdr.statsSize[ISYS_SENSOR_EXP_IDX_LONG], NULL, 0, 0xffff},
    {"short_stats_offset", eUint16, 0, 1, (void *)&gImgHdr.statsOffset[ISYS_SENSOR_EXP_IDX_SHORT], NULL, 0, 0xffff},
    {"short_stats_size", eUint16, 0, 1, (void *)&gImgHdr.statsSize[ISYS_SENSOR_EXP_IDX_SHORT], NULL, 0, 0xffff},
    {}
};

const char *CamResyncEdgeEnumName[] = {
    "POS_EDGE",
    "NEG_EDGE"
};

int32_t CamResyncEdgeEnumValue[] = {
    ePosEdge,
    eNegEdge
};

EnumReadInfo CamResyncEdge = {CamResyncEdgeEnumName, CamResyncEdgeEnumValue};

const char *CamActiveLevelEnumName[] = {
    "ACTIVE_HIGH",
    "ACTIVE_LOW"
};

int32_t CamActiveLevelEnumValue[] = {
    eActiveHigh,
    eActiveLow
};

EnumReadInfo CamActiveLevel = {CamActiveLevelEnumName, CamActiveLevelEnumValue};

const char *CamShiftSyncEnumName[] = {
    "NO_SHIFT",
    "LEFT_SHIFT2",
    "RIGHT_SHIFT2",
    "RIGHT_SHIFT6"
};

int32_t CamShiftSyncEnumValue[] = {
    eNoShift,
    eLeftShift2,
    eRigthShift2,
    eRightShift6
};

EnumReadInfo CamShiftSync = {CamShiftSyncEnumName, CamShiftSyncEnumValue};

const char *CamFieldModeEnumName[] = {
    "FIELD_AUTO",
    "FIELD_DERIVED",
    "FIELD_EVEN",
    "FIELD_ODD"
};

int32_t CamFieldModeEnumValue[] = {
    eFieldAuto,
    eFieldDerived,
    eFieldEven,
    eFieldOdd
};

EnumReadInfo CamFieldMode = {CamFieldModeEnumName, CamFieldModeEnumValue};

const char *CamHvSyncModeEnumName[] = {
    "ACTIVE_LINE",
    "START_ACT_LINE",
    "DEF_V_SAL",
    "IGNORED"
};

int32_t CamHvSyncModeEnumValue[] = {
    eActiveLine,
    eStartactLine,
    eDefVsal,
    eIgnored
};

EnumReadInfo CamHvSyncMode = {CamHvSyncModeEnumName, CamHvSyncModeEnumValue};

const char *CamSyncModeEnumName[] = {
    "HV_SYNC",
    "EMBEDDED_SYNC"
};

int32_t CamSyncModeEnumValue[] = {
    eHvSync,
    eEmbeddedSync
};

EnumReadInfo CamSyncMode = {CamSyncModeEnumName, CamSyncModeEnumValue};

SyntaxEntry UnicamCpiSyntax[] = {
    {"interface_details", eChar, 0, 1, NULL, NULL, 0, 0},
    {"data_shift", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.dataShift, NULL, 0, 0},
    {"data_resync", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.dataReSync, &CamResyncEdge, 0, 0},
    {"h_resync", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.hReSync, &CamResyncEdge, 0, 0},
    {"v_resync", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.vReSync, &CamResyncEdge, 0, 0},
    {"hsync_active", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.hSyncActive, &CamActiveLevel, 0, 0},
    {"vsync_active", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.reSync.vSyncActive, &CamActiveLevel, 0, 0},
    {"fild_bit", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.fildBit, NULL, 0, 0},
    {"h_syncbit", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.hSyncBit, NULL, 0, 0},
    {"v_syncBit", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.vSyncBit, NULL, 0, 0},
    {"shift", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.shift, &CamShiftSync, 0, 0},
    {"enb_sync_shift", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.enbSyncShift, NULL, 0, 0},
    {"scmode", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.scMode, NULL, 0, 0},
    {"field_mode", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.fieldMode, &CamFieldMode, 0, 0},
    {"h_syncmode", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.hSyncMode, &CamHvSyncMode, 0, 0},
    {"v_syncmode", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.vSyncMode, &CamHvSyncMode, 0, 0},
    {"sync_mode", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.cpiParams.framing.syncMode, &CamSyncMode, 0, 0},
    {}
};

/* TG Configuration Syntax Table */
const char *CamTgModeEnumName[] = {
    "TC",
    "AXI"
};

int32_t CamTgModeEnumValue[] = {
    eTc,
    eAxi
};

EnumReadInfo CamTgMode = {CamTgModeEnumName, CamTgModeEnumValue};

typedef enum {
    eZero = 0,
    eOne = 1
} EZeroOrOne;

const char *ZeroOrOneEnumName[] = {
    "ZERO",
    "ONE"
};

int32_t ZeroOrOneEnumValue[] = {
    eZero,
    eOne
};

EnumReadInfo ZeroOrOne = {ZeroOrOneEnumName, ZeroOrOneEnumValue};

const char *CamTgBayerEnumName[] = {
    "RGGB",
    "GBRG",
    "BGGR",
    "GRBG"
};

int32_t CamTgBayerEnumValue[] = {
    eRggb,
    eGbrg,
    eBggr,
    eGrbg
};

EnumReadInfo CamTgBayer = {CamTgBayerEnumName, CamTgBayerEnumValue};

SyntaxEntry UnicamTgSyntax[] = {
    {"interface_details", eChar, 0, 1, NULL, NULL, 0, 0},
    {"clock_frequency", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.clkFreq, NULL, 0, 250000000},
    {"pre_blank_period", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.preBlankPeriod, NULL, 0, 65535},
    {"mode", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.mode, &CamTgMode, 0, 0},
    {"cfg_by_pass", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.cfgByPass, &EnableDisable, 0, 0},
    {"chan", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.chan, &ZeroOrOne, 0, 0},
    {"bayer", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.bayerOrder, &CamTgBayer, 0, 0},
    {"distort_enable", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.distortEn, &EnableDisable, 0, 0},
    {"translate_enable", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.ctrl.translateEn, &EnableDisable, 0, 0},
    {"block_y_size", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.blockYSize, NULL, 0, 15},
    {"block_x_size", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.blockXSize, NULL, 0, 15},
    {"blank_prescale", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.blankPrescale, NULL, 0, 65535},
    {"max", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.max, NULL, 0, 255},
    {"min", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.tgParams.min, NULL, 0, 255},
    {}
};

/* CSI Syntax */
const char *CamNumDataLanesEnumName[] = {
    "SINGLE",
    "DUAL",
    "QUAD"
};

int32_t CamNumDataLanesEnumValue[] = {
    eSingleLane,
    eDualLane,
    eQuadLane
};

EnumReadInfo CamNumDataLanes = {CamNumDataLanesEnumName, CamNumDataLanesEnumValue};

const char *CamDataLaneSyncEnumName[] = {
    "SINGLE_BIT_ERROR",
    "LEADER_SEQ"
};

int32_t CamDataLaneSyncEnumValue[] = {
    eSingleBitError,
    eLeaderSeq
};

EnumReadInfo CamDataLaneSync = {CamDataLaneSyncEnumName, CamDataLaneSyncEnumValue};

const char *CamDataLaneEnableEnumName[] = {
    "AUTO",
    "MANUAL"
};

int32_t CamDataLaneEnableEnumValue[] = {
    eAuto,
    eManual
};

EnumReadInfo CamDataLaneEnable = {CamDataLaneEnableEnumName, CamDataLaneEnableEnumValue};

SyntaxEntry UnicamCsi2Syntax[] = {
    {"interface_details", eChar, 0, 1, NULL, NULL, 0, 0},
    {"lp_clk_frequency", eUint32, 0, 1,
        (void *)&gSensorParams.lpClkFrequency, NULL, 0, 100000000},
    {"hs_clk_frequency", eUint32, 0, 1,
        (void *)&gSensorParams.hsClkFrequency, NULL, 0, 1000000000},
    {"tclk_settle", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.clkTime.clkSettle, NULL, 0, 255},
    {"tclk_term_enable", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.clkTime.clkTermEn, NULL, 0, 255},
    {"td_term_enable", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.dataTime.tdTermEn, NULL, 0, 255},
    {"ths_settle", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.dataTime.thsSettle, NULL, 0, 255},
    {"trx_enable", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.dataTime.trxEnable, NULL, 0, 255},
    {"line_start_packet", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.lineStart, &EnableDisable, 0, 0},
    {"line_end_packet", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.lineEnd, &EnableDisable, 0, 0},
    {"packet_farme_timeout", eHex, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.packetFarmeTimeout, NULL, 0, 0xf},
    {"num_data_lanes", eEnum, 0, 1,
        (void *)&gSensorParams.numLanes, &CamNumDataLanes, 0, 0},
    {"lane_sync", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.laneSync, &CamDataLaneSync, 0, 0},
    {"lane_termination", eEnum, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.laneTermination, &CamDataLaneEnable, 0, 0},
    {"long_vc", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.longExpVc, NULL, 0, 255},
    {"short_vc", eUint32, 0, 1,
        (void *)&gSensorParams.frontEnd.csiParams.shortExpVc, NULL, 0, 255},
    {}
};

/******************************************************************************
FUNCTION NAME: GetUnicamDetailsSyntax

ARGUMENTS: None

DESCRIPTION: This function gets Unicam details synatx table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetSensorModeDetailsSyntax(void)
{
    return SensorModeDetailsSyntax;
}

/******************************************************************************
FUNCTION NAME: GetUnicamCpiSyntax

ARGUMENTS: None

DESCRIPTION: This function gets CPi syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamCpiSyntax(void)
{
    return UnicamCpiSyntax;
}

/******************************************************************************
FUNCTION NAME: GetUnicamTgSyntax

ARGUMENTS: None

DESCRIPTION: This function gets Tg syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamTgSyntax(void)
{
    return UnicamTgSyntax;
}

/******************************************************************************
FUNCTION NAME: GetUnicamCsi2Syntax

ARGUMENTS: None

DESCRIPTION: This function gets Csi2 syntax table.

RETURNS: const SyntaxEntry *

******************************************************************************/
const SyntaxEntry *GetUnicamCsi2Syntax(void)
{
    return UnicamCsi2Syntax;
}
