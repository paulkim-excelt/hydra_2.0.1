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
File Name:  unicam_driver.c
Description: This file implements offline unicam driver.
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "unicam_driver.h"
#include "unicam_image_util.h"
#include "brcm_rdb_cam.h"

/* Macro */
#define NANO_SEC (1000000000UL)
/* Max of */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Unicam Params */
typedef struct
{
    union
    {
        TgParams         tgParams;
        CpiParams        cpiParams;
        Csi2Params       csiParams;
        HispiParams      hispiParams;
    } frontEnd;
    Ctrl                 ctrl;
    AnaCtrl              anaCtrl;
    Priority             priorityCtrl;
    HdrCtrl              hdrCtrl;
    /* Image Pipeline */
    EUnPack              unPack;
    EDpcmDecode          dpcmDecode;
    /* Input params */
    uint32_t             dbBufferEnable;   /* Doubel buffer enable for
                                               HDR only */
    uint32_t             imgWidth;         /* Sensor Image Wiidth */
    uint32_t             imgHeight;        /* Sensor Image Height */
    uint32_t             bpp;              /* Bits per pixel from sensor */
    uint32_t             longBPP;
    uint32_t             shortBPP;
    uint32_t             embeddedBefore;
    uint32_t             embeddedAfter;
    uint32_t             shortEmbeddedBefore;
    uint32_t             shortEmbeddedAfter;
    uint32_t             triggerCaptureEnable;
    uint32_t             multiModeHDR;
    ENumLanes            numLanes;         /* Num Data lanes */
} CamParams;

/* Global Variables */
/* Control Value */
static uint32_t gControlValue = 0UL;
static uint32_t gImageControlValue = 0UL;
/* Cam analogue */
static uint32_t gAnalogueValue = 0UL;
/* TG Ctrl */
static uint32_t gTgControlValue = 0UL;
/* Data lanes */
static uint32_t gDataLaneValue[4] = {0};
/* Main driver structure */
static CamParams gUnicamParams;
/* Need stride info for mode image */
/* Stride info */
uint32_t gUnicamStride = 0UL;

/* Extern function */
void DumpRegValue(uint32_t aOffset, uint32_t aDataValue);

/******************************************************************************
FUNCTION NAME: Unicam_Init

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam init configuration.

RETURNS: None

******************************************************************************/
void Unicam_Init(void)
{
    /* Step 1: Setup the PHY LDO in the power manager */
    /* Step 2: Setup System Clock */
    /* Mem Enable */

    gControlValue = (1 << CAM_CAMCTL_MEN_SHIFT);
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);
}

/******************************************************************************
FUNCTION NAME: Unicam_Pipeline_Cfg

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam pipeline configuration.

RETURNS: None
******************************************************************************/
static void Unicam_Pipeline_Cfg(void)
{
    uint32_t pipeLineValue = 0UL;

    pipeLineValue = (gUnicamParams.dpcmDecode << CAM_CAMIPIPE_DDM_SHIFT);
    pipeLineValue = (pipeLineValue |
            (gUnicamParams.unPack << CAM_CAMIPIPE_PUM_SHIFT));

    /* Reg: CAMIPIPE */
    DumpRegValue(CAM_CAMIPIPE_OFFSET, pipeLineValue);

}

/******************************************************************************
FUNCTION NAME: Unicam_Data_Cfg

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam data control.

RETURNS: None
******************************************************************************/
static void Unicam_Data_Cfg(void)
{
    uint32_t dcs = 0UL;
    uint32_t dcsSE = 0UL;

    dcs |= ((gUnicamParams.embeddedBefore << CAM_CAMDCS_EDL_SHIFT)
            & (CAM_CAMDCS_EDL_MASK));
    dcs |= ((gUnicamParams.embeddedAfter << CAM_CAMDCS_EDL_END_SHIFT) &
            (CAM_CAMDCS_EDL_END_MASK));

    dcsSE |= ((gUnicamParams.shortEmbeddedBefore << CAM_CAMIDATA_SE_EDL_SE_SHIFT)
            & (CAM_CAMIDATA_SE_EDL_SE_MASK));
    dcsSE |= ((gUnicamParams.shortEmbeddedAfter << CAM_CAMIDATA_SE_EDL_END_SE_SHIFT) &
            (CAM_CAMIDATA_SE_EDL_END_SE_MASK));

    /* Reg: CAMDCS */
    DumpRegValue(CAM_CAMDCS_OFFSET, dcs);

    /* Reg: CAMIDATA_SE */
    DumpRegValue(CAM_CAMIDATA_SE_OFFSET, dcsSE);
}

/******************************************************************************
FUNCTION NAME: Unicam_Framesize_Cfg

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam frame size configuration.

RETURNS: None

******************************************************************************/
/* To-do: Need to discuss */
static void Unicam_Framesize_Cfg(void)
{
    uint32_t frameSizeValue = 0UL;
    uint32_t hwSyncCTL = 0xc0000000UL;

    if (gUnicamParams.hdrCtrl.hdrEnable &&
            gUnicamParams.hdrCtrl.sonyHdr &&
            (gUnicamParams.bpp == 10 || gUnicamParams.bpp == 12)) {
        frameSizeValue = (((gUnicamParams.imgHeight / 2) +
                    (gUnicamParams.hdrCtrl.passObLines / 2))
                << CAM_CAMFRSZ_CTL_FRAME_HEIGHT_SHIFT);
    } else if (0UL != gUnicamParams.hdrCtrl.hdrEnable) {
        frameSizeValue = ((gUnicamParams.imgHeight)
                << CAM_CAMFRSZ_CTL_FRAME_HEIGHT_SHIFT);
    } else {
        frameSizeValue = ((gUnicamParams.imgHeight +
                    gUnicamParams.embeddedBefore +
                    gUnicamParams.embeddedAfter) << CAM_CAMFRSZ_CTL_FRAME_HEIGHT_SHIFT);
    }
    frameSizeValue = (frameSizeValue |
            ((gUnicamParams.imgWidth *
              gUnicamParams.bpp + 4) / 8) << CAM_CAMFRSZ_CTL_LINE_LENGTH_SHIFT);
    /* Reg: CAMFRSZ */
    DumpRegValue(CAM_CAMFRSZ_OFFSET, frameSizeValue);
    if (gUnicamParams.ctrl.inputMode == eTg) {
        hwSyncCTL |= ((gUnicamParams.imgWidth *
                       ((gUnicamParams.bpp + 7) & ~7) + 4) / 8)
                        << CAM_CAMISP_HWSYC_CTL_CTL_LINE_LENGTHMEM_SHIFT;
    } else {
        hwSyncCTL |= ((gUnicamParams.imgWidth *
                       gUnicamParams.bpp + 4) / 8)
                       << CAM_CAMISP_HWSYC_CTL_CTL_LINE_LENGTHMEM_SHIFT;
    }
    DumpRegValue(CAM_CAMISP_HWSYC_CTL_OFFSET, hwSyncCTL);
}

/* Unicam_BPP2ExpCode */
static uint32_t Unicam_BPP2ExpCode(uint32_t aBPP)
{
    uint32_t code = 0x0UL;

    switch (aBPP) {
        case 8UL:
            code = 0x3UL;
            break;
        case 10UL:
            code = 0x4UL;
            break;
        case 12UL:
            code = 0x5UL;
            break;
        case 14UL:
            code = 0x6UL;
            break;
        case 16UL:
            code = 0x7UL;
            break;
        default:
            break;
    }

    return code;
}

/******************************************************************************
FUNCTION NAME: Unicam_HDR_Cfg

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam HDR configuration.

RETURNS: None

******************************************************************************/
static void Unicam_HDR_Cfg(void)
{
    uint32_t hdrCtrl = 0x0UL;
    uint32_t exp1Ctl = 0x0UL;

    /* Check Multi-mode */
    if (0UL != gUnicamParams.multiModeHDR) {
        exp1Ctl |= ((Unicam_BPP2ExpCode(gUnicamParams.longBPP) <<
                     CAM_CAMEXP1CTL_PIXF_EXP0_SHIFT) &
                    CAM_CAMEXP1CTL_PIXF_EXP0_MASK);
        exp1Ctl |= ((Unicam_BPP2ExpCode(gUnicamParams.shortBPP) <<
                     CAM_CAMEXP1CTL_PIXF_EXP1_SHIFT) &
                     CAM_CAMEXP1CTL_PIXF_EXP1_MASK);
        hdrCtrl |= CAM_CAMHDRCTL_MULTI_RES_HDR_MASK;
    }

    /* check vcen */
    if (0UL != gUnicamParams.frontEnd.csiParams.csi2VcEn) {
        hdrCtrl |= ((gUnicamParams.frontEnd.csiParams.longExpVc <<
                    CAM_CAMHDRCTL_HDR_CSI2_LE_VC_SHIFT) &
                    CAM_CAMHDRCTL_HDR_CSI2_LE_VC_MASK);
        hdrCtrl |= ((gUnicamParams.frontEnd.csiParams.shortExpVc <<
                    CAM_CAMHDRCTL_HDR_CSI2_SE_VC_SHIFT) &
                    CAM_CAMHDRCTL_HDR_CSI2_SE_VC_MASK);
        hdrCtrl |= CAM_CAMHDRCTL_HDR_CSI2_VC_EN_MASK;
    }

    if (0UL != gUnicamParams.hdrCtrl.hdrEnable) {
        /* Enable HDR */
        hdrCtrl |= CAM_CAMHDRCTL_HDR_EN_MASK;
    }

    DumpRegValue(CAM_CAMEXP1CTL_OFFSET, exp1Ctl);
    DumpRegValue(CAM_CAMHDRCTL_OFFSET, hdrCtrl);
}

/******************************************************************************
FUNCTION NAME: Unicam_Priority_Ctrl

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam Prority configuration.

RETURNS: None

******************************************************************************/
static void Unicam_Priority_Ctrl(void)
{
    uint32_t priorityValue = 0UL;

    priorityValue  = (gUnicamParams.priorityCtrl.burstLen << CAM_CAMPRI_BL_SHIFT);
    priorityValue  = (priorityValue |
            (gUnicamParams.priorityCtrl.burstSpace << CAM_CAMPRI_BS_SHIFT));
    priorityValue  = (priorityValue |
            (gUnicamParams.priorityCtrl.panicPriority << CAM_CAMPRI_PP_SHIFT));
    priorityValue  = (priorityValue |
           (gUnicamParams.priorityCtrl.normalPriority << CAM_CAMPRI_NP_SHIFT));
    priorityValue  = (priorityValue |
            (gUnicamParams.priorityCtrl.panicThresh << CAM_CAMPRI_PT_SHIFT));
    priorityValue  = (priorityValue |
            (gUnicamParams.priorityCtrl.panicEnable << CAM_CAMPRI_PE_SHIFT));
    priorityValue = (priorityValue |
            (((gUnicamParams.ctrl.oeTimeout & 0xff00UL) >> 8UL) << CAM_CAMPRI_EXT_OET_MSB_SHIFT));

    /* Reg: CAMPRI */
    DumpRegValue(CAM_CAMPRI_OFFSET, priorityValue);
}

/******************************************************************************
FUNCTION NAME: Uincam_Setup_Digital_Phy

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam Csi digit phy configuration.

RETURNS: None

******************************************************************************/
static void Uincam_Setup_Digital_Phy(void)
{
    uint32_t dltValue = 0UL;
    uint16_t cltValue = 0U;
    uint8_t  automatic = 0U;
    uint32_t clockValue = 0UL;
    uint32_t temp = 0UL;

    /* Clock lane timing settings */
    temp = gUnicamParams.frontEnd.csiParams.clkTime.clkTermEn;
    cltValue = ((temp ? (temp-1) : 0)
            << CAM_CAMCLT_CLT1_SHIFT) & CAM_CAMCLT_CLT1_MASK;
    cltValue = (cltValue | ((gUnicamParams.frontEnd.csiParams.clkTime.clkSettle
                << CAM_CAMCLT_CLT2_SHIFT) & CAM_CAMCLT_CLT2_MASK));
    DumpRegValue(CAM_CAMCLT_OFFSET, cltValue);

    clockValue = (clockValue | (automatic << CAM_CAMCLK_CLTRE_SHIFT));/*0 = automatic*/
    DumpRegValue(CAM_CAMCLK_OFFSET, clockValue);

    /* Data lane timing settings */
    temp = gUnicamParams.frontEnd.csiParams.dataTime.tdTermEn;
    dltValue = ((temp ? (temp-1) : 0)
            << CAM_CAMDLT_DLT1_SHIFT) & CAM_CAMDLT_DLT1_MASK;
    dltValue = (dltValue |
            ((gUnicamParams.frontEnd.csiParams.dataTime.thsSettle
             << CAM_CAMDLT_DLT2_SHIFT) & CAM_CAMDLT_DLT2_MASK));
    temp = gUnicamParams.frontEnd.csiParams.dataTime.trxEnable;
    dltValue = (dltValue |
            (((temp > 1 ? (temp-2) : 0)
             << CAM_CAMDLT_DLT3_SHIFT) & CAM_CAMDLT_DLT3_MASK));
    DumpRegValue(CAM_CAMDLT_OFFSET, dltValue);

    /* Data lane configuration: DLTREN */
    gDataLaneValue[0] = (gDataLaneValue[0] |
            (gUnicamParams.frontEnd.csiParams.laneTermination
             << CAM_CAMDAT0_DLTREN_SHIFT));
    if (gUnicamParams.numLanes == eDualLane) {
        gDataLaneValue[1] = (gDataLaneValue[1] |
                (gUnicamParams.frontEnd.csiParams.laneTermination
                 << CAM_CAMDAT1_DLTREN_SHIFT));
    } else if (gUnicamParams.numLanes == eQuadLane) {
        gDataLaneValue[1] = (gDataLaneValue[1] |
                (gUnicamParams.frontEnd.csiParams.laneTermination
                 << CAM_CAMDAT1_DLTREN_SHIFT));
        gDataLaneValue[2] = (gDataLaneValue[2] |
                (gUnicamParams.frontEnd.csiParams.laneTermination
                 << CAM_CAMDAT2_DLTREN_SHIFT));
        gDataLaneValue[3] = (gDataLaneValue[3] |
                (gUnicamParams.frontEnd.csiParams.laneTermination
                 << CAM_CAMDAT3_DLTREN_SHIFT));
    }

    /* Data lane configuration: DLSMN */
    gDataLaneValue[0] = (gDataLaneValue[0] |
            (gUnicamParams.frontEnd.csiParams.laneSync
             << CAM_CAMDAT0_DLSMN_SHIFT));
    if (gUnicamParams.numLanes == eDualLane) {
        gDataLaneValue[1] = (gDataLaneValue[1] |
                (gUnicamParams.frontEnd.csiParams.laneSync
                 << CAM_CAMDAT1_DLSMN_SHIFT));
    } else if (gUnicamParams.numLanes == eQuadLane) {
        gDataLaneValue[1] = (gDataLaneValue[1] |
                (gUnicamParams.frontEnd.csiParams.laneSync
                 << CAM_CAMDAT1_DLSMN_SHIFT));
        gDataLaneValue[2] = (gDataLaneValue[2] |
                (gUnicamParams.frontEnd.csiParams.laneSync
                 << CAM_CAMDAT2_DLSMN_SHIFT));
        gDataLaneValue[3] = (gDataLaneValue[3] |
                (gUnicamParams.frontEnd.csiParams.laneSync
                 << CAM_CAMDAT3_DLSMN_SHIFT));
    }
    DumpRegValue(CAM_CAMDAT0_OFFSET, gDataLaneValue[0]);
    DumpRegValue(CAM_CAMDAT1_OFFSET, gDataLaneValue[1]);
    DumpRegValue(CAM_CAMDAT2_OFFSET, gDataLaneValue[2]);
    DumpRegValue(CAM_CAMDAT3_OFFSET, gDataLaneValue[3]);

    /* Set Packet Frame timeout */
    gControlValue = (gControlValue |
            (gUnicamParams.frontEnd.csiParams.packetFarmeTimeout
             << CAM_CAMCTL_PFT_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);
 }

/******************************************************************************
FUNCTION NAME: Unicam_Set_Image_id

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam Csi image id's configuration.

RETURNS: None

******************************************************************************/
static void Unicam_Set_Image_id(void)
{
    uint32_t idi0Value = 0UL;
    uint32_t idi1Value = 0UL;
    uint8_t  virtualChannel;
    if (!gUnicamParams.frontEnd.csiParams.csi2VcEn)
    {
        virtualChannel = 0;/* Need to select Virtual channel 0 */
    }

    if (gUnicamParams.hdrCtrl.hdrEnable &&
            gUnicamParams.frontEnd.csiParams.csi2VcEn) {
        idi0Value = ((gUnicamParams.frontEnd.csiParams.longExpVc << 6) |
                gUnicamParams.frontEnd.csiParams.longImageId);
        idi1Value = ((gUnicamParams.frontEnd.csiParams.shortExpVc << 6) |
                gUnicamParams.frontEnd.csiParams.shortImageId);
        DumpRegValue(CAM_CAMIDI0_OFFSET,
                ((idi1Value & 0xff) << CAM_CAMIDI0_IDI1_SHIFT) |
                ((idi0Value & 0xff) << CAM_CAMIDI0_IDI0_SHIFT));
    } else {
        DumpRegValue(CAM_CAMIDI0_OFFSET,
                ((virtualChannel << 6) | gUnicamParams.frontEnd.csiParams.longImageId));
    }
    /* TODO: Need to check with H/W team */
    /* FS/FE/LS/LE for HDR & non_HDR mode */
    /* As per the CSI2 std : 0x00->FS, 0x01->FE, 0x02->LS & 0x03->LE */
    DumpRegValue(CAM_CAMIDI1_OFFSET, 0x03020100);
    DumpRegValue(CAM_CAMIDI2_OFFSET, 0x43424140);
}

/******************************************************************************
FUNCTION NAME: Unicam_Csi_Line_Sync

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam Csi Line sync configuration.

RETURNS: None

******************************************************************************/
static void Unicam_Csi_Line_Sync(void)
{
    uint32_t lineControlValue = 0UL;

    lineControlValue = (gUnicamParams.frontEnd.csiParams.lineStart
            << CAM_CAMLINCTL_PASS_LS_SHIFT);
    lineControlValue = (lineControlValue |
            (gUnicamParams.frontEnd.csiParams.lineEnd
             << CAM_CAMLINCTL_PASS_LE_SHIFT));
    lineControlValue = (lineControlValue |
            ((!gUnicamParams.frontEnd.csiParams.lineEnd)
             << CAM_CAMLINCTL_CTL_GEN_LE_SYNC_SHIFT));
    DumpRegValue(CAM_CAMLINCTL_OFFSET, lineControlValue);
}

/******************************************************************************
FUNCTION NAME: Unicam_Tg_Cfg

ARGUMENTS: None

DESCRIPTION: This function dumps TG configuration.

RETURNS: None
******************************************************************************/
static void Unicam_Tg_Cfg(void)
{
    uint32_t  tgSizeVal = 0UL;
    uint32_t  blankVal = 0UL;
    uint32_t  vBlank = 0UL;
    uint32_t  hBlank = 0UL;
    uint32_t  maxMinVal = 0UL;
    uint32_t  strideLenVal = 0UL;


    /* Reg : CAM_VC5_TGCTL */
    gTgControlValue = (gUnicamParams.frontEnd.tgParams.ctrl.mode
            << CAM_CAM_VC5_TGCTL_MODE_SHIFT);
    if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eTc)
    {
        gTgControlValue = (gTgControlValue |
                (gUnicamParams.frontEnd.tgParams.ctrl.preBlankPeriod
                 << CAM_CAM_VC5_TGCTL_PRE_B_SHIFT));
    }
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.mode
             << CAM_CAM_VC5_TGCTL_MODE_SHIFT));
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.cfgByPass
             << CAM_CAM_VC5_TGCTL_CFG_BYPASS_SHIFT));
    if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eTc)
    {
        gTgControlValue = (gTgControlValue |
                ((16 - gUnicamParams.frontEnd.tgParams.ctrl.bpp)
                 << CAM_CAM_VC5_TGCTL_BPP_SHIFT));/* To-Do Need to check with BPP */
    } else if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eAxi) {
        gTgControlValue = (gTgControlValue |
                ((gUnicamParams.frontEnd.tgParams.ctrl.bpp - 1)
                 << CAM_CAM_VC5_TGCTL_BPP_SHIFT));/* To-Do Need to check with BPP */
    }

    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.chan
             << CAM_CAM_VC5_TGCTL_CHAN_SHIFT));
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.embeddedData
             << CAM_CAM_VC5_TGCTL_ED_SHIFT));
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.bayerOrder
             << CAM_CAM_VC5_TGCTL_BAYER_SHIFT));
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.distortEn
             << CAM_CAM_VC5_TGCTL_DIS_EN_SHIFT));
    gTgControlValue = (gTgControlValue |
            (gUnicamParams.frontEnd.tgParams.ctrl.translateEn
             << CAM_CAM_VC5_TGCTL_TR_EN_SHIFT));

    DumpRegValue(CAM_CAM_VC5_TGCTL_OFFSET, gTgControlValue);

    /* Reg: CAM_VC5_TGISZ */
    tgSizeVal = (gUnicamParams.frontEnd.tgParams.ctrl.height
            << CAM_CAM_VC5_TGISZ_TG_HEIGHT_SHIFT);
    tgSizeVal = (tgSizeVal |
            (gUnicamParams.frontEnd.tgParams.ctrl.width
             << CAM_CAM_VC5_TGISZ_TG_WIDTH_SHIFT));

    DumpRegValue(CAM_CAM_VC5_TGISZ_OFFSET, tgSizeVal);

    /* Reg : CAM_VC5_ACFG */
    DumpRegValue(CAM_CAM_VC5_ACFG_OFFSET,
            (gUnicamParams.frontEnd.tgParams.ctrl.cont
             << CAM_CAM_VC5_ACFG_CONT_SHIFT));

    /* Reg : CAM_VC5_TGBL */
    vBlank = (12 * (gUnicamParams.frontEnd.tgParams.clkFreq / 100)) / 1000;
    hBlank = (((gUnicamParams.frontEnd.tgParams.clkFreq /
                    gUnicamParams.frontEnd.tgParams.fps) - vBlank) /
                    gUnicamParams.frontEnd.tgParams.ctrl.height) -
                    gUnicamParams.frontEnd.tgParams.ctrl.width;

    blankVal = (vBlank << CAM_CAM_VC5_TGBL_VBLANK_SHIFT);
    blankVal |= (hBlank << CAM_CAM_VC5_TGBL_HBLANK_SHIFT);

    DumpRegValue(CAM_CAM_VC5_TGBL_OFFSET, blankVal);

    /* Reg: CAM_VC5_TGPS */
    if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eTc)
    {
        DumpRegValue(CAM_CAM_VC5_TGPS_OFFSET,
                (gUnicamParams.frontEnd.tgParams.blankPrescale
                 << CAM_CAM_VC5_TGPS_B_PRESCALE_SHIFT));
    }

    /* Reg: CAM_VC5_AMM */
    if (gUnicamParams.frontEnd.tgParams.max > gUnicamParams.frontEnd.tgParams.min)
    {
        maxMinVal = (gUnicamParams.frontEnd.tgParams.max
                << CAM_CAM_VC5_AMM_MAX_SHIFT);
        /* MIN should be set to 0x2 */
        maxMinVal = (maxMinVal |
                (gUnicamParams.frontEnd.tgParams.min
                 << CAM_CAM_VC5_AMM_MIN_SHIFT));
        DumpRegValue(CAM_CAM_VC5_AMM_OFFSET, maxMinVal);
    } else {
        printf("%s: Error : Max %d > Min %d \n", __func__,
                gUnicamParams.frontEnd.tgParams.max, gUnicamParams.frontEnd.tgParams.min);
    }

    if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eAxi)
    {
        strideLenVal = (gUnicamParams.frontEnd.tgParams.ctrl.length
                        << CAM_CAM_VC5_ASL_LENGTH_SHIFT);
        strideLenVal = (strideLenVal |
                (gUnicamParams.frontEnd.tgParams.ctrl.stride
                 << CAM_CAM_VC5_ASL_STRIDE_SHIFT));
        DumpRegValue(CAM_CAM_VC5_ASL_OFFSET, strideLenVal);
        DumpRegValue(CAM_CAM_VC5_ALC_OFFSET,
                (gUnicamParams.frontEnd.tgParams.ctrl.lc
                 << CAM_CAM_VC5_ALC_LC_SHIFT));
    }
}

/******************************************************************************
FUNCTION NAME: UnicamInitDriverStruct

ARGUMENTS: aSensorParam

DESCRIPTION: This function updates the driver structure from user input.

RETURNS:
BCM_ERR_OK
BCM_ERR_INVAL_PARAMS
******************************************************************************/
int32_t UnicamInitDriverStruct(CamSensorParams *aSensorParam)
{
    int32_t ret = BCM_ERR_OK;
    CamSensorParams *sensorParams = (CamSensorParams *)aSensorParam;
    gISPByteSwapEnable = 0UL;

    if (!sensorParams) {
        printf("%s: Invalid CamSensorParams ptr\n", __func__);
        ret =  BCM_ERR_INVAL_PARAMS;
        goto exit;
    }

    /* Default configuration */
    memset(&gUnicamParams, 0, sizeof(gUnicamParams));

    ret = UnicamGetFormatInfo(sensorParams->format,
                    &gUnicamParams.bpp,
                    NULL);
    if (BCM_ERR_OK != ret) {
        printf("%s: Invalid Input format(long) %d\n",
                                __func__, sensorParams->format);
        goto exit;
    }
    gUnicamParams.longBPP = gUnicamParams.bpp;

    /* For CPI */
    if (sensorParams->ifMode == eCpi)
    {
        gUnicamParams.frontEnd.cpiParams.reSync.dataShift =
            sensorParams->frontEnd.cpiParams.reSync.dataShift;
        gUnicamParams.frontEnd.cpiParams.reSync.dwid = gUnicamParams.bpp;
        gUnicamParams.frontEnd.cpiParams.reSync.dataReSync =
            sensorParams->frontEnd.cpiParams.reSync.dataReSync;
        gUnicamParams.frontEnd.cpiParams.reSync.hReSync =
            sensorParams->frontEnd.cpiParams.reSync.hReSync;
        gUnicamParams.frontEnd.cpiParams.reSync.vReSync =
            sensorParams->frontEnd.cpiParams.reSync.vReSync;
        gUnicamParams.frontEnd.cpiParams.reSync.hSyncActive =
            sensorParams->frontEnd.cpiParams.reSync.hSyncActive;
        gUnicamParams.frontEnd.cpiParams.reSync.vSyncActive =
            sensorParams->frontEnd.cpiParams.reSync.vSyncActive;
        gUnicamParams.frontEnd.cpiParams.framing.syncMode =
            sensorParams->frontEnd.cpiParams.framing.syncMode;
        gUnicamParams.frontEnd.cpiParams.framing.vSyncMode =
            sensorParams->frontEnd.cpiParams.framing.vSyncMode;
        gUnicamParams.frontEnd.cpiParams.framing.hSyncMode =
            sensorParams->frontEnd.cpiParams.framing.hSyncMode;
        gUnicamParams.frontEnd.cpiParams.framing.fieldMode =
            sensorParams->frontEnd.cpiParams.framing.fieldMode;
        gUnicamParams.frontEnd.cpiParams.framing.scMode =
            sensorParams->frontEnd.cpiParams.framing.scMode;
        if (gUnicamParams.frontEnd.cpiParams.framing.syncMode == eEmbeddedSync)
        {
            gUnicamParams.frontEnd.cpiParams.framing.enbSyncShift =
                sensorParams->frontEnd.cpiParams.framing.enbSyncShift;
            gUnicamParams.frontEnd.cpiParams.framing.shift =
                sensorParams->frontEnd.cpiParams.framing.shift;
            gUnicamParams.frontEnd.cpiParams.framing.vSyncBit =
                sensorParams->frontEnd.cpiParams.framing.vSyncBit;
            gUnicamParams.frontEnd.cpiParams.framing.hSyncBit =
                sensorParams->frontEnd.cpiParams.framing.hSyncBit;
            gUnicamParams.frontEnd.cpiParams.framing.fildBit =
                sensorParams->frontEnd.cpiParams.framing.fildBit;
        }
        gUnicamParams.frontEnd.cpiParams.windowing.vFront = 0;
        gUnicamParams.frontEnd.cpiParams.windowing.vActive = sensorParams->imgHeight;
        gUnicamParams.frontEnd.cpiParams.windowing.hFront = 0;
        gUnicamParams.frontEnd.cpiParams.windowing.hActive = sensorParams->imgWidth;
        gUnicamParams.frontEnd.cpiParams.windowing.windowingEnb = 1;
        gUnicamParams.frontEnd.cpiParams.windowing.fGate = 0;
        gUnicamParams.frontEnd.cpiParams.windowing.gStat = 0;
        gUnicamParams.frontEnd.cpiParams.code = eVideoData;
    }

    /* For CSI2 */
    if (sensorParams->ifMode == eCsi2)
    {
        /* Time nano sec */
        uint32_t lpClkTime = (NANO_SEC / sensorParams->lpClkFrequency);
        uint32_t hsClkTime = (NANO_SEC / sensorParams->hsClkFrequency);

        ret = UnicamGetFormatInfo(sensorParams->format,
                            NULL,
                            &gUnicamParams.frontEnd.csiParams.longImageId);
        if (BCM_ERR_OK != ret) {
            goto exit;
        }
        if (eRaw16 == sensorParams->format) {
            gISPByteSwapEnable = 1UL;
        }
        gUnicamParams.frontEnd.csiParams.clkTime.clkSettle =
            (sensorParams->frontEnd.csiParams.clkTime.clkSettle / lpClkTime);
        gUnicamParams.frontEnd.csiParams.clkTime.clkTermEn =
            (sensorParams->frontEnd.csiParams.clkTime.clkTermEn / lpClkTime);
        gUnicamParams.frontEnd.csiParams.dataTime.tdTermEn =
            (sensorParams->frontEnd.csiParams.dataTime.tdTermEn/ lpClkTime);
        gUnicamParams.frontEnd.csiParams.dataTime.thsSettle =
            (sensorParams->frontEnd.csiParams.dataTime.thsSettle / lpClkTime);
        gUnicamParams.frontEnd.csiParams.dataTime.trxEnable =
            (sensorParams->frontEnd.csiParams.dataTime.trxEnable / hsClkTime);
        gUnicamParams.frontEnd.csiParams.shortExpVc =
            sensorParams->frontEnd.csiParams.shortExpVc;
        gUnicamParams.frontEnd.csiParams.longExpVc =
            sensorParams->frontEnd.csiParams.longExpVc;
        gUnicamParams.frontEnd.csiParams.csi2VcEn =
            sensorParams->frontEnd.csiParams.csi2VcEn;
        gUnicamParams.frontEnd.csiParams.lineEnd =
            sensorParams->frontEnd.csiParams.lineEnd;
        gUnicamParams.frontEnd.csiParams.lineStart =
            sensorParams->frontEnd.csiParams.lineStart;
        gUnicamParams.frontEnd.csiParams.lineEndSync =
            sensorParams->frontEnd.csiParams.lineEndSync;
        gUnicamParams.frontEnd.csiParams.packetFarmeTimeout =
            sensorParams->frontEnd.csiParams.packetFarmeTimeout;
        gUnicamParams.numLanes = sensorParams->numLanes;
        gUnicamParams.frontEnd.csiParams.laneSync =
            sensorParams->frontEnd.csiParams.laneSync;
        gUnicamParams.frontEnd.csiParams.laneTermination =
            sensorParams->frontEnd.csiParams.laneTermination;
        if (0UL != sensorParams->isHDREnabled) {
            ret = UnicamGetFormatInfo(sensorParams->shortFormat,
                                &gUnicamParams.shortBPP,
                                &gUnicamParams.frontEnd.csiParams.shortImageId);
            if (BCM_ERR_OK != ret) {
                printf("%s: Invalid Input format(short) %d\n",
                                        __func__, sensorParams->shortFormat);
                goto exit;
            }
            gUnicamParams.frontEnd.csiParams.csi2VcEn = 1UL;
            if (gUnicamParams.bpp != gUnicamParams.shortBPP) {
                gUnicamParams.bpp = MAX(gUnicamParams.bpp, gUnicamParams.shortBPP);
                gUnicamParams.multiModeHDR = 1UL;
            }
        }
    }

    /* For TG */
    if (sensorParams->ifMode == eTg)
    {
        gUnicamParams.frontEnd.tgParams.ctrl.preBlankPeriod =
                sensorParams->frontEnd.tgParams.ctrl.preBlankPeriod;
        gUnicamParams.frontEnd.tgParams.ctrl.mode =
                sensorParams->frontEnd.tgParams.ctrl.mode;
        gUnicamParams.frontEnd.tgParams.ctrl.cfgByPass =
                sensorParams->frontEnd.tgParams.ctrl.cfgByPass;
        gUnicamParams.frontEnd.tgParams.ctrl.bpp =
                gUnicamParams.bpp;
        gUnicamParams.frontEnd.tgParams.ctrl.chan =
                sensorParams->frontEnd.tgParams.ctrl.chan;
        gUnicamParams.frontEnd.tgParams.ctrl.embeddedData =
                sensorParams->frontEnd.tgParams.ctrl.embeddedData;
        gUnicamParams.frontEnd.tgParams.ctrl.bayerOrder =
                sensorParams->frontEnd.tgParams.ctrl.bayerOrder;
        gUnicamParams.frontEnd.tgParams.ctrl.distortEn =
                sensorParams->frontEnd.tgParams.ctrl.distortEn;
        gUnicamParams.frontEnd.tgParams.ctrl.translateEn =
                sensorParams->frontEnd.tgParams.ctrl.translateEn;
        gUnicamParams.frontEnd.tgParams.ctrl.width =
                sensorParams->imgWidth;
        gUnicamParams.frontEnd.tgParams.ctrl.height =
                sensorParams->imgHeight;
        if (gUnicamParams.frontEnd.tgParams.ctrl.mode == eAxi) {
            gUnicamParams.frontEnd.tgParams.ctrl.cont =
                    sensorParams->frontEnd.tgParams.ctrl.cont;
            gUnicamParams.frontEnd.tgParams.ctrl.startAddr =
                    sensorParams->frontEnd.tgParams.ctrl.startAddr;
            gUnicamParams.frontEnd.tgParams.ctrl.endAddr =
                    sensorParams->frontEnd.tgParams.ctrl.endAddr;
            gUnicamParams.frontEnd.tgParams.ctrl.stride =
                    sensorParams->frontEnd.tgParams.ctrl.stride;
            gUnicamParams.frontEnd.tgParams.ctrl.length =
                    sensorParams->frontEnd.tgParams.ctrl.length;
            gUnicamParams.frontEnd.tgParams.ctrl.lc =
                    sensorParams->frontEnd.tgParams.ctrl.lc;
            gUnicamParams.frontEnd.tgParams.ctrl.axiTimer =
                    sensorParams->frontEnd.tgParams.ctrl.axiTimer;
        }
        gUnicamParams.frontEnd.tgParams.clkFreq =
                sensorParams->frontEnd.tgParams.clkFreq;
        gUnicamParams.frontEnd.tgParams.fps =
                sensorParams->fps;
        gUnicamParams.frontEnd.tgParams.vBlank =
                sensorParams->frontEnd.tgParams.vBlank;
        gUnicamParams.frontEnd.tgParams.hBlank =
                sensorParams->frontEnd.tgParams.hBlank;
        gUnicamParams.frontEnd.tgParams.blockYSize =
                sensorParams->frontEnd.tgParams.blockYSize;
        gUnicamParams.frontEnd.tgParams.blockXSize =
                sensorParams->frontEnd.tgParams.blockXSize;
        gUnicamParams.frontEnd.tgParams.blankPrescale =
                sensorParams->frontEnd.tgParams.blankPrescale;
        gUnicamParams.frontEnd.tgParams.max =
                sensorParams->frontEnd.tgParams.max;
        gUnicamParams.frontEnd.tgParams.min =
                sensorParams->frontEnd.tgParams.min;
    }

    gUnicamParams.ctrl.dataBufferOverrun = 1;  /* Wrap around */
    gUnicamParams.ctrl.imageBufferOverrun = 1; /* Wrp around */
    gUnicamParams.ctrl.supressLineStart = 0;
    gUnicamParams.ctrl.oeTimeout = 0xffUL;
    gUnicamParams.ctrl.inputMode = sensorParams->ifMode;
    gUnicamParams.priorityCtrl.panicEnable = 1;
    gUnicamParams.priorityCtrl.panicThresh = eOutputFifo90;
    gUnicamParams.priorityCtrl.normalPriority = 0x8;
    gUnicamParams.priorityCtrl.panicPriority = 0xe; /* 0xf */
    gUnicamParams.priorityCtrl.burstSpace = 0x0;
    gUnicamParams.priorityCtrl.burstLen = 0x0;
    gUnicamParams.hdrCtrl.hdrEnable = sensorParams->isHDREnabled;
    gUnicamParams.unPack = sensorParams->unPack;
    gUnicamParams.dpcmDecode = sensorParams->dpcmDecode;
    gUnicamParams.dbBufferEnable = 0;
    gUnicamParams.imgWidth = sensorParams->imgWidth;
    gUnicamParams.imgHeight = sensorParams->imgHeight;
    gUnicamParams.embeddedBefore = sensorParams->embeddedBefore;
    gUnicamParams.embeddedAfter = sensorParams->embeddedAfter;
    gUnicamParams.shortEmbeddedBefore = sensorParams->shortEmbeddedBefore;
    gUnicamParams.shortEmbeddedAfter = sensorParams->shortEmbeddedAfter;
    gUnicamParams.triggerCaptureEnable  = sensorParams->triggerCapture;

exit:
    return ret;
}

/******************************************************************************
FUNCTION NAME: UnicamSetConfiguration

ARGUMENTS: None

DESCRIPTION: This function dumps Unicam configuration.

RETURNS: None
******************************************************************************/
void UnicamSetConfiguration(void)
{

    /* CSI Configuration */
    if (gUnicamParams.ctrl.inputMode == eCsi2)
    {
        /* Configure Digital Phy */
        Uincam_Setup_Digital_Phy();

        /* Set virtualChannel Id & Image Id for CSI2 */
        Unicam_Set_Image_id();

        /* Line sync */
        Unicam_Csi_Line_Sync();
    }

    /* TG Configuration */
    if (gUnicamParams.ctrl.inputMode == eTg)
    {
        /* TG Configuration */
        Unicam_Tg_Cfg();
    }

    /* Configure mode */
    gControlValue = (gControlValue |
            (gUnicamParams.ctrl.inputMode << CAM_CAMCTL_CPM_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);

    /* Configure image pipeline */
    Unicam_Pipeline_Cfg();

    /* Configure data */
    Unicam_Data_Cfg();

    /* Configure frame size */
    Unicam_Framesize_Cfg();

    /* Configure HDR */
    Unicam_HDR_Cfg();

    /* Configure Stride */
    /* TODO:Support only TG Modifiy for other modes */
    if (gUnicamParams.ctrl.inputMode == eTg) {
        gUnicamStride = (((((((gUnicamParams.bpp)+ 7)&~7)/8) *
                        gUnicamParams.imgWidth) + 31) & ~31);
    } else {
        gUnicamStride = ((((gUnicamParams.bpp *
                            gUnicamParams.imgWidth) / 8) + 31) & ~31);
    }
    DumpRegValue(CAM_CAMIBLS_OFFSET, gUnicamStride);

    /* Output Engine Configuration */
    Unicam_Priority_Ctrl();

    gControlValue = (gControlValue |
            ((gUnicamParams.ctrl.oeTimeout & 0xffUL) << CAM_CAMCTL_OET_SHIFT));
    gControlValue = (gControlValue | (1 << CAM_CAMCTL_MEN_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);

    /* If trigger capture enabled */
    if (gUnicamParams.triggerCaptureEnable)
    {
        gImageControlValue = (gImageControlValue | (1 << CAM_CAMICTL_FCM_SHIFT));
        DumpRegValue(CAM_CAMICTL_OFFSET, gImageControlValue);
    }

    if (gUnicamParams.ctrl.inputMode == eTg)
    {
        /* Load the configuration */
        DumpRegValue(CAM_CAM_VC5_ATRIG_OFFSET, (0x1 << CAM_CAM_VC5_ATRIG_TRIG_SHIFT));
    }
}

/******************************************************************************
FUNCTION NAME: Unicam_start

ARGUMENTS: None

DESCRIPTION: This function do Unicam start.

RETURNS: None

******************************************************************************/
void Unicam_start(void)
{

    /* Output Engine in normal operation */
    gControlValue = (gControlValue | (0 << CAM_CAMCTL_SOE_SHIFT));

    /* Camera Peripheral Enable */
    gControlValue = (gControlValue | (1 << CAM_CAMCTL_CPE_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);
}

/******************************************************************************
FUNCTION NAME: Unicam_stop

ARGUMENTS: None

DESCRIPTION: This function do Unicam stop.

RETURNS: None
******************************************************************************/
void Unicam_stop(void)
{
    if (gUnicamParams.ctrl.inputMode == eCsi2 ||
            gUnicamParams.ctrl.inputMode == eHispi)
    {
        /* Disable data lanes, DDL=1 */
        gAnalogueValue = (gAnalogueValue | (1 << CAM_CAMANA_DDL_SHIFT));
        DumpRegValue(CAM_CAMANA_OFFSET, gAnalogueValue);
    }

    /* Abort AXI transfers, SOE=1 */
    gControlValue = (gControlValue | (1 << CAM_CAMCTL_SOE_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);

    /* To-Do: Need to check while( CAMSTA & 0x100 == 0 ); */
    /* Current transaction complete, OES=1 */
    /* Assert soft reset, CPR=1 */
    gControlValue = (gControlValue | (1 << CAM_CAMCTL_CPR_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);
    DumpRegValue(0xff, 100);

    /* Clear soft reset, CPR=0 */
    gControlValue = (gControlValue & ~(1 << CAM_CAMCTL_CPR_SHIFT));
    DumpRegValue(CAM_CAMCTL_OFFSET, gControlValue);

    /* Disable data lanes */
    if (gUnicamParams.ctrl.inputMode == eCsi2 ||
            gUnicamParams.ctrl.inputMode == eHispi)
    {
        DumpRegValue(CAM_CAMDAT0_OFFSET, 0x00000000);
        if (gUnicamParams.numLanes == eDualLane)
        {
            DumpRegValue(CAM_CAMDAT1_OFFSET, 0x00000000);
        } else if (gUnicamParams.numLanes == eQuadLane) {
            DumpRegValue(CAM_CAMDAT1_OFFSET, 0x00000000);
            DumpRegValue(CAM_CAMDAT2_OFFSET, 0x00000000);
            DumpRegValue(CAM_CAMDAT3_OFFSET, 0x00000000);
        }
    }

    if (gUnicamParams.ctrl.inputMode == eTg)
    {
        gTgControlValue = (gTgControlValue |
                (0x0 << CAM_CAM_VC5_TGCTL_START_SHIFT));
        DumpRegValue(CAM_CAM_VC5_TGCTL_OFFSET, gTgControlValue);
    }

    /* Disable clock lane */
    DumpRegValue(CAM_CAMCLKACTL_OFFSET, 0x00000000);

    /* Power down analogue */
    DumpRegValue(CAM_CAMANA_OFFSET, 0x00000000);
}

/******************************************************************************
FUNCTION NAME: Unicam_Deinit

ARGUMENTS: None

DESCRIPTION: This function do deinit.

RETURNS: None
******************************************************************************/
void Unicam_Deinit(void)
{
    /* Disable peripheral */
    DumpRegValue(CAM_CAMCTL_OFFSET, 0x00000000);
}
