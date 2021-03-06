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
    @file dma330_rdb.h
    @brief RDB File for DMA330

    @version 2018May25_rdb
*/

#ifndef DMA330_RDB_H
#define DMA330_RDB_H

#include <stdint.h>

#include <compiler.h>


typedef uint32_t DMA330_DSR_TYPE;
#define DMA330_DSR_DNS_MASK (0x200UL)
#define DMA330_DSR_DNS_SHIFT (9UL)
#define DMA330_DSR_WAKEUP_EVENT_MASK (0x1f0UL)
#define DMA330_DSR_WAKEUP_EVENT_SHIFT (4UL)
#define DMA330_DSR_DMA_STATUS_MASK (0xfUL)
#define DMA330_DSR_DMA_STATUS_SHIFT (0UL)




typedef uint32_t DMA330_DPC_TYPE;
#define DMA330_DPC_PC_MGR_MASK (0xffffffffUL)
#define DMA330_DPC_PC_MGR_SHIFT (0UL)




typedef uint8_t DMA330_RESERVED_TYPE;




typedef uint32_t DMA330_INTEN_TYPE;
#define DMA330_INTEN_EVENT_IRQ_SELECT_MASK (0x3ffUL)
#define DMA330_INTEN_EVENT_IRQ_SELECT_SHIFT (0UL)




typedef uint32_t DMA330_INT_EVENT_RIS_TYPE;
#define DMA330_INT_EVENT_RIS_DMASEV_ACTIVE_MASK (0xffffffffUL)
#define DMA330_INT_EVENT_RIS_DMASEV_ACTIVE_SHIFT (0UL)




typedef uint32_t DMA330_INTMIS_TYPE;
#define DMA330_INTMIS_IRQ_STATUS_MASK (0xffffffffUL)
#define DMA330_INTMIS_IRQ_STATUS_SHIFT (0UL)




typedef uint32_t DMA330_INTCLR_TYPE;
#define DMA330_INTCLR_IRQ_CLR_MASK (0xffffffffUL)
#define DMA330_INTCLR_IRQ_CLR_SHIFT (0UL)




typedef uint32_t DMA330_FSRD_TYPE;
#define DMA330_FSRD_FS_MGR_MASK (0x1UL)
#define DMA330_FSRD_FS_MGR_SHIFT (0UL)




typedef uint32_t DMA330_FSRC_TYPE;
#define DMA330_FSRC_FAULT_STATUS_MASK (0xffUL)
#define DMA330_FSRC_FAULT_STATUS_SHIFT (0UL)




typedef uint32_t DMA330_FTRD_TYPE;
#define DMA330_FTRD_DBG_INSTR_MASK (0x40000000UL)
#define DMA330_FTRD_DBG_INSTR_SHIFT (30UL)
#define DMA330_FTRD_INSTR_FETCH_ERR_MASK (0x10000UL)
#define DMA330_FTRD_INSTR_FETCH_ERR_SHIFT (16UL)
#define DMA330_FTRD_MGR_EVNT_ERR_MASK (0x20UL)
#define DMA330_FTRD_MGR_EVNT_ERR_SHIFT (5UL)
#define DMA330_FTRD_DMAGO_ERR_MASK (0x10UL)
#define DMA330_FTRD_DMAGO_ERR_SHIFT (4UL)
#define DMA330_FTRD_OPERAND_INVALID_MASK (0x2UL)
#define DMA330_FTRD_OPERAND_INVALID_SHIFT (1UL)
#define DMA330_FTRD_UNDEF_INSTR_MASK (0x1UL)
#define DMA330_FTRD_UNDEF_INSTR_SHIFT (0UL)




typedef uint32_t DMA330_FTR_TYPE;
#define DMA330_FTR_LOCKUP_ERR_MASK (0x80000000UL)
#define DMA330_FTR_LOCKUP_ERR_SHIFT (31UL)
#define DMA330_FTR_DBG_INSTR_MASK (0x40000000UL)
#define DMA330_FTR_DBG_INSTR_SHIFT (30UL)
#define DMA330_FTR_DATA_READ_ERR_MASK (0x40000UL)
#define DMA330_FTR_DATA_READ_ERR_SHIFT (18UL)
#define DMA330_FTR_DATA_WRITE_ERR_MASK (0x20000UL)
#define DMA330_FTR_DATA_WRITE_ERR_SHIFT (17UL)
#define DMA330_FTR_INSTR_FETCH_ERR_MASK (0x10000UL)
#define DMA330_FTR_INSTR_FETCH_ERR_SHIFT (16UL)
#define DMA330_FTR_ST_DATA_UNAVAILABLE_MASK (0x2000UL)
#define DMA330_FTR_ST_DATA_UNAVAILABLE_SHIFT (13UL)
#define DMA330_FTR_MFIFO_ERR_MASK (0x1000UL)
#define DMA330_FTR_MFIFO_ERR_SHIFT (12UL)
#define DMA330_FTR_CH_RDWR_ERR_MASK (0x80UL)
#define DMA330_FTR_CH_RDWR_ERR_SHIFT (7UL)
#define DMA330_FTR_CH_PERIPH_ERR_MASK (0x40UL)
#define DMA330_FTR_CH_PERIPH_ERR_SHIFT (6UL)
#define DMA330_FTR_CH_EVNT_ERR_MASK (0x20UL)
#define DMA330_FTR_CH_EVNT_ERR_SHIFT (5UL)
#define DMA330_FTR_OPERAND_INVALID_MASK (0x2UL)
#define DMA330_FTR_OPERAND_INVALID_SHIFT (1UL)
#define DMA330_FTR_UNDEF_INSTR_MASK (0x1UL)
#define DMA330_FTR_UNDEF_INSTR_SHIFT (0UL)




typedef uint32_t DMA330_CSR_TYPE;
#define DMA330_CSR_CNS_MASK (0x200000UL)
#define DMA330_CSR_CNS_SHIFT (21UL)
#define DMA330_CSR_DMAWFP_PERIPH_MASK (0x8000UL)
#define DMA330_CSR_DMAWFP_PERIPH_SHIFT (15UL)
#define DMA330_CSR_DMAWFP_B_NS_MASK (0x4000UL)
#define DMA330_CSR_DMAWFP_B_NS_SHIFT (14UL)
#define DMA330_CSR_WAKEUP_NUMBER_MASK (0x1f0UL)
#define DMA330_CSR_WAKEUP_NUMBER_SHIFT (4UL)
#define DMA330_CSR_CHANNEL_STATUS_MASK (0xfUL)
#define DMA330_CSR_CHANNEL_STATUS_SHIFT (0UL)




typedef uint32_t DMA330_CPC_TYPE;
#define DMA330_CPC_PC_CHNL_MASK (0xffffffffUL)
#define DMA330_CPC_PC_CHNL_SHIFT (0UL)




typedef uint32_t DMA330_SAR_TYPE;
#define DMA330_SAR_SRC_ADDR_MASK (0xffffffffUL)
#define DMA330_SAR_SRC_ADDR_SHIFT (0UL)




typedef uint32_t DMA330_DAR_TYPE;
#define DMA330_DAR_DST_ADDR_MASK (0xffffffffUL)
#define DMA330_DAR_DST_ADDR_SHIFT (0UL)




typedef uint32_t DMA330_CCR_TYPE;
#define DMA330_CCR_ENDIAN_SWAP_SIZE_MASK (0x70000000UL)
#define DMA330_CCR_ENDIAN_SWAP_SIZE_SHIFT (28UL)
#define DMA330_CCR_DST_CACHE_CTRL_MASK (0xe000000UL)
#define DMA330_CCR_DST_CACHE_CTRL_SHIFT (25UL)
#define DMA330_CCR_DST_PROT_CTRL_MASK (0x1c00000UL)
#define DMA330_CCR_DST_PROT_CTRL_SHIFT (22UL)
#define DMA330_CCR_DST_BURST_LEN_MASK (0x3c0000UL)
#define DMA330_CCR_DST_BURST_LEN_SHIFT (18UL)
#define DMA330_CCR_DST_BURST_SIZE_MASK (0x38000UL)
#define DMA330_CCR_DST_BURST_SIZE_SHIFT (15UL)
#define DMA330_CCR_DST_INC_MASK (0x4000UL)
#define DMA330_CCR_DST_INC_SHIFT (14UL)
#define DMA330_CCR_SRC_CACHE_CTRL_MASK (0x3800UL)
#define DMA330_CCR_SRC_CACHE_CTRL_SHIFT (11UL)
#define DMA330_CCR_SRC_PROT_CTRL_MASK (0x700UL)
#define DMA330_CCR_SRC_PROT_CTRL_SHIFT (8UL)
#define DMA330_CCR_SRC_BURST_LEN_MASK (0xf0UL)
#define DMA330_CCR_SRC_BURST_LEN_SHIFT (4UL)
#define DMA330_CCR_SRC_BURST_SIZE_MASK (0xeUL)
#define DMA330_CCR_SRC_BURST_SIZE_SHIFT (1UL)
#define DMA330_CCR_SRC_INC_MASK (0x1UL)
#define DMA330_CCR_SRC_INC_SHIFT (0UL)




typedef uint32_t DMA330_LC0_TYPE;
#define DMA330_LC0_LOOP_COUNTER_ITERATIONS_MASK (0xffUL)
#define DMA330_LC0_LOOP_COUNTER_ITERATIONS_SHIFT (0UL)




typedef uint32_t DMA330_LC1_TYPE;
#define DMA330_LC1_LOOP_COUNTER_ITERATIONS_MASK (0xffUL)
#define DMA330_LC1_LOOP_COUNTER_ITERATIONS_SHIFT (0UL)




typedef uint32_t DMA330_DBGSTATUS_TYPE;
#define DMA330_DBGSTATUS_DBGSTATUS_MASK (0x1UL)
#define DMA330_DBGSTATUS_DBGSTATUS_SHIFT (0UL)




typedef uint32_t DMA330_DBGCMD_TYPE;
#define DMA330_DBGCMD_DBGCMD_MASK (0x3UL)
#define DMA330_DBGCMD_DBGCMD_SHIFT (0UL)




typedef uint32_t DMA330_DBGINST0_TYPE;
#define DMA330_DBGINST0_INSTRUCTION_BYTE_1_MASK (0xff000000UL)
#define DMA330_DBGINST0_INSTRUCTION_BYTE_1_SHIFT (24UL)
#define DMA330_DBGINST0_INSTRUCTION_BYTE_0_MASK (0xff0000UL)
#define DMA330_DBGINST0_INSTRUCTION_BYTE_0_SHIFT (16UL)
#define DMA330_DBGINST0_CHANNEL_NUMBER_MASK (0x700UL)
#define DMA330_DBGINST0_CHANNEL_NUMBER_SHIFT (8UL)
#define DMA330_DBGINST0_DEBUG_THREAD_MASK (0x1UL)
#define DMA330_DBGINST0_DEBUG_THREAD_SHIFT (0UL)




typedef uint32_t DMA330_DBGINST1_TYPE;
#define DMA330_DBGINST1_INSTRUCTION_BYTE_5_MASK (0xff000000UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_5_SHIFT (24UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_4_MASK (0xff0000UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_4_SHIFT (16UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_3_MASK (0xff00UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_3_SHIFT (8UL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_2_MASK (0xffUL)
#define DMA330_DBGINST1_INSTRUCTION_BYTE_2_SHIFT (0UL)




typedef uint32_t DMA330_CR0_TYPE;
#define DMA330_CR0_NUM_EVENTS_MASK (0x3e0000UL)
#define DMA330_CR0_NUM_EVENTS_SHIFT (17UL)
#define DMA330_CR0_NUM_PERIPH_REQ_MASK (0x1f000UL)
#define DMA330_CR0_NUM_PERIPH_REQ_SHIFT (12UL)
#define DMA330_CR0_NUM_CHNLS_MASK (0x70UL)
#define DMA330_CR0_NUM_CHNLS_SHIFT (4UL)
#define DMA330_CR0_MGR_NS_AT_RST_MASK (0x4UL)
#define DMA330_CR0_MGR_NS_AT_RST_SHIFT (2UL)
#define DMA330_CR0_BOOT_EN_MASK (0x2UL)
#define DMA330_CR0_BOOT_EN_SHIFT (1UL)
#define DMA330_CR0_PERIPH_REQ_MASK (0x1UL)
#define DMA330_CR0_PERIPH_REQ_SHIFT (0UL)




typedef uint32_t DMA330_CR1_TYPE;
#define DMA330_CR1_NUM_I_CACHE_LINES_MASK (0xf0UL)
#define DMA330_CR1_NUM_I_CACHE_LINES_SHIFT (4UL)
#define DMA330_CR1_I_CACHE_LEN_MASK (0x7UL)
#define DMA330_CR1_I_CACHE_LEN_SHIFT (0UL)




typedef uint32_t DMA330_CR2_TYPE;
#define DMA330_CR2_BOOT_ADDR_MASK (0xffffffffUL)
#define DMA330_CR2_BOOT_ADDR_SHIFT (0UL)




typedef uint32_t DMA330_CR3_TYPE;
#define DMA330_CR3_INS_MASK (0xffffffffUL)
#define DMA330_CR3_INS_SHIFT (0UL)




typedef uint32_t DMA330_CR4_TYPE;
#define DMA330_CR4_PNS_MASK (0xffffffffUL)
#define DMA330_CR4_PNS_SHIFT (0UL)




typedef uint32_t DMA330_CRD_TYPE;
#define DMA330_CRD_DATA_BUFFER_DEP_MASK (0x3ff00000UL)
#define DMA330_CRD_DATA_BUFFER_DEP_SHIFT (20UL)
#define DMA330_CRD_RD_Q_DEP_MASK (0xf0000UL)
#define DMA330_CRD_RD_Q_DEP_SHIFT (16UL)
#define DMA330_CRD_RD_CAP_MASK (0x7000UL)
#define DMA330_CRD_RD_CAP_SHIFT (12UL)
#define DMA330_CRD_WR_Q_DEP_MASK (0xf00UL)
#define DMA330_CRD_WR_Q_DEP_SHIFT (8UL)
#define DMA330_CRD_WR_CAP_MASK (0x70UL)
#define DMA330_CRD_WR_CAP_SHIFT (4UL)
#define DMA330_CRD_DATA_WIDTH_MASK (0x7UL)
#define DMA330_CRD_DATA_WIDTH_SHIFT (0UL)




typedef uint32_t DMA330_WD_TYPE;
#define DMA330_WD_IRQ_ONLY_MASK (0x1UL)
#define DMA330_WD_IRQ_ONLY_SHIFT (0UL)




typedef uint32_t DMA330_PERIPH_ID_0_TYPE;
#define DMA330_PERIPH_ID_0_PART_NUMBER_0_MASK (0xffUL)
#define DMA330_PERIPH_ID_0_PART_NUMBER_0_SHIFT (0UL)




typedef uint32_t DMA330_PERIPH_ID_1_TYPE;
#define DMA330_PERIPH_ID_1_DESIGNER_0_MASK (0xf0UL)
#define DMA330_PERIPH_ID_1_DESIGNER_0_SHIFT (4UL)
#define DMA330_PERIPH_ID_1_PART_NUMBER_1_MASK (0xfUL)
#define DMA330_PERIPH_ID_1_PART_NUMBER_1_SHIFT (0UL)




typedef uint32_t DMA330_PERIPH_ID_2_TYPE;
#define DMA330_PERIPH_ID_2_REVISION_MASK (0xf0UL)
#define DMA330_PERIPH_ID_2_REVISION_SHIFT (4UL)
#define DMA330_PERIPH_ID_2_DESIGNER_1_MASK (0xfUL)
#define DMA330_PERIPH_ID_2_DESIGNER_1_SHIFT (0UL)




typedef uint32_t DMA330_PERIPH_ID_3_TYPE;
#define DMA330_PERIPH_ID_3_INTEGRATION_CFG_MASK (0x1UL)
#define DMA330_PERIPH_ID_3_INTEGRATION_CFG_SHIFT (0UL)




typedef uint32_t DMA330_PCELL_ID_0_TYPE;
#define DMA330_PCELL_ID_0_0_MASK (0xffUL)
#define DMA330_PCELL_ID_0_0_SHIFT (0UL)




typedef uint32_t DMA330_PCELL_ID_1_TYPE;
#define DMA330_PCELL_ID_1_1_MASK (0xffUL)
#define DMA330_PCELL_ID_1_1_SHIFT (0UL)




typedef uint32_t DMA330_PCELL_ID_2_TYPE;
#define DMA330_PCELL_ID_2_2_MASK (0xffUL)
#define DMA330_PCELL_ID_2_2_SHIFT (0UL)




typedef uint32_t DMA330_PCELL_ID_3_TYPE;
#define DMA330_PCELL_ID_3_3_MASK (0xffUL)
#define DMA330_PCELL_ID_3_3_SHIFT (0UL)




typedef volatile struct COMP_PACKED _DMA330_RDBType {
    DMA330_DSR_TYPE dsr; /* OFFSET: 0x0 */
    DMA330_DPC_TYPE dpc; /* OFFSET: 0x4 */
    DMA330_RESERVED_TYPE rsvd0[24]; /* OFFSET: 0x8 */
    DMA330_INTEN_TYPE inten; /* OFFSET: 0x20 */
    DMA330_INT_EVENT_RIS_TYPE int_event_ris; /* OFFSET: 0x24 */
    DMA330_INTMIS_TYPE intmis; /* OFFSET: 0x28 */
    DMA330_INTCLR_TYPE intclr; /* OFFSET: 0x2c */
    DMA330_FSRD_TYPE fsrd; /* OFFSET: 0x30 */
    DMA330_FSRC_TYPE fsrc; /* OFFSET: 0x34 */
    DMA330_FTRD_TYPE ftrd; /* OFFSET: 0x38 */
    DMA330_RESERVED_TYPE rsvd1[4]; /* OFFSET: 0x3c */
    DMA330_FTR_TYPE ftr0; /* OFFSET: 0x40 */
    DMA330_FTR_TYPE ftr1; /* OFFSET: 0x44 */
    DMA330_FTR_TYPE ftr2; /* OFFSET: 0x48 */
    DMA330_FTR_TYPE ftr3; /* OFFSET: 0x4c */
    DMA330_FTR_TYPE ftr4; /* OFFSET: 0x50 */
    DMA330_FTR_TYPE ftr5; /* OFFSET: 0x54 */
    DMA330_FTR_TYPE ftr6; /* OFFSET: 0x58 */
    DMA330_FTR_TYPE ftr7; /* OFFSET: 0x5c */
    DMA330_RESERVED_TYPE rsvd2[160]; /* OFFSET: 0x60 */
    DMA330_CSR_TYPE csr0; /* OFFSET: 0x100 */
    DMA330_CPC_TYPE cpc0; /* OFFSET: 0x104 */
    DMA330_CSR_TYPE csr1; /* OFFSET: 0x108 */
    DMA330_CPC_TYPE cpc1; /* OFFSET: 0x10c */
    DMA330_CSR_TYPE csr2; /* OFFSET: 0x110 */
    DMA330_CPC_TYPE cpc2; /* OFFSET: 0x114 */
    DMA330_CSR_TYPE csr3; /* OFFSET: 0x118 */
    DMA330_CPC_TYPE cpc3; /* OFFSET: 0x11c */
    DMA330_CSR_TYPE csr4; /* OFFSET: 0x120 */
    DMA330_CPC_TYPE cpc4; /* OFFSET: 0x124 */
    DMA330_CSR_TYPE csr5; /* OFFSET: 0x128 */
    DMA330_CPC_TYPE cpc5; /* OFFSET: 0x12c */
    DMA330_CSR_TYPE csr6; /* OFFSET: 0x130 */
    DMA330_CPC_TYPE cpc6; /* OFFSET: 0x134 */
    DMA330_CSR_TYPE csr7; /* OFFSET: 0x138 */
    DMA330_CPC_TYPE cpc7; /* OFFSET: 0x13c */
    DMA330_RESERVED_TYPE rsvd3[704]; /* OFFSET: 0x140 */
    DMA330_SAR_TYPE sar0; /* OFFSET: 0x400 */
    DMA330_DAR_TYPE dar0; /* OFFSET: 0x404 */
    DMA330_CCR_TYPE ccr0; /* OFFSET: 0x408 */
    DMA330_LC0_TYPE lc0_0; /* OFFSET: 0x40c */
    DMA330_LC1_TYPE lc1_0; /* OFFSET: 0x410 */
    DMA330_RESERVED_TYPE rsvd4[12]; /* OFFSET: 0x414 */
    DMA330_SAR_TYPE sar1; /* OFFSET: 0x420 */
    DMA330_DAR_TYPE dar1; /* OFFSET: 0x424 */
    DMA330_CCR_TYPE ccr1; /* OFFSET: 0x428 */
    DMA330_LC0_TYPE lc0_1; /* OFFSET: 0x42c */
    DMA330_LC1_TYPE lc1_1; /* OFFSET: 0x430 */
    DMA330_RESERVED_TYPE rsvd5[12]; /* OFFSET: 0x434 */
    DMA330_SAR_TYPE sar2; /* OFFSET: 0x440 */
    DMA330_DAR_TYPE dar2; /* OFFSET: 0x444 */
    DMA330_CCR_TYPE ccr2; /* OFFSET: 0x448 */
    DMA330_LC0_TYPE lc0_2; /* OFFSET: 0x44c */
    DMA330_LC1_TYPE lc1_2; /* OFFSET: 0x450 */
    DMA330_RESERVED_TYPE rsvd6[12]; /* OFFSET: 0x454 */
    DMA330_SAR_TYPE sar3; /* OFFSET: 0x460 */
    DMA330_DAR_TYPE dar3; /* OFFSET: 0x464 */
    DMA330_CCR_TYPE ccr3; /* OFFSET: 0x468 */
    DMA330_LC0_TYPE lc0_3; /* OFFSET: 0x46c */
    DMA330_LC1_TYPE lc1_3; /* OFFSET: 0x470 */
    DMA330_RESERVED_TYPE rsvd7[12]; /* OFFSET: 0x474 */
    DMA330_SAR_TYPE sar4; /* OFFSET: 0x480 */
    DMA330_DAR_TYPE dar4; /* OFFSET: 0x484 */
    DMA330_CCR_TYPE ccr4; /* OFFSET: 0x488 */
    DMA330_LC0_TYPE lc0_4; /* OFFSET: 0x48c */
    DMA330_LC1_TYPE lc1_4; /* OFFSET: 0x490 */
    DMA330_RESERVED_TYPE rsvd8[12]; /* OFFSET: 0x494 */
    DMA330_SAR_TYPE sar5; /* OFFSET: 0x4a0 */
    DMA330_DAR_TYPE dar5; /* OFFSET: 0x4a4 */
    DMA330_CCR_TYPE ccr5; /* OFFSET: 0x4a8 */
    DMA330_LC0_TYPE lc0_5; /* OFFSET: 0x4ac */
    DMA330_LC1_TYPE lc1_5; /* OFFSET: 0x4b0 */
    DMA330_RESERVED_TYPE rsvd9[12]; /* OFFSET: 0x4b4 */
    DMA330_SAR_TYPE sar6; /* OFFSET: 0x4c0 */
    DMA330_DAR_TYPE dar6; /* OFFSET: 0x4c4 */
    DMA330_CCR_TYPE ccr6; /* OFFSET: 0x4c8 */
    DMA330_LC0_TYPE lc0_6; /* OFFSET: 0x4cc */
    DMA330_LC1_TYPE lc1_6; /* OFFSET: 0x4d0 */
    DMA330_RESERVED_TYPE rsvd10[12]; /* OFFSET: 0x4d4 */
    DMA330_SAR_TYPE sar7; /* OFFSET: 0x4e0 */
    DMA330_DAR_TYPE dar7; /* OFFSET: 0x4e4 */
    DMA330_CCR_TYPE ccr7; /* OFFSET: 0x4e8 */
    DMA330_LC0_TYPE lc0_7; /* OFFSET: 0x4ec */
    DMA330_LC1_TYPE lc1_7; /* OFFSET: 0x4f0 */
    DMA330_RESERVED_TYPE rsvd11[2060]; /* OFFSET: 0x4f4 */
    DMA330_DBGSTATUS_TYPE dbgstatus; /* OFFSET: 0xd00 */
    DMA330_DBGCMD_TYPE dbgcmd; /* OFFSET: 0xd04 */
    DMA330_DBGINST0_TYPE dbginst0; /* OFFSET: 0xd08 */
    DMA330_DBGINST1_TYPE dbginst1; /* OFFSET: 0xd0c */
    DMA330_RESERVED_TYPE rsvd12[240]; /* OFFSET: 0xd10 */
    DMA330_CR0_TYPE cr0; /* OFFSET: 0xe00 */
    DMA330_CR1_TYPE cr1; /* OFFSET: 0xe04 */
    DMA330_CR2_TYPE cr2; /* OFFSET: 0xe08 */
    DMA330_CR3_TYPE cr3; /* OFFSET: 0xe0c */
    DMA330_CR4_TYPE cr4; /* OFFSET: 0xe10 */
    DMA330_CRD_TYPE crd; /* OFFSET: 0xe14 */
    DMA330_RESERVED_TYPE rsvd13[104]; /* OFFSET: 0xe18 */
    DMA330_WD_TYPE wd; /* OFFSET: 0xe80 */
    DMA330_RESERVED_TYPE rsvd14[348]; /* OFFSET: 0xe84 */
    DMA330_PERIPH_ID_0_TYPE periph_id_0; /* OFFSET: 0xfe0 */
    DMA330_PERIPH_ID_1_TYPE periph_id_1; /* OFFSET: 0xfe4 */
    DMA330_PERIPH_ID_2_TYPE periph_id_2; /* OFFSET: 0xfe8 */
    DMA330_PERIPH_ID_3_TYPE periph_id_3; /* OFFSET: 0xfec */
    DMA330_PCELL_ID_0_TYPE pcell_id_0; /* OFFSET: 0xff0 */
    DMA330_PCELL_ID_1_TYPE pcell_id_1; /* OFFSET: 0xff4 */
    DMA330_PCELL_ID_2_TYPE pcell_id_2; /* OFFSET: 0xff8 */
    DMA330_PCELL_ID_3_TYPE pcell_id_3; /* OFFSET: 0xffc */
} DMA330_RDBType;


#define DMA330_BASE                     (0x4002E000UL)



#define DMA330_MAX_HW_ID                (1UL)

#endif /* DMA330_RDB_H */
