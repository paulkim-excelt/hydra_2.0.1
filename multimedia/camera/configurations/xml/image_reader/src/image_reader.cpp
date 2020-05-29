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
#include <vector>
#include <utility>
#include <iomanip>
#include <fstream>
#include <iostream>

/* Hacks */
#define GNU
#define ISP_IN_IDX_LONG_EXP  (0UL)
#define ISP_IN_IDX_SHORT_EXP (1UL)
#define ISP_IN_IDX_MAX       (2UL)
#define _Static_assert  static_assert

#include "isp_image_v2.h"

///////////////////////////////////////////////////////////////////////////////
// Image parse and outputting routines
///////////////////////////////////////////////////////////////////////////////
static void ValidateMagic(const uint32_t sectionId, const uint32_t magic)
{
    if (sectionId != magic) {
        std::string str("Invalid magic: ");
        str += std::to_string(magic);
        str += " encountered. Expected to see magic: ";
        str += std::to_string(sectionId);
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseInputImageAndFrameConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *const blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& iifr0Cfg = *((const ISPIMGV2_IIFR_BlkCfg0Type*)(blockCfg));

        ValidateMagic(sectionId, iifr0Cfg.magic);

        std::cout << "Frame width: " << iifr0Cfg.width << std::endl;
        std::cout << "Frame height: " << iifr0Cfg.height << std::endl;
        std::cout << "Stride: " << iifr0Cfg.stride << std::endl;
        std::cout << "Format: " << (uint32_t)ISPIMGV2_IIFR_BLKCFG0_GET_FORMAT(&iifr0Cfg) << std::endl;
        std::cout << "Bayer order: " << (uint32_t)ISPIMGV2_IIFR_BLKCFG0_GET_BAYER_ORDER(&iifr0Cfg) << std::endl;
        std::cout << "Transposed: " << (uint32_t)ISPIMGV2_IIFR_BLKCFG0_GET_TRANSPOSED(&iifr0Cfg) << std::endl;
        std::cout << "DPCM mode: " << (uint32_t)ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_MODE(&iifr0Cfg) << std::endl;
        std::cout << "DPCM blocklen: " << (uint32_t)ISPIMGV2_IIFR_BLKCFG0_GET_DPCM_BLOCKLEN(&iifr0Cfg) << std::endl;
        std::cout << "bayer_en: 0x" << std::hex << iifr0Cfg.bayerEn << std::endl;
        std::cout << "ycbcr_en: 0x" << std::hex << iifr0Cfg.yCbCrEn << std::endl;
        std::cout << std::dec;
    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the image_input block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseBlackLevelConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& bl0Cfg = *((const ISPIMGV2_BL_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, bl0Cfg.magic);

        ISPIMGV2_BL_BlkCfg0PwlOffsetDescType pwlOffsetDesc[ISP_IN_IDX_MAX];
        const int32_t ret = ISPIMGV2_BL_BlkCfg0ComputePwlOffsets(&bl0Cfg, pwlOffsetDesc);

        if (BCM_ERR_OK == ret) {

            auto PwlPrintHelper = [&](const uint32_t offset, const uint32_t len, const char *suffix) {
                std::string abs("abs_");
                std::string ord("ord_");

                abs += suffix;
                ord += suffix;

                abs += ": ";
                ord += ": ";

                std::cout << abs;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << bl0Cfg.pwlElements[i].abs;
                }
                std::cout << std::endl;

                std::cout << ord;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << bl0Cfg.pwlElements[i].ord;
                }
                std::cout << std::endl;

                return;
            };

            std::cout << "Dark frame border left: "   << (uint32_t)bl0Cfg.dfBorderLeft << std::endl;
            std::cout << "Dark frame border right: "  << (uint32_t)bl0Cfg.dfBorderRight << std::endl;
            std::cout << "Dark frame border top: "    << (uint32_t)bl0Cfg.dfBorderTop << std::endl;
            std::cout << "Dark frame border bottom: " << (uint32_t)bl0Cfg.dfBorderBottom << std::endl;
            std::cout << "Dark frame multiplier: "    << (uint32_t)bl0Cfg.dfMult << std::endl;
            std::cout << "Threshold: "                << (uint32_t)bl0Cfg.threshold << std::endl;

            std::cout << "Long Exposure params: " << std::endl;
            std::cout << "Lvl_r : " << (uint32_t)bl0Cfg.channelConfig[0].lvlR << std::endl;
            std::cout << "Lvl_b : " << (uint32_t)bl0Cfg.channelConfig[0].lvlB << std::endl;
            std::cout << "Lvl_gr: " << (uint32_t)bl0Cfg.channelConfig[0].lvlGR << std::endl;
            std::cout << "Lvl_gb: " << (uint32_t)bl0Cfg.channelConfig[0].lvlGB << std::endl;
            PwlPrintHelper(pwlOffsetDesc[0].pwlOffsetR,  bl0Cfg.channelConfig[0].pwlLenR,  "R ");
            PwlPrintHelper(pwlOffsetDesc[0].pwlOffsetB,  bl0Cfg.channelConfig[0].pwlLenB,  "B ");
            PwlPrintHelper(pwlOffsetDesc[0].pwlOffsetGR, bl0Cfg.channelConfig[0].pwlLenGR, "GR");
            PwlPrintHelper(pwlOffsetDesc[0].pwlOffsetGB, bl0Cfg.channelConfig[0].pwlLenGB, "GB");

            std::cout << "Short Exposure params: " << std::endl;
            std::cout << "Lvl_r : " << (uint32_t)bl0Cfg.channelConfig[1].lvlR << std::endl;
            std::cout << "Lvl_b : " << (uint32_t)bl0Cfg.channelConfig[1].lvlB << std::endl;
            std::cout << "Lvl_gr: " << (uint32_t)bl0Cfg.channelConfig[1].lvlGR << std::endl;
            std::cout << "Lvl_gb: " << (uint32_t)bl0Cfg.channelConfig[1].lvlGB << std::endl;
            PwlPrintHelper(pwlOffsetDesc[1].pwlOffsetR,  bl0Cfg.channelConfig[1].pwlLenR, "R ");
            PwlPrintHelper(pwlOffsetDesc[1].pwlOffsetB,  bl0Cfg.channelConfig[1].pwlLenB, "B ");
            PwlPrintHelper(pwlOffsetDesc[1].pwlOffsetGR, bl0Cfg.channelConfig[1].pwlLenB, "GR");
            PwlPrintHelper(pwlOffsetDesc[1].pwlOffsetGR, bl0Cfg.channelConfig[1].pwlLenB, "GB");

        } else {
            std::string str("Invalid Black level config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the lens_shading block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseLensShadingConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        (void)blockCfg;
        std::cout << "Lens shading default configuration" << std::endl;
    } else if (ISPIMGV2_BLKCFG_ID_1 == blkCfgId) {
        const uint32_t NUM_TABLE_ELTS_PER_ROW = 16U;
        const auto& ls1Cfg = *((const ISPIMGV2_LS_BlkCfg1Type*)blockCfg);

        ValidateMagic(sectionId, ls1Cfg.magic);

        std::cout << "Pitch: " << (uint32_t)ls1Cfg.pitch << std::endl;
        std::cout << "VPitch: " << (uint32_t)ls1Cfg.vpitch << std::endl;
        std::cout << "Cell size pow x: " << (uint32_t)ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWX(&ls1Cfg) << std::endl;
        std::cout << "Cell size pow y: " << (uint32_t)ISPIMGV2_LS_BLKCFG1_GET_CELLSZ_POWY(&ls1Cfg) << std::endl;
        std::cout << "Table length: " << ls1Cfg.tableEltCount << std::endl;

        #define PRINT_LS_TABLE_COMP(compName)                                                                   \
            std::cout << "Table comp: " << #compName << std::endl;                                              \
            for (uint32_t i = 0U; i < ls1Cfg.tableEltCount; i += NUM_TABLE_ELTS_PER_ROW) {                      \
                const uint32_t numElts = ((ls1Cfg.tableEltCount - i) > NUM_TABLE_ELTS_PER_ROW) ?                \
                    NUM_TABLE_ELTS_PER_ROW : ((ls1Cfg.tableEltCount - i));                                      \
                for (uint32_t j = 0U; j < numElts; j++) {                                                       \
                    std::cout << std::right << std::setw(6) << ls1Cfg.lensShadingTable[i + j].compName << " ";  \
                }                                                                                               \
                std::cout << std::endl;                                                                         \
            }

        PRINT_LS_TABLE_COMP(cvR);
        PRINT_LS_TABLE_COMP(cvGR);
        PRINT_LS_TABLE_COMP(cvGB);
        PRINT_LS_TABLE_COMP(cvB);

        #undef PRINT_LS_TABLE_COMP

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the lens_shading block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseHDRFusionConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& hdr0Cfg = *((const ISPIMGV2_HDR_BlkCfg0Type*)blockCfg);
        const uint32_t hdrLen = ISPIMGV2_HDR_BLKCFG0_GET_PWL_LEN(&hdr0Cfg);

        ValidateMagic(sectionId, hdr0Cfg.magic);

        std::cout << "Swap Exp: " << (uint32_t)ISPIMGV2_HDR_BLKCFG0_GET_EXP_SWAP(&hdr0Cfg) << std::endl;
        std::cout << "Gain R: " << (uint32_t)hdr0Cfg.gainR << std::endl;
        std::cout << "Gain B: " << (uint32_t)hdr0Cfg.gainB << std::endl;
        std::cout << "Threshold Lo: " << (uint32_t)hdr0Cfg.expCombLoThres << std::endl;
        std::cout << "Threshold Hi: " << (uint32_t)hdr0Cfg.expCombHiThres << std::endl;
        std::cout << "Exp Ratio: " << (uint32_t)hdr0Cfg.expRatio << std::endl;
        std::cout << "WB Threshold: " << hdr0Cfg.wbThres << std::endl;
        std::cout << "Exp comb wt: " << ISPIMGV2_HDR_BLKCFG0_GET_EXP_COMB_WT(&hdr0Cfg) << std::endl;

        std::cout << "abs: ";
        for (uint32_t i = 0U; i < hdrLen; i++) {
            std::cout << std::right << std::setw(8) << ISPIMGV2_HDR_BLKCFG0_GET_PWL_ABSISSA_AT_IDX(&hdr0Cfg, i);
        }
        std::cout << std::endl;

        std::cout << "ord: ";
        for (uint32_t i = 0U; i < hdrLen; i++) {
            std::cout << std::right << std::setw(8) << ISPIMGV2_HDR_BLKCFG0_GET_PWL_ORDINATE_AT_IDX(&hdr0Cfg, i);
        }
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the hdr_fusion block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseBayerDenoiseConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& bdn0Cfg = *((const ISPIMGV2_BDN_BlkCfg0Type*)blockCfg);
        const uint32_t bdnLen = ISPIMGV2_BDN_BLKCFG0_GET_PWL_LEN(&bdn0Cfg);

        ValidateMagic(sectionId, bdn0Cfg.magic);

        std::cout << "GainR : " << (uint32_t)bdn0Cfg.gainR  << std::endl;
        std::cout << "GainB : " << (uint32_t)bdn0Cfg.gainB  << std::endl;
        std::cout << "GainGR: " << (uint32_t)bdn0Cfg.gainGR << std::endl;
        std::cout << "GainGB: " << (uint32_t)bdn0Cfg.gainGB << std::endl;

        std::cout << "StrengthR : " << (uint32_t)bdn0Cfg.strengthR << std::endl;
        std::cout << "StrengthB : " << (uint32_t)bdn0Cfg.strengthB << std::endl;
        std::cout << "StrengthGR: " << (uint32_t)bdn0Cfg.strengthGR << std::endl;
        std::cout << "StrengthGB: " << (uint32_t)bdn0Cfg.strengthGB << std::endl;

        std::cout << std::hex;
        std::cout << "Mask: 0x" << ISPIMGV2_BDN_BLKCFG0_GET_MASK(&bdn0Cfg) << std::endl;
        std::cout << std::dec;

        std::cout << "abs: ";
        for (uint32_t i = 0U; i < bdnLen; i++) {
            std::cout << std::right << std::setw(6) << bdn0Cfg.pwlElements[i].abs;
        }
        std::cout << std::endl;

        std::cout << "ord: ";
        for (uint32_t i = 0U; i < bdnLen; i++) {
            std::cout << std::right << std::setw(6) << bdn0Cfg.pwlElements[i].ord;
        }
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the bayer denoise block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseStatisticsConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& st0Cfg = *((const ISPIMGV2_Statistics_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, st0Cfg.magic);

        if (ISPIMGV2_STATISTICS_BLKCFG0_MAX_NUM_REGIONS >= st0Cfg.numGroup1And2Regions) {

            std::cout << "frame width  : " << (uint32_t)st0Cfg.frameWidth << std::endl;
            std::cout << "frame height : " << (uint32_t)st0Cfg.frameHeight << std::endl;

            std::cout << std::endl;
            std::cout << "hMask0    : " << (uint32_t)st0Cfg.hMask0 << std::endl;
            std::cout << "hMask1    : " << (uint32_t)st0Cfg.hMask1 << std::endl;
            std::cout << "Shift     : " << (uint32_t)st0Cfg.shift << std::endl;
            std::cout << "pix shift : " << (uint32_t)st0Cfg.pixelShift << std::endl;
            std::cout << "row factor: " << (uint32_t)st0Cfg.rowFactor << std::endl;
            std::cout << "columns   : " << (uint32_t)st0Cfg.columns << std::endl;

            std::cout << std::endl;
            std::cout << "Region config group 0" << std::endl;
            std::cout << "xoffset   : " << (uint32_t)st0Cfg.group0Regions.offsetX << std::endl;
            std::cout << "yoffset   : " << (uint32_t)st0Cfg.group0Regions.offsetY << std::endl;
            std::cout << "width     : " << (uint32_t)st0Cfg.group0Regions.width << std::endl;
            std::cout << "height    : " << (uint32_t)st0Cfg.group0Regions.height << std::endl;
            std::cout << "hNum      : " << (uint32_t)st0Cfg.group0Regions.hNum << std::endl;
            std::cout << "vNum      : " << (uint32_t)st0Cfg.group0Regions.vNum << std::endl;

            for (uint32_t i = 0UL; i < 3UL; i++) {
                std::cout << std::endl;
                std::cout << "Thresholds group " << i << std::endl;
                std::cout << "r_lo   : " << (uint32_t)st0Cfg.groupThresholds[i].rLo << std::endl;
                std::cout << "r_hi   : " << (uint32_t)st0Cfg.groupThresholds[i].rHi << std::endl;
                std::cout << "g_lo   : " << (uint32_t)st0Cfg.groupThresholds[i].gLo << std::endl;
                std::cout << "g_hi   : " << (uint32_t)st0Cfg.groupThresholds[i].gHi << std::endl;
                std::cout << "b_lo   : " << (uint32_t)st0Cfg.groupThresholds[i].bLo << std::endl;
                std::cout << "b_hi   : " << (uint32_t)st0Cfg.groupThresholds[i].bHi << std::endl;
                std::cout << "rg_lo  : " << (int32_t)st0Cfg.groupThresholds[i].rgLo << std::endl;
                std::cout << "rg_hi  : " << (int32_t)st0Cfg.groupThresholds[i].rgHi << std::endl;
                std::cout << "bg_lo  : " << (int32_t)st0Cfg.groupThresholds[i].bgLo << std::endl;
                std::cout << "bg_hi  : " << (int32_t)st0Cfg.groupThresholds[i].bgHi << std::endl;
            }

            for (uint32_t i = 0UL; i < st0Cfg.numGroup1And2Regions; i++) {
                std::cout << std::endl;
                if (0UL == i) {
                    std::cout << "Region config group 1 and 2" << std::endl;
                }
                std::cout << "region " << (uint32_t)i << std::endl;
                std::cout << "offset x: " << (uint32_t)st0Cfg.group1And2Regions[i].offsetX << std::endl;
                std::cout << "offset y: " << (uint32_t)st0Cfg.group1And2Regions[i].offsetY << std::endl;
                std::cout << "width   : " << (uint32_t)st0Cfg.group1And2Regions[i].width << std::endl;
                std::cout << "height  : " << (uint32_t)st0Cfg.group1And2Regions[i].height << std::endl;
            }

        } else {
            std::string str("Invalid Black level config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the statistics block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseWhiteBalanceAndGainConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& wbGain0Cfg = *((const ISPIMGV2_WBGain_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, wbGain0Cfg.magic);

        std::cout << "GainR: "     << (uint32_t)wbGain0Cfg.gainR << std::endl;
        std::cout << "GainB: "     << (uint32_t)wbGain0Cfg.gainB << std::endl;
        std::cout << "Gain All: "  << (uint32_t)wbGain0Cfg.gainAll << std::endl;
        std::cout << "OffsetR: "   << ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_R(&wbGain0Cfg) << std::endl;
        std::cout << "OffsetG: "   << ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_G(&wbGain0Cfg) << std::endl;
        std::cout << "OffsetB: "   << ISPIMGV2_WBGAIN_BLKCFG0_GET_OFFSET_B(&wbGain0Cfg) << std::endl;
        std::cout << "Threshold: " << (uint32_t)wbGain0Cfg.threshold << std::endl;


    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the white balance and gain block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseDefectivePixelConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& dpc0Cfg = *((const ISPIMGV2_DefectivePixel_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, dpc0Cfg.magic);

        std::cout << "ScaleLo:";
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleLo[0];
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleLo[1];
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleLo[2];
        std::cout << std::endl;

        std::cout << "ScaleHi:";
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleHi[0];
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleHi[1];
        std::cout << std::right << std::setw(3) << (uint32_t)dpc0Cfg.scaleHi[2];
        std::cout << std::endl;

        std::cout << "OffsetLo:";
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetLo[0];
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetLo[1];
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetLo[2];
        std::cout << std::endl;

        std::cout << "OffsetHi:";
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetHi[0];
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetHi[1];
        std::cout << std::right << std::setw(6) << (uint32_t)dpc0Cfg.offsetHi[2];
        std::cout << std::endl;

        std::cout << "Replace Idx Lo: " << (uint32_t)ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_LOW(&dpc0Cfg) << std::endl;
        std::cout << "Replace Idx Hi: " << (uint32_t)ISPIMGV2_DPC_BLKCFG0_GET_REPLACE_IDX_HIGH(&dpc0Cfg) << std::endl;

        std::cout << "Replace Scale Lo: " << (uint32_t)dpc0Cfg.replaceScaleLo << std::endl;
        std::cout << "Replace Scale Hi: " << (uint32_t)dpc0Cfg.replaceScaleHi << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the defective pixel block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseFringingConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& fr0Cfg = *((const ISPIMGV2_Fringing_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, fr0Cfg.magic);

        std::cout << "Pos R : " << (int32_t)fr0Cfg.rPos << std::endl;
        std::cout << "Neg R : " << (int32_t)fr0Cfg.rNeg << std::endl;
        std::cout << "Pos B : " << (int32_t)fr0Cfg.bPos << std::endl;
        std::cout << "Neg B : " << (int32_t)fr0Cfg.bNeg << std::endl;
        std::cout << "Pos GR: " << (int32_t)fr0Cfg.grPos << std::endl;
        std::cout << "Neg GR: " << (int32_t)fr0Cfg.grNeg << std::endl;
        std::cout << "Pos GB: " << (int32_t)fr0Cfg.gbPos << std::endl;
        std::cout << "Neg GB: " << (int32_t)fr0Cfg.gbNeg << std::endl;

        std::cout << "Pos Lim R : " << (uint32_t)fr0Cfg.rPosLim << std::endl;
        std::cout << "Neg Lim R : " << (uint32_t)fr0Cfg.rNegLim << std::endl;
        std::cout << "Pos Lim B : " << (uint32_t)fr0Cfg.bPosLim << std::endl;
        std::cout << "Neg Lim B : " << (uint32_t)fr0Cfg.bNegLim << std::endl;
        std::cout << "Pos Lim GR: " << (uint32_t)fr0Cfg.grPosLim << std::endl;
        std::cout << "Neg Lim GR: " << (uint32_t)fr0Cfg.grNegLim << std::endl;
        std::cout << "Pos Lim GB: " << (uint32_t)fr0Cfg.gbPosLim << std::endl;
        std::cout << "Neg Lim GB: " << (uint32_t)fr0Cfg.gbNegLim << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the fringing block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseCrosstalkConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& xc0Cfg = *((const ISPIMGV2_Crosstalk_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, xc0Cfg.magic);

        std::cout << "abs: ";
        for (uint32_t i = 0U; i < xc0Cfg.pwlLength; i++) {
            std::cout << std::right << std::setw(6) << xc0Cfg.pwlElements[i].abs;
        }
        std::cout << std::endl;

        std::cout << "ord: ";
        for (uint32_t i = 0U; i < xc0Cfg.pwlLength; i++) {
            std::cout << std::right << std::setw(6) << xc0Cfg.pwlElements[i].ord;
        }
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the fringing block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseColorDenoise(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& cdn0Cfg = *((const ISPIMGV2_ColorDenoise_BlkCfg0Type*)blockCfg);
        ISPIMGV2_CDN_BlkCfg0PwlOffsetAndLengthDescType pwlOffsetAndLenDesc{};

        ValidateMagic(sectionId, cdn0Cfg.magic);

        const int32_t ret = ISPIMGV2_CDN_BlkCfg0ComputePwlOffsetsAndLen(&cdn0Cfg, &pwlOffsetAndLenDesc);

        if (BCM_ERR_OK == ret) {

            auto PwlPrintHelper = [&](const uint32_t offset, const uint32_t len, const char *suffix) {
                std::string abs("abs_");
                std::string ord("ord_");

                abs += suffix;
                ord += suffix;

                abs += ": ";
                ord += ": ";

                std::cout << abs;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (uint32_t)cdn0Cfg.pwlElements[i].abs;
                }
                std::cout << std::endl;

                std::cout << ord;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (uint32_t)cdn0Cfg.pwlElements[i].ord;
                }
                std::cout << std::endl;

                return;
            };

            std::cout << "FIR Shift: " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_FIR_SHIFT(&cdn0Cfg) << std::endl;
            std::cout << "IIR attn lines: " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_LINES(&cdn0Cfg) << std::endl;
            std::cout << "IIR attn shift: " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_IIR_ATTN_SHIFT(&cdn0Cfg) << std::endl;
            std::cout << "CenterX: " << (uint32_t)cdn0Cfg.xCenter << std::endl;
            std::cout << "CenterY: " << (uint32_t)cdn0Cfg.yCenter << std::endl;
            std::cout << "Edge threshold: " << (uint32_t)cdn0Cfg.edgeThreshold << std::endl;
            std::cout << "Edge strength: " << (uint32_t)cdn0Cfg.edgeStrength << std::endl;
            std::cout << "Edge min: " << (uint32_t)cdn0Cfg.edgeMin << std::endl;
            std::cout << "Weight G : " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_G(&cdn0Cfg) << std::endl;
            std::cout << "Weight BG: " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_BG(&cdn0Cfg) << std::endl;
            std::cout << "Weight RG: " << (uint32_t)ISPIMGV2_CDN_BLKCFG0_GET_WEIGHT_RG(&cdn0Cfg) << std::endl;

            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetFirBg, pwlOffsetAndLenDesc.pwlLengthFirBg, "fir_bg");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetFirRg, pwlOffsetAndLenDesc.pwlLengthFirRg, "fir_rg");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetIirBG, pwlOffsetAndLenDesc.pwlLengthIirBG, "iir_bg");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetIirRg, pwlOffsetAndLenDesc.pwlLengthIirRg, "iir_rg");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetDist, pwlOffsetAndLenDesc.pwlLengthDist, "dist  ");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetG, pwlOffsetAndLenDesc.pwlLengthG, "g     ");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetSat, pwlOffsetAndLenDesc.pwlLengthSat, "sat   ");

        } else {
            std::string str("Invalid color denoise config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the color denoise block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseDemosaicConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& dm0Cfg = *((const ISPIMGV2_Demosaic_BlkCfg0Type*)blockCfg);

        const char* bayerTypeMap[] = {
            "TRUE",
            "H_COSITED",
            "V_COSITED",
            "HV_COSITED"
        };

        ValidateMagic(sectionId, dm0Cfg.magic);

        std::cout << "Bayer type: " << bayerTypeMap[ISPIMGV2_DM_BLKCFG0_GET_BAYER_TYPE(&dm0Cfg)] << std::endl;
        std::cout << "Threshold: " << (uint32_t)dm0Cfg.threshold << std::endl;
        std::cout << "HV_Bias: " << (uint32_t)ISPIMGV2_DM_BLKCFG0_GET_HV_BIAS(&dm0Cfg) << std::endl;
        std::cout << "G Offset shift: " << (uint32_t)ISPIMGV2_DM_BLKCFG0_GET_OFFSET_SHIFT_G(&dm0Cfg) << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the demosaic block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseSharpenConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {

        const auto& sh0Cfg = *((const ISPIMGV2_Sharpen_BlkCfg0Type*)blockCfg);
        ISPIMGV2_Sharpen_BlkCfg0PwlOffsetAndLengthDescType pwlOffsetAndLenDesc{};

        ValidateMagic(sectionId, sh0Cfg.magic);

        const int32_t ret = ISPIMGV2_Sharpen_BlkCfg0ComputePwlOffsetsAndLen(&sh0Cfg, &pwlOffsetAndLenDesc);

        if (BCM_ERR_OK == ret) {

            auto PwlStringConstructor = [](const char *suffix) {
                std::string abs("abs_");
                std::string ord("ord_");

                abs += suffix;
                ord += suffix;

                abs += ": ";
                ord += ": ";

                return std::make_pair(abs, ord);
            };

            auto PwlPrintHelperUnsigned = [&](const uint32_t offset, const uint32_t len, const char *suffix) {

                const auto strings = PwlStringConstructor(suffix);

                std::cout << strings.first;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (uint32_t)sh0Cfg.pwlElements[i].abs;
                }
                std::cout << std::endl;

                std::cout << strings.second;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (uint32_t)sh0Cfg.pwlElements[i].ord;
                }
                std::cout << std::endl;

                return;
            };

            auto PwlPrintHelperSigned = [&](const uint32_t offset, const uint32_t len, const char *suffix) {

                const auto strings = PwlStringConstructor(suffix);

                std::cout << strings.first;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (int32_t)((int16_t)sh0Cfg.pwlElements[i].abs);
                }
                std::cout << std::endl;

                std::cout << strings.second;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << (int32_t)((int16_t)sh0Cfg.pwlElements[i].ord);
                }
                std::cout << std::endl;

                return;
            };

            std::cout << "Avg shift G: " << (uint32_t)sh0Cfg.avgShiftG << std::endl;
            std::cout << "Isotropic Strength: " << (uint32_t)sh0Cfg.isotropicStrength << std::endl;
            std::cout << "X Center: " << (uint32_t)sh0Cfg.xCenter << std::endl;
            std::cout << "X Center: " << (uint32_t)sh0Cfg.yCenter << std::endl;

            PwlPrintHelperUnsigned(pwlOffsetAndLenDesc.pwlOffsetPosGain, pwlOffsetAndLenDesc.pwlLengthPosGain, "pos_gain ");
            PwlPrintHelperUnsigned(pwlOffsetAndLenDesc.pwlOffsetVGain, pwlOffsetAndLenDesc.pwlLengthVGain, "v_gain   ");
            PwlPrintHelperSigned(pwlOffsetAndLenDesc.pwlOffsetResp, pwlOffsetAndLenDesc.pwlLengthResp, "resp     ");
            PwlPrintHelperSigned(pwlOffsetAndLenDesc.pwlOffsetResp1, pwlOffsetAndLenDesc.pwlLengthResp1, "resp1    ");
            PwlPrintHelperUnsigned(pwlOffsetAndLenDesc.pwlOffsetRespGain, pwlOffsetAndLenDesc.pwlLengthRespGain, "resp_gain");

        } else {
            std::string str("Invalid sharpen config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the sharpen block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseYCConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& yc0Cfg = *((const ISPIMGV2_YCYG_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, yc0Cfg.magic);

        std::cout << "Matrix: ";
        for (uint32_t i = 0U; i < 9U; i++) {
            if ((0U == (i % 3U))) {
                std::cout << std::endl;
            }
            std::cout << std::right << std::setw(6) << (int32_t)yc0Cfg.matrix[i];
        }
        std::cout << std::endl;

        std::cout << "Offsets: " << std::endl;
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_0(&yc0Cfg);
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_1(&yc0Cfg);
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_2(&yc0Cfg);
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the YC block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseYGConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& yg0Cfg = *((const ISPIMGV2_YCYG_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, yg0Cfg.magic);

        std::cout << "Matrix: ";
        for (uint32_t i = 0U; i < 9U; i++) {
            if ((0U == (i % 3U))) {
                std::cout << std::endl;
            }
            std::cout << std::right << std::setw(6) << (int32_t)yg0Cfg.matrix[i];
        }
        std::cout << std::endl;

        std::cout << "Offsets: " << std::endl;
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_0(&yg0Cfg);
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_1(&yg0Cfg);
        std::cout << std::right << std::setw(6) << ISPIMGV2_YCYG_BLKCFG0_GET_OFFSET_2(&yg0Cfg);
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the YG block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseGMConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        ISPIMGV2_Gamma_BlkCfg0PwlOffsetAndLengthDescType pwlOffsetAndLenDesc{};
        const auto& gm0Cfg = *((const ISPIMGV2_Gamma_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, gm0Cfg.magic);

        const auto ret  = ISPIMGV2_Gamma_BlkCfg0ComputePwlOffsetsAndLen(&gm0Cfg, &pwlOffsetAndLenDesc);

        if (BCM_ERR_OK == ret) {

            auto PwlPrintHelper = [&](const uint32_t offset, const uint32_t len, const char *suffix) {
                std::string abs("abs_");
                std::string ord("ord_");

                abs += suffix;
                ord += suffix;

                abs += ": ";
                ord += ": ";

                std::cout << abs;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << gm0Cfg.pwlElements[i].abs;
                }
                std::cout << std::endl;

                std::cout << ord;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << gm0Cfg.pwlElements[i].ord;
                }
                std::cout << std::endl;

                return;
            };

            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetR, pwlOffsetAndLenDesc.pwlLengthR, "r");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetG, pwlOffsetAndLenDesc.pwlLengthG, "g");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetB, pwlOffsetAndLenDesc.pwlLengthB, "b");

        } else {
            std::string str("Invalid gamma config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the gamma block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseFalseColorConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& fc0Cfg = *((const ISPIMGV2_FC_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, fc0Cfg.magic);

        std::cout << "Strength1    : " << (uint32_t)fc0Cfg.strength1 << std::endl;
        std::cout << "Strength2    : " << (uint32_t)fc0Cfg.strength2 << std::endl;
        std::cout << "FilterMixWt  : " << (uint32_t)fc0Cfg.filterMixWt << std::endl;
        std::cout << "FilterDesatWt: " << (uint32_t)fc0Cfg.filterDesatWt << std::endl;
        std::cout << "FilterMixLo  : " << (uint32_t)fc0Cfg.filterMixLo << std::endl;
        std::cout << "FilterMixHi  : " << (uint32_t)fc0Cfg.filterMixHi << std::endl;
        std::cout << "FilterDesatLo: " << (uint32_t)fc0Cfg.filterDesatLo << std::endl;
        std::cout << "FilterDesatHi: " << (uint32_t)fc0Cfg.filterDesatHi << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the False color block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseLTMConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        ISPIMGV2_LTM_BlkCfg0PwlOffsetAndLengthDescType pwlOffsetAndLenDesc{};
        const auto& ltm0Cfg = *((const ISPIMGV2_LTM_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, ltm0Cfg.magic);

        const auto ret  = ISPIMGV2_LTM_BlkCfg0ComputePwlOffsetsAndLen(&ltm0Cfg, &pwlOffsetAndLenDesc);

        if (BCM_ERR_OK == ret) {

            auto PwlPrintHelper = [&](const uint32_t offset, const uint32_t len, const char *suffix) {
                std::string abs("abs_");
                std::string ord("ord_");

                abs += suffix;
                ord += suffix;

                abs += ": ";
                ord += ": ";

                std::cout << abs;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << ltm0Cfg.pwlElements[i].abs;
                }
                std::cout << std::endl;

                std::cout << ord;
                for (uint32_t i  = offset; i < offset + len; i++) {
                    std::cout << std::right << std::setw(6) << ltm0Cfg.pwlElements[i].ord;
                }
                std::cout << std::endl;

                return;
            };

            std::cout << "gf_en  : " << (uint32_t)pwlOffsetAndLenDesc.gfEn << std::endl;
            std::cout << "KHp    : " << (uint32_t)ltm0Cfg.khp << std::endl;
            std::cout << "KColor : " << (uint32_t)ltm0Cfg.kcolor << std::endl;
            std::cout << "Epsilon: " << (uint32_t)ltm0Cfg.epsilonVal << std::endl;
            std::cout << "DivSat : " << (uint32_t)ltm0Cfg.divSat << std::endl;

            std::cout << "Gaussian:" << std::endl;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    int offset = 9*i + j;
                    std::cout << std::setfill(' ') << std::setw(8) << ltm0Cfg.gaussianKernel[offset] << " ";
                }
                std::cout << std::endl;
            }

            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetBgm, pwlOffsetAndLenDesc.pwlLengthBgm, "b");
            PwlPrintHelper(pwlOffsetAndLenDesc.pwlOffsetDgm, pwlOffsetAndLenDesc.pwlLengthDgm, "d");

        } else {
            std::string str("Invalid LTM config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the LTM block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseTMConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& tm0Cfg = *((const ISPIMGV2_TM_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, tm0Cfg.magic);

        if (32U >= tm0Cfg.pwlLength) {

            std::cout << "abs_y:";
            for (uint32_t i = 0UL; i < tm0Cfg.pwlLength; i++) {
                std::cout << std::setfill(' ') << std::setw(6) << (uint32_t)tm0Cfg.pwlElements[i].abs;

            }
            std::cout << std::endl;

            std::cout << "ord_y:";
            for (uint32_t i = 0UL; i < tm0Cfg.pwlLength; i++) {
                std::cout << std::setfill(' ') << std::setw(6) << (uint32_t)tm0Cfg.pwlElements[i].ord;
            }
            std::cout << std::endl;

        } else {
            std::string str("Invalid TM config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the TM block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseCpConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        ISPIMGV2_CP_BlkCfg0PwlOffsetAndLengthDescType pwlOffsetAndLenDesc{};
        const auto& cp0Cfg = *((const ISPIMGV2_CP_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, cp0Cfg.magic);

        const auto ret = ISPIMGV2_CP_BlkCfg0ComputePwlOffsetsAndLen(&cp0Cfg, &pwlOffsetAndLenDesc);

        if (BCM_ERR_OK == ret) {

            uint32_t offset;

            offset = pwlOffsetAndLenDesc.pwlOffsetU;

            std::cout << "abs_u:";
            for (uint32_t i = 0UL; i < pwlOffsetAndLenDesc.pwlLengthU; i++) {
                std::cout << std::setfill(' ') << std::setw(7) << (int32_t)cp0Cfg.pwlElements[i + offset].abs;

            }
            std::cout << std::endl;

            std::cout << "ord_u:";
            for (uint32_t i = 0UL; i < pwlOffsetAndLenDesc.pwlLengthU; i++) {
                std::cout << std::setfill(' ') << std::setw(7) << (uint32_t)cp0Cfg.pwlElements[i + offset].ord;
            }
            std::cout << std::endl;

            offset = pwlOffsetAndLenDesc.pwlOffsetV;

            std::cout << "abs_v:";
            for (uint32_t i = 0UL; i < pwlOffsetAndLenDesc.pwlLengthV; i++) {
                std::cout << std::setfill(' ') << std::setw(7) << (int32_t)cp0Cfg.pwlElements[i + offset].abs;

            }
            std::cout << std::endl;

            std::cout << "ord_v:";
            for (uint32_t i = 0UL; i < pwlOffsetAndLenDesc.pwlLengthV; i++) {
                std::cout << std::setfill(' ') << std::setw(7) << (uint32_t)cp0Cfg.pwlElements[i + offset].ord;
            }
            std::cout << std::endl;

        } else {
            std::string str("Invalid CP config with cfg id: ");
            str += std::to_string(blkCfgId);
            str += "!";
            throw std::invalid_argument(str);
        }

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the CP block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseColorConvertConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& cc0Cfg = *((const ISPIMGV2_CC_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, cc0Cfg.magic);

        std::cout << "Matrix: ";
        for (uint32_t i = 0U; i < 9U; i++) {
            if ((0U == (i % 3U))) {
                std::cout << std::endl;
            }
            std::cout << std::right << std::setw(6) << (int32_t)cc0Cfg.matrix[i];
        }
        std::cout << std::endl;

        std::cout << "Offsets: " << std::endl;
        std::cout << std::right << std::setw(8) << cc0Cfg.offsets[0];
        std::cout << std::right << std::setw(8) << cc0Cfg.offsets[1];
        std::cout << std::right << std::setw(8) << cc0Cfg.offsets[2];
        std::cout << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the color convert block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseLrResizeConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& lr0Cfg = *((const ISPIMGV2_LR_BlkCfg0Type*)blockCfg);

        ValidateMagic(sectionId, lr0Cfg.magic);

        std::cout << "Frame width   : " << (uint32_t)lr0Cfg.frameWidth << std::endl;
        std::cout << "Frame height  : " << (uint32_t)lr0Cfg.frameHeight << std::endl;
        std::cout << "Output width  : " << lr0Cfg.outputWidth << std::endl;
        std::cout << "Output height : " << lr0Cfg.outputHeight << std::endl;
        std::cout << "Shift         : " << (uint32_t)ISPIMGV2_LR_BLKCFG0_GET_SHIFT(&lr0Cfg) << std::endl;
        std::cout << "RB Swap       : " << (uint32_t)ISPIMGV2_LR_BLKCFG0_GET_RB_SWAP(&lr0Cfg) << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the lr resize block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseOutputConfig(
    const uint32_t sectionId,
    const uint32_t blkCfgId,
    const void *blockCfg)
{
    if (ISPIMGV2_BLKCFG_ID_0 == blkCfgId) {
        const auto& out0Cfg = *((const ISPIMGV2_Output_BlkCfg0Type*)(blockCfg));

        ValidateMagic(sectionId, out0Cfg.magic);

        const char *dataFormatMap[] = {"INTERLEAVED", "565", "RGBA32_0", "RGBA32_255", "444",
            "422", "420", "INVAL", "YUYV", "YVYU", "UYVY", "VYUY"};

        const uint32_t dataFormat = ISPIMGV2_OUTPUT_BLKCFG0_GET_DATA_FORMAT(&out0Cfg);

        std::cout << "V flip       : " << (uint32_t)ISPIMGV2_OUTPUT_BLKCFG0_GET_VFLIP(&out0Cfg) << std::endl;
        std::cout << "Big endian   : " << (uint32_t)ISPIMGV2_OUTPUT_BLKCFG0_GET_BIG_ENDIAN(&out0Cfg) << std::endl;
        std::cout << "Output width : " << (uint32_t)ISPIMGV2_OUTPUT_BLKCFG0_GET_OUTPUT_WIDTH(&out0Cfg) << std::endl;
        std::cout << "Sat depth    : " << (uint32_t)ISPIMGV2_OUTPUT_BLKCFG0_GET_SAT_DEPTH(&out0Cfg) << std::endl;
        std::cout << "Shift        : " << (uint32_t)ISPIMGV2_OUTPUT_BLKCFG0_GET_SHIFT(&out0Cfg) << std::endl;
        std::cout << "Data format  : " << dataFormatMap[dataFormat] << std::endl;
        std::cout << "Stride1      : " << out0Cfg.stride1 << std::endl;
        std::cout << "Stride2      : " << out0Cfg.stride2 << std::endl;

    } else {
        std::string str("Block cfg id: ");
        str += std::to_string(blkCfgId);
        str += " is invalid with the output block!";
        throw std::invalid_argument(str);
    }

    return;
}

static void ParseIspImage(const uint8_t *const ispImgBase)
{
    // Helpers
    auto GetPassName = [](const uint8_t passId) {
        if (1U == passId) {
            return "HV";
        } else if (2U == passId) {
            return "CV";
        } else if (3U == passId) {
            return "Common";
        } else {
            std::string str("Invalid pass ID: ");
            str += std::to_string(passId);
            str += " encountered!";
            throw std::invalid_argument(str);
        }
    };

    // Look for the file header
    const auto ispImg = (const ISPIMGV2_ConfigImageType*)(ispImgBase);
    if ((ISYS_CONFIG_ISP != ispImg->imgHdr.cfgType) ||
        (ISPIMGV2_MAGIC_V2 != ispImg->imgHdr.cfgMagic)) {
        throw std::invalid_argument("Invalid ISP image!");
    }

    // Next up, loop over each pass
    const auto& passOffsetTable = ispImg->passOffsetTable;
    const uint32_t numPasses = passOffsetTable.numPasses;
    std::cout << "Num ISP Passes: " << numPasses << std::endl;

    for (uint32_t i = 0; i < numPasses; i++) {

        const auto& passTableElt = passOffsetTable.passes[i];
        const auto& sectionTable = *(const ISPIMGV2_SectionOffsetTableType*)
            (ispImgBase + passTableElt.passSectionTableOffset);

        std::cout << "********************************************************" << std::endl;

        std::cout <<
            "Pass Idx: "      << i <<
            "; Pass type: "   << GetPassName(passOffsetTable.passes[i].passId) <<
            "; NumSections: " << sectionTable.numSections <<
            std::endl;

        // Loop over each section
        for (uint32_t j = 0; j < sectionTable.numSections; j++) {
            const char *blockName;
            void (*parseBlockProc)(const uint32_t sectionId, uint32_t blkCfgId, const void *blockCfg);
            const auto &sectionTableElt = sectionTable.sections[j];
            const uint32_t blockID = ISPIMGV2_GET_BLOCK_ID(sectionTableElt.sectionId);
            const uint32_t blkCfgId = ISPIMGV2_GET_BLKCFG_ID(sectionTableElt.sectionId);
            const void *const sectionPtr = (void*)(ispImgBase + sectionTableElt.sectionOffset);

            if (ISPIMGV2_BLOCK_ID_IIFR == blockID) {
                blockName = "Image input";
                parseBlockProc = ParseInputImageAndFrameConfig;
            } else if (ISPIMGV2_BLOCK_ID_BL == blockID) {
                blockName = "Black level";
                parseBlockProc = ParseBlackLevelConfig;
            } else if (ISPIMGV2_BLOCK_ID_LS == blockID) {
                blockName = "Lens shading";
                parseBlockProc = ParseLensShadingConfig;
            } else if (ISPIMGV2_BLOCK_ID_HDR == blockID) {
                blockName = "HDR Fusion";
                parseBlockProc = ParseHDRFusionConfig;
            } else if (ISPIMGV2_BLOCK_ID_BDN == blockID) {
                blockName = "Bayer denoise";
                parseBlockProc = ParseBayerDenoiseConfig;
            } else if (ISPIMGV2_BLOCK_ID_ST == blockID) {
                blockName = "Statistics";
                parseBlockProc = ParseStatisticsConfig;
            } else if (ISPIMGV2_BLOCK_ID_WBG == blockID) {
                blockName = "White balance and gain";
                parseBlockProc = ParseWhiteBalanceAndGainConfig;
            } else if (ISPIMGV2_BLOCK_ID_DPC == blockID) {
                blockName = "Defective pixel";
                parseBlockProc = ParseDefectivePixelConfig;
            } else if (ISPIMGV2_BLOCK_ID_FG == blockID) {
                blockName = "Fringing";
                parseBlockProc = ParseFringingConfig;
            } else if (ISPIMGV2_BLOCK_ID_XC == blockID) {
                blockName = "Crosstalk";
                parseBlockProc = ParseCrosstalkConfig;
            } else if (ISPIMGV2_BLOCK_ID_CDN == blockID) {
                blockName = "Color denoise";
                parseBlockProc = ParseColorDenoise;
            } else if (ISPIMGV2_BLOCK_ID_DM == blockID) {
                blockName = "Demosaic";
                parseBlockProc = ParseDemosaicConfig;
            } else if (ISPIMGV2_BLOCK_ID_SH == blockID) {
                blockName = "Sharpen";
                parseBlockProc = ParseSharpenConfig;
            } else if (ISPIMGV2_BLOCK_ID_YC == blockID) {
                blockName = "Pre gamma color conversion";
                parseBlockProc = ParseYCConfig;
            } else if (ISPIMGV2_BLOCK_ID_GM == blockID) {
                blockName = "Gamma";
                parseBlockProc = ParseGMConfig;
            } else if (ISPIMGV2_BLOCK_ID_YG == blockID) {
                blockName = "Post gamma color conversion";
                parseBlockProc = ParseYGConfig;
            } else if (ISPIMGV2_BLOCK_ID_FC == blockID) {
                blockName = "False color supression";
                parseBlockProc = ParseFalseColorConfig;
            } else if (ISPIMGV2_BLOCK_ID_LTM == blockID) {
                blockName = "Local tone mapping";
                parseBlockProc = ParseLTMConfig;
            } else if (ISPIMGV2_BLOCK_ID_TM == blockID) {
                blockName = "Tone mapping";
                parseBlockProc = ParseTMConfig;
            } else if (ISPIMGV2_BLOCK_ID_CP == blockID) {
                blockName = "Chrominance stretch";
                parseBlockProc = ParseCpConfig;
            } else if (ISPIMGV2_BLOCK_ID_CC == blockID) {
                blockName = "Color conversion";
                parseBlockProc = ParseColorConvertConfig;
            } else if (ISPIMGV2_BLOCK_ID_LR == blockID) {
                blockName = "Low resolution resize";
                parseBlockProc = ParseLrResizeConfig;
            } else if (ISPIMGV2_BLOCK_ID_HO == blockID) {
                blockName = "Hires output";
                parseBlockProc = ParseOutputConfig;
            } else if (ISPIMGV2_BLOCK_ID_LO == blockID) {
                blockName = "Lores output";
                parseBlockProc = ParseOutputConfig;
            } else {
                std::string str("Invalid Block ID: ");
                str += std::to_string(blockID);
                str += " encountered!";
                throw std::invalid_argument(str);
            }

            std::cout << "******************************" << std::endl;

            std::cout <<
                "Block Idx: "       << j            <<
                "; Block Name: "    << blockName    <<
                "; Block Cfg ID: "  << blkCfgId     <<
                std::endl;

            parseBlockProc(sectionTableElt.sectionId, blkCfgId, sectionPtr);

        }

        std::cout << "********************************************************" << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////
// File read helper
///////////////////////////////////////////////////////////////////////////////
static std::vector<uint8_t> OpenIspImage(const char *const fileName)
{
    std::vector<uint8_t> ret;

    try {
        std::ifstream imgStream(fileName, std::ios::in | std::ios::binary | std::ios::ate);
        const auto size = imgStream.tellg();
        ret.reserve(size);

        imgStream.seekg(0);
        imgStream.read((char*)ret.data(), size);
    } catch (const std::exception& e) {
        std::string str("Failed to read ISP image file: ");
        str += fileName;
        throw std::invalid_argument(str);
    }

    return ret;
}

///////////////////////////////////////////////////////////////////////////////
// Main function and helpers
///////////////////////////////////////////////////////////////////////////////
static void PrintHelp(const char *binName)
{
    std::cout << "Usage: " << binName << " " << "[PATH_TO_ISP_IMAGE]" << std::endl;
    return;
}

int main(int argc, char *argv[])
{
    int ret;

    if (argc != 2) {
        PrintHelp(argv[0]);
        ret = -1;
    } else {
        const auto ispImgBin = OpenIspImage(argv[1]);
        ParseIspImage(ispImgBin.data());
        ret = 0;
    }

    return ret;
}

