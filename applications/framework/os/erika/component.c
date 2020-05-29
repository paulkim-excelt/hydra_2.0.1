/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
/******************************************************************************
File Name  : component.c
Description:

 ******************************************************************************/
#define GetModuleLogLevel() ULOG_LVL_ERROR

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bcm_err.h"
#include "ulog.h"

#if defined(ENABLE_CAM)
#include "camera.h"
#endif  /* ENABLE_CAM */

#if defined(ENABLE_SGFX_ICLUSTER)
#include "sgfx_icluster_comp.h"
#endif /* ENABLE_SGFX_ICLUSTER*/

#if defined(ENABLE_OPENVX)
#include "openvx.h"
#endif  /* ENABLE_OPENVX*/

#if defined(ENABLE_VDUP)
#include "video_dup.h"
#endif  /* ENABLE_VDUP*/

#if defined(ENABLE_STITCH) || defined(ENABLE_SCALE_PLACE)
#include "stitch.h"
#endif  /* ENABLE_STITCH */

#if defined(ENABLE_LDC)
#include "ldc.h"
#endif  /* ENABLE_LDC */

#if defined(ENABLE_PPR)
#include "ppr.h"
#endif  /* ENABLE_PPR */

#if defined(ENABLE_VENC)
#include "venc.h"
#endif  /* ENABLE_VENC */

#if defined(ENABLE_VDEC)
#include "vdec.h"
#endif  /* ENABLE_VDEC */

#if defined(ENABLE_AVB_TALKER)
#include "avb_talker.h"
#endif  /* ENABLE_AVB_TALKER */

#if defined(ENABLE_RTP_SENDER)
#include "rtp_sender.h"
#endif  /* ENABLE_RTP_SENDER */

#if defined(ENABLE_RTP_RECEIVER)
#include "rtp_receiver.h"
#endif  /* ENABLE_RTP_RECEIVER */

#if defined(ENABLE_AVB_LISTENER)
#include "avb_listener.h"
#endif  /* ENABLE_AVB_LISTENER */

#if defined(ENABLE_AVCE)
#include "avce.h"
#endif  /* ENABLE_AVCE */

#if defined(ENABLE_AVCD)
#include "avcd.h"
#endif  /* ENABLE_AVCD */

#if defined(ENABLE_VGCC)
#include "vgc_comp.h"
#endif /* ENABLE_VGCC */

#if defined(ENABLE_PALLET_GRAPHICS)
#include "pgx_comp.h"
#endif /* ENABLE_PALLET_GRAPHICS */

#if defined(ENABLE_GRAPHICS_SOURCE)
#include "gfxsrc_comp.h"
#endif /* ENABLE_GRAPHICS_SOURCE */

#if defined(ENABLE_AUDIO_RECORD)
#include "audio_record.h"
static const MSYS_CompFnTblType AudioRecord_CompFnTable =
{
    AudioRecord_SendCmd,
    AudioRecord_RecvResp,
    AudioRecord_GetMediaOutPort,
    NULL,
};

static const MSYS_CompFnTblType* AudioRecord_GetCompFnTable(void)
{
    return &AudioRecord_CompFnTable;
}
#endif /* ENABLE_AUDIO_RECORD */

#if defined(ENABLE_AUDIO_PLAY)
#include "audio_play.h"
static const MSYS_CompFnTblType AudioPlay_CompFnTable =
{
    AudioPlay_SendCmd,
    AudioPlay_RecvResp,
    AudioPlay_GetMediaOutPort,
    NULL,
};

static const MSYS_CompFnTblType* AudioPlay_GetCompFnTable(void)
{
    return &AudioPlay_CompFnTable;
}
#endif /* ENABLE_AUDIO_PLAY */


#if defined(ENABLE_FMWK_EX_SRC_COMP)
#include "ex_src_comp.h"
/* Component function table for AVC Decoder */
static const MSYS_CompFnTblType ExSrc_CompFnTable =
{
    ExSrc_SendCmd,
    ExSrc_RecvResp,
    ExSrc_GetOutPort,
    NULL,
};

static const MSYS_CompFnTblType* ExSrc_GetCompFnTable(void)
{
    return &ExSrc_CompFnTable;
};
#endif  /* ENABLE_FMWK_EX_SRC_COMP */

#if defined(ENABLE_FMWK_EX_INT_COMP)
#include "ex_int_comp.h"
/* Component function table for AVC Decoder */
static const MSYS_CompFnTblType ExInt_CompFnTable =
{
    ExInt_SendCmd,
    ExInt_RecvResp,
    ExInt_GetOutPort,
    NULL,
};

static const MSYS_CompFnTblType* ExInt_GetCompFnTable(void)
{
    return &ExInt_CompFnTable;
};
#endif  /* ENABLE_FMWK_EX_INT_COMP */

#if defined(ENABLE_FMWK_EX_SNK_COMP)
#include "ex_sink_comp.h"
/* Component function table for AVC Decoder */
static const MSYS_CompFnTblType ExSnk_CompFnTable =
{
    ExSnk_SendCmd,
    ExSnk_RecvResp,
    ExSnk_GetOutPort,
#if defined(ENABLE_EX_SNK_IN_PORT)
    ExSnk_GetInPort,
#else
    NULL,
#endif
};

static const MSYS_CompFnTblType* ExSnk_GetCompFnTable(void)
{
    return &ExSnk_CompFnTable;
};
#endif  /* ENABLE_FMWK_EX_SNK_COMP */


const MSYS_CompFnTblType* MSYS_GetCompFnTbl(MSYS_CompIDType aCompIDName)
{
    const MSYS_CompFnTblType *retVal = NULL;
    switch(aCompIDName)
    {
#if defined(ENABLE_CAM)
    case MSYS_COMP_ID_CAM0:
        retVal = CAMERA_GetCompFnTable();
        break;
#endif  /* ENABLE_CAM */
#if defined(ENABLE_OPENVX)
    case MSYS_COMP_ID_OVX0:
        retVal = OVX0_GetCompFnTable();
        break;
#endif /* ENABLE_OPENVX*/
#if defined(ENABLE_VDUP)
    case MSYS_COMP_ID_VDUP:
        retVal = VDUP_GetCompFnTable();
        break;
#endif /* ENABLE_VDUP*/
#if defined(ENABLE_STITCH) || defined(ENABLE_SCALE_PLACE)
    case MSYS_COMP_ID_STCH:
        retVal = STCH_GetCompFnTable();
        break;
#endif /* ENABLE_STITCH */
#if defined(ENABLE_LDC)
    case MSYS_COMP_ID_LDC:
        retVal = LDC_GetCompFnTable();
        break;
#endif /* ENABLE_LDC */
#if defined(ENABLE_PPR)
    case MSYS_COMP_ID_PPR:
        retVal = PPR_GetCompFnTable();
        break;
#endif /* ENABLE_PPR */
#if defined(ENABLE_VENC)
    case MSYS_COMP_ID_VENC:
        retVal = VENC_GetCompFnTable();
        break;
#endif /* ENABLE_VENC */
#if defined(ENABLE_VDEC)
    case MSYS_COMP_ID_VDEC:
        retVal = VDEC_GetCompFnTable();
        break;
#endif /* ENABLE_VDEC */
#if defined(ENABLE_AVB_TALKER)
    case MSYS_COMP_ID_AVBT:
        retVal = AVBT_GetCompFnTable();
        break;
#endif  /* ENABLE_AVB_TALKER */
#if defined(ENABLE_AVB_LISTENER)
    case MSYS_COMP_ID_AVBL:
        retVal = AVBL_GetCompFnTable();
        break;
#endif  /* ENABLE_AVB_LISTENER */
#if defined(ENABLE_RTP_SENDER)
    case MSYS_COMP_ID_RTPS:
        retVal = RTPS_GetCompFnTable();
        break;
#endif  /* ENABLE_AVB_LISTENER */
#if defined(ENABLE_RTP_RECEIVER)
    case MSYS_COMP_ID_RTPR:
        retVal = RTPR_GetCompFnTable();
        break;
#endif  /* ENABLE_RTP_RECEIVER */
#if defined(ENABLE_AVCE)
    case MSYS_COMP_ID_AVCE:
        retVal = AVCE_GetCompFnTable();
        break;
#endif  /* ENABLE_AVCE */
#if defined(ENABLE_AVCD)
    case MSYS_COMP_ID_AVCD:
        retVal = AVCD_GetCompFnTable();
        break;
#endif  /* ENABLE_AVCD */
#if defined(ENABLE_VGCC)
    case MSYS_COMP_ID_VGCC:
        retVal = VGCC_GetCompFnTable();
        break;
#endif /* ENABLE_VGCC */
#if defined(ENABLE_PALLET_GRAPHICS)
    case MSYS_COMP_ID_PGX:
        retVal = Pgx_GetCompFnTable();
        break;
#endif /* ENABLE_PALLET_GRAPHICS */
#if defined(ENABLE_GRAPHICS_SOURCE)
    case MSYS_COMP_ID_GFXSRC:
        retVal = GfxSrc_GetCompFnTable();
        break;
#endif /* ENABLE_GRAPHICS_SOURCE */
#if defined(ENABLE_SGFX_ICLUSTER)
    case MSYS_COMP_ID_SGFXIC:
        retVal = SGfxICluster_GetCompFnTable();
        break;
#endif /* ENABLE_SGFX_ICLUSTER */
#if defined(ENABLE_AUDIO_RECORD)
    case MSYS_COMP_ID_AREC:
        retVal = AudioRecord_GetCompFnTable();
        break;
#endif /* ENABLE_AUDIO_RECORD */
#if defined(ENABLE_AUDIO_PLAY)
    case MSYS_COMP_ID_APLY:
        retVal = AudioPlay_GetCompFnTable();
        break;
#endif /* ENABLE_AUDIO_PLAY */
#if defined(ENABLE_FMWK_EX_SRC_COMP)
    case MSYS_COMP_ID_EX_SRC:
        retVal = ExSrc_GetCompFnTable();
        break;
#endif  /* ENABLE_FMWK_EX_SRC_COMP */
#if defined(ENABLE_FMWK_EX_INT_COMP)
    case MSYS_COMP_ID_EX_INT:
        retVal = ExInt_GetCompFnTable();
        break;
#endif  /* ENABLE_FMWK_EX_INT_COMP */
#if defined(ENABLE_FMWK_EX_SNK_COMP)
    case MSYS_COMP_ID_EX_SNK:
        retVal = ExSnk_GetCompFnTable();
        break;
#endif  /* ENABLE_FMWK_EX_SNK_COMP */
    default:
        ULOG_ERR("GetComponentFnTable: Unknown component magic ID: %llx\n",
                 aCompIDName);
        break;
    }
    return retVal;
}
