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
/**
    @defgroup grp_avcelib AVC Encoder Library
    @ingroup grp_avce

    @addtogroup grp_avcelib
    @{
    @section sec_avcelib_overview Overview
    AVCE Library takes the raw video in YUV420 format as input and encodes the
    data into a bitstream which is compliant and conforming to Advanced Video
    Coding specifications [@todo link to AVCE SPEC].
    <BR>Once a complete MB Row is available, Encoding is started. User has to
    provide buffer to fill the bitstream data.
    AVCE is non-reentrant library. While initializing the AVCE library, user
    provided context is initialized with required values. This context shall be
    maintained by the client/application for subsequent calls to the library
    till de-initialization.

    @section sec_avcelib_fsm State Machine
    @image avce_lib_statemachine.jpg
    @todo Detailed Description of state machine to be populated here

    @section sec_avcelib_seq Sequence Diagrams
    @image avce_lib_sequence.jpg
    @todo Detailed Description of sequence diagram to be populated here

    @section sec_comp_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |
    @todo Update Performance numbers

    @includedoc avce_all.dep

    @limitations Only YUV 420 planar format is supported

    @file avce_lib.h
    @brief This file contains the AVC Encoder functions
    This file contains the AVC Encoder functions

    @version 0.65 Import from docx
*/

#ifndef AVCE_LIB
#define AVCE_LIB

#include <stdint.h>
#include <lw_queue.h>
/**
    @brief API IDS For Error reporting
    @trace #BRCM_ARCH_AVCELIB_CONFIG_MACRO #BRCM_REQ_AVCELIB_CONFIG
*/
#define AVCE_LIB_PROCESS_API_ID (0xC0UL)

/**
    @brief Maximum number of cores supported for encoder
    @trace #BRCM_ARCH_AVCELIB_CONFIG_MACRO #BRCM_REQ_AVCELIB_CONFIG
*/
#define AVCE_LIB_MAX_NUM_CORES  (2UL)

/**
    @brief Size of AVCE library's private data

    @trace #BRCM_ARCH_AVCELIB_CONTEXT_TYPE
*/
#define AVCE_LIB_DATA_SIZE      (0x20UL)    /**< @brief Internal data */

/**
    @brief Max number of MB rows

    @trace #BRCM_ARCH_AVCELIB_CONTEXT_TYPE
*/
#define AVCE_LIB_MAX_NUM_MBROWS (128UL)    /**< @brief Maximum number of MB rows */


/**
    @brief Size of the buffer to store the core context
    @trace #BRCM_ARCH_AVCELIB_CONFIG_MACRO #BRCM_REQ_AVCELIB_CONFIG
*/
#define AVCE_LIB_CORE_CONTEXT_BUF_SIZE (16UL * 1024UL)

#define AVCE_LIB_INTERNAL_USE_BUF_SIZE (5UL * 1024UL)

/* This should be power of two */
#define AVCE_LIB_MAX_EPB_BUFS (8UL)
/**
    @name AVCELib_State IDs
    @{
    @brief Brief description of AVCELib_State
    @trace #BRCM_ARCH_AVCELIB_STATE_TYPE #BRCM_REQ_AVCELIB_STATE
*/
typedef uint32_t AVCELib_State;
#define AVCE_LIB_STATE_RESET       (0UL)    /**< @brief Reset state */
#define AVCE_LIB_STATE_INIT        (1UL)    /**< @brief Library Initialized */
#define AVCE_LIB_STATE_IDLE        (2UL)    /**< @brief HW is ready */
#define AVCE_LIB_STATE_RUNNING     (3UL)    /**< @brief HW is busy processing */
#define AVCE_LIB_STATE_ERROR       (4UL)    /**< @brief Error occured */
/** @} */

/**
    @name Flags
    @{
    @brief Status bit flags returned by #AVCELib_Process
    @trace #BRCM_ARCH_AVCELIB_PSTATUS_TYPE #BRCM_REQ_AVCELIB_PROCESS
*/
#define AVCE_LIB_MBROW_PROCESSED    (0x1UL)     /**< @brief MB Row processed */
#define AVCE_LIB_START_OF_SLICE     (0x2UL)     /**< @brief Start of Slice.*/
#define AVCE_LIB_END_OF_SLICE       (0x4UL)     /**< @brief End of Slice.*/
#define AVCE_LIB_START_OF_FRAME     (0x8UL)     /**< @brief Start of Frame.*/
#define AVCE_LIB_END_OF_FRAME       (0x10UL)    /**< @brief End of Frame.*/
#define AVCE_LIB_BITSTREAM_FILLED   (0x20UL)    /**< @brief Fill Buffer request processed */
#define AVCE_LIB_BITSTREAM_BUFFER   (0x40UL)    /**< @brief More data present with encoder.
                                                    Use Fill buffer to get data */
#define AVCE_LIB_BUFFER_EXCEEDED    (0x80UL)    /**< @brief Temp BS buffer size was not sufficient */
/** @} */

/**
    @name AVCELib_ProcStatus defines
    @{
    @brief Brief description of AVCELib_ProcStatus
    @trace #BRCM_ARCH_AVCELIB_STREAMSTATE_TYPE #BRCM_REQ_AVCELIB_MULTISTREAM
*/
typedef uint32_t AVCELib_StreamState;
#define AVCE_LIB_STREAM_IDLE       (0UL)          /**< @brief Stream is idle */
#define AVCE_LIB_STREAM_ENCODING   (1UL)          /**< @brief Stream is getting encoded */
/** @} */

typedef uint64_t LWQ_BufferType;

typedef struct _AVCELib_EPBBuf{
    uint8_t numMBRows;
    uint8_t bufIdx[AVCE_LIB_MAX_NUM_CORES];
    uint32_t bufSize[AVCE_LIB_MAX_NUM_CORES];
    uint32_t bufOffset[AVCE_LIB_MAX_NUM_CORES];
    uint8_t sliceStart;
    uint8_t sliceEnd;
    uint8_t frameStart;
    uint8_t frameEnd;
    uint8_t qp;
    uint8_t mbRow;
    uint8_t frameNum;
} AVCELib_EPBBuf;

/**
    @brief Stream context structure to hold the stream specific information
    @trace #BRCM_ARCH_AVCELIB_STREAMCONTEXT_TYPE #BRCM_REQ_AVCELIB_MULTISTREAM
 */
typedef struct _AVCELib_StreamContext{
    AVCELib_StreamState streamState;                  /**< @brief Stream State */
    uint32_t encodeJobIdx;                            /**< @brief Index of the job */
    uint32_t epbJobIdx;                               /**< @brief EPB job idx */
    uint8_t nextEpbBufIdx;                            /**< @brief Next epb buf index */
    AVCELib_EPBBuf epbBuf[AVCE_LIB_MAX_EPB_BUFS];     /**< @brief EPB bufs */
} AVCELib_StreamContext;

/**
    @brief AVCE Lib Context structure
    @trace #BRCM_ARCH_AVCELIB_CONTEXT_TYPE #BRCM_REQ_AVCELIB_ENCODE
 */
typedef struct _AVCELib_Context  {
    uint32_t magic;                         /**< @brief Magic to verify the authenticity   */
    AVCELib_State state;                    /**< @brief Current state of the Library       */
    LWQ_BufferType tempBufQ;                /**< @brief Status of the temporary buffers */
    LWQ_BufferType freeQ;                   /**< @brief Circular Q to represent free EPB bufs*/
    LWQ_BufferType processQ;                /**< @brief Circular Q to represent EPB bufs under use*/
    AVCELib_StreamContext streamContext;    /**< @brief Stream Context */
    uint32_t profile;                       /**< @brief Profile to be used in encoding */
    uint32_t level;                         /**< @brief Level to be used in encoding */
    uint32_t width;                         /**< @brief Width of the video */
    uint32_t height;                        /**< @brief Height of the video */
    uint32_t cropLeft;                      /**< @brief Crop Left */
    uint32_t cropRight;                     /**< @brief Crop Right */
    uint32_t cropTop;                       /**< @brief Crop Top */
    uint32_t cropBottom;                    /**< @brief Crop Bottom */
    uint32_t lowComplexityMode;             /**< @brief 0- All modes: 1 - dc mode */
    uint32_t numPPCores;                    /**< @brief Number of cores to use */
    uint8_t  sliceRowFlag[AVCE_LIB_MAX_NUM_MBROWS];
                                            /**< @brief for all MBrow of a frame
                                                 1 -> slice starts in the row
                                                 0 -> midslice */
    uint8_t qpData[AVCE_LIB_MAX_NUM_MBROWS][2UL];
                                            /**< @brief Rolling QP */
    uint32_t data[AVCE_LIB_DATA_SIZE];      /**< @brief Context Data */
} AVCELib_Context;

/**
    @brief AVCE Configuration structure
    @trace #BRCM_ARCH_AVCELIB_CONFIG_TYPE #BRCM_REQ_AVCELIB_CONFIG
 */
typedef struct _AVCELib_Config {
    uint32_t magic;                 /**< @brief Magic 0x45435641*/
    uint32_t version;               /**< @brief Version of config */
    uint32_t width;                 /**< @brief Width of the video */
    uint32_t height;                /**< @brief Height of the video */
    uint32_t cropLeft;              /**< @brief Crop Left */
    uint32_t cropRight;             /**< @brief Crop Right */
    uint32_t cropTop;               /**< @brief Crop Top */
    uint32_t cropBottom;            /**< @brief Crop Bottom */
    uint32_t fps;                   /**< @brief Framerate */
    uint32_t bitrate;               /**< @brief Bitrate in bits per second */
    uint32_t profile;               /**< @brief Profile to be used in encoding */
    uint32_t level;                 /**< @brief Level to be used in encoding */
    uint32_t numPPCores;            /**< @brief Number of PP cores to be used in library */
    uint32_t minQP;                 /**< @brief Minimum QP value. Allowed values[10 to 16]*/
    uint32_t maxQP;                 /**< @brief Maximum QP value. Allowed values[34 to 44]*/
    uint32_t sliceCfg[128UL];       /**< @brief Row number to specify where each slice should start*/
} AVCELib_Config;

/**
    @name AVCELib_YUVFmt defines
    @{
    @brief Brief description of AVCELib_YUVFmt
    @trace #BRCM_ARCH_AVCELIB_YUVFORMAT_TYPE #BRCM_REQ_AVCELIB_ENCODE
*/
typedef uint16_t AVCELib_YUVFmt;
#define AVCE_LIB_YUV_420P           (0U)    /**< @brief Planar Format */
#define AVCE_LIB_YUV_420SP          (1U)    /**< @brief Semi Planar Format */
/** @} */

/**
    @brief AVCE input YUV buffer structure
    @trace #BRCM_ARCH_AVCELIB_YUVBUFFER_TYPE #BRCM_REQ_AVCELIB_ENCODE
 */
typedef struct _AVCELib_YUVBuffer {
    uint8_t *lumaAddr;                      /**< @brief Pointer to luma data */
    uint8_t *cbAddr;                        /**< @brief Pointer to chroma cb data */
    uint8_t *crAddr;                        /**< @brief Pointer to chroma cr data */
    uint16_t lumaStride;                    /**< @brief Luma stride */
    uint16_t chromaStride;                  /**< @brief Chroma stride */
    AVCELib_YUVFmt format;                  /**< @brief Video format */
    uint8_t frameNo;                        /**< @brief Current Frame number */
    uint8_t mbRow;                          /**< @brief Current Macroblock Row number  */
} AVCELib_YUVBuffer;

/** @brief Initialize the AVCE library

    This API would initialize the context variables of the AVCE library.
    This API should be called only once. To reset the context, AVCELib_DeInit
    should be called.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aContext    Pointer to the library structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Otherwise
    @retval     #BCM_ERR_INVAL_STATE    aContext->state is not #AVCE_LIB_STATE_RESET
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_INIT_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_Init(AVCELib_Context *aContext);


/** @brief Configures the AVCE library

    This API would configure the resolution, bit rate, frame rate, profile,
    level and other basic parameters of the AVCE configuration structure

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aContext        Pointer to the library structure
    @param [in] aConfig         Pointer to configuration structure
    @param [in] aLowCmplxMode   Flag (0-Disable, 1-Enable)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_PARAMS       aConfig is passed with invalid values
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not AVCE_LIB_STATE_INIT
    @retval     #BCM_ERR_INVAL_PARAMS       aConfig is NULL
    @retval     #BCM_ERR_INVAL_MAGIC        aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS       aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_CONFIGURE_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_Configure(AVCELib_Context *aContext,
                          const AVCELib_Config *aConfig,
                          uint8_t aLowCmplxMode,
                          uint32_t aNumBsBufs);

/** @brief Boots the PP with provided binary image

    This API allocates the v-MP core and boots the core with a given binary image.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in] aContext Pointer to library context structure
    @param [in] aConfig  16-bit Config IDs
    @param [in] aBinPtr  Pointer to binary data
    @param [in] aBinSize Size of binary data
    @param [in] aTempMem pointer to the temparory memory
                This should be 16KB + (WIDTH*2) aligned to 1KB multiples

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_UNKNOWN        PP response is invalid
    @retval     #BCM_ERR_INVAL_PARAMS   (aConfig is not valid) or
                                        (aBinPtr is provided with invalid data) or
                                        (aBinSize is 0) or
                                        (aBinPtr is NULL)
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_BOOT_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_Boot(AVCELib_Context *aContext,
                  uint16_t aConfig,
                  const uint8_t *const aBinPtr,
                  uint32_t aBinSize);

/** @brief Sets the target Bitrate for the Encoder

    The purpose of this API is to set the target bit rate of the AVC Encoder in
    the AVCE Context structure.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     aContext    Pointer to library context structure
    @param [in]     aBitRate    Bitrate in bits per second

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Otherwise
    @retval     #BCM_ERR_INVAL_PARAMS   (aMbps is less than 2000000) or
                                        (aMbps is greater than 100000000)
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_SETBR_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_SetBitRate(AVCELib_Context *aContext, uint32_t aBitRate);

/** @brief Encodes MB Row

    This API starts encoding the mb row given to it. Changes the state of
    Encoder to AVCE_LIB_STATE_RUNNING if successful.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     aContext    Pointer to library context structure
    @param [in]     aYUVBuffer  Structure containing pointers to one MB Row of YUV data

    Return values are documented in reverse-chronological order
    @retval     Others                  as returned by #PP_SendMsg
    @retval     #BCM_ERR_INVAL_STATE    aContext->state is not #AVCE_LIB_STATE_IDLE
    @retval     #BCM_ERR_INVAL_PARAMS   (aYUVBuffer content is invalid) or
                                        (aYUVBuffer is NULL)
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_ENCODE_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_Encode(AVCELib_Context *aContext,
                        AVCELib_YUVBuffer *aYUVBuffer,
                        uint32_t aNumMbRows,
                        uint8_t *aLibIntBuf);

/** @brief Provides AVC Encoder with a buffer to fill bitstream

    This API provides a valid buffer which can hold atleast 16 bytes to the AVC
    Encoder to fill the bitstream for the size requested.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]   aContext    Pointer to library context structure
    @param[out]     aBuffer     Buffer to store the bitstream
    @param[in]      aSize       Buffer size
    @param[inout]   aTempMem    Temporary buffer pointer (for internal use)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success. Buffer has been accepted.
    @retval     #BCM_ERR_UNKNOWN        Failed to accept fillbuffer request. PP_SendMsg() failed
    @retval     #BCM_ERR_AGAIN          Buffer has not been accepted. Client has to resubmit
    @retval     #BCM_ERR_INVAL_PARAMS   (aSize is less than 16) or
                                        (aBuffer is NULL)
    @retval     #BCM_ERR_INVAL_STATE    aContext->state is not #AVCE_LIB_STATE_RUNNING
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL

    @post None

    @trace  #BRCM_ARCH_AVCE_FILLSTREAM_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_FillBitStream(AVCELib_Context *aContext,
                                uint8_t *aBuffer,
                                uint32_t aSize);


/** @brief Gets filled size of bitstream buffer

    This API gets filled size of bitstream buffer previously submitted with AVCELib_FillBuffer

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aContext    Pointer to library context structure
    @param[out] aSize       Pointer to Memory where the filled size needs to be updated

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success. aSize is updated with filled size
    @retval     #BCM_ERR_INVAL_PARAMS   (aContext is NULL) or
                                        (aSize is NULL)
    @retval     #BCM_ERR_EAGAIN         Buffer has been under processing. Client has to request again
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid
    @retval     #BCM_ERR_NOMEM          No Buffer was submitted

    @post None

    @trace  #BRCM_ARCH_AVCE_GETSTREAMSIZE_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_GetBitStreamSize(AVCELib_Context *aContext,
                                uint32_t *aSize);

/** @brief Process Events from PP

    Process Events from PP (shall be called by client when MAILBOX event is triggered to client.
    Client shall call this API in a loop until this API returns zero)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aContext    Pointer to library context structure
    @param[inout]   aTempMem    Pointer to Memory where the filled size needs to be updated

    Return values are documented in reverse-chronological order
    @retval     BITMASK         as described in #AVCELib_ProcStatus

    @post None

    @trace  #BRCM_ARCH_AVCE_PROCESS_PROC  #BRCM_REQ_AVCELIB_PROCESS

    @limitations None
*/
uint32_t AVCELib_Process(AVCELib_Context *aContext);

/** @brief Deinitialize the AVCE library

    This API de-initialises/resets the context variables of the AVCE library.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]   aContext    Pointer to library context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success.
    @retval     Others                  as returned by #PP_FreeCore if it fails
    @retval     #BCM_ERR_INVAL_PARAMS   aContext is NULL
    @retval     #BCM_ERR_INVAL_MAGIC    aContext->magic is not valid

    @post None

    @trace  #BRCM_ARCH_AVCE_DEINIT_PROC  #BRCM_REQ_AVCELIB_ENCODE

    @limitations None
*/
int32_t AVCELib_DeInit(AVCELib_Context *aContext);

#endif /* AVCE_LIB */
/** @} */