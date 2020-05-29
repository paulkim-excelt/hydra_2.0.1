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
#ifndef ISP_FILTER_H
#define ISP_FILTER_H

/******************************************************************************
File Name:  isp_filter.h
Description: This file contains required reg list ranges.
 ******************************************************************************/
#include <stdint.h>
#include <isp_rdb.h>

typedef uint32_t ISP_BlockType;
#define ISP_BLOCK_TYPE_BAYER  (0x1UL)
#define ISP_BLOCK_TYPE_YCBCR  (0x2UL)
#define ISP_BLOCK_TYPE_COMMON (0x3UL)

/* Reg list Struct */
typedef struct {
    uint32_t startAddr;
    uint32_t endAddr;
    ISP_BlockType blockType;
    uint32_t mask;
} RegFilter;

/* ISP Reg Filter list */
static const RegFilter gIspRegFilter[47] = {
    {0x0030, 0x003c, ISP_BLOCK_TYPE_COMMON, 0UL},                 /* FR */
    {0x0050, 0x0050, ISP_BLOCK_TYPE_COMMON, 0UL},                 /* FR - MOSAIC */
    {0x0090, 0x00a8, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_WG_MASK},  /* WG */
    {0x00b0, 0x00cc, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_YG_MASK},  /* YG */
    {0x0100, 0x0100, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_II_MASK},  /* II - CTRL */
    {0x010c, 0x0110, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_II_MASK},  /* II */
    {0x0140, 0x015c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BL_MASK},  /* BL_LVL */
    {0x0160, 0x02ac, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BL_MASK},  /* BL */
    {0x02d0, 0x02ec, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_DPA_MASK}, /* DP */
    {0x0300, 0x031c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BL_MASK},  /* BL */
    {0x0340, 0x037c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BL_MASK},  /* BL */
    {0x03a0, 0x043c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_HDR_MASK}, /* HF */
    {0x04d0, 0x04d4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_LS_MASK},  /* LS - CTRL */
    {0x0500, 0x052c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_XC_MASK},  /* XC */
    {0x0550, 0x056c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_XC_MASK},  /* FG */
    {0x0580, 0x0580, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BD_MASK},  /* DN - CTRL */
    {0x0590, 0x05a4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BD_MASK},  /* DN - ABSC */
    {0x05b0, 0x05c4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BD_MASK},  /* DN - ORD */
    {0x05d0, 0x05f4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_BD_MASK},  /* DN - OTHERS */
    {0x06d0, 0x06d4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_DM_MASK},  /* DM */
    {0x0700, 0x0700, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_YI_MASK},  /* YI */
    {0x0710, 0x071c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_YI_MASK},  /* YI */
    {0x0750, 0x076c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_YC_MASK},  /* YC */
    {0x0800, 0x08c0, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_GM_MASK},  /* GM */
    {0x0970, 0x0988, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_FC_MASK},  /* FC */
    {0x09b4, 0x09b8, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_SH_MASK},  /* SH */
    {0x0b30, 0x0b40, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LR_MASK},  /* LR */
    {0x0b50, 0x0b6c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_CC_MASK},  /* CC */
    {0x0ba0, 0x0ca4, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_ST_MASK},  /* ST */
    {0x0cdc, 0x0d6c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_ST_MASK},  /* ST */
    {0x0d70, 0x0d78, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_HO_MASK},  /* HO */
    {0x0d88, 0x0d8c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_HO_MASK},  /* HO */
    {0x0dc0, 0x0dc8, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LO_MASK},  /* LO */
    {0x0dd8, 0x0ddc, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LO_MASK},  /* LO */
    {0x0df8, 0x0df8, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LO_MASK},  /* LO */
    {0x1000, 0x3200, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_LS_MASK},  /* LS */
    {0x3300, 0x347c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_GM_MASK},  /* GM */
    {0x3500, 0x353c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_CP_MASK},  /* TM */
    {0x3540, 0x3584, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_CP_MASK},  /* CP, TM/CP SHIFT */
    {0x35c0, 0x367c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_CP_MASK},  /* CP+TM */
    {0x3800, 0x394c, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_SH_MASK},  /* SH */
    {0x3a00, 0x3abc, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_GM_MASK},  /* GM */
    {0x3b00, 0x3c3c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_HDR_MASK}, /* BL1 */
    {0x3c80, 0x3dd8, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LTM_MASK}, /* LTM */
    {0x3e80, 0x3ffc, ISP_BLOCK_TYPE_YCBCR, ISP_FR_YCBCR_EN_LTM_MASK}, /* ISP-LTM */
    {0x4000, 0x407c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_HDR_MASK}, /* BL1 */
    {0x4c00, 0x4d0c, ISP_BLOCK_TYPE_BAYER, ISP_FR_BAYER_EN_CDN_MASK}  /* CDN */
};

#endif /* ISP_FILTER_H */
