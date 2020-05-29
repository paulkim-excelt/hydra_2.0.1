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
    @defgroup grp_camera_impl Implementation
    @ingroup grp_camera

    @addtogroup grp_camera_impl
    @{
    @section sec_camera_impl_overview Overview
    @image html camera_overview.jpg "Camera Media Component hierarchy"
    Above figure depicts a block diagram representation of Camera sub system.
    Camera Media Component abstracts different imaging components to provide
    YUV streaming functionality.

    @section sec_camera_impl_fsm State Machine
    @image html camera_state_machine.jpg
    Above figure depicts the state machine for Camera Media Component. It also
    depicts the commands which triggers the state transition.

    Camera Media Component has the following states:
    - @b Reset: All sub-components are in uninitialized state.
    - @b Initialized: Individual components are initialized.
    - @b Ready: Individual components are configured. Camera can be reconfigured
      in @b Ready state as well.
    - @b Running: Individual components are enabled and Camera is streaming.
    - @b Error: One or more individual components went to error state.

    @section sec_camera_impl_seq Sequence Diagrams
    @tagseq camera_impl_seq_diagram.svg "Sequence diagram"
    @startseq
    span_width = 50;
    node_width = 150;

    group {label = "client"; color = "#EFFFFF";  CAMERA_CLIENT;};
    group {label = "ipc"; color = "#FFEFFF"; CAMERA_CLIENT_IF; CAMERA_SERVER_IF;};
    group {label = "server"; color = "#FFFFEF"; CAMERA_SERVER_TASK;};
    group {label = "sub-modules"; color = "#FFFEFF"; CSN; ISYS; TUNER;};

    CAMERA_CLIENT  -> CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_INIT_CMD)"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => TUNER [label = "Tuner_Init()"];
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Init()"];
        CAMERA_SERVER_TASK => CSN [label = "CSN_Init()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_INIT_CMD)"];
    === Initialization Complete ===

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Mode config"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Configure()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with ISP Config"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Configure()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with UNICAM Config"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Configure()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Sensor Config"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "CSN_Configure()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Tuner config"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "Tuner_Configure()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    === Configuration Complete ===

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_START_CMD)"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => TUNER [label = "Tuner_Start()"];
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Start()"];
        CAMERA_SERVER_TASK => CSN [label = "CSN_Start()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_START_CMD)"];
    === Start Complete ===

    CAMERA_CLIENT  -> CAMERA_CLIENT_IF[label = "MSGQ_SendMsg(outPort->dataQ)"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_FillBuffer()"];
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_GetFilledBuffer()"];
        CAMERA_CLIENT  <- CAMERA_SERVER_TASK[label = "MSGQ_CompleteMsgIdx(outPort->dataQ)"];
    };
    === Buffer transfer complete ===

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_STOP_CMD)"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => TUNER [label = "CSN_Stop()"];
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Stop()"];
        CAMERA_SERVER_TASK => CSN [label = "Tuner_Stop()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_STOP_CMD)"];
    === Stop Complete ===

    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_SendCmd(COMP_DEINIT_CMD)"];
    CAMERA_CLIENT_IF -> CAMERA_SERVER_IF [label = "signal"];
    CAMERA_SERVER_IF -> CAMERA_SERVER_TASK [label = "signal"] {
    CAMERA_CLIENT_IF <-- CAMERA_SERVER_IF;
    CAMERA_CLIENT <-- CAMERA_CLIENT_IF;
        CAMERA_SERVER_TASK => TUNER [label = "CSN_Deinit()"];
        CAMERA_SERVER_TASK => ISYS [label = "ISYS_Deinit()"];
        CAMERA_SERVER_TASK => CSN [label = "Tuner_Deinit()"];
        CAMERA_CLIENT <-- CAMERA_SERVER_TASK [label = "Notify completion"];
    };
    CAMERA_CLIENT  => CAMERA_CLIENT_IF[label = "CAMERA_RecvResp(COMP_DEINIT_CMD)"];
    === De-initialization Complete ===
    @endseq

    @file camera_comp.c
    @brief Camera Media Component Implementation
    This source file contains the implementation of camera media component
    interfaces.
    @version 0.1 Initial version
*/

#include <string.h>
#include <bcm_err.h>
#include <bcm_time.h>
#include <camera.h>
#include <system.h>
#include <imgl.h>
#include <isys.h>
#include <cam_erika.h>
#include <camera_sensor.h>
#include <camera_sensor_osil.h>
#include <tuner.h>
#include <bcm_utils.h>
#include <osil/bcm_osil.h>
#include "camera_internal.h"
#ifdef CAMERA_TUNING
#include <cache.h>
#include <bcm_utils.h>
#include <ulog.h>
#include <pp.h>
#endif /* CAMERA_TUNING */

/**
    @name Camera API IDs
    @{
    @brief API IDs for Camera Media Component
*/

#define BRCM_SWDSGN_CAMERA_UCM_STRIDE_ALIGN_MACRO        (0x80U) /**< @brief #CAMERA_UCM_STRIDE_ALIGN         */
#define BRCM_SWDSGN_CAMERA_ISP_STRIDE1_ALIGN_MACRO       (0x81U) /**< @brief #CAMERA_ISP_STRIDE1_ALIGN        */
#define BRCM_SWDSGN_CAMERA_ISP_STRIDE2_ALIGN_MACRO       (0x82U) /**< @brief #CAMERA_ISP_STRIDE2_ALIGN        */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_422I_MACRO    (0x83U) /**< @brief #CAMERA_CALC_FRAME_SIZE_422I     */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_422P_MACRO    (0x84U) /**< @brief #CAMERA_CALC_FRAME_SIZE_422P     */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_444P_MACRO    (0x85U) /**< @brief #CAMERA_CALC_FRAME_SIZE_444P     */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_420SP_MACRO   (0x86U) /**< @brief #CAMERA_CALC_FRAME_SIZE_420SP    */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_420P_MACRO    (0x87U) /**< @brief #CAMERA_CALC_FRAME_SIZE_420P     */
#define BRCM_SWDSGN_CAMERA_CALC_FRAME_SIZE_MACRO         (0x88U) /**< @brief #CAMERA_CALC_FRAME_SIZE          */
#define BRCM_SWDSGN_CAMERA_ISP_HV_FRAME_SIZE_MACRO       (0x89U) /**< @brief #CAMERA_ISP_HV_FRAME_SIZE        */
#define BRCM_SWDSGN_CAMERA_ISP_HV_SLICE_SIZE_MACRO       (0x8AU) /**< @brief #CAMERA_ISP_HV_SLICE_SIZE        */
#define BRCM_SWDSGN_CAMERA_YUV_BUFFER_GLOBAL             (0x8BU) /**< @brief #CAMERA_YUVBufferGlobal          */
#define BRCM_SWDSGN_CAMERA_RAW_BUFFER_GLOBAL             (0x8CU) /**< @brief #CAMERA_RawBufferGlobal          */
#define BRCM_SWDSGN_CAMERA_TUNING_Y_SLICE_GLOBAL         (0x8DU) /**< @brief #CAMERA_YSliceGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_U_SLICE_GLOBAL         (0x8EU) /**< @brief #CAMERA_USliceGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_V_SLICE_GLOBAL         (0x8FU) /**< @brief #CAMERA_VSliceGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_Y_FRAME_GLOBAL         (0x90U) /**< @brief #CAMERA_YFrameGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_U_FRAME_GLOBAL         (0x91U) /**< @brief #CAMERA_UFrameGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_V_FRAME_GLOBAL         (0x92U) /**< @brief #CAMERA_VFrameGlobal             */
#define BRCM_SWDSGN_CAMERA_TUNING_CONTEXT_GLOBAL         (0x93U) /**< @brief #CAMERA_TuningStateGlobal        */
#define BRCM_SWDSGN_CAMERA_CONTEXT_GLOBAL                (0x94U) /**< @brief #CAMERA_CompStateGlobal          */
#define BRCM_SWDSGN_CAMERA_MEDIA_BUFFER_GLOBAL           (0x95U) /**< @brief #CAMERA_MediaBufferGlobal        */
#define BRCM_SWDSGN_CAMERA_HV_MEDIA_OUTPORT_GLOBAL       (0x96U) /**< @brief #CAMERA_OutPortHVGlobal          */
#define BRCM_SWDSGN_CAMERA_HV_DATA_MSG_HDRQ_GLOBAL       (0x97U) /**< @brief #CAMERA_DataMsgHdrQHVGlobal      */
#define BRCM_SWDSGN_CAMERA_CV_MEDIA_OUTPORT_GLOBAL       (0x98U) /**< @brief #CAMERA_OutPortCVGlobal          */
#define BRCM_SWDSGN_CAMERA_CV_DATA_MSG_HDRQ_GLOBAL       (0x99U) /**< @brief #CAMERA_DataMsgHdrQCVGlobal      */
#define BRCM_SWDSGN_CAMERA_AUX_MEDIA_OUTPORT_GLOBAL      (0x9AU) /**< @brief #CAMERA_OutPortAuxGlobal         */
#define BRCM_SWDSGN_CAMERA_AUX_DATA_MSG_HDRQ_GLOBAL      (0x9BU) /**< @brief #CAMERA_DataMsgHdrQAuxGlobal     */
#define BRCM_SWDSGN_CAMERA_MEDIA_OUTPORT_CFG_GLOBAL      (0x9CU) /**< @brief #CAMERA_MediaOutPortConfigGlobal */
#define BRCM_SWDSGN_CAMERA_CMD_MSG_HDRQ_GLOBAL           (0x9DU) /**< @brief #CAMERA_CmdMsgHdrQGlobal         */
#define BRCM_SWDSGN_CAMERA_CMD_MSGQ_GLOBAL               (0x9EU) /**< @brief #CAMERA_CmdMsgQGlobal            */
#define BRCM_SWDSGN_CAMERA_CMD_CMDQ_GLOBAL               (0x9FU) /**< @brief #CAMERA_CmdQGlobal               */
#define BRCM_SWDSGN_CAMERA_REPORT_ERROR_PART_PROC        (0xA0U) /**< @brief #CAMERA_ReportError              */
#define BRCM_SWDSGN_CAMERA_GET_DMSG_HV_PART_PROC         (0xA1U) /**< @brief #CAMERA_GetDataMsgHV             */
#define BRCM_SWDSGN_CAMERA_GET_DMSG_CV_PART_PROC         (0xA2U) /**< @brief #CAMERA_GetDataMsgCV             */
#define BRCM_SWDSGN_CAMERA_GET_DMSG_AUX_PART_PROC        (0xA3U) /**< @brief #CAMERA_GetDataMsgAux            */
#define BRCM_SWDSGN_CAMERA_GET_CMD_MSG_PART_PROC         (0xA4U) /**< @brief #CAMERA_GetCmdMsg                */
#define BRCM_SWDSGN_CAMERA_DEINIT_PART_PROC              (0xA5U) /**< @brief #CAMERA_Deinitialize             */
#define BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC (0xA6U) /**< @brief #CAMERA_ErrorStateHandler        */
#define BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC (0xA7U) /**< @brief #CAMERA_ResetStateHandler        */
#define BRCM_SWDSGN_CAMERA_INIT_STATE_HANDLER_PART_PROC  (0xA8U) /**< @brief #CAMERA_InitStateHandler         */
#define BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC (0xA9U) /**< @brief #CAMERA_ReadyStateHandler        */
#define BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC   (0xAAU) /**< @brief #CAMERA_RunStateHandler          */
#define BRCM_SWDSGN_CAMERA_TASK_PART_PROC                (0xABU) /**< @brief #CAMERA_Task                     */
#define BRCM_SWDSGN_CAMERA_SEND_CMD_PROC                 (0xACU) /**< @brief #CAMERA_SendCmd                  */
#define BRCM_SWDSGN_CAMERA_RECV_RESP_PROC                (0xADU) /**< @brief #CAMERA_RecvResp                 */
#define BRCM_SWDSGN_CAMERA_GET_MEDIA_OUT_PORT_PROC       (0xAEU) /**< @brief #CAMERA_GetMediaOutPort          */
#define BRCM_SWDSGN_CAMERA_GET_CFG_MEDIA_OUT_PORT_PROC   (0xAFU) /**< @brief #CAMERA_GetMediaOutPortConfig    */
#define BRCM_SWDSGN_CAMERA_PRINT_STATISTICS_PROC         (0xB0U) /**< @brief #CAMERA_PrintStatistics          */
#define BRCM_SWDSGN_CAMERA_CONFIG_DATA_TYPE              (0xB1U) /**< @brief #CAMERA_ConfigDataType           */
#define BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC       (0xB2U) /**< @brief #CAMERA_ConfigureInternal        */
#define BRCM_SWDSGN_CAMERA_SET_STATE_PART_PROC           (0xB3U) /**< @brief #CAMERA_SetState                 */
#define BRCM_SWDSGN_CAMERA_CANCEL_PORT_MSGS_PART_PROC    (0xB4U) /**< @brief #CAMERA_CancelPortMsgs           */
/** @} */

/**
    @brief DMA Invalid index

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define ISYS_DMA_INVALID_INDEX          (0xffffffffUL)

/**
    @brief Frame size calculation macro for 422I

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_CALC_FRAME_SIZE_422I(w, h, bps, align1, align2) \
((CAMERA_ALIGN((((w) * 2UL * (bps))/8UL), (align1))) * (h))

/**
    @brief Frame size calculation macro for 422P

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_CALC_FRAME_SIZE_422P(w, h, bps, align1, align2) \
(((CAMERA_ALIGN((((w) * (bps))/8UL), (align1))) * (h)) + 2UL * \
 ((CAMERA_ALIGN((((w) * (bps) / 2UL)/8UL), (align2))) * (h)))

/**
    @brief Frame size calculation macro for 444P

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_CALC_FRAME_SIZE_444P(w, h, bps, align1, align2) \
((CAMERA_ALIGN((((w) * (bps))/8UL), (align1)) * (h)) + 2UL * \
 (CAMERA_ALIGN((((w) * (bps))/8UL), (align2)) * (h)))

/**
    @brief Frame size calculation macro for 420SP

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_CALC_FRAME_SIZE_420SP(w, h, bps, align1, align2) \
((CAMERA_ALIGN((((w) * (bps))/8UL), (align1)) * (h)) + \
 (CAMERA_ALIGN((((w) * (bps))/8UL), (align2)) * (h) / 2UL))

/**
    @brief Frame size calculation macro for 420P

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_CALC_FRAME_SIZE_420P(w, h, bps, align1, align2) \
((CAMERA_ALIGN((((w) * (bps))/8UL), (align1)) * (h)) + 2UL * \
 (CAMERA_ALIGN((((w) * (bps) / 2UL)/8UL), (align2)) * (h) / 2UL))

/* Map cal size macros */
/**
    @brief Map frame size macro to format specific macro

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#if defined CAMERA_ISP_HV_OUT_FORMAT_422I
#define CAMERA_ISP_HV_CALC_FRAME_SIZE CAMERA_CALC_FRAME_SIZE_422I
#elif defined CAMERA_ISP_HV_OUT_FORMAT_422P
#define CAMERA_ISP_HV_CALC_FRAME_SIZE CAMERA_CALC_FRAME_SIZE_422P
#elif defined CAMERA_ISP_HV_OUT_FORMAT_444P
#define CAMERA_ISP_HV_CALC_FRAME_SIZE CAMERA_CALC_FRAME_SIZE_444P
#elif defined CAMERA_ISP_HV_OUT_FORMAT_420SP
#define CAMERA_ISP_HV_CALC_FRAME_SIZE CAMERA_CALC_FRAME_SIZE_420SP
#else /* CAMERA_ISP_HV_OUT_FORMAT_420P */
#define CAMERA_ISP_HV_CALC_FRAME_SIZE CAMERA_CALC_FRAME_SIZE_420P
#endif

/**
    @brief Declare frame size of HV output

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_ISP_HV_FRAME_SIZE CAMERA_ISP_HV_CALC_FRAME_SIZE( \
                                   MEDIA_SYSTEM_MAX_VIDEO_WIDTH, \
                                   MEDIA_SYSTEM_MAX_VIDEO_HEIGHT, \
                                   CAMERA_ISP_HV_OUT_BPS, \
                                   ISYS_ISP_STRIDE1_ALIGN, \
                                   ISYS_ISP_STRIDE2_ALIGN)

/**
    @brief Declare slice size of HV output

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_ISP_HV_SLICE_SIZE CAMERA_ISP_HV_CALC_FRAME_SIZE(\
                                   MEDIA_SYSTEM_MAX_VIDEO_WIDTH, \
                                   16UL, \
                                   CAMERA_ISP_HV_OUT_BPS, \
                                   ISYS_ISP_STRIDE1_ALIGN, \
                                   ISYS_ISP_STRIDE2_ALIGN)

/**
    @brief YUV internal output buffers

    Memory for internal YUV buffers

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_ISP_HV_OUT_BPS_MACRO
    @trace #BRCM_SWARCH_CAMERA_ALIGN_MACRO
*/
#if (defined CAMERA_ENABLE_DUAL_PASS && !defined CAMERA_DP_ENABLE_OPTION2)
static uint8_t CAMERA_YUVBufferGlobal[CAMERA_ISP_HV_FRAME_SIZE * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
#else
static uint8_t CAMERA_YUVBufferGlobal[CAMERA_ISP_HV_SLICE_SIZE * CAMERA_NUM_ISP_INT_BUFFERS] COMP_ALIGN(256) COMP_SECTION(".cam.yuv.uncached");
#endif /* (defined CAMERA_ENABLE_DUAL_PASS && !defined CAMERA_DP_ENABLE_OPTION2) */

/**
    @brief RAW internal output buffers

    Memory for internal RAW buffers

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RAW_OUT_BPP_MACRO
    @trace #BRCM_SWARCH_CAMERA_MAX_NUM_RAW_CHANNELS_MACRO
    @trace #BRCM_SWARCH_CAMERA_DP_MACRO
    @trace #BRCM_SWARCH_CAMERA_ALIGN_MACRO
*/
#if (defined CAMERA_ENABLE_DUAL_PASS && defined CAMERA_DP_ENABLE_OPTION2)
static uint8_t CAMERA_RawBufferGlobal[CAMERA_ALIGN(((CAMERA_MAX_SENSOR_WIDTH * CAMERA_RAW_OUT_BPP) / 8UL), 32UL) * CAMERA_MAX_SENSOR_HEIGHT] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
#elif defined CAMERA_ENABLE_DUAL_PASS
static uint8_t CAMERA_RawBufferGlobal[CAMERA_ALIGN(((CAMERA_MAX_SENSOR_WIDTH * CAMERA_RAW_OUT_BPP) / 8UL), 32UL) * CAMERA_MAX_SENSOR_HEIGHT * CAMERA_MAX_NUM_RAW_CHANNELS * 2UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
#else
static uint8_t *CAMERA_RawBufferGlobal = NULL;
#endif /* CAMERA_ENABLE_DUAL_PASS */

#ifdef CAMERA_TUNING
/**
    @brief Y slice memory allocated for slice capture

    Y slice memory allocated for slice capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_YSliceGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL] COMP_ALIGN(256) COMP_SECTION(".cam.yuv.uncached");

/**
    @brief U slice memory allocated for slice capture

    U slice memory allocated for slice capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_USliceGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL / 4UL] COMP_ALIGN(256) COMP_SECTION(".cam.yuv.uncached");

/**
    @brief V slice memory allocated for slice capture

    V slice memory allocated for slice capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_VSliceGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * 16UL / 4UL] COMP_ALIGN(256) COMP_SECTION(".cam.yuv.uncached");

#ifdef CAMERA_TUNING_FC
/**
    @brief Y frame memory allocated for frame capture

    Y frame memory allocated for frame capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_YFrameGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");

/**
    @brief U frame memory allocated for frame capture

    U frame memory allocated for frame capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_UFrameGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT / 4UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");

/**
    @brief V frame memory allocated for frame capture

    V frame memory allocated for frame capture request from camera tuning
    application

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
uint8_t CAMERA_VFrameGlobal[MEDIA_SYSTEM_MAX_VIDEO_WIDTH * MEDIA_SYSTEM_MAX_VIDEO_HEIGHT / 4UL] COMP_ALIGN(256) COMP_SECTION(".cam.raw.slowmem");
#endif /* CAMERA_TUNING_FC */

/**
    @brief Global context for CAMERA_TuningStateType

    State for Camera tuning slice/frame capture

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_TUNING_CONTEXT_TYPE

*/
static CAMERA_TuningStateType CAMERA_TuningStateGlobal =
{
    .fcState = CAMERA_TUNING_FC_STATE_IDLE,
    .sliceNum = 0UL,
    .dmaJobIdx = ISYS_DMA_INVALID_INDEX,
};

/* CAMERA_InitTuningContext */
static void CAMERA_InitTuningContext(void)
{
    CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_IDLE;
    CAMERA_TuningStateGlobal.sliceNum = 0UL;
    CAMERA_TuningStateGlobal.dmaJobIdx = ISYS_DMA_INVALID_INDEX;
}

#endif

/* CAMERA_StateSEGraph */
static const uint8_t CAMERA_StateSEGraph[5U][2U] = {{(uint8_t) CAMERA_STATE_INIT, (uint8_t) CAMERA_STATE_INIT},
                                                    {(uint8_t) CAMERA_STATE_RESET, (uint8_t) CAMERA_STATE_READY},
                                                    {(uint8_t) CAMERA_STATE_RUNNING, (uint8_t) CAMERA_STATE_RESET},
                                                    {(uint8_t) CAMERA_STATE_READY, (uint8_t) CAMERA_STATE_READY},
                                                    {(uint8_t) CAMERA_STATE_RESET, (uint8_t) CAMERA_STATE_RESET}};

/**
    @brief Global context for CAMERA_CompState

    State for camera media component

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_COMP_CONTEXT_TYPE
*/
static CAMERA_CompState CAMERA_CompStateGlobal =
{
    .state = CAMERA_STATE_RESET,
    .csnResetInProgress = 0U,
    .hdr = NULL,
    .ptmState = CAMERA_PTM_LOAD_STATE_IDLE,
    .cvReqState = CAMERA_CV_REQ_STATE_IDLE,
};

/** @brief Camera Report Error method

    Wrapper for BCM_ReportError.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aApiID      API ID for the method
    @param[in]      aErr        Error type
    @param[in]      aVal1       First uint32_t data to be printed
    @param[in]      aVal2       Second uint32_t data to be printed
    @param[in]      aVal3       Third uint32_t data to be printed

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_NUM_MEDIA_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUTPORT_CONFIG_PROC

    @limitations None

    @code{.unparsed}
    Call #BCM_ReportError
    @endcode
*/
static void CAMERA_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal1,
                            uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {CAMERA_CompStateGlobal.state, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CAM_ID, 0U, aApiID, aErr, 4UL, values);
}

/* CAMERA_InitContext */
static void CAMERA_InitContext(void)
{
    CAMERA_CompStateGlobal.csnResetInProgress = 0U;
    CAMERA_CompStateGlobal.hdr = NULL;
    CAMERA_CompStateGlobal.ptmState = CAMERA_PTM_LOAD_STATE_IDLE;
    CAMERA_CompStateGlobal.cvReqState = CAMERA_CV_REQ_STATE_IDLE;
}

/* CAMERA_SetState */
static void CAMERA_SetState(CAMERA_State aState)
{
#ifdef CAMERA_OutPortSE
    if ((CAMERA_STATE_ERROR == aState) &&
        (CAMERA_STATE_RUNNING == CAMERA_CompStateGlobal.state)) {
        int32_t ret = WDS_DisableSE(&CAMERA_OutPortSE);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_SET_STATE_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
    }
#endif
    if (aState != CAMERA_CompStateGlobal.state) {
#ifdef CAMERA_StateSE
        int32_t ret = WDS_NotifyState(&CAMERA_StateSE,
                              (WDS_StateType) aState);
        if (ret != BCM_ERR_OK) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_SET_STATE_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
#endif /* CAMERA_StateSE */
        CAMERA_CompStateGlobal.state = aState;
    }
}

/**
    @brief Camera media buffer

    #MediaBuffer for camera media component

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
static MSYS_BufferType CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_MAX][MSGQ_SIZE];

/** @brief Get data message for HV port

    Retrieves data message for HV port

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex       Index for data message to be retrieved

    Return values are documented in reverse-chronological order
    @retval     void *           Pointer to the message
    @retval     NULL             aIndex < #MSGQ_SIZE

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO

    @limitations None

    @code{.unparsed}
    if #MSGQ_SIZE > aIndex
    return HV #CAMERA_MediaBufferGlobal for aIndex
    @endcode

*/
static void* CAMERA_GetDataMsgHV(uint32_t aIndex)
{
    void *dataMsg = NULL;

    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_HV][aIndex];
    }

    return dataMsg;
}

/** @brief Get data message for CV port

    Retrieves data message for CV port

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex       Index for data message to be retrieved

    Return values are documented in reverse-chronological order
    @retval     void *           Pointer to the message
    @retval     NULL             aIndex < #MSGQ_SIZE

    @post None

    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO

    @limitations None

    @code{.unparsed}
    if #MSGQ_SIZE > aIndex
    return CV #CAMERA_MediaBufferGlobal for aIndex
    @endcode

*/
static void* CAMERA_GetDataMsgCV(uint32_t aIndex)
{
    void *dataMsg = NULL;

    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_CV][aIndex];
    }

    return dataMsg;
}

/** @brief Get data message for Auxiliary port

    Retrieves data message for Auxiliary port

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex       Index for data message to be retrieved

    Return values are documented in reverse-chronological order
    @retval     void *           Pointer to the message
    @retval     NULL             aIndex < #MSGQ_SIZE

    @post None

    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO

    @limitations None

    @code{.unparsed}
    if #MSGQ_SIZE > aIndex
    return Auxiliary #CAMERA_MediaBufferGlobal for aIndex
    @endcode
*/
static void* CAMERA_GetDataMsgAux(uint32_t aIndex)
{
    void *dataMsg = NULL;

    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_AUX][aIndex];
    }

    return dataMsg;
}

/**
    @brief Declare output port for HV

    @anchor CAMERA_OutPortHVGlobal
    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_GET_NUM_MEDIA_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUTPORT_CONFIG_PROC
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO
*/
MSYS_DECLARE_OUT_PORT(CAMERA_OutPortHVGlobal, 0UL, CAMERA_DataMsgHdrQHVGlobal,
                        CAMERA_Task, CAMERA_GetDataMsgHV,
                        MSYS_COMP_ID_CAM0, SystemEvent8);

/**
    @brief Declare output port for CV

    @anchor CAMERA_OutPortCVGlobal
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_GET_NUM_MEDIA_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUTPORT_CONFIG_PROC
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO
*/
MSYS_DECLARE_OUT_PORT(CAMERA_OutPortCVGlobal, 1UL, CAMERA_DataMsgHdrQCVGlobal,
                        CAMERA_Task, CAMERA_GetDataMsgCV,
                        MSYS_COMP_ID_CAM0, SystemEvent9);

/**
    @brief Declare output port for Aux

    @anchor CAMERA_OutPortAuxGlobal
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWARCH_CAMERA_GET_NUM_MEDIA_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUTPORT_CONFIG_PROC
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO
*/
MSYS_DECLARE_OUT_PORT(CAMERA_OutPortAuxGlobal,2UL, CAMERA_DataMsgHdrQAuxGlobal,
                        CAMERA_Task, CAMERA_GetDataMsgAux,
                        MSYS_COMP_ID_CAM0, SystemEvent10);

/**
    @brief Declare camera media component command queue

    @anchor CAMERA_CmdMsgHdrQGlobal
    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC
*/
MSGQ_DEFINE_HDRQ(CAMERA_CmdMsgHdrQGlobal);

/**
    @brief Command Message queue for Camera

    Command message queue for camera media component

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC
*/
static MSYS_CmdType CAMERA_CmdMsgQGlobal[MSGQ_SIZE];

/** @brief Get command message

    Retrieves command message for camera media component

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aIndex       Index for message

    Return values are documented in reverse-chronological order
    @retval     void *           Pointer to the message
    @retval     NULL             aIndex < #MSGQ_SIZE

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC

    @limitations None

    @code{.unparsed}
    if #MSGQ_SIZE > aIndex
    return #CAMERA_CmdMsgQGlobal for aIndex
    @endcode
*/
static void* CAMERA_GetCmdMsg(uint32_t aIndex)
{
    void *cmdMsg = NULL;
    if (MSGQ_SIZE > aIndex) {
        cmdMsg = (void *)&CAMERA_CmdMsgQGlobal[aIndex];
    }

    return cmdMsg;
}

/**
    @brief Camera command message queue definition

    Define elements for camera media component message queue

    @anchor CAMERA_CmdQGlobal
    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC
*/
MSGQ_DEFINE(CAMERA_CmdQGlobal, CAMERA_Task, CAM_EVENT_CQUEUE,
            MSGQ_CLIENT_SIGNAL_ENABLE, MSYS_CmdType, &CAMERA_CmdMsgHdrQGlobal, CAMERA_GetCmdMsg);

/** @brief De-initialize camera media component

    De-initialize all subcomponents of Camera

    @behavior Sync, Non-reentrant

    @pre None

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    if Camera Sensor state is either READY | ERROR | INITIALISED
        Deinitialize Camera Sensor
        Report error if failed
    if ISYS state is either READY | ERROR | INITIALISED
        Deinitialize ISYS
        Report error if failed
    if Tuner state is either READY | ERROR | INITIALISED
        Deinitialize Tuner
        Report error if failed
    @endcode

*/
static void CAMERA_Deinitialize(void)
{
    int32_t ret;

    CAMERA_InitContext();
#ifdef CAMERA_TUNING
    CAMERA_InitTuningContext();
#endif /* CAMERA_TUNING */
    if ((CSN_STATE_INITIALISED == CSN_GetState())
        || (CSN_STATE_READY == CSN_GetState())
        || (CSN_STATE_ERROR == CSN_GetState())) {
        /* Deinitialize Sensor */
        ret = CSN_Deinit();
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_DEINIT_PART_PROC, ret, __LINE__, 0UL, 0UL);
        }
    }

    if ((ISYS_STATE_INITIALISED == ISYS_GetState())
         || (ISYS_STATE_READY == ISYS_GetState())
         || (ISYS_STATE_ERROR == ISYS_GetState())) {
        /* Deinitialize ISYS */
        ret = ISYS_Deinit();
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_DEINIT_PART_PROC, ret, __LINE__, 0UL, 0UL);
        }
    }

    if ((TUNER_STATE_INITIALISED == Tuner_GetState())
         || (TUNER_STATE_READY == Tuner_GetState())
         || (TUNER_STATE_ERROR == Tuner_GetState())) {
        /* Deinitialize Tuner */
        ret = Tuner_Deinit();
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_DEINIT_PART_PROC, ret, __LINE__, 0UL, 0UL);
        }
    }
}

/* CAMERA_CancelPortMsgs */
static void CAMERA_CancelPortMsgs(void)
{
    uint32_t msgIdx = 0xFFFFFFFFUL;
    int32_t msgRet;
    int32_t ret;

    msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, &msgIdx);

    while (BCM_ERR_OK == msgRet) {
        CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_HV][msgIdx].response =
                                                                BCM_ERR_CANCELLED;
        ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, msgIdx);
        if (BCM_ERR_OK != ret) {
            break;
        }
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, &msgIdx);
    }

    msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, &msgIdx);

    while (BCM_ERR_OK == msgRet) {
        CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_CV][msgIdx].response =
                                                                BCM_ERR_CANCELLED;
        ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, msgIdx);
        if (BCM_ERR_OK != ret) {
            break;
        }
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, &msgIdx);
    }

    msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, &msgIdx);

    while (BCM_ERR_OK == msgRet) {
        CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_AUX][msgIdx].response =
                                                                BCM_ERR_CANCELLED;
        ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, msgIdx);
        if (BCM_ERR_OK != ret) {
            break;
        }
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, &msgIdx);
    }

    return;
}

/** @brief Error state handler

    Error state handler for camera media component. Only de-initialize requests
    are processed in this state

    @behavior Sync, Non-reentrant

    @pre Camera Media Component is in Error state

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    Retrieve outstanding command
    if command is MSYS_COMP_CMD_DEINIT
        Call #CAMERA_Deinitialize
        Check response
        Complete command with status
    @endcode

*/
static void CAMERA_ErrorStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    uint32_t flags = 0UL;
    int32_t retVal = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&CAMERA_CmdQGlobal, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &CAMERA_CmdMsgQGlobal[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            CAMERA_Deinitialize();
            if ((ISYS_STATE_POWEROFF == ISYS_GetState())
                && (TUNER_STATE_RESET == Tuner_GetState())
                && (CSN_STATE_POWEROFF == CSN_GetState())) {
                CAMERA_SetState(CAMERA_STATE_RESET);
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
            }
        } else if (processCmd->cmd == MSYS_COMP_CMD_STOP) {
            /* Stop Tuner so that no more events are processed */
            if (TUNER_STATE_RUNNING == Tuner_GetState()) {
                ret = Tuner_Stop();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }

            if (CSN_STATE_STREAMING == CSN_GetState()) {
                flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
                if (0UL != (flags & CSN_STATUS_ERROR)) {
                    ret = BCM_ERR_UNKNOWN;
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
                if (0UL == (flags & CSN_STATUS_BUSY)) {
                    /* Stop Sensor */
                    ret = CSN_Stop();
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC,
                                ret, __LINE__, 0UL, 0UL);
                        goto func_exit;
                    } else {
                        ret = BCM_ERR_OK;
                    }
                }
            }

            if (((TUNER_STATE_READY == Tuner_GetState()) ||
                 (TUNER_STATE_ERROR == Tuner_GetState())) &&
                ((CSN_STATE_READY == CSN_GetState()) ||
                 (CSN_STATE_ERROR == CSN_GetState()))) {
                if (ISYS_STATE_STREAMING == ISYS_GetState()) {
                    /* Stop ISYS */
                    ret = ISYS_Stop();
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                        goto func_exit;
                    } else {
                        ret = BCM_ERR_OK;
                    }
                }

                if ((ISYS_STATE_READY == ISYS_GetState()) ||
                     (ISYS_STATE_ERROR == ISYS_GetState())) {
                    /* Stop Tuner Task */
                    retVal = BCM_SetEvent(CAMERA_TUNER_TASK, TUNER_EVENT_QUIT);
                    if (BCM_ERR_OK != retVal) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC,
                                retVal, __LINE__, 0UL, 0UL);
                        goto func_exit;
                    }
                }
            }

            if (0UL != (CAMERA_CompStateGlobal.event & TUNER_EVENT_CMD_COMPLETE)) {
                CAMERA_CompStateGlobal.tunerActive = 0x0UL;

                /* Cancel any port message */
                CAMERA_CancelPortMsgs();
                CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL;
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
            }
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
            ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_ERROR_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }
    }

func_exit:
    return;
}

/** @brief Reset state handler

    Reset state handler for camera media component.

    @behavior Sync, Non-reentrant

    @pre Camera Media Component is in Reset state

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    Initialize MediaPort for HV
    if successful
        Initialize MediaPort for CV
        if successful
            Check for error and return if failed
            if successful
                Initialize MediaPort for Aux
                if successful
                    Retrieve any command queued
                    if (cmd == MSYS_COMP_CMD_INIT)
                        if Tuner is in Ready state
                            Initialize Tuner
                        if ISYS is in PowerOff state
                            Initialize ISYS
                        if Camera Sensor is in PowerOff state
                            Initialize Camera Sensor
    Move to error state if any of the operation fails
    Complete command with final status
    @endcode

*/
static void CAMERA_ResetStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&CAMERA_CmdQGlobal, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &CAMERA_CmdMsgQGlobal[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            if (TUNER_STATE_RESET == Tuner_GetState()) {
                /* Initialize Tuner */
                ret = Tuner_Init();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }
            if (ISYS_STATE_POWEROFF == ISYS_GetState()) {
                /* Initialize ISYS */
                ret = ISYS_Init();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }
            if (CSN_STATE_POWEROFF == CSN_GetState()) {
                if (0U == CAMERA_CompStateGlobal.csnResetInProgress) {
                    /* Initialize Sensor */
                    ret = CSN_Init();
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                        goto func_exit;
                    } else {
                        if (BCM_ERR_BUSY == ret) {
                            CAMERA_CompStateGlobal.csnResetInProgress = 1U;
                        } else {
                            CAMERA_CompStateGlobal.csnResetInProgress = 0U;
                        }
                    }
                }
            }
            if ((ISYS_STATE_INITIALISED == ISYS_GetState())
                && (CSN_STATE_INITIALISED == CSN_GetState())) {
                CAMERA_CompStateGlobal.csnResetInProgress = 0U;
                ret = MSYS_OutPortInit(&CAMERA_OutPortHVGlobal);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }

                ret = MSYS_OutPortInit(&CAMERA_OutPortCVGlobal);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }

                ret = MSYS_OutPortInit(&CAMERA_OutPortAuxGlobal);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
                /* Initialize globals */
                CAMERA_SetState(CAMERA_STATE_INIT);
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
            } else {
                if (1U == CAMERA_CompStateGlobal.csnResetInProgress) {
                    ret = BCM_ERR_BUSY;
                }
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }

func_exit:
    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState())) {
            CAMERA_SetState(CAMERA_STATE_ERROR);
        }
        if (NULL != processCmd) {
            processCmd->response = ret;
            ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RESET_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }

    }
}

/**
    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
typedef union _CAMERA_ConfigDataType {
    const uint8_t* hdr;
    const struct _CSN_ImageType   *cameraSensorImg;
} CAMERA_ConfigDataType;

/**
    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC
*/
static int32_t CAMERA_ConfigureInternal(MSYS_CmdType *aProcessCmd, uint32_t *aFlags)
{
    int32_t ret = BCM_ERR_OK;
    ISYS_InfoType info;
    const uint8_t *configHdr;
    CAMERA_ConfigDataType config;

    config.hdr = aProcessCmd->cfgData;
    ISYS_GetInfo(&info);
    if (0UL == aProcessCmd->size) {
        if (CAMERA_PTM_LOAD_STATE_IDLE == CAMERA_CompStateGlobal.ptmState) {
            ret = IMGL_LoadImg((uint16_t)aProcessCmd->configID, info.scratchMem, 0UL,
                    info.scratchMemSize, CAM_EVENT_PTM, &CAMERA_CompStateGlobal.imglStatus);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                                                ret, __LINE__, 0UL, 0UL);
                goto func_exit;
            }
            CAMERA_CompStateGlobal.ptmState = CAMERA_PTM_LOAD_STATE_REQ;
            goto func_exit;
        } else if (CAMERA_PTM_LOAD_STATE_REQ == CAMERA_CompStateGlobal.ptmState) {
            /* Wait for the PTM load complete */
            goto func_exit;
        } else {
            /* (CAMERA_PTM_LOAD_STATE_RCVD == CAMERA_CompStateGlobal.ptmState) */
            int32_t ptmStatus;

            ptmStatus = IMGL_GetStatus(&CAMERA_CompStateGlobal.imglStatus);
            if (ptmStatus != BCM_ERR_OK) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                                      (int32_t) ptmStatus, __LINE__, 0UL, 0UL);
                ret = BCM_ERR_UNKNOWN;
                goto func_exit;
            }
            configHdr = info.scratchMem;
            CAMERA_CompStateGlobal.ptmState = CAMERA_PTM_LOAD_STATE_IDLE;
        }
    } else {
        configHdr = config.hdr;
    }
    {
        const ISYS_ConfigsType cfg = {
                .Cfg.pHdr = configHdr,
                .isysMemPool = {CAMERA_YUVBufferGlobal, CAMERA_RawBufferGlobal},
                .isysMemsize = {sizeof(CAMERA_YUVBufferGlobal), sizeof(CAMERA_RawBufferGlobal)},
#ifdef CAMERA_ENABLE_DUAL_PASS
#ifdef CAMERA_DP_ENABLE_OPTION2
                .dpMode = ISYS_DUAL_PASS_MODE_LL,
#else
                .dpMode = ISYS_DUAL_PASS_MODE_FRAME,
#endif /* CAMERA_DP_ENABLE_OPTION2 */
#else
                .dpMode = ISYS_DUAL_PASS_MODE_NONE,
#endif /* CAMERA_ENABLE_DUAL_PASS */
            };

        switch (aProcessCmd->configID & 0xFFF0UL) {
            case MSYS_CONFIG_CAM_MOD:
            case MSYS_CONFIG_CAM_UCM:
            case MSYS_CONFIG_CAM_ISP:
                ret = ISYS_Configure(&cfg);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                            ret, (uint32_t)__LINE__, (uint32_t)aProcessCmd->configID, 0UL);
                    goto func_exit;
                }
            break;
            case MSYS_CONFIG_CAM_CTN:
                ret = Tuner_Configure(cfg.Cfg.pTunerImg);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                            ret, __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            break;
            case MSYS_CONFIG_CAM_CSN:
                if (CSN_STATE_INITIALISED == CSN_GetState() ||
                    CSN_STATE_READY == CSN_GetState()) {
                    /* Process asynchronous events to sensor */
                    *aFlags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
                    if (0UL != ((*aFlags) & CSN_STATUS_ERROR)) {
                        ret = BCM_ERR_UNKNOWN;
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                                ret, __LINE__, 0UL, 0UL);
                        goto func_exit;
                    }
                    if (0UL != ((*aFlags) & CSN_STATUS_BUSY)) {
                        (*aFlags) = CSN_STATUS_BUSY;
                    } else {
                        ret = CSN_Configure(config.cameraSensorImg);
                        if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                                    ret, __LINE__, 0UL, 0UL);
                            goto func_exit;
                        }
                        if (BCM_ERR_BUSY == ret) {
                            (*aFlags) = CSN_STATUS_BUSY;
                            ret = BCM_ERR_OK;
                        }
                    }
                }
            break;
            default:
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_CONFIGURE_INTERNAL_PROC,
                        BCM_ERR_INVAL_PARAMS, __LINE__, aProcessCmd->configID, 0UL);
            break;
        }
    }

func_exit:
    return ret;
}

/** @brief Init state handler

    Init state handler for camera media component.

    @behavior Sync, Non-reentrant

    @pre Camera Media Component is in Init state

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    Retrieve command from Camera command queue
    if(cmd == MSYS_COMP_CMD_CONFIG)
        Call CAMERA_ConfigureInternal
    else if (cmd == MSYS_COMP_CMD_DEINIT)
        De initialize Camera
        if ((ISYS_STATE_POWEROFF == ISYS_GetState())
            && (TUNER_STATE_RESET == Tuner_GetState())
            && (CSN_STATE_POWEROFF == CSN_GetState()))
            state = CAMERA_STATE_RESET;

    if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState()))
            state = CAMERA_STATE_ERROR;

    if (NULL != processCmd) {
        processCmd->response = ret;
        if (0UL == (flags & CSN_STATUS_BUSY) &&
                    CAMERA_PTM_LOAD_STATE_IDLE == ptmState)
            Complete Message
    @endcode

*/
static void CAMERA_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    uint32_t flags = 0UL;

    ret = MSGQ_GetMsgIdx(&CAMERA_CmdQGlobal, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &CAMERA_CmdMsgQGlobal[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            ret = CAMERA_ConfigureInternal(processCmd, &flags);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_INIT_STATE_HANDLER_PART_PROC,
                        ret, __LINE__, 0UL, 0UL);
                goto func_exit;
            }
            if ((ISYS_STATE_READY == ISYS_GetState())
                && (TUNER_STATE_READY == Tuner_GetState())
                && (CSN_STATE_READY == CSN_GetState())) {
                CAMERA_SetState(CAMERA_STATE_READY);
            }
        } else if (processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            CAMERA_Deinitialize();
            if ((ISYS_STATE_POWEROFF == ISYS_GetState())
                && (TUNER_STATE_RESET == Tuner_GetState())
                && (CSN_STATE_POWEROFF == CSN_GetState())) {
                CAMERA_SetState(CAMERA_STATE_RESET);
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }

func_exit:
    if (BCM_ERR_OK != ret) {
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState())) {
            CAMERA_SetState(CAMERA_STATE_ERROR);
        }
    }

    if (NULL != processCmd) {
        processCmd->response = ret;
        if ((0UL == (flags & CSN_STATUS_BUSY)) &&
            (CAMERA_PTM_LOAD_STATE_IDLE == CAMERA_CompStateGlobal.ptmState)) {
            ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_INIT_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }
    }
}

/** @brief Ready state handler

    Ready state handler for camera media component.

    @behavior Sync, Non-reentrant

    @pre Camera Media Component is in Ready state

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    Retrieve command message
    if(cmd == MSYS_COMP_CMD_CONFIG)
        Call CAMERA_ConfigureInternal
    else if (cmd == MSYS_COMP_CMD_START)
        if (TUNER_STATE_READY == Tuner_GetState())
            ret = ISYS_Start()
            Check ret
        if (ISYS_STATE_READY == ISYS_GetState())
            ret = ISYS_Start()
            Check ret
        if (CSN_STATE_READY == CSN_GetState())
            flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC)
            if (0UL != (flags & CSN_STATUS_ERROR))
                Report error
            else if (0UL != (flags & CSN_STATUS_CMD_COMPLETE))
                Activate Tuner Task
                Check for error
                tunerActive = 0x1UL;
            else if (0UL != (flags & CSN_STATUS_BUSY))
                Nothing to do
            else
                ret = CSN_Start()
                Check ret
                flags = CSN_STATUS_BUSY;
        if ((ISYS_STATE_STREAMING == ISYS_GetState())
            && (TUNER_STATE_RUNNING == Tuner_GetState())
            && (CSN_STATE_STREAMING == CSN_GetState()))
            ISYS_GetInfo(&info)
            outIdx = (0UL != (info.mode.passProps[ISYS_PASS_IDX_HV].outputEnable & ISYS_OUT_ENABLE_LORES)) ?
                                   ISYS_OUT_IDX_LORES : ISYS_OUT_IDX_HIRES
            lastSliceNum = ((outputProps[outIdx].height+15UL) >> 4UL) - 1UL
            curSliceNum = 0UL
            state = CAMERA_STATE_RUNNING
    else if (processCmd->cmd == MSYS_COMP_CMD_DEINIT)
        CAMERA_Deinitialize();
        if ((ISYS_STATE_POWEROFF == ISYS_GetState())
            && (TUNER_STATE_RESET == Tuner_GetState())
            && (CSN_STATE_POWEROFF == CSN_GetState()))
            state = CAMERA_STATE_RESET;
    if (BCM_ERR_OK != ret)
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState()))
            state = CAMERA_STATE_ERROR;

    if (NULL != processCmd)
        response = ret;
        if (0UL == (flags & CSN_STATUS_BUSY))
            Complete command message
    @endcode

*/
static void CAMERA_ReadyStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    uint32_t flags = 0UL;
    int32_t retVal;

    ret = MSGQ_GetMsgIdx(&CAMERA_CmdQGlobal, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &CAMERA_CmdMsgQGlobal[cmdMsgIdx];
        /* validate the command */
       if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            ret = CAMERA_ConfigureInternal(processCmd, &flags);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC,
                        ret, __LINE__, 0UL, 0UL);
                goto func_exit;
            }
        } else if (processCmd->cmd == MSYS_COMP_CMD_START) {
            if (TUNER_STATE_READY == Tuner_GetState()) {
                /* Start */
                ret = Tuner_Start();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }
            if (ISYS_STATE_READY == ISYS_GetState()) {
                /* Start */
                ret = ISYS_Start();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }
            if (CSN_STATE_READY == CSN_GetState()) {
                flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
                if (0UL != (flags & CSN_STATUS_ERROR)) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
                if (0UL != (flags & CSN_STATUS_BUSY)) {
                    flags = CSN_STATUS_BUSY;
                } else {
                    ret = CSN_Start();
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC,
                                ret, __LINE__, 0UL, 0UL);
                        goto func_exit;
                    }

                    if (BCM_ERR_BUSY == ret) {
                        flags = CSN_STATUS_BUSY;
                        ret = BCM_ERR_OK;
                    } else {
                        retVal = BCM_ActivateTask(CAMERA_TUNER_TASK);
                        if (BCM_ERR_OK != retVal) {
                            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC,
                                    retVal, __LINE__, 0UL, 0UL);
                            ret = BCM_ERR_UNKNOWN;
                            goto func_exit;
                        }
                        CAMERA_CompStateGlobal.tunerActive = 0x1UL;
                    }
                }
            }

            if ((ISYS_STATE_STREAMING == ISYS_GetState())
                && (TUNER_STATE_RUNNING == Tuner_GetState())
                && (CSN_STATE_STREAMING == CSN_GetState())) {
                ISYS_InfoType info;
                ISYS_OutIDXType outIdx;

                /* Fill embedded offsets */
                CSN_GetEmbOffsets(&CAMERA_CompStateGlobal.embOffsets);
                CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL;
                ISYS_GetInfo(&info);
                outIdx = (0UL != (info.mode.passProps[ISYS_PASS_IDX_HV].outputEnable & ISYS_OUT_ENABLE_LORES)) ?
                                       ISYS_OUT_IDX_LORES : ISYS_OUT_IDX_HIRES;
                CAMERA_CompStateGlobal.lastSliceNum =
                    ((info.mode.passProps[ISYS_PASS_IDX_HV].outputProps[outIdx].height + 15UL) >> 4UL) - 1UL;
                CAMERA_CompStateGlobal.curSliceNum = CAMERA_CompStateGlobal.lastSliceNum;
                CAMERA_SetState(CAMERA_STATE_RUNNING);
#ifdef CAMERA_OutPortSE
                {
                    WDS_ConfigType wdsCfg;
                    uint32_t i;

                    wdsCfg.mode = WDS_MODE_LOGICAL | WDS_MODE_DEADLINE | WDS_MODE_FREQ;
                    if (0UL != info.mode.fps) {
                        wdsCfg.deadlineInMs = (1000UL / info.mode.fps);
                        wdsCfg.minFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) - 1UL;
                        wdsCfg.maxFrequency = WDS_CHECK_TIME_US/(wdsCfg.deadlineInMs * 1000UL) + 1UL;
                    } else {
                        wdsCfg.deadlineInMs = 0UL;
                        wdsCfg.minFrequency = 0UL;
                        wdsCfg.maxFrequency = 0UL;
                    }
                    wdsCfg.lStart = (WDS_StateType) 0U;
                    wdsCfg.lEnd = (WDS_StateType) CAMERA_CompStateGlobal.lastSliceNum;
                    BCM_MemSet(CAMERA_OutPortSE.buffer, WDS_INVALID_GRAPH_BUF_ENTRY,
                               ((uint32_t) CAMERA_OutPortSE.maxState) *
                               ((uint32_t) CAMERA_OutPortSE.maxTransitionsPerState));
                    for (i = wdsCfg.lStart; i < wdsCfg.lEnd; i++) {
                        CAMERA_OutPortSE.buffer[i] = (WDS_StateType)(i + 1UL);
                    }
                    ret = WDS_EnableSE(&CAMERA_OutPortSE, &wdsCfg);
                    if (BCM_ERR_OK != ret) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    }
                }
#endif /* CAMERA_OutPortSE */
            }
        } else if (processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            CAMERA_Deinitialize();
            if ((ISYS_STATE_POWEROFF == ISYS_GetState())
                && (TUNER_STATE_RESET == Tuner_GetState())
                && (CSN_STATE_POWEROFF == CSN_GetState())) {
                CAMERA_SetState(CAMERA_STATE_RESET);
            }
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }

func_exit:
    if (BCM_ERR_OK != ret) {
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState())) {
            CAMERA_SetState(CAMERA_STATE_ERROR);
        }
    }

    if (NULL != processCmd) {
        processCmd->response = ret;
        if ((0UL == (flags & CSN_STATUS_BUSY)) &&
            (CAMERA_PTM_LOAD_STATE_IDLE == CAMERA_CompStateGlobal.ptmState)) {
            ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_READY_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }
    }
}

/** @brief Run state handler

    Run state handler for camera media component.

    @behavior Sync, Non-reentrant

    @pre Camera Media Component is in Running state

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_CUSTOM_CMD_MACRO
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    isysFlag = ISYS_ProcessEvent();

    if (0UL != (isysFlag & ISYS_EVENT_ERROR))
        Report error
        state = CAMERA_STATE_ERROR;

    Retrieve command message
    Check for status

    if (MSYS_COMP_CMD_STOP == processCmd->cmd)
        flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC)
        if ((0UL == (flags & CSN_STATUS_BUSY)) &&
                    (CSN_STATE_READY != CSN_GetState()))
            ret = CSN_Stop()
            Check for error
        else
            if (0UL != (flags & CSN_STATUS_ERROR))
                Report error

        if ((CSN_STATE_READY == CSN_GetState()) &&
                (ISYS_STATE_READY != ISYS_GetState()) &&
                (TUNER_STATE_READY != Tuner_GetState()) &&
                (lastSliceNum == curSliceNum))

            ret = ISYS_Stop()
            Check ret and report failure

            ret = Tuner_Stop()
            Check ret and report failure

            Stop Tuner Task

        if ((CSN_STATE_READY == CSN_GetState()) &&
                (ISYS_STATE_READY == ISYS_GetState()) &&
                (TUNER_STATE_READY == Tuner_GetState()) &&
                (0UL != (event & TUNER_EVENT_CMD_COMPLETE)))
            tunerActive = 0x0UL

    else if (CAMERA_CUSTOM_CMD_CAPTURE_SLICE == cmd)
        if (CAMERA_TUNING_FC_STATE_IDLE == fcState)
            sliceNum = (processCmd->customData[0] + 1UL)
            fcState = CAMERA_TUNING_FC_STATE_REQ

    else if (CAMERA_CUSTOM_CMD_CAPTURE_FRAME == cmd)
        if (CAMERA_TUNING_FC_STATE_IDLE == fcState)
            sliceNum = 0UL
            fcState = CAMERA_TUNING_FC_STATE_REQ

    if (ISYS_STATE_STREAMING == ISYS_GetState())
        msgRet = Retrieve HV data message
        while (BCM_ERR_OK == msgRet)
            Retrieve media buffer for HV
            if(0xffffffff == CAMERA_CompStateGlobal.mediaBufferIdx)
                Fill buffer parameters
                ISYS_FillBuffer for HV
                CAMERA_CompStateGlobal.mediaBufferIdx = mediaBufferIdx;

            msgRet = ISYS_GetFilledBuffer for HV

        if ((BCM_ERR_OK == msgRet)) {
            Fill Media buffer information
            if (((buf.sliceNum == 0UL) && (0UL == CAMERA_TuningStateGlobal.sliceNum))
                && (CAMERA_TuningStateGlobal.fcState == CAMERA_TUNING_FC_STATE_RUN))
                    CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_IDLE
                    Clean caches
                    response = BCM_ERR_OK;
                    Complete camera tuning FC message

            if ((CAMERA_TuningStateGlobal.fcState == CAMERA_TUNING_FC_STATE_RUN) &&
                ((0UL == CAMERA_TuningStateGlobal.sliceNum) ||
                 ((buf.sliceNum + 1UL) == CAMERA_TuningStateGlobal.sliceNum)))
                    Copy slice to global buffer

            Complete data message for HV

            CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL

       Retrieve data message for CV
       while (BCM_ERR_OK == msgRet) {
            while (BCM_ERR_OK == msgRet)
            Retrieve media buffer for CV
            if (CAMERA_CV_REQ_STATE_IDLE == cvReqState)
                Fill buffer parameters
                ISYS_FillBuffer for CV

            msgRet = ISYS_GetFilledBuffer for CV

            if ((BCM_ERR_OK == msgRet) && (CAMERA_CV_REQ_STATE_POSTED == cvReqState))
                Fill Media buffer information
                cvReqState = CAMERA_CV_REQ_STATE_IDLE
                Complete message for CV

        Retrieve data message of Auxiliary port
        while (BCM_ERR_OK == msgRet)

        Retrieve media buffer for Auxiliary port

        Fill metadata from sensor

        Fill ISYS_StatisticsType

        Fill statistics from sensor

        Retrieve data message for Auxiliary port

    if ((ISYS_STATE_READY == ISYS_GetState())
        && (TUNER_STATE_READY == Tuner_GetState())
        && (CSN_STATE_READY == CSN_GetState())
        && (0UL == tunerActive))
        if (CAMERA_CompStateGlobal.mediaBufferIdx < MSGQ_SIZE)
            Cancel and complete media buffer for HV
            CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL

            if (CAMERA_CV_REQ_STATE_POSTED == cvReqState)
                Cancel and complete media buffer for CV
                cvReqState  = CAMERA_CV_REQ_STATE_IDLE;
        state = CAMERA_STATE_READY

    if (BCM_ERR_OK != ret)
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState()))
            state = CAMERA_STATE_ERROR


    if (NULL != processCmd)
        processCmd->response = ret
        if ((CAMERA_STATE_READY == CAMERA_CompStateGlobal.state) ||
            (BCM_ERR_OK != ret))
            Complete command message
@endcode

*/
static void CAMERA_RunStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    int32_t msgRet = BCM_ERR_OK;
    uint32_t flags = 0UL;
    uint32_t isysFlag = 0UL;
    uint32_t mediaBufferIdx;
    int32_t retVal = BCM_ERR_OK;
    ISYS_BufferType buf = {
        .sliceNum = 0UL,
        .timestamp = 0ULL,
    };

    isysFlag = ISYS_ProcessEvent();

    if (0UL != (isysFlag & ISYS_EVENT_ERROR)) {
        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, BCM_ERR_UNKNOWN,
                __LINE__, 0UL, 0UL);
        CAMERA_SetState(CAMERA_STATE_ERROR);
    }

#ifdef CAMERA_TUNING
    /* Check for Camera Tuning DMA completion */
    if ((CAMERA_TUNING_FC_STATE_RUN == CAMERA_TuningStateGlobal.fcState) &&
        (ISYS_DMA_INVALID_INDEX != CAMERA_TuningStateGlobal.dmaJobIdx)) {

        ret = PP_DMAStatus(CAMERA_TuningStateGlobal.dmaJobIdx);
        if ((BCM_ERR_OK == ret) || (BCM_ERR_BUSY == ret)) {
            if (BCM_ERR_OK == ret) {
                CAMERA_TuningStateGlobal.dmaJobIdx = ISYS_DMA_INVALID_INDEX;
            } else {
                ret = BCM_ERR_OK;
            }
        } else {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
    }
#endif /* CAMERA_TUNING */

    msgRet = MSGQ_GetMsgIdx(&CAMERA_CmdQGlobal, &cmdMsgIdx);
    if (BCM_ERR_OK == msgRet) {
        processCmd = &CAMERA_CmdMsgQGlobal[cmdMsgIdx];
        /* validate the command */
        if (MSYS_COMP_CMD_STOP == processCmd->cmd) {
            /* Stop Tuner so that no more events are processed */
            if (TUNER_STATE_READY != Tuner_GetState()) {
                ret = Tuner_Stop();
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
            }
            if (CSN_STATE_STREAMING == CSN_GetState()) {
                flags = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA, CSN_EVENT_SOURCE_IIC);
                if (0UL != (flags & CSN_STATUS_ERROR)) {
                    ret = BCM_ERR_UNKNOWN;
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
                if (0UL == (flags & CSN_STATUS_BUSY)) {
                    /* Stop Sensor */
                    ret = CSN_Stop();
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC,
                                ret, __LINE__, 0UL, 0UL);
                        goto func_exit;
                    } else {
                        ret = BCM_ERR_OK;
                    }
                }
            }

            if ((TUNER_STATE_READY == Tuner_GetState()) &&
                    (CSN_STATE_READY == CSN_GetState()) &&
                    (ISYS_STATE_READY != ISYS_GetState()) &&
                    (CAMERA_CompStateGlobal.lastSliceNum == CAMERA_CompStateGlobal.curSliceNum)) {
                /* Stop ISYS */
                ret = ISYS_Stop();
                if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                    goto func_exit;
                }
                if (BCM_ERR_BUSY != ret) {
                    /* Stop Tuner Task */
                    retVal = BCM_SetEvent(CAMERA_TUNER_TASK, TUNER_EVENT_QUIT);
                    if (BCM_ERR_OK != retVal) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC,
                                retVal, __LINE__, 0UL, 0UL);
                        ret = BCM_ERR_UNKNOWN;
                        goto func_exit;
                    }
                } else {
                    ret = BCM_ERR_OK;
                }
            }
            if ((CSN_STATE_READY == CSN_GetState()) &&
                    (ISYS_STATE_READY == ISYS_GetState()) &&
                    (TUNER_STATE_READY == Tuner_GetState()) &&
                    (0UL != (CAMERA_CompStateGlobal.event & TUNER_EVENT_CMD_COMPLETE))){
                CAMERA_CompStateGlobal.tunerActive = 0x0UL;
            }
#ifdef CAMERA_TUNING
        } else if (CAMERA_CUSTOM_CMD_CAPTURE_SLICE == processCmd->cmd) {
            if (CAMERA_TUNING_FC_STATE_IDLE == CAMERA_TuningStateGlobal.fcState) {
                CAMERA_TuningStateGlobal.sliceNum = (processCmd->customData[0] + 1UL);
                CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_REQ;
            }
            if ((CAMERA_TUNING_FC_STATE_RUN == CAMERA_TuningStateGlobal.fcState) &&
                (ISYS_DMA_INVALID_INDEX == CAMERA_TuningStateGlobal.dmaJobIdx)) {
                CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_IDLE;
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
            }
#ifdef CAMERA_TUNING_FC
        } else if (CAMERA_CUSTOM_CMD_CAPTURE_FRAME == processCmd->cmd) {
            if (CAMERA_TUNING_FC_STATE_IDLE == CAMERA_TuningStateGlobal.fcState) {
                CAMERA_TuningStateGlobal.sliceNum = 0UL;
                CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_REQ;
            }
            if ((CAMERA_TUNING_FC_STATE_RUN == CAMERA_TuningStateGlobal.fcState) &&
                (ISYS_DMA_INVALID_INDEX == CAMERA_TuningStateGlobal.dmaJobIdx) &&
                (CAMERA_CompStateGlobal.lastSliceNum == CAMERA_CompStateGlobal.curSliceNum)) {
                CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_IDLE;
                CAMERA_TuningStateGlobal.sliceNum = 0UL;
                processCmd->response = BCM_ERR_OK;
                ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
            }
#endif /* CAMERA_TUNING_FC */
#endif /* CAMERA_TUNING */
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
        }
    }

    if (ISYS_STATE_STREAMING == ISYS_GetState()) {
        /* Process HV messages */
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, &mediaBufferIdx);

        while (BCM_ERR_OK == msgRet) {
            /* Submit Buffer */
            MSYS_BufferType *mediaBuffer;

            /* Break out from the loop if we are in stop command and received
               all the slices of frame */
            if ((TUNER_STATE_READY == Tuner_GetState()) &&
                    (CSN_STATE_READY == CSN_GetState()) &&
                    (ISYS_STATE_READY != ISYS_GetState()) &&
                    (CAMERA_CompStateGlobal.lastSliceNum == CAMERA_CompStateGlobal.curSliceNum)) {
                    CAMERA_CompStateGlobal.mediaBufferIdx = mediaBufferIdx;
                    break;
            }

            mediaBuffer = &CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_HV][mediaBufferIdx];
            if (CAMERA_CompStateGlobal.mediaBufferIdx == 0xFFFFFFFFUL) {
                mediaBuffer->response = BCM_ERR_BUSY;

                buf.plane[0].addr = mediaBuffer->buffer[0].data;
                buf.plane[0].size = mediaBuffer->buffer[0].size;
                buf.plane[0].stride = mediaBuffer->buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[0].isEnabled = (0UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                buf.plane[1].addr = mediaBuffer->buffer[1].data;
                buf.plane[1].size = mediaBuffer->buffer[1].size;
                buf.plane[1].stride = mediaBuffer->buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[1].isEnabled = (1UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                buf.plane[2].addr = mediaBuffer->buffer[2].data;
                buf.plane[2].size = mediaBuffer->buffer[2].size;
                buf.plane[2].stride = mediaBuffer->buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[2].isEnabled = (2UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                {
                    const ISYS_BufferType bufConst = buf;
                    ret = ISYS_FillBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &bufConst);
                    if (BCM_ERR_OK != ret) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    } else {
                        CAMERA_CompStateGlobal.mediaBufferIdx = mediaBufferIdx;
                    }
                }
            }

            msgRet = ISYS_GetFilledBuffer(ISYS_PASS_IDX_HV, ISYS_OUT_IDX_MAX, &buf);
            if ((BCM_ERR_OK == msgRet) && (0xFFFFFFFFUL != CAMERA_CompStateGlobal.mediaBufferIdx)) {
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->seqNum = buf.sliceNum;
                CAMERA_CompStateGlobal.curSliceNum = buf.sliceNum;
                mediaBuffer->timestamp = buf.timestamp;
                mediaBuffer->frameStart = (0UL == buf.sliceNum) ? 1U : 0U;
                mediaBuffer->frameEnd = (CAMERA_CompStateGlobal.lastSliceNum == buf.sliceNum) ? 1U : 0U;
                mediaBuffer->frameNum = buf.frameNum;
                /* Copy final sizes */
                mediaBuffer->buffer[0].size = buf.plane[0].size;
                mediaBuffer->buffer[1].size = buf.plane[1].size;
                mediaBuffer->buffer[2].size = buf.plane[2].size;

#ifdef CAMERA_TUNING
                if ((CAMERA_TuningStateGlobal.fcState == CAMERA_TUNING_FC_STATE_REQ) &&
                    (((0UL == CAMERA_TuningStateGlobal.sliceNum) && (0UL == buf.sliceNum)) ||
                     ((0UL != CAMERA_TuningStateGlobal.sliceNum) &&
                      ((buf.sliceNum + 1UL) == CAMERA_TuningStateGlobal.sliceNum)))) {
                    CAMERA_TuningStateGlobal.fcState = CAMERA_TUNING_FC_STATE_RUN;
                }

                if ((CAMERA_TUNING_FC_STATE_RUN == CAMERA_TuningStateGlobal.fcState) &&
                    (ISYS_DMA_INVALID_INDEX == CAMERA_TuningStateGlobal.dmaJobIdx)) {
                    PP_DMABufferType dmaInBuf, dmaOutBuf;
                    uint32_t i;
                    uint8_t *sliceBufBase[ISYS_MAX_IMAGE_PLANES];

#ifdef CAMERA_TUNING_FC
                    if (0UL == CAMERA_TuningStateGlobal.sliceNum) {
                        sliceBufBase[0] = CAMERA_YFrameGlobal;
                        sliceBufBase[1] = CAMERA_UFrameGlobal;
                        sliceBufBase[2] = CAMERA_VFrameGlobal;
                    } else
#endif /* CAMERA_TUNING_FC */
                    {
                        sliceBufBase[0] = CAMERA_YSliceGlobal;
                        sliceBufBase[1] = CAMERA_USliceGlobal;
                        sliceBufBase[2] = CAMERA_VSliceGlobal;
                    }

                    /* Initialize channel number */
                    dmaInBuf.numCH = 0UL;
                    dmaOutBuf.numCH = 0UL;
                    for (i = 0UL; i < ISYS_MAX_IMAGE_PLANES; i++) {
                        if (0U != buf.plane[i].isEnabled) {
                            uint8_t *dstAddr;
                            /* Update destination address based on slice/frame type */
                            dstAddr = sliceBufBase[i];
                            if (0UL == CAMERA_TuningStateGlobal.sliceNum) {
                                dstAddr += (buf.sliceNum *
                                                buf.plane[i].height *
                                                buf.plane[i].widthInBytes);
                            }

                            /* Initialize DMA-In configurations */
                            dmaInBuf.channel[i].addr = mediaBuffer->buffer[i].data;
                            dmaInBuf.channel[i].width = buf.plane[i].widthInBytes;
                            dmaInBuf.channel[i].height = buf.plane[i].height;
                            dmaInBuf.channel[i].stride = buf.plane[i].stride;
                            dmaInBuf.numCH++;

                            /* Initialize DMA-out configurations */
                            dmaOutBuf.channel[i].addr = dstAddr;
                            dmaOutBuf.channel[i].width = buf.plane[i].widthInBytes;
                            dmaOutBuf.channel[i].height = buf.plane[i].height;
                            dmaOutBuf.channel[i].stride = buf.plane[i].widthInBytes;
                            dmaOutBuf.numCH++;
                        }
                    }
                    ret = PP_DMAStart(&dmaInBuf,
                                      &dmaOutBuf,
                                      &CAMERA_TuningStateGlobal.dmaJobIdx);
                    if (BCM_ERR_OK != ret) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    }
                } else {
                    if (CAMERA_TUNING_FC_STATE_RUN == CAMERA_TuningStateGlobal.fcState) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    }
                }
#endif /* CAMERA_TUNING */
#ifdef CAMERA_OutPortSE
                {
                    ret = WDS_NotifyState(&CAMERA_OutPortSE,
                                          (WDS_StateType) mediaBuffer->seqNum);
                    if (ret != BCM_ERR_OK) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    }
                }
#endif /* CAMERA_OutPortSE */
                ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
                CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL;

                msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortHVGlobal.dataQ, &mediaBufferIdx);
            }
        }

        /* Process CV messages */
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, &mediaBufferIdx);
        while (BCM_ERR_OK == msgRet) {
            /* Submit Buffer */
            MSYS_BufferType *mediaBuffer;
            mediaBuffer = &CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_CV][mediaBufferIdx];
            if (CAMERA_CV_REQ_STATE_IDLE == CAMERA_CompStateGlobal.cvReqState) {
                buf.plane[0].addr = mediaBuffer->buffer[0].data;
                buf.plane[0].size = mediaBuffer->buffer[0].size;
                buf.plane[0].stride = mediaBuffer->buffer[0].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[0].isEnabled = (0UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                buf.plane[1].addr = mediaBuffer->buffer[1].data;
                buf.plane[1].size = mediaBuffer->buffer[1].size;
                buf.plane[1].stride = mediaBuffer->buffer[1].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[1].isEnabled = (1UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                buf.plane[2].addr = mediaBuffer->buffer[2].data;
                buf.plane[2].size = mediaBuffer->buffer[2].size;
                buf.plane[2].stride = mediaBuffer->buffer[2].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                buf.plane[2].isEnabled = (2UL < mediaBuffer->numChannels) ? 1UL : 0UL;
                {
                    const ISYS_BufferType bufConst = buf;
                    ret = ISYS_FillBuffer(ISYS_PASS_IDX_CV, ISYS_OUT_IDX_MAX, &bufConst);
                    if (BCM_ERR_OK != ret) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    } else {
                        CAMERA_CompStateGlobal.cvReqState = CAMERA_CV_REQ_STATE_POSTED;
                    }
                }
            }

            msgRet = ISYS_GetFilledBuffer(ISYS_PASS_IDX_CV, ISYS_OUT_IDX_MAX, &buf);
            if ((BCM_ERR_OK == msgRet) && (CAMERA_CV_REQ_STATE_POSTED == CAMERA_CompStateGlobal.cvReqState)) {
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->seqNum = buf.sliceNum;
                mediaBuffer->timestamp = buf.timestamp;
                mediaBuffer->frameStart = 1U;
                mediaBuffer->frameEnd = 1U;
                mediaBuffer->frameNum = buf.frameNum;
                /* Copy final sizes */
                mediaBuffer->buffer[0].size = buf.plane[0].size;
                mediaBuffer->buffer[1].size = buf.plane[1].size;
                mediaBuffer->buffer[2].size = buf.plane[2].size;

                ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, mediaBufferIdx);
                if (BCM_ERR_OK != ret) {
                    CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                            __LINE__, 0UL, 0UL);
                }
                CAMERA_CompStateGlobal.cvReqState = CAMERA_CV_REQ_STATE_IDLE;

                msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortCVGlobal.dataQ, &mediaBufferIdx);
            }
        }

        /* Process Auxiliary messages */
        msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, &mediaBufferIdx);
        while (BCM_ERR_OK == msgRet) {
            MSYS_BufferType *mediaBuffer;
            int32_t status = BCM_ERR_OK;
            uint32_t i;

            mediaBuffer = &CAMERA_MediaBufferGlobal[CAMERA_OUT_PORT_IDX_AUX][mediaBufferIdx];

            /* Fill metadata from sensor */
            if (NULL != mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].data) {
                uint32_t usedSize = 0UL;
                uint32_t edSize[ISYS_SENSOR_EXP_IDX_MAX] = {0UL, 0UL};
                uint8_t * edBuf[ISYS_SENSOR_EXP_IDX_MAX] = {NULL, NULL};

                /* Fill offsets and sizes */
                for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
                    edBuf[i] = mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].data + usedSize;
                    edSize[i] = (uint32_t) CAMERA_CompStateGlobal.embOffsets.metaSize[i];
                    usedSize += edSize[i];
                }

                /* Check sizes */
                if (usedSize > mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].size) {
                    status = BCM_ERR_NOMEM;
                } else {
                    for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
                        if (0U != CAMERA_CompStateGlobal.embOffsets.metaSize[i]) {
                            status = ISYS_GetMetadataBuf(i,
                                                     edBuf[i],
                                                     CAMERA_CompStateGlobal.embOffsets.metaOffset[i],
                                                     CAMERA_CompStateGlobal.embOffsets.metaSize[i]);
                            if (BCM_ERR_OK != status) {
                                mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].size = 0UL;
                                /* Ignore error if metadata is not found */
                                if (BCM_ERR_NOT_FOUND == status) {
                                    status = BCM_ERR_OK;
                                }
                                break;
                            }
                        }
                    }
                    if (BCM_ERR_OK == status) {
                        mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].size =
                            usedSize;
                    }
                }
            }

            /* Fill ISYS_StatisticsType */
            if ((BCM_ERR_OK == status) &&
                (NULL != mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_ISYS_STATS].data)) {
                if (mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_ISYS_STATS].size >=
                                sizeof(ISYS_StatisticsType)) {
                    union {
                        uint8_t *pU8;
                        ISYS_StatisticsType *stats;
                    } statsPtr;

                    statsPtr.pU8 = mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_ISYS_STATS].data;
                    status = ISYS_GetStatistics(statsPtr.stats);
                    if (BCM_ERR_OK != status) {
                        mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_ISYS_STATS].size = 0UL;
                    } else {
                        mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_ISYS_STATS].size =
                                                                sizeof(ISYS_StatisticsType);
                    }
                } else {
                    status = BCM_ERR_NOMEM;
                }
            }

            /* Fill statistics from sensor */
            if ((BCM_ERR_OK == status) &&
                (NULL != mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_SENSOR_STATS].data)) {
                uint32_t usedSize = 0UL;
                uint32_t edSize[ISYS_SENSOR_EXP_IDX_MAX] = {0UL, 0UL};
                uint8_t * edBuf[ISYS_SENSOR_EXP_IDX_MAX] = {NULL, NULL};

                /* Fill offsets and sizes */
                for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
                    edBuf[i] = mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_SENSOR_STATS].data + usedSize;
                    edSize[i] = (uint32_t) CAMERA_CompStateGlobal.embOffsets.statsSize[i];
                    usedSize += edSize[i];
                }

                /* Check sizes */
                if (usedSize > mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_SENSOR_STATS].size) {
                    status = BCM_ERR_NOMEM;
                } else {
                    for (i = 0UL; i < ISYS_SENSOR_EXP_IDX_MAX; i++) {
                        if (0U != CAMERA_CompStateGlobal.embOffsets.statsSize[i]) {
                            status = ISYS_GetSensorStatsBuf(i,
                              edBuf[i],
                              CAMERA_CompStateGlobal.embOffsets.statsOffset[i],
                              CAMERA_CompStateGlobal.embOffsets.statsSize[i],
                              NULL);
                            if (BCM_ERR_OK != status) {
                                mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_SENSOR_STATS].size = 0UL;
                                /* Ignore error if statistics is not found */
                                if (BCM_ERR_NOT_FOUND == status) {
                                    status = BCM_ERR_OK;
                                }
                                break;
                            }
                        }
                    }
                    if (BCM_ERR_OK == status) {
                        mediaBuffer->buffer[MSYS_CHANNEL_TYPE_CAM_METADATA].size =
                            usedSize;
                    }
                }
            }

            mediaBuffer->response = status;
            ret = MSGQ_CompleteMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, mediaBufferIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
            msgRet = MSGQ_GetMsgIdx(&CAMERA_OutPortAuxGlobal.dataQ, &mediaBufferIdx);
        }
    }

    if ((ISYS_STATE_READY == ISYS_GetState())
        && (TUNER_STATE_READY == Tuner_GetState())
        && (CSN_STATE_READY == CSN_GetState())
        && (0UL == CAMERA_CompStateGlobal.tunerActive)) {
        /* Cancel any port message */
        CAMERA_CancelPortMsgs();

        /* Reset Flags */
        CAMERA_CompStateGlobal.cvReqState  = CAMERA_CV_REQ_STATE_IDLE;
        CAMERA_CompStateGlobal.mediaBufferIdx = 0xFFFFFFFFUL;
        CAMERA_SetState(CAMERA_STATE_READY);
#ifdef CAMERA_OutPortSE
        {
            ret = WDS_DisableSE(&CAMERA_OutPortSE);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }
#endif
    }

func_exit:
    if (BCM_ERR_OK != ret) {
        if ((ISYS_STATE_ERROR == ISYS_GetState())
            || (TUNER_STATE_ERROR == Tuner_GetState())
            || (CSN_STATE_ERROR == CSN_GetState())) {
            CAMERA_SetState(CAMERA_STATE_ERROR);
        }
    }

    if (NULL != processCmd) {
        processCmd->response = ret;
        if ((CAMERA_STATE_READY == CAMERA_CompStateGlobal.state) ||
            ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret))) {
#ifdef CAMERA_TUNING
            /* Cleanup any DMA request */
            if (ISYS_DMA_INVALID_INDEX != CAMERA_TuningStateGlobal.dmaJobIdx) {
                do {
                    ret = PP_DMAStatus(CAMERA_TuningStateGlobal.dmaJobIdx);
                    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
                        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                                __LINE__, 0UL, 0UL);
                    }
                } while (BCM_ERR_BUSY == ret);
                CAMERA_TuningStateGlobal.dmaJobIdx = ISYS_DMA_INVALID_INDEX;
            }
#endif /* CAMERA_TUNING */
            if (BCM_ERR_BUSY == ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
            ret = MSGQ_CompleteMsgIdx(&CAMERA_CmdQGlobal, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RUN_STATE_HANDLER_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }
    }
}

/** @brief Camera TASK

    Main task context for camera media component

    @behavior Sync, Non-reentrant

    @pre None

    @param  void

    @return void

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_CUSTOM_CMD_MACRO
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @limitations None

    @code{.unparsed}
    do
        ret = BCM_WaitEvent(CAM_EVENTS |
                            SystemEvent8 |
                            SystemEvent9 |
                            SystemEvent10 |
                            CAM_EVENT_PTM)
        ret = BCM_GetEvent(CAMERA_Task, &mask)
        Check ret
        CAMERA_CompStateGlobal.event = (uint32_t) mask
        ret = BCM_ClearEvent(mask)
        Check ret
        if (0UL != (CAM_EVENT_PTM & mask))
            ptmState = CAMERA_PTM_LOAD_STATE_RCVD

        switch(CAMERA_CompStateGlobal.state)
        case CAMERA_STATE_RESET:
            CAMERA_ResetStateHandler()
            break
        case CAMERA_STATE_INIT:
            CAMERA_InitStateHandler()
            break
        case CAMERA_STATE_READY:
            CAMERA_ReadyStateHandler()
            break
        case CAMERA_STATE_RUNNING:
            CAMERA_RunStateHandler()
            break
        case CAMERA_STATE_ERROR:
            CAMERA_ErrorStateHandler()
            break
        default:
            break;
    while (CAMERA_STATE_RESET != state)

    ret = BCM_TerminateTask()
    Report error
    @endcode

*/
TASK(CAMERA_Task)
{

    int32_t ret;
    BCM_EventMaskType mask = 0UL;

#ifdef CAMERA_StateSE
    {
        WDS_ConfigType wdsCfg;

        wdsCfg.mode = WDS_MODE_LOGICAL;
        wdsCfg.deadlineInMs = 0UL;
        wdsCfg.minFrequency = 0UL;
        wdsCfg.maxFrequency = 0UL;
        wdsCfg.lStart = (uint8_t) CAMERA_STATE_RESET;
        wdsCfg.lEnd = (uint8_t) CAMERA_STATE_RESET;
        BCM_MemSet(CAMERA_StateSE.buffer, WDS_INVALID_GRAPH_BUF_ENTRY,
                    ((uint32_t) CAMERA_StateSE.maxState) *
                    ((uint32_t) CAMERA_StateSE.maxTransitionsPerState));
        BCM_MemCpy(CAMERA_StateSE.buffer, &CAMERA_StateSEGraph[0][0], sizeof(CAMERA_StateSEGraph));
        ret = WDS_EnableSE(&CAMERA_StateSE, &wdsCfg);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }

        /* Notify RESET state as starting point */
        ret = WDS_NotifyState(&CAMERA_StateSE,
                              (WDS_StateType) CAMERA_STATE_RESET);
        if (ret != BCM_ERR_OK) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
    }
#endif

    do {
        ret = BCM_WaitEvent(CAM_EVENTS |
                            SystemEvent8 |
                            SystemEvent9 |
                            SystemEvent10 |
                            CAM_EVENT_PTM |
                            CAM_EVENT_CSN_SAFE_STATE |
                            CAM_EVENT_CSN_RESET);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
        ret = BCM_GetEvent(CAMERA_Task, &mask);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
        CAMERA_CompStateGlobal.event = (uint32_t) mask;
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }

        if (0UL != (CAM_EVENT_PTM & mask)) {
            CAMERA_CompStateGlobal.ptmState = CAMERA_PTM_LOAD_STATE_RCVD;
        }

        if (0UL != (CAM_EVENT_CSN_RESET & mask)) {
            uint32_t csnMask = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA,
                                             CSN_EVENT_SOURCE_RESET);
            if (CSN_STATUS_ERROR == (csnMask & CSN_STATUS_ERROR)) {
                CAMERA_SetState(CAMERA_STATE_ERROR);
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }

        if (0UL != (CAM_EVENT_CSN_SAFE_STATE & mask)) {
            uint32_t csnMask = CSN_ProcessEvent(CSN_CLIENT_IDX_CAMERA,
                                             CSN_EVENT_SOURCE_SAFETY);
            if (CSN_STATUS_ERROR == (csnMask & CSN_STATUS_ERROR)) {
                CAMERA_SetState(CAMERA_STATE_ERROR);
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                        __LINE__, 0UL, 0UL);
            }
        }

        if (0UL != (TUNER_EVENT_ERROR & mask)) {
            CAMERA_SetState(CAMERA_STATE_ERROR);
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }

        switch(CAMERA_CompStateGlobal.state) {
        case CAMERA_STATE_RESET:
            CAMERA_ResetStateHandler();
            break;
        case CAMERA_STATE_INIT:
            CAMERA_InitStateHandler();
            break;
        case CAMERA_STATE_READY:
            CAMERA_ReadyStateHandler();
            break;
        case CAMERA_STATE_RUNNING:
            CAMERA_RunStateHandler();
            break;
        case CAMERA_STATE_ERROR:
            CAMERA_ErrorStateHandler();
            break;
        default:
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, BCM_ERR_UNKNOWN,
                    __LINE__, CAMERA_CompStateGlobal.state, 0UL);
            break;
        }
    } while ((CAMERA_STATE_RESET != CAMERA_CompStateGlobal.state) ||
             (0U != CAMERA_CompStateGlobal.csnResetInProgress));

#ifdef CAMERA_StateSE
    {
        ret = WDS_DisableSE(&CAMERA_StateSE);
        if (BCM_ERR_OK != ret) {
            CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                    __LINE__, 0UL, 0UL);
        }
    }
#endif

    ret = BCM_TerminateTask();
    if (BCM_ERR_OK != ret) {
        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_TASK_PART_PROC, ret,
                __LINE__, 0UL, 0UL);
    }
}

/**
    Detailed design of CAMERA_SendCmd

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_SEND_CMD_PROC

    @code{.c}
    retVal = BCM_ERR_INVAL_PARAMS;
    if ((NULL != aCmd)
        && (MSYS_COMP_ID_CAM0 == aCmd->compID)
        && (NULL != aHdr)) {
        if (MSYS_COMP_CMD_INIT == aCmd->cmd)
            ret = BCM_ActivateTask(CAMERA_Task);
        Post the command to the component's Command Queue
    return retVal
    @endcode
*/
static int32_t CAMERA_SendCmd(const MSYS_CmdType *const aCmd, const uint32_t aMask,
        const MSGQ_MsgHdrType ** const aHdr)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;
    int32_t ret;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_CAM0 == aCmd->compID)
        && (NULL != aHdr)) {
        if (MSYS_COMP_CMD_INIT == aCmd->cmd) {
            ret = BCM_ActivateTask(CAMERA_Task);
            if (BCM_ERR_OK != ret) {
                CAMERA_ReportError(BRCM_SWDSGN_CAMERA_SEND_CMD_PROC,
                        ret, __LINE__, 0UL, 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        retVal = MSGQ_SendMsg(&CAMERA_CmdQGlobal, (const void * const)aCmd, aMask, aHdr);
    } else {
        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_SEND_CMD_PROC,
                           BCM_ERR_INVAL_PARAMS,
                           __LINE__, 0UL, 0UL);
    }

    return retVal;
}

/**
    Detailed design of CAMERA_RecvResp

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_RECV_RESP_PROC

    @code{.c}
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd))
        retVal = MSGQ_RecvMsg(&CAMERA_CmdQGlobal, aHdr, (void * const) aCmd)
    else
        Report error
    return retVal
    @endcode
*/
static int32_t CAMERA_RecvResp(const MSGQ_MsgHdrType * const aHdr, MSYS_CmdType *const aCmd)
{
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        retVal = MSGQ_RecvMsg(&CAMERA_CmdQGlobal, aHdr, (void * const) aCmd);
    } else {
        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_RECV_RESP_PROC,
                           BCM_ERR_INVAL_PARAMS,
                           __LINE__, 0UL, 0UL);
    }

    return retVal;
}

/**
    Detailed design of CAMERA_GetMediaOutPort

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
    @trace #BRCM_SWARCH_CAMERA_GET_M_OUT_PORT_PROC
    @trace #BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO

    @code{.c}
    const MSYS_OutPortType *outPort = NULL

    switch(aIndex)
        case CAMERA_OUT_PORT_IDX_HV:
            outPort = &CAMERA_OutPortHVGlobal
            break;
        case CAMERA_OUT_PORT_IDX_CV:
            outPort = &CAMERA_OutPortCVGlobal
            break;
        case CAMERA_OUT_PORT_IDX_AUX:
            outPort = &CAMERA_OutPortAuxGlobal
            break;
        default:
            Report error
            break;

    return outPort
    @endcode
*/
static const MSYS_OutPortType* CAMERA_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    switch(aIndex)
    {
    case CAMERA_OUT_PORT_IDX_HV:
        outPort = &CAMERA_OutPortHVGlobal;
        break;
    case CAMERA_OUT_PORT_IDX_CV:
        outPort = &CAMERA_OutPortCVGlobal;
        break;
    case CAMERA_OUT_PORT_IDX_AUX:
        outPort = &CAMERA_OutPortAuxGlobal;
        break;
    default:
        CAMERA_ReportError(BRCM_SWDSGN_CAMERA_GET_MEDIA_OUT_PORT_PROC,
                           BCM_ERR_INVAL_PARAMS,
                           aIndex, __LINE__, 0UL);
        break;
    }

    return outPort;
}

/**
    @brief Camera Media Component function table instance

    Constant instance of statically allocated function table for camera media
    component.

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
*/
static const MSYS_CompFnTblType CAMERA_FnTable =
{
    CAMERA_SendCmd,
    CAMERA_RecvResp,
    CAMERA_GetMediaOutPort,
    NULL,
};

/** @} */

/**
    @addtogroup grp_camera_ifc
    @{
*/

/**
    Detailed design of CAMERA_GetCompFnTable

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_AUXILIARY_DATA
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWARCH_CAMERA_GET_COMP_FN_TABLE

    @code{.c}
    return &CAMERA_FnTable
    @endcode
*/
const MSYS_CompFnTblType* CAMERA_GetCompFnTable(void)
{
    return &CAMERA_FnTable;
}

/** @} */
