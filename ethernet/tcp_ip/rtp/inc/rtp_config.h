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
  @file rtp_config.h
  @brief Data structures of the RTP Library
  This file exposes the configuration structures of the RTP Library
  which are used for component configuration.
  @version 0.51 imported from the MS word document
*/

#ifndef RTP_CONFIG_H
#define RTP_CONFIG_H

/** @def RTP_MAX_NUM_PORTS
    @brief A macro indicating maximum number of ports

    @trace #BRCM_ARCH_RTP_MAX_NUM_PORTS_MACRO #BRCM_REQ_RTP_MAX_NUM_PORTS
*/
#define RTP_MAX_NUM_PORTS (0x4)

/**
  @name RTP_MediaFormat
  @{
  @brief Media format list

  @trace #BRCM_ARCH_RTP_MEDIAFORMAT_TYPE #BRCM_REQ_RTP_MediaFormat
*/
typedef uint32_t RTP_MediaFormat;             /**< @brief RTP Media format */

/** @def VID_H264_MEDIA_TYPE
    @brief H264 encoded video format
*/
#define VID_H264_MEDIA_TYPE                      (0UL)

/** @def AUD_PCM_MEDIA_TYPE
    @brief Uncompressed PCM audio format
*/
#define AUD_PCM_MEDIA_TYPE                       (1UL)
/** @} */

/**
  @name RTP_PayloadFmt
  @{
  @brief Payload format for H264 - RFC 6184

  @trace #BRCM_ARCH_RTP_PAYLOAD_FMT_TYPE #BRCM_REQ_RTP_PayloadFmt
*/
typedef uint32_t RTP_PayloadFmt;             /**< @brief RTP Payload format */

/** @def RTP_PAYLOAD_FORMAT_UNDEFINED
    @brief Unspecified payload format
*/
#define RTP_PAYLOAD_FORMAT_UNDEFINED             (0UL)

/** @def RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE
    @brief H264 video – Fragmentation payload unit type A
    as defined by RFC 6184
*/
#define RTP_H264_VID_PAYLOAD_FORMAT_FU_A_SLICE   (1UL)

/** @def RTP_H264_VID_PAYLOAD_FORMAT_MPEGTS
    @brief H264 video in MPEG2 Transport stream packet
    as defined by ISO/IEC 13818-1
*/
#define RTP_H264_VID_PAYLOAD_FORMAT_MPEGTS       (2UL)
/** @} */

/** @def RTP_PAYLOAD_FRAGMENT_LEN
    @brief The maximum supported fragmentation size in bytes
    for each FU-A based RTP packet

    @trace #BRCM_ARCH_RTP_PAYLOAD_FRAGMENT_LEN_MACRO #BRCM_REQ_RTP_PAYLOAD_FRAGMENT_LEN
*/
#define RTP_PAYLOAD_FRAGMENT_LEN                 (64000UL)

/** @def RTCP_CNAME_STRING_LEN
    @brief The maximum supported length for CNAME value in
    RTCP Sender SDES packet

    @trace #BRCM_ARCH_RTCP_CNAME_STRING_LEN_MACRO #BRCM_REQ_RTCP_CNAME_STRING_LEN
*/
#define RTCP_CNAME_STRING_LEN                    (256UL)

/** @def RTCP_1733_NAME_STRING_LEN
    @brief The length for name in RTCP 1733 packet

    @trace #BRCM_ARCH_RTCP_1733_NAME_STRING_LEN_MACRO #BRCM_REQ_RTCP_1733_NAME_STRING_LEN
*/
#define RTCP_1733_NAME_STRING_LEN                (4UL)

/** @def RTCP_GM_IDENTITY_VALUE_LEN
    @brief The length for GM identity value in RTCP Sender 1733 packet

    @trace #BRCM_ARCH_RTCP_GM_IDENTITY_VALUE_LEN_MACRO #BRCM_REQ_RTCP_GM_IDENTITY_VALUE_LEN
*/
#define RTCP_GM_IDENTITY_VALUE_LEN               (10UL)


/**
    @brief   Structure for specifying the RTCP configuration parameters

    @trace #BRCM_ARCH_RTCP_STREAM_SENDER_CONFIG_TYPE #BRCM_REQ_RTCP_StreamSenderConfig
*/
typedef struct _RTCP_StreamSenderConfig {
    uint32_t isRTCPEnabled;   /**< @brief Flag to enable RTCP support */

    uint8_t  cnameStr[RTCP_CNAME_STRING_LEN];
                              /**< @brief CNAME string used to uniquely
                                   identify the RTP stream in case of
                                   SSRC ID clash */

    uint8_t  name1733Str[RTCP_1733_NAME_STRING_LEN];
                              /**< @brief 1733 NAME string value */

    uint64_t streamID;        /**< @brief Stream ID used to map the
                                   RTP stream in AVB domain */

    uint32_t rtcpPayloadID;   /**< @brief Payload ID for UDP Transmission
                                   of RTCP stream*/

    INET_EndPointIPPort endPtCfg; /**< @brief UDP port params for RTCP packets */

    uint32_t rsv;             /**< @brief Reserved */
} RTCP_StreamSenderConfig;

/**
    @brief   Structure used to add the header information of the RTP
    configuration image

    @trace #BRCM_ARCH_RTP_COMMONIMGHDR_TYPE #BRCM_REQ_RTP_CommonImgHdr
*/
typedef struct _RTP_CommonImgHdr {
    uint32_t cfgMagic;        /**< @brief Magic */

    uint32_t cfgType;         /**< @brief Config Type */
} RTP_CommonImgHdr;

/**
    @brief   Configuration structure which specifies the RTP stream
    payload parameters.

    @trace #BRCM_ARCH_RTP_STREAMINFO_TYPE #BRCM_REQ_RTP_StreamInfo
*/
typedef struct _RTP_StreamInfo {
    RTP_MediaFormat  mediaType;   /**< @brief Payload media format */

    uint32_t SSRCValue;           /**< @brief Synchronization source identifier
                                       to be used in each RTP header used to
                                       uniquely identify this stream */

    RTP_PayloadFmt payloadFmt;    /**< @brief Payload format */

    uint32_t rtpPayloadID;        /**< @brief Paylod ID to be used for UDP */

    INET_EndPointIPPort endPtCfg; /**< @brief UDP port params for RTP packets */
} RTP_StreamInfo;

/**
    @brief   Structure used to specify stream parameters for
    RTP sender component

    @trace #BRCM_ARCH_RTPS_STREAMCFG_TYPE #BRCM_REQ_RTPS_StreamCfg
*/
typedef struct _RTPS_StreamCfg {
    RTP_StreamInfo strInfo;           /**< @brief RTP Stream Parameters */
    RTCP_StreamSenderConfig rtcpCfg;  /**< @brief RTCP Control Stream Params */
} RTPS_StreamCfg;

/**
    @brief   Structure used to specify stream parameters for
    RTP listener component

    @trace #BRCM_ARCH_RTPR_STREAMCFG_TYPE #BRCM_REQ_RTPR_StreamCfg
*/
typedef struct _RTPR_StreamCfg {
    RTP_StreamInfo strInfo;           /**< @brief RTP Stream Parameters */

    uint32_t rtcpPayloadID;           /**< @brief Payload ID */
} RTPR_StreamCfg;

/**
    @brief   Config structure for RTP sender component

    @trace #BRCM_ARCH_RTPS_CONFIG_TYPE #BRCM_REQ_RTPS_Config
*/
typedef struct _RTPS_Config {
    RTP_CommonImgHdr imgHdr;          /**< @brief Common Header */

    uint32_t numInpPort;              /**< @brief Number of input ports */

    uint32_t rsv;                     /**< @brief Reserved */

    RTPS_StreamCfg inpStreamCfg[RTP_MAX_NUM_PORTS];
                                      /**< @brief RTP input stream config */
} RTPS_Config;

/**
    @brief   Config structure for RTP listener component

    @trace #BRCM_ARCH_RTPR_CONFIG_TYPE #BRCM_REQ_RTPR_Config
*/
typedef struct _RTPR_Config {
    RTP_CommonImgHdr imgHdr;          /**< @brief Common Header */

    uint32_t numOutPort;              /**< @brief Number of output ports */

    uint32_t rsv;                     /**< @brief Reserved */

    RTPR_StreamCfg outStreamCfg[RTP_MAX_NUM_PORTS];
                                      /**< @brief RTP output stream config */
} RTPR_Config;

#endif /* RTP_CONFIG_H */
/** @} */
