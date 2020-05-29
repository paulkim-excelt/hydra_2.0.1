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
    @defgroup grp_avb_1722_ifc 1722 Interface
    @ingroup grp_avb

    @addtogroup grp_avb_1722_ifc
    @{

    @section sec_avb_scope Scope

    Interfaces of the Audio Video Bridging (AVB) library are provided
    which can be used by the component developer for developing a
    media component or an application developer to directly use the
    functionalities of the AVB library.

    Below Functionalities are currently supported:
    - 1722 talker (with 1722 packet header addition) for H264 video
      and PCM audio payload
    - 1722 listener (with 1722 header parsing and payload extraction)
      for H264 video and PCM audio payload
    - CRF packet creation and CRF packet parsing support
    - Network time based time-stamping in 1722 packets

    Not covered in the current implementation:
    - 1722 talker and listener for other payload formats
      E.g. YUV, compressed audio etc.
    - 1722.1 control interface
    - Stream reservations using SRP

    For details on the low level details of 1722-2016 specification,
    refer @ref sec_ref_1722_spec

    @section sec_avb_overview Overview
    @image html 1722lib_overview.jpg "Library Overview"

    AVB library comprises interface for
    - Talker: This sub component adds the 1722 header to the data
      payload along with network time based time stamping.
    - Listener: This sub component parses the incoming 1722 packets,
      validates the header fields and outputs the payload data.
      It picks the network timestamp value from the header and also
      verifies for lost packets using the packet sequence value.
    - CRF: This sub component creates/parses the CRF packet used
      for sending/receiving media clock information for synchronization.

    @section sec_avb_seq Sequence Diagrams
    @image html 1722lib_seq.jpg

    Above figure depicts a typical flow of application/client to
    interact with AVB library APIs.

    @section sec_1722lib_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @includedoc 1722lib.dep

    @limitations None

    @file avb_core.h
    @brief Interface to 1722 Library.
    This file exposes the interfaces to the AVB Library
    which manages the 1722 talker, 1722 listener and CRF modules.
    @version 1.0 Changelist1
*/

#ifndef AVB_CORE_H
#define AVB_CORE_H

#include <bcm_utils.h>
#include <bcm_err.h>

/**
    @name 1722 Library IDs
    @{
    @brief Architecture IDs for 1722 library interface
*/
#define BRCM_SWARCH_AVB_AVTP_BUFFER_SIZE_MACRO         (0x00U) /**< @brief #AVB_AVTP_BUFFER_SIZE */
#define BRCM_SWARCH_AVB_PAYLOAD_TYPE                   (0x01U) /**< @brief #AVB_PayloadType */
#define BRCM_SWARCH_AVB_L2PACKET_TYPE                  (0x02U) /**< @brief #AVB_L2PacketType */
#define BRCM_SWARCH_AVB_1722TLKRLIBCONTEXT_TYPE        (0x03U) /**< @brief #AVB_1722TlkrLibContextType */
#define BRCM_SWARCH_AVB_1722TLKRPAYLOADINFO_TYPE       (0x04U) /**< @brief #AVB_1722TlkrPayloadInfoType */
#define BRCM_SWARCH_AVB_1722TLKRBUFPARAMS_TYPE         (0x05U) /**< @brief #AVB_1722TlkrBufParamsType */
#define BRCM_SWARCH_AVB_1722LSTRPAYLOADEXTRACT_TYPE    (0x06U) /**< @brief #AVB_1722LstrPayloadExtractType */
#define BRCM_SWARCH_AVB_1722LSTRLIBCONTEXT_TYPE        (0x07U) /**< @brief #AVB_1722LstrLibContextType */
#define BRCM_SWARCH_AVB_1722LSTRBUFPARAMS_TYPE         (0x08U) /**< @brief #AVB_1722LstrBufParamsType */
#define BRCM_SWARCH_AVB_ADD1722HEADER_PROC             (0x09U) /**< @brief #AVB_Add1722Header */
#define BRCM_SWARCH_AVB_EXTRACT1722HEADER_PROC         (0x0AU) /**< @brief #AVB_Extract1722Header */
#define BRCM_SWARCH_AVB_DETECTL2PACKET_PROC            (0x0BU) /**< @brief #AVB_DetectL2Packet */
#define BRCM_SWARCH_AVB_PCM_PAYLOADCFG_TYPE            (0x0CU) /**< @brief #AVB_PCM_PayloadCfgType */
#define BRCM_SWARCH_AVB_AUDSAMPLEFORMAT_TYPE           (0x0DU) /**< @brief #AVB_AudSampleFormatType */
#define BRCM_SWARCH_AVB_AUDSAMPFREQ_TYPE               (0x0EU) /**< @brief #AVB_AudSampFreqType */
#define BRCM_SWARCH_AVB_AUDCHANNELS_TYPE               (0x0FU) /**< @brief #AVB_AudChannelsType */
#define BRCM_SWARCH_AVB_AUDTSMODE_TYPE                 (0x10U) /**< @brief #AVB_AudTSModeType */
#define BRCM_SWARCH_AVB_PAYLFORMAT_TYPE                (0x11U) /**< @brief #AVB_PaylFormatType */
#define BRCM_SWARCH_AVB_1722TLKRCFG_TYPE               (0x12U) /**< @brief #AVB_1722TlkrCfgType */
#define BRCM_SWARCH_AVB_1722LSTRLIBCFG_TYPE            (0x13U) /**< @brief #AVB_1722LstrLibCfgType */
/** @} */

/**
    @brief Size of AVTP Buffer size used

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
#define AVB_AVTP_BUFFER_SIZE            (1500UL)

/**
    @name AVB_PayloadType
    @{
    @brief Type of AVB Payload

    These state values are returned by the library function
    #AVB_Extract1722Header after parsing the input 1722 packet
    containing either H264 video or PCM payload.

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_PayloadType;             /**< @brief H264 Payload type */
#define AVB_PAYLOAD_UNDEFINED         (0x0UL) /**< @brief undefined payload */
#define AVB_PAYLOAD_H264_SPS_PPS_NAL  (0x1UL) /**< @brief payload with H264 SPS/PPS NAL data */
#define AVB_PAYLOAD_H264_IDR_START    (0x2UL) /**< @brief payload with H264 NAL start data */
#define AVB_PAYLOAD_H264_IDR_MID      (0x3UL) /**< @brief payload with H264 NAL intermediate data */
#define AVB_PAYLOAD_H264_IDR_END      (0x4UL) /**< @brief H264 NAL end payload */
#define AVB_PAYLOAD_PCM               (0x5UL) /**< @brief payload with PCM audio data */
/** @} */

/**
    @name AVB_L2PacketType
    @{
    @brief AVB packet type

    These state values are returned by the library function
    #AVB_DetectL2Packet after parsing the input 1722 packet

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_L2PacketType;            /**< @brief L2 Packet type */
#define AVB_L2PACKET_UNDEFINED        (0x0UL) /**< @brief L2 packet with undefined payload */
#define AVB_L2PACKET_COMPRESS_VIDEO   (0x1UL) /**< @brief L2 packet with compressed video payload */
#define AVB_L2PACKET_AVTP_AUDIO       (0x2UL) /**< @brief L2 packet with audio payload */
#define AVB_L2PACKET_CLOCK_REF        (0x3UL) /**< @brief L2 packet with CRF data */
/** @} */

/**
    @name AVB_PaylFormatType
    @{
    @brief List of supported payload formats

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_PaylFormatType;          /**< @brief Payload Format */
#define AVB_PAYLFORMAT_H264           (0x0UL) /**< @brief H264 payload type */
#define AVB_PAYLFORMAT_PCM            (0x1UL) /**< @brief PCM payload type */
/** @} */

/**
    @name AVB_AudSampleFormatType
    @{
    @brief Audio sample format values with respect to memory usage

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_AudSampleFormatType;         /**< @brief Audio sample format */
#define AVB_AUDSAMPLEFORMAT_USERSPECIFIED (0x0UL) /**< @brief User specified */
#define AVB_AUDSAMPLEFORMAT_FLOAT32       (0x1UL) /**< @brief 32 bits Float type */
#define AVB_AUDSAMPLEFORMAT_INT32         (0x2UL) /**< @brief 32 bits Int type */
#define AVB_AUDSAMPLEFORMAT_INT24         (0x3UL) /**< @brief 24 bits Int type */
#define AVB_AUDSAMPLEFORMAT_INT16         (0x4UL) /**< @brief 16 bits Int type */
/** @} */

/**
    @name AVB_AudSampFreqType
    @{
    @brief Audio sample frequency values

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_AudSampFreqType;        /**< @brief Audio frequency */
#define AVB_AUDSAMPFREQ_USER_SPEC    (0x0UL) /**< @brief User specified */
#define AVB_AUDSAMPFREQ_8KHZ         (0x1UL) /**< @brief 8KHZ rate */
#define AVB_AUDSAMPFREQ_16KHZ        (0x2UL) /**< @brief 16KHZ rate */
#define AVB_AUDSAMPFREQ_32KHZ        (0x3UL) /**< @brief 32KHZ rate */
#define AVB_AUDSAMPFREQ_44_1KHZ      (0x4UL) /**< @brief 44_1KHZ rate */
#define AVB_AUDSAMPFREQ_48KHZ        (0x5UL) /**< @brief 48KHZ rate */
/** @} */

/**
    @name AVB_AudChannelsType
    @{
    @brief Number of audio channels

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_AudChannelsType;         /**< @brief Audio channels */
#define AVB_AUDCHANNELS_MONO          (0x0UL) /**< @brief 1 channel, MONO */
#define AVB_AUDCHANNELS_STEREO        (0x1UL) /**< @brief 2 channel, STEREO */
/** @} */

/**
    @name AVB_AudTSModeType
    @{
    @brief Timestamping mode in 1722 packets containing audio payload

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef uint32_t AVB_AudTSModeType;           /**< @brief Audio timestamp mode */
#define AVB_AUDTSMODE_NORMAL          (0x0UL) /**< @brief Timestamp every packet */
#define AVB_AUDTSMODE_SPARSE          (0x1UL) /**< @brief Timestamp every 8th packet */
/** @} */

/**
    @brief PCM audio configuration

    Structure used to specify PCM payload parameters for
    input/output port of AVB Talker/Listener component.

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef struct _AVB_PCM_PayloadCfgType {
    AVB_AudSampleFormatType sampleFormat;     /**< @brief User Audio sample format */
    AVB_AudSampFreqType     frequency;        /**< @brief Audio sampling rate */
    AVB_AudChannelsType     channelsPerFrame; /**< @brief Samples/Channels per frame */
    uint32_t                bitDepth;         /**< @brief Number of valid bits in the
                                                   audio sample (Valid:0-32) */
    AVB_AudTSModeType       tsMode;           /**< @brief Timestamp mode */
} AVB_PCM_PayloadCfgType;

/**
    @brief 1722 Talker configuration structure

    Structure used to specify the configuration information for
    1722 packet creation

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef struct _AVB_1722TlkrCfgType {
    uint64_t               streamID;            /**< @brief Stream ID value */
    AVB_PaylFormatType     payloadFormat;       /**< @brief Payload format type */
    AVB_PCM_PayloadCfgType audCfg;              /**< @brief PCM payload config */
    uint32_t               mttNs;               /**< @brief Max Transmit Delay in ns */
    uint32_t               talkerProcDelayNs;   /**< @brief Talker processing delay in ns */
    uint32_t               listenerProcDelayNs; /**< @brief Listener processing delay in ns */
    uint32_t               rsv;                 /**< @brief Reserved */
} AVB_1722TlkrCfgType;

/**
    @brief 1722 Talker library context structure

    Structure used to manage the scratch/internal memory of 1722
    talker library module per stream

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722TlkrLibContextType {
    uint8_t   SeqNumCount; /**< @brief To track the packet sequence number */
    uint64_t  pktTs;       /**< @brief Timestamp used for netTS computation */
    uint32_t  netTS;       /**< @brief Network PTS computed for the system time */
} AVB_1722TlkrLibContextType;

/**
    @brief 1722 Talker payload structure

    Structure used to specify the payload information for
    1722 packet creation

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722TlkrPayloadInfoType {
    uint64_t timestamp;  /**< @brief Timestamp to be filled/passed */
    uint32_t sliceStart; /**< @brief Slice Start indicator */
    uint32_t sliceEnd;   /**< @brief Slice End indicator */
    uint32_t frameEnd;   /**< @brief Frame End indicator */
    uint32_t streamInfo; /**< @brief Stream information data based on payload format */
} AVB_1722TlkrPayloadInfoType;

/**
    @brief 1722 talker params for header 1722 addition structure

    Structure used to specify parameters of 1722 packets for header
    addition by the library and used by API #AVB_Add1722Header

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722TlkrBufParamsType {
    AVB_1722TlkrCfgType         strCfgForHdr;  /**< @brief Stream config using which 1722
                                                    header is created (R) */
    AVB_1722TlkrLibContextType  libMem;        /**< @brief Scratch memory variables for
                                                    the library (R/W) */
    AVB_1722TlkrPayloadInfoType payInfo;       /**< @brief Payload information for 1722
                                                    packet creation (W) */
} AVB_1722TlkrBufParamsType;

/**
    @brief 1722 listener payload information structure

    Structure used to manage the extracted payload params for the
    1722 packet

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722LstrPayloadExtractType {
    uint32_t        payLoadOffset;   /**< @brief Payload offset within packet */
    uint32_t        payLoadLen;      /**< @brief Actual payload length in incoming Rx packet */
    uint8_t         pktSeqNum;       /**< @brief Sequence number in incoming packets */
    uint8_t         pad[3U];         /**< @brief Reserved */
    uint64_t        pktSysTime;      /**< @brief System time associated with this payload */
    uint32_t        pktNetTime;      /**< @brief Network time associated with this payload */
    uint32_t        isTSUncertain;   /**< @brief Flag to indicate network time uncertainity */
    uint32_t        isTSValid;       /**< @brief Flag to indicate timestamp validity */
    uint32_t        isSliceEnd;      /**< @brief Flag to indicate end of a slice */
    AVB_PayloadType payType;         /**< @brief Value that specifies the detected payload type */
    uint64_t        streamID;        /**< @brief Stream ID */
} AVB_1722LstrPayloadExtractType;

/**
    @brief 1722 listener library context structure

    Structure used to manage the scratch memory of 1722
    listener library module.

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722LstrLibContextType {
    /**< @brief No internal memory needed for now */
} AVB_1722LstrLibContextType;

/**
    @brief 1722 listener library configuration structure

    Structure used to provide the configuration parameters to the
    1722 listener library module.

    @trace #BRCM_SWREQ_AVB_1722_CONFIG
*/
typedef struct _AVB_1722LstrLibCfgType {
    /**< @brief No configuration needed for now */
} AVB_1722LstrLibCfgType;

/**
    @brief Params structure for extraction of 1722 packet

    Structure used to provide information of the 1722 packet parsed by
    the 1722 listener. This API is used by the API
    #AVB_Extract1722Header.

    @trace #BRCM_SWREQ_AVB_1722
*/
typedef struct _AVB_1722LstrBufParamsType {
    AVB_1722LstrLibCfgType         libCfg;     /**< @brief Library config (R) */
    AVB_1722LstrLibContextType     libMem;     /**< @brief Scratch memory variables
                                                    for the library (R/W) */
    AVB_1722LstrPayloadExtractType payDetails; /**< @brief Payload details of input
                                                    1722 packet (W) */
} AVB_1722LstrBufParamsType;

/** @brief API to Add 1722 Header to payload.

    This is a library function used to add 1722 header
    to the input data payload.

    @behavior Sync, Reentrant

    @pre None

    @param[in]      aBuf            Pointer to buffer containing payload data
    @param[in]      aPayloadLen     Length of payload data
    @param[inout]   aParams         Parameters for header generation

    Return values are documented in reverse-chronological order
    @retval  #BCM_ERR_INVAL_PARAMS  For invalid arguments or configuration
    @retval  #BCM_ERR_OK            On Success

    @post None

    @trace #BRCM_SWREQ_AVB_1722

    @limitations None
*/
int32_t AVB_Add1722Header (uint8_t *aBuf,
                           uint32_t aPayloadLen,
                           AVB_1722TlkrBufParamsType *aParams);

/** @brief API to de-packetize 1722 packets and form NAL payload.

    This is a library function used to parse the 1722 packet
    and extract the data payload details and other parameters from the
    1722 header.

    @behavior Sync, Reentrant

    @pre None

    @param[in]   aBuf                 Input Rx data buffer pointer
    @param[in]   aBufLen              Input Rx data buffer length
    @param[out]  aParams              Output payload params

    Return values are documented in reverse-chronological order
    @retval  #BCM_ERR_INVAL_PARAMS    For invalid arguments or configuration
    @retval  #BCM_ERR_OK              On Success

    @post None

    @trace #BRCM_SWREQ_AVB_1722

    @limitations None
*/

int32_t AVB_Extract1722Header (uint8_t *aBuf,
                               uint32_t aBufLen,
                               AVB_1722LstrBufParamsType *aParams);

/** @brief API to identify the 1722 packet payload type.

    This is a Core library function used to identify the packet payload
    type to help identify the appropriate parse 1722 function for
    invocation.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aBuf                 Input Rx packet
    @param[out]  aPktType             Packet type

    Return values are documented in reverse-chronological order
    @retVal  #BCM_ERR_NOSUPPORT       In case an unsupported or an invalid
                                      packet is provided
    @retval  #BCM_ERR_OK              On Success

    @post None

    @trace #BRCM_SWREQ_AVB_1722

    @limitations None
*/
int32_t AVB_DetectL2Packet(uint8_t *aBuf,
                           AVB_L2PacketType *aPktType);

#endif /* AVB_CORE_H */
/** @} */
