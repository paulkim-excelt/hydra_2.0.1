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
    @addtogroup grp_isp_il
    @{

    @file isp_osil_erika.c
    @brief ISP Integration
    This source file contains the integration of ISP to system
    @version 0.1 Initial draft
*/

#include <compiler.h>
#include <system.h>
#include <ee.h>
#include <string.h>
#include <osil/bcm_osil.h>
#include <cam_erika.h>
#include <isp_osil.h>

/**
    @name ISP OSIL Design IDs
    @{
    @brief Design IDs for ISP OSIL
*/
#define BRCM_SWDSGN_ISP_REPORT_ERROR_PART_PROC  (0x81U) /**< @brief #ISP_OSIL_ReportError */
#define BRCM_SWDSGN_ISP_SYS_CMD_REQ_PROC        (0x82U) /**< @brief #ISP_SysCmdReq        */
#define BRCM_SWDSGN_ISP_COMMON_IRQ_HANDLER_PROC (0x83U) /**< @brief #ISP_CommonIRQHandler */
#define BRCM_SWDSGN_ISP_IRQ_HANDLER_PROC        (0x84U) /**< @brief #ISP_IRQHandler       */
#define BRCM_SWDSGN_ISP_IRQ_ERR_HANDLER_PROC    (0x85U) /**< @brief #ISP_NLinesIRQHandler */
/** @} */

/**
    Detailed design of ISP_OSILl_ReportError

    @trace #BRCM_SWARCH_ISP_SYS_CMD_HANDLER_PROC
    @trace #BRCM_SWARCH_ISP_SYS_CMD_REQ_PROC
    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER
*/
static void ISP_OSIL_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_ISP_ID, 0U, aApiID, aErr, 4UL, values);
}

/**
    Detailed design of ISP_SysCmdReq

    @trace #BRCM_SWARCH_ISP_SYS_CMD_REQ_PROC
    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
int32_t ISP_SysCmdReq(const uint32_t aCmd, struct _ISP_IOType *aIO)
{
    int32_t ret;
    SVC_RequestType sysReqIO;

    /* Build SVC command struct */
    sysReqIO.sysReqID = SVC_ISP_ID;
    sysReqIO.magicID = SVC_MAGIC_ISP_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t*) aIO;
    sysReqIO.response = BCM_ERR_OK;

    /* Send SVC command */
    ret = SVC_Request(&sysReqIO);
    if (BCM_ERR_OK == ret) {
        ret = sysReqIO.response;
    }

    return ret;
}

/**
    Detailed design of ISP_CommonIRQHandler

    @trace #BRCM_SWARCH_ISP_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER
*/
static void ISP_CommonIRQHandler(void)
{
    StatusType ret;
    uint32_t ispStatus;

    /* Process Interrupt */
    ispStatus = ISP_DrvProcessInterrupt();
    if (0UL != (ispStatus & ISP_HW_MASK_CTRL)) {
        ret = SetEvent(ISP_TASK, ISP_EVENT);
        if (E_OK != ret) {
            ISP_OSIL_ReportError(BRCM_SWDSGN_ISP_COMMON_IRQ_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }

#ifndef ISP_DISABLE_TUNERS
    if (0UL != (ispStatus & ISP_HW_MASK_STATS)) {
        ret = SetEvent(ISP_TUNER_TASK, ISP_STATS_EVENT);
        if (E_OK != ret) {
            ISP_OSIL_ReportError(BRCM_SWDSGN_ISP_COMMON_IRQ_HANDLER_PROC,
                    (int32_t) ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }
#endif /* ISP_DISABLE_TUNERS */
}

/**
    Detailed design of ISP_IRQHandler

    @trace #BRCM_SWARCH_ISP_IRQ_HANDLER_PROC
    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER
*/
ISR2(ISP_IRQHandler)
{
    ISP_CommonIRQHandler();
}

/**
    Detailed design of ISP_NLinesIRQHandler

    @trace #BRCM_SWARCH_ISP_IRQ_ERR_HANDLER_PROC
    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER
*/
ISR2(ISP_NLinesIRQHandler)
{
    ISP_CommonIRQHandler();
}

/** @} */