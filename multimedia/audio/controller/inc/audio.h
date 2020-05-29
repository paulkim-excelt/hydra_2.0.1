/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_audio Audio
    @ingroup grp_drivers

    @addtogroup grp_audio
    @{
    @section sec_audio_overview Overview
    @image html audio_overview.jpg "Component Overview"
    Audio driver configures and manages on-chip audio controllers. Audio driver
    allows simultaneous playback and record on all the audio controllers. Audio
    driver interface restricts single playback client per controller and also
    restricts single record client per controller.
    Audio driver supports playback and record of following audio data buffer
    format:
    - Signed audio data samples
    - Channel interleaved
      + Stereo Format
      <table>
        <tr>
            <td>Left<BR>channel<BR>sample 1
            <td>Right<BR>channel<BR>sample 1
            <td>Left<BR>channel<BR>sample 2
            <td>Right<BR>channel<BR>sample 2
            <td>...
            <td>Left<BR>channel<BR>sample n
            <td>Right<BR>channel<BR>sample n
      </table>
      + TDM format
      <table>
        <tr>
            <td>Channel 1<BR>sample 1
            <td>Channel 2<BR>sample 1
            <td>Channel 3<BR>sample 1
            <td>Channel 4<BR>sample 1
            <td>Channel 1<BR>sample 2
            <td>Channel 2<BR>sample 2
            <td>Channel 3<BR>sample 2
            <td>Channel 4<BR>sample 2
            <td>...
            <td>...
            <td>Channel 1<BR>sample n
            <td>Channel 2<BR>sample n
            <td>Channel 3<BR>sample n
            <td>Channel 4<BR>sample n
      </table>
    - Audio data sample should be MSB aligned with in 4byte words
    - Audio data samples should be arranged in little endian manner in the
      memory

    @section sec_audio_fsm State Machine
    @subsection subsec_audio_fsm_drv Audio Driver State Machine
    @image html audio_state_machine_drv.jpg "Audio driver state machine"
    On moving the audio driver to INIT state, play state and record state
    transitions to #AUDIO_PLAY_STATE_RESET and #AUDIO_REC_STATE_RESET
    respectively.

    @subsection subsec_audio_fsm_play Audio Driver Play State Machine
    @image html audio_state_machine_play.jpg "Audio driver play state machine"

    @subsection subsec_audio_fsm_rec Audio Driver Record State Machine
    @image html audio_state_machine_rec.jpg "Audio driver record state machine"

    @section sec_audio_seq Sequence Diagrams
    @subsection subsec_audio_seq_init Driver Init-Deinit
    @image html audio_seq_diagram_init.jpg "Driver init–deinit sequence diagram"

    @subsection subsec_audio_seq_play Audio Play
    @image html audio_seq_diagram_play.jpg "Driver play sequence diagram"

    @subsection subsec_audio_seq_rec Audio Recored
    @image html audio_seq_diagram_rec.jpg "Driver recored sequence diagram"

    @section sec_audio_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   NA                                              |

    @limitations Audio driver interface restricts single playback client per
    controller.
    @limitations Audio driver interface restricts single record client per
    controller.

    @file audio.h
    @brief Aduio application interface
    This header file contains APIs for Audio driver.
    @version 0.20 Imported from docx
*/

#ifndef AUDIO_H
#define AUDIO_H

#include <dma.h>

/**
    @name Component API IDs
    @{
    @brief API IDs for Component
*/
#define BRCM_SWARCH_AUDIO_HW_ID_TYPE            (0x00U) /**< @brief #Audio_HwIDType */
#define BRCM_SWARCH_AUDIO_LEN_TYPE              (0x01U) /**< @brief #Audio_LenType */
#define BRCM_SWARCH_AUDIO_FREQ_TYPE             (0x02U) /**< @brief #Audio_FreqType */
#define BRCM_SWARCH_AUDIO_CLK_FREQ_TYPE         (0x03U) /**< @brief #Audio_ClkFreqType */
#define BRCM_SWARCH_AUDIO_TST_TYPE              (0x04U) /**< @brief #Audio_TSType */
#define BRCM_SWARCH_AUDIO_CHAN_NUM_TYPE         (0x05U) /**< @brief #Audio_ChanNumType */
#define BRCM_SWARCH_AUDIO_MCLK_SRC_TYPE         (0x06U) /**< @brief #Audio_MClkSrcType */
#define BRCM_SWARCH_AUDIO_INF_MODE_TYPE         (0x07U) /**< @brief #Audio_InfModeType */
#define BRCM_SWARCH_AUDIO_INT_DATA_DELAY_TYPE   (0x08U) /**< @brief #Audio_InfDataDlyType */
#define BRCM_SWARCH_AUDIO_INF_DATA_JUST_TYPE    (0x09U) /**< @brief #Audio_InfDataJustType */
#define BRCM_SWARCH_AUDIO_SLOT_MASK_TYPE        (0x0AU) /**< @brief #Audio_SlotMaskType */
#define BRCM_SWARCH_AUDIO_WIDTH_TYPE            (0x0BU) /**< @brief #Audio_WidthType */
#define BRCM_SWARCH_AUDIO_PACK_TYPE             (0x0CU) /**< @brief #Audio_PackType */
#define BRCM_SWARCH_AUDIO_PLAY_STATE_TYPE       (0x0DU) /**< @brief #Audio_PlayStateType */
#define BRCM_SWARCH_AUDIO_REC_STATE_TYPE        (0x0EU) /**< @brief #Audio_RecStateType */
#define BRCM_SWARCH_AUDIO_PLAY_STATS_TYPE       (0x0FU) /**< @brief #Audio_PlayStatsType */
#define BRCM_SWARCH_AUDIO_REC_STATS_TYPE        (0x10U) /**< @brief #Audio_RecStatsType */
#define BRCM_SWARCH_AUDIO_BUF_INFO_TYPE         (0x11U) /**< @brief #Audio_BufInfoType */
#define BRCM_SWARCH_AUDIO_CFG_TYPE              (0x12U) /**< @brief #Audio_CfgType */
#define BRCM_SWARCH_AUDIO_PLAY_CFG_TYPE         (0x13U) /**< @brief #Audio_PlayCfgType */
#define BRCM_SWARCH_AUDIO_REC_CFG_TYPE          (0x14U) /**< @brief #Audio_RecCfgType */
#define BRCM_SWARCH_AUDIO_INFO_TYPE             (0x15U) /**< @brief #Audio_InfoType */
#define BRCM_SWARCH_AUDIO_INIT_PROC             (0x16U) /**< @brief #Audio_Init */
#define BRCM_SWARCH_AUDIO_DEINIT_PROC           (0x17U) /**< @brief #Audio_DeInit */
#define BRCM_SWARCH_AUDIO_PLAY_CFG_PROC         (0x18U) /**< @brief #Audio_PlayCfg */
#define BRCM_SWARCH_AUDIO_PLAY_START_PROC       (0x19U) /**< @brief #Audio_PlayStart */
#define BRCM_SWARCH_AUDIO_PLAY_STOP_PROC        (0x1AU) /**< @brief #Audio_PlayStop */
#define BRCM_SWARCH_AUDIO_GET_PLAY_STATE_PROC   (0x1BU) /**< @brief #Audio_GetPlayState */
#define BRCM_SWARCH_AUDIO_DEQUEUE_PLAY_BUF_PROC (0x1CU) /**< @brief #Audio_DeQueuePlayBuf */
#define BRCM_SWARCH_AUDIO_QUEUE_PLAY_BUF_PROC   (0x1DU) /**< @brief #Audio_QueuePlayBuf */
#define BRCM_SWARCH_AUDIO_GET_PLAY_INFO_PROC    (0x1EU) /**< @brief #Audio_GetPlayInfo */
#define BRCM_SWARCH_AUDIO_GET_PLAY_STATS_PROC   (0x1FU) /**< @brief #Audio_GetPlayStats */
#define BRCM_SWARCH_AUDIO_REC_CFG_PROC          (0x20U) /**< @brief #Audio_RecCfg */
#define BRCM_SWARCH_AUDIO_REC_START_PROC        (0x21U) /**< @brief #Audio_RecStart */
#define BRCM_SWARCH_AUDIO_REC_STOP_PROC         (0x22U) /**< @brief #Audio_RecStop */
#define BRCM_SWARCH_AUDIO_GET_REC_STATE_PROC    (0x23U) /**< @brief #Audio_GetRecState */
#define BRCM_SWARCH_AUDIO_DEQUEUE_REC_BUF_PROC  (0x24U) /**< @brief #Audio_DeQueueRecBuf */
#define BRCM_SWARCH_AUDIO_QUEUE_REC_BUF_PROC    (0x25U) /**< @brief #Audio_QueueRecBuf */
#define BRCM_SWARCH_AUDIO_GET_REC_INFO_PROC     (0x26U) /**< @brief #Audio_GetRecInfo */
#define BRCM_SWARCH_AUDIO_GET_REC_STATS_PROC    (0x27U) /**< @brief #Audio_GetRecStats */
/** @} */

/**
    @name Audio_HwIDType
    @{
    @brief Audio controller hardware ID type

    0 corresponds to 1st controller in chip and N corresponds to last controller
    in the chip. Refer to @ref sec_ref_trm.

    @trace #BRCM_SWREQ_AUDIO_HW_ID
*/
typedef uint32_t Audio_HwIDType;    /**< @brief Audio controller hardware ID
                                         type */
/** @} */

/**
    @name Audio_LenType
    @{
    @brief Audio Length type

    This type is used to specify the size of queue, dequeue buffers etc.

    @trace #BRCM_SWREQ_AUDIO_LEN
*/
typedef uint32_t Audio_LenType;     /**< @brief Audio length type */
/** @} */

/**
    @name Audio_FreqType
    @{
    @brief Audio Frequency type

    This type is used to specify the sampling frequency of the audio interface
    for audio play or audio record.

    @trace #BRCM_SWREQ_AUDIO_FREQ
*/
typedef uint32_t Audio_FreqType;    /**< @brief Audio Frequency type */
/** @} */

/**
    @name Audio_ClkFreqType
    @{
    @brief Audio Clock Frequency type

    This type is used to specify the input clock frequency of clock source to
    audio controller.

    @trace #BRCM_SWREQ_AUDIO_CLK_FREQ
*/
typedef uint32_t Audio_ClkFreqType; /**< @brief Audio clock frequency type */
/** @} */

/**
    @name Audio_TSType
    @{
    @brief Audio timestamp type

    This type is used to specify timestamp of the captured buffer.

    @trace #BRCM_SWREQ_AUDIO_TST
*/
typedef uint32_t Audio_TSType;      /**< @brief Audio timestamp type */
/** @} */

/**
    @name Audio_ChanNumType
    @{
    @brief Audio channel number type

    This type is used to specify number of audio data channels being played or
    recorded on audio controller.

    @trace #BRCM_SWREQ_AUDIO_CHAN_NUM
*/
typedef uint32_t Audio_ChanNumType; /**< @brief Audio channel number type */
/** @} */

/**
    @name Audio_MClkSrcType
    @{
    @brief Audio master clock source

    This type is used to specify the master source clock for the audio
    controller.

    @trace #BRCM_SWREQ_AUDIO_MCLK_SRC
*/
typedef uint32_t Audio_MClkSrcType;     /**< @brief Audio master clock source
                                             type */
#define AUDIO_MCLK_SRC_NCO_0    (1UL)   /**< @brief Master clock source is NCO
                                             0 */
#define AUDIO_MCLK_SRC_NCO_1    (2UL)   /**< @brief Master clock source is NCO
                                             1 */
/** @} */

/**
    @name Audio_InfModeType
    @{
    @brief Audio interface mode

    This type is used to specify the mode of the interface between audio
    controller and audio codec chip.

    @note At present, audio driver is tested in only
    #AUDIO_INF_MODE_BCLK_WS_MASTER mode.

    @trace #BRCM_SWREQ_AUDIO_INF_MODE
*/
typedef uint32_t Audio_InfModeType;             /**< @brief Audio interface mode
                                                     type */
#define AUDIO_INF_MODE_BCLK_WS_MASTER   (1UL)   /**< @brief Bit clock: Master;
                                                     Word select: Master */
#define AUDIO_INF_MODE_BCLK_WS_SLAVE    (2UL)   /**< @brief Bit clock: Slave;
                                                     Word select: Slave */
/** @} */

/**
    @name Audio_InfDataDlyType
    @{
    @brief Audio interface data delay

    @trace #BRCM_SWREQ_AUDIO_INT_DATA_DELAY
*/
typedef uint32_t Audio_InfDataDlyType;              /**< @brief Audio interface
                                                         data delay type */
#define AUDIO_INF_DATA_DLY_TRUE             (1UL)   /**< @brief Data is delayed
                                                         w.r.t to word select by
                                                         1 bit */
#define AUDIO_INF_DATA_DLY_FALSE            (2UL)   /**< @brief Data is not
                                                         delayed w.r.t to word
                                                         select by 1 bit */
/** @} */

/**
    @name Audio_InfDataJustType
    @{
    @brief Audio interface data justification

    @trace #BRCM_SWREQ_AUDIO_INF_DATA_JUST
*/
typedef uint32_t Audio_InfDataJustType;     /**< @brief Audio interface data
                                                 justification type */
#define AUDIO_INF_DATA_JUST_RIGHT   (1UL)   /**< @brief Data right justified */
#define AUDIO_INF_DATA_JUST_LEFT    (2UL)   /**< @brief Data left justified */
/** @} */

/**
    @name Audio_SlotMaskType
    @{
    @brief Audio slot mask

    This type is to specify the slots to be enabled for audio data play or
    record on the interface between audio controller and audio codec chip. The
    value of this type shall be in the range 0 - 0xFFFUL, where setting 0th bit
    enables the 0th slot, 1st bit enables the 1st slot and so on. At max 16
    slots can be enabled in the interface (hence the maximum value 0xFFF).

    @trace #BRCM_SWREQ_AUDIO_SLOT_MASK
*/
typedef uint32_t Audio_SlotMaskType;    /**< @brief Audio slot mask type */
/** @} */

/**
    @name Audio_WidthType
    @{
    @brief Audio width type

    This type is to specify the sample width, slot width and word select width
    in terms of number of bits.

    @trace #BRCM_SWREQ_AUDIO_WIDTH
*/
typedef uint32_t Audio_WidthType;   /**< @brief Audio width type */
/** @} */

/**
    @name Audio_PackType
    @{
    @brief Audio sample packing

    This type is used to specify the packing information of samples in data
    buffer.

    @trace #BRCM_SWREQ_AUDIO_PACK
*/
typedef uint32_t Audio_PackType;            /**< @brief Audio sample packing
                                                 type */
#define AUDIO_PACK_4B_MSB_ALIGN_LE  (1UL)   /**< @brief Packing information:
                                                 - Sample pack is 4bytes.
                                                 - Aligned to MSB for sample
                                                   width = 16bit/20bit/24bit.
                                                   LSB shall be filled with zero
                                                 - Samples shall be placed in
                                                   little endian order in
                                                   memory */
/** @} */

/**
    @name Audio_PlayStateType
    @{
    @brief Audio play state

    This type is to specify the play state of audio controller channel.

    @trace #BRCM_SWREQ_AUDIO_PLAY_STATE
*/
typedef uint32_t Audio_PlayStateType;           /**< @brief Audio play state
                                                     type */
#define AUDIO_PLAY_STATE_RESET          (1UL)   /**< @brief Audio play state:
                                                     reset */
#define AUDIO_PLAY_STATE_CONFIGURED     (2UL)   /**< @brief Audio play state:
                                                     configured */
#define AUDIO_PLAY_STATE_STOPPED        (3UL)   /**< @brief Audio play state:
                                                     stopped */
#define AUDIO_PLAY_STATE_BUFFERING      (4UL)   /**< @brief Audio play state:
                                                     buffering */
#define AUDIO_PLAY_STATE_RUNNING        (5UL)   /**< @brief Audio play state:
                                                     running */
#define AUDIO_PLAY_STATE_UNDERRUN       (6UL)   /**< @brief Audio play state:
                                                     underrun */
#define AUDIO_PLAY_STATE_STOP_IN_PROG   (7UL)   /**< @brief Audio play state:
                                                     stop in progress */
#define AUDIO_PLAY_STATE_ERROR          (8UL)   /**< @brief Audio play state:
                                                     error */
/** @} */

/**
    @name Audio_RecStateType
    @{
    @brief Audio record state

    This type is to specify the record state of audio controller channel.

    @trace #BRCM_SWREQ_AUDIO_REC_STATE
*/
typedef uint32_t Audio_RecStateType;            /**< @brief Audio record state
                                                     type */
#define AUDIO_REC_STATE_RESET           (1UL)   /**< @brief Audio record state reset */
#define AUDIO_REC_STATE_CONFIGURED      (2UL)   /**< @brief Audio record state configured */
#define AUDIO_REC_STATE_STOPPED         (3UL)   /**< @brief Audio record state stopped */
#define AUDIO_REC_STATE_BUFFERING       (4UL)   /**< @brief Audio record state buffering */
#define AUDIO_REC_STATE_RUNNING         (5UL)   /**< @brief Audio record state running */
#define AUDIO_REC_STATE_OVERRUN         (6UL)   /**< @brief Audio record state overrun */
#define AUDIO_REC_STATE_STOP_IN_PROG    (7UL)   /**< @brief Audio record state stop in progress */
#define AUDIO_REC_STATE_ERROR           (8UL)   /**< @brief Audio record state error */
/** @} */

/**
    @brief Audio play statistics

    @trace #BRCM_SWREQ_AUDIO_PLAY_STATS
*/
typedef struct {
    uint32_t bufPlayed;     /**< @brief Count of buffer played by the
                                 controller */
    uint32_t underFlowCnt;  /**< @brief Count of buffer under-flows */
} Audio_PlayStatsType;

/**
    @brief Audio record statistics

    @trace #BRCM_SWREQ_AUDIO_REC_STATS
*/
typedef struct {
    uint32_t bufRec;        /**< @brief Count of buffer recorded by the
                                 controller */
    uint32_t overFlowCnt;   /**< @brief Count of buffer over-flows */
} Audio_RecStatsType;

/**
    @brief Audio buffer information

    @trace #BRCM_SWREQ_AUDIO_BUF_INFO
*/
typedef struct {
    uint8_t *buf;       /**< @brief Pointer to the data buffer */
    Audio_TSType TS;    /**< @brief Timestamp associated with recorded buffer
                             data */
} Audio_BufInfoType;

/**
    @brief Audio configuration

    @trace #BRCM_SWREQ_AUDIO_CFG
*/
typedef struct {
    Audio_HwIDType hwID;                /**< @brief Hardware ID */
    Audio_ClkFreqType masterClk;        /**< @brief Master clock value */
    Audio_MClkSrcType masterClkSrc;     /**< @brief Master clock source */
    Audio_FreqType freq;                /**< @brief Sample frequency */
    Audio_InfModeType infMode;          /**< @brief Interface mode */
    Audio_ChanNumType numSlot;          /**< @brief Number of slot in audio
                                             frame. This is the number of slots
                                             on interface between audio
                                             controller and audio codec/mic
                                             chip. */
    Audio_WidthType slotWidth;          /**< @brief Slot width in terms of bits.
                                             This is the width of each slot on
                                             the interface between audio
                                             controller and audio codec/mic
                                             chip. */
    Audio_WidthType wordSelWidth;       /**< @brief Word select length in terms
                                             of bits. This indicates the
                                             duration (in bit cycles) for which
                                             word select lines shall be kept
                                             asserted on interface between audio
                                             controller and audio codec/mic
                                             chip. */
    Audio_InfDataDlyType txDataDly;     /**< @brief Data delayed by 1 bit w.r.t
                                             word select */
    Audio_SlotMaskType txSlotMask;      /**< @brief TX slot mask */
    Audio_InfDataJustType txDataJust;   /**< @brief TX data justification */
    Audio_InfDataDlyType rxDataDly;     /**< @brief Data delayed by 1 bit w.r.t
                                             word select */
    Audio_SlotMaskType rxSlotMask;      /**< @brief RX slot mask */
    Audio_InfDataJustType rxDataJust;   /**< @brief RX data justification */
    uint8_t *const playBuf;             /**< @brief Pointer to play buffer */
    uint32_t playBufSize;               /**< @brief Play buffer size in bytes */
    uint32_t playBufNum;                /**< @brief Number of play buffers */
    DMA_XferReqType *const playXferReq; /**< @brief Play DMA transfer
                                             descriptors memory */
    uint8_t *const recBuf;              /**< @brief Pointer to record buffer */
    uint32_t recBufSize;                /**< @brief Record buffer size in
                                             bytes */
    uint32_t recBufNum;                 /**< @brief Number of record buffers */
    DMA_XferReqType *const recXferReq;  /**< @brief Record DMA transfer
                                             descriptors memory */
} Audio_CfgType;

/**
    @brief Audio play configuration

    @trace #BRCM_SWREQ_AUDIO_PLAY_CFG
*/
typedef struct {
    Audio_WidthType sampleWidth;    /**< @brief Sample width */
    Audio_PackType samplePack;      /**< @brief Sample packing */
    Audio_ChanNumType numChan;      /**< @brief Number of audio channels */
    Audio_LenType startThreshold;   /**< @brief Play start threshold in bytes.
                                         Play back will starts when data queued
                                         with driver is greater than or equal to
                                         play @c startThreshold. */
} Audio_PlayCfgType;

/**
    @brief Audio record configuration

    @trace #BRCM_SWREQ_AUDIO_REC_CFG
*/
typedef struct {
    Audio_WidthType sampleWidth;    /**< @brief Sample width */
    Audio_PackType samplePack;      /**< @brief Sample packing */
    Audio_ChanNumType numChan;      /**< @brief Number of audio channels */
    Audio_LenType startThreshold;   /**< @brief Record start threshold in bytes.
                                         Record starts when empty buffer
                                         available with driver is greater than
                                         or equal to @c startThreshold */
} Audio_RecCfgType;

/**
    @brief Audio information

    This type is to retrieve information from driver.

    @trace #BRCM_SWREQ_AUDIO_INFO
*/
typedef struct {
    Audio_FreqType freq;    /**< @brief Sample frequency */
    uint32_t bufSize;       /**< @brief Buffer size in bytes */
    uint32_t bufNum;        /**< @brief Number of buffers */
} Audio_InfoType;

/** @brief Initialize audio driver

    API to initialize audio driver.

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aConfig     Pointer to configuration

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_INIT

    @limitations Audio driver configuration should be defined as constant static
    data and this API preserves @a aConfig pointer for future use.

    @limitations Also, it is required to configure the audio controller clock
    source to correct frequency as mentioned in Audio_CfgType::masterClk before
    initializing audio driver.
*/
extern void Audio_Init(Audio_HwIDType aID,
                       const Audio_CfgType *const aConfig);

/** @brief De-initialize audio driver

    API to de-initialize audio driver.

    @behavior Sync, Re-entrant for different @a aID

    @pre This API shall be called only when play and record are not in
    running state.

    @param[in]      aID         Hardware ID

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_DEINIT

    @limitations None
*/
extern void Audio_DeInit(Audio_HwIDType aID);

/************************* Audio Play APIs **************************/

/** @brief Configure audio playback

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aConfig     Pointer to play configurations

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     Others                  As returned from #DMA_PeriphXferSetup
    @retval     #BCM_ERR_UNKNOWN        Unable to obtain DMA channel for
                                        for audio playback (Refer to
                                        #SysCfg_GetDMAChannel)
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - aConfig->numChan < 2UL
                                        - aConfig->samplePack is not
                                          #AUDIO_PACK_4B_MSB_ALIGN_LE
                                        - aConfig is NULL
                                        - Invalid aID

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_PLAY_CFG

    @limitations None
*/
extern int32_t Audio_PlayCfg(Audio_HwIDType aID,
                        Audio_PlayCfgType *const aConfig);

/** @brief Start audio playback

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play start successful
    @retval     Others                  As returned from #DMA_PeriphXferTrigger
    @retval     Others                  As returned from #DMA_PeriphFlush
    @retval     #BCM_ERR_INVAL_STATE    API called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @a aID

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_PLAY_START

    @limitations None
*/
extern int32_t Audio_PlayStart(Audio_HwIDType aID);

/** @brief Stop audio playback

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play stop successful
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @a aID

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_PLAY_STOP

    @limitations None
*/
extern int32_t Audio_PlayStop(Audio_HwIDType aID);

/** @brief Retrieve play state

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aState      Pointer to retrieve play state (valid only if
                                this API returns #BCM_ERR_OK)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully retrieved
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters:
                                        - @a aState is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_PLAY_STATE

    @limitations None
*/
extern int32_t Audio_GetPlayState(Audio_HwIDType aID,
                           Audio_PlayStateType *const aState);

/** @brief Get play buffer

    API to get play buffer. Client shall fill the audio samples to the buffer
    pointed by @a aBuf and queue the buffer back to driver for playing using
    #Audio_QueuePlayBuf API.

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aLen        Buffer length in bytes
    @param[out]     aBuf        Pointer to dequeue empty play buffer pointer

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play buffer dequeue successful
    @retval     #BCM_ERR_EAGAIN         No empty buffer to dequeue
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_INVAL_PARAMS   @a aLen size is more than maximum
                                        allowed size
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aBuf is NULL
                                        - @a aLen is zero
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_DEQUEUE_PLAY_BUF

    @limitations None
*/
extern int32_t Audio_DeQueuePlayBuf(Audio_HwIDType aID,
                             Audio_LenType aLen,
                             uint8_t **const aBuf);

/** @brief Queue filled play buffer

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aLen        Buffer length in bytes
    @param[in]      aBuf        Pointer to filled play buffer. This pointer is
                                previously dequeued from the driver using
                                #Audio_DeQueuePlayBuf API


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play buffer queue successful
    @retval     Others                  As returned from #DMA_PeriphXferTrigger
    @retval     Others                  As returned from #DMA_PeriphFlush
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_INVAL_PARAMS   aBuf is invalid
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aBuf is NULL
                                        - @a aLen is zero
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_QUEUE_PLAY_BUF

    @limitations None
*/
extern int32_t Audio_QueuePlayBuf(Audio_HwIDType aID,
                           Audio_LenType aLen,
                           uint8_t *const aBuf);

/** @brief Retrieve play information

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aInfo       Pointer to play information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play information retrieval successful
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aInfo is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_PLAY_INFO

    @limitations None
*/
extern int32_t Audio_GetPlayInfo(Audio_HwIDType aID,
                           Audio_InfoType *const aInfo);

/** @brief Retrieve play statistics

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aStats      Pointer to retrieve play statistics

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aStats is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_PLAY_STATS

    @limitations None
*/
extern int32_t Audio_GetPlayStats(Audio_HwIDType aID,
                           Audio_PlayStatsType *const aStats);

/*********************** Audio Record APIs ***************************/

/** @brief Configure audio record

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aConfig     Pointer to record configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Play buffer queue successful
    @retval     Others                  As returned from from
                                        #DMA_PeriphXferSetup
    @retval     #BCM_ERR_UNKNOWN        Unable to obtain DMA channel for audio
                                        record (refer to #SysCfg_GetDMAChannel)
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters:
                                        - Invalid configuration
                                        - @a aConfig is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_REC_CFG

    @limitations None
*/
extern int32_t Audio_RecCfg(Audio_HwIDType aID,
                        Audio_RecCfgType *const aConfig);

/** @brief Start audio record

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record start successful
    @retval     Others                  As returned from #DMA_PeriphXferTrigger
    @retval     Others                  As returned from #DMA_PeriphFlush
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @a aID

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_REC_START

    @limitations None
*/
extern int32_t Audio_RecStart(Audio_HwIDType aID);

/** @brief Stop audio record

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record stop successful
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid @a aID

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_REC_STOP

    @limitations None
*/
extern int32_t Audio_RecStop(Audio_HwIDType aID);

/** @brief Retrieve record state

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aState      Pointer to record state

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record state retrieved successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aState is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_REC_STATE

    @limitations None
*/
extern int32_t Audio_GetRecState(Audio_HwIDType aID,
                          Audio_RecStateType *const aState);

/** @brief Dequeue recorded buffer

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aLen        Buffer length in bytes
    @param[out]     aBufInfo    Pointer to dequeue recorded buffer information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record buffer dequeue successful
    @retval     Others                  As returned from  #DMA_PeriphXferTrigger
    @retval     Others                  As returned from  #DMA_PeriphFlush
    @retval     #BCM_ERR_EAGAIN         No empty buffer to dequeue
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_INVAL_PARAMS   @a aLen size is more than maximum
                                        allowed size
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aBufInfo is NULL
                                        - @a aLen is zero
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_DEQUEUE_REC_BUF

    @limitations None
*/
extern int32_t Audio_DeQueueRecBuf(Audio_HwIDType aID,
                         Audio_LenType aLen,
                         Audio_BufInfoType *const aBufInfo);

/** @brief Empty recorded buffer

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[in]      aLen        Buffer length in bytes
    @param[out]     aBuf        Pointer to empty record buffer. This pointer is
                                previously dequeued from the driver using
                                #Audio_DeQueueRecBuf.

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record buffer queue successful
    @retval     #BCM_ERR_INVAL_STATE    Interface called in wrong state
    @retval     #BCM_ERR_INVAL_PARAMS   aBuf is invalid
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aBuf is NULL
                                        - @a aLen is zero
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_QUEUE_REC_BUF

    @limitations None
*/
extern int32_t Audio_QueueRecBuf(Audio_HwIDType aID,
                         Audio_LenType aLen,
                         uint8_t *const aBuf);

/** @brief Retrieve record information

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aInfo       Pointer to record information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record information retrieval successful
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aInfo is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_REC_INFO

    @limitations None
*/
extern int32_t Audio_GetRecInfo(Audio_HwIDType aID,
                           Audio_InfoType *const aInfo);

/** @brief Retrieve record statistics

    @behavior Sync, Re-entrant for different @a aID

    @pre TBD

    @param[in]      aID         Hardware ID
    @param[out]     aStats      Pointer to record statistcs

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Record statistics retrieval successful
    @retval     #BCM_ERR_UNINIT         Audio driver is not initialized
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameters
                                        - @a aStats is NULL
                                        - @a aID is invalid

    @post TBD

    @trace #BRCM_SWREQ_AUDIO_GET_REC_STATS

    @limitations None
*/
extern int32_t Audio_GetRecStats(Audio_HwIDType aID,
                           Audio_RecStatsType *const aStats);

#endif /* AUDIO_H */
/** @} */
