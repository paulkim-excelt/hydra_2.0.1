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
    @addtogroup grp_avtdrv_il
    @{

    @file avt_osil.c
    @brief AVT driver Integration
    This source file contains the integration of AVT driver to system
    @version 0.50 Imported from docx
*/

#include <stddef.h>
#include <osil/avt_osil.h>
#include "ee_internal.h"
#include "ee.h"

/**
    @name AVT Design IDs
    @{
    @brief Design IDs for AVT interface APIs
*/
#define BRCM_SWDSGN_AVT_SYSCMDREQ_PROC              (0x40U) /**< @brief #AVT_SysCmdReq              */
#define BRCM_SWDSGN_AVT_SYSSNAPSHOTIRQHANDLER_PROC  (0x41U) /**< @brief #AVT_SysSnapShotIRQHandler  */
#define BRCM_SWDSGN_AVT_SYSPERIODICIRQHANDLER_PROC  (0x42U) /**< @brief #AVT_SysPeriodicIRQHandler  */
/** @} */

/**
    @trace  #BRCM_SWARCH_AVT_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_AVT_KERNEL_INTERFACE

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
*/
int32_t AVT_SysCmdReq(AVT_CmdType aCmd, AVT_IOType *const aIO)
{
    int32_t retVal = BCM_ERR_UNKNOWN;
    SVC_RequestType sysReqIO;

    if (NULL != aIO) {
        sysReqIO.sysReqID = SVC_AVT_ID;
        sysReqIO.magicID = SVC_MAGIC_AVT_ID;
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
    @trace  #BRCM_SWARCH_AVT_SNAPSHOT_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_AVT_KERNEL_HANDLER
*/
ISR2(AVT_SysSnapShotIRQHandler)
{
    AVT_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_AVT_PERIODIC_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_AVT_KERNEL_HANDLER
*/
ISR2(AVT_SysPeriodicIRQHandler)
{
    StatusType ret;
    AVT_DrvIRQHandler();
#if !defined(__OO_NO_ALARMS__) && defined(SystemTimer)
    ret = IncrementCounterHardware(SystemTimer);
    if (E_OK != ret) {
        /*KLOG_ERR("Error in IncrementCounterHardware (%d)\n", ret);*/
    }
#endif
}
/** @} */
