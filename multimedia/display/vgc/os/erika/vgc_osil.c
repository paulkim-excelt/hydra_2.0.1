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
    @addtogroup grp_vgc_il
    @{

    @file vgc_osil.c
    @brief VGC Component Integration
    This source file contains the integration of VGC to system

    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/


#include <board.h>
#include <vgc_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"
#include <vgc.h>

 /**
     @name Component Design IDs
     @{
     @brief Design IDs for the component
 */

#define  BRCM_SWDSGN_VGC_IL_ULOG_ID_MACRO             (0xC0U)    /**< @brief #VGC_IL_ULOG_ID 		*/
#define  BRCM_SWDSGN_VGC_IL_KLOG_ID_MACRO             (0xC1U)    /**< @brief #VGC_IL_KLOG_ID 		*/
#define  BRCM_SWDSGN_VGC_IL_OSIL_UERR_MACRO           (0xC2U)    /**< @brief #VGC_OSIL_UERR 		*/
#define  BRCM_SWDSGN_VGC_IL_OSIL_KERR_MACRO           (0xC3U)    /**< @brief #VGC_OSIL_KERR 		*/
#define  BRCM_SWDSGN_VGC_VID_CNT_MACRO                (0xC4U)    /**< @brief #VGC_GFX_CNT 			*/
#define  BRCM_SWDSGN_VGC_GFX_CNT_MACRO                (0xC5U)    /**< @brief #VGC_VID_CNT 			*/
#define  BRCM_SWDSGN_VGC_VID_HEIGHT_MACRO             (0xC6U)    /**< @brief #VGC_VID_HEIGHT 		*/
#define  BRCM_SWDSGN_VGC_VIDMEM_GLOBAL                (0xC7U)    /**< @brief #VGC_VidMem 			*/
#define  BRCM_SWDSGN_VGC_GFXMEM_GLOBAL                (0xC8U)    /**< @brief #VGC_GfxMem 			*/
#define  BRCM_SWDSGN_VGC_VIDSZ_GLOBAL                 (0xC9U)    /**< @brief #VGC_VidSz 			*/
#define  BRCM_SWDSGN_VGC_GFXSZ_GLOBAL                 (0xCAU)    /**< @brief #VGC_GfxSz 			*/
#define  BRCM_SWDSGN_VGC_VIDCOUNT_GLOBAL              (0xCBU)    /**< @brief #VGC_VidCount 			*/
#define  BRCM_SWDSGN_VGC_GFXCOUNT_GLOBAL              (0xCCU)    /**< @brief #VGC_GfxCount 			*/
#define  BRCM_SWDSGN_VGC_SVCIO_TYPE                   (0xCDU)    /**< @brief #VGC_SVCIOType 		*/
#define  BRCM_SWDSGN_VGC_OSIL_ERR_PROC                (0xCEU)    /**< @brief #VGC_OsilErr 			*/
#define  BRCM_SWDSGN_VGC_SYSCMDREQ_PROC               (0xCFU)    /**< @brief #VGC_SysCmdReq 		*/
#define  BRCM_SWDSGN_VGC_SYSCMDHANDLER_PROC           (0xD0U)    /**< @brief #VGC_SysCmdHandler 	*/
#define  BRCM_SWDSGN_VGC_PWRDOWN_IRQ_HNDLR_PROC       (0xD1U)    /**< @brief #VGC_PwrDownIRQHandler */
#define  BRCM_SWDSGN_VGC_DSI_IRQ_HNDLR_PROC           (0xD2U)    /**< @brief #VGC_DSIIRQHandler 	*/
#define  BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC            (0xD3U)    /**< @brief #VGC_PVIRQHandler 		*/
#define  BRCM_SWDSGN_VGC_BVB_IRQ_HNDLR_PROC           (0xD4U)    /**< @brief #VGC_BVBIRQHandler 	*/
#define  BRCM_SWDSGN_VGC_EOT_IRQ_HNDLR_PROC           (0xD5U)    /**< @brief #VGC_EOTIRQHandler 	*/
/** @} */


/**
    @brief todo

    @trace #BRCM_SWREQ_VGC_COMMON
*/
#ifndef VGC_VID_BPP
#error "Video bits per pixel- VGC_VID_BPP is undefined"
#endif

/**
    @brief todo

    @trace #BRCM_SWREQ_VGC_COMMON
*/
#ifndef VGC_GFX_BPP
#error "Graphics bits per pixel- VGC_GFX_BPP is undefined"
#endif

/**
    @brief todo

    @trace #BRCM_SWREQ_VGC_COMMON
*/
#ifndef VGC_WIDTH
#error "VGC_WIDTH is undefined"
#endif

/**
    @brief todo

    @trace #BRCM_SWREQ_VGC_COMMON
*/
#ifndef VGC_HEIGHT
#error "VGC_HEIGHT is undefined"
#endif

#ifdef ENABLE_VGC_DISPLAY
/**
    @brief Number of Graphics buffers

    @trace #BRCM_SWARCH_VGC_GFXMEM_GLOBAL
    @trace #BRCM_SWARCH_VGC_GFXSZ_GLOBAL
    @trace #BRCM_SWARCH_VGC_GFXCOUNT_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
#define VGC_GFX_CNT         (3UL)

/**
    @brief Number of Video buffers

    @trace #BRCM_SWARCH_VGC_VIDMEM_GLOBAL
    @trace #BRCM_SWARCH_VGC_VIDSZ_GLOBAL
    @trace #BRCM_SWARCH_VGC_VIDCOUNT_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
#define VGC_VID_CNT         (3UL)

/**
    @brief video Height

    @trace #BRCM_SWARCH_VGC_VIDMEM_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
#define VGC_VID_HEIGHT      (VGC_HEIGHT)

/* Multiple video/graphics buffers could be allocated, but all of them
 * contiguous, equal in size and allocated as a single large video/graphics
 * buffer
 */
/**
    @brief Video Memory Start Address

    @trace #BRCM_SWARCH_VGC_VIDMEM_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
uint8_t VGC_VidMem[(VGC_VID_CNT * VGC_WIDTH * VGC_VID_HEIGHT * VGC_VID_BPP) / 8UL] COMP_SECTION(".vgc.VGC_VidMem") COMP_ALIGN(256);

/**
    @brief Graphics Memory Start Address

    @trace #BRCM_SWARCH_VGC_GFXMEM_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
uint8_t VGC_GfxMem[(VGC_GFX_CNT * VGC_WIDTH * VGC_HEIGHT * VGC_GFX_BPP) / 8UL] COMP_SECTION(".vgc.cached.VGC_GfxMem") COMP_ALIGN(256);

/**
    @brief Video Memory size per frame

    @trace #BRCM_SWARCH_VGC_VIDSZ_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
const uint32_t VGC_VidSz = sizeof(VGC_VidMem) / VGC_VID_CNT;

/**
    @brief Graphics Memory size per frame

    @trace #BRCM_SWARCH_VGC_GFXSZ_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
const uint32_t VGC_GfxSz = sizeof(VGC_GfxMem) / VGC_GFX_CNT;

/**
    @brief Video buffer Count

    @trace #BRCM_SWARCH_VGC_VIDCOUNT_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
const uint32_t VGC_VidCount = VGC_VID_CNT;

/**
    @brief Graphics buffer count

    @trace #BRCM_SWARCH_VGC_GFXCOUNT_GLOBAL
    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
const uint32_t VGC_GfxCount = VGC_GFX_CNT;
#endif

/** @brief VGC OSIL Error Log

    This API is for logging errors

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]      aCompID       Component ID
    @param[in]      aApiID        API ID
    @param[in]      aErr          Error Code
    @param[in]      aVal0         Value0
    @param[in]      aVal1         Value1
    @param[in]      aVal2         Value2
    @param[in]      aVal3         Value3

    Return values are documented in reverse-chronological order
    @return void

    @post none

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

    @limitations none

    @code{.unparsed}
    values = {aVal0, aVal1, aVal2, aVal3}
    BCM_ReportError(aCompID, 0U, aApiID, aErr, 4UL, values)
    @endcode
*/
static void VGC_OsilErr(uint16_t aCompID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(aCompID, 0U, aApiID, aErr, 4UL, values);
}

/**
    @brief VGC User Log Identifier

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_IL_ULOG_ID             (BCM_VGC_ID & BCM_LOGMASK_USER)

/**
    @brief VGC OSIL User Log Error

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_OSIL_UERR(api, err, val0, val1, val2, val3)   \
    VGC_OsilErr(VGC_IL_ULOG_ID, api, err, val0, val1, val2, val3)

/**
    @brief VGC Kernel Log Identifier

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER
*/
#define VGC_IL_KLOG_ID             (BCM_VGC_ID | BCM_LOGMASK_KERNEL)

/**
    @brief VGC OSIL Kernel Log Error

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER
*/
#define VGC_OSIL_KERR(api, err, val0, val1, val2, val3)   \
    VGC_OsilErr(VGC_IL_KLOG_ID, api, err, val0, val1, val2, val3)

/**
    @code{.c}

    Build SVC Request
    ret = SVC_Request(&sysReqIO)
    if (ret is BCM_ERR_OK)
        ret = sysReqIO.response
    else
        Log error

    @trace #BRCM_SWARCH_VGC_SYSCMDREQ_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

    return ret
    @endcode
*/

int32_t VGC_SysCmdReq(const uint32_t aCmd, VGC_IOType *aIO)
{
    int32_t ret;
    SVC_RequestType sysReqIO;

    sysReqIO.response = BCM_ERR_UNKNOWN;

    /* Build SVC command struct */
    sysReqIO.sysReqID = SVC_VGC_ID;
    sysReqIO.magicID = SVC_MAGIC_VGC_ID;
    sysReqIO.cmd = aCmd;
    sysReqIO.svcIO = (uint8_t*)aIO;
    /* Send SVC command */
    ret = SVC_Request(&sysReqIO);
    if (BCM_ERR_OK == ret) {
        ret = sysReqIO.response;
    } else {
        VGC_OSIL_UERR(BRCM_SWARCH_VGC_SYSCMDREQ_PROC, ret, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
    }
    return ret;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER
*/
typedef union _VGC_SVCIOType {
    uint8_t *data;
    VGC_IOType *io;
} VGC_SVCIOType;

/**
    @brief todo

    @trace #BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}
    if aSysIO.vgcIO is not NULL
        if aMagicID is SVC_MAGIC_VGC_ID
            ret = VGCDrv_CmdHandler(aCmd, aSysIO.vgcIO)
            if ret is not BCM_ERR_OK
                Log error
        else
            Log error
    else
        Log error

    @endcode
*/
void VGC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    int32_t ret;
    VGC_SVCIOType vgc;
    vgc.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_VGC_ID == aMagicID){
#ifdef ENABLE_VGC_DISPLAY
            ret = VGC_DispDrvCmdHandler(aCmd, vgc.io);
#else
            ret = VGC_MwbDrvCmdHandler(aCmd, vgc.io);
#endif
            if (BCM_ERR_OK != ret) {
                VGC_OSIL_KERR(BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC, ret, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
            }
        } else {
            VGC_OSIL_KERR(BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
        }
    } else {
        VGC_OSIL_KERR(BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC, BCM_ERR_INVAL_PARAMS, (uint32_t)__LINE__, aCmd, 0UL, 0UL);
    }
}

#ifdef ENABLE_VGC_DISPLAY
/**
    @brief todo

    @trace #BRCM_SWARCH_VGC_DISP_DRV_PWRDOWN_INT_HANDLR_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}

    @endcode
*/
ISR2(VGC_PwrDownIRQHandler)
{
    VGC_DispDrvPwrDownInterruptHandler();
}

/**
    @brief todo

    @trace #BRCM_SWARCH_VGC_DISP_DRV_DSI_INT_HANDLR_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}
    VGC_DispDrvDSIInterruptHandler();


    @endcode
*/
ISR2(VGC_DSIIRQHandler)
{
    VGC_DispDrvDSIInterruptHandler();
}

/**
    @brief todo

    @trace #BRCM_SWARCH_VGC_DISP_DRV_PV_INT_HANDLR_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}
    ret = VGC_DispDrvPVInterruptHandler(&isStopped, &lineNumber);
    if isStopped is TRUE
        if BCM_SetEvent(VGC_ClientTask, VGC_STOP_EVENT) is not E_OK
            Log Error
    else if lineNumber is VGC_HEIGHT
        if BCM_SetEvent(VGC_ClientTask, VGC_VSYNC_EVENT) is not E_OK
            Log Error
    else if lineNumber > 0UL
        if BCM_SetEvent(VGC_ClientTask, VGC_LINE_EVENT) is not E_OK
            Log Error
    else
        Log Error

    if ret is not BCM_ERR_OK
        Log Error
    @endcode
*/
ISR2(VGC_PVIRQHandler)
{
    uint32_t isStopped = FALSE;
    int32_t ret;
    uint32_t lineNumber = 0UL;

    ret = VGC_DispDrvPVInterruptHandler(&isStopped, &lineNumber);
    if (TRUE == isStopped) {
        /* Trigger VGC service task from here */
        if (E_OK != SetEvent(VGC_ClientTask, VGC_STOP_EVENT)) {
            VGC_OSIL_KERR(BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC, ret, (uint32_t)__LINE__,
                    (uint32_t)VGC_ClientTask, (uint32_t)VGC_STOP_EVENT, 0UL);
        }
    } else if (VGC_HEIGHT == lineNumber) {
        if (E_OK != SetEvent(VGC_ClientTask, VGC_VSYNC_EVENT)) {
            VGC_OSIL_KERR(BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC, ret, (uint32_t)__LINE__,
                    (uint32_t)VGC_ClientTask, (uint32_t)VGC_VSYNC_EVENT, isStopped);
        }
    } else if (lineNumber > 0UL) {
        /* Trigger VGC service task from here */
        if (E_OK != SetEvent(VGC_ClientTask, VGC_LINE_EVENT)) {
            VGC_OSIL_KERR(BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC, ret, (uint32_t)__LINE__,
                    (uint32_t)VGC_ClientTask, (uint32_t)VGC_LINE_EVENT, 0UL);
        }
    } else {
        VGC_OSIL_KERR(BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC, ret, (uint32_t)__LINE__,
        (uint32_t)VGC_ClientTask, (uint32_t)isStopped, (uint32_t)lineNumber);
    }

    if (BCM_ERR_OK != ret) {
        VGC_OSIL_KERR(BRCM_SWDSGN_VGC_PV_IRQ_HNDLR_PROC, ret, (uint32_t)__LINE__, isStopped, 0UL, 0UL);
    }
}
#endif

/**
    @brief BVB interrupt handled through INTR2 controller

    @trace #BRCM_SWARCH_VGC_MWB_DRV_BVB_INT_HANDLR_PROC
    @trace #BRCM_SWARCH_VGC_DISP_DRV_BVB_INT_HANDLR_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}
    VGC_MwbDrvMwbInterruptHandler();

    @endcode
*/
ISR2(VGC_BVBIRQHandler)
{
#ifndef ENABLE_VGC_DISPLAY
    VGC_MwbDrvBvbInterruptHandler();
#else
    VGC_DispDrvBvbInterruptHandler();
#endif
}

#ifndef ENABLE_VGC_DISPLAY
/**
    @brief Interrupt for writeback mode

    @trace #BRCM_SWARCH_VGC_MWB_DRV_EOT_INT_HANDLR_PROC
    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @code{.c}
    VGC_MwbDrvEOTInterruptHandler();
    if BCM_SetEvent(VGC_ClientTask, VGC_STOP_EVENT) is not E_OK
        Log error
    @endcode
*/
ISR2(VGC_EOTIRQHandler)
{
    VGC_MwbDrvEOTInterruptHandler();
    /* Trigger VGC service task from here */
    if (BCM_ERR_OK != SetEvent(VGC_ClientTask, VGC_STOP_EVENT)) {
        VGC_OSIL_KERR(BRCM_SWDSGN_VGC_EOT_IRQ_HNDLR_PROC, BCM_ERR_UNKNOWN, (uint32_t)__LINE__,
                (uint32_t)VGC_ClientTask, (uint32_t)VGC_STOP_EVENT, 0UL);
    }
}
#endif
/** @} */
