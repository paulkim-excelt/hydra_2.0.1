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
    @defgroup grp_camera_ifc Interface
    @ingroup grp_camera

    @defgroup grp_tuners Tuners
    @ingroup grp_camera
    @addtogroup grp_tuners
    @{
    This section contains configuration interfaces for various tuners supported
    by Camera subsystem.
    @}

    @addtogroup grp_camera_ifc
    @{
    @section sec_camera_overview Overview
    @image html camera_overview.jpg "Camera Media Component hierarchy"
    Above figure depicts a block diagram representation of Camera system.
    Camera Media Component abstracts different imaging components to provide
    YUV streaming functionality.

    Camera Media Component manages following 3 sub-components,
    -# Camera Sensor driver
        <BR>Camera sensor driver contains following two parts,
        -# Camera sensor common driver (CSN) to manages IIC interactions
        -# Camera module driver (CSM) to enable sensor specific functionalities
           (eg: parsing of meta-data and statistics)
    -# ISYS Library
        <BR>ISYS library is the core of the camera sub-system which abstracts camera
        receiver as well as ISP.
    -# Tuner framework
        <BR>Camera requires dynamic update of parameters to achieve better
        image quality for changing lighting conditions. Tuner framework manages
        supported algorithms(tuners) to tune various imaging (camera sensor as
        well as ISP) parameters.

    @todo Add links to individual components once available

    Camera Media Component exposes following media output ports,
    -# Human Vision(HV) port
        <BR>HV port streams camera data for viewfinder applications
    -# Computer Vision(CV) port
        <BR>CV port streams camera data for CV applications
        requiring different processing compared to HV.
    -# Auxiliary port
        <BR>Auxiliary port outputs following non-image data,
        -# Image metadata from camera sensor
        -# Image statistics from ISYS
        -# Image statistics from camera sensor

    @section sec_camera_fsm State Machine
    @image html camera_state_machine.jpg
    Above figure depicts the state machine for Camera Media Component. It also
    depicts the commands which triggers the state transitions.
    Camera Media Component has the following states:
    - @b Reset: All sub-components are in uninitialized state.
    - @b Initialized: Individual components are initialized.
    - @b Ready: Individual components are configured. Camera can be reconfigured
      in @b Ready state as well.
    - @b Running: Individual components are enabled and Camera is streaming.
    - @b Error: One or more individual components went to error state.

    @todo Use svg for state machine diagram

    @section sec_camera_seq Sequence Diagrams
    @tagseq camera_ifc_seq_diagram.svg "Camera Interface Sequence diagram"
    @startseq
    span_width = 256;
    node_width = 200;
    group {label = "Camera Client"; color = "#EFFFFF";  CAMERA_CLIENT;};
    group {label = "Camera Media Component Interface"; color = "#FFEFFF"; CAMERA_IF;};
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_INIT_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_INIT_CMD)"];
    === Initialization Complete ===
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Mode config"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with ISP Config"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with UNICAM Config"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Sensor Config"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_CONFIG_CMD) with Tuner config"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_CONFIG_CMD)"];
    === Configuration Complete ===
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_START_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_START_CMD)"];
    === Start Complete ===
    CAMERA_CLIENT  => CAMERA_IF[label = "MSGQ_SendMsg(outPort->dataQ)"];
    === Data buffer queue Complete ===
    CAMERA_IF  => CAMERA_CLIENT[label = "MSGQ_CompleteMsgIdx(outPort->dataQ)"];
    === Data buffer fill Complete ===
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_STOP_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_STOP_CMD)"];
    === Stop Complete ===
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_SendCmd(COMP_DEINIT_CMD)"];
    CAMERA_CLIENT  => CAMERA_IF[label = "CAMERA_RecvResp(COMP_DEINIT_CMD)"];
    === De-initialization Complete ===
    @endseq

    Above diagram depicts the typical sequence of camera media component
    interface interaction by the camera client.

    Client shall move the component to Running state using sequence of
    CAMERA_SendCmd with COMP_INIT_CMD, COMP_CONFIG_CMD, COMP_START_CMD before
    queuing buffers to pull camera output.

    Component receives data requests on its output port and return the buffers
    once it is filled.

    After the streaming session is completed, client shall stop streaming
    followed by de-initialization of component.

    @section sec_camera_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   TBD                                             |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   TBD                                             |

    @todo Add performance details

    @includedoc camera.dep

    @limitations None

    @file camera.h
    @brief Data structures and API's of the Camera Media Component
    This file describes the interfaces exposed by the Camera Media Component to
    be used by peer elements.
    @version 0.23 Added CAMERA_GetCompFnTable and removed component APIs
    @version 0.22 Added interface and design details
    @version 0.21 Misra fixes
    @version 0.2 Fixed some TBDs
    @version 0.1 Initial version
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <msg_queue.h>
#include <component.h>
#include <media_port.h>

/**
    @name Camera Media Component API IDs
    @{
    @brief API IDs for Camera Media Component
*/
#define BRCM_SWARCH_CAMERA_CUSTOM_CMD_MACRO               (0x0U)  /**< @brief #CAMERA_CUSTOM_CMD_CAPTURE_FRAME */
#define BRCM_SWARCH_CAMERA_OUT_PORT_IDX_MACRO             (0x1U)  /**< @brief #CAMERA_OUT_PORT_IDX_HV          */
#define BRCM_SWARCH_CAMERA_NUM_ISP_INT_BUFFERS_MACRO      (0x2U)  /**< @brief #CAMERA_NUM_ISP_INT_BUFFERS      */
#define BRCM_SWARCH_CAMERA_ISP_HV_OUT_BPS_MACRO           (0x3U)  /**< @brief #CAMERA_ISP_HV_OUT_BPS           */
#define BRCM_SWARCH_CAMERA_MAX_NUM_RAW_CHANNELS_MACRO     (0x4U)  /**< @brief #CAMERA_MAX_NUM_RAW_CHANNELS     */
#define BRCM_SWARCH_CAMERA_RAW_OUT_BPP_MACRO              (0x5U)  /**< @brief #CAMERA_RAW_OUT_BPP              */
#define BRCM_SWARCH_CAMERA_MAX_WIDTH_SENSOR_MACRO         (0x6U)  /**< @brief #CAMERA_MAX_SENSOR_WIDTH         */
#define BRCM_SWARCH_CAMERA_MAX_HEIGHT_SENSOR_MACRO        (0x7U)  /**< @brief #CAMERA_MAX_SENSOR_HEIGHT        */
#define BRCM_SWARCH_CAMERA_GET_COMP_FN_TABLE              (0x8U)  /**< @brief #CAMERA_GetCompFnTable           */
/** @} */

/**
    @name Camera custom commands
    @{
    @brief Custom command macros for camera media component

    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
*/
#define CAMERA_CUSTOM_CMD_CAPTURE_FRAME (MSYS_COMP_CMD_CUSTOM0)   /**< @brief Command to capture yuv frame */
#define CAMERA_CUSTOM_CMD_CAPTURE_SLICE (MSYS_COMP_CMD_CUSTOM1)   /**< @brief Command to capture yuv slice */
/** @} */

/**
    @name Camera output port index
    @{
    @brief Index for camera output ports

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_STATS_META_STREAM
*/
#define CAMERA_OUT_PORT_IDX_HV   (0UL) /**< @brief Port index for HV                    */
#define CAMERA_OUT_PORT_IDX_CV   (1UL) /**< @brief Port index for CV                    */
#define CAMERA_OUT_PORT_IDX_AUX  (2UL) /**< @brief Port index for Auxiliary data        */
#define CAMERA_OUT_PORT_IDX_MAX  (3UL) /**< @brief Maximum number of camera outout port */
/** @} */

#ifndef CAMERA_NUM_ISP_INT_BUFFERS
/**
    @brief Number of ISP internal buffers

    Macro for number of Macro Block(MB) rows to be allocated for ISP
    Supported values: Minimum 3UL
    (1 for ISP, 1 for DMA, and +1 considering a slow client)

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_NUM_ISP_INT_BUFFERS (3UL)
#endif /* CAMERA_NUM_ISP_INT_BUFFERS */

#ifndef CAMERA_ISP_HV_OUT_BPS
/**
    @brief Bits per sample considered for ISP HV output

    Bits per sample considered for ISP HV output.
    Supported values: 8UL, 16UL

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_ISP_HV_OUT_BPS   (8UL)
#endif /* CAMERA_ISP_HV_OUT_BPS */

#ifndef CAMERA_MAX_NUM_RAW_CHANNELS
/**
    @brief Number of streams coming out of sensor

    Number of streams(different exposures) coming out of sensor for ISP to
    process.
    Supported values: 1UL, 2UL

    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_MAX_NUM_RAW_CHANNELS (2UL)
#endif /* CAMERA_MAX_NUM_RAW_CHANNELS */

#ifndef CAMERA_RAW_OUT_BPP
/**
    @brief Bits per pixel to be considered for RAW input

    Macro for bits per pixel considered for RAW input

    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_RAW_OUT_BPP      (16UL)
#endif /* CAMERA_RAW_OUT_BPP */

#ifndef CAMERA_MAX_SENSOR_WIDTH
/**
    @brief Maximum sensor width

    Macro for maximum width of sensor considered during allocation

    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_MAX_SENSOR_WIDTH (MEDIA_SYSTEM_MAX_VIDEO_WIDTH)
#endif /* CAMERA_MAX_SENSOR_WIDTH */

#ifndef CAMERA_MAX_SENSOR_HEIGHT
/**
    @brief Maximum sensor height

    Macro for maximum height of sensor considered during allocation

    @trace #BRCM_SWREQ_CAMERA_DUAL_PASS
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
*/
#define CAMERA_MAX_SENSOR_HEIGHT (MEDIA_SYSTEM_MAX_VIDEO_HEIGHT)
#endif /* CAMERA_MAX_SENSOR_HEIGHT */

/** @brief Get handle to camera media component function table

    This is an interface function for framework/application to get handle to
    camera media component function table.

    @behavior Sync, Re-entrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval     MSYS_CompFnTblType*           Pointer to function table

    @post None

    @trace #BRCM_SWREQ_CAMERA_MEDIA_COMPONENT
    @trace #BRCM_SWREQ_CAMERA_CONFIGURATION
    @trace #BRCM_SWREQ_CAMERA_TUNING_SLICE_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_TUNING_FRAME_CAPTURE
    @trace #BRCM_SWREQ_CAMERA_AUXILIARY_DATA

    @limitations None
*/
const MSYS_CompFnTblType* CAMERA_GetCompFnTable(void);

#endif /* CAMERA_H */
/** @} */
