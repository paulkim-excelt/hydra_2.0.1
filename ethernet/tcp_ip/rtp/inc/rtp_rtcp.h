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

/**

  @addtogroup grp_rtplib
  @{

  @file rtp_rtcp.h
  @brief Application interface for RTP-RTCP library
  This file provides the interface/APIs which can be used to packetize,
  parse and send the control information from the RTP/RTCP libary.
  It also contains APIs to query for parameters and statistics.
  @version 0.51 Imported from the MS word document
*/

#ifndef RTP_PAYLOAD_H
#define RTP_PAYLOAD_H

/* Includes */
#include <inet.h>
#include "rtp_config.h"

/**
    @name RTP Library API IDs
    @{
    @brief API IDs for RTP Library
 */
#define BRCM_ARCH_RTP_PTS_TIMESTAMP_OFFSET_MACRO       (1UL) /**< @brief #RTP_PTS_TIMESTAMP_OFFSET_VAL_IN_MS */
#define BRCM_ARCH_RTP_MAX_NUM_PORTS_MACRO              (2UL) /**< @brief #RTP_MAX_NUM_PORTS */
#define BRCM_ARCH_RTP_PAYLOAD_FRAGMENT_LEN_MACRO       (3UL) /**< @brief #RTP_PAYLOAD_FRAGMENT_LEN */
#define BRCM_ARCH_RTCP_CNAME_STRING_LEN_MACRO          (4UL) /**< @brief #RTCP_CNAME_STRING_LEN */
#define BRCM_ARCH_RTCP_1733_NAME_STRING_LEN_MACRO      (5UL) /**< @brief #RTCP_1733_NAME_STRING_LEN */
#define BRCM_ARCH_RTCP_GM_IDENTITY_VALUE_LEN_MACRO     (6UL) /**< @brief #RTCP_GM_IDENTITY_VALUE_LEN */
#define BRCM_ARCH_RTPPACKET_TYPE                       (7UL) /**< @brief #RTPPacketType */
#define BRCM_ARCH_RTP_H264_STREAM_INFO_TYPE            (8UL) /**< @brief #RTP_H264StreamInfo */
#define BRCM_ARCH_RTP_SENDERLIB_PARAM_TYPE             (9UL) /**< @brief #RTP_SenderLibParam */
#define BRCM_ARCH_RTP_SENDER_PAYLOAD_INFO_TYPE        (10UL) /**< @brief #RTP_SenderPayloadInfo */
#define BRCM_ARCH_RTP_MEDIA_PACKET_TYPE               (11UL) /**< @brief #RTP_MediaPacket */
#define BRCM_ARCH_RTCP_1733SR_PACKET_TYPE             (12UL) /**< @brief #RTCP_1733SRPacket */
#define BRCM_ARCH_RTP_PARSED_PKT_INFO_TYPE            (13UL) /**< @brief #RTP_ParsedPktInfo */
#define BRCM_ARCH_RTP_PAYLOAD_INFO_TYPE               (14UL) /**< @brief #RTP_PayloadInfo */
#define BRCM_ARCH_RTP_DATABUFFER_TYPE                 (15UL) /**< @brief #RTP_DataBufferType */
#define BRCM_ARCH_RTP_MEDIAFORMAT_TYPE                (16UL) /**< @brief #RTP_MediaFormat */
#define BRCM_ARCH_RTP_PAYLOAD_FMT_TYPE                (17UL) /**< @brief #RTP_PayloadFmt */
#define BRCM_ARCH_RTCP_STREAM_SENDER_CONFIG_TYPE      (18UL) /**< @brief #RTCP_StreamSenderConfig */
#define BRCM_ARCH_RTP_COMMONIMGHDR_TYPE               (19UL) /**< @brief #RTP_CommonImgHdr */
#define BRCM_ARCH_RTP_STREAMINFO_TYPE                 (20UL) /**< @brief #RTP_StreamInfo */
#define BRCM_ARCH_RTPS_STREAMCFG_TYPE                 (21UL) /**< @brief #RTPS_StreamCfg */
#define BRCM_ARCH_RTPR_STREAMCFG_TYPE                 (22UL) /**< @brief #RTPR_StreamCfg */
#define BRCM_ARCH_RTPS_CONFIG_TYPE                    (23UL) /**< @brief #RTPS_Config */
#define BRCM_ARCH_RTPR_CONFIG_TYPE                    (24UL) /**< @brief #RTPR_Config */
#define BRCM_ARCH_RTP_PACKETIZE_PROC                  (25UL) /**< @brief #RTP_Packetize */
#define BRCM_ACH_RTP_PACKET_PARSE_PROC                (26UL) /**< @brief #RTP_PacketParse */
#define BRCM_ARCH_RTP_CREATE_1733_SR_PROC             (27UL) /**< @brief #RTCP_Create1733SenderReport */
/** @} */

/**
  @name RTPPacketType
  @{
  @brief Type of RTP Packet

  @trace #BRCM_ARCH_RTPPACKET_TYPE #BRCM_REQ_RTPPacketType
*/
typedef uint32_t RTPPacketType;       /**< @brief RTP Packet type */

/** @def PACKET_PAYLOAD_UNDEFINED
    @brief  Value assigned at default before the packet is
    parsed by the library
*/
#define PACKET_PAYLOAD_UNDEFINED      0x0UL

/** @def PACKET_RTP_MEDIA_PAYLOAD
    @brief  Detected packet is a RTP packet containing video payload
*/
#define PACKET_RTP_MEDIA_PAYLOAD      0x1UL

/** @def PACKET_RTCP_1733_SR_PAYLOAD
    @brief  Detected packet is a RTCP 1733 packet
*/
#define PACKET_RTCP_1733_SR_PAYLOAD   0x2UL
/** @} */

/** @def RTP_PTS_TIMESTAMP_OFFSET_VAL_IN_MS
    @brief  Timestamp to be offset in milliseconds

    @trace #BRCM_ARCH_RTP_PTS_TIMESTAMP_OFFSET_MACRO #BRCM_REQ_RTP_PTS_TIMESTAMP_OFFSET_VAL_IN_MS
*/
#define RTP_PTS_TIMESTAMP_OFFSET_VAL_IN_MS    (100UL)

/**
    @brief H.264 Stream Params in RTP header (to be filled by client)
    during packetization

    @trace #BRCM_ARCH_RTP_H264_STREAM_INFO_TYPE #BRCM_REQ_RTP_H264StreamInfo
*/
typedef struct _RTP_H264StreamInfo {
    uint32_t isStartofFrame;    /**< @brief Flag indicating Start of Frame */

    uint32_t isEndofFrame;      /**< @brief Flag indicating End of Frame */

    uint32_t isSliceStart;      /**< @brief Flag indicating Start of Slice */

    uint32_t isSliceEnd;        /**< @brief Flag indicating End of Slice */

    uint32_t isMetaData;        /**< @brief Flag indicating whether this
                                     payload is having metadata (SPS/PPS) */

    uint32_t streamInfo;        /**< @brief H264 NAL reference IDC value
                                     for the payload in case the isSliceStart
                                     is set to 1UL */

    uint64_t timeStamp;         /**< @brief TimeStamp information */

    uint32_t rtpPktCount;       /**< @brief Count of RTP Packets created and
                                     successfully transmitted. */

    uint32_t payloadBytesSent;  /**< @brief Number of payload bytes sent overflow
                                     RTP Packet. */
}RTP_H264StreamInfo;

/**
    @brief Scratch memory used by RTP libary for creation for RTP/RTCP
    header during packetization

    @trace #BRCM_ARCH_RTP_SENDERLIB_PARAM_TYPE #BRCM_REQ_RTP_SenderLibParam
*/
typedef struct _RTP_SenderLibParam {
    uint16_t packetSeqNum;    /**< @brief Sequence number of the RTP Packet */

    uint32_t rtpTimestamp;    /**< @brief The RTP timestamp value used in the
                                   last RTP media payload packet is stored for
                                   use and creation of RTCP 1733 Sender report */

    uint32_t netTimestamp;    /**< @brief Network timestamp for mapping RTP
                                   timestamp in the last RTCP packet. */
}RTP_SenderLibParam;

/**
    @brief RTP Payload parameters used for RTP header addition and
    packetization

    @trace #BRCM_ARCH_RTP_SENDER_PAYLOAD_INFO_TYPE RTP_SenderPayloadInfo
*/
typedef struct _RTP_SenderPayloadInfo {
    RTPS_StreamCfg strParams;    /**< @brief RTP Stream configuration */

    RTP_H264StreamInfo strInfo;  /**< @brief H.264 payload information */

    RTP_SenderLibParam libParam; /**< @brief Scratch memory used by library */

}RTP_SenderPayloadInfo;

/**
    @brief Payload information for the RTP media packet

    @trace #BRCM_ARCH_RTP_MEDIA_PACKET_TYPE #BRCM_REQ_RTP_MediaPacket
*/
typedef struct _RTP_MediaPacket {
    uint32_t packetSeqNum;      /**< @brief Sequence number of RTP Packet */

    uint32_t NALType;           /**< @brief NAL type */

    uint32_t isSliceAggnStart;  /**< @brief Flag to indicate if this is start
                                     of a new slice/NAL Aggregation */

    uint32_t isSliceAggnEnd;    /**< @brief Flag indicating end of slice OR
                                     NAL aggregation */

    uint32_t payloadLen;        /**< @brief Actual length of the payload */

    uint32_t payloadOffset;     /**< @brief Offset of the payload */

    uint32_t rtpTS;             /**< @brief Timestamp present in the
                                     RTP packet in 90KHZ scale */
}RTP_MediaPacket;

/**
    @brief RTCP information for a sender report packet

    @trace #BRCM_ARCH_RTCP_1733SR_PACKET_TYPE #BRCM_REQ_RTCP_1733SRPacket
*/
typedef struct _RTCP_1733SRPacket {
    uint8_t  cnameVal[RTCP_CNAME_STRING_LEN];  /**< @brief CNAME String */

    uint32_t rtpPktsSent;               /**< @brief Number of RTP packets
                                             created and sent. */

    uint32_t rtpPayloadBytesSent;       /**< @brief Number of Payload
                                             bytes sent */

    uint64_t streamID;                  /**< @brief StreamID */

    uint16_t  gmTimeBaseInd;            /**< @brief GM Time base indicator
                                             value present in RTCP packet */

    uint8_t  gmIdentityValue[RTCP_GM_IDENTITY_VALUE_LEN];
                                       /**< @brief 1733 GM Identity */

    uint32_t rtpRefTime;               /**< @brief RTP reference timestamp */

    uint32_t networkRefTime;           /**< @brief Network reference time */
}RTCP_1733SRPacket;

/**
    @brief Extracted RTP params from the RTP packet

    @trace #BRCM_ARCH_RTP_PARSED_PKT_INFO_TYPE #BRCM_REQ_RTP_ParsedPktInfo
*/
typedef struct _RTP_ParsedPktInfo {
    uint32_t SSRCid;                   /**< @brief Stream ID of RTP source */

    RTPPacketType packetType;          /**< @brief Identified RTP/RTCP packet */

    RTP_MediaPacket mediaPkt;          /**< @brief Valid only if the packet type
                                            is media */

    RTCP_1733SRPacket sr1733Pkt;       /**< @brief Valid only if the packet type
                                            is 1733 SR */
}RTP_ParsedPktInfo;

/**
    @brief Payload information of the RTP packet

    @trace #BRCM_ARCH_RTP_PAYLOAD_INFO_TYPE #BRCM_REQ_RTP_PayloadInfo
*/
typedef struct _RTP_PayloadInfo {
    RTPR_StreamCfg strParams;          /**< @brief RTP stream configuration */

    RTP_ParsedPktInfo info;            /**< @brief Packet information to be
                                            filled by the library */

}RTP_PayloadInfo;

/**
    @brief Data buffer type of the RTP packet

    @trace #BRCM_ARCH_RTP_DATABUFFER_TYPE #BRCM_REQ_RTP_DataBufferType
*/
typedef struct _RTP_DataBufferType {
    uint8_t *buffer;                   /**< @brief Pointer to the buffer data */

    uint32_t length;                   /**< @brief Length of the data */

    uint32_t offset;                   /**< @brief Offset */

} RTP_DataBufferType;

/**
 * @brief Function to Packetize the H264 Payload
 *
 * @behavior Sync, Re-entrant
 *
 * @pre None
 *
 * This function creates the RTP header for the payload at the pointer passed
 * as output buffer. The library does not copy the input payload data to the
 * output buffer (to avoid a memory copy and memory optimizations). The client
 * which invokes this API shall copy the payload or adjust the output buffer
 * pointer at appropriate offset, so that payload copy can be avoided.
 *
 * @param[in]   inpBuf                Input payload params
 * @param[out]  outBuf                Output RTP packet params
 * @param[in]   data                  The Payload params
 *
 * @retval      #BCM_ERR_OK            Input data has been processed
 * @retval      #BCM_ERR_EAGAIN        Input data yet to be processed fully.
 *                                     The application needs to invoke this
 *                                     API again with the same input buffer
 *                                     and stream parameters but with a new
 *                                     output buffer for the complete payload
 *                                     to be processed
 * @retval      #BCM_ERR_INVAL_PARAMS  Invalid params
 *
 * @post None
 *
 * @trace #BRCM_ARCH_RTP_PACKETIZE_PROC #BRCM_REQ_BCM_REQ_RTP_PACKETIZE
 *
 * @limitations None
 */
int32_t RTP_Packetize (RTP_DataBufferType *inpBuf,
                       RTP_DataBufferType *outBuf,
                       RTP_SenderPayloadInfo *data);

/**
 * @brief Function to parse the RTP/RTCP packet for payload extraction
 *
 * @behavior Sync, Re-entrant
 *
 * @pre None
 *
 * This function parses the RTP/RTCP packet and retrieves the payload
 * information which the client can use for either payload extraction
 * or media clock synchronization in case if its RTCP 1733 packet
 *
 * @param[in]   buf                   Input RTP packet
 * @param[in]   bufLen                Total packet length
 * @param[in]   pktParams             The Payload params
 *
 * @retval      #BCM_ERR_OK            Input data has been processed
 * @retval      #BCM_ERR_NOSUPPORT     Packet payload parse not supported
 * @retval      #BCM_ERR_INVAL_PARAMS  Invalid params
 *
 * @post None
 *
 * @trace #BRCM_ACH_RTP_PACKET_PARSE_PROC #BRCM_REQ_RTP_PacketParse
 *
 * @limitations None
 */
int32_t RTP_PacketParse (uint8_t *buf,
                         uint32_t bufLen,
                         RTP_PayloadInfo *pktParams);

/**
 * @brief Function to Packetize the 1733 RTCP Sender Report
 *
 * @behavior Sync, Re-entrant
 *
 * @pre None
 *
 * This function creates the RTCP sender report packet using the payload
 * configuration details passed. This packet needs to be sent by the
 * client at periodic intervals to the receiver to help the listener
 * recreate the media clock.
 *
 * @param[out]  outBuf                Output RTCP packet.
 * @param[in]   data                  The RTP stream params.
 *
 * @retval      #BCM_ERR_OK            RTCP packet created.
 * @retval      #BCM_ERR_NOSUPPORT     API not supported
 * @retval      #BCM_ERR_INVAL_PARAMS  Invalid params.
 *
 * @post None
 *
 * @trace #BRCM_ARCH_RTP_CREATE_1733_SR_PROC #BRCM_REQ_RTCP_CREATE1733SR
 *
 * @limitations None
 */
int32_t RTCP_Create1733SenderReport (RTP_DataBufferType *outBuf,
                                     RTP_SenderPayloadInfo *data);

#endif /* RTP_PAYLOAD_H */
/** @} */
