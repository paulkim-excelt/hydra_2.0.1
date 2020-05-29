/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_camera_impl
    @{
    @file camera_internal.h
    @brief Interfaces used by camera media component implementation
    This file contains common structures used by camera media component
    @version 0.1 Initial version
*/

#ifndef CAMERA_INTERNAL_H
#define CAMERA_INTERNAL_H

/**
    @name Camera internal API IDs
    @{
    @brief Internal API IDs for Camera Media Component
*/
#define BRCM_SWARCH_CAMERA_ALIGN_MACRO           (0x0U) /**< @brief #CAMERA_ALIGN           */
#define BRCM_SWARCH_CAMERA_STATE_TYPE            (0x1U) /**< @brief #CAMERA_State           */
#define BRCM_SWARCH_CAMERA_PTM_LOAD_STATE_TYPE   (0x2U) /**< @brief #CAMERA_PTM_Load_State  */
#define BRCM_SWARCH_CAMERA_CV_REQ_STATE_TYPE     (0x3U) /**< @brief #CAMERA_CV_REQ_State    */
#define BRCM_SWARCH_CAMERA_EVENT_TYPE            (0x4U) /**< @brief #CAMERA_Event           */
#define BRCM_SWARCH_CAMERA_COMP_CONTEXT_TYPE     (0x5U) /**< @brief #CAMERA_CompState       */
#define BRCM_SWARCH_CAMERA_TUNING_FC_STATE_TYPE  (0x6U) /**< @brief #CAMERA_TUNING_FC_State */
#define BRCM_SWARCH_CAMERA_TUNING_CONTEXT_TYPE   (0x7U) /**< @brief #CAMERA_TuningStateType */

/**
    @brief Align @a x to @a y bytes

    Macro for making the data aligned, 'y' needs to be a power of 2

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_ALIGN(x, y)                 \
    ((((uint32_t)(x)) + (((uint32_t)(y)) - 1UL)) & (~(uint32_t)((y) - 1UL)))

/**
    @name CAMERA_State
    @{
    @brief Camera state type
    Refer @ref sec_camera_fsm for more details

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
*/
typedef uint32_t CAMERA_State;          /**< @brief CAMERA_State typedef */
#define CAMERA_STATE_RESET      (0UL)   /**< @brief Reset state          */
#define CAMERA_STATE_INIT       (1UL)   /**< @brief Initialized state    */
#define CAMERA_STATE_READY      (2UL)   /**< @brief Ready state          */
#define CAMERA_STATE_RUNNING    (3UL)   /**< @brief Running state        */
#define CAMERA_STATE_ERROR      (4UL)   /**< @brief Error state          */
/** @} */

/**
    @name CAMERA_PTM_Load_State
    @{
    @brief PTM Load state type
    IDLE -> REQ (Requested state) -> RCVD (Response received) -> IDLE

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
typedef uint32_t CAMERA_PTM_Load_State;/**< @brief CAMERA_PTM_Load_State typedef             */
#define CAMERA_PTM_LOAD_STATE_IDLE     (0UL)  /**< @brief PTM load state machine is idle     */
#define CAMERA_PTM_LOAD_STATE_REQ      (1UL)  /**< @brief PTM Load Requested                 */
#define CAMERA_PTM_LOAD_STATE_RCVD     (2UL)  /**< @brief Recieved response for the PTM load */
/** @} */

/**
    @name CAMERA_CV_REQ_State
    @{
    @brief CV Request state
    IDLE -> POSTED -> IDLE (On completion)

    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
*/
typedef uint32_t CAMERA_CV_REQ_State; /**< @brief CAMERA_CV_REQ_State typedef             */
#define CAMERA_CV_REQ_STATE_IDLE             (0UL) /**< @brief CV Request state machine is idle */
#define CAMERA_CV_REQ_STATE_POSTED           (1UL) /**< @brief CV Request is posted             */
/** @} */

/**
    @brief Camera asynchronous event

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
*/
typedef uint32_t CAMERA_Event;

/**
    @brief Camera component internal state

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
*/
typedef struct _CAMERA_CompState {
    CAMERA_State state;              /**< @brief state as #CAMERA_State             */
    uint8_t csnResetInProgress;      /**< @brief Is CSN reset in progress?          */
    uint32_t curSliceNum;            /**< @brief Current slice number               */
    uint32_t lastSliceNum;           /**< @brief Last slice number                  */
    uint32_t tunerActive;            /**< @brief Is tuner task active or not        */
    CAMERA_Event event;              /**< @brief event as #CAMERA_Event             */
    IMGL_LoadStatusType imglStatus;  /**< @brief Image Loader Status type           */
    const MSGQ_MsgHdrType* hdr;      /**< @brief hdr as #MSGQ_MsgHdrType            */
    CAMERA_PTM_Load_State ptmState;  /**< @brief ptmState as #CAMERA_PTM_Load_State */
    CAMERA_CV_REQ_State cvReqState;  /**< @brief cvReqState as #CAMERA_CV_REQ_State */
    CSN_EmbOffsetsType embOffsets;   /**< @brief embOffsets as #CSN_EmbOffsetsType  */
    uint32_t mediaBufferIdx;         /**< @brief Media buffer index to be stored    */
} CAMERA_CompState;

/**
    @name CAMERA_TUNING_FC_State
    @{
    @brief Camera tuning Frame Capture state
    IDLE -> REQ -> RUN -> IDLE (On completion)

    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
*/
typedef uint32_t CAMERA_TUNING_FC_State; /**< @brief CAMERA_TUNING_FC_State typedef               */
#define CAMERA_TUNING_FC_STATE_IDLE      (0UL) /**< @brief Camera Tuning FC state machine is idle */
#define CAMERA_TUNING_FC_STATE_REQ       (1UL) /**< @brief Frame capture is requested             */
#define CAMERA_TUNING_FC_STATE_RUN       (2UL) /**< @brief Frame capture request is processing    */
#define CAMERA_TUNING_FC_STATE_DONE      (3UL) /**< @brief Frame capture request is completed     */
/** @} */

/**
    @brief Camera Tuning state Type

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
*/
typedef struct _CAMERA_TuningStateType {
    CAMERA_TUNING_FC_State fcState;   /**< @brief state as #CAMERA_TUNING_FC_State */
    uint32_t sliceNum;                /**< @brief Slice number requested           */
    uint32_t dmaJobIdx;               /**< @brief DMA job index for slice DMA      */
} CAMERA_TuningStateType;

#endif /* CAMERA_INTERNAL_H */
/** @} */
