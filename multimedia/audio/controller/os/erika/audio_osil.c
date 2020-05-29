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

/**
    @addtogroup grp_audio_il
    @{

    @file audio_osil.c
    @brief Audio driver Integration
    This source file contains the integration of Audio driver to system.
    @version 0.20 Imported from docx
*/

#include <audio_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"

static void Audio_ReportError(uint32_t aInstanceID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_IIS_ID, (uint8_t)aInstanceID, aApiID,
                    aErr, 4UL, values);
}

#if defined(ENABLE_I2S0_PLAY)
uint8_t COMP_SECTION(".iis.play0buf")
    Audio_Play0Buf[AUDIO_PLAY0_BUF_MAX_NUM][AUDIO_PLAY0_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Play0DMAXfer[AUDIO_PLAY0_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Play0ClientTask, APLY_0_COMP_EVENT)
    @endcode
*/
void I2S0_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Play0ClientTask, APLY_0_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(0UL, BRCM_SWARCH_AUDIO_IL_I2S0_DMA_TX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S0_PLAY */

#if defined(ENABLE_I2S0_REC)
uint8_t COMP_SECTION(".iis.rec0buf")
    Audio_Rec0Buf[AUDIO_REC0_BUF_MAX_NUM][AUDIO_REC0_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Rec0DMAXfer[AUDIO_REC0_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Record0ClientTask, AREC_0_COMP_EVENT)
    @endcode
*/
void I2S0_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Record0ClientTask, AREC_0_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(0UL, BRCM_SWARCH_AUDIO_IL_I2S0_DMA_RX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S0_REC */

#if defined(ENABLE_I2S1_PLAY)
uint8_t COMP_SECTION(".iis.play1buf")
    Audio_Play1Buf[AUDIO_PLAY1_BUF_MAX_NUM][AUDIO_PLAY1_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Play1DMAXfer[AUDIO_PLAY1_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Play1ClientTask, APLY_1_COMP_EVENT)
    @endcode
*/
void I2S1_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Play1ClientTask, APLY_1_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(1UL, BRCM_SWARCH_AUDIO_IL_I2S1_DMA_TX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S1_PLAY */

#if defined(ENABLE_I2S1_REC)
uint8_t COMP_SECTION(".iis.rec1buf")
    Audio_Rec1Buf[AUDIO_REC1_BUF_MAX_NUM][AUDIO_REC1_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Rec1DMAXfer[AUDIO_REC1_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Record1ClientTask, AREC_1_COMP_EVENT)
    @endcode
*/
void I2S1_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Record1ClientTask, AREC_1_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(1UL, BRCM_SWARCH_AUDIO_IL_I2S1_DMA_RX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S1_REC */

#if defined(ENABLE_I2S2_PLAY)
uint8_t COMP_SECTION(".iis.play2buf")
    Audio_Play2Buf[AUDIO_PLAY2_BUF_MAX_NUM][AUDIO_PLAY2_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Play2DMAXfer[AUDIO_PLAY2_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Play2ClientTask, APLY_2_COMP_EVENT)
    @endcode
*/
void I2S2_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_PlayDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Play2ClientTask, APLY_2_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(2UL, BRCM_SWARCH_AUDIO_IL_I2S2_DMA_TX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S2_PLAY */

#if defined(ENABLE_I2S2_REC)
uint8_t COMP_SECTION(".iis.rec2buf")
    Audio_Rec2Buf[AUDIO_REC2_BUF_MAX_NUM][AUDIO_REC2_BUF_SIZE];
DMA_XferReqType COMP_SECTION(".data.drivers")
    Audio_Rec2DMAXfer[AUDIO_REC2_BUF_MAX_NUM];

/**
    @code{.c}
    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus)
    BCM_SetEvent(Audio_Record2ClientTask, AREC_2_COMP_EVENT)
    @endcode
*/
void I2S2_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                        DMA_XferStatusType aStatus)
{
    StatusType status = E_OK;

    Audio_RecDMAXferDoneCb(aID, aReqIdx, aStatus);
    status = SetEvent(Audio_Record2ClientTask, AREC_2_COMP_EVENT);
    if (E_OK != status) {
        Audio_ReportError(2UL, BRCM_SWARCH_AUDIO_IL_I2S2_DMA_RX_DONE_CB_PROC, status,
                0UL, 0UL, 0UL, 0UL);
    }
}
#endif  /* ENABLE_I2S2_REC */


/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aIO is not NULL {
        Build SVC request
        ret = SVC_Request(&sysReqIO)
        if ret is BCM_ERR_OK {
            if  sysReqIO.response is BCM_ERR_OK
                ret = aIO->retVal
            else
                ret = sysReqIO.response
        }
    }

    return ret
    @endcode
*/
int32_t Audio_SysCmdReq(Audio_CmdType aCmd, AudioIO *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_UNKNOWN;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_IIS_ID;
        sysReqIO.magicID = SVC_MAGIC_IIS_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t*)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == sysReqIO.response) {
                retVal = aIO->retVal;
            } else {
                retVal = sysReqIO.response;
            }
        }
    }

    return retVal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion
*/
typedef union _Audio_SVCIOType {
    uint8_t *data;
    AudioIO *io;
} Audio_SVCIOType;

/**
    @code{.c}
    if aSysIO is not NULL {
        if aMagicID is SVC_MAGIC_IIS_ID
             aSysIO->retVal = Audio_CmdHandler(aCmd, aSysIO)
        else
            aSysIO->retVal = BCM_ERR_INVAL_MAGIC
    }
    @endcode
*/
void Audio_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    Audio_SVCIOType audio;
    audio.data = aSysIO;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_IIS_ID) {
            audio.io->retVal = Audio_CmdHandler(aCmd, audio.io);
        } else {
            audio.io->retVal = BCM_ERR_INVAL_MAGIC;
        }
    }
}
/** @} */
