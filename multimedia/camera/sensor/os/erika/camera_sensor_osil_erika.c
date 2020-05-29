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
    @defgroup grp_csn_il_impl Implementation
    @ingroup grp_csn_il
    @addtogroup grp_csn_il_impl
    @{
    @file camera_sensor_osil_erika.c
    @brief Camera sensor Integration
    This source file contains the integration of Camera sensor to system
    @version 0.1 Initial draft
*/

#include <osil/bcm_osil.h>
#include <cam_erika.h>
#include <camera_sensor_osil.h>

/**
    @name Camera Sensor OSIL Design IDs
    @{
    @brief Design IDs for CSN OSIL
*/
#define BRCM_SWDSGN_CSN_OSIL_REPORT_ERROR_PROC  (0x81U) /**< @brief #CSN_OSIL_ReportError */
#define BRCM_SWDSGN_CSN_SAFE_STATE_CB_PROC      (0x82U) /**< @brief #CSN_SafeStateCb      */
/** @} */

/**
    Detailed design of CSN_OSIL_ReportError

    @trace #BRCM_SWARCH_CSN_SAFE_STATE_CB_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static void CSN_OSIL_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CSN_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    Detailed design of CSN_SafeStateCb

    @trace #BRCM_SWARCH_CSN_SAFE_STATE_CB_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
extern void CSN_SafeStateCb(void)
{
    int32_t ret = BCM_ERR_OK;

    ret = BCM_SetEvent(CSN_TASK, CAM_EVENT_CSN_SAFE_STATE);
    if (BCM_ERR_OK != ret) {
        CSN_OSIL_ReportError(BRCM_SWDSGN_CSN_SAFE_STATE_CB_PROC,
                (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }
}

/** @} */
