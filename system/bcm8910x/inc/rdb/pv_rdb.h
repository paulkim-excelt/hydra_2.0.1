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
/**
    @file pv_rdb.h
    @brief RDB File for PV

    @version 2018May25_rdb
*/

#ifndef PV_RDB_H
#define PV_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t PV_C_TYPE;
#define PV_C_HVS_PIXRC2_MASK (0x20000000UL)
#define PV_C_HVS_PIXRC2_SHIFT (29UL)
#define PV_C_SPARE_MASK (0x10000000UL)
#define PV_C_SPARE_SHIFT (28UL)
#define PV_C_PIX_F_UPPER_MASK (0x8000000UL)
#define PV_C_PIX_F_UPPER_SHIFT (27UL)
#define PV_C_FIFO_FULL_LEVEL_HIGH_MASK (0x6000000UL)
#define PV_C_FIFO_FULL_LEVEL_HIGH_SHIFT (25UL)
#define PV_C_PIX_CLK_MUX_EN_MASK (0x1000000UL)
#define PV_C_PIX_CLK_MUX_EN_SHIFT (24UL)
#define PV_C_PIX_F_MASK (0xe00000UL)
#define PV_C_PIX_F_SHIFT (21UL)
#define PV_C_FIFO_FULL_LEVEL_MASK (0x1f8000UL)
#define PV_C_FIFO_FULL_LEVEL_SHIFT (15UL)
#define PV_C_FIFOCLRC_MASK (0x4000UL)
#define PV_C_FIFOCLRC_SHIFT (14UL)
#define PV_C_HVS_PIXRC1_MASK (0x2000UL)
#define PV_C_HVS_PIXRC1_SHIFT (13UL)
#define PV_C_HVS_PIXRC0_MASK (0x1000UL)
#define PV_C_HVS_PIXRC0_SHIFT (12UL)
#define PV_C_PIX_STRETCH_MASK (0xff0UL)
#define PV_C_PIX_STRETCH_SHIFT (4UL)
#define PV_C_PCLK_SEL_MASK (0xcUL)
#define PV_C_PCLK_SEL_SHIFT (2UL)
#define PV_C_FIFO_CLR_MASK (0x2UL)
#define PV_C_FIFO_CLR_SHIFT (1UL)
#define PV_C_PVEN_MASK (0x1UL)
#define PV_C_PVEN_SHIFT (0UL)




typedef uint32_t PV_VC_TYPE;
#define PV_VC_USBAV_MODE_MASK (0x800000UL)
#define PV_VC_USBAV_MODE_SHIFT (23UL)
#define PV_VC_VSYNCD_MASK (0x7fffc0UL)
#define PV_VC_VSYNCD_SHIFT (6UL)
#define PV_VC_FIRST_INT_POL_ODD_MASK (0x20UL)
#define PV_VC_FIRST_INT_POL_ODD_SHIFT (5UL)
#define PV_VC_INT_MODE_MASK (0x10UL)
#define PV_VC_INT_MODE_SHIFT (4UL)
#define PV_VC_DSI_VMODE_MASK (0x8UL)
#define PV_VC_DSI_VMODE_SHIFT (3UL)
#define PV_VC_MODE_MASK (0x4UL)
#define PV_VC_MODE_SHIFT (2UL)
#define PV_VC_FRAMEC_MASK (0x2UL)
#define PV_VC_FRAMEC_SHIFT (1UL)
#define PV_VC_VIDEN_MASK (0x1UL)
#define PV_VC_VIDEN_SHIFT (0UL)




typedef uint32_t PV_VSYNCD_EVEN_TYPE;
#define PV_VSYNCD_EVEN_EVEN_MASK (0x1ffffUL)
#define PV_VSYNCD_EVEN_EVEN_SHIFT (0UL)




typedef uint32_t PV_HORZA_TYPE;
#define PV_HORZA_HBP_WIDTH_MASK (0xffff0000UL)
#define PV_HORZA_HBP_WIDTH_SHIFT (16UL)
#define PV_HORZA_HS_WIDTH_MASK (0xffffUL)
#define PV_HORZA_HS_WIDTH_SHIFT (0UL)




typedef uint32_t PV_HORZB_TYPE;
#define PV_HORZB_HFP_WIDTH_MASK (0xffff0000UL)
#define PV_HORZB_HFP_WIDTH_SHIFT (16UL)
#define PV_HORZB_HACT_WIDTH_MASK (0xffffUL)
#define PV_HORZB_HACT_WIDTH_SHIFT (0UL)




typedef uint32_t PV_VERTA_TYPE;
#define PV_VERTA_VBP_WIDTH_MASK (0xffff0000UL)
#define PV_VERTA_VBP_WIDTH_SHIFT (16UL)
#define PV_VERTA_VS_WIDTH_MASK (0xffffUL)
#define PV_VERTA_VS_WIDTH_SHIFT (0UL)




typedef uint32_t PV_VERTB_TYPE;
#define PV_VERTB_VFP_WIDTH_MASK (0xffff0000UL)
#define PV_VERTB_VFP_WIDTH_SHIFT (16UL)
#define PV_VERTB_VACT_WIDTH_MASK (0xffffUL)
#define PV_VERTB_VACT_WIDTH_SHIFT (0UL)




typedef uint32_t PV_VERTA_EVEN_TYPE;
#define PV_VERTA_EVEN_VBP_WIDTH_EVEN_MASK (0xffff0000UL)
#define PV_VERTA_EVEN_VBP_WIDTH_EVEN_SHIFT (16UL)
#define PV_VERTA_EVEN_VS_WIDTH_EVEN_MASK (0xffffUL)
#define PV_VERTA_EVEN_VS_WIDTH_EVEN_SHIFT (0UL)




typedef uint32_t PV_VERTB_EVEN_TYPE;
#define PV_VERTB_EVEN_VFP_WIDTH_EVEN_MASK (0xffff0000UL)
#define PV_VERTB_EVEN_VFP_WIDTH_EVEN_SHIFT (16UL)
#define PV_VERTB_EVEN_VACT_WIDTH_EVEN_MASK (0xffffUL)
#define PV_VERTB_EVEN_VACT_WIDTH_EVEN_SHIFT (0UL)




typedef uint32_t PV_INTEN_TYPE;
#define PV_INTEN_PROG_PULSE_MASK (0x800UL)
#define PV_INTEN_PROG_PULSE_SHIFT (11UL)
#define PV_INTEN_OF_UF_MASK (0x400UL)
#define PV_INTEN_OF_UF_SHIFT (10UL)
#define PV_INTEN_VID_IDLE_MASK (0x200UL)
#define PV_INTEN_VID_IDLE_SHIFT (9UL)
#define PV_INTEN_VFP_END_MASK (0x100UL)
#define PV_INTEN_VFP_END_SHIFT (8UL)
#define PV_INTEN_VBP_START_MASK (0x80UL)
#define PV_INTEN_VBP_START_SHIFT (7UL)
#define PV_INTEN_VACT_START_MASK (0x40UL)
#define PV_INTEN_VACT_START_SHIFT (6UL)
#define PV_INTEN_VFP_START_MASK (0x20UL)
#define PV_INTEN_VFP_START_SHIFT (5UL)
#define PV_INTEN_VSYNC_START_MASK (0x10UL)
#define PV_INTEN_VSYNC_START_SHIFT (4UL)
#define PV_INTEN_HFP_START_MASK (0x8UL)
#define PV_INTEN_HFP_START_SHIFT (3UL)
#define PV_INTEN_HACT_START_MASK (0x4UL)
#define PV_INTEN_HACT_START_SHIFT (2UL)
#define PV_INTEN_HBP_START_MASK (0x2UL)
#define PV_INTEN_HBP_START_SHIFT (1UL)
#define PV_INTEN_HSYNC_START_MASK (0x1UL)
#define PV_INTEN_HSYNC_START_SHIFT (0UL)




typedef uint32_t PV_INTSTAT_TYPE;
#define PV_INTSTAT_PROG_PULSE_MASK (0x800UL)
#define PV_INTSTAT_PROG_PULSE_SHIFT (11UL)
#define PV_INTSTAT_OF_UF_MASK (0x400UL)
#define PV_INTSTAT_OF_UF_SHIFT (10UL)
#define PV_INTSTAT_VID_IDLE_MASK (0x200UL)
#define PV_INTSTAT_VID_IDLE_SHIFT (9UL)
#define PV_INTSTAT_VFP_END_MASK (0x100UL)
#define PV_INTSTAT_VFP_END_SHIFT (8UL)
#define PV_INTSTAT_VBP_START_MASK (0x80UL)
#define PV_INTSTAT_VBP_START_SHIFT (7UL)
#define PV_INTSTAT_VACT_START_MASK (0x40UL)
#define PV_INTSTAT_VACT_START_SHIFT (6UL)
#define PV_INTSTAT_VFP_START_MASK (0x20UL)
#define PV_INTSTAT_VFP_START_SHIFT (5UL)
#define PV_INTSTAT_VSYNC_START_MASK (0x10UL)
#define PV_INTSTAT_VSYNC_START_SHIFT (4UL)
#define PV_INTSTAT_HFP_START_MASK (0x8UL)
#define PV_INTSTAT_HFP_START_SHIFT (3UL)
#define PV_INTSTAT_HACT_START_MASK (0x4UL)
#define PV_INTSTAT_HACT_START_SHIFT (2UL)
#define PV_INTSTAT_HBP_START_MASK (0x2UL)
#define PV_INTSTAT_HBP_START_SHIFT (1UL)
#define PV_INTSTAT_HSYNC_START_MASK (0x1UL)
#define PV_INTSTAT_HSYNC_START_SHIFT (0UL)




typedef uint32_t PV_STAT_TYPE;
#define PV_STAT_PV_FIFO_RCOUNT_MASK (0xff000000UL)
#define PV_STAT_PV_FIFO_RCOUNT_SHIFT (24UL)
#define PV_STAT_PV_FIFO_COUNT_MASK (0xff0000UL)
#define PV_STAT_PV_FIFO_COUNT_SHIFT (16UL)
#define PV_STAT_VID_EN_USBAV_MASK (0x800UL)
#define PV_STAT_VID_EN_USBAV_SHIFT (11UL)
#define PV_STAT_HVS_OF_MASK (0x400UL)
#define PV_STAT_HVS_OF_SHIFT (10UL)
#define PV_STAT_PV_UF_MASK (0x200UL)
#define PV_STAT_PV_UF_SHIFT (9UL)
#define PV_STAT_HVS_UF_MASK (0x100UL)
#define PV_STAT_HVS_UF_SHIFT (8UL)
#define PV_STAT_VFP_MASK (0x80UL)
#define PV_STAT_VFP_SHIFT (7UL)
#define PV_STAT_VACT_MASK (0x40UL)
#define PV_STAT_VACT_SHIFT (6UL)
#define PV_STAT_VBP_MASK (0x20UL)
#define PV_STAT_VBP_SHIFT (5UL)
#define PV_STAT_VSYNC_MASK (0x10UL)
#define PV_STAT_VSYNC_SHIFT (4UL)
#define PV_STAT_HFP_MASK (0x8UL)
#define PV_STAT_HFP_SHIFT (3UL)
#define PV_STAT_HACT_MASK (0x4UL)
#define PV_STAT_HACT_SHIFT (2UL)
#define PV_STAT_HBP_MASK (0x2UL)
#define PV_STAT_HBP_SHIFT (1UL)
#define PV_STAT_HSYNC_MASK (0x1UL)
#define PV_STAT_HSYNC_SHIFT (0UL)




typedef uint32_t PV_HACT_ACT_TYPE;
#define PV_HACT_ACT_DSI_ACT_MASK (0xffffUL)
#define PV_HACT_ACT_DSI_ACT_SHIFT (0UL)




typedef uint32_t PV_MUX_CFG_TYPE;
#define PV_MUX_CFG_RGB_PIXEL_MUX_MODE_MASK (0x3cUL)
#define PV_MUX_CFG_RGB_PIXEL_MUX_MODE_SHIFT (2UL)
#define PV_MUX_CFG_PIXEL_BIT_ORDER_MASK (0x2UL)
#define PV_MUX_CFG_PIXEL_BIT_ORDER_SHIFT (1UL)
#define PV_MUX_CFG_PIXEL_MUX_ENDIANNESS_MASK (0x1UL)
#define PV_MUX_CFG_PIXEL_MUX_ENDIANNESS_SHIFT (0UL)




typedef uint32_t PV_C2_TYPE;
#define PV_C2_HVS_PIXELS_PER_CYCLE_MASK (0x20UL)
#define PV_C2_HVS_PIXELS_PER_CYCLE_SHIFT (5UL)
#define PV_C2_HVS_PV_PROG_PULSE_EN_MASK (0x10UL)
#define PV_C2_HVS_PV_PROG_PULSE_EN_SHIFT (4UL)
#define PV_C2_HVS_PV_REQ_LINE_OFFSET_MASK (0xfUL)
#define PV_C2_HVS_PV_REQ_LINE_OFFSET_SHIFT (0UL)




typedef uint32_t PV_PULSE_MATCH_V_TYPE;
#define PV_PULSE_MATCH_V_HVS_ST_MASK (0x70000UL)
#define PV_PULSE_MATCH_V_HVS_ST_SHIFT (16UL)
#define PV_PULSE_MATCH_V_HVS_CNT_MASK (0xffffUL)
#define PV_PULSE_MATCH_V_HVS_CNT_SHIFT (0UL)




typedef uint32_t PV_PULSE_MATCH_H_TYPE;
#define PV_PULSE_MATCH_H_HVS_MATCST_MASK (0x70000UL)
#define PV_PULSE_MATCH_H_HVS_MATCST_SHIFT (16UL)
#define PV_PULSE_MATCH_H_HVS_MATCCNT_MASK (0xffffUL)
#define PV_PULSE_MATCH_H_HVS_MATCCNT_SHIFT (0UL)




typedef uint32_t PV_VERSION_TYPE;
#define PV_VERSION_MAJOR_MASK (0xff0000UL)
#define PV_VERSION_MAJOR_SHIFT (16UL)
#define PV_VERSION_MINOR_MASK (0xffffUL)
#define PV_VERSION_MINOR_SHIFT (0UL)




typedef volatile struct COMP_PACKED _PV_RDBType {
    PV_C_TYPE c; /* OFFSET: 0x0 */
    PV_VC_TYPE vc; /* OFFSET: 0x4 */
    PV_VSYNCD_EVEN_TYPE vsyncd_even; /* OFFSET: 0x8 */
    PV_HORZA_TYPE horza; /* OFFSET: 0xc */
    PV_HORZB_TYPE horzb; /* OFFSET: 0x10 */
    PV_VERTA_TYPE verta; /* OFFSET: 0x14 */
    PV_VERTB_TYPE vertb; /* OFFSET: 0x18 */
    PV_VERTA_EVEN_TYPE verta_even; /* OFFSET: 0x1c */
    PV_VERTB_EVEN_TYPE vertb_even; /* OFFSET: 0x20 */
    PV_INTEN_TYPE inten; /* OFFSET: 0x24 */
    PV_INTSTAT_TYPE intstat; /* OFFSET: 0x28 */
    PV_STAT_TYPE stat; /* OFFSET: 0x2c */
    PV_HACT_ACT_TYPE dsi_hact_act; /* OFFSET: 0x30 */
    PV_MUX_CFG_TYPE pv_mux_cfg; /* OFFSET: 0x34 */
    PV_C2_TYPE c2; /* OFFSET: 0x38 */
    PV_PULSE_MATCH_V_TYPE pulse_match_v; /* OFFSET: 0x3c */
    PV_PULSE_MATCH_H_TYPE pulse_match_h; /* OFFSET: 0x40 */
    PV_VERSION_TYPE pv_version; /* OFFSET: 0x44 */
} PV_RDBType;


#define PV_BASE                         (0x400A5000UL)



#define PV_MAX_HW_ID                    (1UL)


#define PV_INTSTAT_RESERVED0_MASK       (0xfffff000UL)


#define PV_C_FIFO_FULL_LEVEL_9          (0x00000009UL)


#define PV_C_FIFO_FULL_LEVEL_HIGH_3     (0x00000003UL)


#define PV_C_PIX_F_18_24BPP_DSI_CMD_VID  (0x00000004UL)


#define PV_C_PCLK_SEL_DSI               (0x00000000UL)


#define PV_C_PCLK_SEL_DVP               (0x00000001UL)


#define PV_PULSE_MATCH_V_V_ST_V_ADR     (4UL)


#define PV_PULSE_MATCH_H_H_ST_H_FP      (4UL)

#endif /* PV_RDB_H */