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
    @addtogroup grp_candrv_il
    @{

    @file can_osil.c
    @brief CAN driver Integration
    This file contains the integration of CAN driver to the system
    @version 0.96 Imported from docx
*/

#include <osil/can_osil.h>
#include <bcm_err.h>
#include <comms_osil.h>
#include <stddef.h>
#include "ee.h"

/**
  @name CAN Driver design IDs
  @{
  @brief Design IDs for CAN Driver
*/
#define BRCM_SWDSGN_CAN_IRQLINE0HANDLER_PROC         (0x40U) /**< @brief #CAN_IRQLine0Handler   */
#define BRCM_SWDSGN_CAN_IRQLINE1HANDLER_PROC         (0x41U) /**< @brief #CAN_IRQLine1Handler   */
#define BRCM_SWDSGN_CAN_SYSCMDREQ_PROC               (0x42U) /**< @brief #CAN_SysCmdReq         */
/** @} */

/**
    @brief CAN IRQ LINE0 Handler

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLERINTERNAL_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/
ISR2(CAN_IRQLine0Handler)
{
    StatusType status = E_OK;

    CAN_IRQLine0HandlerInternal(0UL);

    status = SetEvent(CAN_IRQ_PROCESS_TASK, CAN_EVENT);
    if (E_OK != status) {
        const uint32_t values[4] = {(uint32_t)status, 0UL, 0UL, 0UL};
        BCM_ReportError(BCM_CAN_ID, 0U, BRCM_SWDSGN_CAN_IRQLINE0HANDLER_PROC,
                            BCM_ERR_OK, 4UL, values);
    }
}
/**
    @brief [Usage of CAN_IRQLine1_Handler]

    @trace  #BRCM_SWARCH_CAN_IRQLINE0HANDLERINTERNAL_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_HANDLER
*/

ISR2(CAN_IRQLine1Handler)
{
    StatusType status = E_OK;

    CAN_IRQLine1HandlerInternal(0UL);

    status = SetEvent(CAN_IRQ_PROCESS_TASK, CAN_EVENT);
    if (E_OK != status) {
        const uint32_t values[4] = {(uint32_t)status, 0UL, 0UL, 0UL};
        BCM_ReportError(BCM_CAN_ID, 0U, BRCM_SWDSGN_CAN_IRQLINE1HANDLER_PROC,
                            BCM_ERR_OK, 4UL, values);
    }
}

/**
    @trace  #BRCM_SWARCH_CAN_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_CAN_KERNEL_INTERFACE

    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aIO is not NULL {
        Build SVC request
        ret = SVC_Request(&sysReqIO)
        if ret is BCM_ERR_OK
            if sysReqIO.response is BCM_ERR_OK
                ret = aIO->retVal
            else
                ret = sysReqIO.response
    }
    return ret
    @endcode
*/
int32_t CAN_SysCmdReq(CAN_CmdType aCmd, CAN_IOType * const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_UNKNOWN;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_CAN_ID;
        sysReqIO.magicID = SVC_MAGIC_CAN_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t *)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK == retVal) {
            if (BCM_ERR_OK == sysReqIO.response) {
                retVal = aIO->retVal;
            } else {
                retVal = sysReqIO.response;
            }
        }
    }

    return retVal;
}

/** @} */
