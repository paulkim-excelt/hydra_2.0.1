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

/** @defgroup grp_audio_il Integration Guide
    @ingroup grp_audio

    @addtogroup grp_audio_il
    @{
    @section sec_audio_integration Integration Guide
    This section serves as the integration guide to integrate this driver into
    the system. This section describes Audio driver integration layer more
    detail.

    Audio driver is implemented with #SysCmdReq (system command request) and
    #SysCmdHandler (system command handler) model. This section also gives the
    necessary information on how to install command handler for Audio driver
    onto #SysCmdHandler (system command handler).

    @section sec_audio_il_seq Sequence Diagram
    @subsection subsec_audio_il_seq_svc Driver SVC Layer
    The following sequence diagram depicts the sequence for #Audio_Init as an
    example. All other Audio APIs shall follow the same sequence.
    @image html audio_seq_il_diagram_svc.jpg "Driver SVC layer sequence diagram"

    @subsection subsec_audio_il_seq_play Audio Play Integration
    The following sequence diagram depicts the sequence audio play integration
    sequence in running state as an example for audio driver – DMA interaction.
    Audio record shall also follow similar sequence.
    @image html audio_seq_il_diagram_play.jpg "Audio play integration sequence"

    @file audio_osil.h
    @brief Audio driver Integration Interfaces
    This header file contains the integration interfaces for Audio driver.
    @version 0.20 Imported from docx
*/

#ifndef AUDIO_OSIL_H
#define AUDIO_OSIL_H

#include <inttypes.h>
#include <system.h>
#include <audio.h>
#include <dma.h>

/**
    @name Audio driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for Audio driver.
*/
#define BRCM_SWARCH_AUDIO_IL_PLAY_COMP_EVENT_MACRO      (0x40U) /**< @brief #APLY_0_COMP_EVENT */
#define BRCM_SWARCH_AUDIO_IL_REC_COMP_EVENT_MACRO       (0x41U) /**< @brief #AREC_0_COMP_EVENT */
#define BRCM_SWARCH_AUDIO_IL_PLAY_BUF_INFO_MACRO        (0x42U) /**< @brief #AUDIO_PLAY0_BUF_SIZE */
#define BRCM_SWARCH_AUDIO_IL_REC_BUF_INFO_MACRO         (0x43U) /**< @brief #AUDIO_REC0_BUF_SIZE */
#define BRCM_SWARCH_AUDIO_IL_CMD_TYPE                   (0x44U) /**< @brief #Audio_CmdType */
#define BRCM_SWARCH_AUDIO_IL_IO_TYPE                    (0x45U) /**< @brief #AudioIO */
#define BRCM_SWARCH_AUDIO_IL_PLAY0_BUF_GLOBAL           (0x46U) /**< @brief #Audio_Play0Buf */
#define BRCM_SWARCH_AUDIO_IL_PLAY0_DMA_XFER_GLOBAL      (0x47U) /**< @brief #Audio_Play0DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_REC0_BUF_GLOBAL            (0x48U) /**< @brief #Audio_Rec0Buf */
#define BRCM_SWARCH_AUDIO_IL_REC0_DMA_XFER_GLOBAL       (0x49U) /**< @brief #Audio_Rec0DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_PLAY1_BUF_GLOBAL           (0x4AU) /**< @brief #Audio_Play1Buf */
#define BRCM_SWARCH_AUDIO_IL_PLAY1_DMA_XFER_GLOBAL      (0x4BU) /**< @brief #Audio_Play1DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_REC1_BUF_GLOBAL            (0x4CU) /**< @brief #Audio_Rec1Buf */
#define BRCM_SWARCH_AUDIO_IL_REC1_DMA_XFER_GLOBAL       (0x4DU) /**< @brief #Audio_Rec1DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_PLAY2_BUF_GLOBAL           (0x4EU) /**< @brief #Audio_Play2Buf */
#define BRCM_SWARCH_AUDIO_IL_PLAY2_DMA_XFER_GLOBAL      (0x4FU) /**< @brief #Audio_Play2DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_REC2_BUF_GLOBAL            (0x50U) /**< @brief #Audio_Rec2Buf */
#define BRCM_SWARCH_AUDIO_IL_REC2_DMA_XFER_GLOBAL       (0x51U) /**< @brief #Audio_Rec2DMAXfer */
#define BRCM_SWARCH_AUDIO_IL_I2S0_DMA_TX_DONE_CB_PROC   (0x52U) /**< @brief #I2S0_DmaTxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_I2S0_DMA_RX_DONE_CB_PROC   (0x53U) /**< @brief #I2S0_DmaRxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_I2S1_DMA_TX_DONE_CB_PROC   (0x54U) /**< @brief #I2S1_DmaTxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_I2S1_DMA_RX_DONE_CB_PROC   (0x55U) /**< @brief #I2S1_DmaRxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_I2S2_DMA_TX_DONE_CB_PROC   (0x56U) /**< @brief #I2S2_DmaTxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_I2S2_DMA_RX_DONE_CB_PROC   (0x57U) /**< @brief #I2S2_DmaRxDoneCb */
#define BRCM_SWARCH_AUDIO_IL_PLAY_DMA_XFER_DONE_CB_PROC (0x58U) /**< @brief #Audio_PlayDMAXferDoneCb */
#define BRCM_SWARCH_AUDIO_IL_REC_DMA_XFER_DONE_CB_PROC  (0x59U) /**< @brief #Audio_RecDMAXferDoneCb */
#define BRCM_SWARCH_AUDIO_IL_CMD_HANDLER_PROC           (0x5AU) /**< @brief #Audio_CmdHandler */
#define BRCM_SWARCH_AUDIO_IL_SYS_CMD_HANDLER_PROC       (0x5BU) /**< @brief #Audio_SysCmdHandler */
#define BRCM_SWARCH_AUDIO_IL_SYS_CMD_REQ_PROC           (0x5CU) /**< @brief #Audio_SysCmdReq */
/** @} */

/**
    @name Audio Play Completion Events
    @{
    @brief Events denoting play compeletion

    Audio driver shall set these events on completion of play of each audio
    driver data buffer.

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY_COMP_EVENT
*/
#define APLY_0_COMP_EVENT           (SystemEvent1)  /**< @brief Audio play 0
                                                         data played out (DMA
                                                         completion) event  */
#define APLY_1_COMP_EVENT           (SystemEvent2)  /**< @brief Audio play 1
                                                         data played out (DMA
                                                         completion) event  */
#define APLY_2_COMP_EVENT           (SystemEvent3)  /**< @brief Audio play 2
                                                         data played out (DMA
                                                         completion) event  */
/** @} */

/**
    @name Audio Record Completion Events
    @{
    @brief Events denoting record compeletion

    Audio driver shall set these events on completion of record of each audio
    driver data buffer.

    @trace #BRCM_SWREQ_AUDIO_IL_REC_COMP_EVENT
*/
#define AREC_0_COMP_EVENT           (SystemEvent1)  /**< @brief Audio record 0
                                                         data record (DMA
                                                         completion) event */
#define AREC_1_COMP_EVENT           (SystemEvent2)  /**< @brief Audio record 1
                                                         data record (DMA
                                                         completion) event */
#define AREC_2_COMP_EVENT           (SystemEvent3)  /**< @brief Audio record 2
                                                         data record (DMA
                                                         completion) event */
/** @} */

/**
    @name Audio Play Buffer Information
    @{
    @brief Audio play buffer information

    Audio driver play buffer information is used to statically allocate driver
    buffer in the integration layer and passed to the driver using
    #Audio_Init.

    @note Audio play buffer size must be aligned to 32bytes to meet DMA burst
    requirements.

    @note The values mentioned for macros are example and can be changed by
    integrator as per system requirement. Integrator may choose to allocate
    buffers only for the play channel in use.

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY_BUF_INFO
*/
#define AUDIO_PLAY0_BUF_SIZE        (4320UL)    /**< @brief Audio play 0 buffer
                                                     size  */
#define AUDIO_PLAY0_BUF_MAX_NUM     (8UL)       /**< @brief Audio play 0 buffer
                                                     number */
#define AUDIO_PLAY1_BUF_SIZE        (4320UL)    /**< @brief Audio play 1 buffer
                                                     size */
#define AUDIO_PLAY1_BUF_MAX_NUM     (8UL)       /**< @brief Audio play 1 buffer
                                                     number */
#define AUDIO_PLAY2_BUF_SIZE        (4320UL)    /**< @brief Audio play 2 buffer
                                                     size  */
#define AUDIO_PLAY2_BUF_MAX_NUM     (8UL)       /**< @brief Audio play 2 buffer
                                                     number */
/** @} */

/**
    @name Audio Record Buffer Information
    @{
    @brief Audio record buffer information

    Audio driver record buffer information is used to statically allocate driver
    buffer in the integration layer and passed to the driver using #Audio_Init.

    @note Audio record buffer size must be aligned to 32bytes to meet DMA burst
    requirements .

    @note The values mentioned for macros are example and can be changed by
    integrator as per system requirement. Integrator may choose to allocate
    buffers only for the record channels in use.

    @trace #BRCM_SWREQ_AUDIO_IL_REC_BUF_INFO
*/
#define AUDIO_REC0_BUF_SIZE         (4320UL)    /**< @brief Audio record 0
                                                     buffer size */
#define AUDIO_REC0_BUF_MAX_NUM      (8UL)       /**< @brief Audio record 0
                                                     buffer number */
#define AUDIO_REC1_BUF_SIZE         (4320UL)    /**< @brief Audio record 1
                                                     buffer size  */
#define AUDIO_REC1_BUF_MAX_NUM      (8UL)       /**< @brief Audio record 1
                                                     buffer number */
#define AUDIO_REC2_BUF_SIZE         (4320UL)    /**< @brief Audio record 2
                                                     buffer size */
#define AUDIO_REC2_BUF_MAX_NUM      (8UL)       /**< @brief Audio record 2
                                                     buffer number */
/** @} */

/**
    @name Audio_CmdType
    @{
    @brief Audio driver system command request

    @trace #BRCM_SWREQ_AUDIO_IL_CMD
*/
typedef uint32_t Audio_CmdType; /**< @brief Audio driver system command request
                                     type*/
/** @} */

/**
    @brief Audio IO structure

    Audio IO structure to be used for #Audio_CmdHandler.

    @trace #BRCM_SWREQ_AUDIO_IL_IO
*/
typedef struct _AudioIO {
    int32_t retVal;               /**< @brief Return value */
    const Audio_CfgType *config;  /**< @brief Pointer to Audio driver configuration */
    Audio_HwIDType hwID;          /**< @brief Hardware channel ID */
    Audio_PlayCfgType *playCfg;   /**< @brief Pointer to Audio play config */
    Audio_RecCfgType *recCfg;     /**< @brief Pointer to Audio record config */
    Audio_LenType lenIn;          /**< @brief Audio buffer length input */
    Audio_LenType *lenOut;        /**< @brief Pointer to audio buffer length out */
    uint8_t *bufIn;               /**< @brief Pointer to input audio buffer */
    uint8_t **bufOut;             /**< @brief Pointer to output audio buffer */
} AudioIO;

#if defined(ENABLE_I2S0_PLAY)
/**
    @brief Audio driver play 0 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    playback purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY0_BUF_GLOBAL
*/
extern uint8_t Audio_Play0Buf[AUDIO_PLAY0_BUF_MAX_NUM][AUDIO_PLAY0_BUF_SIZE];

/**
    @brief Audio driver play 0 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY0_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Play0DMAXfer[AUDIO_PLAY0_BUF_MAX_NUM];
#endif  /* ENABLE_I2S0_PLAY */

#if defined(ENABLE_I2S0_REC)
/**
    @brief Audio driver record 0 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    record purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_REC0_BUF_GLOBAL
*/
extern uint8_t Audio_Rec0Buf[AUDIO_REC0_BUF_MAX_NUM][AUDIO_REC0_BUF_SIZE];

/**
    @brief Audio driver record 0 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_REC0_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Rec0DMAXfer[AUDIO_REC0_BUF_MAX_NUM];
#endif  /* ENABLE_I2S0_REC */

#if defined(ENABLE_I2S1_PLAY)
/**
    @brief Audio driver play 1 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    playback purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY1_BUF_GLOBAL
*/
extern uint8_t Audio_Play1Buf[AUDIO_PLAY1_BUF_MAX_NUM][AUDIO_PLAY1_BUF_SIZE];

/**
    @brief Audio driver play 1 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY1_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Play1DMAXfer[AUDIO_PLAY1_BUF_MAX_NUM];
#endif  /* ENABLE_I2S1_PLAY */

#if defined(ENABLE_I2S1_REC)
/**
    @brief Audio driver record 1 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    record purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_REC1_BUF_GLOBAL
*/
extern uint8_t Audio_Rec1Buf[AUDIO_REC1_BUF_MAX_NUM][AUDIO_REC1_BUF_SIZE];

/**
    @brief Audio driver record 1 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_REC1_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Rec1DMAXfer[AUDIO_REC1_BUF_MAX_NUM];
#endif  /* ENABLE_I2S1_REC */

#if defined(ENABLE_I2S2_PLAY)
/**
    @brief Audio driver play 2 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    playback purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY2_BUF_GLOBAL
*/
extern uint8_t Audio_Play2Buf[AUDIO_PLAY2_BUF_MAX_NUM][AUDIO_PLAY2_BUF_SIZE];

/**
    @brief Audio driver play 2 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_PLAY2_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Play2DMAXfer[AUDIO_PLAY2_BUF_MAX_NUM];
#endif  /* ENABLE_I2S2_PLAY */

#if defined(ENABLE_I2S2_REC)
/**
    @brief Audio driver record 2 data buffer

    This buffer shall be contiguous memory buffer allocated in non-cached DMA
    accessible memory region. This buffer shall be used by driver for audio
    record purpose.

    @trace #BRCM_SWREQ_AUDIO_IL_REC2_BUF_GLOBAL
*/
extern uint8_t Audio_Rec2Buf[AUDIO_REC2_BUF_MAX_NUM][AUDIO_REC2_BUF_SIZE];

/**
    @brief Audio driver record 2 DMA transfer descriptors

    @trace #BRCM_SWREQ_AUDIO_IL_REC2_DMA_XFER_GLOBAL
*/
extern DMA_XferReqType Audio_Rec2DMAXfer[AUDIO_REC2_BUF_MAX_NUM];
#endif  /* ENABLE_I2S2_REC */

/** @brief I2S0 DMA transmit done callback

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator.
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S0_DMA_TX_DONE_CB

    @limitations None
*/
extern void I2S0_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief I2S0 DMA receive done callback.

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S0_DMA_RX_DONE_CB

    @limitations None
 */
extern void I2S0_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief I2S1 DMA transmit done callback

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator.
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S1_DMA_TX_DONE_CB

    @limitations None
*/
extern void I2S1_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief I2S1 DMA receive done callback

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S1_DMA_RX_DONE_CB

    @limitations None
 */
extern void I2S1_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief I2S2 DMA transmit done callback

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator.
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S2_DMA_TX_DONE_CB

    @limitations None
*/
extern void I2S2_DmaTxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief I2S2 DMA receive done callback

    This callback function is invoked by DMA driver in interrupt context after
    completion of DMA request.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This callback shall be implemented by the integrator
    @note This callback function shall be updated as part of DMA channel
    configurations by the integrator.

    @trace  #BRCM_SWREQ_AUDIO_IL_I2S2_DMA_RX_DONE_CB

    @limitations None
 */
extern void I2S2_DmaRxDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                             DMA_XferStatusType aStatus);

/** @brief Audio play DMA transfer done callback

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This interface shall be called from #I2S0_DmaTxDoneCb,
    #I2S1_DmaTxDoneCb and #I2S2_DmaTxDoneCb for audio driver processing on each
    transmit DMA completion.

    @trace  #BRCM_SWREQ_AUDIO_IL_PLAY_DMA_XFER_DONE_CB

    @limitations None
 */
extern void Audio_PlayDMAXferDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                                    DMA_XferStatusType aStatus);

/** @brief Audio record DMA transfer done callback

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aID         DMA channel ID
    @param[in]      aReqIdx     Index of the request processed
    @param[in]      aStatus     Transfer status

    @return     void

    @post TBD

    @note This interface shall be called from #I2S0_DmaRxDoneCb,
    #I2S1_DmaRxDoneCb and #I2S2_DmaRxDoneCb for audio driver processing on each
    receive DMA completion.

    @trace  #BRCM_SWREQ_AUDIO_IL_REC_DMA_XFER_DONE_CB

    @limitations None
 */
extern void Audio_RecDMAXferDoneCb(DMA_ChannelIDType aID, uint32_t aReqIdx,
                                    DMA_XferStatusType aStatus);

/** @brief Internal command handler

    This interface handles commands requested from system command handler.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aCmd        Command requested by user
    @param[inout]   aIO         In-Out structure containing request parameters

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command request completed successfully
    @retval     Others                  API dependent error values
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid command
    @retval     #BCM_ERR_UNKNOWN        @a aIO is NULL
    @return     void

    @post TBD

    @note This interface shall be called from #Audio_SysCmdHandler

    @trace  #BRCM_SWREQ_AUDIO_IL_CMD_HANDLER

    @limitations None
 */
extern int32_t Audio_CmdHandler(Audio_CmdType aCmd,
                                AudioIO *const aIO);

/** @brief Command handler interface

    This command handler interface shall handle all the commands requested from
    the system command handler.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aMagicID    Audio driver SVC Magic ID
    @param[in]      aCmd        Command requested from SysCmdHandler
    @param[inout]   aSysIO      #uint8_t * to be iterpreted as #AudioIO

    @return     void

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to the SVC Layer.

    @trace  #BRCM_SWREQ_AUDIO_IL_SYS_CMD_HANDLER

    @limitations None
 */
extern void Audio_SysCmdHandler(uint32_t aMagicID,
                                uint32_t aCmd,
                                uint8_t * aSysIO);

/** @brief Interface to raise a request to Audio driver

    @behavior Sync, Re-entrant

    @pre TBD

    @param[in]      aCmd        Audio system request command
    @param[inout]   aIO         Pointer to AudioIO

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Command raised successfully
    @retval     Others                  As returned from #Audio_CmdHandler
    @retval     #BCM_ERR_INVAL_MAGIC    Invalid SVC magic
    @retval     #BCM_ERR_UNKNOWN        Integration error

    @post TBD

    @note This interface shall be implemented by the integrator and shall
    install it to the SVC Layer.

    @trace  #BRCM_SWREQ_AUDIO_IL_SYS_CMD_REQ

    @limitations None
 */
extern int32_t Audio_SysCmdReq(Audio_CmdType aCmd,
                               AudioIO * const aIO);

#endif /* AUDIO_OSIL_H */
/** @} */
