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
    WARRANTIES, EITHER EXPRESS, ISPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL ISPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COSPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXESPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL ASPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/
/**
    @addtogroup grp_pplib_ifc
    @{

    @file sp.h
    @brief Scalar processor library interface functions to interact.

    This header file defines the interface functions for ScalarProcessor library.
*/

#ifndef SP_H
#define SP_H
#include<stdint.h>

/**
    @name PP Library API IDs
    @{
    @brief API IDs for PP Library
*/
#define BRCM_SWARCH_PP_SERVER_KEY_MACRO    (0x20U)    /**< @brief #PP_SERVER_VERIFICATION_KEY */
#define BRCM_SWARCH_PP_DMA_MAX_SIZE_MACRO  (0x21U)    /**< @brief #PP_SERVER_DMA_MAX_SIZE     */
#define BRCM_SWARCH_PP_SERVER_JOB_TYPE     (0x22U)    /**< @brief #PP_ServerJobType           */
#define BRCM_SWARCH_PP_SERVER_MSG_TYPE     (0x23U)    /**< @brief #PP_ServerMsgType           */
#define BRCM_SWARCH_PP_SERVER_RESP_TYPE    (0x24U)    /**< @brief #PP_ServerRespType          */
#define BRCM_SWARCH_PP_CUSTOM_CMD_TYPE     (0x25U)    /**< @brief #PP_CustomCmdIDType         */
#define BRCM_SWARCH_PP_ENCODE_MSG_TYPE     (0x26U)    /**< @brief #PP_EncodeMsgType           */
#define BRCM_SWARCH_PP_DECODE_MSG_TYPE     (0x27U)    /**< @brief #PP_DecodeMsgType           */
#define BRCM_SWARCH_PP_EPB_DEL_MSG_TYPE    (0x28U)    /**< @brief #PP_EPBRemoveMsgType        */
#define BRCM_SWARCH_PP_EPB_ADD_MSG_TYPE    (0x29U)    /**< @brief #PP_EpbAddMsgType           */
#define BRCM_SWARCH_PP_DMA_COPY_MSG_TYPE   (0x2AU)    /**< @brief #PP_DMACopyMsgType          */
#define BRCM_SWARCH_PP_ALLOC_VSP_PROC      (0x2BU)    /**< @brief #PP_AllocSPCore             */
#define BRCM_SWARCH_PP_BOOTCORE_EXT_PROC   (0x2CU)    /**< @brief #PP_BootCoreExt             */
#define BRCM_SWARCH_PP_CUSTOM_CMD_PROC     (0x2DU)    /**< @brief #PP_CustomCmd               */
#define BRCM_SWARCH_PP_POST_JOB_PROC       (0x2EU)    /**< @brief #PP_PostJob                 */
#define BRCM_SWARCH_PP_GET_JOB_STATUS_PROC (0x2FU)    /**< @brief #PP_GetJobStatus            */
#define BRCM_SWARCH_PP_DMA_CONFIG_TYPE     (0x30U)    /**< @brief #PP_DMAConfigMsgType        */
#define BRCM_SWARCH_PP_DMA_CHN_ATRIB_TYPE  (0x31U)    /**< @brief #PP_DMAChannelAttrType      */
#define BRCM_SWARCH_PP_CAM_DMA_START_PROC  (0x32U)    /**< @brief #PP_CameraDMAStart          */
#define BRCM_SWARCH_PP_CAM_DMA_STATUS_PROC (0x33U)    /**< @brief #PP_CameraDMAStatus         */

/** @}*/

/**
    @brief SP Server verification key (ABI:AVC Baseline Intra)

    @trace #BRCM_SWREQ_PP_SERVER
*/
#define PP_SERVER_VERIFICATION_KEY (0x414249UL)

/**
    @brief SP Server maximum dma size per row

    @trace #BRCM_SWREQ_PP_SERVER
*/
#define PP_SERVER_DMA_MAX_SIZE     (8192UL)

/** SP Server specifiic job types
    @trace #BRCM_SWREQ_PP_INIT
*/
typedef uint32_t PP_ServerJobType;
#define PP_SERVER_JOB_ENCODE     (0UL)   /**< Encoder jobs                */
#define PP_SERVER_JOB_EPB        (1UL)   /**< EPB jobs                    */
#define PP_SERVER_JOB_DECODE     (2UL)   /**< Decoder Jobs                */
#define PP_SERVER_JOB_DMA        (3UL)   /**< DMA Jobs                    */
#define PP_SERVER_JOB_MAX_TYPE   (4UL)   /**< Maximum number of job types */
#define PP_SERVER_JOB_MASK       (3UL)   /**< Mask to get the job type    */

/** SP Server specific message types
    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef uint32_t PP_ServerMsgType;
#define PP_SERVER_MSG_CMD               (0x8000UL)                                   /**< Message command          */
#define PP_SERVER_MSG_RESP              (0x9000UL)                                   /**< Message response         */
#define PP_SERVER_MSG_CONFIG_ISP        (0xA000UL)                                   /**< ISP configuration message*/
#define PP_SERVER_MSG_VARIANT_OFFSET    (0x0100UL)                                   /**< Message variant offset   */
#define PP_SERVER_MSG_CMD_ENCODE        (PP_SERVER_MSG_CMD | PP_SERVER_JOB_ENCODE)   /**< Encoder command message  */
#define PP_SERVER_MSG_RESP_ENCODE       (PP_SERVER_MSG_RESP | PP_SERVER_JOB_ENCODE)  /**< Encoder response message */
#define PP_SERVER_MSG_CMD_EPB_ADDITION  (PP_SERVER_MSG_CMD | PP_SERVER_JOB_EPB)      /**< EPB add command message  */
#define PP_SERVER_MSG_RESP_EPB_ADDITION (PP_SERVER_MSG_RESP | PP_SERVER_JOB_EPB)     /**< EPB add response message */
#define PP_SERVER_MSG_CMD_EPB_REMOVAL   \
       ((PP_SERVER_MSG_CMD + PP_SERVER_MSG_VARIANT_OFFSET) | PP_SERVER_JOB_EPB)      /**< EPB removal cmd message  */
#define PP_SERVER_MSG_RESP_EPB_REMOVAL  \
       ((PP_SERVER_MSG_RESP + PP_SERVER_MSG_VARIANT_OFFSET) | PP_SERVER_JOB_EPB)     /**< EPB removal resp message */
#define PP_SERVER_MSG_CMD_DECODE        (PP_SERVER_MSG_CMD | PP_SERVER_JOB_DECODE)   /**< Decoder command message  */
#define PP_SERVER_MSG_RESP_DECODE       (PP_SERVER_MSG_RESP | PP_SERVER_JOB_DECODE)  /**< Decoder response message */
#define PP_SERVER_MSG_CMD_DMA           (PP_SERVER_MSG_CMD | PP_SERVER_JOB_DMA)      /**< DMA command message      */
#define PP_SERVER_MSG_RESP_DMA          (PP_SERVER_MSG_RESP | PP_SERVER_JOB_DMA)     /**< DMA response message     */
#define PP_SERVER_MSG_CMD_CAMERA_DMA    \
    ((PP_SERVER_MSG_CMD + PP_SERVER_MSG_VARIANT_OFFSET) | PP_SERVER_JOB_DMA)         /**< Camera DMA command message      */
#define PP_SERVER_MSG_RESP_CAMERA_DMA   \
    ((PP_SERVER_MSG_RESP + PP_SERVER_MSG_VARIANT_OFFSET) | PP_SERVER_JOB_DMA)        /**< Camera DMA response message     */

#define PP_SERVER_MSG_CMD_ISP           (PP_SERVER_MSG_CONFIG_ISP)                   /**< Configure ISP ID */
#define PP_SERVER_MSG_NOT_SUPPORTED     (0xFFFFFF00UL)                               /**< Unsupported message      */
#define PP_SERVER_MSG_ERR_OCCURED       (0xFFFFFFFFUL)                               /**< Error message      */

/** SP Server message specific response
    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef uint32_t PP_ServerRespType;
#define PP_SERVER_RESP_OK       (0UL)   /**< @brief Job successful */
#define PP_SERVER_RESP_WRONG    (1UL)   /**< @brief Job was wrong */
#define PP_SERVER_RESP_ERROR    (2UL)   /**< @brief Error occured */
#define PP_SERVER_RESP_OVERFLOW (3UL)   /**< @brief Buffer overflow */

/** SP Driver specific commands handled by PPDrv_CmdHandler
    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef uint32_t PP_CustomCmdIDType;
#define PP_CUSTOM_CMD1 (0x50695073UL)   /**< Enable ISPV interrupt   */
#define PP_CUSTOM_CMD2 (0x50695074UL)   /**< Disable ISPV interrupt  */
#define PP_CUSTOM_CMD3 (0x50695075UL)   /**< Enable ISPV             */
#define PP_CUSTOM_CMD4 (0x50695076UL)   /**< Disable ISPV            */
#define PP_CUSTOM_CMD5 (0x50695077UL)   /**< Configure for HS        */

/**
    @brief Encode message

    This takes all the necessary parameters to encode one or two MB rows.

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_EncodeMsgType {
    PP_ServerRespType response;   /**< @brief Response for the message                           */
    uint32_t totalCoeffAddr;      /**< @breif TC buffer of size 1024 byte                        */
    uint32_t mp0Addr;             /**< @brief MP0 gpData1 address                                */
    uint32_t mp1Addr;             /**< @brief MP1 gpData1 address. Should be NULL if unused      */
    uint32_t mbDataAddr0;         /**< @brief Expected 2*8*1024 size memory for MP0              */
    uint32_t mbDataAddr1;         /**< @brief Expected 2*8*1024 size memory for MP1              */
    uint32_t bsAddr0;             /**< @brief Expected 16*1024 size memory for MP0 bitstream     */
    uint32_t bsAddr1;             /**< @brief Expected 16*1024 size memory for MP1 bitstream     */
    uint32_t bsSize0InBits;       /**< @brief Would be updated after encoding                    */
    uint32_t bsSize1InBits;       /**< @brief Would be updated after encoding                    */
    uint8_t  bsSizeExceeded;      /**< @brief BS buffer is not sufficient                        */
    uint8_t  sliceStart;          /**< @brief Flags indicating slice start                       */
    int16_t  qpChange;            /**< @brief Flags Flags indicating qp change from last row     */
    uint8_t  numMBs;              /**< @brief Total MBs per row                                  */
    uint8_t  spWaitIdx;           /**< @brief For debugging                                      */
    uint8_t  spFinished0;         /**< @brief For debugging                                      */
    uint8_t  spFinished1;         /**< @brief For debugging                                      */
    uint8_t  mp0Assigned;         /**< @brief For debugging                                      */
    uint8_t  mp0Finished;         /**< @brief For debugging                                      */
    uint8_t  mp1Assigned;         /**< @brief For debugging                                      */
    uint8_t  mp1Finished;         /**< @brief For debugging                                      */
    uint8_t  spAssigned0;         /**< @brief For debugging                                      */
    uint8_t  spAssigned1;         /**< @brief For debugging                                      */
    uint8_t  spLastData0;         /**< @brief For debugging                                      */
    uint8_t  spLastData1;         /**< @brief For debugging                                      */
} PP_EncodeMsgType;

/**
    @brief Decode message

    This takes all the necessary parameters to decode one slice.

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_DecodeMsgType {
    PP_ServerRespType response;    /**< @brief Response for the message    */
    uint32_t streamAddr;           /**< @brief Stream address              */
    uint32_t streamSize;           /**< @brief Stream size                 */
    uint16_t sliceStart;           /**< @brief Flag indicating slice start */
    uint8_t  mbCol;                /**< @brief MB column number            */
    uint8_t  mbRow;                /**< @brief MB row number               */
    uint32_t mbDataAddr;           /**< @brief Address to store MB data    */
    uint32_t totalCoeffAddr;       /**< @brief Address to store TC         */
    uint32_t usedSize;             /**< @brief Decoded size                */
    uint32_t usedOffset;           /**< @brief Decoded address offset      */
    uint16_t numMBs;               /**< @brief Number of MBs               */
    uint8_t  widthInMBs;           /**< @brief Width in MBs                */
    uint8_t  initQP;               /**< @brief Initial QP                  */
    int8_t   log2MaxFrmNum;        /**< @brief Log2 Max Frame Number       */
    int8_t   pocType;              /**< @brief POC type                    */
    int8_t   log2MaxPOC;           /**< @brief Log2 Max POC                */
    int8_t   dblkCtrlFlag;         /**< @brief Deblocking control flag     */
    int32_t  decLumaLeftTC;        /**< @brief Luma left TC                */
    int32_t  decChromaLeftTC;      /**< @brief Chroma left TC              */
    uint8_t  qpUsed;               /**< @brief QP used                     */
    uint8_t  reserved[7UL];        /**< @brief Reserved                    */
} PP_DecodeMsgType;

/**
    @brief EPB removal message

    This takes all the necessary parameters to do EPB removal.

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_EPBRemoveMsgType {
    PP_ServerRespType response; /**< @brief Response for the message        */
    uint32_t srcAddr;           /**< @brief Stream address                  */
    uint32_t srcSize;           /**< @brief Stream size in bytes            */
    uint32_t dstAddr;           /**< @brief Address to store the result     */
    uint32_t dstSize;           /**< @brief Size of the result              */
    uint32_t reserved[9UL];     /**< #brief Reserved                        */
} PP_EPBRemoveMsgType;

/**
    @brief EPB addition message

    This takes all the necessary parameters to add EPB

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_EpbAddMsgType {
    PP_ServerRespType response;   /**< @brief Response for the message                   */
    uint32_t dstBufAddr;          /**< @brief Destination adress                         */
    uint32_t dstBufMaxSize;       /**< @brief Dst buffer max size                        */
    uint32_t dstBufFilledSize;    /**< @brief Dst buffer filled size                     */
    uint32_t srcBuf0Addr;         /**< @brief Source buffer 0 address                    */
    uint32_t srcBuf0SizeInBits;   /**< @brief Source buffer 0 size                       */
    uint32_t srcBuf1Addr;         /**< @brief Source buffer 1 address                    */
    uint32_t srcBuf1SizeInBits;   /**< @brief Source buffer 1 size                       */
    uint16_t firstMB;             /**< @brief First MB Number                            */
    uint8_t qp;                   /**< @brief Quantization parameter                     */
    uint8_t numMBs;               /**< @brief Number of MBs                              */
    uint8_t widthInMBs;           /**< @brief Width in MBs                               */
    int8_t qpDelta;               /**< @brief Change in QP                               */
    int8_t nalRefIdc;             /**< @brief NAL reference IDC                          */
    int8_t nalUnitType;           /**< @brief NAL unit type                              */
    int8_t ppsID;                 /**< @brief PPS ID                                     */
    int8_t log2MaxFrmNum;         /**< @brief log2MaxFrmNum                              */
    uint8_t frmNum;               /**< @brief frmNum                                     */
    int8_t idrPicID;              /**< @brief IDR Pic ID                                 */
    int8_t pocType;               /**< @brief POC type                                   */
    int8_t poc;                   /**< @brief POC                                        */
    int8_t log2MaxPOC;            /**< @brief Log2 max POC                               */
    int8_t dblkCtrlFlag;          /**< @brief Deblocking control flag                    */
    int8_t disableDblk;           /**< @brief Disable deblocking                         */
    int8_t sliceAlpha;            /**< @brief Slice alpha                                */
    int8_t sliceBeta;             /**< @brief Slice beta                                 */
    uint8_t sliceStart;           /**< @brief Slice start flag                           */
    uint8_t sliceEnd;             /**< @brief Slice end flag                             */
    uint8_t lastData;             /**< @brief Previous MB Row last data                  */
    uint8_t lastDataSize;         /**< @brief Last data size in bits                     */
    uint8_t epbState;             /**< @brief Number of zeros in previous MB Row at last */
} PP_EpbAddMsgType;

/**
    @brief PP DMA channel attributes structure

    Structure for storing the parameters for each channel

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_DMAChannelAttrType {
    uint32_t inAddr;    /**< @brief Source address                 */
    uint32_t outAddr;   /**< @brief Destination address            */
    uint16_t width;     /**< @brief Width in bytes                 */
    uint16_t height;    /**< @brief Number of rows                 */
    uint16_t inStride;  /**< @brief Source stride in bytes         */
    uint16_t outStride; /**< @brief Destination stride in bytes    */
} PP_DMAChannelAttrType;

/**
    @brief DMA copy message

    This takes all the necessary parameters to dma copy

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_DMACopyMsgType {
    PP_ServerRespType response;         /**< @brief Response for the message */
    uint8_t frameNum;                   /**< @brief Would be updated         */
    uint8_t mbNum;                      /**< @brief Number of channels       */
    uint8_t bufIdx;                     /**< @brief Number of channels       */
    uint8_t numCh;                      /**< @brief Number of channels       */
    PP_DMAChannelAttrType chnInfo[3UL]; /**< @brief channel attributes       */
} PP_DMACopyMsgType;

/**
    @brief Camera DMA configuration message

    This takes all the necessary parameters to configure camera dma

    @trace #BRCM_SWREQ_PP_SERVER
*/
typedef struct _PP_DMAConfigMsgType {
    PP_ServerRespType response;         /**< @brief Response for the message */
    uint32_t ispID;
    uint32_t irqStatusAddr;
    uint32_t bufAttrAddr;
    uint32_t numBufs;
    uint32_t reserved[9UL];
} PP_DMAConfigMsgType;

/** @brief Allocate a SP Core
 *
 * The API allocates a SP core to the client and gives a unique ID
 *
 * @param[out]  aCoreID    ID of the SP core allocated
 * @return        status   returns the status
    @trace #BRCM_SWREQ_PP_CORE
 */
int32_t PP_AllocSPCore(uint32_t *const aCoreID );
/** @brief Boot a core allocated previously
 *
 * This API boots the v-MP core with the provided boot binary
 * but with optional parameter to initialize the mailbox.
 *
 * @param[in]  aCoreID      Core ID which needs to be freed
 * @param[in]  aBinPtr      Pointer to binary to boot
 * @param[in]  aBinSize     size of binary
 * @param[in]  aEnableMbox  flag to initialize mailbox
 *
 * @retval      BCM_ERR_OK              Core successfully booted
 * @retval      BCM_ERR_INVAL_PARAMS    Invalid core ID
 * @retval      BCM_ERR_INVAL_PARAMS    aCoreID is NULL
 * @retval      BCM_ERR_INVAL_PARAMS    aBinPtr is NULL
 * @retval      BCM_ERR_INVAL_PARAMS    aBinSize is zero
 * @retval      BCM_ERR_INVAL_STATE     Core in invalid state
    @trace #BRCM_SWREQ_PP_CORE
 */
int32_t PP_BootCoreExt(const uint32_t aCoreID,
                        const uint8_t * const aBinPtr,
                        const uint32_t aBinSize,
                        const uint32_t aEnableMbox);
/** @brief Send a Custom command to SP Driver
 *
 * This API sends a custom request to the v-MP/v-SP core.
 *
 * @param[in]  aCoreID          Core ID to custom command needs to be applied
 * @param[in]  aCustomCmdID     Request type
 * @param[in]  aCustom          32-bit custom data
        for PP_CUSTOM_CMD5 this should be (ispID << 16UL) | (numBufs)
 * @return     status   Returns error if write fails.
    @trace #BRCM_SWREQ_PP_SERVER
 */
int32_t PP_CustomCmd(const uint32_t aCoreID,
                     const PP_CustomCmdIDType aCustomCmdID,
                     const uint32_t aCustom);
/**
    @trace #BRCM_SWREQ_PP_SERVER
*/
int32_t PP_CameraDMAStart(PP_DMACopyMsgType *aMessage, uint32_t *aJobIdx, uint8_t aWaitForInt);

/**
    @trace #BRCM_SWREQ_PP_SERVER
*/
int32_t PP_CameraDMAStatus(PP_DMACopyMsgType *aMessage, uint32_t aJobIdx);

/**
    @trace #BRCM_SWREQ_PP_SERVER
*/
int32_t PP_PostJob(const uint32_t aMsgType,
                   const char *const aPayload,
                   uint32_t *const aIndex);

/**
    @trace #BRCM_SWREQ_PP_SERVER
*/
int32_t PP_GetJobStatus(const uint32_t aIndex,
                   uint32_t *const aMsgType,
                   char *const aPayload);
#endif /* SP_H */
/** @} */
