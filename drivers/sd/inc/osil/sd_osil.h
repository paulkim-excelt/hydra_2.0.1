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
#ifndef SD_OSIL_H
#define SD_OSIL_H
#include <system.h>
#include <sd.h>

#define SD_STATE_HANDLER_API_ID           (0x40U)
#define SD_SYSCMD_REQ_API_ID              (0x41U)
#define SD_CMD_HANDLER_API_ID             (0x42U)
#define SD_SYSCMD_HANDLER_API_ID          (0x43U)

typedef uint32_t SD_EventMaskType;
#define SD_EVENT_COMMAND_REQ_MASK              (1UL)
#define SD_EVENT_CMD_COMPLETE_MASK             (2UL)
#define SD_EVENT_DATA_COMPLETE_MASK            (4UL)
#define SD_EVENT_IO_CARD_INTR_MASK             (8UL)

typedef uint32_t SD_SysCmdType;

typedef struct _SDMMCIO {
    int32_t                 retVal;     /**< Return value specific to the cmd */
    SD_SlotIDType           hwID;
    const SD_ConfigType     *config;
    SD_ClientIDType         clientID;
    SD_CardStateType        *mmcCardState;
    const SD_CmdType        *mmcCmd;
    SD_DataType             *mmcData;
    uint32_t                *cmdResp;
    uint32_t                startAddr;
    uint32_t                numBlocks;
    uint8_t                 *clientBuf;
    SD_IO_RW_OpType         ioOpType;
    uint32_t                regAddr;
    uint32_t                func;
    uint32_t                autoIncr;
    uint32_t                blkSz;
    uint32_t                blkCnt;
    uint32_t                opStatus;
    SD_OpResultType         opResult;
    SD_XferWidthType        width;
    uint32_t                cardIntr;
    uint32_t                suspendBusIntr;
} SDMMCIO;

extern void SD_EventHandler(SD_SlotIDType aID, SD_EventMaskType aEventMask);
extern int32_t SD_SysCmdReq(SD_SysCmdType aCmd,
        SDMMCIO *const aIO);
extern int32_t SD_CmdHandler(SD_SysCmdType aCmd,
        SDMMCIO *const aIO);
extern void SD_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd,
        uint8_t *aSysIO);
#endif /* SD_OSIL_H */
