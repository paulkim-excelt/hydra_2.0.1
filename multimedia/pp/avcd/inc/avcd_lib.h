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
    @defgroup grp_avcdlib AVC Decoder Library
    @ingroup grp_avcd

    @addtogroup grp_avcdlib
    @{
    @section sec_avcdlib_overview Overview
    AVCD Library takes the bitstream and decodes it to form raw video in YUV420
    format output, which is compliant and conforming to Advanced Video Coding
    specifications.
    <BR>AVCD is non-reentrant library. While initializing the AVCD library, user
    provided context is initialized with required values.
    This context shall be maintained by the media component/application for
    subsequent calls to the library till de-initialization. The decoder supports
    EPB removal.

    @section sec_avcdlib_fsm State Machine

    @section sec_avcdlib_seq Sequence Diagrams

    @section sec_avcdlib_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |

    @includedoc avcd_all.dep

    @limitations Only Baseline Profile, Intra frame Decoding is supported
    @limitations No support for 4x4 processing, deblocking and 16x16 planner mode

    @file avcd_lib.h
    @brief This file contains the AVC Decoder functions

    @version 0.6 Export comments from docx
*/

#ifndef AVCD_LIB
#define AVCD_LIB

#include <stdint.h>

/**
    @brief Maximum number of streams supported
    @trace #BRCM_ARCH_AVCDLIB_CONFIG_MACRO #BRCM_REQ_AVCDLIB_CONFIG
*/
#define AVCD_LIB_MAX_NUM_STREAMS (4UL)

/**
    @brief Maximum number of cores supported
    @trace #BRCM_ARCH_AVCDLIB_CONFIG_MACRO #BRCM_REQ_AVCDLIB_CONFIG
*/
#define AVCD_LIB_MAX_NUM_CORES (4UL)

/**
    @brief Size of the AVCD library private data in words
    @trace #BRCM_ARCH_AVCDLIB_CONFIG_MACRO #BRCM_REQ_AVCDLIB_CONFIG
*/
#define AVCD_LIB_DATA_SIZE       (0x20UL)

/**
    @name AVCDLib_State IDs
    @{
    @brief Brief description of AVCDLib_State
    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef uint32_t AVCDLib_State;
#define AVCD_LIB_STATE_RESET    0UL /**< @brief Reset state */
#define AVCD_LIB_STATE_INIT     1UL /**< @brief Library Initialized */
#define AVCD_LIB_STATE_IDLE     2UL /**< @brief Ready for decoding */
#define AVCD_LIB_STATE_RUNNING  3UL /**< @brief Decoding under process */
#define AVCD_LIB_STATE_ERROR    4UL /**< @brief Error occured */
/** @}*/

/**
    @name AVCDLib_StreamState
    @{
    @brief Brief description of AVCDLib_StreamState
    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef uint32_t AVCDLib_StreamState;
#define AVCD_LIB_STREAM_IDLE       (0UL)    /**< @brief Stream is idle */
#define AVCD_LIB_STREAM_DECODING   (1UL)    /**< @brief Streamis getting decoded */
/** @}*/

/**
    @name AVCDLib_Stream
    @{
    @brief Brief description of AVCDLib_StreamID
    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef uint32_t AVCDLib_StreamID;
#define AVCD_LIB_STREAM_0      0UL /**< @brief Stream 0 */
#define AVCD_LIB_STREAM_1      1UL /**< @brief Stream 1 */
#define AVCD_LIB_STREAM_2      2UL /**< @brief Stream 2 */
#define AVCD_LIB_STREAM_3      3UL /**< @brief Stream 3 */
/** @}*/

/**
    @name AVCDLib_SliceType
    @{
    @brief Brief description of AVCDLib_SliceType
    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef uint32_t AVCDLib_SliceType;
#define AVCD_UNKNOWN_SLICE  (0x0UL) /**< @brief Unknown Slice */
#define AVCD_CONFIG_SLICE   (0x1UL) /**< @brief Configuration Slice */
#define AVCD_VIDEO_SLICE    (0x2UL) /**< @brief Video Slice */
/** @}*/

/**
    @name AVCDLib_Flags
    @{
    @brief AVCD Library process flags
    These flags needs to be left shifted by the corresponding stream ID #AVCDLib_StreamID
    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef uint32_t AVCDLib_Flags;
#define AVCD_LIB_SLICE_PROCESSED     0x0001UL /**< @brief Slice Processed */
#define AVCD_LIB_FILL_BUFFER_REQUEST 0x0010UL /**< @brief More data available */
#define AVCD_LIB_FILL_BUFFER_DONE    0x0100UL /**< @brief Fill request served */
#define AVCD_LIB_ERROR_DETECTED      0x1000UL /**< @brief Error detected */
/** @}*/

/**
    @brief Stream context structure to hold the stream specific information

    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef struct _AVCDLIB_StreamContext{
    AVCDLib_StreamState streamState;        /**< @brief Stream State */
    uint32_t  coreIdx;                      /**< @brief Index of the core being used */
    uint32_t  jobIdx;                       /**< @brief Index of the job */
    uint32_t moreData;                      /**< @brief Flag indicating availability of more data */
    uint32_t sliceInProgress;               /**< @brief Flag indicating if a slice is under process */
    uint32_t fillBufReq;                    /**< @brief Flag indicating buffer request */
    uint32_t fw0TempIdx;                    /**< @brief Temporary buffer index used */
    uint32_t fw1Queued;                     /**< @brief Flag indicating if a fill buffer request is being served */
    uint32_t numMBs;                        /**< @brief Number of MBs to process for current MB Row */
    uint32_t filledBufIdx;                  /**< @brief MB row index of the last filled buffer */
    uint32_t width;                         /**< @brief Width of the video */
    uint32_t height;                        /**< @brief Height of the video */
    uint32_t profile;                       /**< @brief Profile of the decoding stream */
    uint32_t level;                         /**< @brief Level of the decoding stream */
    uint32_t initQP;                        /**< @brief Initial QP of the decoding stream */
    uint32_t widthInMBs;                    /**< @brief Width of the video in MBs */
    uint8_t msgPayLoad[56UL];               /**< @brief Message payload */
    uint8_t *bsBuffer;                      /**< @brief Bit stream buffer */
    uint32_t bsSize;                        /**< @brief Bit stream buf size */
    uint32_t dblkCtrlFlag;                  /**< @brief Deblock control flag preset */
}AVCDLIB_StreamContext;

/**
    @brief AVCD Libray Ccontext structure

    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef struct _AVCDLib_Context  {
    uint32_t magic;                 /**< @brief Magic to verify the authenticity */
    AVCDLib_State state;            /**< @brief Current state of the library */
    uint32_t fw0State;              /**< @brief Status indicating which stream is using the firmware 0 */
    uint32_t fw1State;              /**< @brief Status indicating which stream is using the firmware 1 */
    uint32_t tempBufSts;            /**< @brief Status of the temporary buffers */
    AVCDLIB_StreamContext streamContext[AVCD_LIB_MAX_NUM_STREAMS]; /**< @brief Stream context structures */
    uint32_t coreID[AVCD_LIB_MAX_NUM_CORES];      /**< @brief MP Core IDs */
    uint32_t data[AVCD_LIB_DATA_SIZE];            /**< @brief Private data */
} AVCDLib_Context;

/**
    @brief AVCD Library configuration structure

    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef struct _AVCDLib_Config {
    uint32_t magic;                 /**< @brief Magic 0x44435641*/
    uint32_t version;               /**< @brief Version of config */
    uint32_t width;                 /**< @brief Width of the video */
    uint32_t height;                /**< @brief Height of the video */
    uint32_t profile;               /**< @brief Profile to be used in decoding */
    uint32_t level;                 /**< @brief Level to be used in decoding */
    uint32_t numPPCores;            /**< @brief Number of PP cores to be used in library */
} AVCDLib_Config;

/**
    @brief AVCD Library input YUV buffer structure

    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef struct _AVCDLib_YUVBuffer {
    uint8_t *yAddr;                 /**< @brief Pointer to luma data */
    uint8_t *uAddr;                 /**< @brief Pointer to chroma cb data */
    uint8_t *vAddr;                 /**< @brief Pointer to chroma cr data */
    uint16_t width;                 /**< @brief Width of the frame */
    uint16_t yStride;               /**< @brief Luma stride */
    uint16_t cStride;               /**< @brief Chroma stride */
    uint16_t height;                /**< @brief Height of the frame */
    uint16_t format;                /**< @brief Video format */
} AVCDLib_YUVBuffer;

/**
    @brief AVC Decoder Library Stream information structure

    @trace #BRCM_ARCH_AVCDLIB_STATE_TYPE #BRCM_REQ_AVCDLIB_GROUPMACRO
*/
typedef struct _AVCDLib_StreamInfo {
    AVCDLib_SliceType sliceType;    /**< @brief Slice type */
    uint8_t mbRow;                  /**< @brief First MB Row of the slice if Video slice */
    uint8_t mbCol;                  /**< @brief First MB col of the slice if Video slice */
    uint8_t newFrame;               /**< @brief Falg to indicate start of new frame */
    uint8_t reserved[25UL];         /**< @brief Reserved for future use */
} AVCDLib_StreamInfo;

/** @brief Initialize the AVCD library

    This API would initialize the context variables of the AVCD library.
    This API should be called only once.
    To reset the context, AVCDLib_DeInit should be called.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aContext    Pointer to the library structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not #AVCD_LIB_STATE_RESET
    @retval     #BCM_ERR_INVAL_PARAMS       aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_INIT_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_Init(AVCDLib_Context *aContext);

/** @brief Configures the AVCD library

    This API would configure the resolution, profile, level and other basic parameters.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in] aContext         Pointer to the library structure
    @param[in] aStreamID        ID of the stream under configuration as defined by #AVCDLib_StreamID
    @param[in] aConfig          pointer to configuration structure
    @param[in] aTempMemSize     Size of the temparory memory
    @param[in] aNumStreams      Total number of streams


    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Otherwise
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   (aNumStreams is more than #AVCD_LIB_MAX_NUM_STREAMS) or
                                        (aStreamID is invalid) or
                                        (aTempMemSize is 0UL) or
                                        (aConfig is passed with invalid values) or
                                        (aConfig is NULL) or
                                        (aContext is NULL)
    @retval     #BCM_ERR_INVAL_STATE    aContext->state is not #AVCD_LIB_STATE_INIT

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_CONFIG_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_Configure(AVCDLib_Context *aContext,
                          AVCDLib_StreamID aStreamID,
                          const AVCDLib_Config *aConfig,
                          uint32_t aTempMemSize,
                          uint32_t aNumStreams);

/** @brief Boots the PP with provided binary image

    This API allocates the v-MP core and boots the core with a given binary image.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in] aContext Pointer to library context structure
    @param [in] aConfig  16-bit Config IDs
    @param [in] aBinPtr  Pointer to binary data
    @param [in] aBinSize Size of binary data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successful
    @retval     #BCM_ERR_UNKNOWN        #PP_BootCore failed
    @retval     #BCM_ERR_INVAL_PARAMS   (aConfig is not valid) or
                                        (aBinPtr is provided with invalid data) or
                                        (aBinSize is 0) or
                                        (aBinPtr is NULL)
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_BOOT_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_Boot(AVCDLib_Context *aContext,
                     uint16_t aConfig,
                     const uint8_t *const aBinPtr,
                     uint32_t aBinSize);

/** @brief Remove emulation prevention bytes

    Removes emulation prevention bytes from input buffer and copies to data to
    output buffer

    @behavior Sync, Non-reentrant

    @pre None

    @param [out]   aOut  Pointer to store the output
    @param [in]    aIn   pointer to input buffer
    @param [inout] aSize Size of the input buffer. It will get updated with size used after the EPB copy.

    @retval     void

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_EPBCOPY_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
void AVCDLib_EPBCopy(uint8_t *aOut,
                     uint8_t *aIn,
                     uint32_t *aSize);

/** @brief Decodes the header and updates streaminfo structure

    Decodes Stream configuration header. Updates the Slice type, MBRow and MBCol information in the
    streaminfo structure. If it is PPS slice, it updates the initQP parameter of streamContext
    structure of corresponding stream ID.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamID   ID of the stream as defined by #AVCDLib_StreamID
    @param [in]    aBuffer     Buffer to read to decode the configuration. Minimum size should be 6 bytes
    @param [in]    aSize       Size of the buffer provided
    @param [out]   aStreamInfo Pointer to store the stream information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        Library state is neither #AVCD_LIB_STATE_IDLE nor #AVCD_LIB_STATE_RUNNING
    @retval     #BCM_ERR_INVAL_MAGIC        aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS       (aBuffer data is invalid) or
                                            (aStreamID is invalid) or
                                            (aSize is less than 6 bytes) or
                                            (aStreamInfo is NULL) or
                                            (aBuffer is NULL) or
                                            (aContext is NULL)

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_GETSTREAMINFO_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_GetStreamInfo(AVCDLib_Context *aContext,
                              AVCDLib_StreamID aStreamID,
                              uint8_t *aBuffer,
                              uint32_t aSize,
                              AVCDLib_StreamInfo *aStreamInfo);

/** @brief Starts Decoding of a new slice

    Starts decoding of a new slice. Changes the state of Decoder to #AVCD_LIB_STATE_RUNNING if successful.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamID   ID of the stream as defined by #AVCDLib_StreamID
    @param [in]    aBsBuffer   Bit stream buffer to read from
    @param [in]    aBsSize     Bit stream size
    @param [in]    aCurSlice   Pointer to current slice #AVCDLib_StreamInfo structure

    Return values are documented in reverse-chronological order
    @retval     as returned by #PP_SendMsg
    @retval     #BCM_ERR_EAGAIN         firmware is busy
    @retval     #BCM_ERR_INVAL_STATE    stream specified by aStreamID is already under decoding state
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   (aBsBuffer is not a video slice) or
                                        (aStreamID is invalid) or
                                        (aCurSlice is NULL) or
                                        (aBsSize is less than 6 bytes) or
                                        (aBuffer is NULL) or
                                        (aContext is NULL)

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_DECODESLICE_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_DecodeSlice(AVCDLib_Context *aContext,
                            AVCDLib_StreamID aStreamID,
                            uint8_t *aBsBuffer,
                            uint32_t aBsSize,
                            AVCDLib_StreamInfo *aCurSlice,
                            uint8_t* aTempMem);

/** @brief Fills the buffer with decoded data

    Decodes the bitstream and fills the provided buffer with it.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamID   ID of the stream as defined by #AVCDLib_StreamID
    @param [in]    aTempMem    Pointer to the temporary memory
    @param [out]   aYUVBuffer  Structure containing pointers to one MB Row of YUV data

    Return values are documented in reverse-chronological order
    @retval     as returned by #PP_SendMsg
    @retval     #BCM_ERR_EAGAIN         (firmware is busy) or
                                        (No temporary  buffer is available)
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   (aYUVBuffer content is invalid) or
                                        (aYUVBuffer is NULL) or
                                        (aTempMem is NULL) or
                                        (aStreamID is invalid) or
                                        (aContext is NULL)

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_FILLBUFFER_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
int32_t AVCDLib_FillBuffer(AVCDLib_Context *aContext,
                           AVCDLib_StreamID aStreamID,
                           uint8_t* aTempMem,
                           AVCDLib_YUVBuffer *aYUVBuffer);

/** @brief Gets the MB row index of the buffer filled

    Returns the macro block row index of the filled buffer

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamID   ID of the stream as defined by #AVCDLib_StreamID

    Return values are documented in reverse-chronological order
    @retval     MB row index of the buffer filled

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_GETFILLEDMBROWIDX_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
uint32_t AVCDLib_GetFilledBufMBRowIdx(AVCDLib_Context *aContext,
                                      AVCDLib_StreamID aStreamID);

/** @brief Process Events from PP

    Process Events from PP (shall be called by client when MAILBOX event is triggered to client.
    Client shall call this API in a loop until this API returns zero)

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     aContext    Pointer to library context structure
    @param [inout]     aTempMem       Pointer to temporary Memory

    @retval BITMASK as described in #AVCDLib_Flags

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_PROCESS_PROC  #BRCM_REQ_AVCDLIB

    @limitations None
*/
AVCDLib_Flags AVCDLib_Process(AVCDLib_Context *aContext,
                              uint8_t* aTempMem);

/** @brief Deinitialize the AVCD library

    This API de-initialises/resets the context variables of the AVCD library.

    @behavior Sync, Non-reentrant

    @pre None

    @param [inout]     aContext    Pointer to library context structure

    Return values are documented in reverse-chronological order
    @retval #BCM_ERR_OK                Success.
    @retval Others as returned by #PP_FreeCore if it fails
    @retval #BCM_ERR_INVAL_PARAMS     aContext is NULL
    @retval #BCM_ERR_INVAL_MAGIC      aContext->magic is not valid

    @post None

    @trace  #BRCM_ARCH_AVCDLIB_DEINIT_PROC  #BRCM_REQ_AVCE_MEDIA_COMPONENT

    @limitations None
*/
int32_t AVCDLib_DeInit(AVCDLib_Context *aContext);

#endif /* AVCD_LIB */
/** @} */