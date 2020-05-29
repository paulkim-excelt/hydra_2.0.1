/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_ccudrv_il
    @{

    @file ccu_osil.c
    @brief CCU driver Integration
    This source file contains the integration of CCU driver to system.
    @version 0.22 Imported from docx
*/

#include <osil/ccu_osil.h>
#include "ee.h"

/**
    @name CCU Integration Design IDs
    @{
    @brief Integration Design IDs for CCU OSIL
*/
#define BRCM_SWDSGN_CCU_IRQHANDLER0_PROC    (0x50U) /**< @brief #CCU_IRQHandler0 */
#define BRCM_SWDSGN_CCU_IRQHANDLER1_PROC    (0x51U) /**< @brief #CCU_IRQHandler1 */
#define BRCM_SWDSGN_CCU_IRQHANDLER2_PROC    (0x52U) /**< @brief #CCU_IRQHandler2 */
#define BRCM_SWDSGN_CCU_IRQHANDLER3_PROC    (0x53U) /**< @brief #CCU_IRQHandler3 */
#define BRCM_SWDSGN_CCU_UDCIRQHANDLER_PROC  (0x54U) /**< @brief #CCU_UDCIRQHandler */
#define BRCM_SWDSGN_CCU_ETRIRQHANDLER_PROC  (0x55U) /**< @brief #CCU_ETRIRQHandler */
#define BRCM_SWDSGN_CCU_SYSCMDREQ_PROC      (0x56U) /**< @brief #CCU_SysCmdReq */
#define BRCM_SWDSGN_CCU_SVCIO_TYPE          (0x57U) /**< @brief #CCU_SVCIOType */
#define BRCM_SWDSGN_CCU_SYSCMDHANDLER_PROC  (0x58U) /**< @brief #CCU_SysCmdHandler */
/** @} */

/**
    @trace  #BRCM_SWARCH_CCU_IRQHANDLER0_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_IRQHandler0)
{
    CCU_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_CCU_IRQHANDLER1_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_IRQHandler1)
{
    CCU_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_CCU_IRQHANDLER2_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_IRQHandler2)
{
    CCU_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_CCU_IRQHANDLER3_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_IRQHandler3)
{
    CCU_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_CCU_UDCIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_UDCIRQHandler)
{
    CCU_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_CCU_ETRIRQHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
ISR2(CCU_ETRIRQHandler)
{
    CCU_DrvIRQHandler();
}


/**
    @code{.c}
    ret = BCM_ERR_UNKNOWN

    if aIO is not NULL {
        Build SVC request
        ret = SVC_Request(&sysReqIO)
        if ret is BCM_ERR_OK {
            if sysReqIO.response is retVal
                ret = aIO->retVal
            else
                ret = sysReqIO.response
        }
    }

    return ret
    @endcode

    @trace  #BRCM_SWARCH_CCU_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_INTERFACE
*/
int32_t CCU_SysCmdReq(CCU_CmdType aCmd, CCU_IOType *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_UNKNOWN;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_CCT_ID;
        sysReqIO.magicID = SVC_MAGIC_CCT_ID;
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
