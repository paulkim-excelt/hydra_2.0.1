/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
#ifndef SDHC_H
#define SDHC_H

#include <stdint.h>
#include <compiler.h>
#include <sd.h>

typedef uint32_t SDHC_CardStateType;
#define SDHC_CARD_STATE_REMOVED             (0UL)
#define SDHC_CARD_STATE_INSERTED            (1UL)

typedef uint32_t SDHC_BusPowerStateType;
#define SDHC_BUS_PWR_STATE_OFF            (0x0UL)
#define SDHC_BUS_PWR_STATE_ON             (0x1UL)

/* initialize the Host controller */
extern int32_t SDHC_Init(SD_SlotIDType aID, SD_ClkType aClkFreq,
        SD_BusVoltageType aDetectVolt);

extern int32_t SDHC_GetCardState(SD_SlotIDType aID,
        SDHC_CardStateType *const aState);

extern int32_t SDHC_SendCmd(SD_SlotIDType aID, SD_CmdType const *aCmd,
        const SD_DataType *const aData);

extern int32_t SDHC_GetCmdStatus(SD_SlotIDType aID,
        SD_CmdType *aCmd,
        SD_RespDataType aData);

extern int32_t SDHC_SetBusWidth(SD_SlotIDType aID,
        SD_XferWidthType aWidth);

extern int32_t SDHC_SetBusPowerState(SD_SlotIDType aID,
        SDHC_BusPowerStateType aState,
        SD_BusVoltageType aVolt);

extern int32_t SDHC_SetDataTimeout(SD_SlotIDType aID,
        uint32_t aTimeout);

extern int32_t SDHC_is1V8SignallingSupported(SD_SlotIDType aID,
        uint32_t *const aSupported);

int32_t SDHC_SetSDClkFreq(SD_SlotIDType aID, uint32_t aSDFreq);

extern int32_t SDHC_EnableCardInterrupt(SD_SlotIDType aID);
extern int32_t SDHC_DisableCardInterrupt(SD_SlotIDType aID);

extern void SDHC_MaskIntr(SD_SlotIDType aID);
extern void SDHC_UnMaskIntr(SD_SlotIDType aID);
#endif /* SDHC_H */
