/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_vgc_il Integration
    @ingroup grp_vgc

    @addtogroup grp_vgc_il
    @{

    @section sec_vgc_il_seq Sequence Diagrams
    @subsection subsec_vgc_il_sync Synchronoous Call
    @image html vgc_seq_sync.jpg
    Typical call flow for a synchronous call such as VGC_Init invoked by the Client. The same sequence will hold good for the rest of the synchronous APIs exposed by VGC.

    @subsection subsec_vgc_il_async    Asynchronoous Call
    @image html vgc_seq_async.jpg
    Typical call flow for an asynchronous call such as VGC_Stop invoked by the Client.

    @file vgc_osil.h
    @brief VGC integration interface
    This file defines the interfaces exposed by VGC driver to Integrate to an OS
    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

#ifndef VGC_OSIL_H
#define VGC_OSIL_H

#include <system.h>
#include <bcm_utils.h>
#include <vgc.h>

/**
    @name VGC Driver Integration Interface IDs
    @{
    @brief Integration Interface IDs for VGC Driver
*/
#define BRCM_SWARCH_VGC_LINES_PER_MB_MACRO                (0x40U) /**< @brief #VGC_IL_LINES_PER_MB                */
#define BRCM_SWARCH_VGC_CMD_TYPE                          (0x41U) /**< @brief #VGC_CmdType                        */
#define BRCM_SWARCH_VGC_STOP_EVENT_MACRO                  (0x42U) /**< @brief #VGC_STOP_EVENT                     */
#define BRCM_SWARCH_VGC_LINE_EVENT_MACRO                  (0x43U) /**< @brief #VGC_LINE_EVENT                     */
#define BRCM_SWARCH_VGC_VGCIO_TYPE                        (0x44U) /**< @brief #VGC_IOType                         */
#define BRCM_SWARCH_VGC_VIDMEM_GLOBAL                     (0x45U) /**< @brief #VGC_VidMem                         */
#define BRCM_SWARCH_VGC_GFXMEM_GLOBAL                     (0x46U) /**< @brief #VGC_GfxMem                         */
#define BRCM_SWARCH_VGC_VIDSZ_GLOBAL                      (0x47U) /**< @brief #VGC_VidSz                          */
#define BRCM_SWARCH_VGC_GFXSZ_GLOBAL                      (0x48U) /**< @brief #VGC_GfxSz                          */
#define BRCM_SWARCH_VGC_VIDCOUNT_GLOBAL                   (0x49U) /**< @brief #VGC_VidCount                       */
#define BRCM_SWARCH_VGC_GFXCOUNT_GLOBAL                   (0x4AU) /**< @brief #VGC_GfxCount                       */
#define BRCM_SWARCH_VGC_SYSCMDREQ_PROC                    (0x4BU) /**< @brief #VGC_SysCmdReq                      */
#define BRCM_SWARCH_VGC_SYSCMDHANDLER_PROC                (0x4CU) /**< @brief #VGC_SysCmdHandler                  */
#define BRCM_SWARCH_VGC_DISP_DRV_CMDHDLR_PROC             (0x4DU) /**< @brief #VGC_DispDrvCmdHandler              */
#define BRCM_SWARCH_VGC_MWB_DRV_CMD_HANDLR_PROC           (0x4EU) /**< @brief #VGC_MwbDrvCmdHandler               */
#define BRCM_SWARCH_VGC_DISP_DRV_PV_INT_HANDLR_PROC       (0x4FU) /**< @brief #VGC_DispDrvPVInterruptHandler      */
#define BRCM_SWARCH_VGC_DISP_DRV_BVB_INT_HANDLR_PROC      (0x50U) /**< @brief #VGC_DispDrvBvbInterruptHandler     */
#define BRCM_SWARCH_VGC_MWB_DRV_BVB_INT_HANDLR_PROC       (0x51U) /**< @brief #VGC_MwbDrvBvbInterruptHandler      */
#define BRCM_SWARCH_VGC_DISP_DRV_PWRDOWN_INT_HANDLR_PROC  (0x52U) /**< @brief #VGC_DispDrvPwrDownInterruptHandler */
#define BRCM_SWARCH_VGC_MWB_DRV_EOT_INT_HANDLR_PROC       (0x53U) /**< @brief #VGC_MwbDrvEOTInterruptHandler      */
#define BRCM_SWARCH_VGC_DISP_DRV_DSI_INT_HANDLR_PROC      (0x54U) /**< @brief #VGC_DispDrvDSIInterruptHandler     */
/** @} */

/**
    @brief Number of lines per MacroBlock
    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_IL_LINES_PER_MB    (16UL)

/**
    @brief VGC SVC Command

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef uint32_t VGC_CmdType;                         /**< @brief VGC SVC Command */
#define VGC_CMD_INIT                  (0x1UL)      /**< @brief VGC_Init */
#define VGC_CMD_DEINIT                (0x2UL)      /**< @brief VGC_Deinit */
#define VGC_CMD_START                 (0x3UL)      /**< @brief VGC_Start */
#define VGC_CMD_STOP                  (0x4UL)      /**< @brief VGC_Stop */
#define VGC_CMD_SETBG                 (0x5UL)      /**< @brief VGC_SetBackgroundColor */
#define VGC_CMD_DISSURF               (0x6UL)      /**< @brief VGC_DisableSurface */
#define VGC_CMD_ENSURF                (0x7UL)      /**< @brief VGC_EnableSurface */
#define VGC_CMD_QUEUE                 (0x8UL)      /**< @brief VGC_QueueBuffer */
#define VGC_CMD_DEQUEUE               (0x9UL)      /**< @brief VGC_DequeueBuffer */
#define VGC_CMD_RELEASE               (0xAUL)      /**< @brief VGC_ReleaseBuffer */
#define VGC_CMD_GETSTATS              (0xBUL)      /**< @brief VGC_GetStats */
#define VGC_CMD_CSC                   (0xCUL)      /**< @brief VGC_CSC */
#define VGC_CMD_COLORKEY              (0xDUL)      /**< @brief VGC_ColorKey */
#define VGC_CMD_LOADPALETTE           (0xEUL)      /**< @brief VGC_LoadPalette */
#define VGC_CMD_GETSTATUS             (0xFUL)      /**< @brief VGC_GetStatus */
#define VGC_CMD_MWB                   (0x10UL)     /**< @brief VGC_MWB */

/**
    @brief Reserved events for signalling Stop to Client
    Event to trigger Stop completion

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_STOP_EVENT                (SystemEvent2)

/**
    @brief Reserved events for signalling Line to Client
    Event to trigger completion of a macro block - 16lines

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
#define VGC_LINE_EVENT                (SystemEvent3)


/**
    @brief  Structure to define input and output parameters

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE
*/
typedef struct _VGC_IOType {
    int32_t status;                            /**< @brief status of system call */
    const uint32_t size;                       /**< @brief size of buffer */
    uint32_t enable;                           /**< @brief enable/disable */
#ifndef ENABLE_VGC_DISPLAY
    const VGC_MwbCfgType *const cfg;           /**< @brief configuration structure pointer */
    VGC_MwbStatsType *const stats;             /**< @brief statistics information */
#else
    const VGC_DispCfgType *const cfg;          /**< @brief configuration structure pointer */
    VGC_DispStatsType *const stats;            /**< @brief statistics information */
    const VGC_SurfaceType surfType;            /**< @brief surface type */
#endif
    const VGC_SurfAttrType *const vidSurfAttr; /**< @brief input Video surface attributes */
    const VGC_BufferType *const vidInBuff;     /**< @brief input Video buffer parameters */
    const VGC_SurfAttrType *const gfxSurfAttr; /**< @brief input Graphics surface attributes */
    const VGC_BufferType *const gfxInBuff;     /**< @brief input Graphics buffer parameters */
    const uint8_t *const outBuff;              /**< @brief output buffer address */
    const VGC_BufferType *const inBuff;        /**< @brief input buffer parameters */
    VGC_BufferType *const inoutBuff;           /**< @brief output buffer parameters */
    const VGC_PixelType *const pixel;          /**< @brief input pixel information */
    const VGC_CSCType *const csc;              /**< @brief CSC matrix co-efficients */
    const VGC_ColorKeyType *const colorKey;    /**< @brief Color Key information */
    const uint32_t *const palette;             /**< @brief Pointer to Palette */
    uint32_t *const lineNumber;                /**< @brief Pointer to LineNumber */
    uint32_t *const state;                     /**< @brief Pointer to state */
} VGC_IOType;

#ifdef ENABLE_VGC_DISPLAY
/**
    @brief Video Memory Start Address

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern uint8_t VGC_VidMem[];

/**
    @brief Graphics Memory Start Address

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern uint8_t VGC_GfxMem[];

/**
    @brief Video Memory size per frame

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern const uint32_t VGC_VidSz;

/**
    @brief Graphics Memory size per frame

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern const uint32_t VGC_GfxSz;

/**
    @brief Video buffer Count

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern const uint32_t VGC_VidCount;

/**
    @brief Graphics buffer count

    @trace #BRCM_SWREQ_VGC_DISPLAY
*/
extern const uint32_t VGC_GfxCount;
#endif

/** @brief VGC system command request

    This API will trigger a system command request for a given command

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       aCmd            command to be executed in system context
    @param[inout]    aIO             VGC IO structure to exchange information

    Return values are documented in reverse-chronological order
    @retval          BCM_ERR_OK          success
    @retval          others              failure

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_INTERFACE

    @limitations none
*/
extern int32_t VGC_SysCmdReq(const uint32_t aCmd, VGC_IOType *aIO);

/** @brief VGC system command request handler

    This API will handle the system command request and call internal handler
    after validating the MAGIC ID

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       aMagicID        Magic ID of VGC
    @param[in]       acmd            command to be executed in system context
    @param[inout]    aSysIO          System IO structure to exchange information

    Return values are documented in reverse-chronological order
    @return          none

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

/** @brief VGC system command request internal handler

    This API will process the system command request for a given command

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       acmd            command to be executed in system context
    @param[inout]    aVGCIO          VGC IO structure to exchange information

    Return values are documented in reverse-chronological order
    @retval          BCM_ERR_OK          success
    @retval          others              failure

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern int32_t VGC_DispDrvCmdHandler(uint32_t aCmd, VGC_IOType *aVGCIO);

/** @brief VGC MWB Driver system command request internal handler

    This API will process the system command request for a given command

    @behavior Sync, Non-reentrant

    @pre none

    @param[in]       acmd            command to be executed in system context
    @param[inout]    aVGCIO          VGC IO structure to exchange information

    Return values are documented in reverse-chronological order
    @retval          BCM_ERR_OK          success
    @retval          others              failure

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern int32_t VGC_MwbDrvCmdHandler(uint32_t aCmd, VGC_IOType *aVGCIO);

/** @brief BVB Interrupt Handler

    This API will handle BVB interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param           none

    <br>Return values are documented in reverse-chronological order
    @return          void

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_MwbDrvBvbInterruptHandler(void);

/** @brief EOT Interrupt Handler

    This API will handle EOT interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param           none

    <br>Return values are documented in reverse-chronological order
    @return          void

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_MwbDrvEOTInterruptHandler(void);

/** @brief PixelValve Interrupt Handler

    This API will handle PixelValve interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param[out]      aIsStopped      TRUE : VGC Stopped
                                    FALSE: VGC is active
    @param[out]      aLineNumber     0    : No line interrupt
                                    !0   : Particular line interrupt

    Return values are documented in reverse-chronological order
    @retval          BCM_ERR_OK          Success
    @retval          BCM_ERR_UNKNOWN     Unknown error

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern int32_t VGC_DispDrvPVInterruptHandler(uint32_t *aIsStopped,
                                              uint32_t *aLineNumber);
/** @brief BVB Interrupt Handler

    This API will handle BVB interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param           none

    <br>Return values are documented in reverse-chronological order
    @return          void

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_DispDrvBvbInterruptHandler(void);

/** @brief PowerDown Interrupt Handler

    This API will handle PowerDown interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param           none

    <br>Return values are documented in reverse-chronological order
    @return          void

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_DispDrvPwrDownInterruptHandler(void);

/** @brief DSI Interrupt Handler

    This API will handle DSI  interrupt

    @behavior Sync, Non-reentrant

    @pre none

    @param           none

    <br>Return values are documented in reverse-chronological order
    @return          void

    @post none

    @trace #BRCM_SWREQ_VGC_KERNEL_HANDLER

    @limitations none
*/
extern void VGC_DispDrvDSIInterruptHandler(void);

#endif /* VGC_OSIL_H */

/** @} */
