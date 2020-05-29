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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_rfs_il RFS Implementation and Integration
    @ingroup grp_rfs

    @addtogroup grp_rfs_il
    @{

    @file rfs.c
    @brief Remote File System (RFS) implementation and integration
    This source file contains the implementation and integration
    details of RFS.
    @version 0.1 Initial version
*/
#include <stdint.h>
#include <rfs.h>
#include <msg_queue.h>
#include <system.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <tftp.h>
#include <inet_cfg.h>
#include <osil/bcm_osil.h>
#include "ee.h"
/**
    @name RFS Design IDs
    @{
    @brief Design IDs for RFS

*/
#define BRCM_SWDSGN_RFS_OFFSET_FILE_END_MACRO (0xC0U)   /**< @brief #RFS_OFFSET_FILE_END   */
#define BRCM_SWDSGN_RFS_MSG_TYPE              (0xC1U)   /**< @brief #RFS_MsgType           */
#define BRCM_SWDSGN_RFS_MSG_READ_TYPE         (0xC2U)   /**< @brief #RFS_MsgReadType       */
#define BRCM_SWDSGN_RFS_MSG_WRITE_TYPE        (0xC3U)   /**< @brief #RFS_MsgWriteType      */
#define BRCM_SWDSGN_RFS_MSG_READ_GLOBAL       (0xC4U)   /**< @brief #RFS_MsgRead           */
#define BRCM_SWDSGN_RFS_MSG_WRITE_GLOBAL      (0xC5U)   /**< @brief #RFS_MsgWrite          */
#define BRCM_SWDSGN_RFS_READ_CONTEXT_GLOBAL   (0xC6U)   /**< @brief #RFS_ReadContext       */
#define BRCM_SWDSGN_RFS_WRITE_CONTEXT_GGLOBAL (0xC7U)   /**< @brief #RFS_WriteContext      */
#define BRCM_SWDSGN_RFS_GET_READ_MSG_PROC     (0xC8U)   /**< @brief #RFS_GetMsgRead        */
#define BRCM_SWDSGN_RFS_GET_WRITE_MSG_PROC    (0xC9U)   /**< @brief #RFS_GetMsgWrite       */
#define BRCM_SWDSGN_RFS_READ_HEADER_Q_GLOBAL  (0xCAU)   /**< @brief #RFS_READHDRQ          */
#define BRCM_SWDSGN_RFS_READ_Q_GLOBAL         (0xCBU)   /**< @brief #RFS_READQ             */
#define BRCM_SWDSGN_RFS_WRTE_HEADER_Q_GLOBAL  (0xCBU)   /**< @brief #RFS_WRITEHDRQ         */
#define BRCM_SWDSGN_RFS_WRITE_Q_GLOBAL        (0xCCU)   /**< @brief #RFS_WRITEQ            */
#define BRCM_SWDSGN_RFS_REPORT_ERROR_PROC     (0xCDU)   /**< @brief #RFS_ReportError       */
#define BRCM_SWDSGN_RFS_NUM_TO_CHR_LUT_GLOBAL (0xCEU)   /**< @brief #RFS_NumToCharTable    */
#define BRCM_SWDSGN_RFS_CONSTR_FILENAME_PROC  (0xCFU)   /**< @brief #RFS_ConstructFileName */
#define BRCM_SWDSGN_RFS_READ_PROCESS_PROC     (0xD0U)   /**< @brief #RFS_ReadProcess       */
#define BRCM_SWDSGN_RFS_WRITE_PROCESS_PROC    (0xD1U)   /**< @brief #RFS_WriteProcess      */
#define BRCM_SWDSGN_RFS_OPEN_PROC             (0xD2U)   /**< @brief #RFS_Open              */
#define BRCM_SWDSGN_RFS_CLOSE_PROC            (0xD3U)   /**< @brief #RFS_Close             */
#define BRCM_SWDSGN_RFS_READ_PROC             (0xD4U)   /**< @brief #RFS_Read              */
#define BRCM_SWDSGN_RFS_WRITE_PROC            (0xD5U)   /**< @brief #RFS_Write             */
#define BRCM_SWDSGN_RFS_SEEK_PROC             (0xD6U)   /**< @brief #RFS_Seek              */
#define BRCM_SWDSGN_RFS_GET_STATUS_PROC       (0xD7U)   /**< @brief #RFS_GetStatus         */
#define BRCM_SWDSGN_RFS_ALARM_CB_PROC         (0xD8U)   /**< @brief #RFS_AlarmCb           */
#define BRCM_SWDSGN_RFS_IL_TASK_PROC          (0xD9U)   /**< @brief #RFS_Task              */
/** @} */

/**
    @brief Identifies the end of file

    Identifies the offset at end of file

    @trace #BRCM_SWARCH_RFS_OPEN_PROC
    @trace #BRCM_SWARCH_RFS_SEEK_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
#define RFS_OFFSET_FILE_END                   (0xFFFFFFFFUL)

/**
    @brief RFS message details

    RFS message details.

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
 */
typedef struct _RFS_MsgType {
    uint8_t fileName[RFS_MAX_FILENAME_LEN]; /**< @brief File name               */
    uint32_t offset;                        /**< @brief Offset                  */
    uint32_t inLen;                         /**< @brief Length requested        */
    uint32_t actLen;                        /**< @brief Length serviced         */
    int32_t errorStatus;                    /**< @brief Error status            */
} RFS_MsgType;

/**
    @brief RFS read message details

    RFS read message details.

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
 */
typedef struct _RFS_MsgReadType {
    RFS_MsgType params;                     /**< @brief Message details         */
    uint8_t *buffer;                        /**< @brief Buffer to read into     */
} RFS_MsgReadType;

/**
    @brief RFS write message details

    RFS write message details.

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
 */
typedef struct _RFS_MsgWriteType {
    RFS_MsgType params;                     /**< @brief Message details         */
    const uint8_t *buffer;                  /**< @brief Buffer to write from    */
} RFS_MsgWriteType;

/**
    @brief Global RFS read messages buffer

    Global RFS read messages buffer

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
static RFS_MsgReadType RFS_MsgRead[MSGQ_SIZE];

/**
    @brief Global RFS write messages buffer

    Global RFS write messages buffer

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
static RFS_MsgWriteType RFS_MsgWrite[MSGQ_SIZE];

/**
    @brief Global TFTP context for RFS read

    Global TFTP context for RFS read

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
static TFTP_ContextType RFS_ReadContext;

/**
    @brief Global TFTP context for RFS write

    Global TFTP context for RFS write

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
static TFTP_ContextType RFS_WriteContext;

/** @brief To get the read message

    It returns the read message at the given index

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIdx       Index of the message requested

    @return void* (pointer to the message)

    @post None

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations  None

    Returns pointer to the message if the given index exists else returns NULL.

    @code{.unparsed}
    *msg = NULL

    if MSGQ_SIZE is greater than aIdx {
        msg = (void *)(&RFS_MsgRead[aIdx])
    }

    return msg
    @endcode

*/
static void *RFS_GetMsgRead(uint32_t aIdx)
{
    void *msg = NULL;

    if (MSGQ_SIZE > aIdx) {
        msg = (void *)(&RFS_MsgRead[aIdx]);
    }

    return msg;
}

/** @brief To get the write message

    It returns the write message at the given index

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIdx       Index of the message requested

    @return void* (pointer to the message)

    @post None

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations  None

    Returns pointer to the message if the given index exists else returns NULL.

    @code{.unparsed}
    *msg = NULL

    if MSGQ_SIZE is greater than aIdx {
        msg = (void *)(&RFS_MsgWrite[aIdx])
    }

    return msg
    @endcode

*/
static void *RFS_GetMsgWrite(uint32_t aIdx)
{
    void *msg = NULL;

    if (MSGQ_SIZE > aIdx) {
        msg = (void *)(&RFS_MsgWrite[aIdx]);
    }

    return msg;
}

/**
    @brief Global RFS read queue header

    Global RFS read queue header

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
MSGQ_DEFINE_HDRQ(RFS_READHDRQ);

/**
    @brief Global RFS read queue

    Global RFS read queue

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
MSGQ_DEFINE(RFS_READQ, RFS_Task, RFS_READQ_EVENT_ID, MSGQ_CLIENT_SIGNAL_ENABLE,
                RFS_MsgReadType, &RFS_READHDRQ, RFS_GetMsgRead);

/**
    @brief Global RFS write queue header

    Global RFS write queue header

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
MSGQ_DEFINE_HDRQ(RFS_WRITEHDRQ);

/**
    @brief Global RFS write queue

    Global RFS write queue

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
MSGQ_DEFINE(RFS_WRITEQ, RFS_Task, RFS_WRITEQ_EVENT_ID, MSGQ_CLIENT_SIGNAL_ENABLE,
                RFS_MsgWriteType, &RFS_WRITEHDRQ, RFS_GetMsgWrite);

/** @brief RFS Error reporting

    RFS error reporting

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aApiID      API ID
    @param[in]      aErr        Error ID
    @param[in]      aVal0       Variable 0
    @param[in]      aVal1       Variable 1
    @param[in]      aVal2       Variable 2
    @param[in]      aVal3       Variable 3

    @return void

    @post None


    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None

    Error reporting of RFS.

    @code{.unparsed}
    call ReportError
    @endcode

*/
static void RFS_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RFS_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief Global RFS LUT

    Look up table to convert nibbles to character.

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT
*/
static const char RFS_NumToCharTable[] = {
    '0','1','2','3',
    '4','5','6','7',
    '8','9','A','B',
    'C','D','E','F'
};

/** @brief RFS construct file name

    Constructs final filename.

    @behavior Sync, Re-entrant

    @pre None

    @param[inout]   aDest       Pointer to store the result
    @param[in]      msg         Pointer to structure holding the message
                                details i.e. file name,offset and length

    @return void

    @post None


    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations aDest is expected to be sufficient to hold the result

    Construts the final name by appending the filename with '+' follwed
    by 8 character hex representation of offset follwed by a '+' with
    8 character hex representation of length

    @code{.unparsed}
    len = strnlen((char *)msg->fileName, RFS_MAX_FILENAME_LEN);
    BCM_MemCpy(aDest, msg->fileName, len);
    temp = msg->offset;
    aDest[len] = '+';
    cur = &aDest[len + 8UL];
    for (i = 0UL; i < 8UL; i++) {
        *cur-- = RFS_NumToCharTable[temp & 0xFUL];
        temp >>= 4UL;
    }
    temp = msg->inLen;
    aDest[len + 9UL] = '+';
    cur = &aDest[len + 17UL];
    for (i = 0UL; i < 8UL; i++) {
        *cur-- = RFS_NumToCharTable[temp & 0xFUL];
        temp >>= 4UL;
    }
    aDest[len + 18UL] = '\0';
    @endcode

*/
static void RFS_ConstructFileName(char *aDest, RFS_MsgType *msg)
{
    uint32_t i;
    uint32_t temp;
    uint32_t len;
    char *cur;

    len = strnlen((char *)msg->fileName, RFS_MAX_FILENAME_LEN);
    BCM_MemCpy(aDest, msg->fileName, len);

    temp = msg->offset;
    aDest[len] = '+';
    cur = &aDest[len + 8UL];
    for (i = 0UL; i < 8UL; i++) {
        *cur-- = RFS_NumToCharTable[temp & 0xFUL];
        temp >>= 4UL;
    }
    temp = msg->inLen;
    aDest[len + 9UL] = '+';
    cur = &aDest[len + 17UL];
    for (i = 0UL; i < 8UL; i++) {
        *cur-- = RFS_NumToCharTable[temp & 0xFUL];
        temp >>= 4UL;
    }
    aDest[len + 18UL] = '\0';
}

/** @brief RFS read process

    Starts RFS read if a request is pending and can be started
    else if a request is already in process, checks its status

    @behavior Sync, Re-entrant

    @pre None

    @param[in] void

    @return void

    @post None

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None

    If TFTP read context is free and a read request is pending
    in the queue then start the read operation. If it fails to start, complete
    the message with error. If TFTP read context is in use, then check its status and
    update client if its done.

    @code{.unparsed}
    do
        complete = false
        ret = MSGQ_GetMsgIdx(&RFS_READQ, &idx);
        if ret is ERR_OK
            if RFS_ReadContext.state is TFTP_STATE_IDLE
                if (ERR_OK == MSGQ_GetMsgIdx(&RFS_READQ, &idx)) {
                    RFS_ConstructFileName(fileName, &RFS_MsgRead[idx].params);
                    retVal = TFTP_ClientRead(&RFS_ReadContext,
                                             fileName);
                    if (ERR_OK != retVal) {
                        RFS_MsgRead[idx].params.errorStatus = retVal;
                        RFS_MsgRead[idx].params.outlen = 0;
                        complete = true;
                    }
                }
            else if RFS_ReadContext.state is TFTP_STATE_READ
                retVal = TFTP_GetDataHandler(&RFS_ReadContext,
                                             RFS_MsgRead[idx].buffer,
                                             RFS_MsgRead[idx].params.inLen,
                                             &outLen);
                if ((ERR_OK == retVal) || (ERR_BUSY != retVal)) {
                    uint32_t idx = 0UL;
                    RFS_MsgRead[idx].params.actLen = outLen;
                    RFS_MsgRead[idx].params.errorStatus = retVal;
                    complete = true;
                }
            }
            if complete is true
                MSGQ_CompleteMsgIdx(&RFS_READQ, idx);
    loop until complete flag is true
    @endcode
*/
static void RFS_ReadProcess(void)
{
    int32_t retVal;
    uint32_t idx = 0UL;
    RFS_MsgReadType *msg = NULL;
    uint32_t completeMsg = 0UL;

    do {
        completeMsg = 0UL;
        retVal = MSGQ_GetMsgIdx(&RFS_READQ, &idx);
        if (BCM_ERR_OK == retVal) {
            msg = &RFS_MsgRead[idx];
            if (TFTP_STATE_INIT == RFS_ReadContext.state) {
                /* start */
                char fileName[TFTP_MAX_FILENAME_LEN];
                RFS_ConstructFileName(fileName, &msg->params);

                /* send the request to tftp */
                retVal = TFTP_ClientRead(&RFS_ReadContext,
                                        (uint8_t *)fileName);
                if (BCM_ERR_OK != retVal) {
                    /* complete the message with error */
                    msg->params.errorStatus = retVal;
                    msg->params.actLen = 0UL;
                    completeMsg = 1UL;
                    RFS_ReportError(BRCM_SWDSGN_RFS_READ_PROCESS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
                }
            } else if (TFTP_STATE_READ == RFS_ReadContext.state) {
                /* check status */
                retVal = TFTP_GetDataHandler(&RFS_ReadContext,
                                             msg->buffer,
                                             msg->params.inLen,
                                             &msg->params.actLen);
                if ((BCM_ERR_OK == retVal) || (BCM_ERR_BUSY != retVal)) {
                    /* complete */
                    msg->params.errorStatus = retVal;
                    completeMsg = 1UL;
                }
            }
            if (0UL != completeMsg) {
                retVal = MSGQ_CompleteMsgIdx(&RFS_READQ, idx);
                if (BCM_ERR_OK != retVal) {
                    RFS_ReportError(BRCM_SWDSGN_RFS_READ_PROCESS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    } while (0UL != completeMsg);
}

/** @brief RFS write process

    Starts RFS write if a request is pending and can be started
    else if a request is already in process, checks its status

    @behavior Sync, Re-entrant

    @pre None

    @param[in] void

    @return void

    @post None

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None

    If TFTP write context is free and a write request is pending
    in the queue then start the write operation. If it fails to start, complete
    the message with error. If TFTP write context is in use, then check its status and
    update client if its done.

    @code{.unparsed}
    do
        complete = false
        ret = MSGQ_GetMsgIdx(&RFS_WRITEQ, &idx);
        if ret is ERR_OK
            if RFS_ReadContext.state is TFTP_STATE_IDLE
                if (ERR_OK == MSGQ_GetMsgIdx(&RFS_WRITEQ, &idx)) {
                    RFS_ConstructFileName(fileName, &RFS_MsgWrite[idx].params);
                    retVal = TFTP_ClientWrite(&RFS_ReadContext,
                                              fileName);
                    if (ERR_OK != retVal) {
                        RFS_MsgWrite[idx].params.errorStatus = retVal;
                        RFS_MsgWrite[idx].params.outlen = 0;
                        complete = true;
                    }
                }
            else if RFS_ReadContext.state is TFTP_STATE_WRITE
                retVal = TFTP_PutDataHandler(&RFS_ReadContext,
                                             RFS_MsgWrite[idx].buffer,
                                             RFS_MsgWrite[idx].params.inLen,
                                             &outLen);
                if ((ERR_OK == retVal) || (ERR_BUSY != retVal)) {
                    uint32_t idx = 0UL;
                    RFS_MsgWrite[idx].params.actLen = outLen;
                    RFS_MsgWrite[idx].params.errorStatus = retVal;
                    complete = true;
                }
            }
            if complete is true
                MSGQ_CompleteMsgIdx(&RFS_WRITEQ, idx);
    loop until complete flag is true
    @endcode
*/
static void RFS_WriteProcess(void)
{
    int32_t retVal;
    uint32_t idx = 0UL;
    uint32_t outLen = 0UL;
    RFS_MsgWriteType *msg = NULL;
    uint32_t completeMsg = 0UL;

    do {
        completeMsg = 0UL;
        retVal = MSGQ_GetMsgIdx(&RFS_WRITEQ, &idx);
        if (BCM_ERR_OK == retVal) {
            msg = &RFS_MsgWrite[idx];
            if (TFTP_STATE_INIT == RFS_ReadContext.state) {
                /* start */
                char fileName[TFTP_MAX_FILENAME_LEN];
                RFS_ConstructFileName(fileName, &msg->params);

                /* send the request to tftp */
                retVal = TFTP_ClientWrite(&RFS_ReadContext,
                                         (uint8_t *)fileName);
                if (BCM_ERR_OK != retVal) {
                    /* complete the message with error */
                    msg->params.errorStatus = retVal;
                    msg->params.actLen = 0UL;
                    completeMsg = 1UL;
                    RFS_ReportError(BRCM_SWDSGN_RFS_WRITE_PROCESS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
                }
            } else if (TFTP_STATE_WRITE == RFS_ReadContext.state) {
                /* check status */
                retVal = TFTP_PutDataHandler(&RFS_ReadContext,
                                             msg->buffer,
                                             msg->params.inLen,
                                             &outLen);
                if ((BCM_ERR_OK == retVal) || (BCM_ERR_BUSY != retVal)) {
                    /* complete and update size for buffer of non multiple of 512 bytes size */
                    if (0UL == msg->params.actLen) {
                        msg->params.actLen = outLen;
                    }
                    msg->params.errorStatus = retVal;
                    completeMsg = 1UL;
                } else if ((BCM_ERR_BUSY == retVal) && (0UL < outLen) && (msg->params.inLen == outLen)) {
                    msg->params.actLen = outLen;
                    /* For a buffer of %512, we need to send a buffer of */
                    /* length 0 to indicate end of payload.              */
                    msg->params.inLen = 0UL;
                }
            }
            if (0UL != completeMsg) {
                retVal = MSGQ_CompleteMsgIdx(&RFS_WRITEQ, idx);
                if (BCM_ERR_OK != retVal) {
                    RFS_ReportError(BRCM_SWDSGN_RFS_WRITE_PROCESS_PROC, retVal, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    } while (0UL != completeMsg);
}

/**
    Detailed design of RFS_Open

    @trace #BRCM_SWARCH_RFS_OPEN_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else
        Copy file name to context
        if aMode is RFS_MODE_READ
            aHandle->offset = 0UL
            aHandle->state = RFS_STATE_READ
        else if aMode is RFS_MODE_WRITE
            aHandle->offset = 0UL;
            aHandle->state = RFS_STATE_WRITE
        else if aMode is RFS_MODE_APPEND
            aHandle->offset = RFS_OFFSET_FILE_END
            aHandle->state = RFS_STATE_WRITE
        else
            aHandle->state = RFS_STATE_RESET
            ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode
*/
int32_t RFS_Open(RFS_FileHandleType *const aHandle,
                 const uint8_t *const aFileName,
                 const RFS_ModeType aMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((NULL == aHandle) ||
        (NULL == aFileName)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RFS_MAX_FILENAME_LEN == strnlen((char *)aFileName, RFS_MAX_FILENAME_LEN)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        aHandle->hdr = NULL;

        /* copy the filename */
        BCM_MemCpy(aHandle->fileName,aFileName,strnlen((char *)aFileName, RFS_MAX_FILENAME_LEN) + 1UL);

        switch(aMode) {
            case RFS_MODE_READ:
                /* reset the offset */
                aHandle->offset = 0UL;
                aHandle->state = RFS_STATE_READ;
                break;
            case RFS_MODE_WRITE:
                /* reset the offset */
                aHandle->offset = 0UL;
                aHandle->state = RFS_STATE_WRITE;
                break;
            case RFS_MODE_APPEND:
                /* reset the offset */
                aHandle->offset = RFS_OFFSET_FILE_END;
                aHandle->state = RFS_STATE_WRITE;
                break;
            default:
                aHandle->state = RFS_STATE_RESET;
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
        }
    }

    return retVal;
}

/**
    Detailed design of RFS_Close

    @trace #BRCM_SWARCH_RFS_CLOSE_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else aHandle->state is RFS_STATE_RESET
        ret = ERR_INVAL_PARAMS;
    else
        memset the aHandle
        aHandle->state = RFS_STATE_RESET
    return ret
    @endcode
*/
int32_t RFS_Close(RFS_FileHandleType *const aHandle)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL == aHandle) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RFS_STATE_RESET == aHandle->state) {
        /* File was not opened */
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        BCM_MemSet(aHandle,0U,sizeof(RFS_FileHandleType));
        aHandle->state = RFS_STATE_RESET;
    }

    return retVal;
}

/**
    Detailed design of RFS_Read

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else aHandle->state is not RFS_STATE_READ or aHandle->hdr is NULL
        ret = ERR_INVAL_PARAMS;
    else
        RFS_MsgReadType msg;
        msg.buffer = aBuffer;
        BCM_MemCpy(msg.params.fileName,aHandle->fileName,RFS_MAX_FILENAME_LEN);
        msg.params.inLen = aLen;
        msg.params.offset = aHandle->offset;
        msg.params.errorStatus = BCM_ERR_OK;
        msg.params.actLen = 0UL;
        ret = MSGQ_SendMsg(&RFS_READQ, (void *)&msg, aClientMask, &aHandle->hdr);
    return ret
    @endcode
*/
int32_t RFS_Read(RFS_FileHandleType *const aHandle,
                 uint8_t *const aBuffer,
                 const uint32_t aLen,
                 const uint32_t aClientMask)
{
    int32_t retVal = BCM_ERR_OK;

    if ((NULL == aHandle) ||
        (NULL == aBuffer) ||
        (0UL == aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((RFS_STATE_READ != aHandle->state) ||
               (NULL != aHandle->hdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        RFS_MsgReadType msg;
        /* construct the request */
        msg.buffer = aBuffer;
        BCM_MemCpy(msg.params.fileName,aHandle->fileName,RFS_MAX_FILENAME_LEN);
        msg.params.inLen = aLen;
        msg.params.offset = aHandle->offset;
        msg.params.errorStatus = BCM_ERR_UNKNOWN;
        msg.params.actLen = 0UL;

        retVal = MSGQ_SendMsg(&RFS_READQ, (void *)&msg, aClientMask, &aHandle->hdr);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
        }
    }

    return retVal;
}

/**
    Detailed design of RFS_Write

    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else aHandle->state is not RFS_STATE_WRITE or aHandle->hdr is NULL
        ret = ERR_INVAL_PARAMS;
    else
        RFS_MsgWriteType msg;
        msg.buffer = aBuffer;
        BCM_MemCpy(msg.params.fileName,aHandle->fileName,RFS_MAX_FILENAME_LEN);
        msg.params.inLen = aLen;
        msg.params.offset = aHandle->offset;
        msg.params.errorStatus = BCM_ERR_OK;
        msg.params.actLen = 0UL;

        ret = MSGQ_SendMsg(&RFS_WRITEQ, (void *)&msg, aClientMask, &aHandle->hdr);
    return ret
    @endcode
*/
int32_t RFS_Write(RFS_FileHandleType *const aHandle,
                  const uint8_t *const aBuffer,
                  const uint32_t aLen,
                  const uint32_t aClientMask)
{
    int32_t retVal = BCM_ERR_OK;

    if ((NULL == aHandle) ||
        (NULL == aBuffer) ||
        (0UL == aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((RFS_STATE_WRITE != aHandle->state) ||
               (NULL != aHandle->hdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        RFS_MsgWriteType msg;
        /* construct the request */
        msg.buffer = aBuffer;
        BCM_MemCpy(msg.params.fileName,aHandle->fileName,RFS_MAX_FILENAME_LEN);
        msg.params.inLen = aLen;
        msg.params.offset = aHandle->offset;
        msg.params.errorStatus = BCM_ERR_UNKNOWN;
        msg.params.actLen = 0UL;

        retVal = MSGQ_SendMsg(&RFS_WRITEQ, (void *)&msg, aClientMask, &aHandle->hdr);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
        }
    }

    return retVal;
}

/**
    Detailed design of RFS_Seek

    @trace #BRCM_SWARCH_RFS_SEEK_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else aHandle->state is RFS_STATE_RESET
        ret = ERR_INVAL_PARAMS
    else
        Copy file name to context
        if aSeekType is RFS_SEEK_SET
            aHandle->offset = aLen
        else if aSeekType is RFS_SEEK_CUR
            if aHandle->offset is not RFS_OFFSET_FILE_END
                aHandle->offset += aLen
        else if aSeekType is RFS_SEEK_END
            aHandle->offset = RFS_OFFSET_FILE_END
        else
            ret = BCM_ERR_INVAL_PARAMS
    return ret
    @endcode
*/
int32_t RFS_Seek(RFS_FileHandleType *const aHandle,
                 const RFS_SeekType aSeekType,
                 const uint32_t aLen)
{
    int32_t retVal = BCM_ERR_BUSY;
    if (NULL == aHandle) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RFS_STATE_RESET == aHandle->state) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        retVal = BCM_ERR_OK;
        switch(aSeekType) {
            case RFS_SEEK_SET:
                aHandle->offset = aLen;
                break;
            case RFS_SEEK_CUR:
                if (RFS_OFFSET_FILE_END != aHandle->offset) {
                    aHandle->offset += aLen;
                }
                break;
            case RFS_SEEK_END:
                aHandle->offset = RFS_OFFSET_FILE_END;
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
        }
    }

    return retVal;
}

/**
    Detailed design of RFS_GetStatus

    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else aHandle->state is RFS_STATE_RESET or aHandle->hdr is NULL
        ret = ERR_INVAL_PARAMS;
    else if aHandle->state is RFS_STATE_READ
        RFS_MsgWriteType msg;
        ret = MSGQ_RecvMsg(&RFS_WRITEQ, aHandle->hdr, (void *)&msg);
        if  ret is ERR_OK
            aHandle->hdr = NULL
            if msg.params.errorStatus is ERR_OK
                *aLen = msg.params.actLen
                if aHandle->offset is not RFS_OFFSET_FILE_END
                    aHandle->offset += msg.params.actLen
            else
                *aLen = 0
            if msg.params.errorStatus is ERR_NOMEM
                or msg.params.errorStatus is ERR_TIME_OUT
                or msg.params.errorStatus is ERR_OK
                ret = msg.params.errorStatus
            else
                ret = ERR_UNKNOWN

        else
            ret = ERR_BUSY;
    else
        RFS_MsgReadType msg;
        retVal = MSGQ_RecvMsg(&RFS_READQ, aHandle->hdr, (void *)&msg);
        if  ret is ERR_OK
            aHandle->hdr = NULL
            if msg.params.errorStatus is ERR_OK
                *aLen = msg.params.actLen
                if aHandle->offset is not RFS_OFFSET_FILE_END
                    aHandle->offset += msg.params.actLen
            else
                *aLen = 0
            if msg.params.errorStatus is ERR_TIME_OUT
                or msg.params.errorStatus is ERR_OK
                ret = msg.params.errorStatus
            else
                ret = ERR_UNKNOWN

        else
            ret = ERR_BUSY;
    return ret
    @endcode
*/
int32_t RFS_GetStatus(RFS_FileHandleType *const aHandle,
                      uint32_t *const aLen)
{
    int32_t retVal = BCM_ERR_BUSY;
    if ((NULL == aHandle) ||
        (NULL == aLen)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if ((RFS_STATE_RESET == aHandle->state) ||
               (NULL == aHandle->hdr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (RFS_STATE_WRITE == aHandle->state) {
        RFS_MsgWriteType msg;
        retVal = MSGQ_RecvMsg(&RFS_WRITEQ, aHandle->hdr, (void *)&msg);
        if (BCM_ERR_OK == retVal) {
            aHandle->hdr = NULL;
            if (BCM_ERR_OK == msg.params.errorStatus) {
                if (RFS_OFFSET_FILE_END != aHandle->offset) {
                    aHandle->offset += msg.params.actLen;
                }
                *aLen = msg.params.actLen;
            } else {
                *aLen = 0UL;
            }

            if ((BCM_ERR_OK == msg.params.errorStatus) ||
                (BCM_ERR_TIME_OUT == msg.params.errorStatus) ||
                (BCM_ERR_NOT_FOUND == msg.params.errorStatus)) {
                retVal = msg.params.errorStatus;
            } else {
                retVal = BCM_ERR_UNKNOWN;
                RFS_ReportError(BRCM_SWDSGN_RFS_GET_STATUS_PROC, msg.params.errorStatus, __LINE__, 0UL, 0UL, 0UL);
            }
        } else {
            retVal = BCM_ERR_BUSY;
        }
    } else {
        /* RFS_STATE_READ */
        RFS_MsgReadType msg;
        retVal = MSGQ_RecvMsg(&RFS_READQ, aHandle->hdr, (void *)&msg);
        if (BCM_ERR_OK == retVal) {
            aHandle->hdr = NULL;
            if (BCM_ERR_OK == msg.params.errorStatus) {
                aHandle->offset += msg.params.actLen;
                *aLen = msg.params.actLen;
            } else {
                *aLen = 0UL;
            }
            if ((BCM_ERR_OK == msg.params.errorStatus) ||
                (BCM_ERR_TIME_OUT == msg.params.errorStatus) ||
                (BCM_ERR_NOMEM == msg.params.errorStatus) ||
                (BCM_ERR_NOT_FOUND == msg.params.errorStatus)) {
                retVal = msg.params.errorStatus;
            } else {
                retVal = BCM_ERR_UNKNOWN;
                RFS_ReportError(BRCM_SWDSGN_RFS_GET_STATUS_PROC, msg.params.errorStatus, __LINE__, 0UL, 0UL, 0UL);
            }

            aHandle->hdr = NULL;
        } else {
            retVal = BCM_ERR_BUSY;
        }
    }

    return retVal;
}
/** @brief RFS Alarm call back

    Call back for RFS Alarm. This sets the event to RFS task for time outs

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations None

    @code{.unparsed}
        if RFS_ReadContext.state is TFTP_STATE_READ or RFS_WriteContext.state is TFTP_STATE_WRITE
            SetEvent(RFS_Task, RFS_ALARM_EVENT_ID);
    @endcode

*/
void RFS_AlarmCb(void)
{
    if ((TFTP_STATE_READ == RFS_ReadContext.state) ||
        (TFTP_STATE_WRITE == RFS_WriteContext.state)) {
        uint8_t erikaRet;
        erikaRet = SetEvent(RFS_Task, RFS_ALARM_EVENT_ID);
        if (E_OK != erikaRet) {
            RFS_ReportError(BRCM_SWDSGN_RFS_ALARM_CB_PROC, (int32_t)erikaRet, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @brief Task for RFS

    Task for RFS

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      void

    @return void

    @post None

    @trace #BRCM_SWARCH_RFS_READ_PROC
    @trace #BRCM_SWARCH_RFS_WRITE_PROC
    @trace #BRCM_SWARCH_RFS_GET_STATUS_PROC
    @trace #BRCM_SWREQ_RFS_CLIENT

    @limitations Never terminates

    @code{.unparsed}
        TFTP_Init(&RFS_ReadContext,...)
        TFTP_Init(&RFS_WriteContext,...)
        loop forever
            BCM_WaitEvent(INET_EVENT|READQ_EVENT|WRITEQ_EVENT|ALARM_EVENT)
            BCM_GetEvent(RFS_Task,&mask)
            BCM_ClearEvent(mask)

            if mask & INET_EVENT
                RFS_ProcessRead()
                RFS_ProcessWrite()
            RFS_StartRead()
            RFS_StartWrite()
    @endcode

*/
TASK(RFS_Task)
{
    int32_t retVal;
    int32_t ret;
    BCM_EventMaskType mask = 0UL;
    TFTP_ConfigType readCfg = {BCM_RFS_ID,
                               INET_UDP_PAYLOAD_ID0,
                               INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL,
                               300UL};
    TFTP_ConfigType writeCfg = {BCM_RFS_ID,
                               INET_UDP_PAYLOAD_ID1,
                               INET_CONVERT_IP_ADDRESS((192U), (168U), (10U), (1U)),
                               3UL,
                               300UL};

    /* initialize TFTP contexts */
    retVal  = TFTP_Init(&RFS_ReadContext, &readCfg);
    if (BCM_ERR_OK != retVal) {
        RFS_ReportError(BRCM_SWDSGN_RFS_IL_TASK_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }

    retVal  = TFTP_Init(&RFS_WriteContext, &writeCfg);
    if (BCM_ERR_OK != retVal) {
        RFS_ReportError(BRCM_SWDSGN_RFS_IL_TASK_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }

    do {
        ret = BCM_WaitEvent(RFS_INET_EVENT_ID | RFS_READQ_EVENT_ID | RFS_WRITEQ_EVENT_ID | RFS_ALARM_EVENT_ID);
        if (BCM_ERR_OK != ret) {
            RFS_ReportError(BRCM_SWDSGN_RFS_IL_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(RFS_Task,&mask);
        if (BCM_ERR_OK != ret) {
            RFS_ReportError(BRCM_SWDSGN_RFS_IL_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            RFS_ReportError(BRCM_SWDSGN_RFS_IL_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        if (0UL != (mask & (RFS_INET_EVENT_ID | RFS_READQ_EVENT_ID | RFS_ALARM_EVENT_ID))) {
            RFS_ReadProcess();
        }

        if (0UL != (mask & (RFS_INET_EVENT_ID | RFS_WRITEQ_EVENT_ID | RFS_ALARM_EVENT_ID))) {
            RFS_WriteProcess();
        }

    } while(1UL);

}
/** @} */


