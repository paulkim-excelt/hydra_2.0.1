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
    @file imgsys_rdb.h
    @brief RDB File for IMGSYS

    @version 2018May25_rdb
*/

#ifndef IMGSYS_RDB_H
#define IMGSYS_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t IMGSYS_CBUF_FIFO_PARAMS_TYPE;
#define IMGSYS_CBUF_FIFO_PARAMS_WM_MASK (0xff0000UL)
#define IMGSYS_CBUF_FIFO_PARAMS_WM_SHIFT (16UL)
#define IMGSYS_CBUF_FIFO_PARAMS_SIZE_MASK (0xff00UL)
#define IMGSYS_CBUF_FIFO_PARAMS_SIZE_SHIFT (8UL)
#define IMGSYS_CBUF_FIFO_PARAMS_OFFSET_MASK (0xffUL)
#define IMGSYS_CBUF_FIFO_PARAMS_OFFSET_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_CTRL_TYPE;
#define IMGSYS_CBUF_FIFO_CTRL_CH1_WM_EN_MASK (0x80UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH1_WM_EN_SHIFT (7UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH0_WM_EN_MASK (0x40UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH0_WM_EN_SHIFT (6UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH1_OV_EN_MASK (0x20UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH1_OV_EN_SHIFT (5UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH0_OV_EN_MASK (0x10UL)
#define IMGSYS_CBUF_FIFO_CTRL_CH0_OV_EN_SHIFT (4UL)
#define IMGSYS_CBUF_FIFO_CTRL_INTR_EN_MASK (0x1UL)
#define IMGSYS_CBUF_FIFO_CTRL_INTR_EN_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_STATUS_TYPE;
#define IMGSYS_CBUF_FIFO_STATUS_CH1_WM_ERR_MASK (0x80UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH1_WM_ERR_SHIFT (7UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH0_WM_ERR_MASK (0x40UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH0_WM_ERR_SHIFT (6UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH1_OV_ERR_MASK (0x20UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH1_OV_ERR_SHIFT (5UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH0_OV_ERR_MASK (0x10UL)
#define IMGSYS_CBUF_FIFO_STATUS_CH0_OV_ERR_SHIFT (4UL)
#define IMGSYS_CBUF_FIFO_STATUS_INTR_STAT_WM_ERR_MASK (0x2UL)
#define IMGSYS_CBUF_FIFO_STATUS_INTR_STAT_WM_ERR_SHIFT (1UL)
#define IMGSYS_CBUF_FIFO_STATUS_INTR_STAT_OV_ERR_MASK (0x1UL)
#define IMGSYS_CBUF_FIFO_STATUS_INTR_STAT_OV_ERR_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_LC_STAT_TYPE;
#define IMGSYS_CBUF_FIFO_LC_STAT_LINES_TO_DRAIN1_MASK (0xff000000UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINES_TO_DRAIN1_SHIFT (24UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINES_TO_DRAIN0_MASK (0xff0000UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINES_TO_DRAIN0_SHIFT (16UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINE_FIFO_CNT1_MASK (0xff00UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINE_FIFO_CNT1_SHIFT (8UL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINE_FIFO_CNT0_MASK (0xffUL)
#define IMGSYS_CBUF_FIFO_LC_STAT_LINE_FIFO_CNT0_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_FS_STAT_TYPE;
#define IMGSYS_CBUF_FIFO_FS_STAT_NUM_ACKS_MASK (0x3ff0000UL)
#define IMGSYS_CBUF_FIFO_FS_STAT_NUM_ACKS_SHIFT (16UL)
#define IMGSYS_CBUF_FIFO_FS_STAT_NUM_FS_MASK (0x3ffUL)
#define IMGSYS_CBUF_FIFO_FS_STAT_NUM_FS_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_FE_STAT_TYPE;
#define IMGSYS_CBUF_FIFO_FE_STAT_NUM_FE_MASK (0x3ffUL)
#define IMGSYS_CBUF_FIFO_FE_STAT_NUM_FE_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_L0_STAT_TYPE;
#define IMGSYS_CBUF_FIFO_L0_STAT_NUM_LINES_ACK_0_MASK (0x7ff0000UL)
#define IMGSYS_CBUF_FIFO_L0_STAT_NUM_LINES_ACK_0_SHIFT (16UL)
#define IMGSYS_CBUF_FIFO_L0_STAT_NUM_LINES_RCV_0_MASK (0x7ffUL)
#define IMGSYS_CBUF_FIFO_L0_STAT_NUM_LINES_RCV_0_SHIFT (0UL)




typedef uint32_t IMGSYS_CBUF_FIFO_L1_STAT_TYPE;
#define IMGSYS_CBUF_FIFO_L1_STAT_NUM_LINES_ACK_0_MASK (0x7ff0000UL)
#define IMGSYS_CBUF_FIFO_L1_STAT_NUM_LINES_ACK_0_SHIFT (16UL)
#define IMGSYS_CBUF_FIFO_L1_STAT_NUM_LINES_RCV_0_MASK (0x7ffUL)
#define IMGSYS_CBUF_FIFO_L1_STAT_NUM_LINES_RCV_0_SHIFT (0UL)




typedef uint32_t IMGSYS_DBG_SEL_TYPE;
#define IMGSYS_DBG_SEL_SEL_MASK (0x1ffUL)
#define IMGSYS_DBG_SEL_SEL_SHIFT (0UL)




typedef uint32_t IMGSYS_MEM_CTL_TYPE;
#define IMGSYS_MEM_CTL_CTL_MASK (0xffUL)
#define IMGSYS_MEM_CTL_CTL_SHIFT (0UL)




typedef volatile struct COMP_PACKED _IMGSYS_RDBType {
    IMGSYS_CBUF_FIFO_PARAMS_TYPE cbuf_fifo_params; /* OFFSET: 0x0 */
    IMGSYS_CBUF_FIFO_CTRL_TYPE cbuf_fifo_ctrl; /* OFFSET: 0x4 */
    IMGSYS_CBUF_FIFO_STATUS_TYPE cbuf_fifo_status; /* OFFSET: 0x8 */
    IMGSYS_CBUF_FIFO_LC_STAT_TYPE cbuf_fifo_lc_stat; /* OFFSET: 0xc */
    IMGSYS_CBUF_FIFO_FS_STAT_TYPE cbuf_fifo_fs_stat; /* OFFSET: 0x10 */
    IMGSYS_CBUF_FIFO_FE_STAT_TYPE cbuf_fifo_fe_stat; /* OFFSET: 0x14 */
    IMGSYS_CBUF_FIFO_L0_STAT_TYPE cbuf_fifo_l0_stat; /* OFFSET: 0x18 */
    IMGSYS_CBUF_FIFO_L1_STAT_TYPE cbuf_fifo_l1_stat; /* OFFSET: 0x1c */
    IMGSYS_DBG_SEL_TYPE img_sys_dbg_sel; /* OFFSET: 0x20 */
    IMGSYS_MEM_CTL_TYPE img_sys_mem_ctl; /* OFFSET: 0x24 */
} IMGSYS_RDBType;


#define IMGSYS_BASE                     (0x40084000UL)



#define IMGSYS_MAX_HW_ID                (1UL)

#endif /* IMGSYS_RDB_H */