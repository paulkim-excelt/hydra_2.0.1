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
    @defgroup grp_csn_board_impl Board specific sensor implementation
    @ingroup grp_csn_board

    @addtogroup grp_csn_board_impl
    @{
    @section sec_csn_board_impl_overview Overview
    @todo Add overview

    @file camera_sensor_board.c
    @brief CSN Board specific sensor implementation
    This source file contains the implementation of functions for Board specific
    CSN APIs.
    @version 0.1 Initial version
*/

#include <camera_sensor_board.h>
#include <cam_erika.h>
#ifdef ENABLE_CAM_MODULE_OX3A10
#include <gpio.h>
#include <pca9673.h>
#endif /* ENABLE_CAM_MODULE_OX3A10 */
#include <bcm_time.h>

/**
    @name CSN-Board Design IDs
    @{
    @brief Design IDs for CSN-Board
*/
#define BRCM_SWDSGN_CSN_BOARD_RESET_STATE_TYPE                    (0x80U)   /**< @brief #CSN_BoardResetStateType             */
#define BRCM_SWDSGN_CSN_BOARD_CONTEXT_TYPE                        (0x81U)   /**< @brief #CSN_BoardContextType                */
#define BRCM_SWDSGN_CSN_BOARD_CONTEXT_GLOBAL                      (0x82U)   /**< @brief #CSN_BoardContext                    */
#define BRCM_SWDSGN_CSN_BOARD_SEND_RESET_PULSE_PROC               (0x83U)   /**< @brief #CSN_BoardSendResetPulse             */
#define BRCM_SWDSGN_CSN_BOARD_GET_RESET_PULSE_STATUS_PROC         (0x84U)   /**< @brief #CSN_BoardGetResetPulseStatus        */
#define BRCM_SWDSGN_CSN_BOARD_SEND_RESET_RELEASE_PULSE_PROC       (0x85U)   /**< @brief #CSN_BoardSendResetReleasePulse      */
#define BRCM_SWDSGN_CSN_BOARD_GET_RESET_RELEASE_PULSE_STATUS_PROC (0x86U)   /**< @brief #CSN_BoardGetResetReleasePulseStatus */
#define BRCM_SWDSGN_CSN_BOARD_RESET_DELAY_PROC                    (0x87U)   /**< @brief #CSN_BoardResetDelay                 */
#define BRCM_SWDSGN_CSN_BOARD_PRE_RESET_DELAY_PROC                (0x88U)   /**< @brief #CSN_BoardPreResetDelay              */
#define BRCM_SWDSGN_CSN_BOARD_POST_RESET_DELAY_PROC               (0x89U)   /**< @brief #CSN_BoardPostResetDelay             */
#define BRCM_SWDSGN_CSN_BOARD_RESET_HANDLER_PROC                  (0x8aU)   /**< @brief #CSN_BoardResetHandler               */
#define BRCM_SWDSGN_CSN_BOARD_RESET_REQ_PROC                      (0x8bU)   /**< @brief #CSN_BoardResetReq                   */
#define BRCM_SWDSGN_CSN_BOARD_ENABLE_SAFE_STATE_CB_PROC           (0x8cU)   /**< @brief #CSN_BoardEnableSafeStateCb          */
#define BRCM_SWDSGN_CSN_BOARD_DISABLE_SAFE_STATE_CB_PROC          (0x8dU)   /**< @brief #CSN_BoardDisableSafeStateCb         */
/** @} */

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_REQ_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
typedef uint32_t CSN_BoardResetStateType;
#define CSN_BOARD_RESET_STATE_NONE        (0U)
#define CSN_BOARD_RESET_STATE_PULSE_REQ   (1U)
#define CSN_BOARD_RESET_STATE_PULSE_DONE  (2U)
#define CSN_BOARD_RESET_STATE_RELEASE_REQ (3U)
#define CSN_BOARD_RESET_STATE_COMPLETED   (4U)
#define CSN_BOARD_RESET_STATE_ERROR       (5U)

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_REQ_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
typedef struct _CSN_BoardContextType {
#ifdef ENABLE_CAM_MODULE_OX3A10
    uint32_t jobID;
#endif /* ENABLE_CAM_MODULE_OX3A10 */
    CSN_BoardResetStateType resetState;
} CSN_BoardContextType;

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_REQ_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static CSN_BoardContextType CSN_BoardContext;

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static int32_t CSN_BoardSendResetPulse(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = PCA9673_OutputChannel(PCA9673_P1_CHANNEL_2,
                                PCA9673_LOW,
                                CAM_EVENT_CSN_RESET,
                                &CSN_BoardContext.jobID);
    if (BCM_ERR_OK == ret) {
        ret = BCM_ERR_BUSY;
    }
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static int32_t CSN_BoardGetResetPulseStatus(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = PCA9673_GetStatus(CSN_BoardContext.jobID);
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static int32_t CSN_BoardSendResetReleasePulse(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = PCA9673_OutputChannel(PCA9673_P1_CHANNEL_2,
                                PCA9673_HIGH,
                                CAM_EVENT_CSN_RESET,
                                &CSN_BoardContext.jobID);
    if (BCM_ERR_OK == ret) {
        ret = BCM_ERR_BUSY;
    }
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static int32_t CSN_BoardGetResetReleasePulseStatus(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = PCA9673_GetStatus(CSN_BoardContext.jobID);
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static void CSN_BoardResetDelay(void)
{
#ifdef ENABLE_CAM_MODULE_OX3A10
    BCM_CpuNDelay(50UL*1000UL); //50us delay
#endif /* ENABLE_CAM_MODULE_OX3A10 */
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static void CSN_BoardPreResetDelay(void)
{
#ifdef ENABLE_CAM_MODULE_OX3A10
    BCM_CpuNDelay(50UL*1000UL); //50us delay
#endif /* ENABLE_CAM_MODULE_OX3A10 */
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
static void CSN_BoardPostResetDelay(void)
{
#ifdef ENABLE_CAM_MODULE_OX3A10
    BCM_CpuNDelay(50UL*1000UL*1000UL); //50ms delay
#endif /* ENABLE_CAM_MODULE_OX3A10 */
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_HANDLER_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
extern int32_t CSN_BoardResetHandler(void)
{
    int32_t ret = BCM_ERR_OK;

    while ((BCM_ERR_OK == ret) &&
           ((CSN_BOARD_RESET_STATE_ERROR != CSN_BoardContext.resetState) &&
            (CSN_BOARD_RESET_STATE_COMPLETED != CSN_BoardContext.resetState))) {
        switch(CSN_BoardContext.resetState) {
            case CSN_BOARD_RESET_STATE_NONE:
                CSN_BoardPreResetDelay();
                ret = CSN_BoardSendResetPulse();
                if (BCM_ERR_OK == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_PULSE_DONE;
                } else if (BCM_ERR_BUSY == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_PULSE_REQ;
                } else {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_ERROR;
                }
                break;
            case CSN_BOARD_RESET_STATE_PULSE_REQ:
                ret = CSN_BoardGetResetPulseStatus();
                if (BCM_ERR_OK == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_PULSE_DONE;
                } else {
                    if (BCM_ERR_BUSY != ret) {
                        CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_ERROR;
                    }
                }
                break;
            case CSN_BOARD_RESET_STATE_PULSE_DONE:
                CSN_BoardResetDelay();
                ret = CSN_BoardSendResetReleasePulse();
                if (BCM_ERR_OK == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_COMPLETED;
                } else if (BCM_ERR_BUSY == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_RELEASE_REQ;
                } else {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_ERROR;
                }
                break;
            case CSN_BOARD_RESET_STATE_RELEASE_REQ:
                ret = CSN_BoardGetResetReleasePulseStatus();
                if (BCM_ERR_OK == ret) {
                    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_COMPLETED;
                } else {
                    if (BCM_ERR_BUSY != ret) {
                        CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_ERROR;
                    }
                }
                break;
            default:
                /* Code is not expected to be here */
                CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_ERROR;
                break;
        }
    }

    if (CSN_BOARD_RESET_STATE_COMPLETED == CSN_BoardContext.resetState) {
        CSN_BoardPostResetDelay();
        ret = BCM_ERR_OK;
    } else if (CSN_BOARD_RESET_STATE_ERROR == CSN_BoardContext.resetState) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        ret = BCM_ERR_BUSY;
    }

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_RESET_REQ_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
extern int32_t CSN_BoardResetReq(void)
{
    int32_t ret;

#ifdef ENABLE_CAM_MODULE_OX3A10
    CSN_BoardContext.jobID = 0xFFFFFFFFUL;
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    CSN_BoardContext.resetState = CSN_BOARD_RESET_STATE_NONE;
    ret = CSN_BoardResetHandler();

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_ENABLE_SAFE_STATE_CB_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
extern int32_t CSN_BoardEnableSafeStateCb(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = GPIO_EnableInterrupt(GPIO_CHANNEL_8);
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/**
    @trace #BRCM_SWARCH_CSN_BOARD_DISABLE_SAFE_STATE_CB_PROC
    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
extern int32_t CSN_BoardDisableSafeStateCb(void)
{
    int32_t ret = BCM_ERR_OK;

#ifdef ENABLE_CAM_MODULE_OX3A10
    ret = GPIO_DisableInterrupt(GPIO_CHANNEL_8);
#endif /* ENABLE_CAM_MODULE_OX3A10 */

    return ret;
}

/** @} */
