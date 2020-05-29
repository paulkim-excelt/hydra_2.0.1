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

#include <string.h>
#include <bcm_err.h>
#include <spi.h>
#include <spi_osil.h>
#include "spi_drv.h"

static void SPI_ReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
                            uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
                            uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_SPI_ID & BCM_LOGMASK_USER), aInstanceID, aApiID, aErr, 4UL,
            values);
}

void SPI_IRQHandler(SPI_HwIDType aHwID)
{
    SPIDrv_IRQHandler(aHwID);
}

void SPI_IRQEventHandler(SPI_HwIDType aHwID)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    spiIO.hwID = aHwID;
    retVal = SPI_SysCmdReq(SPI_CMD_IRQ_EVENT_HANDLE, &spiIO);
    if (BCM_ERR_OK != retVal) {
        SPI_ReportError(aHwID, BRCM_ARCH_SPI_IL_IRQ_EVENT_HANDLER_PROC, retVal,
                (uint32_t)spiIO.retVal, 0UL, 0UL, 0UL);
    }
}

void SPI_Cancel(SPI_SeqIDType aSeqID)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    spiIO.seqID = aSeqID;
    retVal = SPI_SysCmdReq(SPI_CMD_CANCEL, &spiIO);
    if (BCM_ERR_OK != retVal) {
        SPI_ReportError(0U, BRCM_ARCH_SPI_CANCEL_PROC, retVal, (uint32_t)spiIO.retVal,
                (uint32_t)aSeqID, 0UL, 0UL);
    }
}

SPI_StateType SPI_GetHWState(SPI_HwIDType aHwID)
{
    return SPIDrv_GetHWState(aHwID);
}

int32_t SPI_SyncXfer(SPI_SeqIDType aSeqID)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    spiIO.seqID = aSeqID;
    retVal = SPI_SysCmdReq(SPI_CMD_SYNC_XFER, &spiIO);

    return retVal;
}

SPI_SeqStatusType SPI_GetSeqStatus(SPI_SeqIDType aSeqID)
{
    return SPIDrv_GetSeqStatus(aSeqID);
}

SPI_XferPktStatusType SPI_GetXferPktStatus(SPI_XferPktIDType aXferPktID)
{
    return SPIDrv_GetXferPktStatus(aXferPktID);
}

SPI_StateType SPI_GetDrvState(void)
{
    return SPIDrv_GetDrvState();
}

int32_t SPI_SetupBuf(SPI_DataPktIDType aDataPktID,
                     const SPI_DataBufType *const aSrc,
                     SPI_DataBufType *const aDest,
                     SPI_DataSizeType aLen)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    spiIO.dataPktID = aDataPktID;
    spiIO.src = aSrc;
    spiIO.dest = aDest;
    spiIO.xferSize = aLen;
    retVal = SPI_SysCmdReq(SPI_CMD_SETUP_BUF, &spiIO);

    return retVal;
}

int32_t SPI_AsyncXfer(SPI_SeqIDType aSeqID)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    spiIO.seqID = aSeqID;
    retVal = SPI_SysCmdReq(SPI_CMD_ASYNC_XFER, &spiIO);

    return retVal;
}

int32_t SPI_DeInit(void)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    retVal = SPI_SysCmdReq(SPI_CMD_DEINIT, &spiIO);

    return retVal;
}

void SPI_Init(void)
{
    int32_t retVal;
    SPIIO spiIO;

    spiIO.retVal = BCM_ERR_UNKNOWN;
    retVal = SPI_SysCmdReq(SPI_CMD_INIT, &spiIO);
    if (BCM_ERR_OK != retVal) {
        SPI_ReportError(0U, BRCM_ARCH_SPI_INIT_PROC, retVal, (uint32_t)spiIO.retVal,
                0UL, 0UL, 0UL);
    }
}

int32_t SPI_CmdHandler(SPI_CmdType aCmd, SPIIO *const aIO)
{
    int32_t retVal;

    if (NULL != aIO) {
        switch (aCmd) {
        case SPI_CMD_INIT:
            retVal = SPIDrv_Init();
            break;
        case SPI_CMD_DEINIT:
            retVal = SPIDrv_DeInit();
            break;
        case SPI_CMD_ASYNC_XFER:
            retVal = SPIDrv_AsyncXfer(aIO->seqID);
            break;
        case SPI_CMD_SETUP_BUF:
            retVal = SPIDrv_SetupBuf(aIO->dataPktID, aIO->src, aIO->dest,
                    aIO->xferSize);
            break;
        case SPI_CMD_SYNC_XFER:
            retVal = SPIDrv_SyncXfer(aIO->seqID);
            break;
        case SPI_CMD_CANCEL:
            retVal = SPIDrv_Cancel(aIO->seqID);
            break;
        case SPI_CMD_IRQ_EVENT_HANDLE:
            SPIDrv_IRQEventHandler(aIO->hwID);
            retVal = BCM_ERR_OK;
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
        }
    } else {
        retVal = BCM_ERR_UNKNOWN;
    }

    return retVal;
}
