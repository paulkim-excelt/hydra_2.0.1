/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_vgc_media_comp_impl Implementation
    @ingroup grp_vgc_media_comp

    @addtogroup grp_vgc_media_comp_impl
    @{

    @file vgc_comp.c
    @brief VGC Component
    This source file contains the implementation of VGC Component
    VGC component <--> Other component interaction
    Interfaces for component<-->component interactions are defined in
    media_port.h. Components work in pull model, where the VGC component raises
    a MSYS_FillBuffer() request to the framework which shall route the request to the
    corresponding source component's output port. Note that any output<-->input
    port mapping shall be provided by the Application that uses the components.

    @version 0.2 version
*/

#include <component.h>
#include <media_port.h>
#include <msg_queue.h>
#include <lw_queue.h>
#include <ee.h>
#include <vgc_osil.h>
#include <bcm_utils.h>
#include <syscfg.h>
#include <cache.h>
#include <osil/bcm_osil.h>
#include <vgc.h>

#ifndef ENABLE_VGCC_OUTPUT_PORT
#include <board.h>
#include <vgc_display_service.h>
#endif

/**
    @name Component Design IDs
    @{
    @brief Design IDs for Component
*/

#define BRCM_SWDSGN_VGCC_CMDQ_EVENT_MACRO              (0x80U)   /**< @brief #VGCC_CMDQ_EVENT */
#define BRCM_SWDSGN_VGCC_INPORT_VIDEO_BUFFERS_MACRO    (0x81U)   /**< @brief #VGCC_INPORT_VIDEO_BUFFERS */
#define BRCM_SWDSGN_VGCC_VGCD_EVENT_MACRO              (0x82U)   /**< @brief #VGCC_VGCD_EVENT */
#define BRCM_SWDSGN_VGCC_DISP_EVENTS_MACRO             (0x83U)   /**< @brief #VGCC_DISP_EVENTS */
#define BRCM_SWDSGN_VGCC_VID_SRC_EVENTS_MACRO          (0x84U)   /**< @brief #VGCC_VID_SRC_EVENTS */
#define BRCM_SWDSGN_VGCC_GFX_SRC_EVENTS_MACRO          (0x85U)   /**< @brief #VGCC_GFX_SRC_EVENTS */
#define BRCM_SWDSGN_VGCC_SINK_EVENTS_MACRO             (0x86U)   /**< @brief #VGCC_SINK_EVENTS */
#define BRCM_SWDSGN_VGCC_CHANNEL_MAX_MACRO             (0x87U)   /**< @brief #VGCC_CHANNEL_MAX */
#define BRCM_SWDSGN_VGCC_INIT_BUF_INFO_MACRO           (0x88U)   /**< @brief #VGCC_INIT_BUF_INFO */
#define BRCM_SWDSGN_VGCC_TOTAL_GRAPHICS_BUFFERS_MACRO  (0x89U)   /**< @brief #VGCC_TOTAL_GRAPHICS_BUFFERS */
#define BRCM_SWDSGN_VGCC_GRAPHICS_MEDIA_FORMAT_MACRO   (0x8AU)   /**< @brief #VGC_GRAPHICS_MEDIA_FORMAT */
#define BRCM_SWDSGN_VGCC_GRAPHICS_FORMAT_MACRO         (0x8BU)   /**< @brief #VGC_GRAPHICS_FORMAT */
#define BRCM_SWDSGN_VGCC_STATE_TYPE                    (0x8CU)   /**< @brief #VGCC_StateType */
#define BRCM_SWDSGN_VGCC_COMPTYPE_TYPE                 (0x8DU)   /**< @brief #VGCC_CompType */
#define BRCM_SWDSGN_VGCC_BUFINFO_TYPE                  (0x8EU)   /**< @brief #VGCC_BufInfoType */
#define BRCM_SWDSGN_VGCC_COMP_GLOBAL                   (0x8FU)   /**< @brief #VGCC_Comp */
#define BRCM_SWDSGN_VGCC_CMDMSGHDRQ_GLOBAL             (0x90U)   /**< @brief #VGCC_CmdMsgHdrQ */
#define BRCM_SWDSGN_VGCC_CMDMSG_GLOBAL                 (0x91U)   /**< @brief #VGCC_CmdMsg */
#define BRCM_SWDSGN_VGCC_CMDQ_GLOBAL                   (0x92U)   /**< @brief #VGCC_CmdQ */
#define BRCM_SWDSGN_VGCC_MEDIABUFFER_GLOBAL            (0x93U)   /**< @brief #VGCC_MediaBuffer */
#define BRCM_SWDSGN_VGCC_MWBVID_GLOBAL                 (0x94U)   /**< @brief #VGCC_MwbVid */
#define BRCM_SWDSGN_VGCC_MWBGFX_GLOBAL                 (0x95U)   /**< @brief #VGCC_MwbGfx */
#define BRCM_SWDSGN_VGCC_MWBVIDSZ_GLOBAL               (0x96U)   /**< @brief #VGCC_MwbVidSz */
#define BRCM_SWDSGN_VGCC_MWBGFXSZ_GLOBAL               (0x97U)   /**< @brief #VGCC_MwbGfxSz */
#define BRCM_SWDSGN_VGCC_OUTPORT_GLOBAL                (0x98U)   /**< @brief #VGCC_OutPort */
#define BRCM_SWDSGN_VGCC_DATAMSGHDRQ_GLOBAL            (0x99U)   /**< @brief #VGCC_DataMsgHdrQ */
#define BRCM_SWDSGN_VGCC_SENDCMD_GLOBAL                (0x9AU)   /**< @brief #VGCC_SendCmd */
#define BRCM_SWDSGN_VGCC_RECVRESP_GLOBAL               (0x9BU)   /**< @brief #VGCC_RecvResp */
#define BRCM_SWDSGN_VGCC_GET_MEDIA_OUT_PORT_GLOBAL     (0x9CU)   /**< @brief #VGCC_GetMediaOutPort */
#define BRCM_SWDSGN_VGCC_COMP_FN_TABLE_GLOBAL          (0x9DU)   /**< @brief #VGCC_CompFnTable */
#define BRCM_SWDSGN_VGCC_GET_COMP_FN_TABLE_GLOBAL      (0x9EU)   /**< @brief #VGCC_GetCompFnTable */
#define BRCM_SWDSGN_VGCC_VIDEOBUFINFO_GLOBAL           (0x9FU)   /**< @brief #VGCC_VideoBufInfo */
#define BRCM_SWDSGN_VGCC_VIDFREEQ_GLOBAL               (0xA0U)   /**< @brief #VGCC_VidFreeQ */
#define BRCM_SWDSGN_VGCC_VIDFILLEDQ_GLOBAL             (0xA1U)   /**< @brief #VGCC_VidFilledQ */
#define BRCM_SWDSGN_VGCC_VIDBLENDQ_GLOBAL              (0xA2U)   /**< @brief #VGCC_VidBlendQ */
#define BRCM_SWDSGN_VGCC_VIDEOINPORT_GLOBAL            (0xA3U)   /**< @brief #VGCC_VideoInPort */
#define BRCM_SWDSGN_VGCC_CFG_GLOBAL                    (0xA4U)   /**< @brief #VGCC_Cfg */
#define BRCM_SWDSGN_VGCC_PALETTEDATA_GLOBAL            (0xA5U)   /**< @brief #VGCC_PaletteData */
#define BRCM_SWDSGN_VGCC_GFXBUFINFO_GLOBAL             (0xA6U)   /**< @brief #VGCC_GfxBufInfo */
#define BRCM_SWDSGN_VGCC_GFXFREEQ_GLOBAL               (0xA7U)   /**< @brief #VGCC_GfxFreeQ */
#define BRCM_SWDSGN_VGCC_GFXFILLEDQ_GLOBAL             (0xA8U)   /**< @brief #VGCC_GfxFilledQ */
#define BRCM_SWDSGN_VGCC_GFXBLENDQ_GLOBAL              (0xA9U)   /**< @brief #VGCC_GfxBlendQ */
#define BRCM_SWDSGN_VGCC_GFXINPORT_GLOBAL              (0xAAU)   /**< @brief #VGCC_GfxInPort */
#define BRCM_SWDSGN_VGCC_TASK_GLOBAL                   (0xABU)   /**< @brief #VGCC_Task */
#define BRCM_SWDSGN_VGCC_GETCMDMSG_PROC                (0xACU)   /**< @brief #VGCC_GetCmdMsg */
#define BRCM_SWDSGN_VGCC_GETDATAMSG_PROC               (0xADU)   /**< @brief #VGCC_GetDataMsg */
#define BRCM_SWDSGN_VGCC_RUNSTATEHANDLER_PROC          (0xAEU)   /**< @brief #VGCC_RunStateHandler */
#define BRCM_SWDSGN_VGCC_READYSTATEHANDLER_PROC        (0xAFU)   /**< @brief #VGCC_ReadyStateHandler */
#define BRCM_SWDSGN_VGCC_FILLVIDEOBUFFER_PROC          (0xB0U)   /**< @brief #VGCC_FillVideoBuffer */
#define BRCM_SWDSGN_VGCC_FILLVIDEOBUFFERDONE_PROC      (0xB1U)   /**< @brief #VGCC_FillVideoBufferDone */
#define BRCM_SWDSGN_VGCC_INITSTATEHANDLER_PROC         (0xB2U)   /**< @brief #VGCC_InitStateHandler */
#define BRCM_SWDSGN_VGCC_RESETSTATEHANDLER_PROC        (0xB3U)   /**< @brief #VGCC_ResetStateHandler */
#define BRCM_SWDSGN_VGCC_FILLGFXBUFFER_PROC            (0xB4U)   /**< @brief #VGCC_FillGfxBuffer */
#define BRCM_SWDSGN_VGCC_FILLGFXBUFFERDONE_PROC        (0xB5U)   /**< @brief #VGCC_FillGfxBufferDone */
#define BRCM_SWDSGN_VGCC_CHANNELBUFFERCOMP_GLOBAL      (0xB6U)   /**< @brief #VGCC_CHANNEL_BUFFER_COMP */

/** @} */

/**
    @name VGCC_StateType
    @{
    @brief VGC Component State Type

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
typedef uint32_t VGCC_StateType;   /**< @brief VGC Component State type */
#define VGCC_STATE_RESET   (0UL)   /**< @brief VGC component Reset state */
#define VGCC_STATE_INIT    (1UL)   /**< @brief VGC component Init state */
#define VGCC_STATE_READY   (2UL)   /**< @brief VGC component Ready state */
#define VGCC_STATE_RUNNING (3UL)   /**< @brief VGC component Running state */

/** @} */

/**
    @brief Event mask for VGC component's command Queue

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_CMDQ_EVENT        SystemEvent0

/**
    @brief Event mask for VGC component’s display service

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_VGCD_EVENT        SystemEvent1

/**
    @brief Event mask for VGC display events

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_DISP_EVENTS    (VGC_VSYNC_EVENT | VGC_LINE_EVENT)

/**
    @brief Event mask for VGC display events

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_VID_SRC_EVENTS (SystemEvent4)

/**
    @brief Event mask for VGC display events

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_GFX_SRC_EVENTS (SystemEvent5)

/**
    @brief Event mask for VGC display events

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_SINK_EVENTS    (SystemEvent8)

/**
    @brief Maximum number of Channels for VGC component's input port

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_CHANNEL_MAX       (3UL)

#ifdef ENABLE_VIDEO_PORT
/**
    @brief Inport video buffers count

    Maximum number of import video buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#ifndef VGCC_INPORT_VIDEO_BUFFERS
#define VGCC_INPORT_VIDEO_BUFFERS (2UL)
#endif

#endif /* ENABLE_VIDEO_PORT */

#ifdef ENABLE_GRAPHICS_PORT
 /**
    @brief Total number of Graphics buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
/* Number of graphics buffers */
#ifndef VGCC_TOTAL_GRAPHICS_BUFFERS
#define VGCC_TOTAL_GRAPHICS_BUFFERS     (4UL)
#endif
#endif /* ENABLE_GRAPHICS_PORT */

/**
    @brief  Structure to hold VGC component information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
typedef struct _VGCC_CompType {
    VGCC_StateType state;            /**< @brief State of the component */
    uint32_t width;                  /**< @brief Processing width */
    uint32_t height;                 /**< @brief Processing height */
    uint32_t vidHeight;              /**< @brief Height of video buffer */
    uint32_t gfxHeight;              /**< @brief Height of graphics buffer */
    uint32_t stopInProgress;         /**< @brief Stop action has been requested */
    uint32_t dropsDetected;          /**< @brief Drop detected */
    VGC_SurfAttrType videoSurfAttr;  /**< @brief Video surface attributes */
    VGC_BufferType videoBuf;         /**< @brief Buffer type */
    uint32_t vidFbdIdx      ;        /**< @brief Index to the next buffer to be used */
    uint32_t videoNextFillReqIdx;    /**< @brief Index of the next MB row buffer to be submitted */
    uint32_t videoPendingReq;        /**< @brief Number of pending requests to source */
    uint32_t videoUseThisBuffer;     /**< @brief Flag to indicate the sanity of the buffer */
    VGC_SurfAttrType gfxSurfAttr;    /**< @brief Graphics surface attributes */
    uint32_t gfxFillReqCnt;          /**< @brief Number of MSYS_FillBuffer requests made */
    uint32_t gfxFillDoneCnt;         /**< @brief Number of MSYS_FillBuffer requests serviced successfully */
    uint32_t gfxFillCnclCnt;         /**< @brief Number of MSYS_FillBuffer requests cancelled */
    uint32_t gfxNextBufIdx;          /**< @brief Index to the next buffer to be used */
} VGCC_CompType;

/**
    @brief Global Component information

    Global variable to hold component information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static VGCC_CompType VGCC_Comp COMP_SECTION(".data.display")= {
    .state = VGCC_STATE_RESET,
    .width = 0UL,
    .height = 0UL,
    .vidHeight = 0UL,
    .gfxHeight = 0UL,
    .stopInProgress = 0UL,
    .videoBuf.addr = NULL,
    .videoBuf.idx = 0xFFFFFFFFUL,
    .dropsDetected = 0UL,
    .vidFbdIdx = 0UL,
    .videoNextFillReqIdx = 0UL,
    .videoPendingReq = 0UL,
    .videoUseThisBuffer = 0UL,
    .gfxFillReqCnt = 0UL,
    .gfxFillDoneCnt = 0UL,
    .gfxFillCnclCnt = 0UL,
    .gfxNextBufIdx = 0UL,
};

/**
    @brief  Structure to define Buffer information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
typedef struct _VGCC_BufInfoType {
    uint64_t timestamp; /**< @brief Timestamp */
    uint32_t mbRowNum;  /**< @brief Macroblock Row number */
    uint32_t bufIdx;    /**< @brief Buffer Index */
    uint32_t qIdx;      /**< @brief Queue Index */
    uint8_t* addr;      /**< @brief Address */
    uint32_t frameNum;  /**< @brief Frame number */
    uint16_t frameStart;/**< @brief Frame start */
    uint16_t frameEnd;  /**< @brief Frame end */
} VGCC_BufInfoType;

/**
    @brief Initialise VGC component buffer information
    Macro to initialise the buffer information to default values

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_INIT_BUF_INFO(aBufInfo)    \
{                                       \
    aBufInfo.timestamp = 0ULL;          \
    aBufInfo.mbRowNum = 0UL;            \
    aBufInfo.bufIdx = 0UL;              \
    aBufInfo.qIdx = 0UL;                \
    aBufInfo.addr = NULL;               \
    aBufInfo.frameNum = 0UL;            \
    aBufInfo.frameStart = 0U;           \
    aBufInfo.frameEnd = 0U;             \
}                                       \

/**
    @brief Macro function to initialize the MediaBuffer structure

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#define VGCC_CHANNEL_BUFFER_COMP(chType, aData, aSize, aWidth, aHeight, aStride)            \
{                                                                                           \
        .channelType = chType,                                                              \
        .data = aData,                                                                      \
        .size = aSize,                                                                      \
        .attribs = {aWidth, aHeight, aStride, 0UL, 0UL}                                     \
}

/**
    @brief Global Command messages

    Array for commands to VGC component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static MSYS_CmdType VGCC_CmdMsg[MSGQ_SIZE] COMP_SECTION(".bss.display");

/** @brief Get Command Message

    GetMessage function to get an indexed command message

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      i       Index of message

    Return values are documented in reverse-chronological order
    @retval     #NULL       Parameter 'i' is greater than #MSGQ_SIZE
    @retval     !NULL        Pointer to i-th command message

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    if (MSGQ_SIZE > i)
        return &VGCC_CmdMsg[i]
    else return NULL
    @endcode

*/
static void * VGCC_GetCmdMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &VGCC_CmdMsg[i];
    }

    return msg;
}

/**
    @brief Global Command message queue

    Define the command queue for the VGC component, on which framework shall post different commands for the VGC component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
MSGQ_DEFINE_V2 (VGCC_CmdQ, VGCC_Task, VGCC_CMDQ_EVENT, MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType,
                VGCC_CmdMsgHdrQ, VGCC_GetCmdMsg, ".data.display");


#ifdef ENABLE_VGCC_OUTPUT_PORT
/**
    @brief Global Media buffer

    Media buffer for output

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static MSYS_BufferType VGCC_MediaBuffer[MSGQ_SIZE] COMP_SECTION(".bss.display");

/**
   @brief Component Video Buffer

    Buffer for Blend Video Component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static uint8_t VGCC_MwbVid[(VGCC_INPORT_VIDEO_BUFFERS * VGC_WIDTH * VGC_HEIGHT * VGC_VID_BPP) / 8UL]
                COMP_SECTION(".vgc.VGCC_MwbVid") COMP_ALIGN(256);

/**
    @brief Component gfx Buffer

    Buffer for Blend gfx Component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static uint8_t VGCC_MwbGfx[(VGCC_TOTAL_GRAPHICS_BUFFERS * VGC_WIDTH * VGC_HEIGHT * VGC_GFX_BPP) / 8UL]
                COMP_SECTION(".vgc.cached.VGCC_MwbGfx")  COMP_ALIGN(256);


/**
    @brief Video Memory size per frame

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
const uint32_t VGCC_MwbVidSz = sizeof(VGCC_MwbVid) / VGCC_INPORT_VIDEO_BUFFERS;

/**
    @brief Graphics Memory size per frame

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
const uint32_t VGCC_MwbGfxSz = sizeof(VGCC_MwbGfx) / VGCC_TOTAL_GRAPHICS_BUFFERS;

/** @brief Get Data Message

    GetMessage function to get an indexed data message

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      i       Index of message

    Return values are documented in reverse-chronological order
    @retval     #NULL       Parameter 'i' is greater than #MSGQ_SIZE
    @retval     !NULL        Pointer to i-th data message

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    if (MSGQ_SIZE > i)
        return &VGCC_MediaBuffer[i]
    else return NULL
    @endcode

*/
static void *VGCC_GetDataMsg(uint32_t i)
{
    void *msg = NULL;
    if (MSGQ_SIZE > i) {
        msg = (void *) &VGCC_MediaBuffer[i];
    }
    return msg;
}

/**
    @brief Global Output Media port

    Output Media Port

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
MSYS_DECLARE_OUT_PORT_V2(VGCC_OutPort, 0UL, VGCC_DataMsgHdrQ,
                          VGCC_Task, VGCC_GetDataMsg,
                          MSYS_COMP_ID_VGCC, SystemEvent8, ".data.display");

/**
    @brief Global output media port configuration

    Output Media Port Configuration

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
#endif


/**
    @code{.c}
    if (aCmd is not NULL) and (aCmd->compID is MSYS_COMP_ID_VGCC) and (aHdr is not NULL)
#ifdef ENABLE_VGCC_OUTPUT_PORT
        if (aCmd->cmd is MSYS_COMP_CMD_INIT)
            Activate Task VGCC_Task
#endif
        return MSGQ_SendMsg(&VGCC_CmdQ, (const void * const)aCmd, aMask, aHdr);
    return BCM_ERR_INVAL_PARAMS
    @endcode

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static int32_t VGCC_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
                      const MSGQ_MsgHdrType** const aHdr)
{
    int32_t retVal;

    if ((NULL != aCmd) && (MSYS_COMP_ID_VGCC == aCmd->compID) && (NULL != aHdr)) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            BCM_ActivateTask(VGCC_Task);
        }
#endif        /* Post the command to the VGC component's command queue */
        retVal = MSGQ_SendMsg(&VGCC_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/**
    @code{.c}
    if ((aHdr is not NULL) && (aCmd is not NULL))
        return MSGQ_RecvMsg(&VGCC_CmdQ, aHdr, (void* const)aCmd);
    else
        return BCM_ERR_INVAL_PARAMS;
    @endcode

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static int32_t VGCC_RecvResp(const MSGQ_MsgHdrType* const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        /* Receive response from the VGC component's command queue */
        retVal = MSGQ_RecvMsg(&VGCC_CmdQ, aHdr, (void* const)aCmd);
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    return retVal;
}

/**
    @brief Framework shall use this API to get the number of output ports for this VGCC
    @code{.c}
#ifdef ENABLE_VGCC_OUTPUT_PORT
    return 1UL;
#else
    return 0UL;
#endif
    @endcode

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/

/**
    @code{.c}
#ifdef ENABLE_VGCC_OUTPUT_PORT
    if (aIndex is 0UL)
        return &VGCC_OutPort;
#endif
    return NULL
    @endcode

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static const MSYS_OutPortType* VGCC_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *port = NULL;
#ifdef ENABLE_VGCC_OUTPUT_PORT
    if (0UL == aIndex) {
        port = &VGCC_OutPort;
    }
#endif

    return port;
}

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static const MSYS_CompFnTblType VGCC_CompFnTable =
{
    VGCC_SendCmd,
    VGCC_RecvResp,
    VGCC_GetMediaOutPort,
    NULL,
};

/**
    @brief todo

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
const MSYS_CompFnTblType* VGCC_GetCompFnTable(void)
{
    return &VGCC_CompFnTable;
}

#ifdef ENABLE_VIDEO_PORT
/**
    @brief Global Video buffer information

    Array of Video Buffer information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static VGCC_BufInfoType VGCC_VideoBufInfo[VGCC_INPORT_VIDEO_BUFFERS] COMP_SECTION(".bss.display");

/**
    @brief Global video free buffer circular queue

    Circular queue to maintain Video Fill buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_VidFreeQ COMP_SECTION(".bss.display");

/**
    @brief Global video Filled buffer circular queue

    Circular queue to maintain Video Filled buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_VidFilledQ COMP_SECTION(".bss.display");

/**
    @brief Global video Blend buffer circular queue

    Circular queue to maintain Video Blend buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_VidBlendQ COMP_SECTION(".bss.display");

/** @brief Fill Video buffer done callback

    This callback shall be invoked from framework to this VGC component to notify completion of video buffer

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      aMediaBuffer       Pointer to filled media buffer

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    if ((aMediaBuffer is not NULL) and (aMediaBuffer->numChannels is VGCC_CHANNEL_MAX))
        if (aMediaBuffer->response is BCM_ERR_OK) {
            if (aMediaBuffer->buffer[0UL].size is not 0UL) {
                bufIdx = aMediaBuffer->bufIdx;
#ifdef ENABLE_VGCC_OUTPUT_PORT
                VGCC_INIT_BUF_INFO(VGCC_VideoBufInfo[bufIdx])
                VGCC_VideoBufInfo[bufIdx].mbRowNum = aMediaBuffer->seqNum
                VGCC_VideoBufInfo[bufIdx].frameNum = aMediaBuffer->frameNum
                VGCC_VideoBufInfo[bufIdx].frameStart = aMediaBuffer->frameStart
                VGCC_VideoBufInfo[bufIdx].frameEnd = aMediaBuffer->frameEnd
                VGCC_VideoBufInfo[bufIdx].bufIdx = aMediaBuffer->bufIdx
                VGCC_VideoBufInfo[bufIdx].timestamp = aMediaBuffer->timestamp
                VGCC_VideoBufInfo[bufIdx].qIdx = GVGCC_Comp.videoBuf.idx;
                VGCC_VideoBufInfo[bufIdx].addr = GVGCC_Comp.videoBuf.addr;
                retVal = LWQ_Push(&VGCC_VidFilledQ, bufIdx)
                if (retVal is not BCM_ERR_OK)
                    Log error
#else
                surfAttr = GVGCC_Comp.videoSurfAttr;
                retVal = VGC_QueueBuffer(&surfAttr, &GVGCC_Comp.videoBuf, NULL, NULL);
                if (retVal is not BCM_ERR_OK)
                    Log error
                retVal = LWQ_Push(&VGCC_VidFreeQ, bufIdx)
                if (retVal is not BCM_ERR_OK)
                    Log error
#endif
                GVGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
                GVGCC_Comp.videoBuf.addr = NULL;

                VGCC_FillVideoBuffer();
    @endcode

*/
static void VGCC_FillVideoBufferDone(const MSYS_InPortType *const aInPort,
                                                MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal;

    VGCC_Comp.videoPendingReq--;
    if ((NULL != aMediaBuffer)
        && (VGCC_CHANNEL_MAX == aMediaBuffer->numChannels)) {
        if (BCM_ERR_OK == aMediaBuffer->response) {
            if (0UL != aMediaBuffer->buffer[0UL].size) {
                /* Submit to display */
                uint8_t bufIdx = (uint8_t)aMediaBuffer->bufIdx;
                VGCC_VideoBufInfo[bufIdx].mbRowNum = aMediaBuffer->seqNum;
                VGCC_VideoBufInfo[bufIdx].frameNum = aMediaBuffer->frameNum;
                VGCC_VideoBufInfo[bufIdx].frameStart = aMediaBuffer->frameStart;
                VGCC_VideoBufInfo[bufIdx].frameEnd = aMediaBuffer->frameEnd;
                VGCC_VideoBufInfo[bufIdx].bufIdx = aMediaBuffer->bufIdx;
                VGCC_VideoBufInfo[bufIdx].timestamp = aMediaBuffer->timestamp;
                retVal = LWQ_Push(&VGCC_VidFilledQ, bufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
#ifdef VGCC_VideoPortSe
                retVal = WDS_NotifyState(&VGCC_VideoPortSe, aMediaBuffer->seqNum);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
#endif
            } else {
                uint8_t bufIdx = (uint8_t)aMediaBuffer->bufIdx;
#ifndef ENABLE_VGCC_OUTPUT_PORT
                VGCC_Comp.videoUseThisBuffer = 0UL;
#endif
                retVal = LWQ_Push(&VGCC_VidFreeQ, bufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
            }
        }
    }
}

/**
    @brief Global input media port0

    Define video input port for this VGC component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
MSYS_DECLARE_IN_PORT_V2(VGCC_VideoInPort, 0UL, MSYS_COMP_ID_VGCC, SystemEvent4,
                        VGCC_VideoInPortRW, VGCC_FillVideoBufferDone, ".data.display");

/**
    @code{.unparsed}
    if ((aIdx is less than VGCC_TOTAL_VIDEO_BUFFERS) and (VGCC_Comp.stopInProgress is 0UL))
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
            .numChannels = VGCC_CHANNEL_MAX,
            .bufIdx = aIdx,
            .seqNum = 0UL,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_Y,
            .buffer[0].size = VGCC_Comp.width * VGC_IL_LINES_PER_MB,
            .buffer[0].data = &VGCC_VideoData[aIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = VGCC_Comp.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = VGC_IL_LINES_PER_MB,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = VGCC_Comp.width,

            .buffer[1].channelType = MSYS_CHANNEL_TYPE_U,
            .buffer[1].size = (VGCC_Comp.width/2) * 8UL,
            .buffer[1].data = &VGCC_VideoData[aIdx][VGCC_Comp.width * VGC_IL_LINES_PER_MB],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = VGCC_Comp.width/2,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 8UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = VGCC_Comp.width/2,

            .buffer[2].channelType = MSYS_CHANNEL_TYPE_V,
            .buffer[2].size = (VGCC_Comp.width/2) * 8UL,
            .buffer[2].data = &VGCC_VideoData[aIdx][(VGCC_Comp.width * VGC_IL_LINES_PER_MB)
                                + (VGCC_Comp.width * 4UL)],
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = VGCC_Comp.width/2,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 8UL,
            .buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = VGCC_Comp.width/2,
        };
        MSYS_FillBuffer(&VGCC_VideoInPort, &mediaBuffer)

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @endcode
*/
static void VGCC_FillVideoBuffer(void)
{
    int32_t retVal = BCM_ERR_OK;
    uint8_t vidBufIdx = 0x0;

    if (0UL == VGCC_Comp.stopInProgress) {
        if ((0UL != LWQ_Length(VGCC_VidFreeQ)) &&
            (0xFFFFFFFFUL != VGCC_Comp.videoBuf.idx)) {
#ifndef ENABLE_VGCC_OUTPUT_PORT
#ifndef ENABLE_VGCC_FRAME
            if ((0UL == VGCC_Comp.videoNextFillReqIdx) && (0UL != VGCC_Comp.vidFbdIdx)) {
                retVal = BCM_ERR_INVAL_STATE;
            }
#endif
#endif
            if (BCM_ERR_OK == retVal) {
                retVal = LWQ_Pop(&VGCC_VidFreeQ,&vidBufIdx);
                VGCC_INIT_BUF_INFO((VGCC_VideoBufInfo[vidBufIdx]));
#ifdef ENABLE_VGCC_OUTPUT_PORT
                VGCC_VideoBufInfo[vidBufIdx].qIdx = vidBufIdx;
                VGCC_VideoBufInfo[vidBufIdx].addr = &VGCC_MwbVid[vidBufIdx * VGCC_MwbVidSz];
#else
                VGCC_VideoBufInfo[vidBufIdx].qIdx = VGCC_Comp.videoBuf.idx;
                VGCC_VideoBufInfo[vidBufIdx].addr = VGCC_Comp.videoBuf.addr;
#endif
                VGCC_VideoBufInfo[vidBufIdx].mbRowNum = VGCC_Comp.videoNextFillReqIdx;
            }
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }

        if (BCM_ERR_OK == retVal){
            const MSYS_BufferType mediaBuffer = {
                .timestamp = 0ULL,
                .version = sizeof(MSYS_BufferType),
                .format = MSYS_BUFFER_FORMAT_VIDEO_YUV_420P,
                .numChannels = VGCC_CHANNEL_MAX,
                .response = BCM_ERR_UNKNOWN,
                .seqNum = 0UL,
                .frameNum = 0UL,
                .syncStart = 0UL,
                .syncEnd = 0UL,
                .frameStart = 0U,
                .frameEnd = 0U,
                .bufIdx = vidBufIdx,

                .buffer = {
                    VGCC_CHANNEL_BUFFER_COMP(
                        MSYS_CHANNEL_TYPE_Y,
                        (VGCC_VideoBufInfo[vidBufIdx].addr +
                            (VGCC_Comp.videoNextFillReqIdx * VGCC_Comp.width * VGC_IL_LINES_PER_MB)),
                        (VGCC_Comp.width * VGCC_Comp.vidHeight),
                        VGCC_Comp.width,
                        VGCC_Comp.vidHeight,
                        VGCC_Comp.width
                        ),
                    VGCC_CHANNEL_BUFFER_COMP(
                        MSYS_CHANNEL_TYPE_U,
                        (VGCC_VideoBufInfo[vidBufIdx].addr +
                            (VGCC_Comp.width * VGCC_Comp.vidHeight) +
                                (VGCC_Comp.videoNextFillReqIdx * (VGC_IL_LINES_PER_MB / 2UL) * (VGCC_Comp.width / 2UL))),
                        (VGCC_Comp.width / 2UL) * (VGCC_Comp.vidHeight / 2UL),
                        (VGCC_Comp.width / 2UL),
                        (VGCC_Comp.vidHeight / 2UL),
                        VGCC_Comp.width / 2UL
                        ),
                    VGCC_CHANNEL_BUFFER_COMP(
                        MSYS_CHANNEL_TYPE_V,
                        (VGCC_VideoBufInfo[vidBufIdx].addr +
                            (VGCC_Comp.width * VGCC_Comp.vidHeight) +
                                ((VGCC_Comp.width * VGCC_Comp.vidHeight) / 4UL) +
                                    (VGCC_Comp.videoNextFillReqIdx * (VGC_IL_LINES_PER_MB / 2UL) * (VGCC_Comp.width / 2UL))),
                        (VGCC_Comp.width / 2UL) * (VGCC_Comp.vidHeight / 2UL),
                        (VGCC_Comp.width / 2UL),
                        (VGCC_Comp.vidHeight / 2UL),
                        (VGCC_Comp.width / 2UL)
                        ),
                    VGCC_CHANNEL_BUFFER_COMP(MSYS_CHANNEL_TYPE_INVALID, (NULL), (0UL), 0UL, 0UL, 0UL)
                },

                .streamInfo = {
                    MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX,
                    MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX
                }
            };
            (void)MSYS_FillBuffer(&VGCC_VideoInPort, &mediaBuffer);
            VGCC_Comp.videoPendingReq++;

#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
#else
#ifndef ENABLE_VGCC_FRAME
            VGCC_Comp.videoNextFillReqIdx++;
            if (VGCC_Comp.videoNextFillReqIdx == ((VGCC_Comp.height + 15UL) >> 4UL)) {
                VGCC_Comp.videoNextFillReqIdx = 0UL;
            }
#endif
#endif
        }
    }
}

#endif


#ifdef ENABLE_GRAPHICS_PORT
/**
    @brief Global graphics palette

    Array for graphics palette

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static uint32_t VGCC_PaletteData[VGCC_TOTAL_GRAPHICS_BUFFERS][256UL]
            COMP_SECTION(".vgcc.uncached.VGCC_PaletteData") COMP_ALIGN(256);

/**
    @brief Global Graphics Buffer information

    Array of Graphics Buffer information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static VGCC_BufInfoType VGCC_GfxBufInfo[VGCC_TOTAL_GRAPHICS_BUFFERS] COMP_SECTION(".bss.display");

/**
    @brief Global graphics fill buffer circular queue

    Circular queue to maintain Graphics Fill buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_GfxFreeQ COMP_SECTION(".bss.display");
/**
    @brief Global graphics Filled buffer circular queue

    Circular queue to maintain Graphics Filled buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_GfxFilledQ COMP_SECTION(".bss.display");

/**
    @brief Global Graphics Blend buffer circular queue

    Circular queue to maintain Graphics Blend buffers

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
static LWQ_BufferType VGCC_GfxBlendQ COMP_SECTION(".bss.display");

/** @brief Fill graphics buffer done callback

    This callback shall be invoked from framework to this VGC component to notify completion of graphics buffer

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      aMediaBuffer       Pointer to filled media buffer

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    if ((aMediaBuffer is not NULL) and (aMediaBuffer->numChannels is 2UL))
        if (aMediaBuffer->response is BCM_ERR_OK)
            if ((aMediaBuffer->buffer[0UL].size is not 0UL))
                uint8_t bufIdx = aMediaBuffer->bufIdx;
                VGCC_INIT_BUF_INFO(VGCC_GfxBufInfo[bufIdx]);
                VGCC_GfxBufInfo[bufIdx].mbRowNum = aMediaBuffer->seqNum;
                VGCC_GfxBufInfo[bufIdx].bufIdx = aMediaBuffer->bufIdx;
                retVal = LWQ_Push(&VGCC_LWQGfxFill, (char*)&bufIdx, 1UL);
                if (retVal != BCM_ERR_OK)
                    Log error
                Increment VGCC_Comp.gfxFillDoneCnt
            else
                Increment VGCC_Comp.gfxFillCnclCnt
    @endcode

*/
static void VGCC_FillGfxBufferDone(const MSYS_InPortType *const aInPort,
                                             MSYS_BufferType *const aMediaBuffer)
{
    int32_t retVal;
#ifndef ENABLE_VGCC_OUTPUT_PORT
    VGC_BufferType gfxBuf;
#endif

    if ((NULL != aMediaBuffer)
        && (2UL == aMediaBuffer->numChannels)) {
        if (BCM_ERR_OK == aMediaBuffer->response) {
            uint8_t bufIdx = (uint8_t)aMediaBuffer->bufIdx; /* Use the same index to store bufInfo */
            if (0UL != aMediaBuffer->buffer[0UL].size) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
                VGCC_GfxBufInfo[bufIdx].mbRowNum = aMediaBuffer->seqNum;
                VGCC_GfxBufInfo[bufIdx].bufIdx = aMediaBuffer->bufIdx;
                retVal = LWQ_Push(&VGCC_GfxFilledQ, bufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
#else
                const VGC_SurfAttrType surfAttr = VGCC_Comp.gfxSurfAttr;
                (void)VGC_LoadPalette(&VGCC_PaletteData[bufIdx][0UL], 64UL);

                gfxBuf.idx = VGCC_GfxBufInfo[bufIdx].qIdx;
                gfxBuf.addr = VGCC_GfxBufInfo[bufIdx].addr;
                retVal = VGC_QueueBuffer(NULL, NULL, &surfAttr, &gfxBuf);
                if (BCM_ERR_OK != retVal) {
                    /* Report error */
                }
                retVal = LWQ_Push(&VGCC_GfxFreeQ, (uint8_t)bufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
#endif
                VGCC_Comp.gfxFillDoneCnt++;
            } else {
#ifndef ENABLE_VGCC_OUTPUT_PORT
                gfxBuf.idx = VGCC_GfxBufInfo[bufIdx].qIdx;
                gfxBuf.addr = VGCC_GfxBufInfo[bufIdx].addr;
                /* Buffer size 0 implies that this buffer was cancelled */
                retVal = VGC_ReleaseBuffer(VGC_DISP_SURFACE_GFX, &gfxBuf);
                if (BCM_ERR_OK != retVal) {
                }
#endif /* ENABLE_VGCC_OUTPUT_PORT not defined */
                VGCC_Comp.gfxFillCnclCnt++;

                retVal = LWQ_Push(&VGCC_GfxFreeQ, (uint8_t)bufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
            }
        }
    }
}

/**
    @brief Global input media port1

    Define graphics input port for this VGC component

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
MSYS_DECLARE_IN_PORT_V2(VGCC_GfxInPort, 1UL, MSYS_COMP_ID_VGCC, SystemEvent5,
                        VGCC_GfxInPortRW, VGCC_FillGfxBufferDone, ".data.display");

/** @brief graphics buffer fill reqeust

    Compose MSYS_BufferType structure and request framework to fill graphics data

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      aIdx       Index of buffer

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    if ((aIdx is less than VGCC_TOTAL_GRAPHICS_BUFFERS) and (VGCC_Comp.stopInProgress is 0UL))
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = VGC_GRAPHICS_MEDIA_FORMAT,
            .numChannels = 2UL,
            .bufIdx = aIdx,
            .seqNum = 0UL,

            .buffer[0].channelType = MSYS_CHANNEL_TYPE_PALETTE,
            .buffer[0].size = VGCC_Comp.width * VGC_IL_LINES_PER_MB * VGC_GFX_BPP/8UL,
            .buffer[0].data = &VGCC_GfxData[aIdx][0UL],
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = VGCC_Comp.width,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = VGC_IL_LINES_PER_MB,
            .buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = VGCC_Comp.width * VGC_GFX_BPP/8UL,

            .buffer[1].channelType = MSYS_CHANNEL_TYPE_ARGB,
            .buffer[1].size = 1024UL,
            .buffer[1].data = &VGCC_PaletteData[aIdx][0UL],
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_WIDTH] = 0UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT] = 0UL,
            .buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE] = 0UL,
        };
        MSYS_FillBuffer(&VGCC_GfxInPort, &mediaBuffer);
        Increment VGCC_Comp.gfxFillReqCnt
    @endcode

*/
static void VGCC_FillGfxBuffer(void)
{
    uint8_t gfxBufIdx = 0;
    int32_t retVal = BCM_ERR_UNINIT;

    /* dequeue buffer if unavilable */
    if (0UL == VGCC_Comp.stopInProgress) {
        if (0UL != LWQ_Length(VGCC_GfxFreeQ)) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
            /* Get a buffer to store the palatte data */
            retVal = LWQ_Pop(&VGCC_GfxFreeQ, &gfxBufIdx);
            VGCC_INIT_BUF_INFO((VGCC_GfxBufInfo[gfxBufIdx]));
            VGCC_GfxBufInfo[gfxBufIdx].qIdx = gfxBufIdx;
            VGCC_GfxBufInfo[gfxBufIdx].addr = &VGCC_MwbGfx[gfxBufIdx * VGCC_MwbGfxSz];
            retVal = BCM_ERR_OK;
#else
            VGC_BufferType gfxBuf;
            retVal = VGC_DequeueBuffer(VGC_DISP_SURFACE_GFX,
                        VGCC_Comp.gfxSurfAttr.window.buffRect.width*VGCC_Comp.gfxSurfAttr.window.buffRect.height*VGC_GFX_BPP/8UL,
                        &gfxBuf);
            if (BCM_ERR_OK == retVal) {
                /* Get a buffer to store the palatte data */
                retVal = LWQ_Pop(&VGCC_GfxFreeQ, &gfxBufIdx);
                VGCC_INIT_BUF_INFO((VGCC_GfxBufInfo[gfxBufIdx]));
                VGCC_GfxBufInfo[gfxBufIdx].qIdx = gfxBuf.idx;
                VGCC_GfxBufInfo[gfxBufIdx].addr = gfxBuf.addr;
            }
#endif
        } else {
            retVal = BCM_ERR_INVAL_STATE;
        }

    }
    if (BCM_ERR_OK == retVal){
        const MSYS_BufferType mediaBuffer = {
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
#if (4UL == VGC_GFX_BPP)
            .format = MSYS_BUFFER_FORMAT_GFX_P4_ARGB,
#elif (32UL == VGC_GFX_BPP)
            .format = MSYS_BUFFER_FORMAT_GFX_ARGB8888,
#else
#error "Unsupported format"
#endif
            .numChannels = 2UL,
            .bufIdx = gfxBufIdx,
            .seqNum = 0UL,
            .frameNum = 0UL,
            .syncStart = 0UL,
            .syncEnd = 0UL,
            .frameStart = 0U,
            .frameEnd = 0U,

            .buffer = {
#if (4UL == VGC_GFX_BPP)
                VGCC_CHANNEL_BUFFER_COMP(
                    MSYS_CHANNEL_TYPE_PALETTE,
                    (VGCC_GfxBufInfo[gfxBufIdx].addr),
                    (VGCC_Comp.width * VGCC_Comp.vidHeight * VGC_GFX_BPP/8UL),
                    VGCC_Comp.width,
                    VGCC_Comp.vidHeight,
                    VGCC_Comp.width * VGC_GFX_BPP/8UL
                    ),
#elif (32UL == VGC_GFX_BPP)
                VGCC_CHANNEL_BUFFER_COMP(
                    MSYS_CHANNEL_TYPE_ARGB,
                    (VGCC_GfxBufInfo[gfxBufIdx].addr),
                    (VGCC_Comp.width * VGCC_Comp.vidHeight * VGC_GFX_BPP/8UL),
                    VGCC_Comp.width,
                    VGCC_Comp.vidHeight,
                    VGCC_Comp.width * VGC_GFX_BPP/8UL
                    ),
#else
#error "Unsupported format"
#endif
                VGCC_CHANNEL_BUFFER_COMP(
                    MSYS_CHANNEL_TYPE_ARGB,
                    ((uint8_t*)&VGCC_PaletteData[gfxBufIdx][0UL]),
                    (1024UL),
                    0UL,
                    0UL,
                    0UL
                    ),
                VGCC_CHANNEL_BUFFER_COMP(MSYS_CHANNEL_TYPE_INVALID, (NULL), (0UL), 0UL, 0UL, 0UL),
                VGCC_CHANNEL_BUFFER_COMP(MSYS_CHANNEL_TYPE_INVALID, (NULL), (0UL), 0UL, 0UL, 0UL)
            },
            .streamInfo = {
                MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX,
                MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX, MSYS_STREAM_INFO_MAX
            }
        };
        (void)MSYS_FillBuffer(&VGCC_GfxInPort, &mediaBuffer);
        VGCC_Comp.gfxFillReqCnt++;
    }
}
#endif

#ifdef ENABLE_VGCC_OUTPUT_PORT
/**
    @brief Global VGC configuration

    VGC driver configuration information

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT
*/
const static VGC_MwbCfgType VGCC_Cfg = {
    .cmp = {
        .bg = {
            .alpha = 0x00,
            .y_r   = 0x00,
            .cb_g  = 0x80,
            .cr_b  = 0x80,
            .fmt = VGC_PIXFMT_YUV444P,
        },
        .panel = {
            .width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH,
            .height = VGC_IL_LINES_PER_MB,
        },
    },
    .gfd = {
        .csc = {
            .c00 = 0x0840,
            .c01 = 0x1020,
            .c02 = 0x0320,
            .c03 = 0x0200,
            .c04 = 0x0000,
            .c10 = 0xFB40,
            .c11 = 0xF6C0,
            .c12 = 0x0E00,
            .c13 = 0x1000,
            .c14 = 0x0000,
            .c20 = 0x0E00,
            .c21 = 0xF440,
            .c22 = 0xFDC0,
            .c23 = 0x1000,
            .c24 = 0x0000,
        },
    },
    .mwb = {
        .fmt = VGC_PIXFMT_YUV420P,
        .width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH,
        .height = VGC_IL_LINES_PER_MB,
        .buffHeight = VGC_IL_LINES_PER_MB,
    },
    .misc = {
        .output = VGC_OUTPUT_MWB,
        .yuvToRgbBypass = TRUE,
        .lvdsCrCbSwap = TRUE,
        .iifCrCbSwap  = TRUE,
    },
};
#endif

/** @brief Reset state handler

    Helper function that handles all transitions from Reset state

    @behavior Sync, Non-reentrant

    @pre none

    @param    none

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (retVal is BCM_ERR_OK)
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        if (processCmd->cmd is MSYS_COMP_CMD_INIT)
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_MwbInit(&VGCC_Cfg);
            MSYS_OutPortInit(&VGCC_OutPort);
#endif
            VGCC_Comp.stopInProgress = 0UL;
            VGCC_Comp.state = VGCC_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        else
            processCmd->response = BCM_ERR_UNINIT;
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal is not BCM_ERR_OK)
            Log error
    @endcode

*/
static void VGCC_ResetStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the Framework if any */
    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        if (MSYS_COMP_CMD_INIT == processCmd->cmd) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_MwbInit(&VGCC_Cfg);
            MSYS_OutPortInit(&VGCC_OutPort);
#endif
            VGCC_Comp.stopInProgress = 0UL;
            VGCC_Comp.state = VGCC_STATE_INIT;
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_UNINIT;
        }
        /* Mark the command as completed */
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal != BCM_ERR_OK) {
            /* Log error */
        }
    }
}

/** @brief Init state handler

    Helper function that handles all transitions from INIT state

    @behavior Sync, Non-reentrant

    @pre none

    @param    none

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    Cmd *processCmd = NULL;

    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (retVal is BCM_ERR_OK)
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch (processCmd->cmd)
        case MSYS_COMP_CMD_CONFIG:
            BCM_MemSet(VGCC_GfxData, 0U, sizeof(VGCC_GfxData));
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGCC_Comp.width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            VGCC_Comp.height = MEDIA_SYSTEM_MAX_VIDEO_HEIGHT;
#else
            VGCC_Comp.width = DISPLAY_WIDTH;
            VGCC_Comp.height = DISPLAY_HEIGHT;
#endif
            VGCC_Comp.vidFbdIdx = 0UL;
            srcWidth = VGCC_Comp.width;

#ifdef ENABLE_VGCC_OUTPUT_PORT
            srcHeight = VGC_IL_LINES_PER_MB;
            VGCC_Comp.vidHeight = srcHeight;
#else
            srcHeight = VGCC_Comp.height;
            VGCC_Comp.vidHeight = VGCC_Comp.height;
#endif

            VGCC_Comp.gfxHeight = srcHeight;
            vidHeight = VGCC_Comp.vidHeight;
            gfxHeight = VGCC_Comp.gfxHeight;

#ifdef ENABLE_VIDEO_PORT
            VGC_EnableSurface(VGC_DISP_SURFACE_VID);
            VGCC_Comp.videoSurfAttr.window.buffRect.width = srcWidth;
            VGCC_Comp.videoSurfAttr.window.buffRect.height = vidHeight;
            VGCC_Comp.videoSurfAttr.window.surfaceOffset.x = 0UL;
            VGCC_Comp.videoSurfAttr.window.surfaceOffset.y = 0UL;
            VGCC_Comp.videoSurfAttr.window.dispRect.width = srcWidth;
            VGCC_Comp.videoSurfAttr.window.dispRect.height = srcHeight;
            VGCC_Comp.videoSurfAttr.window.canvasOffset.x = 0UL;
            VGCC_Comp.videoSurfAttr.window.canvasOffset.y = 0UL;
            VGCC_Comp.videoSurfAttr.fmt = VGC_PIXFMT_YUV420P;
            VGCC_Comp.videoSurfAttr.blend.back = VGC_BLENDFACT_ZERO;
            VGCC_Comp.videoSurfAttr.blend.front = VGC_BLENDFACT_ONE;
            VGCC_Comp.videoSurfAttr.blend.alpha = 0x0UL;
#endif

#ifdef ENABLE_GRAPHICS_PORT
            VGC_EnableSurface(VGC_DISP_SURFACE_GFX);
            VGCC_Comp.gfxSurfAttr.window.buffRect.width = srcWidth;
            VGCC_Comp.gfxSurfAttr.window.buffRect.height = gfxHeight;
            VGCC_Comp.gfxSurfAttr.window.surfaceOffset.x = 0UL;
            VGCC_Comp.gfxSurfAttr.window.surfaceOffset.y = 0UL;
            VGCC_Comp.gfxSurfAttr.window.dispRect.width = srcWidth;
            VGCC_Comp.gfxSurfAttr.window.dispRect.height = srcHeight;
            VGCC_Comp.gfxSurfAttr.window.canvasOffset.x = 0UL;
            VGCC_Comp.gfxSurfAttr.window.canvasOffset.y = 0UL;
            VGCC_Comp.gfxSurfAttr.fmt = VGC_GRAPHICS_FORMAT;
            VGCC_Comp.gfxSurfAttr.blend.back = VGC_BLENDFACT_ONE_MINUS_FRONT;
            VGCC_Comp.gfxSurfAttr.blend.front = VGC_BLENDFACT_CONST;
            VGCC_Comp.gfxSurfAttr.blend.alpha = 0xFFUL;
#endif
            VGCC_Comp.state = VGCC_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_Deinit();
#endif
            VGCC_Comp.state = VGCC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal is not BCM_ERR_OK)
            Log error
    @endcode

*/
static void VGCC_InitStateHandler(void)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint32_t srcWidth, srcHeight;
    uint32_t vidHeight, gfxHeight;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch (processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
#ifdef ENABLE_VGCC_OUTPUT_PORT
            BCM_MemSet(VGCC_MwbGfx, 0U, VGCC_TOTAL_GRAPHICS_BUFFERS * VGCC_MwbGfxSz);
            DCACHE_CleanRange((uint32_t)VGCC_MwbGfx, VGCC_TOTAL_GRAPHICS_BUFFERS * VGCC_MwbGfxSz);
            VGCC_Comp.width = MEDIA_SYSTEM_MAX_VIDEO_WIDTH;
            VGCC_Comp.height = MEDIA_SYSTEM_MAX_VIDEO_HEIGHT;
#else
            BCM_MemSet(VGC_GfxMem, 0U, VGC_GfxCount * VGC_GfxSz);
            DCACHE_CleanRange((uint32_t)VGC_GfxMem, VGC_GfxCount * VGC_GfxSz);
            VGCC_Comp.width = DISPLAY_WIDTH;
            VGCC_Comp.height = DISPLAY_HEIGHT;
#endif
            VGCC_Comp.vidFbdIdx = 0UL;
            VGCC_Comp.videoNextFillReqIdx = 0UL;
            VGCC_Comp.videoPendingReq = 0UL;
            VGCC_Comp.videoUseThisBuffer = 0UL;
            srcWidth = VGCC_Comp.width;

#ifdef ENABLE_VGCC_OUTPUT_PORT
            srcHeight = VGC_IL_LINES_PER_MB;
            VGCC_Comp.vidHeight = srcHeight;
#else
            srcHeight = VGCC_Comp.height;
            VGCC_Comp.vidHeight = VGCC_Comp.height;
#endif

            VGCC_Comp.gfxHeight = srcHeight;
            vidHeight = VGCC_Comp.vidHeight;
            gfxHeight = VGCC_Comp.gfxHeight;

#ifdef ENABLE_VIDEO_PORT
#ifndef ENABLE_VGCC_OUTPUT_PORT
            (void)VGC_EnableSurface(VGC_DISP_SURFACE_VID);
#endif
            VGCC_Comp.videoSurfAttr.window.buffRect.width = srcWidth;
            VGCC_Comp.videoSurfAttr.window.buffRect.height = vidHeight;
            VGCC_Comp.videoSurfAttr.window.surfaceOffset.x = 0UL;
            VGCC_Comp.videoSurfAttr.window.surfaceOffset.y = 0UL;
            VGCC_Comp.videoSurfAttr.window.dispRect.width = srcWidth;
            VGCC_Comp.videoSurfAttr.window.dispRect.height = srcHeight;
            VGCC_Comp.videoSurfAttr.window.canvasOffset.x = 0UL;
            VGCC_Comp.videoSurfAttr.window.canvasOffset.y = 0UL;
            VGCC_Comp.videoSurfAttr.fmt = VGC_PIXFMT_YUV420P;
            VGCC_Comp.videoSurfAttr.blend.back = VGC_BLENDFACT_ZERO;
            VGCC_Comp.videoSurfAttr.blend.front = VGC_BLENDFACT_ONE;
            VGCC_Comp.videoSurfAttr.blend.alpha = 0x0UL;
#endif

#ifdef ENABLE_GRAPHICS_PORT
#ifndef ENABLE_VGCC_OUTPUT_PORT
            (void)VGC_EnableSurface(VGC_DISP_SURFACE_GFX);
#endif
            VGCC_Comp.gfxSurfAttr.window.buffRect.width = srcWidth;
            VGCC_Comp.gfxSurfAttr.window.buffRect.height = gfxHeight;
            VGCC_Comp.gfxSurfAttr.window.surfaceOffset.x = 0UL;
            VGCC_Comp.gfxSurfAttr.window.surfaceOffset.y = 0UL;
            VGCC_Comp.gfxSurfAttr.window.dispRect.width = srcWidth;
            VGCC_Comp.gfxSurfAttr.window.dispRect.height = srcHeight;
            VGCC_Comp.gfxSurfAttr.window.canvasOffset.x = 0UL;
            VGCC_Comp.gfxSurfAttr.window.canvasOffset.y = 0UL;
#if (4UL == VGC_GFX_BPP)
            VGCC_Comp.gfxSurfAttr.fmt = VGC_PIXFMT_P4_RGB;
#elif (32UL == VGC_GFX_BPP)
            VGCC_Comp.gfxSurfAttr.fmt = VGC_PIXFMT_ARGB8888;
#else
#error "Unsupported format"
#endif
            VGCC_Comp.gfxSurfAttr.blend.back = VGC_BLENDFACT_ONE_MINUS_FRONT;
            VGCC_Comp.gfxSurfAttr.blend.front = VGC_BLENDFACT_CONST;
            VGCC_Comp.gfxSurfAttr.blend.alpha = 0xFFUL;
#endif
            VGCC_Comp.state = VGCC_STATE_READY;
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
            /* Deinitialize this component.
             */
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_Deinit();
#endif
            VGCC_Comp.state = VGCC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal != BCM_ERR_OK) {
            /* Log error */
        }
    }
}

/** @brief Ready state handler

    Helper function that handles all transitions from Ready state

    @behavior Sync, Non-reentrant

    @pre none

    @param    none

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    Cmd *processCmd = NULL;

    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (retVal is BCM_ERR_OK)
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch(processCmd->cmd)
        case MSYS_COMP_CMD_CONFIG:
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_START:
            VGCC_Comp.state = VGCC_STATE_RUNNING;
#ifdef ENABLE_VIDEO_PORT
            LWQ_Init(&VGCC_LWQVidFill);
            LWQ_Init(&VGCC_VidFilledQ);
            LWQ_Init(&VGCC_VidBlendQ);
            for (i = 0UL; i < VGCC_TOTAL_VIDEO_BUFFERS; i++)
                VGCC_FillVideoBuffer(i);
#endif
#ifdef ENABLE_GRAPHICS_PORT
            LWQ_Init(&VGCC_LWQGfxFill);
            LWQ_Init(&VGCC_GfxFilledQ);
            LWQ_Init(&VGCC_LWQGfxBlend);
            for (i = 0UL; i < VGCC_TOTAL_GRAPHICS_BUFFERS; i++)
                VGCC_FillGfxBuffer(i);
#endif

            BCM_SetEvent(VGCC_Task, SystemEvent4| SystemEvent5);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_Deinit();
#endif
            VGCC_Comp.state = VGCC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal is not BCM_ERR_OK)
            Log error
    @endcode

*/
static void VGCC_ReadyStateHandler(void)
{
    uint32_t i;
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;

    /* Try to get any command from the FrameWork if any */
    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_CONFIG:
            /* Configure this component with processCmd->CfgData.
             * In this example we do not have anything to configure.
             */
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_START:
            VGCC_Comp.state = VGCC_STATE_RUNNING;

            VGCC_Comp.vidFbdIdx = 0UL;
            VGCC_Comp.videoNextFillReqIdx = 0UL;
            VGCC_Comp.videoPendingReq = 0UL;
            VGCC_Comp.videoUseThisBuffer = 0UL;

#ifdef ENABLE_VIDEO_PORT
            LWQ_Init(&VGCC_VidFreeQ);
            LWQ_Init(&VGCC_VidFilledQ);
            LWQ_Init(&VGCC_VidBlendQ);
            for (i = 0UL; i < VGCC_INPORT_VIDEO_BUFFERS; i++) {
                (void)LWQ_Push(&VGCC_VidFreeQ, (uint8_t)i);
            }
#ifdef VGCC_VideoPortSe
            {
                WDS_ConfigType wdsCfg;
                wdsCfg.mode = WDS_MODE_LOGICAL | WDS_MODE_DEADLINE | WDS_MODE_FREQ;
                wdsCfg.deadlineInMs = 1000UL/MEDIA_SYSTEM_MAX_VIDEO_FPS;
                wdsCfg.minFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) - 1UL;
                wdsCfg.maxFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) + 1UL;
                wdsCfg.lStart = 0UL;
                wdsCfg.lEnd = ((MEDIA_SYSTEM_MAX_VIDEO_HEIGHT + 15UL) >> 4UL) - 1UL;
                BCM_MemSet(VGCC_VideoPortSe.buffer, WDS_INVALID_GRAPH_BUF_ENTRY,
                            VGCC_VideoPortSe.maxState * VGCC_VideoPortSe.maxTransitionsPerState);
                for (i = wdsCfg.lStart; i < wdsCfg.lEnd; i++) {
                    VGCC_VideoPortSe.buffer[i] = (WDS_StateType)(i + 1UL);
                }
                retVal = WDS_EnableSE(&VGCC_VideoPortSe, &wdsCfg);
                if (BCM_ERR_OK != retVal) {
                    /* Log error */
                }
            }
#endif
#ifdef ENABLE_VGCC_OUTPUT_PORT
            for (i = 0UL; i < VGCC_INPORT_VIDEO_BUFFERS; i++) {
                    VGCC_FillVideoBuffer();
            }
#else
            /* Deque buffer */
            retVal = VGC_DequeueBuffer(VGC_DISP_SURFACE_VID,
                        (VGCC_Comp.videoSurfAttr.window.buffRect.width * VGCC_Comp.videoSurfAttr.window.buffRect.height * 3UL) / 2UL,
                        &VGCC_Comp.videoBuf);
            if (BCM_ERR_OK != retVal) {
                /* Report error */
            }

            for (i = 0UL; i < VGCC_INPORT_VIDEO_BUFFERS; i++) {
                VGCC_FillVideoBuffer();
            }
#endif


#endif
#ifdef ENABLE_GRAPHICS_PORT
            LWQ_Init(&VGCC_GfxFreeQ);
            LWQ_Init(&VGCC_GfxFilledQ);
            LWQ_Init(&VGCC_GfxBlendQ);

            for (i = 0UL; i < VGCC_TOTAL_GRAPHICS_BUFFERS; i++) {
                (void)LWQ_Push(&VGCC_GfxFreeQ, (uint8_t)i);
                VGCC_FillGfxBuffer();
            }

#endif
            /* Signal the input port of this VGC component to start processing */
            (void)BCM_SetEvent(VGCC_Task, SystemEvent4| SystemEvent5);
            processCmd->response = BCM_ERR_OK;
            break;
        case MSYS_COMP_CMD_DEINIT:
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGC_Deinit();
#endif
            VGCC_Comp.state = VGCC_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            break;
        }
        retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
        if (retVal != BCM_ERR_OK) {
            /* Log error */
        }
    }
}

/** @brief Run state handler

    Helper function that handles all transitions from Run state

    @behavior Sync, Non-reentrant

    @pre none

    @param    none

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
    Cmd *processCmd = NULL
    gfxBufIdx = 0
    vidBufIdx = 0

    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (retVal is BCM_ERR_OK) {
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch(processCmd->cmd)
        case MSYS_COMP_CMD_STOP:
            VGCC_Comp.stopInProgress = 1UL;
            break;
        case MSYS_COMP_CMD_DEINIT:
            VGCC_Comp.stopInProgress = 1UL;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
            break;

    if (VGCC_STATE_RUNNING == VGCC_Comp.state)
#ifdef ENABLE_VGCC_OUTPUT_PORT
        uint32_t mediaBufferIdx;
        VGC_StateType state;
        uint32_t lineNumber;
        VGC_GetStatus(&state, &lineNumber);
        if (state is VGC_STATE_IDLE)
            retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
            if (retVal is BCM_ERR_OK)
                cnt = LWQ_Pop(&VGCC_VidBlendQ, (char*)&vidBufIdx, 1UL);
                if (cnt is 1UL)
                    VGCC_MediaBuffer[mediaBufferIdx].numChannels = 3UL;
                    VGCC_MediaBuffer[mediaBufferIdx].seqNum = VGCC_VideoBufInfo[vidBufIdx].mbRowNum;
                    VGCC_MediaBuffer[mediaBufferIdx].response = BCM_ERR_OK;
                    VGCC_MediaBuffer[mediaBufferIdx].timestamp = VGCC_VideoBufInfo[vidBufIdx].timestamp;
                    VGCC_MediaBuffer[mediaBufferIdx].frameNum = VGCC_VideoBufInfo[vidBufIdx].frameNum;
                    VGCC_MediaBuffer[mediaBufferIdx].frameStart = VGCC_VideoBufInfo[vidBufIdx].frameStart;
                    VGCC_MediaBuffer[mediaBufferIdx].frameEnd = VGCC_VideoBufInfo[vidBufIdx].frameEnd;
                    MSGQ_CompleteMsgIdx(&VGCC_OutPort.dataQ, mediaBufferIdx);
                    VGCC_FillVideoBuffer(vidBufIdx);
                cnt = LWQ_Pop(&VGCC_LWQGfxBlend, (char*)&gfxBufIdx, 1UL);
                if (cnt is 1UL)
                    VGCC_FillGfxBuffer(gfxBufIdx);

        if (state is VGC_STATE_IDLE)
            cnt = LWQ_Peek(&VGCC_VidFilledQ, (char*)&vidBufIdx, 1UL);
            if ((cnt is 1UL) and (VGCC_CheckJobStatus(&VGCC_VideoBufInfo[vidBufIdx]) is BCM_ERR_OK))
                VGC_BufferType videoBuf
                videoBuf.idx = VGCC_VideoBufInfo[vidBufIdx].qIdx;
                videoBuf.addr = VGCC_VideoBufInfo[vidBufIdx].addr;
                retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
                if (retVal is BCM_ERR_OK)
                    const VGC_SurfAttrType vSurfAttr = VGCC_Comp.videoSurfAttr;
                    cnt = LWQ_Peek(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
                    if ((cnt is 1UL) and
                        (VGCC_CheckJobStatus(&VGCC_GfxBufInfo[gfxBufIdx]) is BCM_ERR_OK) and
                        (VGCC_VideoBufInfo[vidBufIdx].mbRowNum is VGCC_GfxBufInfo[gfxBufIdx].mbRowNum))
                        const VGC_SurfAttrType gSurfAttr = VGCC_Comp.gfxSurfAttr;
                        VGC_BufferType gfxBuf;
                        gfxBuf.idx = VGCC_GfxBufInfo[gfxBufIdx].qIdx;
                        gfxBuf.addr = VGCC_GfxBufInfo[gfxBufIdx].addr;
                        VGC_LoadPalette(&VGCC_PaletteData[VGCC_GfxBufInfo[gfxBufIdx].bufIdx][0UL], 64UL);
                        retVal = VGC_MWBProcess(&vSurfAttr, &videoBuf,
                                        &gSurfAttr, &gfxBuf,
                                        VGCC_MediaBuffer[mediaBufferIdx].buffer[0UL].data);
                        if (retVal is BCM_ERR_OK)
                            LWQ_Pop(&VGCC_VidFilledQ, (char*)&vidBufIdx, 1UL);
                            LWQ_Pop(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
                            retVal = LWQ_Push(&VGCC_VidBlendQ, (char*)&vidBufIdx, 1UL);
                            if (retVal is not BCM_ERR_OK)
                                Log error
                            retVal = LWQ_Push(&VGCC_LWQGfxBlend, (char*)&gfxBufIdx, 1UL);
                            if (retVal is not BCM_ERR_OK)
                                Log error
                    else
                        retVal = VGC_MWBProcess(&vSurfAttr, &videoBuf,
                                        NULL, NULL, VGCC_MediaBuffer[mediaBufferIdx].buffer[0UL].data);
                        if (retVal is BCM_ERR_OK)
                            LWQ_Pop(&VGCC_VidFilledQ, (char*)&vidBufIdx, 1UL);
                            retVal = LWQ_Push(&VGCC_VidBlendQ, (char*)&vidBufIdx, 1UL);
                            if (retVal is not BCM_ERR_OK)
                                Log error
#else
#ifdef ENABLE_VIDEO_PORT
        cnt = LWQ_Peek(&VGCC_VidFilledQ, (char*)&vidBufIdx, 1UL);
        if ((cnt is 1UL) and (VGCC_CheckJobStatus(&VGCC_VideoBufInfo[vidBufIdx]) is BCM_ERR_OK))
            if (((VGCC_Comp.vidHeight/VGC_IL_LINES_PER_MB) -1UL) is VGCC_VideoBufInfo[vidBufIdx].mbRowNum)
                VGC_BufferType videoBuf;
                videoBuf.idx = VGCC_VideoBufInfo[vidBufIdx].qIdx;
                videoBuf.addr = VGCC_VideoBufInfo[vidBufIdx].addr;
                const VGC_SurfAttrType surfAttr = VGCC_Comp.videoSurfAttr;
                retVal = VGC_QueueBuffer(&surfAttr, &videoBuf, NULL, NULL);
                VGCC_Comp.videoBuf.idx = 0xFFFFFFFF;
                VGCC_Comp.videoBuf.addr = NULL;
                if (retVal is not BCM_ERR_OK)
                    Log error
            LWQ_Pop(&VGCC_VidFilledQ, (char*)&vidBufIdx, 1UL);
            VGCC_FillVideoBuffer(vidBufIdx);
#endif
#ifdef ENABLE_GRAPHICS_PORT
        cnt = LWQ_Peek(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
        if ((cnt is 1UL) and (VGCC_CheckJobStatus(&VGCC_GfxBufInfo[gfxBufIdx]) is BCM_ERR_OK))
            if (((VGCC_Comp.gfxHeight/VGC_IL_LINES_PER_MB) -1UL) is VGCC_GfxBufInfo[gfxBufIdx].mbRowNum)
                const VGC_SurfAttrType surfAttr = VGCC_Comp.gfxSurfAttr;
                VGC_LoadPalette(&VGCC_PaletteData[VGCC_GfxBufInfo[gfxBufIdx].bufIdx][0UL], 64UL);
                retVal = VGC_QueueBuffer(NULL, NULL, &surfAttr, &VGCC_Comp.gfxBuf);
                if (retVal is not BCM_ERR_OK)
                    Log error
                VGCC_Comp.gfxBuf.idx = 0xFFFFFFFF;
                VGCC_Comp.gfxBuf.addr = NULL;
            LWQ_Pop(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
            VGCC_FillGfxBuffer(gfxBufIdx);
#endif
#endif

#ifdef ENABLE_VIDEO_PORT
        VGCC_ProcessVideo();
#endif
#ifdef ENABLE_GRAPHICS_PORT
        VGCC_ProcessGraphics();
#endif

    if (VGCC_Comp.stopInProgress is 1UL)
        isEmpty = 1UL;
#ifdef ENABLE_VGCC_OUTPUT_PORT
        mediaBufferIdx;
#endif
#ifdef ENABLE_VIDEO_PORT
        MSYS_InPortCancelAll(&VGCC_VideoInPort);
        if (isEmpty is 1UL)
            LWQ_Empty(&VGCC_LWQVidFill, &isEmpty);
        if (isEmpty is 1UL)
            LWQ_Empty(&VGCC_VidFilledQ, &isEmpty);
        if (isEmpty is 1UL)
            LWQ_Empty(&VGCC_VidBlendQ, &isEmpty);
        if ((isEmpty is 1UL) and (VGCC_Comp.videoBuf.idx is not 0xFFFFFFFFUL))
            retVal = VGC_ReleaseBuffer(VGC_DISP_SURFACE_VID, &VGCC_Comp.videoBuf);
            if (BCM_ERR_OK is retVal) {
                VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
                VGCC_Comp.videoBuf.addr = NULL;
#endif
#ifdef ENABLE_VGCC_OUTPUT_PORT
        retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
        if (retVal is BCM_ERR_OK)
            VGCC_MediaBuffer[mediaBufferIdx].numChannels = 3UL;
            VGCC_MediaBuffer[mediaBufferIdx].response = BCM_ERR_OK;
            VGCC_MediaBuffer[mediaBufferIdx].buffer[0].size = 0UL;
            VGCC_MediaBuffer[mediaBufferIdx].buffer[1].size = 0UL;
            VGCC_MediaBuffer[mediaBufferIdx].buffer[2].size = 0UL;
            MSGQ_CompleteMsgIdx(&VGCC_OutPort.dataQ, mediaBufferIdx);
#endif
        if (isEmpty is 1UL)
#ifdef ENABLE_GRAPHICS_PORT
            uint32_t count = 0;
            MSYS_InPortCancelAll(&VGCC_GfxInPort);
            LWQ_Init(&VGCC_LWQGfxFill);
            LWQ_Init(&VGCC_LWQGfxBlend);
            do {
                count = LWQ_Peek(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
                if (count is 1UL)
                    VGC_BufferType gfxBuf;
                    gfxBuf.idx = VGCC_GfxBufInfo[gfxBufIdx].qIdx;
                    gfxBuf.addr = VGCC_GfxBufInfo[gfxBufIdx].addr;
                    retVal = VGC_ReleaseBuffer(VGC_DISP_SURFACE_GFX, &gfxBuf);
                    if (retVal is not BCM_ERR_OK)
                        Log error

                    if (VGCC_CheckJobStatus(&VGCC_GfxBufInfo[gfxBufIdx]) is not BCM_ERR_OK)
                        break;
                    else
                        count = LWQ_Pop(&VGCC_GfxFilledQ, (char*)&gfxBufIdx, 1UL);
            } while (count is 1UL);
            LWQ_Empty(&VGCC_GfxFilledQ, &isEmpty);

            VGCC_Comp.gfxBuf.idx = 0xFFFFFFFFUL;
            VGCC_Comp.gfxBuf.addr = NULL;
#endif
            if (isEmpty is 1UL)
                VGCC_Comp.state = VGCC_STATE_READY;
                VGCC_Comp.stopInProgress = 0UL;
                if (processCmd is not NULL)
                    processCmd->response = BCM_ERR_OK;
                    retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
    @endcode

*/
static void VGCC_RunStateHandler(EventMaskType mask)
{
    int32_t retVal;
    uint32_t cmdQIdx;
    MSYS_CmdType *processCmd = NULL;
    uint8_t gfxBufIdx = 0;

    /* Check if there are any commands in the command queue. If there are any,
     * process the command first.
     */
    retVal = MSGQ_GetMsgIdx(&VGCC_CmdQ, &cmdQIdx);
    if (BCM_ERR_OK == retVal) {
        processCmd = &VGCC_CmdMsg[cmdQIdx];
        switch(processCmd->cmd) {
        case MSYS_COMP_CMD_STOP:
            VGCC_Comp.stopInProgress = 1UL;
            break;
        case MSYS_COMP_CMD_DEINIT:
            VGCC_Comp.stopInProgress = 1UL;
            break;
        default:
            processCmd->response = BCM_ERR_INVAL_STATE;
            /* Mark the processed command as complete */
            retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
            break;
        }
    }

    if (VGCC_STATE_RUNNING == VGCC_Comp.state) {
        uint32_t cnt;
        uint8_t vidBufIdx = 0;
#ifdef ENABLE_VGCC_OUTPUT_PORT
        uint32_t mediaBufferIdx;
        VGC_StateType state;
        uint32_t lineNumber;
#endif

#ifdef ENABLE_VIDEO_PORT
       (void)MSYS_InPortProcess(&VGCC_VideoInPort);
#endif
#ifdef ENABLE_GRAPHICS_PORT
       (void)MSYS_InPortProcess(&VGCC_GfxInPort);
#endif

#ifdef ENABLE_VGCC_OUTPUT_PORT
        /* check hardware status and service outport*/
        VGC_GetStatus(&state, &lineNumber);
        if (VGC_STATE_IDLE == state) {
            retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                if (0UL != LWQ_Length(VGCC_VidBlendQ)) {
                    LWQ_Pop(&VGCC_VidBlendQ, &vidBufIdx);
                    VGCC_MediaBuffer[mediaBufferIdx].numChannels = 3UL;
                    VGCC_MediaBuffer[mediaBufferIdx].seqNum = VGCC_VideoBufInfo[vidBufIdx].mbRowNum;
                    VGCC_MediaBuffer[mediaBufferIdx].response = BCM_ERR_OK;
                    VGCC_MediaBuffer[mediaBufferIdx].timestamp = VGCC_VideoBufInfo[vidBufIdx].timestamp;
                    VGCC_MediaBuffer[mediaBufferIdx].frameNum = VGCC_VideoBufInfo[vidBufIdx].frameNum;
                    VGCC_MediaBuffer[mediaBufferIdx].frameStart = VGCC_VideoBufInfo[vidBufIdx].frameStart;
                    VGCC_MediaBuffer[mediaBufferIdx].frameEnd = VGCC_VideoBufInfo[vidBufIdx].frameEnd;
                    MSGQ_CompleteMsgIdx(&VGCC_OutPort.dataQ, mediaBufferIdx);
                    retVal = LWQ_Push(&VGCC_VidFreeQ, vidBufIdx);
                    if (retVal != BCM_ERR_OK) {
                        /* Log error */
                    }
                    if (0UL != LWQ_Length(VGCC_GfxBlendQ)) {
                        LWQ_Pop(&VGCC_GfxBlendQ, &gfxBufIdx);
                        retVal = LWQ_Push(&VGCC_GfxFreeQ, gfxBufIdx);
                        if (retVal != BCM_ERR_OK) {
                            /* Log error */
                        }
                    }
                }
            }
        }

        /* check if new request can be submitted to hw */
        if (VGC_STATE_IDLE == state) {
            cnt = LWQ_Length(VGCC_VidFilledQ);
            if (1UL <= cnt) {
                VGC_BufferType videoBuf;
                vidBufIdx = LWQ_PeekFirst(VGCC_VidFilledQ);
                videoBuf.idx = VGCC_VideoBufInfo[vidBufIdx].qIdx;
                videoBuf.addr = VGCC_VideoBufInfo[vidBufIdx].addr;
                retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
                if (BCM_ERR_OK == retVal) {
                    const VGC_SurfAttrType vSurfAttr = VGCC_Comp.videoSurfAttr;
                    cnt = LWQ_Length(VGCC_GfxFilledQ);
                    gfxBufIdx = LWQ_PeekFirst(VGCC_GfxFilledQ);
                    if ((1UL == cnt) &&
                        (VGCC_VideoBufInfo[vidBufIdx].mbRowNum == VGCC_GfxBufInfo[gfxBufIdx].mbRowNum)) {
                        const VGC_SurfAttrType gSurfAttr = VGCC_Comp.gfxSurfAttr;
                        VGC_BufferType gfxBuf;
                        gfxBuf.idx = VGCC_GfxBufInfo[gfxBufIdx].qIdx;
                        gfxBuf.addr = VGCC_GfxBufInfo[gfxBufIdx].addr;
                        VGC_LoadPalette(&VGCC_PaletteData[VGCC_GfxBufInfo[gfxBufIdx].bufIdx][0UL], 64UL);
                        retVal = VGC_MWBProcess(&vSurfAttr, &videoBuf,
                                        &gSurfAttr, &gfxBuf,
                                        VGCC_MediaBuffer[mediaBufferIdx].buffer[0UL].data);
                        if (BCM_ERR_OK == retVal) {
                            LWQ_Pop(&VGCC_VidFilledQ, &vidBufIdx);
                            retVal = LWQ_Push(&VGCC_VidBlendQ, vidBufIdx);
                            if (retVal != BCM_ERR_OK) {
                               /* Log error */
                            }
                            LWQ_Pop(&VGCC_GfxFilledQ, &gfxBufIdx);
                            retVal = LWQ_Push(&VGCC_GfxBlendQ, gfxBufIdx);
                            if (retVal != BCM_ERR_OK) {
                               /* Log error */
                            }
                        }
                    } else {
                        retVal = VGC_MWBProcess(&vSurfAttr, &videoBuf,
                                        NULL, NULL, VGCC_MediaBuffer[mediaBufferIdx].buffer[0UL].data);
                        if (BCM_ERR_OK == retVal) {
                            LWQ_Pop(&VGCC_VidFilledQ, &vidBufIdx);
                            retVal = LWQ_Push(&VGCC_VidBlendQ, vidBufIdx);
                            if (retVal != BCM_ERR_OK) {
                               /* Log error */
                            }
                        }
                    }
                }
            }
        }
#else
        /* check hw and service */
        cnt = LWQ_Length(VGCC_VidFilledQ);
        if (1UL <= cnt) {
            vidBufIdx = LWQ_PeekFirst(VGCC_VidFilledQ);
#ifndef ENABLE_VGCC_FRAME
            if (VGCC_Comp.vidFbdIdx != VGCC_VideoBufInfo[vidBufIdx].mbRowNum) {
                VGCC_Comp.dropsDetected++;

                /* Find the number of submitted buffers */
                VGCC_Comp.videoNextFillReqIdx = VGCC_VideoBufInfo[vidBufIdx].mbRowNum + VGCC_Comp.videoPendingReq + 1UL;
                if (VGCC_Comp.videoNextFillReqIdx == ((VGCC_Comp.height + 15UL) >> 4UL)) {
                    VGCC_Comp.videoNextFillReqIdx = 0UL;
                }
                VGCC_Comp.videoUseThisBuffer = 0UL;

            } else {
                if (0UL == VGCC_VideoBufInfo[vidBufIdx].mbRowNum) {
                    VGCC_Comp.videoUseThisBuffer = 1UL;
                }
            }

            VGCC_Comp.vidFbdIdx = VGCC_VideoBufInfo[vidBufIdx].mbRowNum + 1UL;
            if (VGCC_Comp.vidFbdIdx == ((VGCC_Comp.height + 15UL) >> 4UL)) {
                VGCC_Comp.vidFbdIdx = 0UL;
                if (0UL != VGCC_Comp.videoUseThisBuffer) {
                    const VGC_SurfAttrType surfAttr = VGCC_Comp.videoSurfAttr;
                    retVal = VGC_QueueBuffer(&surfAttr, &VGCC_Comp.videoBuf, NULL, NULL);
                    if (BCM_ERR_OK != retVal) {
                        /* Report error */
                    }
                    VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
                }
            }
#else
            {
                const VGC_SurfAttrType surfAttr = VGCC_Comp.videoSurfAttr;
                retVal = VGC_QueueBuffer(&surfAttr, &VGCC_Comp.videoBuf, NULL, NULL);
                if (BCM_ERR_OK != retVal) {
                    /* Report error */
                }
                VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
            }
#endif
            retVal = LWQ_Pop(&VGCC_VidFilledQ, &vidBufIdx);
            if (retVal == BCM_ERR_OK) {
                retVal = LWQ_Push(&VGCC_VidFreeQ, vidBufIdx);
                if (retVal != BCM_ERR_OK) {
                    /* Log error */
                }
            }
        }
#endif

        /* submit fill buffer requests */
#ifdef ENABLE_VGCC_OUTPUT_PORT
        VGCC_FillGfxBuffer();
#else
        if (0UL != (mask & VGC_VSYNC_EVENT)) {
#ifdef ENABLE_GRAPHICS_PORT
            VGCC_FillGfxBuffer();
#endif
        }
#endif

#ifdef ENABLE_VIDEO_PORT
        if ((0UL == VGCC_Comp.stopInProgress) &&
            (0xFFFFFFFFUL == VGCC_Comp.videoBuf.idx)) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
            uint8_t idx = 0;
            cnt = LWQ_Length(VGCC_VidFreeQ);
            if (1UL <= cnt) {
                idx = LWQ_PeekFirst(VGCC_VidFreeQ);
                /* assign the available free buffer */
                VGCC_Comp.videoBuf.idx = idx;
                VGCC_Comp.videoBuf.addr = &VGCC_MwbVid[idx * VGCC_MwbVidSz];
            }
#else
            /* dequeue the buffer */
            retVal = VGC_DequeueBuffer(VGC_DISP_SURFACE_VID,
                        (VGCC_Comp.videoSurfAttr.window.buffRect.width * VGCC_Comp.videoSurfAttr.window.buffRect.height * 3UL) / 2UL,
                        &VGCC_Comp.videoBuf);
            if (BCM_ERR_OK != retVal){
                /* report error */
            }
#endif
        }

        if (0xFFFFFFFFUL != VGCC_Comp.videoBuf.idx) {
            VGCC_FillVideoBuffer();
        }
#endif
    }

    if (1UL == VGCC_Comp.stopInProgress) {
       volatile uint32_t isEmpty = 1UL;
#ifdef ENABLE_VGCC_OUTPUT_PORT
        uint32_t mediaBufferIdx;
#endif
#ifdef ENABLE_VIDEO_PORT
        (void)MSYS_InPortCancelAll(&VGCC_VideoInPort);
        if (0UL != LWQ_Length(VGCC_VidFilledQ)) {
            isEmpty = 0UL;
        }
        if (0UL != LWQ_Length(VGCC_VidBlendQ)) {
            isEmpty = 0UL;
        }
        if ((1UL == isEmpty) && (0xFFFFFFFFUL != VGCC_Comp.videoBuf.idx)) {
#ifdef ENABLE_VGCC_OUTPUT_PORT
            VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
            VGCC_Comp.videoBuf.addr = NULL;
#else
            retVal = VGC_ReleaseBuffer(VGC_DISP_SURFACE_VID, &VGCC_Comp.videoBuf);
            if (BCM_ERR_OK == retVal) {
                VGCC_Comp.videoBuf.idx = 0xFFFFFFFFUL;
                VGCC_Comp.videoBuf.addr = NULL;
            }
#endif
        }
#endif
#ifdef ENABLE_VGCC_OUTPUT_PORT
        /* Submit the pending buffers */
        if (1UL == isEmpty) {
            retVal = MSGQ_GetMsgIdx(&VGCC_OutPort.dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == retVal) {
                VGCC_MediaBuffer[mediaBufferIdx].numChannels = 3UL;
                VGCC_MediaBuffer[mediaBufferIdx].response = BCM_ERR_OK;
                VGCC_MediaBuffer[mediaBufferIdx].buffer[0].size = 0UL;
                VGCC_MediaBuffer[mediaBufferIdx].buffer[1].size = 0UL;
                VGCC_MediaBuffer[mediaBufferIdx].buffer[2].size = 0UL;
                MSGQ_CompleteMsgIdx(&VGCC_OutPort.dataQ, mediaBufferIdx);
            }
        }
#endif
        /* Cancel all mediabuffers and move to Ready state */
        if (1UL == isEmpty) {
#ifdef ENABLE_GRAPHICS_PORT
            (void)MSYS_InPortCancelAll(&VGCC_GfxInPort);
            while(0UL != LWQ_Length(VGCC_GfxFilledQ)) {
                retVal = LWQ_Pop(&VGCC_GfxFilledQ, &gfxBufIdx);
                if (BCM_ERR_OK == retVal) {
#ifndef ENABLE_VGCC_OUTPUT_PORT
                    VGC_BufferType gfxBuf;
                    gfxBuf.idx = VGCC_GfxBufInfo[gfxBufIdx].qIdx;
                    gfxBuf.addr = VGCC_GfxBufInfo[gfxBufIdx].addr;
                    retVal = VGC_ReleaseBuffer(VGC_DISP_SURFACE_GFX, &gfxBuf);
                    if (retVal != BCM_ERR_OK) {
                        /* Log error */
                    }
#else
                    retVal = LWQ_Push(&VGCC_GfxFreeQ, gfxBufIdx);
                    if (retVal != BCM_ERR_OK) {
                       /* Log error */
                    }

#endif /* ENABLE_VGCC_OUTPUT_PORT not defined */
                }
            }
#endif
            if (1UL == isEmpty) {
                VGCC_Comp.state = VGCC_STATE_READY;
#ifdef VGCC_VideoPortSe
                {
                    retVal = WDS_DisableSE(&VGCC_VideoPortSe);
                    if (BCM_ERR_OK != retVal) {
                        /* Log error */
                    }
                }
#endif
                VGCC_Comp.stopInProgress = 0UL;
                if (NULL != processCmd) {
                    processCmd->response = BCM_ERR_OK;
                    retVal = MSGQ_CompleteMsgIdx(&VGCC_CmdQ, cmdQIdx);
                }
            }
        }
    }
}

/** @brief VGC Component Task

    Task function which is responsible for input video and graphics buffer processing, optional output buffer processing and VGC driver configuration.

    @behavior Sync, Non-reentrant

    @pre none

    @param    none

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGCC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VGCC_MEDIA_COMPONENT

    @limitations none

    @code{.unparsed}
#ifndef ENABLE_VGCC_OUTPUT_PORT
    loop = BCM_ERR_OK;
#endif

    events = ( VGC_STOP_EVENT  | VGCC_CMDQ_EVENT | VGCC_VGCD_EVENT );
#ifdef ENABLE_VGCC_DISPLAY
    events | = VGCC_DISP_EVENTS;
#endif
#ifdef ENABLE_VIDEO_PORT
    events | = VGCC_VID_SRC_EVENTS;
#endif
#ifdef ENABLE_GRAPHICS_PORT
    events | = VGCC_GFX_SRC_EVENTS;
#endif
#ifdef ENABLE_VGCC_OUTPUT_PORT
    events | = VGCC_SINK_EVENTS;
#endif
    do {
        ret = BCM_WaitEvent(events);
        if (erikaRet is not E_OK)
            Log error

        ret = BCM_GetEvent(VGCC_Task, &eventMask);
        if (erikaRet is not E_OK)
            Log error

        ret = BCM_ClearEvent(eventMask);
        if (erikaRet is not E_OK)
            Log error

#ifndef ENABLE_VGCC_OUTPUT_PORT
        if ((eventMask & VGCC_VGCD_EVENT) is not 0UL)
            loop = VGCD_CtrlProcessMsg();
#endif
        switch(VGCC_Comp.state)
        {
        case VGCC_STATE_RESET:
            VGCC_ResetStateHandler();
            break;
        case VGCC_STATE_INIT:
            VGCC_InitStateHandler();
            break;
        case VGCC_STATE_READY:
            VGCC_ReadyStateHandler();
            break;
        case VGCC_STATE_RUNNING:
            VGCC_RunStateHandler();
            break;
        default:
            break;
        }
#ifndef ENABLE_VGCC_OUTPUT_PORT
    } while (loop is not BCM_ERR_UNINIT)
#else
    } while (VGCC_Comp.state is not VGCC_STATE_RESET)
#endif
    Terminate Task
    @endcode

*/
TASK(VGCC_Task)
{
    BCM_EventMaskType eventMask = 0UL;
    int32_t ret;
    uint32_t events;

#ifndef ENABLE_VGCC_OUTPUT_PORT
    int32_t loop = BCM_ERR_OK;
#endif

    events = ( VGC_STOP_EVENT  | VGCC_CMDQ_EVENT | VGCC_VGCD_EVENT );
#ifdef ENABLE_VGCC_DISPLAY
    events |= VGCC_DISP_EVENTS;
#endif
#ifdef ENABLE_VIDEO_PORT
    events |= VGCC_VID_SRC_EVENTS;
#endif
#ifdef ENABLE_GRAPHICS_PORT
    events |= VGCC_GFX_SRC_EVENTS;
#endif
#ifdef ENABLE_VGCC_OUTPUT_PORT
    events |= VGCC_SINK_EVENTS;
#endif
    do {
        /* Since this task only task for this component, it must wait for all
         * relevent events.
         */
        ret = BCM_WaitEvent(events);
        if (BCM_ERR_OK != ret) {
        }

        ret = BCM_GetEvent(VGCC_Task, &eventMask);
        if (BCM_ERR_OK != ret) {
        }

        ret = BCM_ClearEvent(eventMask);
        if (BCM_ERR_OK != ret) {
        }

#ifndef ENABLE_VGCC_OUTPUT_PORT
        if (0UL != (eventMask & VGCC_VGCD_EVENT)) {
            loop = VGCD_CtrlProcessMsg();
        }
#endif
        /* Call the state handling helper function depending on the state that
         * this VGC component is in.
         */
        switch(VGCC_Comp.state)
        {
        case VGCC_STATE_RESET:
            VGCC_ResetStateHandler();
            break;
        case VGCC_STATE_INIT:
            VGCC_InitStateHandler();
            break;
        case VGCC_STATE_READY:
            VGCC_ReadyStateHandler();
            break;
        case VGCC_STATE_RUNNING:
            VGCC_RunStateHandler(eventMask);
            break;
        default:
#ifndef ENABLE_VGCC_OUTPUT_PORT
        loop = BCM_ERR_UNINIT;
#else
        VGCC_Comp.state = VGCC_STATE_RESET;
#endif
            break;
        }
    /* if this component has moved to the reset state, terminate this task. */
#ifndef ENABLE_VGCC_OUTPUT_PORT
    } while (BCM_ERR_UNINIT != loop);
#else
    } while (VGCC_STATE_RESET != VGCC_Comp.state);
#endif
    (void)BCM_TerminateTask();
}

/** @} */
