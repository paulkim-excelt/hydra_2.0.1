/*****************************************************************************
  Copyright 2016-2017 Broadcom Limited.  All rights reserved.

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
#ifndef DMA330_H
#define DMA330_H

#include <dma.h>

typedef volatile struct {
    uint32_t    DSR;
    uint32_t    DPC;
    uint32_t    RSVD1[6];
    uint32_t    INTEN;
    uint32_t    INT_EVENT_RIS;
    uint32_t    INTMIS;
    uint32_t    INTCLR;
    uint32_t    FSRD;
    uint32_t    FSRC;
    uint32_t    FTRD;
    uint32_t    RSVD2[1];
    uint32_t    FTR0;
    uint32_t    FTR1;
    uint32_t    FTR2;
    uint32_t    FTR3;
    uint32_t    FTR4;
    uint32_t    FTR5;
    uint32_t    FTR6;
    uint32_t    FTR7;
    uint32_t    RSVD3[40];
    uint32_t    CSR_CPC[16];
#define CSR_CHANNEL_STATUS_MASK     (0xFFUL)
#define CHANNEL_STATE_STOPPED       (0x0UL)
#define CHANNEL_STATE_EXECUTING     (0x1UL)
#define CHANNEL_STATE_CACHE_MISS    (0x2UL)
#define CHANNEL_STATE_UPDATING_PC   (0x3UL)
#define CHANNEL_STATE_WFE           (0x4UL)
#define CHANNEL_STATE_AT_BARRIER    (0x5UL)
#define CHANNEL_STATE_WFP           (0x7UL)
#define CHANNEL_STATE_KILLING       (0x8UL)
#define CHANNEL_STATE_COMPL         (0x9UL)
#define CHANNEL_STATE_FAULT_COMPL   (0xEUL)
#define CHANNEL_STATE_FAULTING      (0xFUL)

    uint32_t    RSVD4[176];
    uint32_t    SAR0;
    uint32_t    DAR0;
    uint32_t    CCR0;
    uint32_t    LC0_0;
    uint32_t    LC1_0;
    uint32_t    RSVD5[3];
    uint32_t    SAR1;
    uint32_t    DAR1;
    uint32_t    CCR1;
    uint32_t    LC0_1;
    uint32_t    LC1_1;
    uint32_t    RSVD6[3];
    uint32_t    SAR2;
    uint32_t    DAR2;
    uint32_t    CCR2;
    uint32_t    LC0_2;
    uint32_t    LC1_2;
    uint32_t    RSVD7[3];
    uint32_t    SAR3;
    uint32_t    DAR3;
    uint32_t    CCR3;
    uint32_t    LC0_3;
    uint32_t    LC1_3;
    uint32_t    RSVD8[3];
    uint32_t    SAR4;
    uint32_t    DAR4;
    uint32_t    CCR4;
    uint32_t    LC0_4;
    uint32_t    LC1_4;
    uint32_t    RSVD9[3];
    uint32_t    SAR5;
    uint32_t    DAR5;
    uint32_t    CCR5;
    uint32_t    LC0_5;
    uint32_t    LC1_5;
    uint32_t    RSVD10[3];
    uint32_t    SAR6;
    uint32_t    DAR6;
    uint32_t    CCR6;
    uint32_t    LC0_6;
    uint32_t    LC1_6;
    uint32_t    RSVD11[3];
    uint32_t    SAR7;
    uint32_t    DAR7;
    uint32_t    CCR7;
    uint32_t    LC0_7;
    uint32_t    LC1_7;
    uint32_t    RSVD12[515];
    uint32_t    DBGSTATUS;
    uint32_t    DBGCMD;
    uint32_t    DBGINST0;
    uint32_t    DBGINST1;
    uint32_t    RSVD13[60];
    uint32_t    CR0;
    uint32_t    CR1;
    uint32_t    CR2;
    uint32_t    CR3;
    uint32_t    CR4;
    uint32_t    CRD;
    uint32_t    RSVD14[26];
    uint32_t    WD;
    uint32_t    RSVD15[87];
    uint32_t    periph_id_0;
    uint32_t    periph_id_1;
    uint32_t    periph_id_2;
    uint32_t    periph_id_3;
    uint32_t    pcell_id_0;
    uint32_t    pcell_id_1;
    uint32_t    pcell_id_2;
    uint32_t    pcell_id_3;
} DMA330_RegsType;

extern int32_t DMA330_Init(void);
extern int32_t DMA330_XferSetup(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ);
extern int32_t DMA330_GetTransferStatus(DMA_ChannelIDType aID,
        DMA_XferStatusType *const aStatus);
extern int32_t DMA330_XferTrigger(DMA_ChannelIDType aID, uint32_t aReqIdx);
extern int32_t DMA330_PeriphFlush(DMA_ChannelIDType aID);
#endif /* DMA330_H */
