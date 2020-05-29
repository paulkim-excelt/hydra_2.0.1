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
    @addtogroup grp_i2cdrv_il
    @{

    @file i2c_osil.c
    @brief IIC driver Integration
    This source file contains the integration of IIC driver to system
    @version 0.62 Imported from docx
*/

#include <msg_queue.h>
#include <bcm_err.h>
#include <chip_config.h>
#include <i2c.h>
#include <bcm_utils.h>
#include <osil/i2c_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"

/**
    @name IIC driver Integration Layer Design IDs
    @{
    @brief Integration Layer Design IDs for IIC driver
*/
#define BRCM_SWDSGN_IIC_SYSCMDREQ_PROC         (0x80U) /**< @brief #IIC_SysCmdReq   */
#define BRCM_SWDSGN_IIC_IRQHANDLER0_PROC       (0x81U) /**< @brief #IIC_IRQHandler0 */
#define BRCM_SWDSGN_IIC_IRQHANDLER1_PROC       (0x82U) /**< @brief #IIC_IRQHandler1 */
#define BRCM_SWDSGN_IIC_IRQHANDLER2_PROC       (0x83U) /**< @brief #IIC_IRQHandler2 */
#define BRCM_SWDSGN_IIC_IRQHANDLER3_PROC       (0x84U) /**< @brief #IIC_IRQHandler3 */

/** @} */

/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN
    if aIO is not NULL {
        Build SVC request
        ret = SVC_Request(&sysReqIO)
        if ret is not BCM_ERR_OK
            ret = BCM_ERR_UNKNOWN
        else if sysReqIO.response is not BCM_ERR_OK
            ret = sysReqIO.response
        else
            ret = aIO->retVal
    }

    return ret
    @endcode

    @trace  #BRCM_SWARCH_IIC_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_INTERFACE
*/
int32_t IIC_SysCmdReq(IIC_CmdType aCmd, IIC_IOType *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        TaskType tid;
        (void)GetTaskID(&tid);
        aIO->clientID = tid;
        sysReqIO.sysReqID = SVC_IIC_ID;
        sysReqIO.magicID = SVC_MAGIC_IIC_ID;
        sysReqIO.cmd = aCmd;
        sysReqIO.svcIO = (uint8_t*)aIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        retVal = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
        } else if (BCM_ERR_OK != sysReqIO.response) {
            retVal = sysReqIO.response;
        } else {
            retVal = aIO->retVal;
        }
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_IIC_IRQHANDLER0_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
ISR2(IIC_IRQHandler0)
{
    StatusType status = E_OK;
    IIC_ClientSignalType clientSignal;

    clientSignal = IIC_IRQHandler(0UL);
    if (FALSE != clientSignal.done) {
        status = SetEvent(clientSignal.clientID, clientSignal.clientMask);
        if (E_OK != status) {
            const uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
            BCM_ReportError(BCM_IIC_ID, (uint8_t)0U,
                            BRCM_SWDSGN_IIC_IRQHANDLER0_PROC,
                            (int32_t)status, 4UL, values);
        }
    }
}

/**
    @trace  #BRCM_SWARCH_IIC_IRQHANDLER1_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
ISR2(IIC_IRQHandler1)
{
    StatusType status = E_OK;
    IIC_ClientSignalType clientSignal;

    clientSignal = IIC_IRQHandler(1UL);
    if (FALSE != clientSignal.done) {
        status = SetEvent(clientSignal.clientID, clientSignal.clientMask);
        if (E_OK != status) {
            const uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
            BCM_ReportError(BCM_IIC_ID, (uint8_t)0U,
                            BRCM_SWDSGN_IIC_IRQHANDLER1_PROC,
                            (int32_t)status, 4UL, values);
        }
    }
}

/**
    @trace  #BRCM_SWARCH_IIC_IRQHANDLER2_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
ISR2(IIC_IRQHandler2)
{
    StatusType status = E_OK;
    IIC_ClientSignalType clientSignal;

    clientSignal = IIC_IRQHandler(2UL);
    if (FALSE != clientSignal.done) {
        status = SetEvent(clientSignal.clientID, clientSignal.clientMask);
        if (E_OK != status) {
            const uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
            BCM_ReportError(BCM_IIC_ID, (uint8_t)0U,
                            BRCM_SWDSGN_IIC_IRQHANDLER2_PROC,
                            (int32_t)status, 4UL, values);
        }
    }
}

/**
    @trace  #BRCM_SWARCH_IIC_IRQHANDLER3_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
ISR2(IIC_IRQHandler3)
{
    StatusType status = E_OK;
    IIC_ClientSignalType clientSignal;

    clientSignal = IIC_IRQHandler(3UL);
    if (FALSE != clientSignal.done) {
        status = SetEvent(clientSignal.clientID, clientSignal.clientMask);
        if (E_OK != status) {
            const uint32_t values[4] = {0UL, 0UL, 0UL, 0UL};
            BCM_ReportError(BCM_IIC_ID, (uint8_t)0U,
                            BRCM_SWDSGN_IIC_IRQHANDLER3_PROC,
                            (int32_t)status, 4UL, values);
        }
    }
}

/** @} */
