/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
/**
 * @file bl_chip_misc.h
 *
 * @brief BCM8910x chip misc block
 *
 * This file defines BCM8910x chip misc block
 */

#ifndef BL_BCM8910X_CHIP_MISC_H
#define BL_BCM8910X_CHIP_MISC_H


typedef volatile struct {
    uint16_t    SPARE_REG_0;
    uint16_t    XTAL_CTRL_1;
    uint16_t    XTAL_CTRL_2;
    uint16_t    XTAL_CTRL_3;
    uint16_t    RCOSC_CFG;
    uint16_t    RCOSC_STAT;
    uint16_t    RSVD1[2];
    uint16_t    RCOSC_CTRL;
    uint16_t    PLLSYS_CTRL_1;
    uint16_t    PLLSYS_CTRL_2;
    uint16_t    PLLSYS_CTRL_3;
    uint16_t    PLLSYS_NDIV_FRAC;
    uint16_t    PLLSYS_NDIV;
    uint16_t    PLLSYS_CFG_1;
    uint16_t    PLLSYS_CFG_2;
    uint16_t    PLLSYS_CFG_3;
    uint16_t    PLLSYS_MDIV_1;
    uint16_t    PLLSYS_MDIV_2;
    uint16_t    PLLSYS_SSC_1;
    uint16_t    PLLSYS_SSC_2;
    uint16_t    PLLSYS_SSC_3;

    uint16_t    PLLSYS_STATUS;
#define PLLSYS_STATUS_LOCK_MASK             (0x0001U)
#define PLLSYS_STATUS_LOCK_SHIFT            (0U)
#define PLLSYS_STATUS_LOCK_LOST_MASK        (0x0002U)
#define PLLSYS_STATUS_LOCK_LOST_SHIFT       (1U)
    uint16_t    PLLDISP_CTRL_1;
    uint16_t    PLLDISP_CTRL_2;
    uint16_t    PLLDISP_CTRL_3;
    uint16_t    PLLDISP_NDIV_FRAC;
    uint16_t    PLLDISP_NDIV;
    uint16_t    PLLDISP_CFG_1;
    uint16_t    PLLDISP_CFG_2;
    uint16_t    PLLDISP_CFG_3;
    uint16_t    PLLDISP_SSC_1;
    uint16_t    PLLDISP_SSC_2;
    uint16_t    PLLDISP_SSC_3;

    uint16_t    PLLDISP_STATUS;
#define PLLDISP_STATUS_LOCK_MASK             (0x0001U)
#define PLLDISP_STATUS_LOCK_SHIFT            (0U)
#define PLLDISP_STATUS_LOCK_LOST_MASK        (0x0002U)
#define PLLDISP_STATUS_LOCK_LOST_SHIFT       (1U)

    uint16_t    PLLVID_CTRL_1;
    uint16_t    PLLVID_CTRL_2;
    uint16_t    PLLVID_CTRL_3;
    uint16_t    PLLVID_NDIV_FRAC;
    uint16_t    PLLVID_NDIV;
    uint16_t    PLLVID_CFG_1;
    uint16_t    PLLVID_CFG_2;
    uint16_t    PLLVID_CFG_3;
    uint16_t    PLLVID_SSC_1;
    uint16_t    PLLVID_SSC_2;
    uint16_t    PLLVID_SSC_3;

    uint16_t    PLLVID_STATUS;
#define PLLVID_STATUS_LOCK_MASK             (0x0001U)
#define PLLVID_STATUS_LOCK_SHIFT            (0U)
#define PLLVID_STATUS_LOCK_LOST_MASK        (0x0002U)
#define PLLVID_STATUS_LOCK_LOST_SHIFT       (1U)

    uint16_t    RSVD2[9];
    uint16_t    DDRLDO_CFG;
    uint16_t    DDRLDO_LDO_CNTL_MSB;
    uint16_t    DDRLDO_LDO_CNTL_LSB;
    uint16_t    PAD_CFG;
#define PAD_CFG_RGMII_MODEHV_MASK           (0x0001U)
    uint16_t    IMG_TEST;
    uint16_t    RSVD3[19];
    uint16_t    CHIP_TEST_MODE;
    uint16_t    CLK_PINSTRAP;
    uint16_t    DEBUG_PINSTRAP;
    uint16_t    CPU_TEST;
    uint16_t    INITVTOR_DBG_0;
    uint16_t    INITVTOR_DBG_1;
    uint16_t    VID_RESET_STAT;
    uint16_t    RSVD4[1];
    uint16_t    TEST_PIN_SEL_0;
    uint16_t    TEST_PIN_SEL_1;
    uint16_t    TEST_PIN_SEL_2;
    uint16_t    TPIN_REG_0;
    uint16_t    TPIN_REG_1;
    uint16_t    TPOUT_REG_0;
    uint16_t    TPOUT_REG_1;
    uint16_t    RSVD5[1];
    uint16_t    PLL_INTR;
    uint16_t    DDR_MISC;
    uint16_t    LED_CTRL;
    uint16_t    SPARE_REG_1;
    uint16_t    RSVD6[412];
} BL_CHIPMISC_RegsType;

#endif /* BL_BCM8910X_CHIP_MISC_H */
