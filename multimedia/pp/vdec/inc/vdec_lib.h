/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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

 2. TO THE VDEC_MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE VDEC_MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_vdec_lib_ifc Library Interface
    @ingroup grp_vdec

    @addtogroup grp_vdec_lib_ifc
    @{
    @section sec_vdec_lib_overview Overview

    The current vendor delivery consists of two types of firmwares, one to run
    on v-MP core and other to run on v-SP core. Vendor decoder library provides
    interface to interactions with v-MP and v-SP cores to facilitate decoding
    of a h264 complaint stream.

    <BR>Vendor decoder library provides APIs for the following functionalities.
        -# Initialization
        -# Configuration
        -# Decode multiple streams
        -# Start/Restart v-MP and v-SP
        -# Process events from v-SP
        -# Get decoded frame data
        -# Get stream information
        -# De-Initialization

    When the library is initialized, context is initialized. This context has
    to be passed for all the other functions. This library supports maximum of
    four streams simultaneously. All the APIs working with an individual stream
    need to pass the stream information structure which contains the ID and
    other configuration data. To start decoding of a new frame, the v-MP and
    v-SP cores should be restarted. An API to start (restart if its already
    booted) v-MP and v-SP is provided. This shall also gives configuration data
    needed by these to be ready for decoding. The full frame worth stream data
    to be given to get the responce from the decoding via provided API.
    The underlying v-MP and v-SP cores work in sync and once the frame is
    decoded, a message is sent from v-SP core to ARM, which inturn triggers an
    interrupt to ARM. Interrupt service routine then signals the caller task.
    An API to read and process these message is provided, which returns the
    status of the decoding. The dedoded frame information to be received via
    the API provided, which copy the data using PP DMA. To parse and verify
    the configuration data from the stream, an API is provided.

    @section sec_vdec_lib_fsm State Machine
    @subsection sec_vdec_lib_fsm_context Component state machine
    @tagfsm vdec_lib_state_machine.svg "Component state machine"
    @startfsm
    Reset;Init;Idle;Running;
    begin ->    Reset;
    Reset -> Init [folded, label = "VDEC_Init"];
    Init -> Idle [folded, label = "VDEC_Configure"];
    Idle -> Idle [label = "VDEC_GetStreamInfo"];
    Idle -> Running [label = "VDEC_Decode"];
    Running -> Running [label = "VDEC_CopyFrameData"];
    Running -> Idle [label = "VDEC_GetFilledBufInfo"];
    Idle -> Reset [folded, label = "VDEC_DeInit"];
    @endfsm
    The library initialy is in "Reset" state, upon initialization it moves to
    "Init" state. Upin configuration it moves to "Idle" state. The stream
    information API can be called in this "Idle" state and triggers no state
    change. The start of decoding API shall change the state to "Running".
    On completion of the decoding and passing out the information state changes
    to "Idle"(If all the streams are idle). On De-Initialization state changes
    to "Reset"

    @subsection sec_vdec_lib_fsm_stream Stream state machine
    @tagfsm vdec_lib_state_machine_stream.svg "Stream state machine"
    @startfsm
    Reset;Init;Ready;Busy;Error;
    begin -> Reset;
    Reset -> Init [folded, label = "VDEC_Configure"];
    Init -> Init [label = "VDEC_GetStreamInfo"];
    Init -> Ready [label = "VDEC_GetStreamInfo"];
    Ready -> Busy [label = "VDEC_Decode"];
    Busy -> Ready [label = "VDEC_Process"];
    Ready -> Reset [folded, label = "VDEC_DeInit"];
    @endfsm
    The stream is in "Reset" state initially. Upon confuration,
    it makes transition to "Init" state. With the availability of
    both SPS and PPS, it makes transition to "Ready" state. Upon
    start of decoding, it makes transition to "Busy" state. When
    the decoding is done, it makes transition to "Ready" state.
    Any error takes it to "Error" state and recovers after the
    stream is reset.

    @section sec_vdec_lib_impl_seq Sequence Diagrams
    @tagseq vdec_lib_impl_seq_diagram.svg "Sequence diagram"
    @startseq
    group {label = "client"; color = "#EFFFFF";  Client;};
    group {label = "library"; color = "#FFEFFF"; Library;};
    group {label = "pp"; color = "#FFFFEF"; PP;};
    Client => Library [label = "VDEC_Init"];
    Client => Library [label = "VDEC_Configure"];
    === Initialisation Complete ===
    loop {
        Client => Library [label = "VDEC_GetStreamInfo", leftnote="SPS/PPS"];
        Client => Library [label = "VDEC_GetStreamInfo", leftnote="Video"];
        Client => Library [label = "VDEC_Decode"] {
           Library -> PP [label = "decode"];
        };
        Client <-- PP [label = "notify"];
        Client => Library [label = "VDEC_Process"] {
           Library => PP [label = "receive"];
        };
        Client => Library [label = "VDEC_CopyFrameData"] {
           Library -> PP [label = "copy"];
        };
       Client <-- PP [label = "notify"];
       Client => Library [label = "VDEC_Process"] {
           Library => PP [label = "copy done"];
        };
       Client => Library [label = "VDEC_GetFilledBufInfo"];
    };

    Client => Library [label = "VDEC_DeInit"];
    @endseq

    @section sec_vdec_lib_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   10MIPS  per 720p stream                         |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   minimum 2MP and 1SP core                        |

    @limitations Supports only intra streams

    @file vdec_lib.h
    @brief Application interface for vendor decoder library
    This header file contains the interface functions for vdec Library
    @version 0.1 Initial Version
    @note None
*/

#ifndef VDEC_LIB_H
#define VDEC_LIB_H

#include <stdint.h>
#include <lw_queue.h>
/**
    @name VDEC Library Architecture IDs
    @{
    @brief Architecture IDs for  VDEC Library
*/
#define BRCM_SWARCH_VDEC_CONTEXT_MACRO               (0x00U)   /**< @brief #VDEC_CONTEXT_DATA_SIZE */
#define BRCM_SWARCH_VDEC_STATE_TYPE                  (0x01U)   /**< @brief #VDEC_StateType */
#define BRCM_SWARCH_VDEC_STREAMID_TYPE               (0x02U)   /**< @brief #VDEC_StreamIDType */
#define BRCM_SWARCH_VDEC_SLICE_TYPE                  (0x03U)   /**< @brief #VDEC_SliceType */
#define BRCM_SWARCH_VDEC_YUVFORMAT_TYPE              (0x04U)   /**< @brief #VDEC_YuvFormatType */
#define BRCM_SWARCH_VDEC_BITDEPTH_TYPE               (0x05U)   /**< @brief #VDEC_BitDepthType */
#define BRCM_SWARCH_VDEC_ENTROPY_CODING_TYPE         (0x06U)   /**< @brief #VDEC_EntropyCodingType */
#define BRCM_SWARCH_VDEC_BUFMODE_TYPE                (0x07U)   /**< @brief #VDEC_BufModeType */
#define BRCM_SWARCH_VDEC_STREAMSTATE_TYPE            (0x08U)   /**< @brief #VDEC_StreamStateType */
#define BRCM_SWARCH_VDEC_COPYSTATE_TYPE              (0x09U)   /**< @brief #VDEC_CopyStateType */
#define BRCM_SWARCH_VDEC_FLAGS_TYPE                  (0x0AU)   /**< @brief #VDEC_FlagsType */
#define BRCM_SWARCH_VDEC_STREAM_CONTEXT_TYPE         (0x0BU)   /**< @brief #VDEC_StreamContextType */
#define BRCM_SWARCH_VDEC_CONTEXT_TYPE                (0x0CU)   /**< @brief #VDEC_ContextType */
#define BRCM_SWARCH_VDEC_CONFIG_TYPE                 (0x0DU)   /**< @brief #VDEC_ConfigType */
#define BRCM_SWARCH_VDEC_SLICE_INFO_TYPE             (0x0EU)   /**< @brief #VDEC_SliceInfoType */
#define BRCM_SWARCH_VDEC_STREAM_INFO_TYPE            (0x0FU)   /**< @brief #VDEC_StreamInfoType */
#define BRCM_SWARCH_VDEC_YUV_BUFFER_TYPE             (0x10U)   /**< @brief #VDEC_YUVBufferType */
#define BRCM_SWARCH_VDEC_INIT_PROC                   (0x12U)   /**< @brief #VDEC_Init */
#define BRCM_SWARCH_VDEC_DEINIT_PROC                 (0x13U)   /**< @brief #VDEC_DeInit */
#define BRCM_SWARCH_VDEC_CONFIG_PROC                 (0x14U)   /**< @brief #VDEC_Configure */
#define BRCM_SWARCH_VDEC_GET_STREAM_INFO_PROC        (0x15U)   /**< @brief #VDEC_GetStreamInfo */
#define BRCM_SWARCH_VDEC_DECODE_PROC                 (0x16U)   /**< @brief #VDEC_Decode */
#define BRCM_SWARCH_VDEC_PROCESS_PROC                (0x17U)   /**< @brief #VDEC_Process */
#define BRCM_SWARCH_VDEC_COPY_FRAME_DATA_PROC        (0x18U)   /**< @brief #VDEC_CopyFrameData */
#define BRCM_SWARCH_VDEC_GET_FILLED_BUF_INFO_PROC    (0x19U)   /**< @brief #VDEC_GetFilledBufInfo */
#define BRCM_SWARCH_VDEC_GET_REF_BUF_PROC            (0x1BU)   /**< @brief #VDEC_GetRefBuf */

/** @} */

/**
    @name Context macros
    @{
    @brief Context macros

    Context related macros

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
#define VDEC_CONTEXT_DATA_SIZE               (16UL) /**< @brief Internal data size */
#define VDEC_CONTEXT_MAX_NUM_STREMS          (4UL)  /**< @brief Maximum number of paralell streams */
#define VDEC_CONTEXT_MAX_STREAM_CONFIG_SIZE  (32UL) /**< @brief Stream configuration size limit */
#define VDEC_CONTEXT_MAX_SP_CORES            (2UL)  /**< @brief Maximum number of SP cores supported */
#define VDEC_CONTEXT_MAX_MP_CORES            (8UL)  /**< @brief Maximum number of MP cores supported */
#define VDEC_CONTEXT_MAX_MP_CORES_PER_STREAM (4UL)  /**< @brief MP cores limitation per stream */
/** @} */

/**
    @name VDEC_StateType IDs
    @{
    @brief State IDS for library

    State of the library

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint32_t VDEC_StateType;       /**< @brief VDEC_StateType typedef */
#define VDEC_STATE_RESET    (0UL)      /**< @brief Reset state */
#define VDEC_STATE_INIT     (1UL)      /**< @brief Initialized */
#define VDEC_STATE_IDLE     (2UL)      /**< @brief Configured */
#define VDEC_STATE_RUNNING  (3UL)      /**< @brief Busy state */
/** @} */

/**
    @name VDEC_StreamIDType ID
    @{
    @brief Stream ID

    Stream ID

    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint32_t VDEC_StreamIDType;
#define VDEC_STREAMID_0      (0UL) /**< @brief Stream 0 */
#define VDEC_STREAMID_1      (1UL) /**< @brief Stream 1 */
#define VDEC_STREAMID_2      (2UL) /**< @brief Stream 2 */
#define VDEC_STREAMID_3      (3UL) /**< @brief Stream 3 */
/** @}*/

/**
    @name VDEC_SliceType slice type
    @{
    @brief Slice types

    Slice types
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint32_t VDEC_SliceType;
#define VDEC_SLICE_UNKNOWN  (0x0UL) /**< @brief Unknown Slice */
#define VDEC_SLICE_SPS      (0x1UL) /**< @brief SPS Slice */
#define VDEC_SLICE_PPS      (0x2UL) /**< @brief PPS Slice */
#define VDEC_SLICE_VIDEO    (0x3UL) /**< @brief Video Slice */
/** @}*/

/**
    @name VDEC_YuvFormatType YUV formats
    @{
    @brief YUV formats

    YUV formats
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_YuvFormatType;
#define VDEC_YUVFORMAT_400  (0x0UL) /**< @brief YUV 400 (NOT SUPPORTED)*/
#define VDEC_YUVFORMAT_420  (0x1UL) /**< @brief YUV 420 */
#define VDEC_YUVFORMAT_422  (0x2UL) /**< @brief YUV 422 (NOT SUPPORTED) */
#define VDEC_YUVFORMAT_444  (0x3UL) /**< @brief YUV 444 (NOT SUPPORTED) */
/** @}*/

/**
    @name VDEC_BitDepthType Bit depths
    @{
    @brief Bit depths

    Bit depths allowed
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_BitDepthType;
#define VDEC_BITDEPTH_8   (0x8UL) /**< @brief 8-bit Data */
#define VDEC_BITDEPTH_10  (0xAUL) /**< @brief 10-bit Data */
#define VDEC_BITDEPTH_12  (0xCUL) /**< @brief 12-bit Data */
/** @}*/

/**
    @name VDEC_EntropyCodingType Entropy Coding
    @{
    @brief Entropy Coding

    Entropy Coding used
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_EntropyCodingType;
#define VDEC_ENTROPY_CODING_CAVLC   (0x0UL) /**< @brief CAVLC */
#define VDEC_ENTROPY_CODING_CABAC   (0x1UL) /**< @brief CABAC */
/** @}*/

/**
    @name VDEC_BufModeType Buffer mode
    @{
    @brief Buffer mode

    Buffer mode
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_BufModeType;
#define VDEC_BUFMODE_MBROW  (0x0UL) /**< @brief MB row mode */
#define VDEC_BUFMODE_FRAME  (0x1UL) /**< @brief Frame mode */
/** @}*/

/**
    @name VDEC_StreamStateType Stream state
    @{
    @brief Stream state

    Stream state
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_StreamStateType;
#define VDEC_STREAMSTATE_RESET  (0x0UL) /**< @brief Reset state */
#define VDEC_STREAMSTATE_INIT   (0x1UL) /**< @brief Initialized */
#define VDEC_STREAMSTATE_READY  (0x2UL) /**< @brief Stream is ready to be decoded */
#define VDEC_STREAMSTATE_BUSY   (0x3UL) /**< @brief Decoding is in process */
#define VDEC_STREAMSTATE_ERROR  (0x4UL) /**< @brief Error occcured */
/** @}*/

/**
    @name VDEC_CopyStateType Copy state
    @{
    @brief Copy state

    Copy state
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint8_t VDEC_CopyStateType;
#define VDEC_COPYSTATE_RESET  (0x0UL) /**< @brief Reset state */
#define VDEC_COPYSTATE_INIT   (0x1UL) /**< @brief Initialized */
#define VDEC_COPYSTATE_READY  (0x2UL) /**< @brief Copy core is ready */
#define VDEC_COPYSTATE_BUSY   (0x3UL) /**< @brief Copy is in progress */
/** @}*/

/**
    @name VDEC_FlagsType Flags
    @{
    @brief VDEC Library process flags
    These flags needs to be left shifted by the corresponding stream ID #VDEC_StreamIDType
    @trace #BRCM_SWREQ_VDEC_LIBRARY
*/
typedef uint32_t VDEC_FlagsType;
#define VDEC_FLAGS_DECODE_COMPLETED     (0x00001UL) /**< @brief Decoded the buffer */
#define VDEC_FLAGS_FRAME_AVAILABLE      (0x00010UL) /**< @brief Frame data available */
#define VDEC_FLAGS_FRAME_DATA_COPY_DONE (0x00100UL) /**< @brief Frame copy done */
#define VDEC_FLAGS_ERROR_DETECTED       (0x01000UL) /**< @brief Bit stream errors */
#define VDEC_FLAGS_LIBRARY_ERROR        (0x10000UL) /**< @brief Stream in bad state */
/** @}*/


/**
    @brief VDEC library stream context structure

    Structure to hold the contextual data of a stream

    @trace #BRCM_SWREQ_VDEC_LIBRARY
 */
typedef struct _VDEC_StreamContextType {
    VDEC_StreamStateType state;                  /**< @brief Stream state */
    uint32_t numMPCores;                         /**< @brief Number of MP cores to be used */
    uint8_t numMPCoresInUse;                     /**< @brief Number of MP cores in use */
    uint8_t mpLwqIdx[VDEC_CONTEXT_MAX_MP_CORES_PER_STREAM];
                                                 /**< @brief Core Id */
    uint8_t spLwqIdx;                            /**< @brief Core Id */
    uint8_t isSPCoreInUse;                       /**< @brief Flag indicating if SP core is in use*/
    uint8_t refBufIdx;                           /**< @brief Reference buffer index */
    uint8_t isRefBufInUse;                       /**< @brief Flag indicating if Reference buffer is in use*/
    uint32_t width;                              /**< @brief Width */
    uint32_t widthInMBs;                         /**< @brief Width in MBs */
    uint32_t height;                             /**< @brief Height */
    uint32_t level;                              /**< @brief Level used*/
    uint32_t profile;                            /**< @brief Profile used */
    uint32_t fps;                                /**< @brief Frame rate */
    VDEC_BitDepthType bitDepth;                  /**< @brief Bit depth */
    VDEC_EntropyCodingType entropyCoding;        /**< @brief Entropy coding used */
    VDEC_YuvFormatType chromaFormat;             /**< @brief Chroma format */
    uint32_t spsSize;                            /**< @brief SPS data size */
    uint32_t ppsSize;                            /**< @brief PPS data size */
    LWQ_BufferType copyBufs;                     /**< @brief Buffers ready for copy */
    uint16_t cropTop;                            /**< @brief Crop top in pixels */
    uint16_t cropBottom;                         /**< @brief Crop bottom in pixels */
    uint16_t cropLeft;                           /**< @brief Crop left in pixels */
    uint16_t cropRight;                          /**< @brief Crop right in pixels */
} VDEC_StreamContextType;
/**
    @brief VDEC library context structure

    Structure to hold the contextual data

    @trace #BRCM_SWREQ_VDEC_LIBRARY
 */
typedef struct _VDEC_ContextType {
    uint32_t magic;                                            /**< @brief Magic */
    VDEC_StateType state;                                      /**< @brief State as #VDEC_StateType */
    uint32_t width;                                            /**< @brief Maximum Width supported */
    uint32_t height;                                           /**< @brief Maximum Height supported */
    uint32_t bytesPerPixel;                                    /**< @brief Maximum bytes per pixel supported */
    VDEC_CopyStateType copyState;                              /**< @brief Copy state */
    uint32_t copyCoreID;                                       /**< @brief Copy core ID */
    VDEC_StreamIDType copyStreamID;                            /**< @brief Current stream under copy */
    uint32_t refBufSize;                                       /**< @brief Size of each reference buffer */
    LWQ_BufferType freeRefBufs;                                /**< @brief Free reference buffers */
    LWQ_BufferType freeMPCores;                                /**< @brief Free MP cores */
    LWQ_BufferType freeSPCores;                                /**< @brief Free SP cores */
    uint32_t mpCoreID[VDEC_CONTEXT_MAX_MP_CORES];              /**< @brief MP Core IDs*/
    uint32_t spCoreID[VDEC_CONTEXT_MAX_SP_CORES];              /**< @brief SP Core IDs*/
    VDEC_StreamContextType streamContext[VDEC_CONTEXT_MAX_NUM_STREMS];
                                                               /**< @brief Stream information */
    uint32_t data[VDEC_CONTEXT_DATA_SIZE];                     /**< @brief Internal data */
} VDEC_ContextType;

/**
    @brief VDEC library configuration structure

    Structure to hold the configuration data

    @trace #BRCM_SWREQ_VDEC_LIBRARY
    @note Total number of MP cores should be minimum (maximum of all individual stream config cores)
    and one extra core for dma.
 */
typedef struct _VDEC_ConfigType {
    uint32_t version;                                       /**< @brief Version of config */
    uint32_t width;                                         /**< @brief Maximum Width */
    uint32_t height;                                        /**< @brief Maximum Height */
    uint32_t bytesPerPixel;                                 /**< @brief Maximum bytes per pixel */
    uint32_t totalMPCores;                                  /**< @brief Total Number of MP cores */
    uint32_t streamMPCores[VDEC_CONTEXT_MAX_NUM_STREMS];    /**< @brief Number of MP cores to be used per stream */
    uint32_t numSPCores;                                    /**< @brief Number of SP cores */
    uint32_t frameRate[VDEC_CONTEXT_MAX_NUM_STREMS];        /**< @brief Frame rate */
} VDEC_ConfigType;

/**
    @brief VDEC library slice information structure

    Structure to hold the slice information data

    @trace #BRCM_SWREQ_VDEC_LIBRARY
 */
typedef struct _VDEC_SliceInfoType {
    VDEC_SliceType sliceType;        /**< @brief Slice type */
    uint8_t mbRow;                   /**< @brief First MB Row of the slice if #VDEC_SLICE_VIDEO */
    uint8_t mbCol;                   /**< @brief First MB col of the slice if #VDEC_SLICE_VIDEO */
    uint8_t newFrame;                /**< @brief Falg to indicate start of new frame */
    VDEC_BitDepthType bitDepth;      /**< @brief Bit depth as #VDEC_BitDepthType ,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    VDEC_YuvFormatType chromaFormat; /**< @brief Chroma format as #VDEC_YuvFormatType,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t height;                 /**< @brief Height in pixels,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t width;                  /**< @brief Width in pixels,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t cropTop;                /**< @brief Crop top in pixels ,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t cropBottom;             /**< @brief Crop bottom in pixels,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t cropLeft;               /**< @brief Crop left in pixels,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
    uint16_t cropRight;              /**< @brief Crop right in pixels,
                                                 valid if sliceType is #VDEC_SLICE_SPS */
} VDEC_SliceInfoType;

/**
    @brief VDEC library stream information structure

    Structure to hold the stream information data

    @trace #BRCM_SWREQ_VDEC_LIBRARY
    @note Data pointed by configData should be coherent as this
    would be accessed by hardware.
 */
typedef struct _VDEC_StreamInfoType {
    VDEC_StreamIDType streamID;                               /**< @brief Stream ID as #VDEC_StreamIDType */
    uint8_t *intMemory;                                       /**< @brief Pointer to internal memory */
    uint32_t intMemorySize;                                   /**< @brief Size of the internal memory */
    const uint8_t *mpBinaryCopy;                              /**< @brief Pointer to FW1 cavlc binary */
    uint32_t mpBinaryCopySize;                                /**< @brief FW1 cabac cavlc size */
    const uint8_t *mpBinary8;                                 /**< @brief Pointer to FW0 8 bit binary */
    uint32_t mpBinary8Size;                                   /**< @brief FW0 8 bit binary size */
    const uint8_t *mpBinaryN;                                 /**< @brief Pointer to FW0 n bit binary */
    uint32_t mpBinaryNSize;                                   /**< @brief FW0 n bit binary size */
    const uint8_t *spBinaryCabac;                             /**< @brief Pointer to FW1 cabac binary */
    uint32_t spBinaryCabacSize;                               /**< @brief FW1 cabac binary size */
    const uint8_t *spBinaryCavlc;                             /**< @brief Pointer to FW1 cavlc binary */
    uint32_t spBinaryCavlcSize;                               /**< @brief FW1 cabac cavlc size */
    const uint8_t *configData;                                /**< @brief SPS and PPS */
    uint32_t configDataSize;                                  /**< @brief SPS and PPS size */
} VDEC_StreamInfoType;

/**
    @brief VDEC library YUV buffer structure

    Structure to hold the YUV buffer data

    @trace #BRCM_SWREQ_VDEC_LIBRARY
 */
typedef struct _VDEC_YUVBufferType {
    uint8_t *yAddr;                 /**< @brief Pointer to luma data */
    uint8_t *uAddr;                 /**< @brief Pointer to chroma cb data */
    uint8_t *vAddr;                 /**< @brief Pointer to chroma cr data */
    uint16_t width;                 /**< @brief Width of the frame */
    uint16_t height;                /**< @brief Height of the frame */
    uint16_t yStride;               /**< @brief Luma stride */
    uint16_t cStride;               /**< @brief Chroma stride */
    VDEC_BitDepthType bitDepth;     /**< @brief Bit depth as #VDEC_BitDepthType */
    VDEC_YuvFormatType format;      /**< @brief Video format */
} VDEC_YUVBufferType;

/** @brief Initialize the VDEC library

    This API would initialize the context variables of the VDEC library.
    This API should be called only once.
    To reset the context, #VDEC_DeInit should be called.

    @behavior Sync, Non-reentrant

    @pre aContext->state should be #VDEC_STATE_RESET

    @param[inout]   aContext       Pointer to the context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not #VDEC_STATE_RESET
    @retval     #BCM_ERR_INVAL_PARAMS       aContext is NULL

    @post aContext->state would be #VDEC_STATE_INIT

    @trace #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
int32_t VDEC_Init(VDEC_ContextType *aContext);

/** @brief De-Initialize the VDEC library

    This API would deinitialize the context variables of the VDEC library.

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]   aContext       Pointer to the context structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_PARAMS       aContext is NULL

    @post aContext->state would be #VDEC_STATE_RESET

    @trace #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
int32_t VDEC_DeInit(VDEC_ContextType *aContext);

/** @brief Configure the VDEC library

    This API would configure the library for limits on
    the width, height and cores supported.

    @behavior Sync, Non-reentrant

    @pre aContext is initialized with #VDEC_Init

    @param[inout]   aContext       Pointer to the context structure
    @param[in]      aConfig        Pointer to the configuration structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not #VDEC_STATE_INIT
    @retval     #BCM_ERR_INVAL_MAGIC        aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS       aContext is NULL or
                                            aConfig is NULL
    @retval     #BCM_ERR_INVAL_PARAMS       aConfig data is invalid

    @post aContext->state would be #VDEC_STATE_IDLE

    @trace #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
int32_t VDEC_Configure(VDEC_ContextType *aContext,
                       const VDEC_ConfigType *aConfig,
                       uint32_t aNumRefBufs,
                       uint32_t aRefBufSize);

/** @brief Decodes the header and updates streaminfo structure

    Decodes Stream configuration header. Updates the Slice type, MBRow and MBCol information in the
    streaminfo structure. If it is PPS slice, it updates the PPS data of streamContext
    structure of corresponding stream ID.

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamInfo Pointer to stream info structure
    @param [in]    aBuffer     Buffer to read to decode the configuration. Minimum size should be 6 bytes
    @param [in]    aSize       Size of the buffer provided
    @param [out]   aSliceInfo  Pointer to store the slice information

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        Library state is neither #VDEC_STATE_IDLE nor #VDEC_STATE_RUNNING
    @retval     #BCM_ERR_INVAL_MAGIC        aContext->magic is not valid
    @retval     #BCM_ERR_INVAL_PARAMS       (aBuffer data is invalid) or
                                            (aStreamInfo->streamID is invalid) or
                                            (aSize is less than 6 bytes) or
                                            (aSliceInfo is NULL) or
                                            (aStreamInfo is NULL) or
                                            (aBuffer is NULL) or
                                            (aContext is NULL)

    @post None

    @trace  #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
int32_t VDEC_GetStreamInfo(VDEC_ContextType *aContext,
                           const VDEC_StreamInfoType *aStreamInfo,
                           uint8_t *aBuffer,
                           uint32_t aSize,
                           VDEC_SliceInfoType *aSliceInfo);

/** @brief Start decoding

    This API would start the decoding of the buffer provided

    @behavior Sync, Non-reentrant

    @pre aContext is initialized with #VDEC_Init and configured with #VDEC_Configure
         and both SPS and PPS information available in corresponding stream context

    @param[inout]   aContext       Pointer to the context structure
    @param[in]      aStreamInfo    Pointer to the stream information structure
    @param[in]      aBuffer        Pointer to the stream data
    @param[in]      aSize          Size of the stream data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not #VDEC_STATE_IDLE
    @retval     #BCM_ERR_INVAL_PARAMS       (aSize is less than 6) or
                                            (aBuffer is NULL) or
                                            (aStreamInfo is NULL) or
                                            (aContext is NULL)

    @post aContext->state would be #VDEC_STATE_RUNNING

    @trace #BRCM_SWREQ_VDEC_LIBRARY

    @limitations Its the responsibility of the caller to not give
                 slices from different frames when the stream state
                 is #VDEC_STREAMSTATE_BUSY.
*/
int32_t VDEC_Decode(VDEC_ContextType *aContext,
                   const VDEC_StreamInfoType *aStreamInfo,
                   uint8_t *aBuffer,
                   uint32_t aSize);

/** @brief Process Events from PP

    Process Events from PP (shall be called by client when MAILBOX event
    is triggered to client. Client shall call this API in a loop until this
    API returns zero)

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]     aContext    Pointer to library context structure

    @retval BITMASK as described in #VDEC_FlagsType

    @post None

    @trace  #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
VDEC_FlagsType VDEC_Process(VDEC_ContextType *aContext);

/** @brief Copy frame Buffer request

    This API would copy to provided buffer from decoded data
    if any.

    @behavior Sync, Non-reentrant

    @pre Decoded data is available for the requested stream

    @param[inout]   aContext       Pointer to the context structure
    @param[in]      aStreamInfo    Pointer to the stream information structure
    @param[in]      aBufMode       Buffer mode
    @param[inout]   aYUVBuffer     Pointer to the YUV buffer structure

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Success
    @retval     #BCM_ERR_INVAL_STATE        aContext->state is not (#VDEC_STATE_INIT or #VDEC_STATE_RUNNING)
    @retval     #BCM_ERR_INVAL_PARAMS       (aContext is NULL) or
                                            (aStreamInfo is NULL) or
                                            (aYUVBuffer is NULL) or
                                            aYUVBuffer feilds are invalid

    @post None

    @trace #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
int32_t VDEC_CopyFrameData(VDEC_ContextType *aContext,
                        const VDEC_StreamInfoType *aStreamInfo,
                        VDEC_BufModeType aBufMode,
                        VDEC_YUVBufferType *aYUVBuffer);

/** @brief Gets the information of the buffer copied

    Returns the macro block row index of the copied buffer

    @behavior Sync, Non-reentrant

    @pre None

    @param [in]    aContext    Pointer to library context structure
    @param [in]    aStreamInfo Pointer to stream information structure

    Return values are documented in reverse-chronological order
    @retval     MB row index of the buffer filled( returns 0 in frame mode)

    @post None

    @trace  #BRCM_SWREQ_VDEC_LIBRARY

    @limitations None
*/
uint32_t VDEC_GetFilledBufInfo(VDEC_ContextType *aContext,
                               const VDEC_StreamInfoType *aStreamInfo);

/** @brief Get the reference buffer

    This is an intergation function and should be developed by the
    integrator. This function takes the buffer index and returns
    the address of the reference buffer.

    @behavior NA

    @pre None

    @param[in]   aBufIdx       Index of the buffer

    Return values are documented in reverse-chronological order
    @retval     #0UL                        aBufIdx is not valid
    @retval     Address of the reference buffer as 32-bit unsigned integer

    @post None

    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT

    @limitations None
*/
uint32_t VDEC_GetRefBuf(uint8_t aBufIdx);

#endif /* VDEC_LIB_H */

/** @} */
