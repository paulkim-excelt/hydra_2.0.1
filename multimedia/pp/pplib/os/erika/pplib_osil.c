/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Limited and/or its
 licensors, and may only be used, duplicated, modified or distributed pursuant
 to the terms and conditions of a separate, written license agreement executed
 between you and Broadcom (an "Authorized License").

 Except as set forth in an Authorized License, Broadcom grants no license
 (express or implied), right to use, or waiver of any kind with respect to the
 Software, and Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  if YOU HAVE NO AUTHORIZED
 LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
 IMMEDIATELY NOTifY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.

  Except as expressly set forth in the Authorized License,
 1. This program, including its structure, sequence and organization,
    constitutes the valuable trade secrets of Broadcom, and you shall use all
    reasonable efforts to protect the confidentiality thereof, and to use this
    information only in connection with your use of Broadcom integrated
    circuit products.

 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECifICALLY DISCLAIMS ANY AND ALL IMPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN if BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

#include <ee.h>
#include <osil/pp_osil.h>
#include <osil/bcm_osil.h>
#include <bcm_err.h>

/**
    @addtogroup grp_pplib_il
    @{

    @file pplib_osil.c
    @brief PP Library Integration Layer
    This source file contains the integration of pplib to system
    @version 1.4 Imported from MS Word document
*/

/**
    @name PP Library Integration Design IDs
    @{
    @brief Integration Design IDs for PP Library
*/
#define BRCM_SWDSGN_PP_REQUEST_PROC          (0xC0U)   /**< @brief #PP_SysCmdReq    */
#define BRCM_SWDSGN_PP_DRV_SET_EVENT_PROC    (0xC1U)   /**< @brief #PP_DrvSetEvents */
#define BRCM_SWDSGN_PP_GP_HANDLER_PROC       (0xC2U)   /**< @brief #PP_GPHandler    */
#define BRCM_SWDSGN_PP_ECC_HANDLER_PROC      (0xC3U)   /**< @brief #PP_EccHandler   */
#define BRCM_SWDSGN_PP_WD_HANDLER_PROC       (0xC4U)   /**< @brief #PP_WdHandler    */
/** @} */

/**

    @code{.c}
    SVC_RequestType sysReqIO;
    ret = BCM_ERR_UNKNOWN;
    if (NULL != aPPIO) {
        aPPIO->retVal = BCM_ERR_UNKNOWN;
        sysReqIO.sysReqID = BCM_PPR_SVC_ID;
        sysReqIO.magicID = BCM_PPR_SVC_MAGIC_ID;
        sysReqIO.cmd = aCmd;
        aPPIO->reserved[0] = GetTaskID();
        sysReqIO.sysIO.ppIO = aPPIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;
        SVC_Request(&sysReqIO);
        ret = aPPIO->retVal;
    }
    return ret;
    @endcode
    @trace #BRCM_SWARCH_PP_REQUEST_PROC
    @trace #BRCM_SWREQ_PP_KERNEL_INTERFACE
*/
int32_t PP_SysCmdReq(const PP_DrvCmdType aCmd, PP_IOType *aPPIO)
{
    SVC_RequestType sysReqIO;
    int32_t ret = BCM_ERR_UNKNOWN;
    if (NULL != aPPIO) {
        TaskType tid;
        (void)GetTaskID(&tid);
        aPPIO->retVal = BCM_ERR_UNKNOWN;
        sysReqIO.sysReqID = SVC_PPR_ID;
        sysReqIO.magicID = SVC_MAGIC_PPR_ID;
        sysReqIO.cmd = aCmd;
        aPPIO->reserved[0] = (uint32_t)tid;
        sysReqIO.svcIO = (uint8_t*)aPPIO;
        sysReqIO.response = BCM_ERR_UNKNOWN;

        ret = SVC_Request(&sysReqIO);
        if (BCM_ERR_OK != ret) {
            ret = BCM_ERR_UNKNOWN;
        } else if (BCM_ERR_OK != sysReqIO.response) {
            ret = BCM_ERR_UNKNOWN;
        } else {
            ret = aPPIO->retVal;
        }
    }
    return ret;
}

/** @brief Common function to parse through core bitmask and signal client

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aStatus     Bit mask of all cores for which client needs to
                                be signalled
    @param[in]      aMask       Event Mask to signal client

    @return void

    @post None

    @limitations None

    @code{.c}
    while (0UL != aStatus) {
        coreID = 31 - clz(aStatus);
        client = PPDrv_GetClient(coreID);
        if (1 == client.valid)
            BCM_SetEvent(client.id, aMask);
            aStatus &= ~(1UL << coreID);
    @endcode
    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER
    @trace #BRCM_SWARCH_PP_GP_HANDLER_PROC
    @trace #BRCM_SWARCH_PP_ECC_HANDLER_PROC
    @trace #BRCM_SWARCH_PP_WD_HANDLER_PROC
*/
static void PP_DrvSetEvents(uint32_t aStatus, EventMaskType aMask, PP_ClientType aClientType)
{
    uint32_t intStatus = aStatus;

    while (0UL != intStatus) {
        uint32_t coreID = 31UL - (uint32_t)__builtin_clz(intStatus);
        PP_DrvClientType client = PP_DrvGetClient(coreID, aClientType);
        if (1UL == client.valid) {
            if (E_OK != SetEvent(client.id, aMask)) {
            }
        } else {
            const uint32_t values[4UL] = {aStatus, coreID, (uint32_t)client.id, client.valid};
            BCM_ReportError(BCM_PPR_ID, 0U, BRCM_SWDSGN_PP_DRV_SET_EVENT_PROC,
                    BCM_ERR_UNKNOWN, 4UL, values);
        }
        intStatus &= ~(1UL << coreID);
    }
}

/** @brief Interrupt handler to be in NVIC

    Process interrupts and notify clients

    @behavior Sync, Non-reentrant

    @pre Interrupts to be disabled

    @param[in]      void

    @return void

    @post Interrupts can be enabled

    @limitations None

    @code{.c}
    err_mask = PPDrv_IRQHandler();
    PP_DrvSetEvents (err_mask, err_event);
    @endcode
    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER
    @trace #BRCM_SWARCH_PP_GP_HANDLER_PROC
*/

/*
NOTE:
According to MISRA C-2012 Rule 14.4, the below condition expression "0" does not have an essentially boolean type.
Therefore, it can be ignored because it's an exception.
*/
ISR2(PP_GPHandler)
{
    PP_DrvSetEvents(PP_DrvGPHandler(), PP_GPEvent, PP_CLIENT_GP);
}


/** @brief ECC Error Interrupt handler to be in NVIC

    Process ECC error interrupts and notify clients

    @behavior Sync, Non-reentrant

    @pre Interrupts to be disabled

    @param[in]      void

    @return void

    @post Interrupts can be enabled

    @limitations None

    @code{.c}
    err_mask = PP_DrvEccHandler();
    PP_DrvSetEvents (err_mask, err_event);
    @endcode
    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER
    @trace #BRCM_SWARCH_PP_ECC_HANDLER_PROC
*/

/*
NOTE:
According to MISRA C-2012 Rule 14.4, the below condition expression "0" does not have an essentially boolean type.
Therefore, it can be ignored because it's an exception.
*/
ISR2(PP_EccHandler)
{
    uint32_t status = PP_DrvEccHandler();
    PP_DrvSetEvents(status & ~PP_SERVER_NOTIFY_MASK, PP_ECCEvent, PP_CLIENT_ECC);
    PP_DrvSetEvents(status & PP_SERVER_NOTIFY_MASK, PP_GPEvent, PP_CLIENT_GP);
}


/** @brief Watchdog Error Interrupt handler to be in NVIC

    Process watchdog error interrupts and notify clients

    @behavior Sync, Non-reentrant

    @pre Interrupts to be disabled

    @param[in]      void

    @return void

    @post Interrupts can be enabled

    @limitations None

    @code{.c}
    err_mask = PP_DrvWdHandler();
    PP_DrvSetEvents (wd_mask, wd_event);
    @endcode
    @trace #BRCM_SWREQ_PP_KERNEL_HANDLER
    @trace #BRCM_SWARCH_PP_WD_HANDLER_PROC
*/

/*
NOTE:
According to MISRA C-2012 Rule 14.4, the below condition expression "0" does not have an essentially boolean type.
Therefore, it can be ignored because it's an exception.
*/
ISR2(PP_WdHandler)
{
    PP_DrvSetEvents(PP_DrvWdHandler(),PP_WDEvent, PP_CLIENT_WD);
}

/** @} */
