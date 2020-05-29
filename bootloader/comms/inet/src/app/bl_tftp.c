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

#include <string.h>
#include <bl_tftp.h>
#include <bl_utils.h>
#include <bl_log.h>
#include <bl_bcm_err.h>
#include <bcm_time.h>
#include <bl_udp.h>

#define BRCM_DSGN_BL_TFTP_REPORT_ERROR_PROC           (0xF0U) /**< @brief #BL_TFTP_ReportError */

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

static uint32_t BL_IsFirstPkt = BL_FALSE;
static BL_TFTP_CompType BL_TFTP_Comp = {
    .reTransmit = BL_FALSE,
};

BCM_TimeType BL_TftpT1, BL_TftpT2;

#define BL_UDP_PKT_SIZE        (1536UL)

static uint8_t BL_TxPktBuf[BL_UDP_PKT_SIZE];

static BL_COMP_NEVER_INLINE void BL_TFTP_ReportError(int32_t aErr, uint8_t aInstanceID,
                                                            uint32_t aInfo0,
                                                            uint32_t aInfo1,
                                                            uint32_t aInfo2,
                                                            uint32_t aLineNo)
{
    const uint32_t values[4] = {aInfo0, aInfo1, aInfo2, aLineNo};
    BCM_ReportError(BCM_NET_ID, aInstanceID, BRCM_DSGN_BL_TFTP_REPORT_ERROR_PROC, aErr, 4UL, values);
}

/******************************************************************************
FUNCTION NAME: SendAck

ARGUMENTS:
connectionState -> a pointer to connection's tftpConnectionState
blockNo -> the block number to which acknowledgment has to be sent

DESCRIPTION: This function is used to send an ACK packet

RETURNS: err
******************************************************************************/
static int32_t BL_TFTP_SendAck(BL_TFTP_CompType *const aComp,
                               uint16_t aBlockNo)
{
    int32_t retVal = BL_BCM_ERR_OK;
    BL_TFTP_OpcodeType *ackOpNo;
    uint16_t *ackBlockNo;
    uint32_t len = BL_TFTP_ACK_PKT_SIZE;

    BL_BCM_MemSet(BL_TxPktBuf, 0x0L, BL_TFTP_ACK_PKT_SIZE);

    /* Allocate the pbuf */
    ackOpNo = (uint16_t *)(BL_TxPktBuf);
    ackBlockNo = (uint16_t *)(&BL_TxPktBuf[BL_TFTP_BLOCK_NUM_OFFSET]);
    *ackOpNo = BL_BE2Host16(BL_TFTP_OP_ACK);
    *ackBlockNo = BL_BE2Host16(aBlockNo);
    retVal = BL_UDP_Send(BL_TFTP_PAYLOAD_RCV_FILE, BL_TxPktBuf, len);
    if (BL_BCM_ERR_OK != retVal) {
        goto err;
    }

err:
    return retVal;
}

static void BL_TFTP_SendError(uint32_t aPayloadId,
                              BL_TFTP_ErrType aErrorNo,
                              const char *const aErrMsg,
                              BL_INET_IPAddress aDestIp,
                              BL_INET_Port aDestPort)
{
    BL_TFTP_OpcodeType *errOpCode;
    BL_TFTP_ErrType *errCode;
    char *pktMsg;
    uint32_t retVal;
    uint32_t len = (BL_TFTP_ERR_PKT_SIZE + strnlen(aErrMsg, BL_TFTP_ERR_MSG_MAX_LEN));
    uint32_t lineNo = __LINE__;

    /* Check if it's a listening connection.
     * If so temporarily connect it to the destination passed in the arguments
     */
    errOpCode = (uint16_t *)BL_TxPktBuf;
    errCode = (uint16_t *)(&BL_TxPktBuf[2UL]);
    pktMsg = (char *)(&BL_TxPktBuf[4UL]);
    /* Fill the packet */
    *errOpCode = BL_BE2Host16(BL_TFTP_OP_ERROR);
    *errCode = BL_BE2Host16(aErrorNo);
    (void)strncpy(pktMsg, aErrMsg, BL_TFTP_ERR_MSG_MAX_LEN);

    retVal = BL_UDP_Send(aPayloadId, BL_TxPktBuf, len);
    if (BL_BCM_ERR_OK != retVal) {
        goto err;
    }

err:
    if (BL_BCM_ERR_OK != retVal) {
        lineNo = __LINE__;
        BL_TFTP_ReportError(retVal, 0UL, 0UL, 0UL, 0UL, lineNo);
    }
    return;
}

void BL_TFTP_RecvFileCb(const uint8_t* const aBuf,
                        uint16_t aLen,
                        BL_INET_IPAddress aSrcIPAddr,
                        BL_INET_Port aSrcPort)
{
    BL_TFTP_OpcodeType opCode;
    BL_TFTP_OpcodeType * opCodePtr;
    uint16_t blockNo;
    uint16_t * blockNoPtr;
    int32_t retVal;
    BL_TFTP_CompType *comp = &BL_TFTP_Comp;
    uint32_t lineNo = __LINE__;

    opCodePtr = (BL_TFTP_OpcodeType *)(aBuf);
    opCode = *(opCodePtr);
    opCode = BL_BE2Host16(opCode);
    blockNoPtr = ((uint16_t *)(&aBuf[BL_TFTP_BLOCK_NUM_OFFSET]));
    blockNo = *(blockNoPtr);
    blockNo = BL_BE2Host16(blockNo);
    /* Check the opcode */
    if (opCode != BL_TFTP_OP_DATA) {
        BL_LOG_ERR("It's not a data packet!\n");
        goto err;
    }

    /* Check the block Number */
    if (blockNo != (comp->blockNo + 1U)) {
        lineNo = __LINE__;
        BL_TFTP_ReportError(BL_BCM_ERR_INVAL_PARAMS, 0UL,
                ((blockNo << 16) | (comp->blockNo + 1U)), 0UL, 0UL, lineNo);
        goto err;
    }
    /* Adjust the pbuf so that the user (callback Function) does not see
     * the TFTP header
     */
    if (aLen <= BL_TFTP_DATA_HDR_SIZE) {
        lineNo = __LINE__;
        BL_TFTP_ReportError(BL_BCM_ERR_INVAL_PARAMS, 0UL, aLen, 0UL, 0UL, lineNo);
    }

    retVal = BL_TFTP_RxDataInd(&aBuf[BL_TFTP_DATA_HDR_SIZE],
                            (aLen - BL_TFTP_DATA_HDR_SIZE));
    if (BL_BCM_ERR_OK != retVal) {
        /* Send error packet */
        BL_TFTP_SendError(BL_TFTP_PAYLOAD_RCV_FILE,
                BL_TFTP_ERR_DISK_FULL_ALLOC_EXCEED,
                "Out of Memory", aSrcIPAddr, aSrcPort);
        lineNo = __LINE__;
        BL_TFTP_ReportError(BL_BCM_ERR_NOMEM, 0UL, 0UL, 0UL, 0UL, lineNo);
        comp->state = BL_TFTP_STATE_NONE;
        comp->timeoutCnt = 0UL;
        goto err;
    }

    /* Send an ack for the data */
    retVal = BL_TFTP_SendAck(comp, (comp->blockNo + 1UL));
    if (BL_BCM_ERR_OK != retVal) {
        lineNo = __LINE__;
        BL_TFTP_ReportError(retVal, 0UL, (comp->blockNo + 1UL), 0UL, 0UL, lineNo);
        BL_TFTP_Comp.reTransmit = BL_TRUE;
    } else {
        BL_TFTP_Comp.reTransmit = BL_FALSE;
        comp->blockNo++;
    }
    /* Check if this is the last Packet */
    if (aLen < (BL_TFTP_PKT_SIZE + BL_TFTP_DATA_HDR_SIZE)) {
        BL_LOG_INFO("Got LAST packet!\n");
        BL_TFTP_EndInd(BL_FALSE);
        comp->state = BL_TFTP_STATE_NONE;
        comp->blockNo = 0U;
    }

    BL_TFTP_Comp.enableTimeOut = BL_TRUE;
    BCM_GetSystemTime(&BL_TftpT2);
    comp->timeoutCnt = 0UL;

err:
    return;
}

static int32_t BL_TFTP_RecvFile(BL_INET_IPAddress aRmtIP,
                                BL_INET_Port aRmtPort)
{
    int32_t err = BL_BCM_ERR_OK;
    uint32_t lineNo = __LINE__;

    BL_TFTP_Comp.blockNo = 0;
    BL_TFTP_Comp.state = BL_TFTP_STATE_WRQ;
    BL_TFTP_Comp.timeoutCnt = 0UL;
    BL_TFTP_Comp.rmtIP = aRmtIP;
    BL_TFTP_Comp.rmtPort = aRmtPort;

    err = BL_TFTP_StartInd(aRmtIP);
    if (BL_BCM_ERR_OK != err) {
        BL_TFTP_SendError(BL_TFTP_PAYLOAD_RCV_FILE, BL_TFTP_ERR_NOT_DEFINED,
                "Flashing in progress", aRmtIP, aRmtPort);
        err = BL_BCM_ERR_BUSY;
        goto err;
    }

    /* Send the first Ack */
    err = BL_TFTP_SendAck(&BL_TFTP_Comp, BL_TFTP_Comp.blockNo);
    if (BL_BCM_ERR_OK != err) {
        lineNo = __LINE__;
        BL_TFTP_ReportError(err, 0UL, BL_TFTP_Comp.blockNo, 0UL, 0UL, lineNo);
        BL_TFTP_Comp.reTransmit = BL_TRUE;
    } else {
        BL_TFTP_Comp.reTransmit = BL_FALSE;
        BL_IsFirstPkt = BL_TRUE;
    }
err:
    return err;
}

static void BL_TFTP_CloseConn(void)
{
    BL_TFTP_Comp.state = BL_TFTP_STATE_NONE;
    BL_TFTP_Comp.blockNo = 0U;
    BL_TFTP_Comp.timeoutCnt = 0UL;
    /* inform client that connection is closed */
    BL_TFTP_EndInd(BL_TRUE);
}

static void BL_TFTP_IntStateHandler(void)
{
    int32_t retVal;
    uint32_t lineNo = __LINE__;

    if (BL_TRUE == BL_TFTP_Comp.reTransmit) {
        if (BL_FALSE == BL_IsFirstPkt) {
            retVal = BL_TFTP_SendAck(&BL_TFTP_Comp, (BL_TFTP_Comp.blockNo + 1UL));
            if (BL_BCM_ERR_OK != retVal) {
                BL_TFTP_Comp.reTransmit = BL_TRUE;
                lineNo = __LINE__;
                BL_TFTP_ReportError(retVal, 0UL, (BL_TFTP_Comp.blockNo + 1UL), 0UL, 0UL, lineNo);
            } else {
                BL_TFTP_Comp.reTransmit = BL_FALSE;
                (BL_TFTP_Comp.blockNo)++;
            }
        } else {
            retVal = BL_TFTP_SendAck(&BL_TFTP_Comp, BL_TFTP_Comp.blockNo);
            if (BL_BCM_ERR_OK != retVal) {
                BL_TFTP_Comp.reTransmit = BL_TRUE;
            } else {
                BL_TFTP_Comp.reTransmit = BL_FALSE;
                BL_IsFirstPkt = BL_FALSE;
            }
        }
    }
}

static void BL_TFTP_TimerCb(void)
{
    int32_t retVal;
    uint32_t lineNo = __LINE__;

    BL_TFTP_Comp.enableTimeOut = BL_FALSE;
    switch (BL_TFTP_Comp.state) {
    case BL_TFTP_STATE_NONE:
        BL_LOG_VERB("No active transmission\n");
        break;
    case BL_TFTP_STATE_WRQ:
        BL_LOG_INFO("Timeout.. Resending the Ack!\n");
        retVal = BL_TFTP_SendAck(&BL_TFTP_Comp, (BL_TFTP_Comp.blockNo));
        if (BL_BCM_ERR_OK != retVal) {
            BL_TFTP_Comp.reTransmit = BL_TRUE;
            lineNo = __LINE__;
            BL_TFTP_ReportError(retVal, 0UL, BL_TFTP_Comp.blockNo, 0UL, 0UL, lineNo);
        } else {
            BL_TFTP_Comp.reTransmit = BL_FALSE;
        }
        BL_TFTP_Comp.timeoutCnt++;
        BL_TFTP_Comp.enableTimeOut = BL_TRUE;
        BCM_GetSystemTime(&BL_TftpT2);
        break;
    case BL_TFTP_STATE_RRQ:
    case BL_TFTP_STATE_DALLY:
        break;
    default:
        lineNo = __LINE__;
        BL_TFTP_ReportError(BL_BCM_ERR_INVAL_STATE, 0UL, BL_TFTP_Comp.state, 0UL, 0UL, lineNo);
        BL_TFTP_CloseConn();
        break;
    }
    /* Check if enough timeout have occured to justify killing the connection */
    if (BL_TFTP_Comp.timeoutCnt > BL_TFTP_MAX_TIMEOUTS) {
        /* The connection has timeout many times.  Close the connection */
        BL_LOG_INFO("connection timeout..\n");
        BL_TFTP_CloseConn();
    }
}

void BL_TFTP_RecvRequestCb(const uint8_t* const aBuf,
                           uint16_t aLen,
                           BL_INET_IPAddress aSrcIPAddr,
                           BL_INET_Port aSrcPort)
{
    uint16_t tftpOp;
    uint16_t * tftpOpPtr;
    int32_t retVal;
    uint32_t lineNo = __LINE__;

    /* Check the state of TFTP server */
    if (BL_TFTP_STATE_NONE == BL_TFTP_Comp.state) {
        if (aLen < 2UL) {
            lineNo = __LINE__;
            BL_TFTP_ReportError(BL_BCM_ERR_INVAL_PARAMS, 0UL, (uint32_t)aLen, 0UL, 0UL, lineNo);
        } else {
            /* Parse the packet */
            tftpOpPtr = ((uint16_t *) aBuf);
            tftpOp = *(tftpOpPtr);
            tftpOp = BL_BE2Host16(tftpOp);

            if (tftpOp == BL_TFTP_OP_WRQ) {
                BL_LOG_INFO("Received Write Request!\n");
                retVal = BL_TFTP_RecvFile(aSrcIPAddr, aSrcPort);
                if (BL_BCM_ERR_OK != retVal) {
                    lineNo = __LINE__;
                    BL_TFTP_ReportError(retVal, 0UL, 0UL, 0UL, 0UL, lineNo);
                }
            } else {
                lineNo = __LINE__;
                BL_TFTP_ReportError(BL_BCM_ERR_NOSUPPORT, 0UL, (uint32_t)tftpOp, 0UL, 0UL, lineNo);
            }
        }
    } else {
        lineNo = __LINE__;
        BL_TFTP_ReportError(BL_BCM_ERR_INVAL_STATE, 0UL, (uint32_t)BL_TFTP_Comp.state, 0UL, 0UL, lineNo);
        BL_TFTP_SendError(BL_TFTP_PAYLOAD_RCV_REQ, BL_TFTP_ERR_ACCESS_VIOLATION,
                "Access Violation", aSrcIPAddr, aSrcPort);
    }
}

/**
    @image html bl_tftp_flowchart_init.jpg
*/
void BL_TFTP_Init(void)
{
    /* Close/Reset connection */
    BL_TFTP_CloseConn();
}

void BL_TFTP_StateHandler(void)
{
    BCM_TimeType tDiff;

    BL_TFTP_IntStateHandler();

    /* Process the timer */
    if (BL_TRUE == BL_TFTP_Comp.enableTimeOut) {
        BCM_GetSystemTime(&BL_TftpT1);
        tDiff = BCM_GetTimeAbsDiff(BL_TftpT1, BL_TftpT2);
        if (BL_TFTP_TIMER_INTERVAL <=
                ((tDiff.s * 1000UL) + (tDiff.ns / 1000000UL))) {
            BL_TFTP_TimerCb();
        }
    }
}
