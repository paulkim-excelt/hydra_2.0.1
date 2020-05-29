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
  @defgroup grp_avb_crf_ifc CRF Interface
  @ingroup grp_avb

  @addtogroup grp_avb_crf_ifc
  @{
  @file crf.h
  @brief Interface to CRF functionality within AVB library.
*/

#ifndef CRF_H
#define CRF_H

/**
    @name CRF interface IDs
    @{
    @brief Architecture IDs for CRF interface
*/
#define BRCM_SWARCH_AVB_CRF_MAX_TS_NUM_MACRO      (0x17U) /**< @brief #AVB_CRF_MAX_TS_NUM_COUNT */
#define BRCM_SWARCH_AVB_CRF_TLKRPARAMS_TYPE       (0x18U) /**< @brief #AVB_CRF_TlkrParamsType */
#define BRCM_SWARCH_AVB_CRF_LSTRPARAMS_TYPE       (0x19U) /**< @brief #AVB_CRF_LstrParamsType */
#define BRCM_SWARCH_AVB_CRF_CONSTRUCTPACKET_PROC  (0x1AU) /**< @brief #AVB_CRF_ConstructPacket */
#define BRCM_SWARCH_AVB_CRF_EXTRACTIMESTAMP_PROC  (0x1BU) /**< @brief #AVB_CRF_ExtractTimestamp */
#define BRCM_SWARCH_AVB_CRF_TIMESTAMP_TYPE        (0x1CU) /**< @brief #AVB_CRF_TimestampType */
#define BRCM_SWARCH_AVB_CRF_CLKPULLVAL_TYPE       (0x1DU) /**< @brief #AVB_CRF_ClkPullValType */
#define BRCM_SWARCH_AVB_CRF_TALKERCFG_TYPE        (0x1EU) /**< @brief #AVB_CRF_TalkerCfgType */
#define BRCM_SWARCH_AVB_CRF_LISTENERCFG_TYPE      (0x1FU) /**< @brief #AVB_CRF_ListenerCfgType */
/** @} */

/**
    @brief Maximum number of supported timestamps in a CRF packet

    @trace #BRCM_SWREQ_AVB_CRF
*/
#define AVB_CRF_MAX_TS_NUM_COUNT       (4UL)    /**< @brief Maximum of 4 TS per packet */

/**
    @name AVB_CRF_TimestampType values
    @{
    @brief Time interval of the media payload which is used for
    generation of CRF timestamps

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef uint32_t AVB_CRF_TimestampType;           /**< @brief CRF Timestamp value */
#define AVB_CRF_TIMESTAMP_USER_DEFINED    (0x0UL) /**< @brief CRF timestamps are uer defined */
#define AVB_CRF_TIMESTAMP_AUDIO_SAMPLE    (0x1UL) /**< @brief CRF timestamps are for audio samples */
#define AVB_CRF_TIMESTAMP_VIDEO_FRAME     (0x2UL) /**< @brief CRF timestamps are for video frame */
#define AVB_CRF_TIMESTAMP_VIDEO_LINE      (0x3UL) /**< @brief CRF timestamps are for video line */
/** @} */

/**
    @name CRF Clock Pull Value
    @{
    @brief Clock pull values that are used to indicate the jitter/delta
    in the base clock frequency value

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef uint32_t AVB_CRF_ClkPullValType;               /**< @brief CRF Clock Pull value */
#define AVB_CRF_CLKPULLVAL_MUL_1_0             (0x0UL) /**< @brief Multiply base_frequency field by 1.0 */
#define AVB_CRF_CLKPULLVAL_MUL_1_BY_1_001      (0x1UL) /**< @brief Multiply base_frequency field by 1/1.001 */
#define AVB_CRF_CLKPULLVAL_MUL_1_001           (0x2UL) /**< @brief Multiply base_frequency field by 1.001 */
#define AVB_CRF_CLKPULLVAL_MUL_24_BY_25        (0x3UL) /**< @brief Multiply base_frequency field by 24/25 */
#define AVB_CRF_CLKPULLVAL_MUL_25_BY_24        (0x4UL) /**< @brief Multiply base_frequency field by 25/24 */
#define AVB_CRF_CLKPULLVAL_MUL_1_BY_8          (0x5UL) /**< @brief Multiply base_frequency field by 1/8 */
/** @} */

/**
    @brief CRF Talker configuration

    Structure used to specify CRF parameters for the CRF talker.

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef struct _AVB_CRF_TalkerCfgType {
    uint32_t                clockIndex;        /**< @brief Media clock index - Unique index to identify a media clock */
    uint32_t                baseFrequency;     /**< @brief Media clock base frequency in Hertz. Base sampling rate
                                                    shall be from 1 Hz to 536 870 911 (0x1FFFFFFF) Hz. */
    AVB_CRF_ClkPullValType  pullValue;         /**< @brief Media clock pull value (Pull field specifies the multipler
                                                    modifier of the base_frequency field which is required to
                                                    calculate the appropriate nominal sampling rate of CRF stream.) */
    AVB_CRF_TimestampType   crfTsType;         /**< @brief CRF type value */
    uint32_t                crfTsInterval;     /**< @brief CRF timestamp interval - frequency of sending a CRF packet */
    uint32_t                crfNumTsPerPacket; /**< @brief Number of CRF timestamps per packet
                                                    (<=AVB_CRF_MAX_TS_NUM_COUNT) */
    uint64_t                crfStreamID;       /**< @brief CRF Stream ID */
    uint64_t                mttCRFNs;          /**< @brief Max Transmit Delay in ns to be added to CRF timestamps */
    uint8_t                 crfMCAddr [8];     /**< @brief Multicast Address for sennding the CRF stream */
    uint32_t                pcp;               /**< @brief Priority Code Point Value */
    uint32_t                vlanID;            /**< @brief VLAN ID */
    uint32_t                ethCtrlIdx;        /**< @brief Ethernet controller index */
    uint32_t                rsv;               /**< @brief Reserved for maintaining *8 byte structure */
} AVB_CRF_TalkerCfgType;

/**
    @brief CRF Listener configuration

    Structure used to specify CRF parameters for the CRF Listener.

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef struct _AVB_CRF_ListenerCfgType {
    uint64_t  crfStreamID;  /**< @brief CRF Stream ID for use in
                                 local media clock synchronization */
    uint32_t  clockIndex;   /**< @brief Media clock index - Unique index
                                 to identify a media clock */
} AVB_CRF_ListenerCfgType;

/**
    @brief CRF Talker attribute structure

    Structure used to specify params of CRF for creation of a CRF
    packet to be sent from a end point which is a CRF talker and used
    by library API #AVB_CRF_ConstructPacket.

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef struct _AVB_CRF_TlkrParamsType {
    AVB_CRF_TalkerCfgType crfCfgForHdr;           /**< @brief CRF config using which
                                                       CRF packet is created */
    uint64_t              timestamp[AVB_CRF_MAX_TS_NUM_COUNT];
                                                  /**< @brief Timestamp (local clock) to
                                                       be used for packet creation */
    uint8_t               SeqNumCount;            /**< @brief Track the packet sequence number */
    uint8_t               numTimestamp;           /**< @brief Number of timestamps passed */
} AVB_CRF_TlkrParamsType;

/**
    @brief CRF Listener attribute structure

    Structure used to extract params from a CRF packet which shall be
    used for local media clock synchronization and used by library
    API #AVB_ExtractCRFTimestamp

    @trace #BRCM_SWREQ_AVB_CRF
*/
typedef struct _AVB_CRF_LstrParamsType {
    AVB_CRF_ListenerCfgType crfCfgForHdr;          /**< @brief CRF config using which
                                                        CRF packet is extracted */
    uint64_t                timestamp[AVB_CRF_MAX_TS_NUM_COUNT];
                                                   /**< @brief Timestamp
                                                        (in local clock) for media clock sync  */
    uint8_t                 numTimestamp;          /**< @brief Number of timestamps
                                                        fetched from CRF packet */
    uint8_t                 pktSeqNum;             /**< @brief Sequence number in
                                                        CRF packet */
    uint32_t                isTSUncertain;         /**< @brief Flag to indicate network
                                                        time uncertainity */
    uint32_t                isMediaClockRestart;   /**< @brief Flag to indicate source
                                                        media clock restart */
    uint64_t                crfStreamID;           /**< @brief CRF Stream ID parsed
                                                        in the CRF packet */
    uint32_t                baseFrequency;         /**< @brief This identifies the source
                                                        media clock frequency */
    AVB_CRF_ClkPullValType  pullValue;             /**< @brief Source Media clock
                                                        pull value */
    uint32_t                crfTsInterval;         /**< @brief CRF timestamp interval */
    AVB_CRF_TimestampType   crfTsType;             /**< @brief CRF type value */
} AVB_CRF_LstrParamsType;

/** @brief API to create the CRF packet for transmission

    This is a library interface function used to create a CRF packet
    from the timestamps provided through the CRF configuration.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]    aBuf         Pointer to buffer
    @param[inout] aBufLen      Length of buffer
    @param[inout] aCRFConfig   Config params and timestamps for
                               CRF packet creation

    Return values are documented in reverse-chronological order
    @retval  #BCM_ERR_OK            On Success
    @retval  #BCM_ERR_NOSUPPORT     For a CRF mode that is currently
                                    unsupported
    @retval  #BCM_ERR_INVAL_PARAMS  On error due to buffer pointer being
                                    NULL or the buffer length is zero or
                                    the CRF config params is NULL

    @post None

    @trace  #BRCM_SWREQ_AVB_CRF

    @limitations None
*/
int32_t AVB_CRF_ConstructPacket(uint8_t *aBuf,
                                uint32_t *aBufLen,
                                AVB_CRF_TlkrParamsType *aCRFConfig);

/** @brief API to parse the CRF packet

    This is a library function used to parse the CRF packet
    and extract the timestamps for local clock synchronization.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]   aBuf           Input Rx CRF packet
    @param[in]   aBufLen        Input Rx CRF packet length
    @param[out]  aCRFPktData    Parsed timestamp/s from CRF packet
                                CRF packet creation

    Return values are documented in reverse-chronological order
    @retval  #BCM_ERR_OK            On Success
    @retval  #BCM_ERR_NOSUPPORT     In case an unsupported or an invalid
                                    packet is provided
    @retval  #BCM_ERR_INVAL_PARAMS  On CRF packet parsing failure due to
                                    Invalid arguments or an invalid packet

    @post None

    @trace  #BRCM_SWREQ_AVB_CRF

    @limitations None
*/
int32_t AVB_CRF_ExtractTimestamp(uint8_t *aBuf,
                                uint32_t aBufLen,
                                AVB_CRF_LstrParamsType *aCRFPktData);

#endif /* CRF_H */

/** @} */
