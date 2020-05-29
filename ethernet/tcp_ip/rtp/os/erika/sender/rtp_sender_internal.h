/*****************************************************************************
Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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

#ifndef RTP_SENDER_INTERNAL_H
#define RTP_SENDER_INTERNAL_H

#include <udp.h>
#include "rtp_config.h"
#include <msg_queue.h>
#include <inet.h>

#define RTPS_TRUE                  (0x1U)
#define RTPS_FALSE                 (0x0U)

/** RTP instance ID for error reporting */
#define RTPS_INSTANCE_ID           (0U)

/** RTP internal functions IDs for error reporting */
#define RTPS_FILL_BUFFER_DONE_PROCESS_API_ID    0xC1U
#define RTPS_CHECK_PROC_STATE_API_ID            0xC2U
#define RTPS_CIRCQ_PROCESS_API_ID               0xC3U
#define RTPS_HANDLE_BUF_QUEQING_API_ID          0xC4U
#define RTPS_RESET_STATE_HANDLER_API_ID         0xC5U
#define RTPS_INIT_STATE_HANDLER_API_ID          0xC6U
#define RTPS_READY_STATE_HANDLER_API_ID         0xC7U
#define RTPS_RUN_STATE_HANDLER_API_ID           0xC8U
#define RTPS_FLUSH_BUFFERS_API_ID               0xC9U
#define RTPS_DATA_PROCESS_API_ID                0xCAU
#define RTPS_SEND_CMD_API_ID                    0xCBU
#define RTPS_RECV_RESP_API_ID                   0xCCU
#define RTPS_GET_MEDINPORT_API_ID               0xCDU

#define RTPS_TASK_FUNC_API_ID                   0x41U

#define RTPS_COMP_INPUTPORT_0     0UL
#define RTPS_COMP_INPUTPORT_1     1UL
#define RTPS_COMP_INPUTPORT_2     2UL
#define RTPS_COMP_INPUTPORT_3     3UL

#define RTPS_INPORT_NUM_BUFFERS    (4UL)
#define RTPS_MAX_MPEGTS_BUFSIZE    (1216UL)

/* At 50MBPS, each encoded MB row size shall be <= 4KB    */
/* Hence the closest L2 frame size which is multiple      */
/* of 4KB is selected                                     */
#define RTPS_INPORT_BUF_SIZE       (RTPS_PAYLOAD_FRAGMENT_SIZE * 5UL)

/**********************************************************************
                    Internal Data Structures
**********************************************************************/

/** Statistics maintained by the RTPS component. */
typedef struct _RTP_Sender_Stats{
    /* Number of RTP packets transmitted successfully */
    uint32_t pktTxCount;

    /* Number of Tx packets discarded (error in sending) */
    uint32_t pktTxDiscard;

    /* Number of Tx confirmations Event Recieved */
    uint32_t pktTxConfEventRecvd;

    /* Number of slice start markers encountered */
    uint32_t sliceStartCount;

    /* Number of errors noted when creating the RTP header */
    uint32_t pktTxHdrAddnErrors;

    /* Number of times ethernet output buffers were unavailable */
    uint32_t numEthL2BufUnderflow;

    /* Number of MSYS_FillBuffer API failure */
    uint32_t fillBufFailCount;

    /* Number of RTCP packets transmitted successfully */
    uint32_t pktRTCPTxCount;

} RTP_Sender_Stats;

/* Structure used to maintain circQ entires of each input port */
typedef struct _RTPS_BufInfo
{
    /* Index of memory pool */
    uint32_t bufIdx;

    /* Buffer/Data length */
    uint32_t actualLen;

    /* Size of processed data len */
    uint32_t procLen;

    /** Timestamp to be filled/passed */
    uint64_t timestamp;

    /** Start of slice flag */
    uint32_t syncStart;

    /** End of slice flag */
    uint32_t syncEnd;

    /* marker to notify frame start */
    uint32_t frameStart;

    /* marker to notify frame end */
    uint32_t frameEnd;

    /** Stream info */
    uint32_t streamInfo;

    /* State machine to track is buffer processing status */
    uint32_t dataProcState;
/* State when no data is under process */
#define RTP_NODATA_IN_PROCESS           0UL
/* State when RTP data has been picked for processing */
#define RTP_DATA_BUFFER_IN_USE          1UL
/* State when TS Table packets could not be sent due to L2 buffer underflow */
#define RTP_MPEGTS_TABLES_UNDER_PROCESS 2UL
/* State when RTCP payload could not be sent due to L2 buffer underflow */
#define RTP_1733_UNDER_PROCESS          3UL
/* State when RTP payload could not be sent due to L2 buffer underflow */
#define RTP_MEDIADATA_UNDER_PROCESS     4UL
} RTPS_BufInfo;

/* Internal structure used to manage the RTP component */
typedef struct {
    /* State of the RTPS component */
    uint32_t compState;
#define RTPS_COMP_STATE_RESET  0UL
#define RTPS_COMP_STATE_INIT   1UL
#define RTPS_COMP_STATE_READY  2UL
#define RTPS_COMP_STATE_START  3UL

    /* Flag to indicate if STOP command is in progress */
    uint32_t stopCmdInProgress;

    /* Flag to indicate if STOP command is in progress */
    uint32_t startCmdInProgress;

    /* Count to track number of config images received */
    uint32_t configCount;

    /* Stream configuration for RTP header addition */
    RTP_SenderPayloadInfo bufparam[RTP_MAX_NUM_INPUT_PORTS];

    /* Stream configuration for RTP+TS header addition */
    MPEGTS_PayloadInfoType tsLibParam[RTP_MAX_NUM_INPUT_PORTS];

    /* Number of input ports activated */
    uint32_t numInpPort;

    /* The current data from circularQ that is being processed */
    /* It is used only when ethernet Tx buffersare unavailable */
    /* for processing.                                         */
    RTPS_BufInfo circQData[RTP_MAX_NUM_INPUT_PORTS];

    /* The data structure used to maintain stats */
    RTP_Sender_Stats sndrStats[RTP_MAX_NUM_INPUT_PORTS];

} RTPS_StackHandle;

/**********************************************************************
                    Internal Functions
**********************************************************************/

/* Function to packetize and send the data packets */
void RTPS_DataProcess (uint32_t portIndex);

/* Function to Flush the Tx buffers to Ethernet */
void RTPS_FlushTxBuffers (uint32_t portIndex);

/* Function to Send the Tx buffers to peer component */
void RTPS_HandleBufQueuing (uint32_t portIndex, uint32_t bufIndex);

/* Function to process the data present in circular Q */
void RTPS_CircQDataProcess (uint32_t portIndex);

/* Function to be used for logging */
void RTPS_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                      uint32_t aVal1, uint32_t aVal2, uint32_t aVal3);

/* Function to process incoming data via MSYS_FillBufferDone call */
void RTPS_FillBufferDoneProcess (MSYS_BufferType *const aMediaBuffer);

/* Local function for Error State Check */
int32_t RTPS_CheckProcessingState(void);

/* Function to get message from command Q list */
void* RTPS_GetMessage_CommandQList (uint32_t idx);

/* Latency logging function */
void RTPS_LatencyStatistics(uint64_t timestamp, uint32_t size);

#endif /* RTP_SENDER_INTERNAL_H */
