/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
#include <stddef.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <system.h>
#include <dma.h>
#include <dma_osil.h>
#include "dma330.h"


static void DMA_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_DMA_ID & BCM_LOGMASK_USER), DMA_INSTANCE_ID, aApiID, aErr,
            4UL, values);
}

void DMA_Init(void)
{
    int32_t retVal;
    DMAIO dmaIO;

    dmaIO.retVal = BCM_ERR_UNKNOWN;
    retVal = DMA_SysCmdReq(DMA_CMD_INIT, &dmaIO);
    if (retVal != BCM_ERR_OK) {
        DMA_ReportError(DMA_INIT_API_ID, retVal, 0UL, 0UL,
                0UL, 0UL);
    }
}
int32_t DMA_Mem2MemXfer(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ)
{
    int32_t retVal;
    DMAIO dmaIO;
    dmaIO.channelID = aID;
    dmaIO.xferReq = aXferReq;
    dmaIO.aReqSZ = aReqSZ;
    retVal = DMA_SysCmdReq(DMA_CMD_XFER_REQ, &dmaIO);
    return retVal;
}

int32_t DMA_GetXferStatus(DMA_ChannelIDType aID, DMA_XferStatusType *aStatus)
{
    int32_t retVal;
    DMAIO dmaIO;
    dmaIO.channelID = aID;
    dmaIO.xferStatus = aStatus;
    retVal = DMA_SysCmdReq(DMA_CMD_GET_XFER_STATUS, &dmaIO);
    return retVal;
}

int32_t DMA_Mem2MemXferSetup(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ)
{
    return DMA330_XferSetup(aID, aXferReq, aReqSZ);
}

int32_t DMA_Mem2MemXferTrigger(DMA_ChannelIDType aID)
{
    return DMA330_XferTrigger(aID, 0UL);
}

int32_t DMA_PeriphXferSetup(DMA_ChannelIDType aID,
        DMA_XferReqType aXferReq[], uint32_t aReqSZ)
{
    return DMA330_XferSetup(aID, aXferReq, aReqSZ);
}

int32_t DMA_PeriphXferTrigger(DMA_ChannelIDType aID, uint32_t aReqIdx)
{
    return DMA330_XferTrigger(aID, aReqIdx);
}

int32_t DMA_PeriphFlush(DMA_ChannelIDType aID)
{
    return DMA330_PeriphFlush(aID);
}

int32_t DMA_CmdHandler(DMA_CmdType aCmd, DMAIO *const aIO)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    if (aIO != NULL) {
        switch (aCmd) {
        case DMA_CMD_INIT:
            retVal = DMA330_Init();
            break;
        case DMA_CMD_XFER_REQ:
            retVal = DMA330_XferSetup(aIO->channelID, aIO->xferReq,
                    aIO->aReqSZ);
            if (BCM_ERR_OK == retVal) {
                retVal = DMA330_XferTrigger(aIO->channelID, 0UL);
            }
            break;
        case DMA_CMD_GET_XFER_STATUS:
            retVal = DMA330_GetTransferStatus(aIO->channelID,
                    aIO->xferStatus);
            break;
        default:
            break;
        }
    }
    return retVal;
}
