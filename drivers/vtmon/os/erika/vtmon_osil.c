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
    @addtogroup grp_vtmon_il
    @{

    @file vtmon_osil.c
    @brief VTMON driver Integration
    This source file contains the integration of VTMON driver to system
    @version 0.50 Imported from docx
*/

#include <stddef.h>
#include <stdint.h>
#include <vtmon_osil.h>
#include "ee.h"

/**
    @name VTMON Driver API IDs
    @{
    @brief API IDs for VTMON driver
*/
#define BRCM_SWDSGN_VTMON_IRQHANDLER_PROC       (0x51U) /**< @brief #VTMON_IRQHandler */
#define BRCM_SWDSGN_VTMON_SYSCMDREQ_PROC        (0x52U) /**< @brief #VTMON_SysCmdReq */

/** @} */

/**
    @trace  #BRCM_SWARCH_VTMON_DRV_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_VTMON_KERNEL_HANDLER
*/

ISR2(VTMON_IRQHandler)
{
    VTMON_DrvIRQHandler();
}

/**
    @trace  #BRCM_SWARCH_VTMON_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_VTMON_KERNEL_INTERFACE
*/
int32_t VTMON_SysCmdReq(VTMON_CmdType aCmd, VTMON_IOType *const aIO)
{
    SVC_RequestType sysReqIO;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (aIO != NULL) {
        sysReqIO.sysReqID = SVC_VTM_ID;
        sysReqIO.magicID = SVC_MAGIC_VTM_ID;
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
