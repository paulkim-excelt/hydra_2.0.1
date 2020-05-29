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
    @defgroup grp_ctn_log_impl Implementation
    @ingroup grp_tuners

    @addtogroup grp_ctn_log_impl
    @{
    @section sec_ctn_log_impl_overview Overview
    @todo Add overview

    @section sec_ctn_log_impl_fsm State Machine
    @todo Add state diagram

    @section sec_ctn_log_impl_seq Sequence Diagrams
    @todo Add sequence diagram

    @file ctn_log.c
    @brief Logging implementation for Tuning algorithms
    This header file implements logging APIs for Tuning algorithms
    @version 0.1 Initial version
*/
#include <ctn_log.h>
#ifdef CTN_ENABLE_LOGGING
#include <udp.h>
#include <inet_cfg.h>
#include <circ_queue.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#endif /* CTN_ENABLE_LOGGING */

/**
    @name CTN Logging Design IDs
    @{
    @brief Design IDs for CTN Logging
*/
#define BRCM_SWDSGN_CTN_REPORT_ERROR_PROC          (0x80U)  /**< @brief #CTN_ReportError          */
#define BRCM_SWDSGN_CTN_MAX_LOG_BUF_SIZE_MACRO     (0x81U)  /**< @brief #CTN_MAX_LOG_BUF_SIZE     */
#define BRCM_SWDSGN_CTN_MAX_NUM_LOG_BASE2_MACRO    (0x82U)  /**< @brief #CTN_MAX_NUM_LOG_BASE2    */
#define BRCM_SWDSGN_CTN_CTN_MAX_NUM_LOG_MACRO      (0x83U)  /**< @brief #CTN_MAX_NUM_LOG          */
#define BRCM_SWDSGN_CTN_LOG_SEND_IDX_INVALID_MACRO (0x84U)  /**< @brief #CTN_LOG_SEND_IDX_INVALID */
#define BRCM_SWDSGN_CTN_LOG_INET_COMP_ID_MACRO     (0x85U)  /**< @brief #CTN_LOG_INET_COMP_ID     */
#define BRCM_SWDSGN_CTN_LOG_CONTEXT_TYPE           (0x86U)  /**< @brief #CTN_LogContextType       */
#define BRCM_SWDSGN_CTN_LOG_CONTEXT_GLOBAL         (0x87U)  /**< @brief #CTN_LogContext           */
#define BRCM_SWDSGN_CTN_LE2BE_PROC                 (0x88U)  /**< @brief #CTN_LE2BE                */
#define BRCM_SWDSGN_CTN_INIT_LOG_PROC              (0x89U)  /**< @brief #CTN_InitLog              */
#define BRCM_SWDSGN_CTN_DEINIT_LOG_PROC            (0x8aU)  /**< @brief #CTN_DeinitLog            */
#define BRCM_SWDSGN_CTN_LOG_PROC                   (0x8bU)  /**< @brief #CTN_Log                  */
#define BRCM_SWDSGN_CTN_PROCESS_LOGS_PROC          (0x8cU)  /**< @brief #Tuner_ProcessLogs        */
/** @} */

#ifdef CTN_ENABLE_LOGGING
/**
    @brief Report Error function
*/
static void CTN_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                            uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CTN_ID, 0U, aApiID, aErr, 4UL, values);
}
#endif /* CTN_ENABLE_LOGGING */

#ifdef CTN_ENABLE_LOGGING
/**
    @brief Maximum logging buffer size
*/
#define CTN_MAX_LOG_BUF_SIZE      (256U)

/**
    @brief Maximum number of logging entries(base 2)
*/
#define CTN_MAX_NUM_LOG_BASE2     (6U)

/**
    @brief Maximum number of logging entries
*/
#define CTN_MAX_NUM_LOG           (1U << CTN_MAX_NUM_LOG_BASE2)

/**
    @brief Invalied Index value
*/
#define CTN_LOG_SEND_IDX_INVALID  (-1L)

/**
    @brief Component ID for CTN logging
*/
#define CTN_LOG_INET_COMP_ID      (0x2222ULL)

/**
    @brief Free Queue of buffer indexes
*/
CIRCQ_V2_DEFINE(CTN_LogFreeQ, CTN_MAX_NUM_LOG_BASE2, uint8_t, CTN_LogFreeQBuf,
                CTN_LogFreeQIdx, CIRCQ_OVERWRITE_DISABLE,
                ".data");
/**
    @brief Filled Queue of buffer indexes
*/
CIRCQ_V2_DEFINE(CTN_LogFilledQ, CTN_MAX_NUM_LOG_BASE2, uint8_t, CTN_LogFilledQBuf,
                CTN_LogFilledQIdx, CIRCQ_OVERWRITE_DISABLE,
                ".data");

/**
    @brief Endpoint configuration
*/
static const INET_EndPointIPPort CTN_LogEndPtCfg = {CTN_LOG_UDP_PORT,
                                             CTN_LOG_UDP_PORT,
                                             INET_CONVERT_IP_ADDRESS(
                                             (192U), (168U), (10U), (2U))};
/**
    @brief CTN Logging context type
*/
typedef struct _CTN_LogContextType {
    uint8_t  isInitialised;                                 /* Is the Queue initialised */
    uint8_t  buffer[CTN_MAX_NUM_LOG][CTN_MAX_LOG_BUF_SIZE]; /* Logging buffer pool      */
    uint16_t bufferSize[CTN_MAX_NUM_LOG];                   /* Logging buffer sizes     */
    int32_t  sendIdx;                                       /* Send Idx                 */
} CTN_LogContextType;

/**
    @brief CTN context global
*/
static CTN_LogContextType CTN_LogContext=
{
    .isInitialised = 0U,
    .sendIdx = CTN_LOG_SEND_IDX_INVALID,
};

/**
    @brief Function to convert LE to BE
*/
static inline uint32_t CTN_LE2BE(uint32_t aVal)
{
	return (((aVal >> 24U) & 0x000000ffUL) |
            ((aVal >> 8U)  & 0x0000ff00UL) |
            ((aVal << 8U)  & 0x00ff0000UL) |
            ((aVal << 24U) & 0xff000000UL));
}
#endif /* CTN_ENABLE_LOGGING */

/**
    Detailed design of CTN_InitLog

    @trace #BRCM_SWARCH_CTN_INIT_LOG_PROC
    @trace #BRCM_SWREQ_CTN_LOG
*/
int32_t CTN_InitLog(void)
{
    int32_t ret = BCM_ERR_OK;
#ifdef CTN_ENABLE_LOGGING
    uint32_t i;

    /* Reset CircQs */
    ret = CIRCQ_Reset(&CTN_LogFreeQ);
    if (BCM_ERR_OK != ret) {
        CTN_ReportError(BRCM_SWDSGN_CTN_INIT_LOG_PROC,
                ret, __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }
    /* Add elements to freeQ */
    for (i = 0UL; i < (CTN_MAX_NUM_LOG - 1UL); i++) {
        uint32_t idx = i;
        ret = CIRCQ_Push(&CTN_LogFreeQ, (char *)&idx, 1UL);
        if (BCM_ERR_OK != ret) {
            CTN_ReportError(BRCM_SWDSGN_CTN_INIT_LOG_PROC,
                    ret, __LINE__, i, 0UL, 0UL);
            goto func_exit;
        }
    }
    ret = CIRCQ_Reset(&CTN_LogFilledQ);
    if (BCM_ERR_OK != ret) {
        CTN_ReportError(BRCM_SWDSGN_CTN_INIT_LOG_PROC,
                ret, __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }
    CTN_LogContext.sendIdx = CTN_LOG_SEND_IDX_INVALID;
    CTN_LogContext.isInitialised = 1U;

func_exit:
    if (BCM_ERR_OK != ret) {
        ret = BCM_ERR_UNKNOWN;
    }
#else
    ret = BCM_ERR_NOSUPPORT;
#endif /* CTN_ENABLE_LOGGING */

    return ret;
}

/**
    Detailed design of CTN_DeinitLog

    @trace #BRCM_SWARCH_CTN_DEINIT_LOG_PROC
    @trace #BRCM_SWREQ_CTN_LOG
*/
int32_t CTN_DeinitLog(void)
{
    int32_t ret = BCM_ERR_OK;
#ifdef CTN_ENABLE_LOGGING
    if (0U != CTN_LogContext.isInitialised) {
        if (CTN_LOG_SEND_IDX_INVALID != CTN_LogContext.sendIdx) {
            ret = UDP_CancelSend(CTN_LOG_INET_COMP_ID, INET_UDP_PAYLOAD_ID1);
            if (BCM_ERR_OK != ret) {
                CTN_ReportError(BRCM_SWDSGN_CTN_DEINIT_LOG_PROC,
                        ret, __LINE__, 0UL, 0UL, 0UL);
            }
            CTN_LogContext.sendIdx = CTN_LOG_SEND_IDX_INVALID;
        }

        /* Reset CircQs */
        ret = CIRCQ_Reset(&CTN_LogFreeQ);
        if (BCM_ERR_OK != ret) {
            CTN_ReportError(BRCM_SWDSGN_CTN_DEINIT_LOG_PROC,
                    ret, __LINE__, 0UL, 0UL, 0UL);
        }

        ret = CIRCQ_Reset(&CTN_LogFilledQ);
        if (BCM_ERR_OK != ret) {
            CTN_ReportError(BRCM_SWDSGN_CTN_DEINIT_LOG_PROC,
                    ret, __LINE__, 0UL, 0UL, 0UL);
        }

        CTN_LogContext.isInitialised = 0U;

        if (BCM_ERR_OK != ret) {
            ret = BCM_ERR_UNKNOWN;
        }
    }
#else
    ret = BCM_ERR_NOSUPPORT;
#endif /* CTN_ENABLE_LOGGING */

    return ret;
}

/**
    Detailed design of CTN_Log

    @trace #BRCM_SWARCH_CTN_LOG_PROC
    @trace #BRCM_SWREQ_CTN_LOG
*/
extern int32_t CTN_Log(CTN_LogIdType aLogId,
                  uint8_t aDataType,
                  uint32_t aFrameNo,
                  uint8_t *aData,
                  uint16_t aSize)
{
    int32_t ret = BCM_ERR_OK;
#ifdef CTN_ENABLE_LOGGING
    uint8_t idx = 0U;
    uint32_t cnt = 0U;

    if ((NULL != aData) && (0U != aSize) &&
                           ((aLogId >> 8U) == (CTN_LOG_ID_AGC >> 8U))) {
        if (1U == CTN_LogContext.isInitialised) {
            /* Check Size */
            if (aSize > (CTN_MAX_LOG_BUF_SIZE - sizeof(CTN_LogHdrType))) {
                ret = BCM_ERR_NOMEM;
                CTN_ReportError(BRCM_SWDSGN_CTN_LOG_PROC,
                        ret, __LINE__, 0UL, 0UL, 0UL);
            } else {
                cnt = CIRCQ_Pop(&CTN_LogFreeQ,
                                (char *) &idx,
                                1UL);
                if (1UL == cnt) {
                    union {
                        CTN_LogHdrType *pHdr;
                        uint8_t *pu8Ptr;
                    } ptrParam;
                    uint8_t *mem = &CTN_LogContext.buffer[idx][0];
                    CTN_LogHdrType *hdr;
                    BCM_TimeType timestamp = {0UL, 0UL};

                    ptrParam.pu8Ptr = mem;
                    hdr = ptrParam.pHdr;

                    BCM_GetTime(&timestamp);
                    BCM_MemSet(hdr, 0, sizeof(CTN_LogHdrType));

                    hdr->moduleId = CTN_LE2BE(aLogId);
                    hdr->dataType = aDataType;
                    hdr->frameNo = CTN_LE2BE(aFrameNo);
                    hdr->timeSec = CTN_LE2BE(timestamp.s);
                    hdr->timeNsec = CTN_LE2BE(timestamp.ns);

                    /* Copy data */
                    BCM_MemCpy((mem + sizeof(CTN_LogHdrType)), aData, aSize);

                    /* Fill size */
                    CTN_LogContext.bufferSize[idx] = aSize + sizeof(CTN_LogHdrType);

                    /* Push the idx to filledQ */
                    ret = CIRCQ_Push(&CTN_LogFilledQ, (char *)&idx, 1UL);
                    if (BCM_ERR_OK != ret) {
                        CTN_ReportError(BRCM_SWDSGN_CTN_LOG_PROC,
                                ret, __LINE__, 0UL, 0UL, 0UL);
                        /* Push it back to freeQ */
                        ret = CIRCQ_Push(&CTN_LogFreeQ, (char *)&idx, 1UL);
                        if (BCM_ERR_OK != ret) {
                            CTN_ReportError(BRCM_SWDSGN_CTN_LOG_PROC,
                                    ret, __LINE__, 0UL, 0UL, 0UL);
                        }
                    }
                } else {
                    ret = BCM_ERR_NOMEM;
                }
            }
        } else {
            ret = BCM_ERR_INVAL_STATE;
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
        CTN_ReportError(BRCM_SWDSGN_CTN_LOG_PROC,
                ret, __LINE__, (uint32_t)aLogId, (uint32_t)aSize, 0UL);
    }
#else
    ret = BCM_ERR_NOSUPPORT;
#endif /* CTN_ENABLE_LOGGING */

    return ret;
}

/**
    Detailed design of CTN_ProcessLogs

    @trace #BRCM_SWARCH_CTN_PROCESS_LOGS
    @trace #BRCM_SWREQ_CTN_LOG
*/
extern int32_t CTN_ProcessLogs(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef CTN_ENABLE_LOGGING
    if (1U == CTN_LogContext.isInitialised) {
        uint8_t idx = 0U;
        uint32_t cnt = 0UL;

        if (CTN_LOG_SEND_IDX_INVALID != CTN_LogContext.sendIdx) {
            ret = UDP_Process(CTN_LOG_INET_COMP_ID, INET_UDP_PAYLOAD_ID1);
            if (BCM_ERR_BUSY == ret) {
                goto func_exit;
            } else {
                if (BCM_ERR_OK != ret) {
                    CTN_ReportError(BRCM_SWDSGN_CTN_PROCESS_LOGS_PROC,
                            ret, __LINE__, 0UL, 0UL, 0UL);
                }
                /* Add to FreeQ */
                ret = CIRCQ_Push(&CTN_LogFreeQ, (char *)&CTN_LogContext.sendIdx, 1UL);
                if (BCM_ERR_OK != ret) {
                    CTN_ReportError(BRCM_SWDSGN_CTN_PROCESS_LOGS_PROC,
                            ret, __LINE__, 0UL, 0UL, 0UL);
                    goto func_exit;
                }
                CTN_LogContext.sendIdx = CTN_LOG_SEND_IDX_INVALID;
            }
        }

        cnt = CIRCQ_Pop(&CTN_LogFilledQ,
                             (char *) &idx,
                             1UL);
        while (1UL == cnt) {
            uint8_t *mem = &CTN_LogContext.buffer[idx][0];
            uint16_t size = CTN_LogContext.bufferSize[idx];

            /* Send directly for UDP verification */
            ret = UDP_SendTo(CTN_LOG_INET_COMP_ID,
                             INET_UDP_PAYLOAD_ID1,
                             mem, size, &CTN_LogEndPtCfg);

            if (BCM_ERR_BUSY == ret) {
                /* Save Send IDX */
                CTN_LogContext.sendIdx = (int32_t) idx;
            } else {
                /* Add it to FreeQ */
                ret = CIRCQ_Push(&CTN_LogFreeQ, (char *)&idx, 1UL);
                if (BCM_ERR_OK != ret) {
                    CTN_ReportError(BRCM_SWDSGN_CTN_PROCESS_LOGS_PROC,
                            ret, __LINE__, 0UL, 0UL, 0UL);
                    goto func_exit;
                }
            }
            cnt = CIRCQ_Pop(&CTN_LogFilledQ,
                                 (char *) &idx,
                                 1UL);
        }
    } else {
        ret = BCM_ERR_INVAL_STATE;
    }

func_exit:
    if ((BCM_ERR_OK != ret) && (BCM_ERR_INVAL_STATE != ret)) {
        ret = BCM_ERR_UNKNOWN;
    }
#else
    ret = BCM_ERR_NOSUPPORT;
#endif /* CTN_ENABLE_LOGGING */

    return ret;
}
/** @} */
