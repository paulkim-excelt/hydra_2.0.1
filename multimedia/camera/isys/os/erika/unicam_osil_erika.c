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
    @addtogroup grp_unicam_il
    @{

    @file unicam_osil_erika.c
    @brief UNICAM Integration
    This source file contains the integration of UNICAM to system
    @version 0.1 Initial draft
*/

#include <unicam_osil.h>
#include <system.h>
#include <compiler.h>
#include <ee.h>
#include <string.h>
#include <cam_erika.h>
#include <osil/bcm_osil.h>

/**
    @name UNICAM OSIL Design IDs
    @{
    @brief Design IDs for UNICAM OSIL
*/
#define BRCM_SWDSGN_UNICAM_REPORT_ERROR_PART_PROC (0x81U) /**< @brief #UNICAM_OSIL_ReportError */
#define BRCM_SWDSGN_UNICAM_SEND_CMD_PROC          (0x82U) /**< @brief #UNICAM_SendCmd          */
#define BRCM_SWDSGN_UNICAM_IRQ_HANDLER_PROC       (0x83U) /**< @brief #UNICAM_IRQHandler       */
#define BRCM_SWDSGN_UNICAM_IRQ_ERR_HANDLER_PROC   (0x84U) /**< @brief #UNICAM_IRQErrHandler    */
/** @} */

/** @brief UNICAM OSIL Report Error method

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

    @trace #BRCM_SWARCH_UNICAM_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWARCH_UNICAM_SEND_CMD_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @code{.unparsed}
    Call #BCM_ReportError
    @endcode
*/
static void UNICAM_OSIL_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_UCM_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    Detailed design of UNICAM_SendCmd

    @trace #BRCM_SWARCH_UNICAM_SEND_CMD_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE

    @code{.c}
    Build SVC command struct #SVC_RequestType
    sysReqIO.sysReqID = SVC_UCM_ID
    sysReqIO.magicID = SVC_MAGIC_UCM_ID
    sysReqIO.cmd = aCmd
    sysReqIO.svcIO = (uint8_t*)aCommonIO;
    sysReqIO.response = BCM_ERR_OK;

    Send SVC command
    ret = SVC_Request(&sysReqIO)
    if ret is BCM_ERR_OK
        ret = sysReqIO.response

    return ret
    @endcode
*/
int32_t UNICAM_SendCmd(const uint32_t aCmd, struct _UNICAM_IOType *aIO)
{
    int32_t ret;
    SVC_RequestType sysReqIO;

    /* Build SVC command struct */
    sysReqIO.sysReqID = SVC_UCM_ID;
    sysReqIO.magicID = SVC_MAGIC_UCM_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t*)aIO;
    sysReqIO.response = BCM_ERR_OK;

    /* Send SVC command */
    ret = SVC_Request(&sysReqIO);
    if (BCM_ERR_OK == ret) {
        ret = sysReqIO.response;
    } else {
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

/** @brief UNICAM IRQ Handler

    IRQ handler for UNICAM control/data interrupts.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param  void

    @param  void

    @post None

    @trace #BRCM_SWARCH_UNICAM_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None
    unicamStatus = UNICAM_HW_ProcessInterrupt();
    if (0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_CTRL)) {
        ret = BCM_SetEvent(UNICAM_TASK, UNICAM_EVENT);
    }
    if(0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_DATA_SOF)) {
        ret = BCM_SetEvent(UNICAM_TUNER_TASK, UNICAM_META_EVENT);
    }
    if(0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_DATA_EOF)) {
        ret = BCM_SetEvent(UNICAM_TUNER_TASK, UNICAM_STATS_EVENT);
    }
    @endcode
*/
ISR2(UNICAM_IRQHandler)
{
    StatusType ret;
    uint32_t unicamStatus;

    /* Process Interrupt */
    unicamStatus = UNICAM_HW_ProcessInterrupt();

    if (0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_CTRL)) {
        ret = SetEvent(UNICAM_TASK, UNICAM_EVENT);
        if (E_OK != ret) {
            UNICAM_OSIL_ReportError(BRCM_SWARCH_UNICAM_IRQ_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
#ifndef UNICAM_DISABLE_TUNERS
    if(0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_DATA_SOF)) {
        ret = SetEvent(UNICAM_TUNER_TASK, UNICAM_META_EVENT);
        if (E_OK != ret) {
            UNICAM_OSIL_ReportError(BRCM_SWARCH_UNICAM_IRQ_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }

    if(0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_DATA_EOF)) {
        ret = SetEvent(UNICAM_TUNER_TASK, UNICAM_STATS_EVENT);
        if (E_OK != ret) {
            UNICAM_OSIL_ReportError(BRCM_SWARCH_UNICAM_IRQ_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
#endif /* UNICAM_DISABLE_TUNERS */
}

/** @brief UNICAM Error IRQ Handler

    IRQ handler for UNICAM error interrupts.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param  void

    @param  void

    @post None

    @trace #BRCM_SWARCH_UNICAM_IRQ_ERR_HANDLER_PROC
    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None
    @code{.c}
    unicamStatus = UNICAM_HW_ProcessErrorInterrupt();
    if (0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_CTRL)) {
        ret = BCM_SetEvent(UNICAM_TASK, UNICAM_EVENT);
    }
    @endcode
*/
ISR2(UNICAM_IRQErrHandler)
{
    StatusType ret;
    uint32_t unicamStatus;

    /* Process Interrupt */
    unicamStatus = UNICAM_HW_ProcessErrorInterrupt();

    /* Signal UNICAM Driver Client */
    if (0UL != (unicamStatus & UNICAM_HW_EVENT_MASK_CTRL)) {
        ret = SetEvent(UNICAM_TASK, UNICAM_EVENT);
        if (E_OK != ret) {
            UNICAM_OSIL_ReportError(BRCM_SWARCH_UNICAM_IRQ_ERR_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @} */
