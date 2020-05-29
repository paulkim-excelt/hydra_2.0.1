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
    @defgroup grp_framework_impl Media System Implementation
    @ingroup grp_framework

    @addtogroup grp_framework_impl
    @{
    @todo update details here

    @file media_system.c
    @brief Media System Implementation

    This file implements application interface for media framework

    @version 0.1 Initial version
*/
#include <stddef.h>
#include <compiler.h>
#include <media_system.h>
#include <component.h>
#include <eecfg.h>
#include <bcm_err.h>
#include <ee.h>
#include <imgl.h>
#include <osil/bcm_osil.h>


/**
    @name MSYS Application Interface Design IDs
    @{
    @brief Design IDs for MSYS Application Interface
*/
#define BRCM_SWDSGN_MSYS_REPORT_APP_ERROR_PROC (0x90U) /**< @brief #MSYS_ReporAppError              */
#define BRCM_SWDSGN_MSYS_CONFIGDATA_SIZE_MACRO (0x91U) /**< @brief #MSYS_CONFIG_DATA_SIZE   */
#define BRCM_SWDSGN_MSYS_CONFIGDATA_GLOBAL     (0x92U) /**< @brief #MSYS_ConfigData                 */
#define BRCM_SWDSGN_MSYS_CHECK_PARAMS_PROC     (0x93U) /**< @brief #MSYS_CheckSystemParams          */
#define BRCM_SWDSGN_MSYS_INIT_PROC             (0x94U) /**< @brief #MSYS_Init                       */
#define BRCM_SWDSGN_MSYS_CONFIGURE_PROC        (0x95U) /**< @brief #MSYS_Configure                  */
#define BRCM_SWDSGN_MSYS_START_PROC            (0x96U) /**< @brief #MSYS_Start                      */
#define BRCM_SWDSGN_MSYS_STOP_PROC             (0x97U) /**< @brief #MSYS_Stop                       */
#define BRCM_SWDSGN_MSYS_DEINIT_PROC           (0x98U) /**< @brief #MSYS_DeInit                     */
#define BRCM_SWDSGN_MSYS_GET_STATS_PROC        (0x99U) /**< @brief #MSYS_GetInStats                 */
/** @} */

/**
    @trace #BRCM_SWARCH_MSYS_INIT_PROC
    @trace #BRCM_SWARCH_MSYS_CONFIGURE_PROC
    @trace #BRCM_SWARCH_MSYS_START_PROC
    @trace #BRCM_SWARCH_MSYS_STOP_PROC
    @trace #BRCM_SWARCH_MSYS_DEINIT_PROC
    @trace #BRCM_SWARCH_MSYS_GET_STATS_PROC
    @trace #BRCM_SWREQ_MSYS
*/
static void MSYS_ReporAppError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_MSF_ID, 0U, aApiID, aErr, 4UL, values);
}

/** @brief Media System Configuration Data Size

    @trace #BRCM_SWARCH_MSYS_CONFIGURE_PROC
    @trace #BRCM_SWREQ_MSYS
*/
#ifndef MSYS_CONFIG_DATA_SIZE
#define MSYS_CONFIG_DATA_SIZE   (28UL*1024UL)
#endif

/** @brief Media System Configuration Data Size
    The variable is aligned to 32 bytes because this byte-array is
    type-casted to config structures inside media components
    in the pipeline.
    Not doing so would make the compiler allocate an un-aligned address
    for this base array. This may lead to unaligned access while
    executing an LDRD type of instruction.

    @trace #BRCM_SWARCH_MSYS_CONFIGURE_PROC
    @trace #BRCM_SWREQ_MSYS
*/
static uint8_t MSYS_ConfigData[MSYS_CONFIG_DATA_SIZE] COMP_ALIGN(32) COMP_SECTION(".noload.data.fmwk");

/** @brief Helper function for checking Media System

    @trace #BRCM_SWARCH_MSYS_INIT_PROC
    @trace #BRCM_SWARCH_MSYS_CONFIGURE_PROC
    @trace #BRCM_SWARCH_MSYS_START_PROC
    @trace #BRCM_SWARCH_MSYS_STOP_PROC
    @trace #BRCM_SWARCH_MSYS_DEINIT_PROC
    @trace #BRCM_SWREQ_MSYS
*/
static int32_t MSYS_CheckSystemParams(const MSYS_Type* const aSystem)
{
    int32_t retVal = BCM_ERR_OK;
    if ((NULL == aSystem)
        || (MSYS_MAGIC != aSystem->magicID)
        || (0UL == aSystem->count)
        || (MSYS_MAX_LENGTH < aSystem->count)) {
            retVal = BCM_ERR_INVAL_PARAMS;
        }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_INIT_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_Init(const MSYS_Type* const aSystem)
{
    int32_t ret;
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;

    retVal = MSYS_CheckSystemParams(aSystem);
    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < aSystem->count; i++) {
            const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aSystem->component[i].compID);
            if (NULL == fTbl) {
                retVal = BCM_ERR_INVAL_PARAMS;
                MSYS_ReporAppError(BRCM_SWDSGN_MSYS_INIT_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
                break;
            }
            MSYS_CmdType resp;
            const MSYS_CmdType cmd = {
                .compID = aSystem->component[i].compID,
                .version = 0UL,
                .cmd = MSYS_COMP_CMD_INIT,
                .response = 1,
                .configID = 0UL,
                .cfgData = 0UL,
                .size = 0UL,
            };
            const MSGQ_MsgHdrType* hdr = NULL;
            retVal = fTbl->sendCmd(&cmd, SystemEvent1, &hdr);
            if (BCM_ERR_OK != retVal) {
                retVal = BCM_ERR_UNKNOWN;
                MSYS_ReporAppError(BRCM_SWDSGN_MSYS_INIT_PROC, retVal, __LINE__, i, 0UL, 0UL);
                break;
            }
            ret = BCM_WaitEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                retVal = BCM_ERR_UNKNOWN;
                MSYS_ReporAppError(BRCM_SWDSGN_MSYS_INIT_PROC, retVal, __LINE__, i, 0UL, 0UL);
                break;
            }
            ret = BCM_ClearEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                retVal = BCM_ERR_UNKNOWN;
                MSYS_ReporAppError(BRCM_SWDSGN_MSYS_INIT_PROC, retVal, __LINE__, i, 0UL, 0UL);
                break;
            }
            retVal = fTbl->recvResp(hdr, &resp);
            if ((BCM_ERR_OK != retVal) || (0 != resp.response)) {
                MSYS_ReporAppError(BRCM_SWDSGN_MSYS_INIT_PROC, retVal, __LINE__, i, (uint32_t)resp.response, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_CONFIGURE_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_Configure(const MSYS_Type* const aSystem)
{
    int32_t ret;
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;
    uint32_t j;
    uint32_t lSize = 0UL;

    retVal = MSYS_CheckSystemParams(aSystem);
    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < aSystem->count; i++) {
            for (j = 0UL; j < aSystem->component[i].numConfigs; j++) {
                const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aSystem->component[i].compID);
                const MSGQ_MsgHdrType* hdr = NULL;
                IMGL_LoadStatusType imglStatus;
                if (NULL == fTbl) {
                    MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, 0UL, 0UL);
                    retVal = BCM_ERR_INVAL_PARAMS;
                    break;
                }
                if (0UL != aSystem->component[i].config[j]) {
                    int32_t ptmStatus;
                    retVal = IMGL_LoadImg((uint16_t)aSystem->component[i].config[j], MSYS_ConfigData, 0UL,
                                        MSYS_CONFIG_DATA_SIZE, SystemEvent1, &imglStatus);
                    if (BCM_ERR_OK != retVal) {
                        MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, 0UL, 0UL);
                        break;
                    }
                    ret = BCM_WaitEvent(SystemEvent1);
                    if (BCM_ERR_OK != ret) {
                        MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                        break;
                    }
                    ret = BCM_ClearEvent(SystemEvent1);
                    if (BCM_ERR_OK != ret) {
                        MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                        break;
                    }
                    ptmStatus = IMGL_GetStatus(&imglStatus);
                    if ((BCM_ERR_NOMEM != ptmStatus) && (BCM_ERR_OK != ptmStatus)) {
                        MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, (uint32_t)ptmStatus, 0UL);
                        retVal = BCM_ERR_UNKNOWN;
                        break;
                    } else if (BCM_ERR_NOMEM == ptmStatus) {
                        /* Component is expected to load the image itself in this case */
                        lSize = 0UL;
                    } else {
                        lSize = imglStatus.size;
                    }
                }
                MSYS_CmdType resp;
                const MSYS_CmdType cmd = {
                    .compID = aSystem->component[i].compID,
                    .version = 0UL,
                    .cmd = MSYS_COMP_CMD_CONFIG,
                    .response = 1,
                    .configID = aSystem->component[i].config[j],
                    .cfgData = MSYS_ConfigData,
                    .size = lSize,
                };

                retVal = fTbl->sendCmd(&cmd, SystemEvent1, &hdr);
                if (BCM_ERR_OK != retVal) {
                    MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, 0UL, 0UL);
                    retVal = BCM_ERR_UNKNOWN;
                    break;
                }
                ret = BCM_WaitEvent(SystemEvent1);
                if (BCM_ERR_OK != ret) {
                    MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, 0UL, 0UL);
                    break;
                }
                ret = BCM_ClearEvent(SystemEvent1);
                if (BCM_ERR_OK != ret) {
                    MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, 0UL, 0UL);
                    break;
                }
                retVal = fTbl->recvResp(hdr, &resp);
                if ((BCM_ERR_OK != retVal) || (0 != resp.response)) {
                    MSYS_ReporAppError(BRCM_SWARCH_MSYS_CONFIGURE_PROC, retVal, __LINE__, i, (uint32_t)resp.response, 0UL);
                    retVal = BCM_ERR_UNKNOWN;
                    break;
                }
            }
            if (BCM_ERR_OK != retVal) {
                break;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_START_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_Start(const MSYS_Type * const aSystem)
{
    int32_t ret;
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;

    retVal = MSYS_CheckSystemParams(aSystem);
    if (BCM_ERR_OK == retVal) {

        for (i = aSystem->count; i > 0UL; i--) {
            const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aSystem->component[i-1UL].compID);
            if (NULL == fTbl) {
                retVal = BCM_ERR_INVAL_PARAMS;
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_START_PROC, retVal, __LINE__, i - 1UL, 0UL, 0UL);
                break;
            }
            MSYS_CmdType resp;
            const MSYS_CmdType cmd = {
                .compID = aSystem->component[i - 1UL].compID,
                .version = 0UL,
                .cmd = MSYS_COMP_CMD_START,
                .response = 1,
                .configID = 0UL,
                .cfgData = 0UL,
                .size = 0UL,
            };
            const MSGQ_MsgHdrType* hdr = NULL;
            retVal = fTbl->sendCmd(&cmd, SystemEvent1, &hdr);
            if (BCM_ERR_OK != retVal) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_START_PROC, retVal, __LINE__, i - 1UL, 0UL, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
            ret = BCM_WaitEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_START_PROC, retVal, __LINE__, i - 1UL, (uint32_t)ret, 0UL);
                break;
            }
            ret = BCM_ClearEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_START_PROC, retVal, __LINE__, i - 1UL, (uint32_t)ret, 0UL);
                break;
            }
            retVal = fTbl->recvResp(hdr, &resp);
            if ((BCM_ERR_OK != retVal) || (0 != resp.response)) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_START_PROC, retVal, __LINE__, i - 1UL,
                                   (uint32_t)resp.response, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_STOP_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_Stop(const MSYS_Type * const aSystem)
{
    int32_t ret;
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;

    retVal = MSYS_CheckSystemParams(aSystem);
    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < aSystem->count; i++) {
            const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aSystem->component[i].compID);
            if (NULL == fTbl) {
                retVal = BCM_ERR_INVAL_PARAMS;
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_STOP_PROC, retVal, __LINE__, i, 0UL, 0UL);
                break;
            }
            MSYS_CmdType resp;
            const MSYS_CmdType cmd = {
                .compID = aSystem->component[i].compID,
                .version = 0UL,
                .cmd = MSYS_COMP_CMD_STOP,
                .response = 1,
                .configID = 0UL,
                .cfgData = 0UL,
                .size = 0UL,
            };
            const MSGQ_MsgHdrType* hdr = NULL;
            retVal = fTbl->sendCmd(&cmd, SystemEvent1, &hdr);
            if (BCM_ERR_OK != retVal) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_STOP_PROC, retVal, __LINE__, i, 0UL, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
            ret = BCM_WaitEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_STOP_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                break;
            }
            ret = BCM_ClearEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_STOP_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                break;
            }
            retVal = fTbl->recvResp(hdr, &resp);
            if ((BCM_ERR_OK != retVal) || (0 != resp.response)) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_STOP_PROC, retVal, __LINE__, i,
                                   (uint32_t)resp.response, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_DEINIT_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_DeInit(const MSYS_Type* const aSystem)
{
    int32_t ret;
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;

    retVal = MSYS_CheckSystemParams(aSystem);
    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < aSystem->count; i++) {
            const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aSystem->component[i].compID);
            if (NULL == fTbl) {
                retVal = BCM_ERR_INVAL_PARAMS;
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_DEINIT_PROC, retVal, __LINE__, i, 0UL, 0UL);
                break;
            }
            MSYS_CmdType resp;
            const MSYS_CmdType cmd = {
                .compID = aSystem->component[i].compID,
                .version = 0UL,
                .cmd = MSYS_COMP_CMD_DEINIT,
                .response = 1,
                .configID = 0UL,
                .cfgData = 0UL,
                .size = 0UL,
            };
            const MSGQ_MsgHdrType* hdr = NULL;
            retVal = fTbl->sendCmd(&cmd, SystemEvent1, &hdr);
            if (BCM_ERR_OK != retVal) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_DEINIT_PROC, retVal, __LINE__, i, 0UL, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
            ret = BCM_WaitEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_DEINIT_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                break;
            }
            ret = BCM_ClearEvent(SystemEvent1);
            if (BCM_ERR_OK != ret) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_DEINIT_PROC, retVal, __LINE__, i, (uint32_t)ret, 0UL);
                break;
            }
            retVal = fTbl->recvResp(hdr, &resp);
            if ((BCM_ERR_OK != retVal) || (0 != resp.response)) {
                MSYS_ReporAppError(BRCM_SWARCH_MSYS_DEINIT_PROC, retVal, __LINE__, i,
                                   (uint32_t)resp.response, 0UL);
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_GET_STATS_PROC
    @trace #BRCM_SWREQ_MSYS
*/
int32_t MSYS_GetInStats(const uint64_t aCompId,
                              const uint32_t aInPortIdx,
                              MSYS_PortStatsType* const aStats)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL != aStats) {
        const MSYS_CompFnTblType *fTbl = MSYS_GetCompFnTbl(aCompId);
        if(NULL != fTbl){
            if (NULL != fTbl->getInPort) {
                const MSYS_InPortType * inPort = fTbl->getInPort(aInPortIdx);
                if (NULL != inPort) {
                    *aStats = inPort->inportRW->stats;
                    retVal = BCM_ERR_OK;
                }
            }
        } else {
            MSYS_ReporAppError(BRCM_SWARCH_MSYS_GET_STATS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
        }
    }
    if (BCM_ERR_OK != retVal) {
        MSYS_ReporAppError(BRCM_SWARCH_MSYS_GET_STATS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/** @} */
