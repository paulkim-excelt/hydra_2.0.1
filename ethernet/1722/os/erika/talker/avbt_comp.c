/*****************************************************************************
Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/**
  @defgroup grp_avbt_impl Implementation
  @ingroup grp_avbt

  @addtogroup grp_avbt_impl
  @{
  @file avbt_comp.c
  @brief Data structures and APIs of the AVBT Component
  This file provides the implementation for the task activities of the
  AVBT Component.
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <component.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <system.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#include "avb_talker.h"
#include "avb_core.h"
#include "avbt_comp.h"

/**
    @name AVBT Component Design IDs
    @{
    @brief Design IDs for AVBT component
*/
#define BRCM_SWDSGN_AVBT_NUM_INP_PORTS_GLOBAL            (0xC1U) /**< @brief #AVBT_MAX_NUM_INPUT_PORTS */
#define BRCM_SWDSGN_AVBT_INIT_H264_BUF_INFO_GLOBAL       (0xC2U) /**< @brief #AVBT_INIT_H264_BUF_INFO */
#define BRCM_SWDSGN_AVBT_INIT_PCM_BUF_INFO_GLOBAL        (0xC3U) /**< @brief #AVBT_INIT_PCM_BUF_INFO */
#define BRCM_SWDSGN_AVBT_INPORT0_GLOBAL                  (0xC4U) /**< @brief #AVBT_InPort0 */
#define BRCM_SWDSGN_AVBT_INPORT1_GLOBAL                  (0xC5U) /**< @brief #AVBT_InPort1 */
#define BRCM_SWDSGN_AVBT_INPORT2_GLOBAL                  (0xC6U) /**< @brief #AVBT_InPort2 */
#define BRCM_SWDSGN_AVBT_INPORT3_GLOBAL                  (0xC7U) /**< @brief #AVBT_InPort3 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQIDX0_GLOBAL           (0xC8U) /**< @brief #AVBT_circQFillQIdx0 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQIDX1_GLOBAL           (0xC9U) /**< @brief #AVBT_circQFillQIdx1 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQIDX2_GLOBAL           (0xCAU) /**< @brief #AVBT_circQFillQIdx2 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQIDX3_GLOBAL           (0xCBU) /**< @brief #AVBT_circQFillQIdx3 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQBUF0_GLOBAL           (0xCCU) /**< @brief #AVBT_circQFillQBuf0 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQBUF1_GLOBAL           (0xCDU) /**< @brief #AVBT_circQFillQBuf1 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQBUF2_GLOBAL           (0xCEU) /**< @brief #AVBT_circQFillQBuf2 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQBUF3_GLOBAL           (0xCFU) /**< @brief #AVBT_circQFillQBuf3 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQ0_GLOBAL              (0xD0U) /**< @brief #AVBT_CircQFillQ0 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQ1_GLOBAL              (0xD1U) /**< @brief #AVBT_CircQFillQ1 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQ2_GLOBAL              (0xD2U) /**< @brief #AVBT_CircQFillQ2 */
#define BRCM_SWDSGN_AVBT_CIRCQFILLQ3_GLOBAL              (0xD3U) /**< @brief #AVBT_CircQFillQ3 */
#define BRCM_SWDSGN_AVBT_CONTEXT_GLOBAL                  (0xD4U) /**< @brief #AVBT_Context */
#define BRCM_SWDSGN_AVBT_PORTDATA_GLOBAL                 (0xD5U) /**< @brief #AVBT_PortData */
#define BRCM_SWDSGN_AVBT_INPORT_GLOBAL                   (0xD6U) /**< @brief #AVBT_InPort */
#define BRCM_SWDSGN_AVBT_FILLQ_GLOBAL                    (0xD7U) /**< @brief #AVBT_FillQ */
#define BRCM_SWDSGN_AVBT_SENDCOMMAND_PROC                (0xD8U) /**< @brief #AVBT_SendCommand */
#define BRCM_SWDSGN_AVBT_RECVCMDRESPONSE_PROC            (0xD9U) /**< @brief #AVBT_RecvCmdResponse */
#define BRCM_SWDSGN_AVBT_GETMEDIAOUTPORT_PROC            (0xDBU) /**< @brief #AVBT_GetMediaOutPort */
#define BRCM_SWDSGN_AVBT_PRINTSTATISTICS_PROC            (0xDDU) /**< @brief #AVBT_PrintStatistics */
#define BRCM_SWDSGN_AVBT_RXPKTINDICATION_PROC            (0xDEU) /**< @brief #AVBT_RxPktIndication */
#define BRCM_SWDSGN_AVBT_TXPKTCONFIRMATION_PROC          (0xDFU) /**< @brief #AVBT_TxPktConfirmation */
#define BRCM_SWDSGN_AVBT_BUFAVAILINDICATION_PROC         (0xE0U) /**< @brief #AVBT_BufAvailIndication */
#define BRCM_SWDSGN_AVBT_HANDLEBUFQUEUING_PROC           (0xE1U) /**< @brief #AVBT_HandleBufQueuing  */
#define BRCM_SWDSGN_AVBT_FLUSHTXBUFFERS_PROC             (0xE2U) /**< @brief #AVBT_FlushTxBuffers */
#define BRCM_SWDSGN_AVBT_FILLBUFFERDONEPROCESS_PROC      (0xE3U) /**< @brief #AVBT_FillBufferDoneProcess */
#define BRCM_SWDSGN_AVBT_CIRCQDATAPROCESS_PROC           (0xE4U) /**< @brief #AVBT_CircQDataProcess */
#define BRCM_SWDSGN_AVBT_REPORTERROR_PROC                (0xE5U) /**< @brief #AVBT_ReportError */
#define BRCM_SWDSGN_AVBT_RESETSTATEHANDLER_PROC          (0xE6U) /**< @brief #AVBT_ResetStateHandler */
#define BRCM_SWDSGN_AVBT_INITSTATEHANDLER_PROC           (0xE7U) /**< @brief #AVBT_InitStateHandler */
#define BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC          (0xE8U) /**< @brief #AVBT_ReadyStateHandler */
#define BRCM_SWDSGN_AVBT_RUNSTATECMDHANDLER_PROC         (0xE9U) /**< @brief #AVBT_RunStateCmdHandler */
#define BRCM_SWDSGN_AVBT_RUNSTATEINPUTEVENTHANDLER_PROC  (0xEAU) /**< @brief #AVBT_RunStateInputEventHandler */
#define BRCM_SWDSGN_AVBT_RUNSTATEHANDLER_PROC            (0xEBU) /**< @brief #AVBT_RunStateHandler */
#define BRCM_SWDSGN_AVBT_GETMEDIAINPORT_PROC             (0xF0U) /**< @brief #AVBT_GetMediaInPort */
#define BRCM_SWDSGN_AVBT_COMPFNTABLE_GLOBAL              (0xF1U) /**< @brief #AVBT_CompFnTable */
#define BRCM_SWDSGN_AVBT_GETCOMPFNTABLE_GLOBAL           (0xF2U) /**< @brief #AVBT_GetCompFnTable */
#define BRCM_SWDSGN_AVBT_CONFIGDATA_TYPE                 (0xF3U) /**< @brief #AVBT_ConfigDataType */
#define BRCM_SWDSGN_AVBT_ERIKA_MACRO                     (0xF4U) /**< @brief #AVBT_ERIKA_TASK */
#define BRCM_SWDSGN_AVBT_CMDQLIST_GLOBAL                 (0xF5U) /**< @brief #AVBT_CmdQList */
#define BRCM_SWDSGN_AVBT_GETMESSAGE_COMMANDQLIST_PROC    (0xF6U) /**< @brief #AVBT_GetMessage_CommandQList */
#define BRCM_SWDSGN_AVBT_DEFINE_MSGDHRQ_GLOBAL           (0xF7U) /**< @brief #AVBT_CommandHdrQ */
#define BRCM_SWDSGN_AVBT_CMDQ_GLOBAL                     (0xF8U) /**< @brief #AVBT_CmdQ */
#define BRCM_SWDSGN_AVBT_ERIKA_TASK_PART_PROC            (0xF9U) /**< @brief #AVBT_Task */

/** @} */

#ifndef AVBT_MAX_NUM_INPUT_PORTS
/**
    @brief Macro for default number of input ports

    This macro defines the value for the count of ports to be enabled
    in case the application does not set this macro.

    @trace #BRCM_SWARCH_AVBT_INPUTPORTNUM_TYPE
    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_MAX_NUM_INPUT_PORTS (1UL)
#endif

/**
    @brief Macro to initialize the port CircQ - H264 payload entry

    This macro initializes the members of port CircQ structure before
    invoking the MSYS_FillBuffer API to transfer the empty buffer to
    upstream component. Appropriate H264 video format is updated
    which gets validated by the connected component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_INIT_H264_BUF_INFO(idxVal, dataPtr, portIdxVal)  \
{                                                             \
    .bufIdx = (idxVal),                                       \
    .timestamp = (0ULL),                                      \
    .syncStart = (0UL),                                       \
    .syncEnd = (0UL),                                         \
    .frameEnd = (0U),                                         \
    .numChannels = (1UL),                                     \
    .streamInfo[0] = (0U),                                    \
    .buffer[0].data = (uint8_t*)(dataPtr),                    \
    .buffer[0].size = (AVBT_PORT_BUF_SIZE),                   \
    .buffer[0].attribs[AVBT_PORT_IDX_OFFSET] = (portIdxVal),  \
}

/**
    @brief Macro to initialize the port CircQ - PCM payload entry

    This macro initializes the members of port CircQ structure before
    invoking the MSYS_FillBuffer API to transfer the empty buffer to
    upstream component. Appropriate PCM format value is updated
    which gets validated by the connected component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_INIT_PCM_BUF_INFO(idxVal, dataPtr, portIdxVal)   \
{                                                             \
    .bufIdx = (idxVal),                                       \
    .timestamp = (0ULL),                                      \
    .syncStart = (0UL),                                       \
    .syncEnd = (0UL),                                         \
    .numChannels = (1UL),                                     \
    .format = (MSYS_BUFFER_FORMAT_AUDIO_PCM),                 \
    .streamInfo[0] = (0U),                                    \
    .buffer[0].data = (uint8_t*)(dataPtr),                    \
    .buffer[0].size = (AVBT_PORT_BUF_SIZE),                   \
    .buffer[0].attribs[AVBT_PORT_IDX_OFFSET] = (portIdxVal),  \
    .buffer[0].channelType = (MSYS_CHANNEL_TYPE_LR),          \
}

/**
    @name Erika Task and Events for AVBT Component
    @{
    @brief Erika task ID and event values for the component

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
 */
#define AVBT_ERIKA_TASK                    AVBT_Task    /**< @brief Task for AVB Component */
#define AVBT_ERIKA_EVENT_CMD_PROCESS       SystemEvent0 /**< @brief Signal Command */
#define AVBT_ERIKA_EVENT_DATA_TXBUF_Q      SystemEvent1 /**< @brief Queue Tx buffers based on TxConf */
#define AVBT_ERIKA_EVENT_INPPORT0_PROCESS  SystemEvent4 /**< @brief Input port 0 Data Process */
#define AVBT_ERIKA_EVENT_INPPORT1_PROCESS  SystemEvent5 /**< @brief Input port 1 Data Process */
#define AVBT_ERIKA_EVENT_INPPORT2_PROCESS  SystemEvent6 /**< @brief Input port 2 Data Process */
#define AVBT_ERIKA_EVENT_INPPORT3_PROCESS  SystemEvent7 /**< @brief Input port 3 Data Process */
/** @} */

/**
    @brief Memory for Command Q messages

    Memory for storing the commands in the AVBT component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
MSYS_CmdType AVBT_CmdQList[MSGQ_SIZE];

/**
    @brief Get command from command queue

    API to get the MSGQ_MsgHdrType from CommandQList.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   idx   The index of message queue from which MSG needs to
                       be got
    @retval  Pointer to a message in the command queue

    @post None

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

    @limitations None

    @code{.c}
    check if idx is less than #MSGQ_SIZE

    return the command queue entry from #AVBT_CmdQList
    @endcode
*/
static void* AVBT_GetMessage_CommandQList (uint32_t idx)
{
    void *avbPtr = NULL;

    if (idx < MSGQ_SIZE) {
        avbPtr = (void *)(&AVBT_CmdQList[idx]);
    } else {
        avbPtr = NULL;
    }

    return avbPtr;
}

/**
    @brief Message Header Q for Commands from Framework

    Message header Q to manage Commands to the AVBT component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
MSGQ_DEFINE_HDRQ(AVBT_CommandHdrQ);

/**
    @brief Message Q for Commands from Framework

    Message Q to receive and store the Commands to the AVBT component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
const MSGQ_Type AVBT_CmdQ = MSGQ_INIT(AVBT_ERIKA_TASK,
                            AVBT_ERIKA_EVENT_CMD_PROCESS,
                            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &AVBT_CommandHdrQ,
                            AVBT_GetMessage_CommandQList);

/**
    @brief Global context for AVB Talker Component

    The data structure of the AVBT component which maintains the
    component state, configuration and statistics.

    @trace #BRCM_SWARCH_AVBT_STACKHANDLE_TYPE
    @trace #BRCM_SWREQ_AVBT_INIT
*/
AVBT_StackHandleType AVBT_Context = {0};

#if (AVBT_INPUTPORTNUM_0 < AVBT_MAX_NUM_INPUT_PORTS)
/**
    @brief AVBT Input port 0 Variables

    The data structure for input port 0 of the AVBT component
    which is used by the framework for port handling. The memory for
    port circular queue which is internally managed by the framework
    is created here.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
MSYS_DECLARE_IN_PORT(AVBT_InPort0, 0UL, MSYS_COMP_ID_AVBT, SystemEvent4,
                     InPort_RW0, AVBT_FillBufferDoneProcess);

/**
    @brief AVBT Input port 0 Circular queue index

    The data structure for input port 0 circular queue management.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
CIRCQ_INDEX_DEFINE(AVBT_circQFillQIdx0,AVBT_COMP_CIRCQ_SIZE);

/**
    @brief AVBT Input port 0 Circular queue memory

    The data memory pool for input port 0 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
static AVBT_BufInfoType AVBT_circQFillQBuf0[AVBT_COMP_CIRCQ_SIZE];

/**
    @brief AVBT Input port 0 Circular queue hanlde

    The handle of input port 0 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
static const CIRCQ_Type AVBT_CircQFillQ0 =
        CIRCQ_INIT(&AVBT_circQFillQIdx0, AVBT_COMP_CIRCQ_SIZE, sizeof(AVBT_BufInfoType),
                  (char *)AVBT_circQFillQBuf0, CIRCQ_OVERWRITE_DISABLE);

#endif

#if (AVBT_INPUTPORTNUM_1 < AVBT_MAX_NUM_INPUT_PORTS)
/**
    @brief AVBT Input port 1 Variables

    The data structure for input port 1 of the AVBT component
    which is used by the framework for port handling. The memory for
    port circular queue which is internally managed by the framework
    is created here.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
MSYS_DECLARE_IN_PORT(AVBT_InPort1, 1UL, MSYS_COMP_ID_AVBT, SystemEvent5,
                     InPort_RW1, AVBT_FillBufferDoneProcess);

/**
    @brief AVBT Input port 1 Circular queue index

    The data structure for input port 1 circular queue management.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
CIRCQ_INDEX_DEFINE(AVBT_circQFillQIdx1,AVBT_COMP_CIRCQ_SIZE);

/**
    @brief AVBT Input port 1 Circular queue memory

    The data memory pool for input port 1 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static AVBT_BufInfoType AVBT_circQFillQBuf1[AVBT_COMP_CIRCQ_SIZE];

/**
    @brief AVBT Input port 1 Circular queue hanlde

    The handle of input port 1 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static const CIRCQ_Type AVBT_CircQFillQ1 =
        CIRCQ_INIT(&AVBT_circQFillQIdx1, AVBT_COMP_CIRCQ_SIZE,sizeof(AVBT_BufInfoType),
                  (char *)AVBT_circQFillQBuf1,CIRCQ_OVERWRITE_DISABLE);
#endif

#if (AVBT_INPUTPORTNUM_2 < AVBT_MAX_NUM_INPUT_PORTS)
/**
    @brief AVBT Input port 2 Variables

    The data structure for input port 2 of the AVBT component
    which is used by the framework for port handling. The memory for
    port circular queue which is internally managed by the framework
    is created here.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
MSYS_DECLARE_IN_PORT(AVBT_InPort2, 2UL, MSYS_COMP_ID_AVBT, SystemEvent6,
                     InPort_RW2, AVBT_FillBufferDoneProcess);

/**
    @brief AVBT Input port 2 Circular queue index

    The data structure for input port 2 circular queue management.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
CIRCQ_INDEX_DEFINE(AVBT_circQFillQIdx2,AVBT_COMP_CIRCQ_SIZE);

/**
    @brief AVBT Input port 2 Circular queue memory

    The data memory pool for input port 2 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static AVBT_BufInfoType AVBT_circQFillQBuf2[AVBT_COMP_CIRCQ_SIZE];

/**
    @brief AVBT Input port 2 Circular queue hanlde

    The handle of input port 2 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static const CIRCQ_Type AVBT_CircQFillQ2 =
        CIRCQ_INIT(&AVBT_circQFillQIdx2, AVBT_COMP_CIRCQ_SIZE,sizeof(AVBT_BufInfoType),
                  (char *)AVBT_circQFillQBuf2,CIRCQ_OVERWRITE_DISABLE);
#endif

#if (AVBT_INPUTPORTNUM_3 < AVBT_MAX_NUM_INPUT_PORTS)
/**
    @brief AVBT Input port 3 Variables

    The data structure for input port 3 of the AVBT component
    which is used by the framework for port handling. The memory for
    port circular queue which is internally managed by the framework
    is created here.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
MSYS_DECLARE_IN_PORT(AVBT_InPort3, 3UL, MSYS_COMP_ID_AVBT, SystemEvent7,
                     InPort_RW3, AVBT_FillBufferDoneProcess);

/**
    @brief AVBT Input port 3 Circular queue index

    The data structure for input port 3 circular queue management.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
CIRCQ_INDEX_DEFINE(AVBT_circQFillQIdx3,AVBT_COMP_CIRCQ_SIZE);

/**
    @brief AVBT Input port 3 Circular queue memory

    The data memory pool for input port 3 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static AVBT_BufInfoType AVBT_circQFillQBuf3[AVBT_COMP_CIRCQ_SIZE];

/**
    @brief AVBT Input port 3 Circular queue hanlde

    The handle of input port 3 circular queue.

    @trace #BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT

*/
static const CIRCQ_Type AVBT_CircQFillQ3 =
        CIRCQ_INIT(&AVBT_circQFillQIdx3, AVBT_COMP_CIRCQ_SIZE,sizeof(AVBT_BufInfoType),
                  (char *)AVBT_circQFillQBuf3,CIRCQ_OVERWRITE_DISABLE);
#endif

/**
    @brief Input port buffer memory pool

    The memory pool for the AVBT component ports which are statically
    allocated based on the number of input ports created for
    the component.

    @trace #BRCM_SWARCH_AVBT_PORT_BUFSIZE_MACRO
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWARCH_AVBT_NUM_PORT_BUFFERS_MACRO
    @trace #BRCM_SWREQ_AVBT_INIT
*/
static uint8_t AVBT_PortData[AVBT_MAX_NUM_INPUT_PORTS]
                            [AVBT_PORT_NUM_BUFFERS]
                            [AVBT_PORT_BUF_SIZE]
                            COMP_SECTION(".data.avb_talker") COMP_ALIGN(256);

/**
    @brief Array of input port structures

    Every port of AVBT is referenced by the media framework using
    the port handle. This is an array of all input port structures
    of the AVBT component.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
static const MSYS_InPortType* AVBT_InPort[AVBT_MAX_NUM_INPUT_PORTS] = {
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_0)
    &AVBT_InPort0,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_1)
    &AVBT_InPort1,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_2)
    &AVBT_InPort2,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_3)
    &AVBT_InPort3,
#endif
};

/**
    @brief Array of Circular Queues used for component input ports

    Every port of AVBT uses a circular queue to recieve the filled
    port buffers from the upstream component. This is an array of all
    such input port circular Qs created for all enabled ports.

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_INIT
*/
static const CIRCQ_Type * AVBT_FillQ[AVBT_MAX_NUM_INPUT_PORTS] = {
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_0)
    &AVBT_CircQFillQ0,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_1)
    &AVBT_CircQFillQ1,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_2)
    &AVBT_CircQFillQ2,
#endif
#if (AVBT_MAX_NUM_INPUT_PORTS>AVBT_INPUTPORTNUM_3)
    &AVBT_CircQFillQ3,
#endif
};

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_INIT

   @code{.c}
   invoke #BCM_ReportError to log passed values
   @endcode
 */
static void AVBT_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                             uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_AVB_ID, 0U, aApiID,
                    aErr, 4UL, values);
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   Perform error check on input arguments

   If (command == MSYS_COMP_CMD_INIT)
       Activate component task

   Add command message to command queue

   Return error code returned by the message queue API
   @endcode
 */
static int32_t AVBT_SendCommand (const MSYS_CmdType * const cmd,
                                 const uint32_t clientMask,
                                 const MSGQ_MsgHdrType** const pHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    int32_t status = 0L;

    if ((NULL != cmd)
        && (MSYS_COMP_ID_AVBT == cmd->compID)
        && (NULL != pHdr)) {
        if (cmd->cmd == MSYS_COMP_CMD_INIT) {
            status = BCM_ActivateTask(AVBT_ERIKA_TASK);
        }
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&AVBT_CmdQ, (const void * const)cmd,
                              clientMask, pHdr);
    }

    if ((0L != status) || (BCM_ERR_OK != retVal)) {
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_SENDCOMMAND_PROC, retVal,
                         __LINE__, (uint32_t)status, 0UL, 0UL);
    }

    return retVal;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   Perform error check on input arguments

   Get the command status from command message queue

   Return error code and status as returned by the message queue API
   @endcode
 */
static int32_t AVBT_RecvCmdResponse (const MSGQ_MsgHdrType* const pHdr,
                                     MSYS_CmdType* const cmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != pHdr) && (NULL != cmd)) {
        retVal = MSGQ_RecvMsg(&AVBT_CmdQ, pHdr, (void* const) cmd);
    } else {
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RECVCMDRESPONSE_PROC, retVal,
                        (uint32_t) __LINE__, (uint32_t)cmd,
                        (uint32_t)pHdr, 0UL);
    }

    return retVal;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   return NULL
   @endcode
*/
static const MSYS_InPortType* AVBT_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *port = NULL;

    if (AVBT_MAX_NUM_INPUT_PORTS > aIndex) {
        port = AVBT_InPort[aIndex];
    } else {
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_GETMEDIAINPORT_PROC,
                         BCM_ERR_INVAL_PARAMS,
                         __LINE__, 0UL, 0UL, 0UL);
    }
    return port;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   return NULL
   @endcode
 */
static const MSYS_OutPortType* AVBT_GetMediaOutPort(const uint32_t aIndex)
{
    return NULL;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
static const MSYS_CompFnTblType AVBT_CompFnTable =
{
    AVBT_SendCommand,
    AVBT_RecvCmdResponse,
    AVBT_GetMediaOutPort,
    AVBT_GetMediaInPort,
};

/**
   @brief Function to return Component function table for component

   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
const MSYS_CompFnTblType* AVBT_GetCompFnTable(void)
{
    return &AVBT_CompFnTable;
};

/**
   @trace #BRCM_SWARCH_AVBT_RXPKTINDICATION_PROC
   @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

   @code{.c}
   Not used
   @endcode
 */
void AVBT_RxPktIndication (NIF_CntrlIDType aCtrlIdx,
                           const uint8_t *const aBuf,
                           uint16_t aLen,
                           const uint8_t *const aSrcMacAddr,
                           const uint8_t *const aDestMacAddr,
                           uint32_t aPriority)
{
    /* Rx data is applicable only for 1722 listener */
    return;
}

/**
   @trace #BRCM_SWARCH_AVBT_TXPKTCONFIRMATION_PROC
   @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

   @code{.c}
   Not used
   @endcode
 */
void AVBT_TxPktConfirmation (NIF_CntrlIDType aCtrlIdx,
                             uint32_t aBufIdx)
{
    uint32_t isSignalTask = 0UL;
    uint32_t portIdx;

#ifdef ENABLE_FQTSS
    /* Case when FQTSS is enabled, but stream reservation is set */
    /* as 0% which indicates no shaping. We need to rely on      */
    /* Tx confirmation callback for transmission.                */
    for (portIdx = 0UL;
         portIdx < AVBT_Context.numInpPort;
         portIdx++) {
        if((0UL == AVBT_Context.compCfg[portIdx].percentRsvdBW) &&
           (1UL == AVBT_Context.circQData[portIdx].isDataUnderProcess)){
            isSignalTask = 1UL;
            break;
        }
    }
#else
    /* This callback from ethernet is used only when FQTSS is   */
    /* not enabled. It is used as a trigger to inform the AVBT  */
    /* that ethernet Tx buffers are available. In case when     */
    /* FQTSS is enabled, the #AVBT_BufAvailIndication gets used */
    /* and the AVBT is notified only when buffer credits are    */
    /* available.                                               */
    for (portIdx = 0UL;
         portIdx < AVBT_Context.numInpPort;
         portIdx++) {
        if(1UL == AVBT_Context.circQData[portIdx].isDataUnderProcess){
            isSignalTask = 1UL;
            break;
        }
    }
#endif
    if(1UL == isSignalTask){
        (void)BCM_SetEvent(AVBT_ERIKA_TASK, AVBT_ERIKA_EVENT_DATA_TXBUF_Q);
    }
}

/**
   @trace #BRCM_SWARCH_AVBT_BUFAVAILINDICATION_PROC
   @trace #BRCM_SWREQ_AVBT_NIF_CALLBACK

   @code{.c}
   if AVBT state is #AVBT_COMPSTATE_START
        invoke #AVBT_SignalTxBufferQueuing to set event to AVBT task to process data
   @endcode
 */
void AVBT_BufAvailIndication(NIF_CntrlIDType aCtrlIdx)
{
    /* Signal an Event to queue Tx Buffers */
    if (AVBT_COMPSTATE_START == AVBT_Context.compState) {
        (void)BCM_SetEvent(AVBT_ERIKA_TASK, AVBT_ERIKA_EVENT_DATA_TXBUF_Q);
    }
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   update MSYS_BufferType structure for the passed buffer index

   ret = MSYS_FillBuffer API
   if ret has error
       increment stats
       log error

   return
   @endcode
 */
static void AVBT_HandleBufQueuing (uint32_t portIndex, uint32_t bufIndex)
{
    int32_t retVal = BCM_ERR_OK;

    /* Update the Media Output buffer params based on payload */
    if (AVB_PAYLFORMAT_PCM ==
            AVBT_Context.bufparam[portIndex].strCfgForHdr.payloadFormat) {

        const MSYS_BufferType mediaBuffer = AVBT_INIT_PCM_BUF_INFO(
                        bufIndex,
                        &(AVBT_PortData[portIndex][bufIndex][0U]),
                        portIndex);

        /* Queue the buffer to the peer component */
        retVal = MSYS_FillBuffer(AVBT_InPort[portIndex], &mediaBuffer);

    } else if (AVB_PAYLFORMAT_H264 ==
        AVBT_Context.bufparam[portIndex].strCfgForHdr.payloadFormat) {

        const MSYS_BufferType mediaBuffer = AVBT_INIT_H264_BUF_INFO(
                        bufIndex,
                        &(AVBT_PortData[portIndex][bufIndex][0U]),
                        portIndex);

        /* Queue the buffer to the peer component */
        retVal = MSYS_FillBuffer(AVBT_InPort[portIndex], &mediaBuffer);

    } else {
        /* Nothing */
    }

    if (BCM_ERR_OK != retVal) {
        /* Track the number of MSYS_FillBuffer API failures */
        AVBT_Context.talkerStats[portIndex].fillBufFailCount++;
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_HANDLEBUFQUEUING_PROC, retVal,
                         __LINE__, 0UL, 0UL, 0UL);
    }

    return;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   ret = MSYS_InPortCancelAll API for #portIndex
   if ret has error
       log error

   ret = reset the port circular queue
   if ret has error
       log error

   return
   @endcode
 */
static void AVBT_FlushTxBuffers (uint32_t portIndex)
{
    int32_t retVal = BCM_ERR_OK;

    /* Get back all Tx Buffers from peer component */
    retVal = MSYS_InPortCancelAll (AVBT_InPort[portIndex]);
    if((BCM_ERR_OK != retVal) && (BCM_ERR_CANCELLED != retVal)){
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_FLUSHTXBUFFERS_PROC, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

    /* Empty the circular Q */
    retVal = CIRCQ_Reset(AVBT_FillQ[portIndex]);
    if(BCM_ERR_OK != retVal) {
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_FLUSHTXBUFFERS_PROC, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
    AVBT_Context.isDataAvailable[portIndex] = 0UL;

    return;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_INIT

   @code{.c}

   if aMediaBuffer is valid
        populate AVBT_BufInfoType object from aMediaBuffer
        ret = Add AVBT_BufInfoType object to input port circQ
        if ret has error
            log error
        else
            update aMediaBuffer status as BCM_ERR_OK
   else
        update status as BCM_ERR_INVAL_PARAMS
   return
   @endcode
 */
void AVBT_FillBufferDoneProcess (MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t portindex = 0UL;

    if ((NULL != aMediaBuffer) &&
        (NULL != aMediaBuffer->buffer[0].data)){

        MSYS_ChannelBufferType *mediaBuffer = &aMediaBuffer->buffer[0];
        AVBT_BufInfoType qBuf = {0};

        /* update the buffer with incoming data details */
        qBuf.bufIdx    = aMediaBuffer->bufIdx;
        qBuf.procLen   = 0UL;
        qBuf.actualLen = aMediaBuffer->buffer[0].size;
        qBuf.timestamp = aMediaBuffer->timestamp;
        qBuf.syncStart = aMediaBuffer->syncStart;
        qBuf.syncEnd   = aMediaBuffer->syncEnd;
        qBuf.frameEnd   = aMediaBuffer->frameEnd;
        qBuf.isDataUnderProcess = 1UL;
        qBuf.strInfo   = (uint32_t)
               aMediaBuffer->streamInfo[MSYS_STREAM_INFO_0];
        portindex = mediaBuffer->attribs[AVBT_PORT_IDX_OFFSET];

        /* Push this buffer into fill port Q to be processed later */
        retVal = CIRCQ_Push(AVBT_FillQ[portindex],
                           (char *)&qBuf,1UL);
        if(BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_FILLBUFFERDONEPROCESS_PROC,
                             retVal, (uint32_t)__LINE__,
                             portindex, 0UL, 0UL);
        }
        AVBT_Context.isDataAvailable[portindex] = 1UL;

        /* Update buffer processing status */
        aMediaBuffer->response = BCM_ERR_OK;

        } else {
            if (NULL != aMediaBuffer) {
                /* Update buffer processing status */
                aMediaBuffer->response = BCM_ERR_INVAL_PARAMS;
            }
        }

    return;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   Copy the input buffer params to library stream config structure #AVB_1722TlkrBufParamsType

   ret = #NIF_GetTxBuffer of #AVB_AVTP_BUFFER_SIZE bytes
   if ret has error
       increment stats
           outOfBuffers = TRUE
           goto error

   if the ethernet Tx buffer returned is less than #AVBT_MIN_ETH_BUF_SIZE
       return buffer back using #NIF_Send
       increment stats
       goto error

   fragmentSize = ethDataBufSize - 32UL for 1722 header

   if payload is H264 Video
       if payload is start of slice
           strip the NAL start code

       if #remainingPayLen is (#fragmentSize + 1byte)
            #fragmentSize = #fragmentSize - 40 bytes
       else if #fragmentSize is greater than #remainingPayLen
            addFUAHdr = 0
       else
            addFUAHdr = 1

       if addFUAHdr is set
           #payloadOffset += 2 bytes for the FUA header
           adjust procLen based on if the start code is skipped

   till the input buffer is fully processed
       find the #payloadSize based on input data and ##fragmentSize
       ret = #AVB_Add1722Header to add the 1722 header
       if ret has error
           increment stats and log error
       copy the payload to the ethernet Tx buffer
       increment the input buffer processed bytes based on #payloadSize
       ret = #NIF_Send to send the data out on ethernet
       if ret has error
           increment stats and log error

       if input buffer is fully processed
           break from loop
       else
           ret = #NIF_GetTxBuffer of #AVB_AVTP_BUFFER_SIZE bytes
           if ret has error
               increment stats
                   outOfBuffers = TRUE
                   goto error

           if the ethernet Tx buffer returned is less than #AVBT_MIN_ETH_BUF_SIZE
               return buffer back using #NIF_Send
               increment stats
               goto error

           fragmentSize = ethDataBufSize - 32UL for 1722 header

   error:
       if input buffer is fully processed
           invoke #AVBT_HandleBufQueuing to send back buffers
           reset the buffer params in the component context

   return outOfBuffers
   @endcode
 */
static uint32_t AVBT_CircQDataProcess (uint32_t portIndex)
{
    int32_t retVal = 0;
    AVBT_BufInfoType *bufInfo = NULL;
    uint32_t ethBufIdx = 0UL;
    uint8_t *ethPtr = NULL;
    uint32_t ethDataBufSize = 0UL;
    uint32_t payloadSize = 0UL;
    uint32_t payloadOffset =
            AVBT_Context.compCfg[portIndex].headerLenOffset;
    /* 4 bytes of NAL start code that needs to be skipped */
    uint32_t skipBytes = 4UL;
    uint32_t addFUAHdr = 0UL;
    uint32_t fragmentSize = 0UL;
    uint32_t outOfBuffers = FALSE;

    AVBT_Stream1722CfgType *strCfg = &AVBT_Context.compCfg[portIndex];

    /* Update the buffer param for processing */
    bufInfo = &AVBT_Context.circQData[portIndex];

    /* Send the slice start indicator and stream info */
    AVBT_Context.bufparam[portIndex].payInfo.sliceStart = bufInfo->syncStart;
    AVBT_Context.bufparam[portIndex].payInfo.streamInfo = bufInfo->strInfo;
    AVBT_Context.bufparam[portIndex].payInfo.timestamp = bufInfo->timestamp;

    /* Handling when the upstream component sends zero size buffers */
    if(0UL == bufInfo->actualLen){
        goto error;
    }

    /* Try to get the maximum size pf ethernet Tx buffer */
    ethDataBufSize = AVB_AVTP_BUFFER_SIZE;

    /* Get an Ethernet Tx buffer before proceeding */
    retVal = NIF_GetTxBuffer(AVBT_Context.ethCtrlIdx[portIndex],
                             ETHER_ETHERTYPE_1722, (uint16_t)strCfg->pcp,
                             &ethBufIdx, &ethPtr, &ethDataBufSize);

    /* In case of an error */
    if (BCM_ERR_OK != retVal) {
        /* Increment the stats for failure */
        AVBT_Context.talkerStats[portIndex].pktGetTxBufFailure++;
        outOfBuffers = TRUE;
        goto error;
    }

    if (AVBT_MIN_ETH_BUF_SIZE > ethDataBufSize){
        (void)NIF_Send(AVBT_Context.ethCtrlIdx[portIndex],
                ETHER_ETHERTYPE_1722,
                FALSE,
                ethBufIdx,
                0UL,
                strCfg->mcStreamAddr);
        /* Increment the stats for failure */
        AVBT_Context.talkerStats[portIndex].pktGetTxBufFailure++;
        goto error;
    }

    /* 24 bytes of 1722 Header + 4 bytes of h264 TS + 2 bytes FUA */
    /* rounded to 8 byte aligned value                            */
    fragmentSize = ethDataBufSize - 32UL;

    /* Payload management for H264 payload format data only */
    if (AVB_PAYLFORMAT_H264 ==
            AVBT_Context.bufparam[portIndex].strCfgForHdr.payloadFormat) {

        uint32_t startCodeSkipped = 0UL;
        uint32_t remainingPayLen = 0UL;
        /* H264 timestamp (4 bytes) and NAL header (2 bytes) */
        /* as per 1722-2016 standard. FUA NAL header gets    */
        /* added only for non SPS/PPS data                   */
        payloadOffset += 4UL;

        /* If this is a slice start, then remove the NAL start code */
        /* from the payload                                         */
        if ((1UL == bufInfo->syncStart) && (0UL == bufInfo->procLen)) {
            uint8_t nalStartCode[4U] = {0U, 0U, 0U, 1U};
            retVal = memcmp(&AVBT_PortData[portIndex][bufInfo->bufIdx][0],
                            &nalStartCode[0], sizeof(nalStartCode));
            if (0L == retVal) {
                bufInfo->procLen += skipBytes;
                startCodeSkipped = 1UL;
            }
        }

        /* By default set FU Header addition flag to TRUE */
        addFUAHdr = 1UL;

        /* Based on the sliceStart and sliceEnd markers detect if  */
        /* FU headers need to be added or payload needs to be sent */
        /* as single NAL payload                                   */
        if ((1UL == bufInfo->syncStart) &&
            (1UL == bufInfo->syncEnd)) {

            /* Compute the remaining payload length */
            remainingPayLen = bufInfo->actualLen - bufInfo->procLen;

            /* Case when the payload is n+1 bytes, where 'n' is the    */
            /* maximum size of ethernet payload. This needs special    */
            /* handling. We reduce the fragmentation size from n to    */
            /* n-40 for this case and fragment the payload before      */
            /* transmission                                            */
            if (fragmentSize+1UL == remainingPayLen){
                /* Case when the encoded slice is 1 byte more than     */
                /* the maximum ethernet fragment size. If FUA mode is  */
                /* is used, then the removal of NAL header byte will   */
                /* mean that entire slice is sent in one L2 packet     */
                /* which is a violation of the RFC6184 standard.       */
                /* Single NAL mode cannot be used as the encoded slice */
                /* is 1 byte greater than the ETH fragment size and in */
                /* this case, the NAL header byte is not removed       */
                /* Hence we reduce the FUA fragment size and use FUA   */
                /* mode for payload packetization                      */
                fragmentSize = fragmentSize - 40UL;
            /* Small payload is sent as single NAL packets        */
            } else if (fragmentSize >= remainingPayLen){
                addFUAHdr = 0UL;
            } else {
                addFUAHdr = 1UL;
            }
        }

        /* Offset payload for FUA header and FUA indicator */
        if (1UL == addFUAHdr){
            payloadOffset += 2UL;
            /* Case when FUA header needs to be added so skip */
            /* NAL type present in payload                    */
            if (1UL == startCodeSkipped) {
                bufInfo->procLen += 1UL;
            }
        }
    }

    /* Process the entire buffer, packetize and send them out */
    do{
        /* Check data bytes present and determine payload size */
        if (fragmentSize < (bufInfo->actualLen -
                                            bufInfo->procLen)) {

            payloadSize = fragmentSize;

            /* Send slice end only for last group of data */
            AVBT_Context.bufparam[portIndex].payInfo.sliceEnd = 0UL;

        } else {

            /* Case when <= fragmentSize bytes of payload */
            payloadSize = bufInfo->actualLen - bufInfo->procLen;

            /* Send the slice and frame end flag for last packet of data */
            AVBT_Context.bufparam[portIndex].payInfo.sliceEnd = bufInfo->syncEnd;
            AVBT_Context.bufparam[portIndex].payInfo.frameEnd = bufInfo->frameEnd;

            /* Track slice end marker count */
            if (1UL == AVBT_Context.bufparam[portIndex].payInfo.sliceEnd) {
                AVBT_Context.talkerStats[portIndex].sliceEndCount++;
            }
        }

        /* Get the 1722header */
        retVal = AVB_Add1722Header(ethPtr,
                                   payloadSize,
                                   &AVBT_Context.bufparam[portIndex]);

        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_CIRCQDATAPROCESS_PROC,
                             retVal, (uint32_t) __LINE__,
                             bufInfo->actualLen, bufInfo->procLen, 0UL);
            /* Num errors noted while adding the 1722 header */
            AVBT_Context.talkerStats[portIndex].pktTxHdrAddnErrors++;
        }

        /* Reset the sync start marker to avoid sending multiple */
        /* sync start markings                                   */
        bufInfo->syncStart = 0UL;
        AVBT_Context.bufparam[portIndex].payInfo.sliceStart = 0UL;

        BCM_MemCpy(ethPtr + payloadOffset,
                   &AVBT_PortData[portIndex][bufInfo->bufIdx][0] +
                                bufInfo->procLen,
                   payloadSize);

        /* Update the processed data length */
        bufInfo->procLen += payloadSize;
        /* Add header size to total payload length */
        payloadSize += payloadOffset;

        /* Invoke OSIL function to send packetized data */
        retVal = NIF_Send(AVBT_Context.ethCtrlIdx[portIndex],
                            ETHER_ETHERTYPE_1722, 1UL,
                            ethBufIdx,
                            payloadSize,
                            strCfg->mcStreamAddr);

        /* Check Error of NIF_Send API */
        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_CIRCQDATAPROCESS_PROC,
                             retVal, (uint32_t) __LINE__,
                             0UL, 0UL, 0UL);

            /* Increment the error noted during Ethernet Tx send */
            AVBT_Context.talkerStats[portIndex].pktTxDiscard++;
        } else {

            /* Increment the count of successfully transmitted packets */
            AVBT_Context.talkerStats[portIndex].pktTxCount++;
        }

        /* Reset CIRCQ temporary memory when data is fully processed */
        if (bufInfo->procLen == bufInfo->actualLen) {
            break;
        } else {
            /* Try to get the maximum size pf ethernet Tx buffer */
            ethDataBufSize = AVB_AVTP_BUFFER_SIZE;
            /* Fetch buffers and queue them */
            retVal = NIF_GetTxBuffer(AVBT_Context.ethCtrlIdx[portIndex],
                                     ETHER_ETHERTYPE_1722, (uint16_t)strCfg->pcp,
                                     &ethBufIdx, &ethPtr, &ethDataBufSize);

            /* In case of an error */
            if (BCM_ERR_OK != retVal) {
                /* Increment the stats for failure */
                AVBT_Context.talkerStats[portIndex].pktGetTxBufFailure++;
                outOfBuffers = TRUE;
                goto error;
            }

            if (AVBT_MIN_ETH_BUF_SIZE > ethDataBufSize){
                (void)NIF_Send(AVBT_Context.ethCtrlIdx[portIndex],
                        ETHER_ETHERTYPE_1722,
                        FALSE,
                        ethBufIdx,
                        0UL,
                        strCfg->mcStreamAddr);
                /* Increment the stats for failure */
                AVBT_Context.talkerStats[portIndex].pktGetTxBufFailure++;
                outOfBuffers = TRUE;
                goto error;
            }

            /* 24 bytes of 1722 Header + 4 bytes of h264 TS + 2 bytes FUA */
            /* rounded to 8 byte aligned value                            */
            fragmentSize = ethDataBufSize - 32UL;

        }
    }while (bufInfo->actualLen > bufInfo->procLen);

error:
    /* Reset CIRCQ temporary memory when data is fully processed */
    if (bufInfo->procLen == bufInfo->actualLen) {
        void *ptr = &AVBT_Context.circQData[portIndex];

        /* Queue the buffer back to peer component */
        AVBT_HandleBufQueuing(portIndex,
                        AVBT_Context.circQData[portIndex].bufIdx);

        /* Reset the locally held data buffer details */
        BCM_MemSet(ptr, 0U,
                   sizeof(AVBT_BufInfoType));
    }

    return outOfBuffers;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   check for commands in command Q

       if command == MSYS_COMP_CMD_INIT
           for all enabled ports
               ret = #MSYS_InPortInit to initialize each port
               if ret has error
                   log error
               ret = #CircQ_Init to initialize port circQ
               if ret has error
                   log error
           update RTPS comp state = AVBT_COMPSTATE_INIT

   complete command by updating status
   return
   @endcode
 */
static void AVBT_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t portIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;

    retVal = MSGQ_GetMsgIdx(&AVBT_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBT_CmdQList[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {

            /* Reset the entire handle region */
            BCM_MemSet(&AVBT_Context, 0U,
                       sizeof(AVBT_Context));

            /* Initialize all the input ports */
            for (portIdx = 0UL;
                 portIdx < AVBT_MAX_NUM_INPUT_PORTS;
                 portIdx++) {

                /* Invoke framework function to initialize input port */
                retVal = MSYS_InPortInit (AVBT_InPort[portIdx]);
                if(BCM_ERR_OK != retVal){
                    goto end;
                }

                /* Initialize the circular queue */
                retVal = CIRCQ_Reset(AVBT_FillQ[portIdx]);
                if(BCM_ERR_OK != retVal){
                    goto end;
                }
                AVBT_Context.isDataAvailable[portIdx] = 0UL;

            }

            /* Update component state */
            AVBT_Context.compState = AVBT_COMPSTATE_INIT;
            processCmd->response = BCM_ERR_OK;
        }

end:
        /* Single location error check with all args */
        if (BCM_ERR_OK != retVal){
             AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RESETSTATEHANDLER_PROC,
                              retVal, (uint32_t) __LINE__, portIdx,
                              (uint32_t)(AVBT_Context.ethCtrlIdx[portIdx] >> 32UL),
                              (uint32_t)AVBT_Context.ethCtrlIdx[portIdx]);
        }

        retVal = MSGQ_CompleteMsgIdx(&AVBT_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RESETSTATEHANDLER_PROC, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }

}
/**
    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT
*/
typedef union _AVBT_ConfigDataType {
    const uint8_t* hdr;
    const AVBT_1722ConfigType *cfg;
} AVBT_ConfigDataType;

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}

   check for commands in command Q

       if command == MSYS_COMP_CMD_CONFIG
           if the num_of_ports in config is same as #AVBT_MAX_NUM_INPUT_PORTS
                store configuration in comp context structure
                nifIdx in comp context = #NIF_GetCtrlIdx API
                update RTPS comp state = AVBT_COMPSTATE_READY
           else
                log error
       else
           log error

   complete command by updating status
   return

   @endcode
 */
static void AVBT_InitStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t portIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;
    AVBT_ConfigDataType config;

    retVal = MSGQ_GetMsgIdx(&AVBT_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBT_CmdQList[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {

            /* Check for Valid Configuration Index Value */
            if (AVBT_CONFIG_1722_STR != processCmd->configID) {
                AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_INITSTATEHANDLER_PROC,
                                 processCmd->response,
                                (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            } else {
                config.hdr = processCmd->cfgData;

                /* Check for the number of configured ports */
                if (AVBT_MAX_NUM_INPUT_PORTS != config.cfg->numInpPort){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_INITSTATEHANDLER_PROC,
                                     retVal, (uint32_t) __LINE__,
                                     (uint32_t) AVBT_MAX_NUM_INPUT_PORTS,
                                     (uint32_t) config.cfg->numInpPort,
                                     0UL);
                } else {
                    /* Store the number of input ports activated */
                    AVBT_Context.numInpPort = config.cfg->numInpPort;

                    /* Store the stream configuration */
                    for (portIdx = 0;
                         portIdx < config.cfg->numInpPort;
                         portIdx++) {
                        /* Store the component configuration */
                        AVBT_Context.compCfg[portIdx] = config.cfg->inpStreamCfg[portIdx];
                        /* Store the library configuration */
                        AVBT_Context.bufparam[portIdx].strCfgForHdr =
                                    AVBT_Context.compCfg[portIdx].libCfg;

                        /* Get the SysEth controller index for transmission */
                        retVal = NIF_GetCtrlIdx((uint16_t)AVBT_Context.compCfg[portIdx].vlanID,
                                                BCM_AVB_ID,
                                                (uint8_t)portIdx,
                                                &AVBT_Context.ethCtrlIdx[portIdx]);
                        if (BCM_ERR_OK != retVal) {
                            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_INITSTATEHANDLER_PROC, retVal,
                                             __LINE__, 0UL, 0UL, 0UL);
                            AVBT_Context.compState = AVBT_COMPSTATE_RESET;
                            break;
                        }
                    }
                    if (BCM_ERR_OK == retVal) {
                        AVBT_Context.compState = AVBT_COMPSTATE_READY;
                    }
                    processCmd->response = retVal;

                }
            }
        }

        retVal = MSGQ_CompleteMsgIdx(&AVBT_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_INITSTATEHANDLER_PROC, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }

}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   check for commands in command Q

       if command == MSYS_COMP_CMD_START
           for all enabled ports
               if shaping is enabled
                    ret = #NIF_EnableAVBShaping(bandwith_percentage)
                    if ret is not BCM_ERR_OK
                        log error
               AVBT_HandleBufQueuing to queue buffers
           update AVBT comp state = AVBT_COMPSTATE_START
           update command response = BCM_ERR_OK
       else if command == MSYS_COMP_CMD_DEINIT
           update RTPS comp state = AVBT_COMPSTATE_RESET
           update command response = BCM_ERR_OK
       else
           ret = BCM_ERR_INVAL_PARAMS

   complete command by updating status
   return

   @endcode
 */
static void AVBT_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal = BCM_ERR_OK;
    uint32_t portIdx = 0UL;
    uint32_t bufIdx = 0UL;
    NIF_AVBClassType avbClass;
    AVBT_ConfigDataType config;

    retVal = MSGQ_GetMsgIdx(&AVBT_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBT_CmdQList[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            /* For all enabled ports of AVB Talker component */
            for(portIdx = 0UL;
                portIdx < AVBT_Context.numInpPort;
                portIdx++) {
                AVBT_Stream1722CfgType *strCfg =
                              &AVBT_Context.compCfg[portIdx];

                if ((strCfg->percentRsvdBW > 0UL) &&
                    (strCfg->percentRsvdBW <= 70UL))
                {
                    /* Enable credit shaping */
                    avbClass = (1UL == strCfg->isClassAtraffic) ?
                                NIF_AVBCLASS_A : NIF_AVBCLASS_B;
                    retVal = NIF_EnableAVBShaping(AVBT_Context.ethCtrlIdx[portIdx],
                                                  avbClass,
                                                  strCfg->percentRsvdBW);
                    if (BCM_ERR_OK != retVal) {
                        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC, retVal,
                                __LINE__,
                                (uint32_t)(AVBT_Context.ethCtrlIdx[portIdx] >> 32UL),
                                (uint32_t)AVBT_Context.ethCtrlIdx[portIdx],
                                strCfg->isClassAtraffic);
                    }
                }

                for(bufIdx = 0UL;
                    bufIdx < AVBT_PORT_NUM_BUFFERS;
                    bufIdx++){

                    /* Queueing the Tx buffers */
                    AVBT_HandleBufQueuing(portIdx, bufIdx);
                }
            }

            /* Update component state */
            AVBT_Context.compState = AVBT_COMPSTATE_START;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {

            /* Check for Valid Configuration Index Value */
            if (AVBT_CONFIG_1722_STR != processCmd->configID) {
                AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC,
                                 processCmd->response,
                                (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            } else {
                config.hdr = processCmd->cfgData;

                /* Check for the number of configured ports */
                if (AVBT_MAX_NUM_INPUT_PORTS != config.cfg->numInpPort){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC,
                                     retVal, (uint32_t) __LINE__,
                                     (uint32_t) AVBT_MAX_NUM_INPUT_PORTS,
                                     (uint32_t) config.cfg->numInpPort,
                                     0UL);
                } else {
                    /* Store the number of input ports activated */
                    AVBT_Context.numInpPort = config.cfg->numInpPort;

                    /* Store the stream configuration */
                    for (portIdx = 0;
                         portIdx < config.cfg->numInpPort;
                         portIdx++) {
                        /* Store the component configuration */
                        AVBT_Context.compCfg[portIdx] = config.cfg->inpStreamCfg[portIdx];
                        /* Store the library configuration */
                        AVBT_Context.bufparam[portIdx].strCfgForHdr =
                                    AVBT_Context.compCfg[portIdx].libCfg;

                        /* Get the SysEth controller index for transmission */
                        retVal = NIF_GetCtrlIdx((uint16_t)AVBT_Context.compCfg[portIdx].vlanID,
                                                BCM_AVB_ID,
                                                (uint8_t)portIdx,
                                                &AVBT_Context.ethCtrlIdx[portIdx]);
                        if (BCM_ERR_OK != retVal) {
                            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC, retVal,
                                             __LINE__, 0UL, 0UL, 0UL);
                            break;
                        }
                    }
                    processCmd->response = retVal;
                }
            }
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            AVBT_Context.compState = AVBT_COMPSTATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }

        retVal = MSGQ_CompleteMsgIdx(&AVBT_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_READYSTATEHANDLER_PROC, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   check for commands in command Q

       if command == MSYS_COMP_CMD_STOP
           for all enabled ports
               if shaping is enabled
                    ret = #NIF_DisableAVBShaping
                    if ret is not BCM_ERR_OK
                        log error
               invoke #AVBT_FlushTxBuffers to free buffers
           update AVBT comp state = #AVBT_COMP_STATE_STOP
           update command response = BCM_ERR_OK
       else
           update command response = BCM_ERR_NOSUPPORT

   complete command by updating status
   return
   @endcode
 */
static uint32_t AVBT_RunStateCmdHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t stopped = FALSE;
    uint32_t portIdxCnt = 0UL;
    int32_t retVal;

    retVal = MSGQ_GetMsgIdx(&AVBT_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &AVBT_CmdQList[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            /* Flush buffers of all input ports */
            for (portIdxCnt = 0UL;
                    portIdxCnt < AVBT_Context.numInpPort;
                    portIdxCnt++) {
                /* De-Queue Tx buffers from and return it to driver */
                AVBT_FlushTxBuffers(portIdxCnt);

                /* Disable credit shaping */
                if ((AVBT_Context.compCfg[portIdxCnt].percentRsvdBW > 0UL)
                    || (AVBT_Context.compCfg[portIdxCnt].percentRsvdBW <= 70UL))
                {
                    retVal = NIF_DisableAVBShaping(AVBT_Context.ethCtrlIdx[portIdxCnt]);
                    if (BCM_ERR_OK != retVal) {
                        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RUNSTATECMDHANDLER_PROC, retVal,
                                __LINE__, portIdxCnt,
                                (uint32_t) (AVBT_Context.ethCtrlIdx[portIdxCnt] >> 32UL),
                                (uint32_t) AVBT_Context.ethCtrlIdx[portIdxCnt]);
                    }
                }
            }
            AVBT_Context.compState = AVBT_COMPSTATE_READY;
            AVBT_CmdQList[cmdMsgIdx].response = BCM_ERR_OK;
            stopped = TRUE;
        } else {
            AVBT_CmdQList[cmdMsgIdx].response = BCM_ERR_NOSUPPORT;
        }

        retVal = MSGQ_CompleteMsgIdx(&AVBT_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RUNSTATECMDHANDLER_PROC, retVal,
                    __LINE__, 0UL, 0UL, 0UL);
        }
    }

    return stopped;
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   ret = invoke #MSYS_InPortProcess to get filled buffers
   if ret is not #BCM_ERR_OK
        log error
   @endcode
 */
static void AVBT_RunStateInputEventHandler(uint32_t portIndex)
{
    int32_t retVal;

    /* Call Framework Process in port function */
    retVal = MSYS_InPortProcess(AVBT_InPort[portIndex]);
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        AVBT_ReportError((uint8_t)BRCM_SWDSGN_AVBT_RUNSTATEINPUTEVENTHANDLER_PROC, retVal,
                (uint32_t) __LINE__, portIndex, 0UL, 0UL);
    }
}

/**
   @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
   @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

   @code{.c}
   if #portIndex is lesser than #AVBT_MAX_NUM_INPUT_PORTS
        if previous data is yet to be processed
            *outOfBuffers = #AVBT_CircQDataProcess to process data
        while previous data is fully processed and there is new buffer
            pop buffer from input port circQ
            *outOfBuffers = #AVBT_CircQDataProcess to process data
   return
   @endcode
 */
static void AVBT_RunStateHandler(uint32_t portIndex, uint32_t* const outOfBuffers)
{
    /* Check if port is enabled before proceeding */
    if (AVBT_MAX_NUM_INPUT_PORTS > portIndex) {

        /* Check that any pending data needs to be processed */
        if (1UL == AVBT_Context.circQData[portIndex].isDataUnderProcess) {
            *outOfBuffers = AVBT_CircQDataProcess(portIndex);
        }

        /* Check if new data is present in the Q */
        /* If previous data has been processed and new data is present in Q */
        while ((0UL == AVBT_Context.circQData[portIndex].isDataUnderProcess) &&
               (FALSE == *outOfBuffers)) {

            if (1UL == CIRCQ_Pop(AVBT_FillQ[portIndex],
                                 (char *)&AVBT_Context.circQData[portIndex],
                                 1UL)) {
                *outOfBuffers = AVBT_CircQDataProcess(portIndex);
            } else {
                AVBT_Context.isDataAvailable[portIndex] = 0UL;
                break;
            }
        }
    }
}

/** @brief AVBT component task function

    This task is created for processing all events to the AVB Talker
    component. It waits for commands, network events such as Tx
    confirmation and data from connected upstream component.
    Priority is given to command processing over data processing.
    On Tx confirmation from NIF, it checks if any data is pending
    processing due to underflow of Tx buffers and processes them.

    @behavior Sync, Non-reentrant

    @pre none

    @retval none

    @post none

    @trace #BRCM_SWARCH_AVBT_GETCOMPFNTABLE_PROC
    @trace #BRCM_SWREQ_AVBT_MEDIA_COMPONENT

    @limitations none

    @code{.c}
    WaitEvent(AVBT_ERIKA_EVENT_CMD_PROCESS      |
              AVBT_EVENT_INPPORT0_PROCESS |
              AVBT_EVENT_INPPORT1_PROCESS |
              AVBT_EVENT_INPPORT2_PROCESS |
              AVBT_EVENT_INPPORT3_PROCESS |
              AVBT_EVENT_DATA_TXBUF_Q)
    #GetEvent and #ClearEvent;

    if component state is AVBT_COMPSTATE_RESET:
        invoke #AVBT_ResetStateHandler
    else if component state is AVBT_COMPSTATE_INIT:
        invoke #AVBT_InitStateHandler
    else if component state is AVBT_COMPSTATE_READY:
        invoke #AVBT_ReadyStateHandler
    else if component state is AVBT_COMPSTATE_START:
        if its a command event
            stopped = invoke #AVBT_RunStateCmdHandler
        if stopped is FALSE
            if its a TX confirmation event and data processing is in
            progress on any port
                invoke #AVBT_RunStateHandler to process data

            if input port 0 event
                #AVBT_RunStateInputEventHandler(0) to get filled buffers
                #AVBT_RunStateHandler to process input buffers
            if input port 1 event
                #AVBT_RunStateInputEventHandler(1) to get filled buffers
                #AVBT_RunStateHandler to process input buffers
            if input port 2 event
                #AVBT_RunStateInputEventHandler(2) to get filled buffers
                #AVBT_RunStateHandler to process input buffers
            if input port 3 event
                #AVBT_RunStateInputEventHandler(3) to get filled buffers
                #AVBT_RunStateHandler to process input buffers
    Once component reaches #AVBT_COMPSTATE_RESET state, terminate task
    @endcode
*/
TASK(AVBT_Task)
{
    BCM_EventMaskType mask = 0UL;
    uint32_t port = 0UL;
    uint32_t stopped;
    uint32_t outOfBuffers;

    do {
        (void)BCM_WaitEvent(AVBT_ERIKA_EVENT_CMD_PROCESS  |
                        AVBT_ERIKA_EVENT_INPPORT0_PROCESS |
                        AVBT_ERIKA_EVENT_INPPORT1_PROCESS |
                        AVBT_ERIKA_EVENT_INPPORT2_PROCESS |
                        AVBT_ERIKA_EVENT_INPPORT3_PROCESS |
                        AVBT_ERIKA_EVENT_DATA_TXBUF_Q);

        (void)BCM_GetEvent(AVBT_ERIKA_TASK, &mask);
        (void)BCM_ClearEvent(mask);

        switch(AVBT_Context.compState) {
        case AVBT_COMPSTATE_RESET:
            AVBT_ResetStateHandler();
            break;
        case AVBT_COMPSTATE_INIT:
            AVBT_InitStateHandler();
            break;
        case AVBT_COMPSTATE_READY:
            AVBT_ReadyStateHandler();
            break;
        case AVBT_COMPSTATE_START:
            stopped = FALSE;
            outOfBuffers = FALSE;
            /* Process Commands */
            if (0UL != (mask & AVBT_ERIKA_EVENT_CMD_PROCESS)) {
                stopped = AVBT_RunStateCmdHandler();
            }
            if (FALSE == stopped) {
                /* Process Tx Confirmation first to push out any pending data */
                if (0UL != (mask & AVBT_ERIKA_EVENT_DATA_TXBUF_Q)) {
                    for (port = 0UL; port < AVBT_MAX_NUM_INPUT_PORTS; port++) {

                        if ((1UL == AVBT_Context.circQData[port].isDataUnderProcess) ||
                            (1UL == AVBT_Context.isDataAvailable[port])) {
                            AVBT_RunStateHandler(port, &outOfBuffers);
                            if (TRUE == outOfBuffers) {
                                break;
                            }
                        }
                    }
                }
                /* Process Input port events */
                if (0UL != (mask & AVBT_ERIKA_EVENT_INPPORT0_PROCESS)) {
#if (AVBT_INPUTPORTNUM_0 < AVBT_MAX_NUM_INPUT_PORTS)
                    AVBT_RunStateInputEventHandler(0UL);
                    if (FALSE == outOfBuffers) {
                        AVBT_RunStateHandler(0UL, &outOfBuffers);
                    }
#endif
                }
                if (0UL != (mask & AVBT_ERIKA_EVENT_INPPORT1_PROCESS)) {
#if (AVBT_INPUTPORTNUM_1 < AVBT_MAX_NUM_INPUT_PORTS)
                    AVBT_RunStateInputEventHandler(1UL);
                    if (FALSE == outOfBuffers) {
                        AVBT_RunStateHandler(1UL, &outOfBuffers);
                    }
#endif
                }
                if (0UL != (mask & AVBT_ERIKA_EVENT_INPPORT2_PROCESS)) {
#if (AVBT_INPUTPORTNUM_2 < AVBT_MAX_NUM_INPUT_PORTS)
                    AVBT_RunStateInputEventHandler(2UL);
                    if (FALSE == outOfBuffers) {
                        AVBT_RunStateHandler(2UL, &outOfBuffers);
                    }
#endif
                }
                if (0UL != (mask & AVBT_ERIKA_EVENT_INPPORT3_PROCESS)) {
#if (AVBT_INPUTPORTNUM_3 < AVBT_MAX_NUM_INPUT_PORTS)
                    AVBT_RunStateInputEventHandler(3UL);
                    if (FALSE == outOfBuffers) {
                        AVBT_RunStateHandler(3UL, &outOfBuffers );
                    }
#endif
                }
            }
            break;
        default:
            break;
        }
    }while(AVBT_COMPSTATE_RESET != AVBT_Context.compState);

    (void)BCM_TerminateTask();
}

/** @} */
