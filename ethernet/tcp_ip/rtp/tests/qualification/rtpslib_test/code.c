/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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

/* Includes */
#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <ulog.h>
#include <bcm_err.h>
#include <console.h>
#include <bcm_test.h>
#include <bcm_time.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>
#include "rtp_rtcp.h"

#define GetModuleLogLevel() ULOG_LVL_VERBOSE

uint8_t outTestBuf[64*1024];
uint8_t inTestBuf[5000];

uint8_t testRTPPacket[64] = {
             0x80, 0x60, 0x00, 0x10, 0x00, 0x00, 0x0B, 0x9A,
             0x00, 0x00, 0x01, 0x02, 0x7c, 0x85, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
             0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,};

uint8_t testRTCPPacket[84] = {
             /* SR */
             0x80, 0xC8, 0x00, 0x06, 0x00, 0x00, 0x01, 0x02,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
             0x00, 0x00, 0x01, 0x00,
             /* SDES */
             0x81, 0xCA, 0x00, 0x03, 0x00, 0x00, 0x01, 0x02,
             0x01, 0x06, 0x52, 0x54, 0x50, 0x53, 0x54, 0x52,
             /* 1733 */
             0x80, 0xD0, 0x00, 0x09, 0x00, 0x00, 0x01, 0x02,
             0x52, 0x54, 0x50, 0x53, 0x0A, 0x0B, 0xAA, 0xAA,
             0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
             0x00, 0x01, 0x11, 0x00, 0x00, 0x00, 0x11, 0x11,};

static volatile int32_t rtps_lib_qt1_result = BCM_AT_NOT_STARTED;
static volatile int32_t rtps_lib_qt2_result = BCM_AT_NOT_STARTED;
static volatile int32_t rtps_lib_qt3_result = BCM_AT_NOT_STARTED;

#define RTPS_CHECK_ERR_IT1(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); rtps_lib_qt1_result = ret;}
#define RTPS_CHECK_ERR_IT2(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); rtps_lib_qt2_result = ret;}
#define RTPS_CHECK_ERR_IT3(ret,file,line)  if (BCM_ERR_OK != ret){ ULOG_ERR("\n Error: %d \tfile: %s \t line %d",ret,file,line); rtps_lib_qt3_result = ret;}

static int32_t RTPS_ValidateComposedRTPPacket(
                                    uint8_t *buf,
                                    RTP_SenderPayloadInfo *params);

static int32_t RTPS_ValidateComposedRTCPPacket(
                                    uint8_t *buf,
                                    RTP_SenderPayloadInfo *params);
/**
  @defgroup grp_rtp_qt Qualification Tests
  @ingroup grp_rtplib

  @addtogroup grp_rtp_qt
  @{

  @file code.c
  @brief RTP Library Integration Test
  This source file contains the qualification tests for RTP library
  @version 0.51 imported from MS word doc
*/

/**
 * @brief Brief description of COMP_IT1
 *
 * This test verifies the media payload packetization API of RTP library
 *
 * @code{.unparsed}
 *
 * 1. Validate the slice based fragmentation mode (FU-A) packetization
 * 2. Configure the payload for H264 using the
 *    structure – ‘RTP_SenderPayloadInfo’
 * 3. We will need to verify RTP packetization for SPS, PPS and NAL
 *    data payload packets
 * 4. Invoke the RTP_Packetize API with valid input and output buffers
 * 5. Upon return, the test client validates the output packet buffer
 *    for valid values
 * 6. In case of any error, an error is printed onto console
 * 7. If return code is BCM_ERR_EAGAIN, validate the FU-A RTP packet
 *    created in output buffer and repeat step #2 with a new a output buffer
 * 8. In case of any error, an error is printed onto console
 *
 * @endcode
*/
static void RTPS_LIB_IT1(void);

/**
 * @brief Brief description of COMP_IT2
 *
 * This test verifies the verifies the RTP packet parsing API of RTP library
 *
 * @code{.unparsed}
 *
 * 1. Validate the RTP packet parsing capability of RTP library
 * 2. Prepare a dummy RTP packet with media payload which shall be used
 *    for testing the payload parsing interface
 * 3. Invoke the RTP_PacketParse API with valid arguments
 * 4. Upon return, the test client validates the payload parameters filled
 *    by the library
 * 5. Prepare a dummy compound RTCP packet with 1733 SR packet which shall
 *    be used for testing the packet parsing interface
 * 6. Invoke the RTP_PacketParse API with valid arguments
 * 7. Upon return, the test client validates the packet parameters filled
 *    by the library
 * 8. In case of any error, an error is printed onto console
 *
 * @endcode
*/
static void RTPS_LIB_IT2(void);

/**
 * @brief Brief description of COMP_IT3
 *
 * This test verifies the RTCP compound Sender report creation API of RTP library
 *
 * @code{.unparsed}
 *
 * 1. Validate the RTCP sender report creation API of RTP library
 * 2. Configure the parameters for RTCP SR using the
 *    structure – ‘RTP_SenderPayloadInfo’
 * 3. Invoke the RTCP_Create1733SenderReport API with valid output buffer
 * 4. Upon return, the test client validates the output packet buffer for
 *    valid RTCP packet
 * 5. In case of any error, an error is printed onto console
 *
 * @endcode
*/
static void RTPS_LIB_IT3(void);

/** @} */


int32_t RTPS_ValidateComposedRTPPacket(uint8_t *buf,
                                       RTP_SenderPayloadInfo *params)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    /* Check RTP common header */
    if ((0x80U != *buf) ||
        (0x60U != *(buf+1)) ||
        (0x102UL != Host2BE32(*((uint32_t*)(buf+8))) )) {
        goto error;
    }

    /* Verify the RTP payload header */
    if ((0UL == params->strInfo.isMetaData) &&
        (0x7c != *(buf+12))) {
        goto error;
    }

    retVal = BCM_ERR_OK;

error:
    return retVal;
}

int32_t RTPS_ValidateComposedRTCPPacket(uint8_t *buf,
                                        RTP_SenderPayloadInfo *params)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    int32_t lineVal = 0UL;

    /* Check RTCP initial header */
    if ((0x80U != *buf) ||
        (0xC8U != *(buf+1)) ||
        (0x102UL != Host2BE32(*((uint32_t*)(buf+4))) )) {
        lineVal = __LINE__;
        goto error;
    }

    /* Verify the RTP SR Packet count */
    if (0x10UL != Host2BE32(*((uint32_t*)(buf+20))) ) {
        lineVal = __LINE__;
        goto error;
    }

    /* Verify the RTP SR payload bytes */
    if (1000UL != Host2BE32(*((uint32_t*)(buf+24))) ) {
        lineVal = __LINE__;
        goto error;
    }

    /* Check RTCP header for SDES packet */
    if ((0x81U != *(buf+28)) ||
        (0xCAU != *(buf+29)) ||
        (0x102UL != Host2BE32(*((uint32_t*)(buf+32))) )) {
        lineVal = __LINE__;
        goto error;
    }

    /* Check RTCP header for 1733 packet */
    if ((0x80U != *(buf+44)) ||
        (0xD0U != *(buf+45)) ||
        (0x102UL != Host2BE32(*((uint32_t*)(buf+48))) )) {
        lineVal = __LINE__;
        goto error;
    }

    /* Check RTCP header for 1733 name */
    if (0UL != memcmp(&params->strParams.rtcpCfg.name1733Str,
                      buf+52, 4UL)) {
        lineVal = __LINE__;
        goto error;
    }

    /* Check for valid network and rtp time stamp */
    if ((0U == *((uint32_t*)(buf+76))) ||
        (0U == *((uint32_t*)(buf+80)))) {
        lineVal = __LINE__;
        goto error;
    }

    retVal = BCM_ERR_OK;

error:
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("Error Line Num %u \n",lineVal);
    }

    return retVal;
}

void RTPS_LIB_IT1(void)
{
    int32_t retVal = BCM_ERR_OK;
    RTP_SenderPayloadInfo params;
    RTP_DataBufferType inpBuf, outBuf;

    memset(&params, 0U, sizeof(params));

    /* FU-A : Slice Fragment : SPS NAL Type */
    params.strParams.strInfo.mediaType = VID_H264_MEDIA_TYPE;
    params.strParams.strInfo.payloadFmt = RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE;
    params.strParams.strInfo.SSRCValue = 0x102;
    params.strInfo.isSliceStart = 1UL;
    params.strInfo.isSliceEnd = 1UL;
    params.strInfo.isMetaData = 1UL;
    params.strInfo.streamInfo = 0x67UL; /* SPS */

    inpBuf.buffer = inTestBuf;
    inpBuf.offset = 0UL;
    inpBuf.length = sizeof(inTestBuf);

    outBuf.buffer = outTestBuf;
    outBuf.offset = 0UL;
    outBuf.length = sizeof(outTestBuf);

    retVal = RTP_Packetize(&inpBuf, &outBuf, &params);
    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);

    retVal = RTPS_ValidateComposedRTPPacket(outTestBuf, &params);
    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);

    /* FU-A : Slice Fragment : PPS NAL Type */
    params.strParams.strInfo.mediaType = VID_H264_MEDIA_TYPE;
    params.strParams.strInfo.payloadFmt = RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE;
    params.strParams.strInfo.SSRCValue = 0x102;
    params.strInfo.isSliceStart = 1UL;
    params.strInfo.isSliceEnd = 1UL;
    params.strInfo.isMetaData = 1UL;
    params.strInfo.streamInfo = 0x68UL; /* PPS */

    inpBuf.buffer = inTestBuf;
    inpBuf.offset = 0UL;
    inpBuf.length = sizeof(inTestBuf);

    outBuf.buffer = outTestBuf;
    outBuf.offset = 0UL;
    outBuf.length = sizeof(outTestBuf);

    retVal = RTP_Packetize(&inpBuf, &outBuf, &params);
    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);

    retVal = RTPS_ValidateComposedRTPPacket(outTestBuf, &params);
    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);

    /* FU-A : Slice Fragment : IDR NAL Type */
    params.strParams.strInfo.mediaType = VID_H264_MEDIA_TYPE;
    params.strParams.strInfo.payloadFmt = RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE;
    params.strParams.strInfo.SSRCValue = 0x102;
    params.strInfo.isSliceStart = 1UL;
    params.strInfo.isSliceEnd = 0UL;
    params.strInfo.isMetaData = 0UL;
    params.strInfo.streamInfo = 0x65UL; /* IDR */

    inpBuf.buffer = inTestBuf;
    inpBuf.offset = 0UL;
    inpBuf.length = sizeof(inTestBuf);

    outBuf.buffer = outTestBuf;
    outBuf.offset = 0UL;
    outBuf.length = sizeof(outTestBuf);

    do {
        retVal = RTP_Packetize(&inpBuf, &outBuf, &params);
        if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            break;
        }
    } while (BCM_ERR_EAGAIN == retVal);

    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);
    retVal = RTPS_ValidateComposedRTPPacket(outTestBuf, &params);
    RTPS_CHECK_ERR_IT1(retVal,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == rtps_lib_qt1_result) {
        rtps_lib_qt1_result = BCM_ERR_OK;
        ULOG_ERR("RTPS_LIB_IT1: PASS\n");
    } else {
        ULOG_ERR("RTPS_LIB_IT1: FAIL\n");
    }

    return;
}

void RTPS_LIB_IT2(void)
{
    int32_t retVal = BCM_ERR_OK;
    RTP_PayloadInfo params;

    memset(&params, 0U, sizeof(params));

    /* RTP Packet Parsing test */
    params.strParams.strInfo.mediaType = VID_H264_MEDIA_TYPE;
    params.strParams.strInfo.payloadFmt = RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE;
    params.strParams.strInfo.SSRCValue = 0x102;

    retVal = RTP_PacketParse(testRTPPacket,
                             sizeof(testRTPPacket),
                             &params);
    RTPS_CHECK_ERR_IT2(retVal,__FILE__, __LINE__);

    /* Check if RTP packet values are parsed and fetched properly */
    if ((params.info.SSRCid != params.strParams.strInfo.SSRCValue) ||
        (PACKET_RTP_MEDIA_PAYLOAD != params.info.packetType) ||
        (1UL != params.info.mediaPkt.isSliceAggnStart) ||
        (0UL != params.info.mediaPkt.isSliceAggnEnd) ||
        (5UL != params.info.mediaPkt.NALType) ||
        (0x10UL != params.info.mediaPkt.packetSeqNum) ||
        (50UL != params.info.mediaPkt.payloadLen) ||
        (14UL != params.info.mediaPkt.payloadOffset) ||
        (0UL == params.info.mediaPkt.rtpTS)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    RTPS_CHECK_ERR_IT2(retVal,__FILE__, __LINE__);

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
    retVal = RTP_PacketParse(testRTCPPacket,
                             sizeof(testRTCPPacket),
                             &params);
    RTPS_CHECK_ERR_IT2(retVal,__FILE__, __LINE__);

    /* Check if RTP packet values are parsed and fetched properly */
    if ((params.info.SSRCid != params.strParams.strInfo.SSRCValue) ||
        (PACKET_RTCP_1733_SR_PAYLOAD != params.info.packetType) ||
        (0UL == params.info.sr1733Pkt.gmIdentityValue) ||
        (0xA0BUL != params.info.sr1733Pkt.gmTimeBaseInd) ||
        (0UL == params.info.sr1733Pkt.networkRefTime) ||
        (0x100UL != params.info.sr1733Pkt.rtpPayloadBytesSent) ||
        (0x10UL != params.info.sr1733Pkt.rtpPktsSent) ||
        (0UL == params.info.sr1733Pkt.rtpRefTime) ||
        (0x102UL != params.info.sr1733Pkt.streamID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    RTPS_CHECK_ERR_IT2(retVal,__FILE__, __LINE__);
#endif
    if (BCM_AT_EXECUTING == rtps_lib_qt2_result) {
        rtps_lib_qt2_result = BCM_ERR_OK;
        ULOG_ERR("RTPS_LIB_IT2: PASS\n");
    } else {
        ULOG_ERR("RTPS_LIB_IT2: FAIL\n");
    }

    return;
}

#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
void RTPS_LIB_IT3(void)
{
    int32_t retVal = BCM_ERR_OK;
    RTP_SenderPayloadInfo params;
    RTP_DataBufferType outBuf;
    uint8_t tmpMem[128];
    BCM_TimeType curTime = {0UL, 0UL};

    BCM_GetTime(&curTime);

    memset(&params, 0U, sizeof(params));
    outBuf.buffer = tmpMem;

    /* Validate the RTCP Sender Report creation API */
    params.strParams.strInfo.mediaType = VID_H264_MEDIA_TYPE;
    params.strParams.strInfo.payloadFmt = RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE;
    params.strParams.strInfo.SSRCValue = 0x102;
    params.strInfo.timeStamp = (((uint64_t)(curTime.s)) * BCM_NS_PER_SEC) + curTime.ns - 1000;
    params.strInfo.rtpPktCount = 0x10;
    params.strInfo.payloadBytesSent = 1000UL;
    params.libParam.packetSeqNum = 50;
    BCM_MemCpy(&params.strParams.rtcpCfg.cnameStr, "RTPS\0", 6UL);
    params.strParams.rtcpCfg.isRTCPEnabled = 1UL;
    BCM_MemCpy(&params.strParams.rtcpCfg.name1733Str, "1733", 4UL);
    params.strParams.rtcpCfg.streamID = 0x102ULL;

    retVal = RTCP_Create1733SenderReport(&outBuf, &params);

    RTPS_CHECK_ERR_IT3(retVal,__FILE__, __LINE__);

    /* Check for valid RTCP packet length */
    if (84UL > outBuf.length) {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    RTPS_CHECK_ERR_IT3(retVal,__FILE__, __LINE__);

    /* Check for RCTP packet values */
    retVal = RTPS_ValidateComposedRTCPPacket(outBuf.buffer,
                                             &params);

    RTPS_CHECK_ERR_IT3(retVal,__FILE__, __LINE__);

    if (BCM_AT_EXECUTING == rtps_lib_qt3_result) {
        rtps_lib_qt3_result = BCM_ERR_OK;
        ULOG_ERR("RTPS_LIB_IT3: PASS\n");
    } else {
        ULOG_ERR("RTPS_LIB_IT3: FAIL\n");
    }

    return;
}
#endif

TASK(RTPSLibTest)
{
    RTPS_LIB_IT1();
    RTPS_LIB_IT2 ();
#ifdef RTP_ENABLE_RTCP_1733_SUPPORT
    RTPS_LIB_IT3 ();
#endif
    BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        rtps_lib_qt1_result = BCM_AT_EXECUTING;
        rtps_lib_qt2_result = BCM_AT_EXECUTING;
        rtps_lib_qt3_result = BCM_AT_EXECUTING;
        ULOG_INFO("\n Test started");
        BCM_ActivateTask(RTPSLibTest);
        ret = BCM_ERR_OK;
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = rtps_lib_qt1_result;
    }
    if (2UL == aIndex) {
        ret = rtps_lib_qt2_result;
    }
    if (3UL == aIndex) {
        ret = rtps_lib_qt3_result;
    }
    return ret;
}

void ApplicationInit()
{
    return;
}

