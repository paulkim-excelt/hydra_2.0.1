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
    @defgroup grp_ldc_ifc Interface
    @ingroup grp_ldc

    @addtogroup grp_ldc_ifc
    @{
    @section sec_ldc_ifc_overview Overview
    Below figure depicts a block diagram representation of the LDC library.
    @image html ldc_overview.jpg "Component Overview"
    LDC Library corrects the lens distortion on the input frame. This involves
    finding the actual position of a pixel in the frame as defined by the matrix
    data. Refer to Configuration Script for details on how to generate this
    matrix data and configuration data for the library.

    @todo add reference to matrix data and configuration script

    To apply lens distortion correction for a particular output MB row,
    corresponding locations in the input frame is calculated and those pixels
    will be fetched from the input MB rows. For better performance, LDC library
    maintains memory pool, with a predetermined order of input MB rows. The
    client shall provide this memory pool to library. For details regarding the
    memory pool size requirements refer to Configuration Script. All the input
    buffers will be copied to the required locations in the memory pool using
    PP DMA. When all the required input MB rows are available, correction for
    the output row can be started. In case of loss of some input MB rows,
    library must be reset to start from a new frame.

    @todo Add reference to configuration script in the paragraph above

    LDC Library provides APIs for these functionalities
    - Initialize the library
    - Configure the library
    - Boot a v-MP with user-provided binary
    - Copy the input buffer to memory pool
    - Apply lens distortion correction for given matrix data
    - Process the events from PP
    - Reset the library
    - De-Initialize the library

    LDC Library is non-reentrant per context. While the library is getting
    initialized, user-provided context is initialized with reset values. This
    context needs to be maintained for subsequent calls to the library till
    de-initialization.

    @section sec_ldc_ifc_fsm State Machine
    This section depicts the state machine diagrams for the benefit of
    application developers. Below figure depicts the state machine for LDC
    Library. It also depicts the APIs which triggers the state transition and
    what all APIs are allowed to be called in each state. If any error occurs
    during API calls, the library moves to #LDC_LIB_STATE_ERROR state.
    #LDCLib_DeInit API can be called in any state and when called triggers a
    transition to the RESET state.
    @image html ldc_state_machine.jpg "State machine of LDC Library"

    @todo Migrate to blockdiag format

    @section sec_ldc_ifc_seq Sequence Diagrams
    This section depicts the sequence diagrams for benefit of application
    developers. Below figure depicts a typical flow of application to interact
    with LDC Library.

    @tagseq ldc_seq_diagram.svg "Sequence diagram"
    @startseq
    span_width = 128;
    Client => LDCLibrary [label = "LDCLib_Init"];
    Client => LDCLibrary [label = "LDCLib_Configure"];
    Client => LDCLibrary [label = "LDCLib_Boot"];
    Client => LDCLibrary [label = "LDCLib_CopyBuffer/LDCLib_Correct"];

    === Client waits for notification from LDCLibrary ===

    Client => LDCLibrary [label = "LDCLib_Process"];
    Client => LDCLibrary [label = "LDCLib_DeInit"];
    @endseq

    @section sec_ldc_ifc_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   30 MIPS for 720P@30fps                          |
    | Memory Bandwidth  |   450 MBPS for 720P@30fps                         |
    | HW Utilization    |   Pixel Processor                                 |

    @todo Add reference to  matrix data in the first limitation

    @limitations Matrix data determines the correction. For more details about
    matrix data refer to. These Correction parameters are configurable at 16x16
    pixel blocks.
    @limitations Each 4x4 pixel block in the output can span up to a maximum of
    8x8 pixel block in input.
    @limitations Correction computations are in fixed-point only.
    @limitations Only YUV420 planar format is supported for input and output.

    @file ldc_lib.h
    @brief Lens Distortion Correction library interface
    This file contains the interface definitions for Lens Distortion Correction
    library
*/
#ifndef LDC_LIB_H
#define LDC_LIB_H

#include <stdint.h>

/**
    @name LDC Library API IDs
    @{
    @brief API IDs for LDC library
*/
#define BRCM_SWARCH_LDC_LIB_MAX_NUM_CORES_MACRO     (0x00U) /**< @brief #LDC_LIB_MAX_NUM_CORES */
#define BRCM_SWARCH_LDC_FLAGS_MACRO                 (0x01U) /**< @brief #LDC_LIB_COPY_DONE */
#define BRCM_SWARCH_LDC_LIB_DATA_SIZE_MACRO         (0x02U) /**< @brief #LDC_LIB_DATA_SIZE */
#define BRCM_SWARCH_LDC_LIB_MAX_MB_ROWS_MACRO       (0x03U) /**< @brief #LDC_LIB_MAX_MB_ROWS */
#define BRCM_SWARCH_LDC_LIB_STATE_TYPE              (0x04U) /**< @brief #LDCLib_State */
#define BRCM_SWARCH_LDC_LIB_BUFFER_STATUS_MACRO     (0x05U) /**< @brief #LDC_LIB_BUF_FREE */
#define BRCM_SWARCH_LDC_LIB_CONTEXT_TYPE            (0x06U) /**< @brief #LDCLib_Context */
#define BRCM_SWARCH_LDC_LIB_CONFIG_TYPE             (0x07U) /**< @brief #LDCLib_Config */
#define BRCM_SWARCH_LDC_LIB_YUV_BUFFER_TYPE         (0x08U) /**< @brief #LDCLib_YUVBuffer */
#define BRCM_SWARCH_LDC_LIB_INIT_PROC               (0x09U) /**< @brief #LDCLib_Init */
#define BRCM_SWARCH_LDC_LIB_CONFIGURE_PROC          (0x0AU) /**< @brief #LDCLib_Configure */
#define BRCM_SWARCH_LDC_LIB_BOOT_PROC               (0x0BU) /**< @brief #LDCLib_Boot */
#define BRCM_SWARCH_LDC_LIB_COPY_BUFFER_PROC        (0x0CU) /**< @brief #LDCLib_CopyBuffer */
#define BRCM_SWARCH_LDC_LIB_CORRECT_PROC            (0x0DU) /**< @brief #LDCLib_Correct */
#define BRCM_SWARCH_LDC_LIB_PROCESS_PROC            (0x0EU) /**< @brief #LDCLib_Process */
#define BRCM_SWARCH_LDC_LIB_RESET_BUF_STATE_PROC    (0x0FU) /**< @brief #LDCLib_ResetBufState */
#define BRCM_SWARCH_LDC_LIB_DEINIT_PROC             (0x10U) /**< @brief #LDCLib_DeInit */
/** @} */

/**
    @brief Maximum Number of cores supported

    @trace #BRCM_SWREQ_LDC_CONFIGURATION
*/
#define LDC_LIB_MAX_NUM_CORES   (2UL)   /**< @brief  Maximum Number of cores
                                             supported */

/**
    @name Flags
    @{
    @brief Status bit flags returned by #LDCLib_Process

    @trace #BRCM_SWREQ_LDC_PROCESS
*/
#define LDC_LIB_COPY_DONE       (0x1UL) /**< @brief LDC Row copied */
#define LDC_LIB_CORRECT_DONE    (0x2UL) /**< @brief LDC Row processed */
/** @} */

/**
    @brief Size of LDC library's private data

    @trace #BRCM_SWREQ_LDC_CONTEXT
*/
#define LDC_LIB_DATA_SIZE       (0x20UL)    /**< @brief Internal data */

/**
    @brief Maximum MB rows supported per frame

    @trace #BRCM_SWREQ_LDC_CONFIGURATION
*/
#define LDC_LIB_MAX_MB_ROWS     (0x80UL)    /**< @brief Maximum MB rows
                                                 supported*/
/**
    @name LDCLib_State
    @{
    @brief State of the LDC library

    @trace #BRCM_SWREQ_LDC_STATE
*/
typedef uint32_t LDCLib_State;          /**< @brief LDC library state type */
#define LDC_LIB_STATE_RESET     (0UL)   /**< @brief Reset state */
#define LDC_LIB_STATE_INIT      (1UL)   /**< @brief Library Initialized */
#define LDC_LIB_STATE_IDLE      (2UL)   /**< @brief HW is ready */
#define LDC_LIB_STATE_RUNNING   (3UL)   /**< @brief HW is busy processing */
#define LDC_LIB_STATE_CLEANUP   (4UL)   /**< @brief HW clean up */
#define LDC_LIB_STATE_ERROR     (5UL)   /**< @brief Error occured */
/** @} */

/**
    @name Buffer status
    @{
    @brief Status of buffers in memory pool

    @trace #BRCM_SWREQ_LDC_CONTEXT
*/
#define LDC_LIB_BUF_FREE        (0U)    /**< @brief Buffer is free in the memory
                                             pool */
#define LDC_LIB_BUF_ALLOC       (1U)    /**< @brief Buffer is allocated */
/** @} */

/**
    @brief LDC library context

    @trace #BRCM_SWREQ_LDC_CONTEXT
*/
typedef struct _LDCLib_Context  {
    uint32_t magic;                         /**< @brief Unique identifier
                                                 initialized by library and used
                                                 to verify the authenticity of
                                                 context data */
    LDCLib_State state;                     /**< @brief Current state of the
                                                 library */
    uint32_t width;                         /**< @brief Width of the video
                                                 frame */
    uint32_t height;                        /**< @brief Height of the video
                                                 frame */
    uint32_t poolSize;                      /**< @brief Memory pool size in MB
                                                 rows */
    uint32_t poolMask;                      /**< @brief Memory pool mask. This
                                                 value should be one less of
                                                 some power of 2. Refer to
                                                 Configuration Script for
                                                 details. @todo Add reference to
                                                 Configuration Script */
    uint8_t minMBRow[LDC_LIB_MAX_MB_ROWS];  /**< @brief Array containing the
                                                 minimum MB Row needed in input
                                                 frame for processing the
                                                 current MB row in output frame
                                                 for all MB rows possible. */
    uint8_t maxMBRow[LDC_LIB_MAX_MB_ROWS];  /**< @brief Array containing the
                                                 maximum MB Row needed in input
                                                 frame for processing the
                                                 current MB row in output frame
                                                 for all MB rows possible. */
    uint8_t mbRowIdx[LDC_LIB_MAX_MB_ROWS];  /**< @brief Index of the buffer in
                                                 memory pool used to store the
                                                 MB Row of the input buffer */
    uint8_t dbgIdx[LDC_LIB_MAX_MB_ROWS];    /**< @brief Debug buffer */
    uint8_t bufSts[LDC_LIB_MAX_MB_ROWS];    /**< @brief Status of buffers in
                                        memory pool. (Refer to
                                        @ref LDC_LIB_BUF_FREE "Buffer status") */
    uint32_t data[LDC_LIB_DATA_SIZE];       /**< @brief Private data of
                                                 library */
} LDCLib_Context;

/**
    @brief LDC library configuration

    @trace #BRCM_SWREQ_LDC_CONFIGURATION
*/
typedef struct _LDCLib_Config {
    uint32_t magic;                         /**< @brief Unique identifier to
                                                 verify authenticity. This shall
                                                 be set to 0x45435641 */
    uint32_t version;                       /**< @brief Version of
                                                 configuration */
    uint32_t input_width;                   /**< @brief Width of the input video
                                                 frame */
    uint32_t input_height;                  /**< @brief Height of the input video
                                                 frame */
    uint32_t output_width;                  /**< @brief Width of the output video
                                                 frame */
    uint32_t output_height;                 /**< @brief Height of the output video
                                                 frame */
    uint32_t numCores;                      /**< @brief Number of PP cores to
                                                 use */
    float xc;                               /**< @brief x co-ordinate of the radial
                                                 distortion center in the input
                                                 video frame */
    float yc;                               /**< @brief y co-ordinate of the radial
                                                 distortion center in the input
                                                 video frame */
    float c0;                               /**< @brief Coefficient 0 of the radial
                                                 distortion correction polynomial */
    float c1;                               /**< @brief Coefficient 1 of the radial
                                                 distortion correction polynomial */
    float c2;                               /**< @brief Coefficient 2 of the radial
                                                 distortion correction polynomial */
    float c3;                               /**< @brief Coefficient 3 of the radial
                                                 distortion correction polynomial */
    float c4;                               /**< @brief Coefficient 4 of the radial
                                                 distortion correction polynomial */
} LDCLib_Config;

/**
    @brief YUV buffer structure

    Lens Distortion Correction YUV buffer structure which contains information
    about an MB row data.

    @trace #BRCM_SWREQ_LDC_YUV_BUFFER
*/
typedef struct _LDCLib_YUVBuffer {
    uint8_t     *lumaAddr;      /**< @brief Pointer to luma(Y) data */
    uint8_t     *cbAddr;        /**< @brief Pointer to chroma(cb) data  */
    uint8_t     *crAddr;        /**< @brief Pointer to chroma(cr) data  */
    uint16_t    lumaStride;     /**< @brief Luma stride in bytes */
    uint16_t    chromaStride;   /**< @brief Chroma stride in bytes */
    uint16_t    format;         /**< @brief Video format (This is not applicable
                                     for now) */
    uint8_t     frameNo;        /**< @brief Current Frame number */
    uint8_t     mbRow;          /**< @brief Current MB Row number  */
} LDCLib_YUVBuffer;

/** @brief Initialize context variables of the library

    API to initialize context variables of the library. This context
    (@a aContext) structure should be preserved for any subsequent calls to the
    library.

    @behavior Sync, Non-reentrant

    @pre LDCLib_Context::state of @a aContext should be set to
    #LDC_LIB_STATE_RESET before calling this API.

    @param[inout]   aContext    Pointer to the library context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Library initialized successfully
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL
    @retval     #BCM_ERR_INVAL_STATE    LDCLib_Context::state of @a aContext is
                                        not #LDC_LIB_STATE_RESET

    @post TBD

    @trace #BRCM_SWREQ_LDC_INIT

    @limitations None
*/
int32_t LDCLib_Init(LDCLib_Context *aContext);

/** @brief Configures the LDC library context variables

    API to configure the LDC library context variables

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aContext        Pointer to library context structure
    @param[in]      aConfig         Pointer to configuration structure
    @param[in]      aMatrixData     Pointer to matrix data. Content will be
                                    filled in by this API.
    @param[in]      aMemPoolSize    Size of the memory pool in bytes

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Configuration successful
    @retval     #BCM_ERR_INVAL_MAGIC    @a aContext's LDCLib_Context::magic is
                                        not valid. Context data got corrupted or
                                        Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS   @a aConfig is passed with invalid values
    @retval     #BCM_ERR_INVAL_STATE    aContext->state is not
                                        #LDC_LIB_STATE_INIT
    @retval     #BCM_ERR_INVAL_PARAMS   @a aConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL

    @post TBD

    @trace #BRCM_SWREQ_LDC_CONFIGURE

    @limitations None
*/
int32_t LDCLib_Configure(LDCLib_Context *aContext,
                            const LDCLib_Config *aConfig,
                            uint8_t *aMatrixData,
                            uint32_t aMemPoolSize);

/** @brief Boot the v-MP

    This API boots the v-MP with binary image.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure
    @param[in]      aConfig     NA
    @param[in]      aBinPtr     Pointer to the binary data
    @param[in]      aBinSize    Size of the binary data in bytes

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Successfully booted
    @retval     #BCM_ERR_INVAL_PARAMS   @a aBinPtr is provided with invalid data
    @retval     #BCM_ERR_INVAL_STATE    @a aContext's LDCLib_Context::poolSize
                                        is zero
    @retval     #BCM_ERR_INVAL_MAGIC    @a aContext's LDCLib_Context::magic is
                                        not valid. Context data got corrupted or
                                        Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS   @a aBinSize is zero
    @retval     #BCM_ERR_INVAL_PARAMS   @a aBinPtr is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL

    @post TBD

    @trace #BRCM_SWREQ_LDC_BOOT

    @limitations None
*/
int32_t LDCLib_Boot(LDCLib_Context *aContext,
                        uint32_t aConfig,
                        const uint8_t *const aBinPtr,
                        uint32_t aBinSize);


/** @brief Initiate the copy of the input MB row buffers

    This API initiates the copy of the input MB row  buffers to memory pool
    using DMA.

    @behavior Async, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure
    @param[in]      aMemPool    Pointer to the memory pool
    @param[in]      aYUVBuffer  Structure containing pointers to one MB Row of
                                YUV data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK               Copy not needed.
    @retval     #BCM_ERR_BUSY             Copying Started successfully
    @retval     #BCM_ERR_EAGAIN           @a aContext's LDCLib_Context::state is
                                          #LDC_LIB_STATE_RUNNING and @a
                                          aYUVBuffer's LDCLib_YUVBuffer::mbRow is
                                          zero
    @retval     #BCM_ERR_INVAL_STATE      @a aContext's LDCLib_Context::state is
                                          #LDC_LIB_STATE_IDLE and @a aYUVBuffer's
                                          LDCLib_YUVBuffer::mbRow is non-zero
    @retval     #BCM_ERR_INVAL_STATE      @a aContext's LDCLib_Context::state is
                                          #LDC_LIB_STATE_CLEANUP and @a
                                          aYUVBuffer's LDCLib_YUVBuffer::mbRow is
                                          non-zero
    @retval     #BCM_ERR_EAGAIN           @a aContext's LDCLib_Context::state is
                                          #LDC_LIB_STATE_CLEANUP and @a
                                          aYUVBuffer's LDCLib_YUVBuffer::mbRow is
                                          zero
    @retval     #BCM_ERR_INVAL_STATE      @a aContext's LDCLib_Context::state is
                                          #LDC_LIB_STATE_INIT or
                                          #LDC_LIB_STATE_RESET
    @retval     #BCM_ERR_INVAL_MAGIC      @a aContext's LDCLib_Context::magic is
                                          not valid. Context data got corrupted or
                                          Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS     @a aYUVBuffer's LDCLib_YUVBuffer::mbRow
                                          is invalid
    @retval     #BCM_ERR_INVAL_PARAMS     @a aYUVBuffer is NULL
    @retval     #BCM_ERR_INVAL_PARAMS     @a aMemPool is NULL
    @retval     #BCM_ERR_INVAL_PARAMS     @a aContext is NULL


    @post TBD

    @trace #BRCM_SWREQ_LDC_COPY_BUFFER

    @limitations None
*/
int32_t LDCLib_CopyBuffer(LDCLib_Context *aContext,
                            uint8_t *aMemPool,
                            LDCLib_YUVBuffer *aYUVBuffer);

/** @brief Apply lens distortion correction

    This API applies lens distortion correction on the input with given matrix
    data.

    @behavior Async, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure
    @param[in]      aMemPool    Pointer to memory pool
    @param[in]      aMatrixData Pointer to matrix data. Refer to Matrix data for
                                details
    @param[out]     aYUVBuffer  Structure containing pointers to one MB Row of
                                YUV data

    @todo Add reference to Matrix data in description of aMatrixData

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_BUSY           Correction started successfully
    @retval     #BCM_ERR_EAGAIN         Try Again. Not all the required input MB
                                        rows are available
    @retval     #BCM_ERR_INVAL_PARAMS   @a aYUVBuffer content is invalid
    @retval     #BCM_ERR_INVAL_STATE    @a aContext's LDCLib_Context::state is
                                        not #LDC_LIB_STATE_RUNNING
    @retval     #BCM_ERR_INVAL_MAGIC    @a aContext's LDCLib_Context::magic is
                                        not valid. Context data got corrupted or
                                        Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMatrixData is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMemPool is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aYUVBuffer is NULL
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL

    @post TBD

    @trace #BRCM_SWREQ_LDC_CORRECT

    @limitations None
*/
int32_t LDCLib_Correct(LDCLib_Context *aContext,
                        uint8_t *aMemPool,
                        uint8_t *aMatrixData,
                        LDCLib_YUVBuffer *aYUVBuffer);

/** @brief Process Events from v-MP and DMA

    This API process events from v-MP and DMA. This API shall be called by
    client when a DMA event or a MAILBOX event is triggered to client. Client
    shall call this API in a loop until this API returns zero.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure

    @return     uint32_t                @ref LDC_LIB_COPY_DONE "Status bit flags"

    @post TBD

    @trace #BRCM_SWREQ_LDC_PROCESS

    @limitations None
*/
uint32_t LDCLib_Process(LDCLib_Context *aContext);

/** @brief Reset the memory pool

    This API resets the memory pool related variables of the library. This API
    can be used to reset the values of the library to start the processing a
    new frame.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Reset successfull
    @retval     #BCM_ERR_INVAL_MAGIC    @a aContext's LDCLib_Context::magic is
                                        not valid. Context data got corrupted or
                                        Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL

    @post TBD

    @trace #BRCM_SWREQ_LDC_RESET_BUF_STATE

    @limitations None
*/
int32_t LDCLib_ResetBufState(LDCLib_Context *aContext);

/** @brief De-initialize LDC library

    This API de-initializes the Lens Distortion Correction library.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aContext    Pointer to library context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             De-initialization successfull
    @retval     Others                  As returned from #PP_FreeCore
    @retval     #BCM_ERR_INVAL_MAGIC    @a aContext's LDCLib_Context::magic is
                                        not valid. Context data got corrupted or
                                        Context is not initialized.
    @retval     #BCM_ERR_INVAL_PARAMS   @a aContext is NULL

    @post TBD

    @trace #BRCM_SWREQ_LDC_DEINIT

    @limitations None
*/
int32_t LDCLib_DeInit(LDCLib_Context *aContext);
#endif /* LDC_LIB_H */
/** @} */
