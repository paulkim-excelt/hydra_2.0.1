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
 IMMEDIATELY NOTIFY BROADCOM AND VGCDISHWCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY VGCDISHWCLAIMS ANY AND ALL IMPLIED
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
    @defgroup grp_vgcd_service_ifc Interface
    @ingroup grp_vgcd_service

    @addtogroup grp_vgcd_service_ifc
    @{

    @section sec_vgc_drv_overview Overview
    The VGC Service Driver consists of VGC Service API's

    @includedoc  todo

    @limitations todo

    @file vgc_display_service.h

    @brief VGC interface

    This header file contains the interface functions for VGC Service driver
    VGC Service exposes interfaces necessary to Access from component

    @version todo
*/

#ifndef VGC_DISPLAY_SERVICE_H
#define VGC_DISPLAY_SERVICE_H

#include <stdint.h>
#include <string.h>
#include <msg_queue.h>
#include <vgc_display.h>

/**
     @name VGCD ARCH IDs
     @{
     @brief Arch IDs for  VGCD
*/

#define BRCM_SWARCH_VGCD_INIT_PROC                    (0x01U)  /**< @brief #VGCD_Init               */
#define BRCM_SWARCH_VGCD_DEINIT_PROC                  (0x02U)  /**< @brief #VGCD_Deinit             */
#define BRCM_SWARCH_VGCD_ENABLE_PROC                  (0x03U)  /**< @brief #VGCD_Enable             */
#define BRCM_SWARCH_VGCD_DISABLE_PROC                 (0x04U)  /**< @brief #VGCD_Disable            */
#define BRCM_SWARCH_VGCD_GETCTRL_RESPONSE_PROC        (0x05U)  /**< @brief #VGCD_GetCtrlResponse    */
#define BRCM_SWARCH_VGCD_BRIGHTNESS_PROC              (0x06U)  /**< @brief #VGCD_SetBrightness      */
#define BRCM_SWARCH_VGCD_BACKGROUND_PROC              (0x07U)  /**< @brief #VGCD_SetBackgroundColor */
#define BRCM_SWARCH_VGCD_GETSTATS_PROC                (0x08U)  /**< @brief #VGCD_GetStats           */
#define BRCM_SWARCH_VGCD_CSC_PROC                     (0x09U)  /**< @brief #VGCD_CSC                */
#define BRCM_SWARCH_VGCD_COLORKEY_PROC                (0x0AU)  /**< @brief #VGCD_ColorKey           */
#define BRCM_SWARCH_VGCD_PROCESS_DEINIT_PROC          (0x0BU)  /**< @brief #VGCD_ProcessDeInit      */
#define BRCM_SWARCH_VGCD_CTRL_PROCESSMSG_PROC         (0x0CU)  /**< @brief #VGCD_CtrlProcessMsg     */
/** @} */

/** @brief Asynchronous call to Initialise the VGCDHw Service

    Initialise the display service with the input configuration

    @param[in]    aCfg                   Input parameters to configure composer,
                                         backlight and controller
    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @retVal       BCM_ERR_UNKNOWN        Failed to activate service task
    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_Init(const VGC_DispCfgType *const aCfg,
                            const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Asynchronous call to Deinitialise the VGCDHw service

    To be called after DispSrvc_Stop is called and VGCDHw is stopped,
    this API will deinitialise the service

    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_Deinit(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Asynchronous call to Enable VGCDHw

    This API will start displaying on the screen. If no buffers are
    queued by any of the renderers, then the background is displayed

    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_Enable(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Asynchronous call to Disable VGCDHw

    This API will disable display and turn off the panel

    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_Disable(const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Get the status of asynchronous control message

    This API will get the status of the asynchronous control message whose
    header has to be passed by the caller.

    @param[in]    aHdr                   Pointer to message header

    @return       Return value of the asynchronous call
    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_GetCtrlResponse(const MSGQ_MsgHdrType* const aHdr);

/** @brief Asynchronous call to Set the brightness of display

    This API is sets the brightness of the display panel

    @param[in]    aPercentBrightness     Value of brightness in percentage
    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_SetBrightness(uint32_t aPercentBrightness,
                                     const uint32_t aClientMask,
                                     const MSGQ_MsgHdrType** const aHdr);

/** @brief Asynchronous call to Set background color

    This API sets the background color of the canvas

    @param[in]    aPixel                 Pointer to pixel color information
    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_SetBackgroundColor(const VGC_PixelType *const aPixel,
                                          const uint32_t aClientMask,
                                          const MSGQ_MsgHdrType** const aHdr);

/** @brief Asynchronous call to Get Statistics

    This API provides the statistics information from VGCDHw controller

    @param[inout] aStats                 Pointer to retrieve statistics information
    @param[in]    aClientMask            Client Signal mask
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return       Return value of MSGQ_SendMsg

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_GetStats(VGC_DispStatsType *const aStats,
                                const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Enable/Disable CSC co-efficients

    This API is used to enable/disable Color Space Conversion. Default CSC
    matrix co-efficients can be overridden by required values.

    @param[in]   aEnable                 Boolean to indicate whether to enable/disable
    @param[in]   aCsc                    Pointer to memory where CSC Matrix
                                         co-efficients are available
                                         Ignored if NULL, default values will be used
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return      BCM_ERR_OK              CSC enabled/disabled as requested
    @return      BCM_ERR_UNINIT          Component is not initialised

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_CSC(uint32_t aEnable, const VGC_CSCType *const aCsc,
                           const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);

/** @brief Enable/Disable Color Keying

    This API is used to enable/disable Color Keying.

    @param[in]   aEnable                 Boolean to indicate whether to enable/disable
    @param[in]   aColorKey               Pointer to memory where Colour Keying
                                         information is available
    @param[inout] aHdr                   Pointer to Pointer to message header

    @return      BCM_ERR_OK              Color Key enabled/disabled as requested
    @return      BCM_ERR_UNINIT          Component is not initialised
    @return      BCM_ERR_INVAL_PARAMS    aColorKey is NULL

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_ColorKey(uint32_t aEnable, const VGC_ColorKeyType *const aColorKey,
                                const uint32_t aClientMask, const MSGQ_MsgHdrType** const aHdr);
/** @brief Process Deinit

    Deinitialize the Process

    @param       none

    @return        BCM_ERR_OK              Deinialized

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_ProcessDeInit(void);

/** @brief Process asynchronous messages

    Process any asynchronous messages in the queue

    @param       none

    @return       Return value of MSGQ_GetMsgIdx

    @trace #BRCM_SWREQ_VGCD_SERVICE
*/
extern int32_t VGCD_CtrlProcessMsg(void);

#endif /* VGC_DISPLAY_SERVICE_H */

/** @} */
