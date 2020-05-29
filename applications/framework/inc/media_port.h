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
    @defgroup grp_media_port_ifc Media Port Interface
    @ingroup grp_framework

    @addtogroup grp_media_port_ifc
    @{

    @file media_port.h
    @brief Port interface for use of framework and component interaction
    This file defines the port interfaces provided by the framework to the
    different components. These interfaces shall be used by each component to
    request data from the previous component in dataflow chain.

    @version 0.1 Initial Version
*/

#ifndef MEDIA_PORT_H
#define MEDIA_PORT_H

#include <bcm_err.h>
#include <msg_queue.h>
#include <lw_queue.h>
#include <compiler.h>

/**
    @name Media System Port Architecture IDs
    @{
    @brief Architecture IDs for Media System Port
*/
#define BRCM_SWARCH_MSYS_CHANNEL_TYPE               (0x00U)   /**< @brief #MSYS_ChannelType         */
#define BRCM_SWARCH_MSYS_CHANNEL_ATTRIB_TYPE        (0x01U)   /**< @brief #MSYS_ChannelAttribType   */
#define BRCM_SWARCH_MSYS_CHANNEL_ATTRIBSZ_MACRO     (0x02U)   /**< @brief #MSYS_BUFFER_CHANNEL_ATTRIB_SIZE   */
#define BRCM_SWARCH_MSYS_STREAM_INFO_TYPE           (0x03U)   /**< @brief #MSYS_StreamInfoType      */
#define BRCM_SWARCH_MSYS_CHANNEL_BUFFER_TYPE        (0x04U)   /**< @brief #MSYS_ChannelBufferType   */
#define BRCM_SWARCH_MSYS_MAX_CHEANNELS_MACRO        (0x05U)   /**< @brief #MSYS_BUFFER_MAX_CHANNELS */
#define BRCM_SWARCH_MSYS_BUFFER_FORMAT_TYPE         (0x06U)   /**< @brief #MSYS_BufferFormatType    */
#define BRCM_SWARCH_MSYS_BUFFER_TYPE                (0x07U)   /**< @brief #MSYS_BufferType          */
#define BRCM_SWARCH_MSYS_FB_DONE_TYPE               (0x08U)   /**< @brief #MSYS_FillBufferDoneType  */
#define BRCM_SWARCH_MSYS_FB_DONE_V2_TYPE            (0x09U)   /**< @brief #MSYS_FillBufferDoneV2Type*/
#define BRCM_SWARCH_MSYS_IN_PORT_IDX_MACRO          (0x0AU)   /**< @brief #MSYS_InPortIdxType       */
#define BRCM_SWARCH_MSYS_IN_PORT_EVENT_GLOBAL       (0x0BU)   /**< @brief #MSYS_InPortEvent         */
#define BRCM_SWARCH_MSYS_PORT_STATS_TYPE            (0x0CU)   /**< @brief #MSYS_PortStatsType       */
#define BRCM_SWARCH_MSYS_INPORT_RW_TYPE             (0x0DU)   /**< @brief #MSYS_InPortRWType        */
#define BRCM_SWARCH_MSYS_INPORT_MAGIC_MACRO         (0x0EU)   /**< @brief #MSYS_IN_PORT_MAGIC       */
#define BRCM_SWARCH_MSYS_INPORT_TYPE                (0x0FU)   /**< @brief #MSYS_InPortType          */
#define BRCM_SWARCH_MSYS_DECLARE_INPORT_MACRO       (0x10U)   /**< @brief #MSYS_DECLARE_IN_PORT     */
#define BRCM_SWARCH_MSYS_DECLARE_INPORT_V2_MACRO    (0x11U)   /**< @brief #MSYS_DECLARE_IN_PORT_V2  */
#define BRCM_SWARCH_MSYS_OUTPORT_MAGIC_MACRO        (0x12U)   /**< @brief #MSYS_OUT_PORT_MAGIC      */
#define BRCM_SWARCH_MSYS_OUT_PORT_IDX_MACRO         (0x13U)   /**< @brief #MSYS_OutPortIdxType      */
#define BRCM_SWARCH_MSYS_OUT_PORT_EVENT_GLOBAL      (0x14U)   /**< @brief #MSYS_OutPortEvent        */
#define BRCM_SWARCH_MSYS_OUTPORT_TYPE               (0x15U)   /**< @brief #MSYS_OutPortType         */
#define BRCM_SWARCH_MSYS_DECLARE_OUTPORT_MACRO      (0x16U)   /**< @brief #MSYS_DECLARE_OUT_PORT    */
#define BRCM_SWARCH_MSYS_DECLARE_OUTPORT_V2_MACRO   (0x17U)   /**< @brief #MSYS_DECLARE_OUT_PORT_V2 */
#define BRCM_SWARCH_MSYS_INPORT_INIT_PROC           (0x18U)   /**< @brief #MSYS_InPortInit          */
#define BRCM_SWARCH_MSYS_INPORT_PROCESS_PROC        (0x19U)   /**< @brief #MSYS_InPortProcess       */
#define BRCM_SWARCH_MSYS_INPORT_CANCELALL_PROC      (0x1AU)   /**< @brief #MSYS_InPortCancelAll     */
#define BRCM_SWARCH_MSYS_OUTPORT_INIT_PROC          (0x1BU)   /**< @brief #MSYS_OutPortInit         */
#define BRCM_SWARCH_MSYS_FILLBUFFER_PROC            (0x1CU)   /**< @brief #MSYS_FillBuffer          */
/** @} */

/**
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_ChannelType;
#define MSYS_CHANNEL_TYPE_Y                 (0x0UL)           /**< Video Y plane */
#define MSYS_CHANNEL_TYPE_U                 (0x1UL)           /**< Video U plane */
#define MSYS_CHANNEL_TYPE_V                 (0x2UL)           /**< Video V plane */
#define MSYS_CHANNEL_TYPE_ARGB              (0x3UL)           /**< Graphics ARGB interleaved */
#define MSYS_CHANNEL_TYPE_PALETTE           (0x4UL)           /**< Graphics Palette */
#define MSYS_CHANNEL_TYPE_PGX               (0x5UL)           /**< Compressed graphics palatte*/
#define MSYS_CHANNEL_TYPE_LR                (0x10000UL)       /**< Audio LR interleaved */
#define MSYS_CHANNEL_TYPE_CAM_METADATA      (0x0UL)           /**< Camera sensor metadata */
#define MSYS_CHANNEL_TYPE_CAM_ISYS_STATS    (0x1UL)           /**< ISYS statistics */
#define MSYS_CHANNEL_TYPE_CAM_SENSOR_STATS  (0x2UL)           /**< Camera sensor statistics */
#define MSYS_CHANNEL_TYPE_INVALID           (0xFFFFFFFFUL)    /**<Invalid MSYS Channel Type> */

/**
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_ChannelAttribType;
#define MSYS_CHANNEL_ATTRIB_WIDTH    (0x00000000UL)  /**< video width in pixels */
#define MSYS_CHANNEL_ATTRIB_HEIGHT   (0x00000001UL)  /**< video height in pixels */
#define MSYS_CHANNEL_ATTRIB_STRIDE   (0x00000002UL)  /**< video stride in pixels */
#define MSYS_CHANNEL_ATTRIB_RSVD1    (0x00000003UL)  /**< Reserved field 1 */
#define MSYS_CHANNEL_ATTRIB_RSVD2    (0x00000004UL)  /**< Reserved field 2 */

/**
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint8_t MSYS_StreamInfoType;
#define MSYS_STREAM_INFO_MAX  (8U)
#define MSYS_STREAM_INFO_0    (0U)
#define MSYS_STREAM_INFO_1    (1U)
#define MSYS_STREAM_INFO_2    (2U)
#define MSYS_STREAM_INFO_3    (3U)
#define MSYS_STREAM_INFO_4    (4U)
#define MSYS_STREAM_INFO_5    (5U)
#define MSYS_STREAM_INFO_6    (6U)
#define MSYS_STREAM_INFO_7    (7U)

/**
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_BUFFER_CHANNEL_ATTRIB_SIZE (5UL)

/**
    @brief Channel parameters
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef struct _MSYS_ChannelBufferType {
    MSYS_ChannelType channelType;   /**< @brief Channel Type */
    uint8_t *data;                  /**< @brief Pointer to buffer */
    uint32_t size;                  /**< @brief Valid Size of buffer (input),
                                        Filled Size (output) */
    MSYS_ChannelAttribType
        attribs[MSYS_BUFFER_CHANNEL_ATTRIB_SIZE];
                                    /**< @brief Input channel buffer attributes as
                                        per Channel Buffer Attributes table */
} MSYS_ChannelBufferType;

/**
    @brief Maximum number of Media Port channels
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_BUFFER_MAX_CHANNELS (4UL)

/**
    @brief MediaFormat datatype
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_BufferFormatType;
#define MSYS_BUFFER_FORMAT_RAW_DATA           (0x00000000UL)  /**< Raw data */
#define MSYS_BUFFER_FORMAT_VIDEO_YUV_420P     (0x10000000UL)  /**< Raw video in YUV 420P format */
#define MSYS_BUFFER_FORMAT_VIDEO_YUV_422I     (0x10000001UL)  /**< Raw video in YUV 422I format */
#define MSYS_BUFFER_FORMAT_VIDEO_YUV_422P     (0x10000002UL)  /**< Raw video in YUV 422P format */
#define MSYS_BUFFER_FORMAT_VIDEO_YUV_444P     (0x10000003UL)  /**< Raw video in YUV 444P format */
#define MSYS_BUFFER_FORMAT_GFX_ARGB8888       (0x10000010UL)  /**< Graphics data in ARGB888 formuat */
#define MSYS_BUFFER_FORMAT_GFX_P4_ARGB        (0x10000020UL)  /**< Graphics data in 4-bit Pallete format */
#define MSYS_BUFFER_FORMAT_PGX_COMPRESSED     (0x10000030UL)  /**< Compressed pallet graphics */
#define MSYS_BUFFER_FORMAT_AUDIO_PCM          (0x20000000UL)  /**< Raw audio PCM format */

/**
    @brief Structure to be passed for MSYS_FillBuffer API
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef struct _MSYS_BufferType {
    uint64_t timestamp;     /**< @brief Timestamp to be filled/passed       */
    uint32_t version;       /**< @brief Media Buffer version (Set to size of
                                MSYS_BufferType structure)                      */
    MSYS_BufferFormatType format;/**< @brief Data format                         */
    uint32_t numChannels;   /**< @brief Number of valid channels (shall be
                                less than 8). For YUV planar, number of valid
                                channels shall be set to 3. For 7.1 Audio PCM,
                                number of valid channels shall be set to 8  */
    int32_t     response;   /**< @brief Response from Source component
                                informing whether or not the MSYS_FillBuffer
                                operation succeeded or not. This member
                                carries an error code.                      */
    uint32_t    seqNum;     /**< @brief Sequence number [MB No. for Video]
                                (output, to be filled by source component)  */
    uint32_t    frameNum;   /**< @brief Frame number (output, to be filled by
                                source component)                           */
    uint32_t    syncStart;  /**< @brief First packet (Filled by source
                                component to notify sync point              */
    uint32_t    syncEnd;    /**< @brief Last packet (Filled by source
                                component to notify last packet)            */
    uint16_t frameStart;    /**< @brief Frame Start (Filled by source
                                component to notify frame beginning         */
    uint16_t frameEnd;      /**< @brief Frame End (Filled by source component
                                to notify frame end                         */
    uint32_t    bufIdx;     /**< @brief Buffer Index (Used by Sink component
                                for buffer identification)                  */
    MSYS_ChannelBufferType
        buffer[MSYS_BUFFER_MAX_CHANNELS];/**< @brief Channel specific parameters */
    MSYS_StreamInfoType
        streamInfo[MSYS_STREAM_INFO_MAX]; /**< @brief details about the packet */
} MSYS_BufferType;

/** @brief Media Input Port Index

    @trace #BRCM_SWREQ_MSYS_DEFINITION
*/
typedef uint32_t MSYS_InPortIdxType;
#define MSYS_IN_PORT_IDX_0      (0UL)    /**< @brief Input Port 0 */
#define MSYS_IN_PORT_IDX_1      (1UL)    /**< @brief Input Port 1 */
#define MSYS_IN_PORT_IDX_2      (2UL)    /**< @brief Input Port 2 */
#define MSYS_IN_PORT_IDX_3      (3UL)    /**< @brief Input Port 3 */
#define MSYS_IN_PORT_IDX_MAX    (4UL)    /**< @brief Num Input Port */

/** @brief Media Input port events

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
extern const uint32_t MSYS_InPortEvent[MSYS_IN_PORT_IDX_MAX];
#define MSYS_IN_PORT_EVENT_0  (SystemEvent4)  /**< @brief Event for Input Port 0 */
#define MSYS_IN_PORT_EVENT_1  (SystemEvent5)  /**< @brief Event for Input Port 1 */
#define MSYS_IN_PORT_EVENT_2  (SystemEvent6)  /**< @brief Event for Input Port 2 */
#define MSYS_IN_PORT_EVENT_3  (SystemEvent7)  /**< @brief Event for Input Port 3 */

/** @brief Callback to notify Sink component that buffer is filled.

    Callback to indicate to the Sink component that filled buffer is available.

    @rem        The caller needs to check the size appropriately; size is set to
                zero when the request is cancelled.
    @param[in]   aMediaBuffer    Filled Media Buffer

    @return      void
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
typedef void (*MSYS_FillBufferDoneType)(MSYS_BufferType *const aMediaBuffer);

/** @brief Early declaration for Fill Buffer Done Callback

*/
struct _MSYS_InPortType;

/** @brief Callback to notify Sink component that buffer is filled.

    Callback to indicate to the Sink component that filled buffer is available.

    @rem        The caller needs to check the size appropriately; size is set to
                zero when the request is cancelled.
    @param[in]   aMediaBuffer    Filled Media Buffer

    @return      void
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
typedef void (*MSYS_FillBufferDoneV2Type)(const struct _MSYS_InPortType *const aInPort,
                                            MSYS_BufferType *const aMediaBuffer);

/**
    @brief Media system Port statistics

    @trace #BRCM_SWREQ_MSYS_STATISTICS
 */
typedef struct _MSYS_PortStatsType {
    uint32_t nRx;           /**< @brief Number of MSYS_FillBufferDone responses */
    uint32_t nRxCancelled;  /**< @brief Count of total number of errors encountered */
    uint32_t nRxFailed;     /**< @brief Count of total number of errors encountered */
    uint32_t nTx;           /**< @brief Number of successful MSYS_FillBuffer requests */
    uint32_t nTxFailed;     /**< @brief Number of MSYS_FillBuffer requests failed */
    uint32_t nTxCritFailed; /**< @brief Count of Critical Tx Failures */
    uint32_t avgLatency;    /**< @brief Average latency(from media time-stamp) of first
                             media packet for a frame */
    uint32_t frameCount;    /**< @brief Number of frames received */
    uint64_t byteCount;     /**< @brief Number of bytes processed for this port */
} MSYS_PortStatsType;

/**
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef struct _MSYS_InPortRWType {
    LWQ_BufferType fullQ;
    const MSGQ_MsgHdrType* msgHdr[MSGQ_SIZE];
    MSYS_PortStatsType stats;
} MSYS_InPortRWType;

/**
    @brief Media Inport magic ID: MEIP
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_IN_PORT_MAGIC (0x5049454DUL)

/**
    Input port structure to be passed for MSYS_FillBuffer API. This structure shall
    be placed in read only memory space.
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef struct _MSYS_InPortType {
    uint64_t compID;            /**< @brief Sink component ID as described in
                                    Component ID mapping table */
    uint32_t magicID;           /**< @brief Magic ID containing "MEIP"      */
    MSYS_InPortIdxType portIndex; /**< @brief Input port number             */
    uint32_t eventMask;         /**< @brief Event with which Source Component
                                    will signal this/Source Component       */
    MSYS_InPortRWType *inportRW;/**< @brief Circular queue to maintain the
                                    outstanding fill buffer requests        */
    MSYS_FillBufferDoneType
                fillBufferDone;/**< @brief Function pointer to fillBufferDone
                                    callback function of the component      */
    MSYS_FillBufferDoneV2Type
                fillBufferDoneV2;/**< @brief Function pointer to fillBufferDone
                                    callback function of the component      */
} MSYS_InPortType;


/** @brief Defines a MSYS_InPortType

    Macro defines a MSYS_InPortType with name "aName" and initializes it statically.
    This is placed in Read-only region (marked as const).

    @param[in]   aName           Variable name of MSYS_InPortType type
    @param[in]   aIdx            Port number
    @param[in]   aCompID         Sink component ID as described in Component ID
                                 mapping table
    @param[in]   aSignalEvent    Event mask for input ports
    @param[in]   aPortRWName     Name of Inport RW
    @param[in]   aFillDoneCb     Callback function of MSYS_FillBufferDone type

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_DECLARE_IN_PORT(aName, aIdx, aCompID, aSignalEvent,    \
                                aPortRWName, aFillDoneCb)           \
    static MSYS_InPortRWType (aPortRWName) =  {                     \
        .fullQ = 0ULL,                                              \
        .msgHdr = {0UL},                                            \
        .stats = {0UL}                                              \
    };                                                              \
    static const MSYS_InPortType (aName) =                          \
    {                                                               \
        .compID         = (aCompID),                                \
        .magicID        =  MSYS_IN_PORT_MAGIC,                      \
        .portIndex      = (aIdx),                                   \
        .eventMask      = (aSignalEvent),                           \
        .inportRW       = &(aPortRWName),                           \
        .fillBufferDone = (aFillDoneCb),                            \
        .fillBufferDoneV2 = NULL,                                   \
    };

/** @brief Defines a MSYS_InPortType With Memory Protection

    Macro defines a MSYS_InPortType with name "aName" and initializes it statically.
    This is placed in Read-only region (marked as const).

    @param[in]   aName           Variable name of MSYS_InPortType type
    @param[in]   aIdx            Port number
    @param[in]   aCompID         Sink component ID as described in Component ID
                                 mapping table
    @param[in]   aSignalEvent    Event mask for input ports
    @param[in]   aPortRWName     Name of Inport RW
    @param[in]   aFillDoneCb     Callback function of MSYS_FillBufferDone type
    @param[in]   aSecName        Name of the Memory section

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_DECLARE_IN_PORT_V2(aName, aIdx, aCompID, aSignalEvent,      \
                                    aPortRWName, aFillDoneCb, aSecName)  \
    static COMP_SECTION(aSecName) MSYS_InPortRWType (aPortRWName)  =  {  \
        .fullQ = 0ULL,                                                   \
        .msgHdr = {0UL},                                                 \
        .stats = {0UL}                                                   \
    };                                                                   \
    static const MSYS_InPortType (aName) =                               \
    {                                                                    \
        .compID         = (aCompID),                                     \
        .magicID        =  MSYS_IN_PORT_MAGIC,                           \
        .portIndex      = (aIdx),                                        \
        .eventMask      = (aSignalEvent),                                \
        .inportRW       = &(aPortRWName),                                \
        .fillBufferDone = NULL,                                          \
        .fillBufferDoneV2 = (aFillDoneCb),                               \
    };

/**
    MSYS_OutPortType magic ID: MEOP
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
#define MSYS_OUT_PORT_MAGIC (0x504F454DUL)

/** @brief Media Output Port Index

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef uint32_t MSYS_OutPortIdxType;
#define MSYS_OUT_PORT_IDX_0     (0UL)   /**< @brief Output Port 0 */
#define MSYS_OUT_PORT_IDX_1     (1UL)   /**< @brief Output Port 1 */
#define MSYS_OUT_PORT_IDX_2     (2UL)   /**< @brief Output Port 2 */
#define MSYS_OUT_PORT_IDX_3     (3UL)   /**< @brief Output Port 3 */
#define MSYS_OUT_PORT_IDX_MAX   (4UL)   /**< @brief Num Output Port */

/** @brief Media Output port events

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
extern const uint32_t MSYS_OutPortEvent[MSYS_OUT_PORT_IDX_MAX];
#define MSYS_OUT_PORT_EVENT_0  (SystemEvent8)  /**< @brief Event for Output Port 0 */
#define MSYS_OUT_PORT_EVENT_1  (SystemEvent9)  /**< @brief Event for Output Port 1 */
#define MSYS_OUT_PORT_EVENT_2  (SystemEvent10) /**< @brief Event for Output Port 2 */
#define MSYS_OUT_PORT_EVENT_3  (SystemEvent11) /**< @brief Event for Output Port 3 */

/**
    Output port structure to be returned by component GetOutputPort API. This
    structure shall be placed in const region [as required by MsgQ]
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
typedef struct _MSYS_OutPortType {
    uint64_t compID;            /**< @brief Source component ID as described
                                        in Component ID mapping table       */
    uint32_t magicID;           /**< @brief Magic ID #MSYS_OUT_PORT_MAGIC   */
    MSYS_OutPortIdxType portIndex;   /**< @brief Output port number         */
    const MSGQ_Type dataQ;      /**< @brief MsgQ with MSYS_BufferType type  */
} MSYS_OutPortType;

/** @brief Defines a MSYS_OutPortType

    This macro defines a MSYS_OutPortType with name "aName" and initializes it
    statically.

    @param[in]   aName           Variable name of MSYS_OutPortType type
    @param[in]   aIdx            Port index
    @param[in]   aMsgHdrQ        Variable name of message header queue
    @param[in]   aSrcCompTaskId  Source component task, waiting for sink to
                                 enqueue buffers
    @param[in]   aSrcGetMsg      Function pointer to get the i-th enqueued media
                                 buffer
    @param[in]   aCompID         Source component ID
    @param[in]   aEvent          Event to wake up the source component when a
                                 MSYS_FillBuffer request is posted to this output
                                 port
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
#define MSYS_DECLARE_OUT_PORT(aName, aIdx, aMsgHdrQ, aSrcCompTaskId,    \
                               aSrcGetMsg, aCompID, aEvent)             \
    static MSGQ_HdrQType (aMsgHdrQ) = MSGQ_INIT_HDRQ;                   \
    static const MSYS_OutPortType (aName) =                             \
    {                                                                   \
        .magicID    = MSYS_OUT_PORT_MAGIC,                              \
        .portIndex      = (aIdx),                                       \
        .compID     = (aCompID),                                        \
        .dataQ      = MSGQ_INIT((aSrcCompTaskId), (aEvent),             \
                           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_BufferType,  \
                           &(aMsgHdrQ), (aSrcGetMsg)),                  \
    }

/** @brief Defines a MSYS_OutPortType Memory Protection

    This macro defines a MSYS_OutPortType with name "aName" and initializes it
    statically.

    @param[in]   aName           Variable name of MSYS_OutPortType type
    @param[in]   aIdx            Port index
    @param[in]   aMsgHdrQ        Variable name of message header queue
    @param[in]   aSrcCompTaskId  Source component task, waiting for sink to
                                 enqueue buffers
    @param[in]   aSrcGetMsg      Function pointer to get the i-th enqueued media
                                 buffer
    @param[in]   aCompID         Source component ID
    @param[in]   aEvent          Event to wake up the source component when a
                                 MSYS_FillBuffer request is posted to this output
                                 port
    @param[in]   aSecName        Name of the Memory section

    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
#define MSYS_DECLARE_OUT_PORT_V2(aName, aIdx, aMsgHdrQ, aSrcCompTaskId,      \
                                   aSrcGetMsg, aCompID, aEvent, aSecName)    \
    static COMP_SECTION(aSecName) MSGQ_HdrQType (aMsgHdrQ) = MSGQ_INIT_HDRQ; \
    static const MSYS_OutPortType (aName) =                                  \
    {                                                                        \
        .magicID    = MSYS_OUT_PORT_MAGIC,                                   \
        .portIndex  = (aIdx),                                                \
        .compID     = (aCompID),                                             \
        .dataQ      = MSGQ_INIT((aSrcCompTaskId), (aEvent),                  \
                           MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_BufferType,       \
                           &(aMsgHdrQ), (aSrcGetMsg)),                       \
    }

/** @brief API to Initialize media input port

    @param[in]   aPort   Media Input Port to be initialized

    @return      ERR_OK              Request has been successfully queued
    @return      ERR_INVAL_PARAMS    aPort is NULL
    @return      ERR_INVAL_PARAMS    aPort has an invalid magic
    @return      ERR_INVAL_PARAMS    fillBufferDone callback is NULL
    @return      ERR_INVAL_PARAMS    Invalid port index
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
int32_t MSYS_InPortInit(const MSYS_InPortType* const aPort);

/** @brief Post a fill buffer request to framework

    Post a fill buffer request to framework (framework routes to source
    component). This API is called from the component to post a request to the
    source component.

    @param[in]   aPort           Media Input Port
    @param[in]   aMediaBuffer    Media Buffer request structure

    @return      ERR_OK              Request has been successfully queued
    @return      ERR_INVAL_PARAMS    aPort is NULL
    @return      ERR_INVAL_PARAMS    aPort does not have a valid magic
    @return      ERR_INVAL_PARAMS    aPort fillBufferDone callback is NULL
    @return      ERR_INVAL_PARAMS    aPort's index is invalid
    @return      ERR_INVAL_PARAMS    aMediaBuffer is NULL
    @return      ERR_INVAL_PARAMS    aMediaBuffer has 0 channels
    @return      ERR_INVAL_PARAMS    aMediaBuffer does not have valid parameters
                                     for a channel buffer (size is 0 or address
                                     is NULL)
    @return      ERR_UNKNOWN         Invalid output port returned by application
    @return      ERR_BUSY            If source component is busy (output queue
                                     of source component is full)
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
int32_t MSYS_FillBuffer(const MSYS_InPortType* const aPort,
                   const MSYS_BufferType *const aMediaBuffer);

/** @brief API to process updates in media input port

    API to process updates in media input port. This shall be called by sink
    component task when Media Input Port event is triggered. This function
    will internally raise MSYS_FillBufferDone callbacks.

    @param[in]   aPort   Media Input Port

    @return      ERR_OK              Request succeeded. Atleast one buffer was
                                     delivered to sink with MSYS_FillBufferDone
                                     callbacks.
    @return      ERR_INVAL_PARAMS    aPort is NULL
    @return      ERR_INVAL_PARAMS    aPort does not have a valid magic
    @return      ERR_INVAL_PARAMS    aPort's index is invalid
    @return      ERR_INVAL_PARAMS    aPort's fillBufferDone callback is NULL
    @return      ERR_UNKNOWN         Invalid output port returned by application
    @return      ERR_INVAL_STATE     Input port handle queue in invalid state
    @return      ERR_BUSY            Media Buffer still being processed by
                                     output port
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
int32_t MSYS_InPortProcess(const MSYS_InPortType* const aPort);

/** @brief Cancels all requests posted to the port.

    This API cancels all requests posted to the port. It raises the
    MSYS_FillBufferDone callback for all cancelled/filled buffers in FIFO order.

    @param[in]   aPort   Media input port (on which cancel to be performed)

    @return      ERR_OK              All requests cancelled
    @return      ERR_INVAL_PARAMS    aPort is NULL
    @return      ERR_INVAL_PARAMS    aPort does not have a valid magic
    @return      ERR_INVAL_PARAMS    aPort's index is invalid
    @return      ERR_INVAL_PARAMS    aPort's fillBufferDone callback is NULL
    @return      ERR_UNKNOWN         Invalid output port returned by application
    @return      ERR_INVAL_STATE     Input port's circular queue in invalid
                                     state
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
 */
int32_t MSYS_InPortCancelAll(const MSYS_InPortType* const aPort);

/** @brief API to Initialize media output port

    @param[in]   aPort   Media output Port to be initialized

    @return      ERR_OK              Request has been successfully queued
    @return      ERR_INVAL_PARAMS    aPort is NULL
    @return      ERR_INVAL_PARAMS    aPort has an invalid magic
    @return      ERR_INVAL_PARAMS    Invalid port index
    @trace #BRCM_SWREQ_MSYS_COMPONENT_INTERFACE
*/
int32_t MSYS_OutPortInit(const MSYS_OutPortType* const aPort);

#endif /* MEDIA_PORT_H */

/** @} */
