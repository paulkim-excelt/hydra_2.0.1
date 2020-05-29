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

/***********************************************************************
File Name  : rtp_sender.c
Description: This file provides the implementation for the task
             activities of the RTP Sender component
***********************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <component.h>
#include <msg_queue.h>
#include <circ_queue.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <system.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <cache.h>
#include <osil/bcm_osil.h>

#include "udp.h"
#include "mpegts_lib.h"
#include "rtp_sender.h"
#include "rtp_rtcp.h"
#include "rtp_sender_internal.h"
#include "ee.h"

/** RTP packet size to be used for transmission. In case no */
/** IP level fragmentation is desired, then payload size    */
/** shall be configured to 1440 bytes                       */
#ifndef RTPS_PAYLOAD_FRAGMENT_SIZE
#define RTPS_PAYLOAD_FRAGMENT_SIZE     (1440UL * 1UL)
#endif

#define GetModuleLogLevel() ULOG_LVL_ERROR

#ifndef RTP_MAX_NUM_INPUT_PORTS
/* Number of input ports defined by default */
#define RTP_MAX_NUM_INPUT_PORTS (1UL)
#endif

#define RTPS_PORT_IDX_OFFSET       (MSYS_CHANNEL_ATTRIB_RSVD1)
#define RTPS_COMP_CIRCQ_SIZE       (8UL)

/* These values need to be updated as per system document */
#define RTPS_STR_CONFIG      (MSYS_CONFIG_RTP_SENDER)
#define RTPS_MPEGTS_CONFIG   (MSYS_CONFIG_MPEGTS_LIB)


/** Task for RTP Component */
#define RTPS_ERIKA_TASK                RTPS_Task
/** Signal Command */
#define RTPS_EVENT_CMD_PROCESS         SystemEvent0
/** Queue Tx buffers based on TxConf */
#define RTPS_EVENT_DATA_TXBUF_Q        UDPTxEvent
/** Input port 0 Data Process */
#define RTPS_EVENT_INPPORT0_PROCESS    SystemEvent4
/** Input port 1 Data Process */
#define RTPS_EVENT_INPPORT1_PROCESS    SystemEvent5
/** Input port 2 Data Process */
#define RTPS_EVENT_INPPORT2_PROCESS    SystemEvent6
/** Input port 3 Data Process */
#define RTPS_EVENT_INPPORT3_PROCESS    SystemEvent7


#if (RTPS_COMP_INPUTPORT_0 < RTP_MAX_NUM_INPUT_PORTS)
/* System Framework data structure */
MSYS_DECLARE_IN_PORT(RTPInPort0, 0UL, MSYS_COMP_ID_RTPS, SystemEvent4,
                       RTPInPortRW0, RTPS_FillBufferDoneProcess);

CIRCQ_INDEX_DEFINE(RTPS_circQPortIdx0,RTPS_COMP_CIRCQ_SIZE);
static RTPS_BufInfo RTPS_circQPortBuffer0[RTPS_COMP_CIRCQ_SIZE];
static const CIRCQ_Type RTPS_CIRCQPort0 = CIRCQ_INIT(
                    &RTPS_circQPortIdx0, RTPS_COMP_CIRCQ_SIZE,
                    sizeof(RTPS_BufInfo),
                    (char *)RTPS_circQPortBuffer0,
                    CIRCQ_OVERWRITE_DISABLE);
#endif

#if (RTPS_COMP_INPUTPORT_1 < RTP_MAX_NUM_INPUT_PORTS)
MSYS_DECLARE_IN_PORT(RTPInPort1, 1UL, MSYS_COMP_ID_RTPS, SystemEvent5,
                       RTPInPortRW1, RTPS_FillBufferDoneProcess);

CIRCQ_INDEX_DEFINE(RTPS_circQPortIdx1,RTPS_COMP_CIRCQ_SIZE);
static RTPS_BufInfo RTPS_circQPortBuffer1[RTPS_COMP_CIRCQ_SIZE];
static const CIRCQ_Type RTPS_CIRCQPort1 = CIRCQ_INIT(
                    &RTPS_circQPortIdx1, RTPS_COMP_CIRCQ_SIZE,
                    sizeof(RTPS_BufInfo),
                    (char *)RTPS_circQPortBuffer1,
                    CIRCQ_OVERWRITE_DISABLE);
#endif

#if (RTPS_COMP_INPUTPORT_2 < RTP_MAX_NUM_INPUT_PORTS)
MSYS_DECLARE_IN_PORT(RTPInPort2, 2UL, MSYS_COMP_ID_RTPS, SystemEvent6,
                       RTPInPortRW2, RTPS_FillBufferDoneProcess);

CIRCQ_INDEX_DEFINE(RTPS_circQPortIdx2,RTPS_COMP_CIRCQ_SIZE);
static RTPS_BufInfo RTPS_circQPortBuffer2[RTPS_COMP_CIRCQ_SIZE];
static const CIRCQ_Type RTPS_CIRCQPort2 = CIRCQ_INIT(
                    &RTPS_circQPortIdx2, RTPS_COMP_CIRCQ_SIZE,
                    sizeof(RTPS_BufInfo),
                    (char *)RTPS_circQPortBuffer2,
                    CIRCQ_OVERWRITE_DISABLE);
#endif

#if (RTPS_COMP_INPUTPORT_3 < RTP_MAX_NUM_INPUT_PORTS)
MSYS_DECLARE_IN_PORT(RTPInPort3, 3UL, MSYS_COMP_ID_RTPS, SystemEvent7,
                       RTPInPortRW3, RTPS_FillBufferDoneProcess);

CIRCQ_INDEX_DEFINE(RTPS_circQPortIdx3,RTPS_COMP_CIRCQ_SIZE);
static RTPS_BufInfo RTPS_circQPortBuffer3[RTPS_COMP_CIRCQ_SIZE];
static const CIRCQ_Type RTPS_CIRCQPort3 = CIRCQ_INIT(
                    &RTPS_circQPortIdx3, RTPS_COMP_CIRCQ_SIZE,
                    sizeof(RTPS_BufInfo),
                    (char *)RTPS_circQPortBuffer3,
                    CIRCQ_OVERWRITE_DISABLE);
#endif

/* Declare Input port buffers */
COMP_ALIGN(32)
static uint8_t COMP_SECTION(".data.rtp_sender")
            RTPS_PortData[RTP_MAX_NUM_INPUT_PORTS]
                         [RTPS_INPORT_NUM_BUFFERS]
                         [RTPS_INPORT_BUF_SIZE];

#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
/* Memory for creation of MPEGTS packets/tables for streaming */
COMP_ALIGN(32)
static uint8_t COMP_SECTION(".data.rtp_sender")
                RTPS_OutBufMem[RTP_MAX_NUM_INPUT_PORTS]
                              [RTPS_MAX_MPEGTS_BUFSIZE];
#else
/* Memory for creation of MPEGTS packets/tables for streaming */
COMP_ALIGN(32)
static uint8_t COMP_SECTION(".data.rtp_sender")
                RTPS_OutBufMem[RTP_MAX_NUM_INPUT_PORTS]
                              [RTPS_PAYLOAD_FRAGMENT_SIZE+32UL];
#endif

/* Memory to maintain Message queue data */
static MSYS_CmdType RTPSCommandQList[MSGQ_SIZE];

/* Message Q for Commands from Framework */
MSGQ_DEFINE_HDRQ(RTPS_CommandHdrQ);
static const MSGQ_Type SenderCommandQ = MSGQ_INIT(RTPS_ERIKA_TASK,
                            RTPS_EVENT_CMD_PROCESS,
                            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &RTPS_CommandHdrQ,
                            RTPS_GetMessage_CommandQList);

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
/* Memory to create and send RTCP packets from all ports */
static uint8_t RTPS_RTCPData[RTP_MAX_NUM_INPUT_PORTS][128UL];
#endif

/* Internal data structure of RTP Sender component */
static RTPS_StackHandle RTPS_Context = {0};

extern const MSYS_InPortType* RTPS_InPort[];
/* Pool of input port structure */
const MSYS_InPortType* RTPS_InPort[RTP_MAX_NUM_INPUT_PORTS] = {
#if (RTP_MAX_NUM_INPUT_PORTS>0)
    &RTPInPort0,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>1)
    &RTPInPort1,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>2)
    &RTPInPort2,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>3)
    &RTPInPort3,
#endif
};

extern const CIRCQ_Type * RTPS_CIRCQ[];
/* Array of Circular Queues used for component ports */
const CIRCQ_Type * RTPS_CIRCQ[RTP_MAX_NUM_INPUT_PORTS] = {
#if (RTP_MAX_NUM_INPUT_PORTS>0)
    &RTPS_CIRCQPort0,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>1)
    &RTPS_CIRCQPort1,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>2)
    &RTPS_CIRCQPort2,
#endif
#if (RTP_MAX_NUM_INPUT_PORTS>3)
    &RTPS_CIRCQPort3,
#endif
};

/***********************************************************************
 FUNCTION NAME: RTPS_SendCommand

 ARGUMENTS:
             cmdRespData -> The command-response structure with command
             clientMask  -> The client mask to be used to signal
                           on completion
             pHdr        -> The pointer to message header

 DESCRIPTION: Function to post the commands to the RTP Command Queue

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t RTPS_SendCommand (const MSYS_CmdType * const cmd,
                                 const uint32_t clientMask,
                                 const MSGQ_MsgHdrType** const pHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    int32_t status = 0U;

    if ((NULL != cmd)
        && (MSYS_COMP_ID_RTPS == cmd->compID)
        && (NULL != pHdr)) {
        if (cmd->cmd == MSYS_COMP_CMD_INIT) {
            status = BCM_ActivateTask(RTPS_ERIKA_TASK);
        }
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&SenderCommandQ, (const void * const)cmd,
                              clientMask, pHdr);
    }

    if ((0U != status) || (BCM_ERR_OK != retVal)) {
        RTPS_ReportError((uint8_t)RTPS_SEND_CMD_API_ID, retVal,
                         __LINE__, (uint32_t)status, 0UL, 0UL);
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: RTPS_RecvCmdResponse

 ARGUMENTS:
             pHdr       -> The pointer to message header
             cmdRespData-> The command-response structure with
                           response updated

 DESCRIPTION: Function to receive response for the commands posted

 RETURNS: The Message queue return code

***********************************************************************/
static int32_t RTPS_RecvCmdResponse (const MSGQ_MsgHdrType* const pHdr,
                                     MSYS_CmdType* const cmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != pHdr) && (NULL != cmd)) {
        retVal = MSGQ_RecvMsg(&SenderCommandQ, pHdr, (void* const) cmd);
    } else {
        RTPS_ReportError((uint8_t)RTPS_RECV_RESP_API_ID, retVal,
                        (uint32_t) __LINE__, (uint32_t)cmd,
                        (uint32_t)pHdr, 0UL);
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: RTPS_GetMediaOutPort

 ARGUMENTS:
            aIndex      Output port index

 DESCRIPTION: Function to get the MSYS_OutPortType where the message
              needs to be posted to

 RETURNS: For valid index       Pointer to Media outport structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_OutPortType* RTPS_GetMediaOutPort(const uint32_t aIndex)
{
    return NULL;
}

/***********************************************************************
 FUNCTION NAME: RTPS_GetMediaInPort

 ARGUMENTS: aIndex -> Input port index

 DESCRIPTION: Function to get the MSYS_InPortType

 RETURNS: For valid index       Pointer to Media input structure
          For invalid index     NULL
***********************************************************************/
static const MSYS_InPortType* RTPS_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *port = NULL;

    if (RTP_MAX_NUM_INPUT_PORTS > aIndex) {
        port = RTPS_InPort[aIndex];
    } else {
        RTPS_ReportError(RTPS_GET_MEDINPORT_API_ID,
                         BCM_ERR_INVAL_PARAMS,
                         __LINE__, 0UL, 0UL, 0UL);
    }
    return port;
}

/* Component function table for RTP */
static const MSYS_CompFnTblType RTPS_CompFnTable =
{
    RTPS_SendCommand,
    RTPS_RecvCmdResponse,
    RTPS_GetMediaOutPort,
    RTPS_GetMediaInPort,
};

/* Function to return Component function table for component */
const MSYS_CompFnTblType* RTPS_GetCompFnTable(void)
{
    return &RTPS_CompFnTable;
};

/***********************************************************************
 FUNCTION NAME: RTPS_HandleBufQueuing

 ARGUMENTS: portIndex -> input port index
            bufIndex  -> Buffer index

 DESCRIPTION: Function to queue the buffers to the peer component.

 RETURNS: None

***********************************************************************/
void RTPS_HandleBufQueuing (uint32_t portIndex, uint32_t bufIndex)
{
    int32_t retVal = 0;

    /* Update the media buffer structure */
    const MSYS_BufferType mediaBuffer = {
        .bufIdx = bufIndex,
        .numChannels = 1UL,
        .syncEnd = 0UL,
        .syncStart = 0UL,
        .frameStart = 0U,
        .frameEnd = 0U,
        .buffer = {
            [0] = {
                .data = &(RTPS_PortData[portIndex][bufIndex][0]),
                .size = RTPS_INPORT_BUF_SIZE,
                .attribs[RTPS_PORT_IDX_OFFSET] = portIndex,
               },
          },
      };

    /* Queue the buffer to the peer component */
    retVal = MSYS_FillBuffer(RTPS_InPort[portIndex], &mediaBuffer);
    if (BCM_ERR_OK != retVal) {
        /* Track the number of MSYS_FillBuffer API failures */
        RTPS_Context.sndrStats[portIndex].fillBufFailCount++;
        RTPS_ReportError(RTPS_HANDLE_BUF_QUEQING_API_ID, retVal,
                         __LINE__, 0UL, 0UL, 0UL);
    }

    return;
}

/***********************************************************************
 FUNCTION NAME: RTPS_FlushTxBuffers

 ARGUMENTS: portIndex -> Input port index

 DESCRIPTION: Function to dequeue the Tx buffers from the peer component.

 RETURNS: None

***********************************************************************/
void RTPS_FlushTxBuffers (uint32_t portIndex)
{
    int32_t retVal = 0L;

    /* Get back all Tx Buffers from peer component */
    retVal = MSYS_InPortCancelAll (RTPS_InPort[portIndex]);
    if((BCM_ERR_OK != retVal) && (BCM_ERR_CANCELLED != retVal)) {
        goto error;
    }

    /* Empty the circular Q */
    retVal = CIRCQ_Reset(RTPS_CIRCQ[portIndex]);
    if(BCM_ERR_OK != retVal) {
        goto error;
    }

    /* Clear the internal buffer context */
    BCM_MemSet(&RTPS_Context.circQData[portIndex], 0U, sizeof(RTPS_BufInfo));

error:
    if((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        RTPS_ReportError(RTPS_FLUSH_BUFFERS_API_ID, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
    return;
}

/***********************************************************************
 FUNCTION NAME: RTPS_DataProcess

 ARGUMENTS: portIndex -> Input port index

 DESCRIPTION: Function to invoke the Framework function to get the
              data that needs to be packetized and sent out.

 RETURNS: None

***********************************************************************/
void RTPS_DataProcess (uint32_t portIndex)
{
    int32_t retVal = 0;

    /* Check if port is enabled before proceeding */
    if (RTP_MAX_NUM_INPUT_PORTS <= portIndex) {
        goto error;
    }

    /* Call Framework Process in port function */
    retVal = MSYS_InPortProcess(RTPS_InPort[portIndex]);
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal)) {
        RTPS_ReportError(RTPS_DATA_PROCESS_API_ID, retVal,
                        (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

    /* If previous data has been processed and new data is present in Q */
    while ((RTP_NODATA_IN_PROCESS == RTPS_Context.circQData[portIndex].dataProcState) &&
           (1UL == CIRCQ_Pop(RTPS_CIRCQ[portIndex],
                            (char *)&RTPS_Context.circQData[portIndex],1UL))) {
        /* Set the data process flag and process it */
        RTPS_Context.circQData[portIndex].dataProcState = RTP_DATA_BUFFER_IN_USE;
        RTPS_CircQDataProcess(portIndex);
    }

error:
    return;
}

/***********************************************************************
 FUNCTION NAME: RTPS_FillBufferDoneProcess

 ARGUMENTS: aMediaBuffer -> Pointer to Framework's MSYS_BufferType

 DESCRIPTION: Function to receive the data payload, and process them

 RETURNS: None

***********************************************************************/
void RTPS_FillBufferDoneProcess (MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t portindex = 0UL;

    if ((NULL != aMediaBuffer) &&
        (NULL != aMediaBuffer->buffer[0].data)){

        MSYS_ChannelBufferType *mediaBuffer = &aMediaBuffer->buffer[0];
        RTPS_BufInfo qBuf = {0};

        /* update the buffer with incoming data details */
        qBuf.bufIdx    = aMediaBuffer->bufIdx;
        qBuf.procLen   = 0UL;
        qBuf.actualLen = aMediaBuffer->buffer[0].size;
        qBuf.timestamp = aMediaBuffer->timestamp;
        qBuf.syncStart = aMediaBuffer->syncStart;
        qBuf.syncEnd   = aMediaBuffer->syncEnd;
        qBuf.streamInfo = (uint32_t)
               aMediaBuffer->streamInfo[MSYS_STREAM_INFO_0];
        qBuf.frameStart = aMediaBuffer->frameStart;
        qBuf.frameEnd = aMediaBuffer->frameEnd;
        portindex = mediaBuffer->attribs[RTPS_PORT_IDX_OFFSET];

        /* Push this buffer into fill port Q to be processed later */
        retVal = CIRCQ_Push(RTPS_CIRCQ[portindex],
                           (char *)&qBuf,1UL);
        if(BCM_ERR_OK != retVal) {
            RTPS_ReportError(RTPS_FILL_BUFFER_DONE_PROCESS_API_ID,
                             retVal, (uint32_t)__LINE__,
                             portindex, 0UL, 0UL);
        }

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

#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
/***********************************************************************

 FUNCTION NAME: RTPS_SendMPEGTSTablePackets

 ARGUMENTS: portIndex -> Port index value

 DESCRIPTION: Function to create and transmit the PAT and PMT
              packets along with NULL packets.

 RETURNS: None

***********************************************************************/
static void RTPS_SendMPEGTSTablePackets(uint32_t portIndex)
{
    MPEGTS_DataBufferType inpBuf = {NULL, 0UL, 0UL};
    MPEGTS_DataBufferType outBuf = {NULL, 0UL, 0UL};
    RTP_DataBufferType outRTPBuf = {NULL, 0UL, 0UL};
    RTP_DataBufferType inRTPBuf = {NULL, 0UL, 0UL};
    int32_t retVal;
    uint32_t totalLen = 0UL;

    outRTPBuf.buffer = &RTPS_OutBufMem[portIndex][0];
    outRTPBuf.length = RTPS_MAX_MPEGTS_BUFSIZE;
    outRTPBuf.offset = 0UL;
    outBuf.length = 0UL;

    /* Create the RTP header */
    retVal = RTP_Packetize(&inRTPBuf, &outRTPBuf, &RTPS_Context.bufparam[portIndex]);
    if(BCM_ERR_OK != retVal ){
        goto error;
    }
    totalLen += outRTPBuf.length;

    /* Create the PAT and PMT table */
    outBuf.buffer = &RTPS_OutBufMem[portIndex][0] + outRTPBuf.length;
    retVal = MPEGTS_ConstructPacket(MPEGTS_PACKETTYPE_PAT_PMT,
                                    &inpBuf, &outBuf,
                                    &RTPS_Context.tsLibParam[portIndex]);
    if(BCM_ERR_OK != retVal ){
        goto error;
    }
    totalLen += outBuf.length;

    /* Send the data using INET UDP stack */
    retVal = UDP_SendTo(BCM_RTS_ID,
                        RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                        &RTPS_OutBufMem[portIndex][0],
                        (uint16_t)totalLen,
                        &RTPS_Context.bufparam[portIndex].strParams.strInfo.endPtCfg);

error:
    if (BCM_ERR_OK != retVal) {
        if (BCM_ERR_BUSY == retVal){
            RTPS_Context.sndrStats[portIndex].numEthL2BufUnderflow++;
            RTPS_Context.circQData[portIndex].dataProcState =
                                            RTP_MPEGTS_TABLES_UNDER_PROCESS;
        } else {
            RTPS_Context.sndrStats[portIndex].pktTxDiscard++;
            /* In case of an INET processing error */
            RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                             retVal, (uint32_t)__LINE__,
                             portIndex, outRTPBuf.length, outBuf.length);
        }
    }

    return;
}

/***********************************************************************
 FUNCTION NAME: RTPS_EnableMPEGTSinRTPPacketization

 ARGUMENTS: portIndex -> Port index value
            bufInfo -> Input payload details for processing

 DESCRIPTION: Function to process the data payload, add RTP header
              and TS header, packetize the H264 payload in PES packets
              before transmiting them over Ethernet.

 RETURNS: None

***********************************************************************/
static void RTPS_EnableMPEGTSinRTPPacketization (uint32_t portIndex,
                                                 RTPS_BufInfo *bufInfo)
{
    int32_t retVal = BCM_ERR_OK;
    MPEGTS_DataBufferType outBuf = {NULL, 0UL, 0UL};
    MPEGTS_DataBufferType inpBuf = {NULL, 0UL, 0UL};
    RTP_DataBufferType inRTPBuf = {NULL, 0UL, 0UL};
    RTP_DataBufferType outRTPBuf = {NULL, 0UL, 0UL};
    uint8_t updateStats  = 0U;

    RTPS_Context.tsLibParam[portIndex].strParams.payloadTimestamp =
                                                    bufInfo->timestamp;

    /* Set the frame start flag for TS header creation */
    if(1UL == bufInfo->frameStart){
        RTPS_Context.tsLibParam[portIndex].strParams.isStartOfESFrame = 1UL;
        bufInfo->frameStart = 0UL;

        /* Send the TS tables at every frame start */
        RTPS_SendMPEGTSTablePackets(portIndex);
        if (RTP_MPEGTS_TABLES_UNDER_PROCESS ==
                    RTPS_Context.circQData[portIndex].dataProcState){
            RTPS_Context.sndrStats[portIndex].numEthL2BufUnderflow++;
            goto error;
        }
    }

    /* Process data till completely consumed */
    while (bufInfo->actualLen > bufInfo->procLen) {

        outRTPBuf.buffer = &RTPS_OutBufMem[portIndex][0];
        outRTPBuf.length = RTPS_MAX_MPEGTS_BUFSIZE;
        outRTPBuf.offset = 0UL;

        /* Add the RTP header for the packet */
        retVal = RTP_Packetize(&inRTPBuf, &outRTPBuf,
                               &RTPS_Context.bufparam[portIndex]);
        if(BCM_ERR_OK != retVal ){
            goto error;
        }

        outBuf.buffer = &RTPS_OutBufMem[portIndex][0] +
                         outRTPBuf.length;
        outBuf.length = RTPS_MAX_MPEGTS_BUFSIZE - outRTPBuf.length;

        inpBuf.length = bufInfo->actualLen;
        inpBuf.offset = bufInfo->procLen;
        inpBuf.buffer = &RTPS_PortData[portIndex][bufInfo->bufIdx][0];

        /* Create the TS packet for payload (creates n TS per */
        /* call based on the configuration)                   */
        retVal = MPEGTS_ConstructPacket(MPEGTS_PACKETTYPE_PES,
                                        &inpBuf, &outBuf,
                                        &RTPS_Context.tsLibParam[portIndex]);
        if (BCM_ERR_OK != retVal) {
                /* In case of a processing error */
                RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                                 retVal, (uint32_t)__LINE__,
                                 RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                                 RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount,
                                 RTPS_Context.sndrStats[portIndex].pktTxDiscard);
        }

        /* Update the processed data length */
        bufInfo->procLen = inpBuf.offset;

        /* The total output length should consider the RTP header len */
        outBuf.length += outRTPBuf.length;

        /* Logging frame rate and stream bit rate */
        RTPS_LatencyStatistics(bufInfo->timestamp,outBuf.length);

        /* Send the data using INET UDP library */
        retVal = UDP_SendTo(BCM_RTS_ID,
                            RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                            &RTPS_OutBufMem[portIndex][0],
                            (uint16_t)outBuf.length,
                            &RTPS_Context.bufparam[portIndex].strParams.strInfo.endPtCfg);
        if (BCM_ERR_OK != retVal) {
            /* In case UDP could not send out the buffer */
            if (BCM_ERR_BUSY == retVal) {
                RTPS_Context.sndrStats[portIndex].numEthL2BufUnderflow++;
                RTPS_Context.circQData[portIndex].dataProcState =
                                                RTP_MEDIADATA_UNDER_PROCESS;
                updateStats = 1U;
                goto error;
            } else {
                RTPS_Context.sndrStats[portIndex].pktTxDiscard++;
                /* In case of an INET processing error */
                RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                                 retVal, (uint32_t)__LINE__,
                                 RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                                 RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount,
                                 RTPS_Context.sndrStats[portIndex].pktTxDiscard);
            }
        } else {
            updateStats = 1U;
            RTPS_Context.sndrStats[portIndex].pktTxCount++;
        }

        /* Reset CircQ temporary memory when data is fully processed */
        if (bufInfo->procLen == bufInfo->actualLen) {
            /* If input buffer is completely processed then put it back */
            /* into CircQ to be queued back to upstream component       */
            bufInfo->dataProcState = RTP_NODATA_IN_PROCESS;
            /* Send buffer back to upstream component */
            RTPS_HandleBufQueuing(portIndex, bufInfo->bufIdx);
        }
    }

error:
    if (1U == updateStats) {
        /* Update the size of total RTP packets sent to be used by RTCP. */
        /* Updating this length as until the entire UDP payload is sent  */
        /* out with UDP_Process, the next frame/buffer will not taken for*/
        /* processing, thus avoiding an incorrect RTCP packet.           */
        RTPS_Context.bufparam[portIndex].strInfo.payloadBytesSent +=
                                                outBuf.length;
        /* Increment the number of RTP packets sent for RTCP usage */
        RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount++;
    }

    return;
}

#else

/***********************************************************************
 FUNCTION NAME: RTPS_EnableRTPPacketization

 ARGUMENTS: portIndex -> Port index value
            bufInfo -> Input payload details for processing

 DESCRIPTION: Function to process the data payload, add RTP (RFC6184)
              header for the payload and later transmit the
              packets over Ethernet

 RETURNS: None

***********************************************************************/
static void RTPS_EnableRTPPacketization (uint32_t portIndex,
                                         RTPS_BufInfo *bufInfo)
{
    int32_t retVal = BCM_ERR_OK;
    RTP_DataBufferType inpBuf;
    RTP_DataBufferType outBuf;
    uint8_t updateStats  = 0U;
    uint32_t payloadSize = 0UL;

    /* Process entire buffer data as fragments of RTPS_PAYLOAD_FRAGMENT_SIZE */
    while (bufInfo->actualLen > bufInfo->procLen) {

        payloadSize = 0UL;

        /* Check data bytes present and determine payload size */
        if (RTPS_PAYLOAD_FRAGMENT_SIZE < (bufInfo->actualLen -
                                            bufInfo->procLen)) {

            payloadSize = RTPS_PAYLOAD_FRAGMENT_SIZE;

            /* Send slice/frame end only for last group of data */
            RTPS_Context.bufparam[portIndex].strInfo.isSliceEnd = 0UL;
            RTPS_Context.bufparam[portIndex].strInfo.isEndofFrame = 0UL;

        } else {

            /* Case when <= RTPS_PAYLOAD_FRAGMENT_SIZE bytes of payload */
            payloadSize = bufInfo->actualLen - bufInfo->procLen;

            /* Send the slice end flag for marking in RTP header */
            RTPS_Context.bufparam[portIndex].strInfo.isSliceEnd = bufInfo->syncEnd;
            RTPS_Context.bufparam[portIndex].strInfo.isEndofFrame = bufInfo->frameEnd;
        }

        /* Check and update if its metadata */
        if((0x67UL == bufInfo->streamInfo) ||
           (0x68UL == bufInfo->streamInfo)) {
            RTPS_Context.bufparam[portIndex].strInfo.isMetaData = 1UL;
        } else {
            RTPS_Context.bufparam[portIndex].strInfo.isMetaData = 0UL;
            if(1UL == bufInfo->syncStart) {
                RTPS_Context.sndrStats[portIndex].sliceStartCount++;
            }
        }

        inpBuf.length = payloadSize;
        inpBuf.offset = 0UL;
        inpBuf.buffer = &RTPS_PortData[portIndex][bufInfo->bufIdx][0] +
                         bufInfo->procLen;

        /* RTP header is mostly around 14 bytes. The output buffer pointer is arranged */
        /* such that after addition of 14 bytes of header, the buffer pointer is       */
        /* aligned to 4 bytes. This alignment is needed as we later copy the payload   */
        /* bytes into the output buffer. The memcpy is more optimized when the pointers*/
        /* are aligned to 4 bytes. The RTP header is 12 bytes and the FUA header is    */
        /* two bytes. To make it 4-byte aligned, we offset the memory buffer by 2 bytes*/
        outBuf.buffer = &RTPS_OutBufMem[portIndex][2];
        outBuf.length = 0UL;
        outBuf.offset = 0UL;

        retVal = RTP_Packetize(&inpBuf,
                               &outBuf,
                               &RTPS_Context.bufparam[portIndex]);
        if (BCM_ERR_OK != retVal) {
            RTPS_Context.sndrStats[portIndex].pktTxHdrAddnErrors++;
            RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                        retVal, (uint32_t)__LINE__,
                        inpBuf.length, outBuf.length, inpBuf.offset);
            goto error;
        }

        /* Reset the sync start marker to avoid sending multiple */
        /* sync start markings                                   */
        bufInfo->syncStart = 0UL;
        RTPS_Context.bufparam[portIndex].strInfo.isSliceStart = 0UL;

        /* Update the processed data length */
        bufInfo->procLen += payloadSize;

        /* Logging frame rate and stream bit rate */
        RTPS_LatencyStatistics(bufInfo->timestamp,outBuf.length);

        /* Send the data using INET UDP stack */
        retVal = UDP_SendTo(BCM_RTS_ID,
                            RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                            outBuf.buffer,
                            (uint16_t)outBuf.length,
                            &RTPS_Context.bufparam[portIndex].strParams.strInfo.endPtCfg);
        if (BCM_ERR_OK != retVal) {
            /* In case UDP could not send out the buffer */
            if (BCM_ERR_BUSY == retVal) {
                RTPS_Context.sndrStats[portIndex].numEthL2BufUnderflow++;
                RTPS_Context.circQData[portIndex].dataProcState =
                                                RTP_MEDIADATA_UNDER_PROCESS;
                updateStats = 1U;
                goto error;
            } else {
                RTPS_Context.sndrStats[portIndex].pktTxDiscard++;
                /* In case of an INET processing error */
                RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                                 retVal, (uint32_t)__LINE__,
                                 RTPS_Context.bufparam[portIndex].strParams.strInfo.rtpPayloadID,
                                 RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount,
                                 RTPS_Context.sndrStats[portIndex].pktTxDiscard);
            }
        } else {
            updateStats = 1U;
            RTPS_Context.sndrStats[portIndex].pktTxCount++;
        }

        /* Reset CIRCQ temporary memory when data is fully processed */
        if (bufInfo->procLen == bufInfo->actualLen) {

            /* If input buffer is completely processed then put it back */
            /* into CIRCQ to be queued back to upstream component       */
            bufInfo->dataProcState = RTP_NODATA_IN_PROCESS;
            /* Send buffer back to upstream component */
            RTPS_HandleBufQueuing(portIndex, bufInfo->bufIdx);
        }
    }

error:
    if (1U == updateStats) {
        /* Update the size of total RTP packets sent to be used by RTCP. */
        /* Updating this length as until the entire UDP payload is sent  */
        /* out with UDP_Process, the next frame/buffer will not taken for*/
        /* processing, thus avoiding an incorrect RTCP packet.           */
        RTPS_Context.bufparam[portIndex].strInfo.payloadBytesSent +=
                                                outBuf.length;
        /* Increment the number of RTP packets sent for RTCP usage */
        RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount++;
    }
    return;
}

#endif

/***********************************************************************
 FUNCTION NAME: RTPS_CircQDataProcess

 ARGUMENTS: portIndex -> Port index value

 DESCRIPTION: Function to process the data payload, add RTP header
              and send the packets over Ethernet

 RETURNS: None

***********************************************************************/
void RTPS_CircQDataProcess (uint32_t portIndex)
{
    RTPS_BufInfo *bufInfo = NULL;

    /* Update the buffer param for processing */
    bufInfo = &RTPS_Context.circQData[portIndex];

    /* Copy the buffer parameters */
    RTPS_Context.bufparam[portIndex].strInfo.isSliceStart = bufInfo->syncStart;
    RTPS_Context.bufparam[portIndex].strInfo.streamInfo = bufInfo->streamInfo;
    RTPS_Context.bufparam[portIndex].strInfo.timeStamp = bufInfo->timestamp;

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
    /* Send 1733 RTCP packet once at every video frame start and for NAL data */
    if ((1UL == RTPS_Context.bufparam[portIndex].strParams.rtcpCfg.isRTCPEnabled) &&
        (1UL == bufInfo->syncEnd) &&
        (0x65UL == bufInfo->streamInfo) &&
        (RTP_DATA_BUFFER_IN_USE == RTPS_Context.circQData[portIndex].dataProcState)) {
        int32_t retVal = BCM_ERR_OK;
        RTP_DataBufferType rtcpBuf = {&RTPS_RTCPData[portIndex][0], 128UL, 0UL};

        /* After each frame, transmit the RTCP 1733 packet */
        retVal = RTCP_Create1733SenderReport(
                                &rtcpBuf,
                                &RTPS_Context.bufparam[portIndex]);
        if (BCM_ERR_OK == retVal) {
            /* Send the data using INET UDP stack */
            retVal = UDP_SendTo(BCM_RTS_ID,
                                RTPS_Context.bufparam[portIndex].strParams.rtcpCfg.rtcpPayloadID,
                                rtcpBuf.buffer,
                                (uint16_t)rtcpBuf.length,
                                &RTPS_Context.bufparam[portIndex].strParams.rtcpCfg.endPtCfg);
            if (BCM_ERR_OK != retVal) {
                /* In case UDP could not send out the buffer */
                if (BCM_ERR_BUSY == retVal) {
                    RTPS_Context.sndrStats[portIndex].numEthL2BufUnderflow++;
                    RTPS_Context.circQData[portIndex].dataProcState =
                                                    RTP_1733_UNDER_PROCESS;
                    goto error;
                } else {
                    RTPS_Context.sndrStats[portIndex].pktTxDiscard++;
                    /* In case of an INET processing error */
                    RTPS_ReportError(RTPS_CIRCQ_PROCESS_API_ID,
                                     retVal, (uint32_t)__LINE__,
                                     RTPS_Context.bufparam[portIndex].strParams.rtcpCfg.rtcpPayloadID,
                                     RTPS_Context.bufparam[portIndex].strInfo.rtpPktCount,
                                     RTPS_Context.sndrStats[portIndex].pktTxDiscard);
                }
            } else {
                RTPS_Context.sndrStats[portIndex].pktRTCPTxCount++;
            }
        }
    }
#endif

#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
    RTPS_EnableMPEGTSinRTPPacketization(portIndex, bufInfo);
#else
    RTPS_EnableRTPPacketization(portIndex, bufInfo);
#endif

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
error:
#endif
    return;
}

/***********************************************************************
 FUNCTION NAME: RTPS_ResetStateHandler

 ARGUMENTS: None

 DESCRIPTION: Reset state handling

 RETURNS: None
***********************************************************************/
static void RTPS_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t localCnt = 0UL;
    uint32_t portIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;

    retVal = MSGQ_GetMsgIdx(&SenderCommandQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPSCommandQList[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {

            /* Reset the entire handle region */
            BCM_MemSet(&RTPS_Context, 0U,
                       sizeof(RTPS_Context));

            /* Initialize all the input ports */
            for (portIdx = 0UL;
                 portIdx < RTP_MAX_NUM_INPUT_PORTS;
                 portIdx++) {

                /* Invoke framework function to initialize input port */
                retVal = MSYS_InPortInit (RTPS_InPort[portIdx]);
                if(BCM_ERR_OK != retVal){
                    goto error;
                }

                /* Initialize the circular queue */
                retVal = CIRCQ_Reset(RTPS_CIRCQ[portIdx]);
                if(BCM_ERR_OK != retVal){
                    goto error;
                }
            }

            /* Update component state */
            RTPS_Context.numInpPort = 0UL;
            RTPS_Context.compState = RTPS_COMP_STATE_INIT;
            RTPS_Context.configCount = 0UL;
            processCmd->response = BCM_ERR_OK;
        }

error:
        /* Single location error check with all args */
        if (BCM_ERR_OK != retVal){
             RTPS_ReportError(RTPS_RESET_STATE_HANDLER_API_ID,
                              retVal, (uint32_t) __LINE__,
                              portIdx, localCnt, 0UL);
        }

        retVal = MSGQ_CompleteMsgIdx(&SenderCommandQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            RTPS_ReportError(RTPS_RESET_STATE_HANDLER_API_ID, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }

    return;
}

/**
*/
typedef union _RTPS_ConfigDataType {
    const uint8_t* hdr;
    const RTPS_Config *rtpsImg;
    const MPEGTS_LibConfigType *tsImg;
} RTPS_ConfigDataType;

/***********************************************************************
 FUNCTION NAME: RTPS_InitStateHandler

 ARGUMENTS: None

 DESCRIPTION: Init state handling

 RETURNS: None
***********************************************************************/
static void RTPS_InitStateHandler(void)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    uint32_t portIdx = 0UL;
    int32_t retVal = BCM_ERR_OK;
    RTPS_ConfigDataType config;
#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
    uint32_t expConfigCount = 2UL;
#else
    uint32_t expConfigCount = 1UL;
#endif
    retVal = MSGQ_GetMsgIdx(&SenderCommandQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPSCommandQList[cmdMsgIdx];
        config.hdr = processCmd->cfgData;
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            switch(processCmd->configID & 0xFFF0UL) {
            case RTPS_STR_CONFIG:{
                /* Check for the number of configured ports */
                if (RTP_MAX_NUM_INPUT_PORTS != config.rtpsImg->numInpPort){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    RTPS_ReportError(RTPS_INIT_STATE_HANDLER_API_ID,
                                     retVal, (uint32_t) __LINE__,
                                     (uint32_t) RTP_MAX_NUM_INPUT_PORTS,
                                     (uint32_t) config.rtpsImg->numInpPort,
                                     0UL);
                } else {
                    /* Store the number of input ports activated */
                    RTPS_Context.numInpPort = config.rtpsImg->numInpPort;

                    /* Store the stream configuration */
                    for (portIdx = 0;
                         portIdx < config.rtpsImg->numInpPort;
                         portIdx++) {
                        /* Store the stream configuration */
                        RTPS_Context.bufparam[portIdx].strParams =
                                    config.rtpsImg->inpStreamCfg[portIdx];
                    }
                    RTPS_Context.configCount++;
                    processCmd->response = BCM_ERR_OK;
                }
                break;
            }
#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
            case RTPS_MPEGTS_CONFIG:{
                /* Store the mpegts library configuration */
                for (portIdx = 0;
                     portIdx < RTPS_Context.numInpPort;
                     portIdx++) {
                    BCM_MemCpy(&RTPS_Context.tsLibParam[portIdx].strParams.strCFg,
                               config.tsImg, sizeof(MPEGTS_LibConfigType));
                    /* Reset the library context memory */
                    BCM_MemSet(&RTPS_Context.tsLibParam[portIdx].libParam,
                               0x0U,sizeof(MPEGTS_PacketizerLibContextType));
                }
                RTPS_Context.configCount++;
                processCmd->response = BCM_ERR_OK;
                break;
            }
#endif
            default:{
            RTPS_ReportError(RTPS_INIT_STATE_HANDLER_API_ID,
                             processCmd->response,
                            (uint32_t)__LINE__, 0UL, 0UL, 0UL);
            break;
            }
            }
        }

        /* Only after all the config images are received, update */
        /* the component state                                   */
        if(expConfigCount == RTPS_Context.configCount){
            RTPS_Context.compState = RTPS_COMP_STATE_READY;
        }

        retVal = MSGQ_CompleteMsgIdx(&SenderCommandQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            RTPS_ReportError(RTPS_INIT_STATE_HANDLER_API_ID, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }

}


/***********************************************************************
 FUNCTION NAME: RTPS_ReadyStateHandler

 ARGUMENTS: None

 DESCRIPTION: Ready state handling

 RETURNS: None
***********************************************************************/
static void RTPS_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd    = NULL;
    int32_t retVal     = BCM_ERR_OK;
    uint32_t portIdx   = 0UL;
    uint32_t bufIdx    = 0UL;
    RTPS_ConfigDataType config;

    retVal = MSGQ_GetMsgIdx(&SenderCommandQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPSCommandQList[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            RTPS_Context.startCmdInProgress = 1UL;
            /* For all enabled ports of RTPS component */
            for(portIdx = 0UL;
                portIdx < RTPS_Context.numInpPort;
                portIdx++) {
                for(bufIdx = 0UL;
                    bufIdx < RTPS_INPORT_NUM_BUFFERS;
                    bufIdx++){
                    /* Queueing the Tx buffers */
                    RTPS_HandleBufQueuing(portIdx, bufIdx);
                }
            }
            /* Update component state */
            RTPS_Context.compState = RTPS_COMP_STATE_START;
            RTPS_Context.startCmdInProgress = 0UL;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            config.hdr = processCmd->cfgData;
            switch(processCmd->configID & 0xFFF0UL) {
            case RTPS_STR_CONFIG:{
                /* Check for the number of configured ports */
                if (RTP_MAX_NUM_INPUT_PORTS != config.rtpsImg->numInpPort){
                    retVal = BCM_ERR_INVAL_PARAMS;
                    RTPS_ReportError(RTPS_READY_STATE_HANDLER_API_ID,
                                     retVal, (uint32_t) __LINE__,
                                     (uint32_t) RTP_MAX_NUM_INPUT_PORTS,
                                     (uint32_t) config.rtpsImg->numInpPort,
                                     0UL);
                } else {
                    /* Store the number of input ports activated */
                    RTPS_Context.numInpPort = config.rtpsImg->numInpPort;

                    /* Store the stream configuration */
                    for (portIdx = 0;
                         portIdx < config.rtpsImg->numInpPort;
                         portIdx++) {
                        /* Store the stream configuration */
                        RTPS_Context.bufparam[portIdx].strParams =
                                    config.rtpsImg->inpStreamCfg[portIdx];
                    }
                    RTPS_Context.configCount++;
                    processCmd->response = BCM_ERR_OK;
                }
                break;
            }
#ifdef RTP_ENABLE_MPEGTS_PACKETIZATION
            case RTPS_MPEGTS_CONFIG:{
                /* Store the mpegts library configuration */
                for (portIdx = 0;
                     portIdx < RTPS_Context.numInpPort;
                     portIdx++) {
                    BCM_MemCpy(&RTPS_Context.tsLibParam[portIdx].strParams.strCFg,
                               config.tsImg, sizeof(MPEGTS_LibConfigType));
                    /* Reset the library context memory */
                    BCM_MemSet(&RTPS_Context.tsLibParam[portIdx].libParam,
                               0x0U,sizeof(MPEGTS_PacketizerLibContextType));
                }
                RTPS_Context.configCount++;
                processCmd->response = BCM_ERR_OK;
                break;
            }
#endif
            default:{
            RTPS_ReportError(RTPS_READY_STATE_HANDLER_API_ID,
                             processCmd->response,
                            (uint32_t)__LINE__, 0UL, 0UL, 0UL);
            break;
            }
            }
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            RTPS_Context.compState = RTPS_COMP_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        retVal = MSGQ_CompleteMsgIdx(&SenderCommandQ, cmdMsgIdx);
        if (BCM_ERR_OK != retVal) {
            RTPS_ReportError(RTPS_READY_STATE_HANDLER_API_ID, retVal,
                             __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/***********************************************************************
 FUNCTION NAME: RTPS_RunStateHandler

 ARGUMENTS: eventVal -> The list of events that needs processing

 DESCRIPTION: Running state handling

 RETURNS: None
***********************************************************************/
static void RTPS_RunStateHandler(EventMaskType eventVal)
{
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t retVal = BCM_ERR_OK;
    uint32_t portIdx = 0UL;

    retVal = MSGQ_GetMsgIdx(&SenderCommandQ, &cmdMsgIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &RTPSCommandQList[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(processCmd->cmd == MSYS_COMP_CMD_STOP) {
            RTPS_Context.stopCmdInProgress = 1UL;
        }
    }

    /* If STOP command was not received, then continue to */
    /* processing data                                    */
    if(0UL == RTPS_Context.stopCmdInProgress) {

        if (0U != (eventVal & RTPS_EVENT_DATA_TXBUF_Q)) {

            /* Track the Tx events using port-0 stats */
            RTPS_Context.sndrStats[0].pktTxConfEventRecvd++;

            /* Check if any data on any port is pending process */
            for (portIdx = 0UL;
                 portIdx < RTPS_Context.numInpPort;
                 portIdx++) {
                 RTPS_BufInfo *bufInfo = &RTPS_Context.circQData[portIdx];

                /* Case when Tx confirmation trigger is not needed */
                if(RTP_DATA_BUFFER_IN_USE >= bufInfo->dataProcState){
                    continue;
                /* Case when the PAT/PMT/PCR packets are to be sent */
                } else if(RTP_MPEGTS_TABLES_UNDER_PROCESS == bufInfo->dataProcState){
                    retVal = UDP_Process(BCM_RTS_ID,
                                         RTPS_Context.bufparam[portIdx].strParams.strInfo.rtpPayloadID);
                    if (BCM_ERR_BUSY == retVal) {
                        break;
                    } else if (BCM_ERR_OK == retVal) {
                        RTPS_Context.sndrStats[portIdx].pktTxCount++;
                    } else {
                        RTPS_ReportError(RTPS_RUN_STATE_HANDLER_API_ID, retVal,
                                         __LINE__, portIdx, 0UL, 0UL);
                    }

                    /* Set flag to process media payload data */
                    bufInfo->dataProcState = RTP_DATA_BUFFER_IN_USE;
                    /* Call the data process func to send media payload */
                    RTPS_CircQDataProcess(portIdx);
                /* Case when RTCP packet is pending to be sent */
                } else if(RTP_1733_UNDER_PROCESS == bufInfo->dataProcState) {
                    retVal = UDP_Process(BCM_RTS_ID,
                                         RTPS_Context.bufparam[portIdx].strParams.rtcpCfg.rtcpPayloadID);
                    if (BCM_ERR_BUSY == retVal) {
                        break;
                    } else if (BCM_ERR_OK == retVal) {
                        RTPS_Context.sndrStats[portIdx].pktRTCPTxCount++;
                    } else {
                        RTPS_ReportError(RTPS_RUN_STATE_HANDLER_API_ID, retVal,
                                         __LINE__, portIdx, 0UL, 0UL);
                    }

                    /* Set flag to process media payload data */
                    bufInfo->dataProcState = RTP_MEDIADATA_UNDER_PROCESS;
                    /* Call the data process func to send media payload */
                    RTPS_CircQDataProcess(portIdx);

                /* Case when media payload packet is pending to be sent */
                } else {
                    retVal = UDP_Process(BCM_RTS_ID,
                                         RTPS_Context.bufparam[portIdx].strParams.strInfo.rtpPayloadID);
                    if (BCM_ERR_BUSY == retVal) {
                        break;
                    } else if (BCM_ERR_OK == retVal) {
                        RTPS_Context.sndrStats[portIdx].pktTxCount++;
                    } else {
                        RTPS_ReportError(RTPS_RUN_STATE_HANDLER_API_ID, retVal,
                                         __LINE__, portIdx, 0UL, 0UL);
                        RTPS_Context.sndrStats[portIdx].pktTxDiscard++;
                    }

                    /* In case of completing remaining data Tx, marking */
                    /* data processing as complete and sending buffer   */
                    /* back to the upstream component                   */
                    bufInfo->dataProcState = RTP_NODATA_IN_PROCESS;
                    /* Send buffer back to upstream component */
                    RTPS_HandleBufQueuing(portIdx, bufInfo->bufIdx);
                }
            }
        }

        /* If a data process trigger was got on Port-0 */
        if (0U != (eventVal & RTPS_EVENT_INPPORT0_PROCESS)) {
            RTPS_DataProcess(RTPS_COMP_INPUTPORT_0);
        }

        /* If a data process trigger was got on Port-1 */
        if (0U != (eventVal & RTPS_EVENT_INPPORT1_PROCESS)) {
            RTPS_DataProcess(RTPS_COMP_INPUTPORT_1);
        }

        /* If a data process trigger was got on Port-2 */
        if (0U != (eventVal & RTPS_EVENT_INPPORT2_PROCESS)) {
            RTPS_DataProcess(RTPS_COMP_INPUTPORT_2);
        }

        /* If a data process trigger was got on Port-3 */
        if (0U != (eventVal & RTPS_EVENT_INPPORT3_PROCESS)) {
            RTPS_DataProcess(RTPS_COMP_INPUTPORT_3);
        }
    } else {

        /* Flush buffers of all input ports */
        for (portIdx = 0UL;
             portIdx < RTPS_Context.numInpPort;
             portIdx++) {
            (void)UDP_CancelSend(BCM_RTS_ID,
                                 RTPS_Context.bufparam[portIdx].strParams.rtcpCfg.rtcpPayloadID);
            (void)UDP_CancelSend(BCM_RTS_ID,
                                 RTPS_Context.bufparam[portIdx].strParams.strInfo.rtpPayloadID);
            /* De-Queue Tx buffers from and return it to driver */
            RTPS_FlushTxBuffers(portIdx);
        }

        if (cmdMsgIdx < MSGQ_SIZE) {
            RTPSCommandQList[cmdMsgIdx].response = BCM_ERR_OK;
            retVal = MSGQ_CompleteMsgIdx(&SenderCommandQ, cmdMsgIdx);
            if (BCM_ERR_OK != retVal) {
                RTPS_ReportError(RTPS_RUN_STATE_HANDLER_API_ID, retVal,
                                 __LINE__, 0UL, 0UL, 0UL);
            }
        }
        RTPS_Context.configCount = 0UL;
        RTPS_Context.compState = RTPS_COMP_STATE_READY;
        RTPS_Context.stopCmdInProgress = 0UL;
    }

}

TASK(RTPS_Task)
{
    BCM_EventMaskType mask = 0UL;
    int32_t status = 0L;

    do {

        status = BCM_WaitEvent(RTPS_EVENT_CMD_PROCESS      |
                           RTPS_EVENT_INPPORT0_PROCESS |
                           RTPS_EVENT_INPPORT1_PROCESS |
                           RTPS_EVENT_INPPORT2_PROCESS |
                           RTPS_EVENT_INPPORT3_PROCESS |
                           RTPS_EVENT_DATA_TXBUF_Q);
        if (BCM_ERR_OK != status) {
            RTPS_ReportError(RTPS_TASK_FUNC_API_ID, status,
                           (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_GetEvent(RTPS_ERIKA_TASK, &mask);
        if (BCM_ERR_OK != status) {
            RTPS_ReportError(RTPS_TASK_FUNC_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        status = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != status) {
            RTPS_ReportError(RTPS_TASK_FUNC_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }

        switch(RTPS_Context.compState) {
        case RTPS_COMP_STATE_RESET:
            RTPS_ResetStateHandler();
            break;
        case RTPS_COMP_STATE_INIT:
            RTPS_InitStateHandler();
            break;
        case RTPS_COMP_STATE_READY:
            RTPS_ReadyStateHandler();
            break;
        case RTPS_COMP_STATE_START:
            RTPS_RunStateHandler((EventMaskType)mask);
            break;
        default:
            break;
        }
    }while(RTPS_COMP_STATE_RESET != RTPS_Context.compState);

    status = BCM_TerminateTask();
    if (BCM_ERR_OK != status) {
        RTPS_ReportError(RTPS_TASK_FUNC_API_ID, status,
                            (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
}

/***********************************************************************
 FUNCTION NAME: RTPLatencyStatistics

 ARGUMENTS: timestamp -> 64-bit timestamp

 DESCRIPTION: Function to Print Latency Statistics

 RETURNS: None

***********************************************************************/
void RTPS_LatencyStatistics(uint64_t timestamp, uint32_t size)
{
    static uint64_t oldtime = 0ULL;
    static uint64_t fstime = 0ULL;
    static uint32_t dataSize = 0UL;
    static uint32_t AvgLatency = 0UL;
    static uint32_t MinLatency = 0xFFFFFFFFUL;
    static uint32_t MaxLatency = 0UL;
    static uint32_t Counter = 0UL;
#define COUNTER_MAX     (90ULL)
#define MS_PER_SEC      (1000ULL)
#define BITS_PER_BYTE   (8ULL)

    dataSize += size;
    if (oldtime != timestamp) {
        BCM_TimeType t1 = {0UL, 0UL};
        uint64_t ts;
        uint32_t latency;
        uint64_t fps;

        oldtime = timestamp;
        BCM_GetTime(&t1);
        ts = (((uint64_t)(t1.s)) * BCM_NS_PER_SEC) + t1.ns;
        latency = (uint32_t)((ts - timestamp)/MS_PER_SEC);
        AvgLatency = (15UL *AvgLatency + latency) >> 4UL;
        if (MinLatency > latency) {
            MinLatency = latency;
        }
        if (MaxLatency < latency) {
            MaxLatency = latency;
        }
        Counter++;
        if (Counter == COUNTER_MAX) {
            uint64_t bitrate = (dataSize* BITS_PER_BYTE * MS_PER_SEC)
                                          /(timestamp - fstime);
            fps = (COUNTER_MAX * BCM_NS_PER_SEC)/(timestamp - fstime);
            ULOG_ERR("FPS[ %u ]  BitRate[ %u Mbps ] \n",
                            (uint32_t)fps, (uint32_t)bitrate);
            Counter = 0UL;
            dataSize = 0UL;
            fstime = timestamp;
        }
    }
}

/***********************************************************************
 FUNCTION NAME: RTPS_GetMessage_CommandQList

 ARGUMENTS: i-> The index of message queue from which MSG needs to
                be got

 DESCRIPTION: Function to get the MSGQ_MsgHdrType from CommandQList

 RETURNS: The Message queue header

***********************************************************************/
void* RTPS_GetMessage_CommandQList (uint32_t idx)
{
    void *avbPtr = NULL;

    if (idx < MSGQ_SIZE) {
        avbPtr = (void *)(&RTPSCommandQList[idx]);
    } else {
        avbPtr = NULL;
    }

    return avbPtr;
}

/* Check the RTPS state to verify if data buffers can be processed */
int32_t RTPS_CheckProcessingState(void)
{
    int32_t retVal = BCM_ERR_INVAL_STATE;

    /* Tx Buffers are to be queued to Peer Encoder component only if,      */
    /* 1. The RTPS component is in Ready State and has got a Start Command */
    /*                        OR                                           */
    /* 2. RTPS component is in Start state, without processing any command */
    if (((RTPS_COMP_STATE_READY == RTPS_Context.compState) &&
         (RTPS_TRUE == RTPS_Context.startCmdInProgress)) ||
        ((RTPS_COMP_STATE_START == RTPS_Context.compState) &&
         (RTPS_TRUE != RTPS_Context.stopCmdInProgress))) {
         retVal = BCM_ERR_OK;
    }

    return retVal;
}

/***********************************************************************
 FUNCTION NAME: RTPS_ReportError

 ARGUMENTS: aApiID -> API ID invoking the logging
            aErr -> Error code to be logged
            aVal0, aVal1, aVal2, aVal3 -> Values

 DESCRIPTION: Function to be used for logging

 RETURNS: None

***********************************************************************/
void RTPS_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                     uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_RTS_ID, RTPS_INSTANCE_ID, aApiID,
                    aErr, 4UL, values);
}
