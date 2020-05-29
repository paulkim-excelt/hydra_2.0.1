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
/**
    @defgroup grp_vgcd_service_impl Implementation
    @ingroup grp_vgcd_service

    @addtogroup grp_vgcd_service_impl
    @{
    @section sec_vgcd_overview VGCD Overview

    @file vgc_display.c
    @brief VGC Display Driver Implementation file
    This source file contains the implementation of VGC Display Driver

    @version todo
*/

#include <stdint.h>
#include "ee.h"
#include <msg_queue.h>
#include <system.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>
#include <vgc_display_service.h>

/**
     @name VGCD Design IDs
     @{
     @brief Design IDs for  VGCD
*/
#define BRCM_SWDSGN_VGCD_VGCC_EVENT_MACRO             (0x80U)  /**< @brief #VGCD_VGCC_EVENT */
#define BRCM_SWDSGN_VGCD_CTRLMSG_GLOBAL               (0x81U)  /**< @brief #VGCD_CtrlMsg */
#define BRCM_SWDSGN_VGCD_CTRLMSG_HDRQ_GLOBAL          (0x82U)  /**< @brief #VGCD_CtrlMsgHdrQ */
#define BRCM_SWDSGN_VGCD_CTRLMSGQ_GLOBAL              (0x83U)  /**< @brief #VGCD_CtrlMsgQ */
#define BRCM_SWDSGN_VGCD_CTRLCMD_TYPE                 (0x84U)  /**< @brief #VGCD_CtrlCmdType */
#define BRCM_SWDSGN_VGCD_CTRLMSG_TYPE                 (0x85U)  /**< @brief #VGCD_CtrlMsgType */
#define BRCM_SWDSGN_VGCD_INIT_PROC                    (0x86U)  /**< @brief #VGCD_Init */
#define BRCM_SWDSGN_VGCD_DEINIT_PROC                  (0x87U)  /**< @brief #VGCD_Deinit */
#define BRCM_SWDSGN_VGCD_ENABLE_PROC                  (0x88U)  /**< @brief #VGCD_Enable */
#define BRCM_SWDSGN_VGCD_DISABLE_PROC                 (0x89U)  /**< @brief #VGCD_Disable */
#define BRCM_SWDSGN_VGCD_GETCTRL_RESPONSE_PROC        (0x8AU)  /**< @brief #VGCD_GetCtrlResponse */
#define BRCM_SWDSGN_VGCD_BRIGHTNESS_PROC              (0x8BU)  /**< @brief #VGCD_SetBrightness */
#define BRCM_SWDSGN_VGCD_BACKGROUND_PROC              (0x8CU)  /**< @brief #VGCD_SetBackgroundColor */
#define BRCM_SWDSGN_VGCD_GETSTATS_PROC                (0x8DU)  /**< @brief #VGCD_GetStats */
#define BRCM_SWDSGN_VGCD_CSC_PROC                     (0x8EU)  /**< @brief #VGCD_CSC */
#define BRCM_SWDSGN_VGCD_COLORKEY_PROC                (0x8FU)  /**< @brief #VGCD_ColorKey */
#define BRCM_SWDSGN_VGCD_PROCESS_DEINIT_PROC          (0x90U)  /**< @brief #VGCD_ProcessDeInit */
#define BRCM_SWDSGN_VGCD_CTRL_GETMSG_PROC             (0x91U)  /**< @brief #VGCD_CtrlGetMsg */
#define BRCM_SWDSGN_VGCD_CTRL_PROCESSMSG_PROC         (0x92U)  /**< @brief #VGCD_CtrlProcessMsg */
#define BRCM_SWDSGN_VGCD_UERR_PROC                    (0x93U)  /**< @brief #VGCD_DIS_UErr */

/** @} */

/**
    @brief VGC Service Control Command

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
typedef uint32_t VGCD_CtrlCmdType;
#define VGCD_CTRLCMD_INIT       (0x1UL)
#define VGCD_CTRLCMD_DEINIT     (0x2UL)
#define VGCD_CTRLCMD_ENABLE     (0x3UL)
#define VGCD_CTRLCMD_DISABLE    (0x4UL)
#define VGCD_CTRLCMD_BRIGHTNESS (0x5UL)
#define VGCD_CTRLCMD_BACKGROUND (0x6UL)
#define VGCD_CTRLCMD_GETSTATS   (0x7UL)
#define VGCD_CTRLCMD_CSC        (0x8UL)
#define VGCD_CTRLCMD_COLORKEY   (0x9UL)

/**
    @brief  Structure to define a control message to Display Service

    @trace #BRCM_SWARCH_VGCD_INIT_PROC
    @trace #BRCM_SWARCH_VGCD_DEINIT_PROC
    @trace #BRCM_SWARCH_VGCD_ENABLE_PROC
    @trace #BRCM_SWARCH_VGCD_DISABLE_PROC
    @trace #BRCM_SWARCH_VGCD_GETCTRL_RESPONSE_PROC
    @trace #BRCM_SWARCH_VGCD_BRIGHTNESS_PROC
    @trace #BRCM_SWARCH_VGCD_BACKGROUND_PROC
    @trace #BRCM_SWARCH_VGCD_GETSTATS_PROC
    @trace #BRCM_SWARCH_VGCD_CSC_PROC
    @trace #BRCM_SWARCH_VGCD_COLORKEY_PROC
    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
typedef struct _VGCD_CtrlMsgType {      /**< struct #VGCD_CtrlMsgType */
    VGCD_CtrlCmdType cmd;               /**< Control command */
    uint32_t enable;                    /**< Enable/Disable */
    const VGC_DispCfgType *const cfg;   /**< Pointer to input configuration */
    uint32_t percentBrightness;         /**< brightness percentage */
    const VGC_PixelType *const pixel;   /**< Pointer to pixel information */
    VGC_DispStatsType *const stats;     /**< Pointer to statistics information */
    int32_t result;                     /**< Result of the command */
    const VGC_CSCType *const csc;       /**< CSC co-efficients */
    const VGC_ColorKeyType *const colorKey;    /**< Color Keying information */
    const uint32_t *const palette;      /**< Palette information */
    const uint32_t size;                /**< Size */
} VGCD_CtrlMsgType;

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCD_INIT_PROC
    @trace #BRCM_SWARCH_VGCD_DEINIT_PROC
    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
static void VGCD_DIS_UErr(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                      uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_DIS_ID & BCM_LOGMASK_USER), 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief VGC Service Component Event

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
#define VGCD_VGCC_EVENT        SystemEvent1        /**< @breif same as VGCC_VGCD_EVENT Event */

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
static VGCD_CtrlMsgType VGCD_CtrlMsg[MSGQ_SIZE] COMP_SECTION(".bss.display");

/**
    @brief VGC Service Control Message

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
static void *VGCD_CtrlGetMsg(uint32_t aIdx)
{
    void *dispHwMsg = NULL;
    if (MSGQ_SIZE > aIdx) {
        dispHwMsg = (void *)(&VGCD_CtrlMsg[aIdx]);
    }
    return dispHwMsg;
}

/**
    @brief Initialize MSGQ

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
MSGQ_DEFINE_V2(VGCD_CtrlMsgQ, VGCD_TASK_ID, VGCD_VGCC_EVENT,
            MSGQ_CLIENT_SIGNAL_ENABLE, VGCD_CtrlMsgType, VGCD_CtrlMsgHdrQ,
            VGCD_CtrlGetMsg, ".data.display");

/**
    @brief VGC Service Initialization

    @trace #BRCM_SWARCH_VGCD_INIT_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_Init(const VGC_DispCfgType *const aCfg,
                  const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = aCfg,
        .pixel = NULL,
        .stats = NULL,
        .cmd = VGCD_CTRLCMD_INIT,
        .result = BCM_ERR_UNKNOWN,
    };
    int32_t status;
    int32_t ret;

    status = BCM_ActivateTask(VGCD_TASK_ID);
    if (BCM_ERR_OK == status) {
        ret = MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
    } else {
        ret = BCM_ERR_UNKNOWN;
        VGCD_DIS_UErr(BRCM_SWARCH_VGCD_INIT_PROC, ret, (uint32_t)__LINE__, (uint32_t)status, 0UL, 0UL);
    }
    return ret;
}

/**
    @brief VGC Deinit Process

    @trace #BRCM_SWARCH_VGCD_DEINIT_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_Deinit(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .cmd = VGCD_CTRLCMD_DEINIT,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief Process VGC Enable

    @trace #BRCM_SWARCH_VGCD_ENABLE_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_Enable(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .cmd = VGCD_CTRLCMD_ENABLE,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief Process VGC Disable

    @trace #BRCM_SWARCH_VGCD_DISABLE_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_Disable(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .cmd = VGCD_CTRLCMD_DISABLE,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief VGCD Process Get Control Response

    @trace #BRCM_SWARCH_VGCD_GETCTRL_RESPONSE_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_GetCtrlResponse(const MSGQ_MsgHdrType* const aHdr)
{
    int32_t ret;
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
    };

    ret = MSGQ_RecvMsg(&VGCD_CtrlMsgQ, aHdr, (void *)&dispHwMsg);
    if (BCM_ERR_OK == ret) {
        ret = dispHwMsg.result;
    }
    return ret;
}

/**
    @brief VGCD Process Set Brightness

    @trace #BRCM_SWARCH_VGCD_BRIGHTNESS_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_SetBrightness(uint32_t aPercentBrightness,
                              const uint32_t aClientMask,
                              const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .percentBrightness = aPercentBrightness,
        .cmd = VGCD_CTRLCMD_BRIGHTNESS,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief VGCD Process Set Background Color

    @trace #BRCM_SWARCH_VGCD_BACKGROUND_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_SetBackgroundColor(const VGC_PixelType *const aPixel,
                                   const uint32_t aClientMask,
                                   const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = aPixel,
        .stats = NULL,
        .cmd = VGCD_CTRLCMD_BACKGROUND,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief VGC Service Get Stats

    @trace #BRCM_SWARCH_VGCD_GETSTATS_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_GetStats(VGC_DispStatsType *const aStats,
                         const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = aStats,
        .cmd = VGCD_CTRLCMD_GETSTATS,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCD_CSC_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_CSC(uint32_t aEnable, const VGC_CSCType *const aCsc,
                    const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .enable = aEnable,
        .csc = aCsc,
        .cmd = VGCD_CTRLCMD_CSC,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCD_COLORKEY_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_ColorKey(uint32_t aEnable, const VGC_ColorKeyType *const aColorKey,
                         const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr)
{
    VGCD_CtrlMsgType dispHwMsg = {
        .cfg = NULL,
        .pixel = NULL,
        .stats = NULL,
        .enable = aEnable,
        .colorKey = aColorKey,
        .cmd = VGCD_CTRLCMD_COLORKEY,
        .result = BCM_ERR_UNKNOWN,
    };
    return MSGQ_SendMsg(&VGCD_CtrlMsgQ, (void *)&dispHwMsg, aClientMask, aHdr);
}

/**
    @brief VGC Service Process Deinit

    @trace #BRCM_SWARCH_VGCD_PROCESS_DEINIT_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_ProcessDeInit(void)
{
    int32_t ret;
    uint32_t retries = 10UL;

    /* Deinit backlight */

    do {
        ret = VGC_Deinit();
        if (BCM_ERR_OK != ret) {
            VGCD_DIS_UErr(BRCM_SWARCH_VGCD_PROCESS_DEINIT_PROC, ret, (uint32_t)__LINE__, retries, 0UL, 0UL);
        }
        retries--;
    } while ((BCM_ERR_BUSY == ret) && (0UL != retries));

    if (BCM_ERR_OK != ret) {
        VGCD_DIS_UErr(BRCM_SWARCH_VGCD_PROCESS_DEINIT_PROC, ret, (uint32_t)__LINE__, retries, 0UL, 0UL);
    }
    return ret;
}

/**
    @brief VGCD Message Process control

    @trace #BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
int32_t VGCD_CtrlProcessMsg(void)
{
    VGCD_CtrlMsgType *msg = NULL;
    int32_t retVal = BCM_ERR_OK;
    uint32_t idx;
    int32_t err = MSGQ_GetMsgIdx(&VGCD_CtrlMsgQ, &idx);
    if (BCM_ERR_OK == err) {
        msg = (VGCD_CtrlMsgType *)VGCD_CtrlMsgQ.getMsg(idx);
        switch (msg->cmd){
        case VGCD_CTRLCMD_INIT:
            VGC_DispInit(msg->cfg);
            msg->result = BCM_ERR_OK;
            break;
        case VGCD_CTRLCMD_DEINIT:
            msg->result = VGCD_ProcessDeInit();
            retVal = BCM_ERR_UNINIT;
            break;
        case VGCD_CTRLCMD_ENABLE:
            msg->result = VGC_Start();;
            break;
        case VGCD_CTRLCMD_DISABLE:
            msg->result = VGC_Stop();
            break;
        case VGCD_CTRLCMD_BRIGHTNESS:
            /* TBD */
            msg->result = BCM_ERR_NOSUPPORT;
            break;
        case VGCD_CTRLCMD_BACKGROUND:
            msg->result = VGC_SetBackgroundColor(msg->pixel);
            break;
        case VGCD_CTRLCMD_GETSTATS:
            msg->result = VGC_DispGetStats(msg->stats);
            break;
        case VGCD_CTRLCMD_CSC:
            msg->result = VGC_CSC(msg->enable, msg->csc);
            break;
        case VGCD_CTRLCMD_COLORKEY:
            msg->result = VGC_ColorKey(msg->enable, msg->colorKey);
            break;
        default:
            msg->result = BCM_ERR_INVAL_PARAMS;
            VGCD_DIS_UErr(BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, msg->cmd, 0UL, 0UL);
            break;
        }
        err = MSGQ_CompleteMsgIdx(&VGCD_CtrlMsgQ, idx);
        if (BCM_ERR_OK != err) {
            VGCD_DIS_UErr(BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC, err, (uint32_t)__LINE__, (uint32_t)idx, 0UL, 0UL);
        }
    }

    return retVal;
}
/** @} */
