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
    @defgroup grp_media_port_impl Media Port Implementation
    @ingroup grp_framework

    @addtogroup grp_media_port_impl
    @{
    @todo update details here

    @file media_port.c
    @brief Media Port Implementation

    This file contains the media port related functionality which
    enables component ports to pass media buffers to each other

    @version 0.1 Initial version
*/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <compiler.h>
#include <media_port.h>
#include <media_system.h>
#include <component.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <ee.h>
#include <bcm_time.h>
#include <atomic.h>
#include <cortex.h>

/**
    @name MSYS Port Design IDs
    @{
    @brief Design IDs for MSYS Port
*/
#define BRCM_SWDSGN_MSYS_REPORT_PORT_ERROR_PROC (0x80U) /**< @brief #MSYS_ReporPortError        */
#define BRCM_SWDSGN_MSYS_INPORT_VALIDATE_PROC   (0x81U) /**< @brief #MSYS_InPortValidate        */
#define BRCM_SWDSGN_MSYS_CONNECTED_PORT_PROC    (0x82U) /**< @brief #MSYS_GetConnectedPort      */
#define BRCM_SWDSGN_MSYS_INPORT_INIT_PROC       (0x83U) /**< @brief #MSYS_InPortInit            */
#define BRCM_SWDSGN_MSYS_OUTPORT_INIT_PROC      (0x84U) /**< @brief #MSYS_OutPortInit           */
#define BRCM_SWDSGN_MSYS_BUFFER_VALIDATE_PROC   (0x85U) /**< @brief #MSYS_BufferValidate        */
#define BRCM_SWDSGN_MSYS_FILLBUFFER_PROC        (0x86U) /**< @brief #MSYS_FillBuffer            */
#define BRCM_SWDSGN_MSYS_INPORT_PROCESS_PROC    (0x87U) /**< @brief #MSYS_InPortProcess         */
#define BRCM_SWDSGN_MSYS_INPORT_CANCELALL_PROC  (0x88U) /**< @brief #MSYS_InPortCancelAll       */
#define BRCM_SWDSGN_MSYS_LATENCY_FACTOR_MACRO   (0x89U) /**< @brief #MSYS_LATENCY_LOG2W_FACTOR  */
#define BRCM_SWDSGN_MSYS_IN_PORT_EVENT_GLOBAL   (0x8AU) /**< @brief #MSYS_InPortEvent           */
#define BRCM_SWDSGN_MSYS_OUT_PORT_EVENT_GLOBAL  (0x8BU) /**< @brief #MSYS_OutPortEvent          */
/** @} */

/** @brief Log2 weight factor for average latency calculation

    @trace #BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC
    @trace #BRCM_SWREQ_MSYS_STATISTICS
*/
#define MSYS_LATENCY_LOG2W_FACTOR (3UL)


/**
    @trace #BRCM_SWARCH_MSYS_INPORT_INIT_PROC
    @trace #BRCM_SWARCH_MSYS_OUTPORT_INIT_PROC
    @trace #BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC
    @trace #BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC
    @trace #BRCM_SWARCH_MSYS_FILLBUFFER_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
static void MSYS_ReporPortError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_MSF_ID, 0U, aApiID, aErr, 4UL, values);
}

/** @brief   A helper function to get validate an  MediaInputPort object

    @param   aPort      Sink component's input port object

    @return  BCM_ERR_OK if valid, ERR_INVAL_PARAMS otherwise

    @trace #BRCM_SWARCH_MSYS_INPORT_INIT_PROC
    @trace #BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC
    @trace #BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC
    @trace #BRCM_SWARCH_MSYS_FILLBUFFER_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
static int32_t MSYS_InPortValidate(const MSYS_InPortType* const aPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (NULL == aPort) {
        MSYS_ReporPortError(BRCM_SWDSGN_MSYS_INPORT_VALIDATE_PROC, retVal, __LINE__,
                        (uint32_t)aPort, 0UL, 0UL);
    } else if (aPort->magicID != MSYS_IN_PORT_MAGIC) {
        retVal = BCM_ERR_INVAL_MAGIC;
        MSYS_ReporPortError(BRCM_SWDSGN_MSYS_INPORT_VALIDATE_PROC, retVal,
                        __LINE__, (uint32_t)aPort, aPort->magicID, 0UL);
    } else if ((NULL == aPort->fillBufferDone) && (NULL == aPort->fillBufferDoneV2)) {
        MSYS_ReporPortError(BRCM_SWDSGN_MSYS_INPORT_VALIDATE_PROC, retVal,
                        __LINE__, (uint32_t)aPort, (uint32_t)aPort->fillBufferDone,
                        (uint32_t)aPort->fillBufferDoneV2);
    } else if (NULL == aPort->inportRW) {
        MSYS_ReporPortError(BRCM_SWDSGN_MSYS_INPORT_VALIDATE_PROC, retVal,
                        __LINE__, (uint32_t)aPort, (uint32_t)aPort->inportRW, 0UL);
    } else {
        retVal = BCM_ERR_OK;
    }
    return retVal;
}

/** @brief  A helper function to get valid MediaOutputPort object from the
            source component connected to a sink component's input port
    @param   aPort      Sink component's input port object

    @return  Media Output Port object if valid, NULL otherwise

    @trace #BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC
    @trace #BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC
    @trace #BRCM_SWARCH_MSYS_FILLBUFFER_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
static const MSYS_OutPortType* MSYS_GetConnectedPort(const MSYS_InPortType* const aPort)
{
    const MSYS_OutPortType* outPort = NULL;
    const MSYS_CompFnTblType* compFnTbl;
    const MSYS_Type* mSys = MSYS_Get(0UL);
    uint32_t i;

    if (BCM_ERR_OK == MSYS_InPortValidate(aPort)) {
        /* First get component and port index for the Source port where this buffer
        needs to be enqueued */
        for (i = 0UL; i < mSys->count; i++) {
            if(aPort->compID == mSys->component[i].compID) {
                uint32_t srcCompIdx = mSys->component[i].portMap[aPort->portIndex].srcCompIdx;
                compFnTbl = MSYS_GetCompFnTbl(mSys->component[srcCompIdx].compID);
                if (NULL != compFnTbl) {
                    outPort = compFnTbl->getOutPort(mSys->component[i].portMap[aPort->portIndex].srcPortIdx);
                    if (NULL != outPort) {
                        if ((outPort->magicID != MSYS_OUT_PORT_MAGIC)
                            || (outPort->compID != mSys->component[srcCompIdx].compID)) {
                            MSYS_ReporPortError(BRCM_SWDSGN_MSYS_CONNECTED_PORT_PROC, BCM_ERR_UNKNOWN,
                                        outPort->magicID, (uint32_t)(outPort->compID & 0xFFFFFFFFUL),
                                        (uint32_t)((outPort->compID >> 32UL) & 0xFFFFFFFFUL), 0UL);
                            outPort = NULL;
                        }
                        break;
                    }
                } else {
                    MSYS_ReporPortError(BRCM_SWDSGN_MSYS_CONNECTED_PORT_PROC, BCM_ERR_UNKNOWN,
                                         __LINE__, 0UL, 0UL, 0UL);
                    outPort = NULL;
                    break;
                }
            }
        }
    }
    return outPort;
}

/** @brief   Initialize the input port data structure

    @param   aPort      Sink component's input port object

    @return  BCM_ERR_OK if          initialization succeeded
             BCM_ERR_INVAL_PARAMS   Input port validation failed
             BCM_ERR_INVAL_MAGIC    Input port's handle list's magic invalid
    @trace #BRCM_SWARCH_MSYS_INPORT_INIT_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_InPortInit(const MSYS_InPortType* const aPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t i;

    if (BCM_ERR_OK == MSYS_InPortValidate(aPort)) {
        aPort->inportRW->fullQ = LWQ_BUFFER_INIT_VALUE;
        for (i = 0UL; i < MSGQ_SIZE; i++) {
            aPort->inportRW->msgHdr[i] = NULL;
        }
        aPort->inportRW->stats.nRx = 0UL;
        aPort->inportRW->stats.nRxCancelled = 0UL;
        aPort->inportRW->stats.nRxFailed = 0UL;
        aPort->inportRW->stats.nTx = 0UL;
        aPort->inportRW->stats.nTxFailed = 0UL;
        aPort->inportRW->stats.nTxCritFailed = 0UL;
        aPort->inportRW->stats.avgLatency = 0UL;
        aPort->inportRW->stats.frameCount = 0UL;
        aPort->inportRW->stats.byteCount = 0ULL;
        retVal = BCM_ERR_OK;
    }

    return retVal;
}
/** @brief   Initialize the output port data structure

    @param   aPort      component's output port object

    @return  BCM_ERR_OK if          initialization succeeded
             BCM_ERR_INVAL_PARAMS   Input port validation failed
             BCM_ERR_INVAL_MAGIC    Input port's handle list's magic invalid

    @trace #BRCM_SWARCH_MSYS_OUTPORT_INIT_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_OutPortInit(const MSYS_OutPortType* const aPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    uint32_t idx = 0UL;

    /* Check params */
    if (NULL == aPort) {
        MSYS_ReporPortError(BRCM_SWARCH_MSYS_OUTPORT_INIT_PROC, retVal,
                            (uint32_t)aPort, 0UL, 0UL, 0UL);
    } else if (aPort->magicID != MSYS_OUT_PORT_MAGIC) {
        MSYS_ReporPortError(BRCM_SWARCH_MSYS_OUTPORT_INIT_PROC, retVal,
                            (uint32_t)aPort, aPort->magicID, 0UL, 0UL);
    } else {
        retVal = MSGQ_GetMsgIdx(&aPort->dataQ,&idx);
        (void)idx;
        if (BCM_ERR_BUSY == retVal) {
            retVal = BCM_ERR_OK;
        }
    }

    return retVal;
}
/** @brief   Validate a MSYS_BufferType

    @param[in]   inMediaBuffer     Input media buffer to be Validated
    @return  BCM_ERR_OK            Validation success
             BCM_ERR_INVAL_PARAMS  Input media buffer validation failed

    @trace #BRCM_SWARCH_MSYS_FILLBUFFER_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
static int32_t MSYS_BufferValidate(const MSYS_BufferType* const aMediaBuffer)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;
    if ((NULL == aMediaBuffer)
        || (0UL == aMediaBuffer->numChannels)) {
        MSYS_ReporPortError(BRCM_SWDSGN_MSYS_BUFFER_VALIDATE_PROC, retVal,
                        __LINE__, (uint32_t)aMediaBuffer, 0UL, 0UL);
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        for (i = 0UL; i < aMediaBuffer->numChannels; i++) {
            if ((NULL == aMediaBuffer->buffer[i].data)
                || (0UL == aMediaBuffer->buffer[i].size)) {
                MSYS_ReporPortError(BRCM_SWDSGN_MSYS_BUFFER_VALIDATE_PROC, retVal,
                                __LINE__, (uint32_t)aMediaBuffer,
                                (uint32_t)aMediaBuffer->buffer[i].data,
                                aMediaBuffer->buffer[i].size);
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        }
    }

    return retVal;
}

/** @brief   Post a fill buffer request to framework

    @param   aPort            Sink component's input port object
    @param   inMediaBuffer     Input media buffer to be filled

    @return  BCM_ERR_OK            Request succeeded
             BCM_ERR_INVAL_PARAMS  Input port/media buffer validation failed
             BCM_ERR_INVAL_MAGIC   Input port's handle list's magic invalid
             BCM_ERR_UNKNOWN       Invalid output port returned by application
                                   Input port's write index incorrect
             BCM_ERR_BUSY          Output port's queue is full

    @trace #BRCM_SWARCH_MSYS_FILLBUFFER_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_FillBuffer(const MSYS_InPortType* const aPort,
                   const MSYS_BufferType* const aMediaBuffer)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const MSGQ_MsgHdrType* msgHandle = NULL;
    const MSYS_OutPortType*  outPort = NULL;

    outPort = MSYS_GetConnectedPort(aPort);
    /* Check params */
    if (BCM_ERR_OK != MSYS_InPortValidate(aPort)) {
        /* Do Nothing. Return value is already set */
    } else if (BCM_ERR_OK != MSYS_BufferValidate(aMediaBuffer)) {
        aPort->inportRW->stats.nTxFailed++;
    } else if (NULL == outPort) {
        aPort->inportRW->stats.nTxFailed++;
    } else if (LWQ_BUFFER_MAX_LENGTH == LWQ_Length(aPort->inportRW->fullQ)){
        aPort->inportRW->stats.nTxFailed++;
        retVal = BCM_ERR_NOMEM;
    } else {
        retVal = MSGQ_SendMsg(&outPort->dataQ, aMediaBuffer, aPort->eventMask,
                         (const MSGQ_MsgHdrType** const)&msgHandle);
        if ((BCM_ERR_OK != retVal) || (NULL == msgHandle)) {
            retVal = BCM_ERR_UNKNOWN;
        } else {
            /* Cache the returned handle and the sent buffer in the queues */
            uint8_t hIndex = LWQ_PeekLast(aPort->inportRW->fullQ);
            if (0UL != LWQ_Length(aPort->inportRW->fullQ)) {
                hIndex = (hIndex + 1U) & ((uint8_t)MSGQ_SIZE - 1U);
            }
            aPort->inportRW->msgHdr[hIndex] = msgHandle;
            retVal = LWQ_Push(&aPort->inportRW->fullQ, hIndex);
            if (BCM_ERR_OK != retVal) {
                aPort->inportRW->stats.nTxCritFailed++;
                retVal = BCM_ERR_UNKNOWN;
            } else {
                aPort->inportRW->stats.nTx++;
            }
        }
    }

    return retVal;
}

/** @brief   API invoked by Sink component when input port event is triggered

    @param   aPort            Sink component's input port object

    @return  BCM_ERR_OK            Request succeeded
             BCM_ERR_INVAL_PARAMS  Input port validation failed
             BCM_ERR_INVAL_MAGIC   Input port's handle list's magic invalid
             BCM_ERR_UNKNOWN       Returned in following cases:
                              (1) Invalid output port returned by application
                              (2) No buffers available from output port
                              (3) Buffer address modified by output port
             BCM_ERR_INVAL_STATE   Input port's handle queue in invalid state

    @trace #BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_InPortProcess(const MSYS_InPortType* const aPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const MSYS_OutPortType*  outPort = NULL;
    MSYS_BufferType rxBuf;
    uint32_t i;
    uint32_t tCnt;
    uint8_t hIndex;

    outPort = MSYS_GetConnectedPort(aPort);

    if (BCM_ERR_OK != MSYS_InPortValidate(aPort)) {
        /* Do Nothing. Return value already set */
    } else if (NULL == outPort) {
        aPort->inportRW->stats.nRxFailed++;
    } else {
        retVal = BCM_ERR_NOMEM; /*Defaulting to No buffers in Queue */
        tCnt = LWQ_Length(aPort->inportRW->fullQ);
        for(i = 0UL; i < tCnt; i++) {
            hIndex = LWQ_PeekFirst(aPort->inportRW->fullQ);
            if (NULL == aPort->inportRW->msgHdr[hIndex]) {
                /* Safety Check: Ideally this should not happen. The region
                between read & write indices is 'posted' buffers    */
                retVal = BCM_ERR_INVAL_STATE;
                aPort->inportRW->stats.nRxFailed++;
                break;
            }
            retVal = MSGQ_RecvMsg(&outPort->dataQ, aPort->inportRW->msgHdr[hIndex], &rxBuf);
            if (retVal == BCM_ERR_BUSY) {
                break;
            } else  if (retVal != BCM_ERR_OK) {
                aPort->inportRW->stats.nRxFailed++;
                MSYS_ReporPortError(BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC, retVal,
                                __LINE__, aPort->portIndex,
                                (uint32_t)(aPort->compID & 0xFFFFFFFFUL),
                                (uint32_t)((aPort->compID >> 32UL) & 0xFFFFFFFFUL));
                retVal = BCM_ERR_UNKNOWN;
                break;
            } else {
                if (BCM_ERR_OK != LWQ_Pop(&aPort->inportRW->fullQ, &hIndex)) {
                    /* This should never happen as Peek Succeeded.
                    This could happen if client calls this function from
                    multiple tasks with same port */
                    retVal = BCM_ERR_UNKNOWN;
                    MSYS_ReporPortError(BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC, retVal,
                                        __LINE__, (uint32_t) (aPort->inportRW->fullQ & 0xFFFFFFFFUL),
                                        (uint32_t) ((aPort->inportRW->fullQ >> 32UL) & 0xFFFFFFFFUL), 0UL);
                } else {
                    retVal = BCM_ERR_OK;
                    aPort->inportRW->stats.nRx++;
                    aPort->inportRW->stats.byteCount += rxBuf.buffer[0].size;
                    if (0UL != rxBuf.frameStart) {
                        /* Calculate average latency */
                        BCM_TimeType timestamp;
                        uint32_t latency;
                        aPort->inportRW->stats.frameCount++; /* Count frames */
                        BCM_GetTime(&timestamp);
                        latency =  (uint32_t) ((((uint64_t) timestamp.s *
                                                 (uint64_t) 1000000000UL) +
                                                 (uint64_t) timestamp.ns) - rxBuf.timestamp);
                        if (0UL != aPort->inportRW->stats.avgLatency) {
                            aPort->inportRW->stats.avgLatency = (((1UL << MSYS_LATENCY_LOG2W_FACTOR) - 1UL) *
                                                            aPort->inportRW->stats.avgLatency + latency) /
                                                            (1UL << MSYS_LATENCY_LOG2W_FACTOR);
                        } else {
                            aPort->inportRW->stats.avgLatency = latency;
                        }
                    }
                    if (NULL != aPort->fillBufferDoneV2) {
                        aPort->fillBufferDoneV2(aPort, &rxBuf);
                    } else {
                        aPort->fillBufferDone(&rxBuf);
                    }
                }
            }
        }
    }

    return retVal;
}

/** @brief   Cancels all requests posted to the port. Raises callbacks
            (FillBufferDone) for all cancelled/filled buffers in FIFO order
    @param   aPort            Sink component's input port object

    @return  BCM_ERR_OK            Request succeeded
             BCM_ERR_INVAL_PARAMS  Input port validation failed
             BCM_ERR_INVAL_MAGIC   Input port's handle list's magic invalid
             BCM_ERR_UNKNOWN       Invalid output port returned by application
             BCM_ERR_INVAL_STATE   Input port's handle queue in invalid state

    @trace #BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_InPortCancelAll(const MSYS_InPortType* const aPort)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    const MSYS_OutPortType*  outPort = NULL;
    MSYS_BufferType          rxBuf;
    uint32_t             i;
    uint32_t             j;
    uint32_t             tCnt;
    uint8_t hIndex;

    outPort = MSYS_GetConnectedPort(aPort);

    if (BCM_ERR_OK != MSYS_InPortValidate(aPort)) {
        /* Do Nothing. Return value already set */
    } else if (NULL == outPort) {
        MSYS_ReporPortError(BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC, retVal,
                            __LINE__, aPort->portIndex,
                            (uint32_t)(aPort->compID & 0xFFFFFFFFUL),
                            (uint32_t)((aPort->compID >> 32UL) & 0xFFFFFFFFUL));
    } else {
        retVal = BCM_ERR_OK;
        /* Now traverse all posted buffers (R+1 -> W-1) and cancel them */
        tCnt = LWQ_Length(aPort->inportRW->fullQ);
        for(i = 0UL; i < tCnt; i++) {
            retVal = LWQ_Pop(&aPort->inportRW->fullQ, &hIndex);
            if ((BCM_ERR_OK != retVal) || (NULL == aPort->inportRW->msgHdr[hIndex])) {
                /* Should not happen. Enabling error return to debug till stabilization */
                retVal = BCM_ERR_INVAL_STATE;
                MSYS_ReporPortError(BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC, retVal,
                                __LINE__, aPort->portIndex,
                                (uint32_t)(aPort->compID & 0xFFFFFFFFUL),
                                (uint32_t)((aPort->compID >> 32UL) & 0xFFFFFFFFUL));
                break;
            } else {
                retVal = MSGQ_TryCancelMsg(&outPort->dataQ, aPort->inportRW->msgHdr[hIndex], &rxBuf);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_CANCELLED != retVal)) {
                    /* This should not occur. The framework ensures that Source
                    component is stopped before Sink component. It also ensures
                    that the STOP command is blocking so we are sure that the
                    Source component has stop processing. All slots are thus
                    guaranteed to be in either COMPLETE or POSTED states */
                    MSYS_ReporPortError(BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC, retVal,
                                    __LINE__, aPort->portIndex,
                                    (uint32_t)(aPort->compID & 0xFFFFFFFFUL),
                                    (uint32_t)((aPort->compID >> 32UL) & 0xFFFFFFFFUL));
                    break;
                } else if (BCM_ERR_OK == retVal) {
                    /* Filled buffer already available but since Sink is stopped
                    it cannot do anything with this buffer. Clean up the buffer
                    to simulate cancellation success. Ignore the received response */
                    rxBuf.response = BCM_ERR_OK;
                    aPort->inportRW->stats.nRxCancelled++;
                    aPort->inportRW->stats.byteCount += rxBuf.buffer[0].size;
                    if (NULL != aPort->fillBufferDoneV2) {
                        aPort->fillBufferDoneV2(aPort, &rxBuf);
                    } else {
                        aPort->fillBufferDone(&rxBuf);
                    }
                } else {  /* Same as  else if (BCM_ERR_CANCELLED == retVal) */
                    /* Source has not got a chance to operate on this buffer so
                    we need to clean up so that the Sink component does not
                    misinterpret */
                    for (j = 0UL; j < BCM_MIN(rxBuf.numChannels, MSYS_BUFFER_MAX_CHANNELS); j++) {
                        rxBuf.buffer[j].size = 0UL;
                    }
                    rxBuf.response = BCM_ERR_OK;
                    aPort->inportRW->stats.nRxCancelled++;
                    aPort->inportRW->stats.byteCount += rxBuf.buffer[0].size;
                    if (NULL != aPort->fillBufferDoneV2) {
                        aPort->fillBufferDoneV2(aPort, &rxBuf);
                    } else {
                        aPort->fillBufferDone(&rxBuf);
                    }
                }
            }
        }
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_MSYS_IN_PORT_EVENT_GLOBAL
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
const uint32_t MSYS_InPortEvent[MSYS_IN_PORT_IDX_MAX] =
{
    MSYS_IN_PORT_EVENT_0,
    MSYS_IN_PORT_EVENT_1,
    MSYS_IN_PORT_EVENT_2,
    MSYS_IN_PORT_EVENT_3,
};

/**
    @trace #BRCM_SWARCH_MSYS_OUT_PORT_EVENT_GLOBAL
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
const uint32_t MSYS_OutPortEvent[MSYS_OUT_PORT_IDX_MAX] =
{
    MSYS_OUT_PORT_EVENT_0,
    MSYS_OUT_PORT_EVENT_1,
    MSYS_OUT_PORT_EVENT_2,
    MSYS_OUT_PORT_EVENT_3,
};


/** @} */

