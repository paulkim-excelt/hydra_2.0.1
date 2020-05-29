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

/** @defgroup grp_isp_il Integration Guide
    @ingroup grp_isp
    @addtogroup grp_isp_il
    @{
    @file isp_osil.h
    @brief Integration API's of  ISP
    This header file contains the ISP Integration interfaces
    @version 1.82 Fixed TBDs
    @version 1.81 Imported from AUTOSW_ISYS_Interface_Spec_1.81.docx
*/

#ifndef ISP_OSIL_H
#define ISP_OSIL_H

#include <stdint.h>
#include <system.h>
#include <isp.h>
#include <isp_image.h>
#include <isp_params.h>
#include <isp_configs.h>

/**
    @name ISP integration interface IDs
    @{

    @brief Integration interface IDs for ISP
*/
#define BRCM_SWARCH_ISP_HW_MASK_MACRO              (0x81U)  /**< @brief #ISP_HW_MASK_CTRL        */
#define BRCM_SWARCH_ISP_SVC_CMD_MACRO              (0x82U)  /**< @brief #ISP_SVC_CMD_INIT        */
#define BRCM_SWARCH_ISP_IO_PARAM_TYPE              (0x83U)  /**< @brief #ISP_IOParamType         */
#define BRCM_SWARCH_ISP_IO_TYPE                    (0x84U)  /**< @brief #ISP_IOType              */
#define BRCM_SWARCH_ISP_DRV_PROCESS_INTERRUPT_PROC (0x85U)  /**< @brief #ISP_DrvProcessInterrupt */
#define BRCM_SWARCH_ISP_SYS_CMD_REQ_PROC           (0x86U)  /**< @brief #ISP_SysCmdReq           */
#define BRCM_SWARCH_ISP_CMD_HANDLER_PROC           (0x87U)  /**< @brief #ISP_CmdHandler          */
#define BRCM_SWARCH_ISP_SYS_CMD_HANDLER_PROC       (0x88U)  /**< @brief #ISP_SysCmdHandler       */
#define BRCM_SWARCH_ISP_IRQ_HANDLER_PROC           (0x89U)  /**< @brief #ISP_IRQHandler          */
#define BRCM_SWARCH_ISP_IRQ_ERR_HANDLER_PROC       (0x8aU)  /**< @brief #ISP_NLinesIRQHandler   */
/** @} */

/**
    @name ISP event masks
    @{
    @brief Macros for ISP event masks

    @trace  #BRCM_SWREQ_ISP
*/
#define ISP_HW_MASK_CTRL   (0x1UL)   /**< @brief Macro that notifies ISP control event   */
#define ISP_HW_MASK_STATS  (0x2UL)   /**< @brief Macro that notifies statistics from ISP */
/** @} */

/**
    @name ISP SVC commands
    @{
    @brief Macros for ISP SVC commands

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_SVC_CMD_INIT                    (0x1UL)
#define ISP_SVC_CMD_DEINIT                  (0x2UL)
#define ISP_SVC_CMD_CONFIGURE               (0x3UL)
#define ISP_SVC_CMD_START                   (0x4UL)
#define ISP_SVC_CMD_STOP                    (0x5UL)
#define ISP_SVC_CMD_GETPARAM                (0x6UL)
#define ISP_SVC_CMD_SETPARAM                (0x7UL)
#define ISP_SVC_CMD_DISABLE_HW_SYNC         (0x8UL)
#define ISP_SVC_CMD_GET_EVENT               (0x9UL)
#define ISP_SVC_CMD_TRIGGER                 (0xaUL)
#define ISP_SVC_CMD_CONFIGURE_II            (0xbUL)
#define ISP_SVC_CMD_CONFIGURE_BL            (0xcUL)
#define ISP_SVC_CMD_CONFIGURE_LS            (0xdUL)
#define ISP_SVC_CMD_CONFIGURE_HF            (0xeUL)
#define ISP_SVC_CMD_CONFIGURE_DN            (0xfUL)
#define ISP_SVC_CMD_CONFIGURE_ST            (0x10UL)
#define ISP_SVC_CMD_CONFIGURE_WB            (0x11UL)
#define ISP_SVC_CMD_CONFIGURE_GAIN          (0x12UL)
#define ISP_SVC_CMD_CONFIGURE_DP            (0x13UL)
#define ISP_SVC_CMD_CONFIGURE_FG            (0x14UL)
#define ISP_SVC_CMD_CONFIGURE_XC            (0x15UL)
#define ISP_SVC_CMD_CONFIGURE_CDN           (0x16UL)
#define ISP_SVC_CMD_CONFIGURE_DM            (0x17UL)
#define ISP_SVC_CMD_CONFIGURE_SH            (0x18UL)
#define ISP_SVC_CMD_CONFIGURE_YC            (0x19UL)
#define ISP_SVC_CMD_CONFIGURE_GM            (0x1aUL)
#define ISP_SVC_CMD_CONFIGURE_YG            (0x1bUL)
#define ISP_SVC_CMD_CONFIGURE_FC            (0x1cUL)
#define ISP_SVC_CMD_CONFIGURE_LTM           (0x1dUL)
#define ISP_SVC_CMD_CONFIGURE_CP            (0x1eUL)
#define ISP_SVC_CMD_CONFIGURE_TM            (0x1fUL)
#define ISP_SVC_CMD_CONFIGURE_CC            (0x20UL)
#define ISP_SVC_CMD_CONFIGURE_HO            (0x21UL)
#define ISP_SVC_CMD_CONFIGURE_LR            (0x22UL)
#define ISP_SVC_CMD_CONFIGURE_LO            (0x23UL)
#define ISP_SVC_CMD_CONFIGURE_FR            (0x24UL)
#define ISP_SVC_CMD_CONFIGURE_INBUF         (0x25UL)
#define ISP_SVC_CMD_CONFIGURE_OUTBUF        (0x26UL)
#define ISP_SVC_CMD_CONFIGURE_HS            (0x27UL)
#define ISP_SVC_CMD_CONFIGURE_DUALPASS      (0x28UL)
#define ISP_SVC_CMD_SET_CVBUF               (0x29UL)
#define ISP_SVC_CMD_GET_CVBUF               (0x2aUL)
#define ISP_SVC_CMD_GET_STATISTICS          (0x2bUL)
#define ISP_SVC_CMD_UPDATE_FRAME_NUM        (0x2cUL)
#define ISP_SVC_CMD_UPDATE_FRAME_TIME_STAMP (0x2dUL)
/** @} */

/**
    @brief ISP SVC parameter union

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef union _ISP_IOParamType {
    const ISP_ImageType *cfg;
    ISP_ParamType param;
    ISP_TriggerInfoType *triggerInfo;
    const ISP_RawInputConfigType *iiCfg;
    const ISP_BlackLevelConfigType *blCfg;
    const ISP_LensShadingConfigType *lsCfg;
    const ISP_HDRConfigType *hdrCfg;
    const ISP_BayerDenoiseConfigType *dnCfg;
    const ISP_StatisticsConfigType *stCfg;
    const ISP_WBConfigType *wbCfg;
    const ISP_GainConfigType *gainCfg;
    const ISP_DefectivePixelConfigType *dpCfg;
    const ISP_FringingConfigType *fgCfg;
    const ISP_CrossTalkConfigType *xcCfg;
    const ISP_ColourDenoiseConfigType *cdnCfg;
    const ISP_DemosaicConfigType *dmCfg;
    const ISP_SharpenConfigType *shCfg;
    const ISP_ColourCorrectionConfigType *ycCfg;
    const ISP_GammaConfigType *gmCfg;
    const ISP_RGB2YUVConversionConfigType *ygCfg;
    const ISP_FalseColourConfigType *fcCfg;
    const ISP_LTMConfigType *ltmCfg;
    const ISP_ChrominanceStretchConfigType *cpCfg;
    const ISP_ToneMapConfigType *tmCfg;
    const ISP_ConversionConfigType *ccCfg;
    const ISP_OutputConfigType *outputCfg;
    const ISP_LoresResizeConfigType *lrCfg;
    const ISP_FrameConfigType *frCfg;
    ISP_InBufParamType *inBufCfg;
    ISP_OutBufParamType *outBufCfg;
    ISP_HandshakeParamType *hsCfg;
    ISP_DualPassParamType *dualPassCfg;
    ISP_StatsOutputType **statsP;
} ISP_IOParamType;

/**
    @brief ISP IO SVC Interface structure

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_IOType {
    int32_t status;
    uint32_t *frameNum;
    const uint32_t type;
    uint8_t *isEnabled;
    uint32_t event;
    const ISP_CfgPassType pass;
    ISP_IOParamType *ioParam;
    uint64_t frameTimeStamp;
} ISP_IOType;

/** @brief Process ISP interrupts

    Interface to process the ISP interrupts and returns the notification for client.

    @behavior Sync, Non-reentrant

    @pre None

    @param          void

    Return values are documented in reverse-chronological order
    @retval     #ISP_HW_MASK_CTRL        ISP control event
    @retval     #ISP_HW_MASK_STATS       statistics from ISP
    @return void (for void functions)

    @post None

    @note This interface shall be used by the integrator within the ISP
    interrupt service routine.

    @trace  #BRCM_SWREQ_ISP

    @limitations None

    @snippet isp_osil_erika.c Usage of ISP_DrvProcessInterrupt
*/
uint32_t ISP_DrvProcessInterrupt(void);

/** @brief Send ISP command

    This interface gives provision to execute driver APIs in privileged
    context. Users may call #ISP_CmdHandler in privileged context to
    achieve the same.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmd        ISP command ID
    @param[in]      aCommonIO   Pointer to internal parameter structure

    @return     void

    @post None

    @note This interface shall be implemented by integrator.

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE

    @limitations None
*/
int32_t ISP_SysCmdReq(const uint32_t aCmd, struct _ISP_IOType *aIO);

/** @brief Brief ISP system command handler

    This interface handles all the ISP commands requested from System command
    request.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aMagicID    ISP Magic ID #BCM_ISP_SVC_MAGIC_ID
    @param[in]      aCmd        Command initiated by ISP driver
    @param[inout]   aSysIO      #uint8_t * to be interpreted as  #_Cam_CommonIO

    @return     void

    @post None

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace  #BRCM_SWREQ_ISP_KERNEL_HANDLER

    @limitations None
*/
void ISP_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO);

/** @brief ISP Interrupt Handler to be installed in Vector table

    This interface calls @ref ISP_DrvProcessInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER

    @limitations None

*/
void ISP_IRQHandler(void);

/** @brief ISP Lines Interrupt Handler to be installed in Vector table

    This interface calls @ref ISP_DrvProcessInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_ISP_KERNEL_HANDLER

    @limitations None

*/
void ISP_NLinesIRQHandler(void);

#endif /* ISP_OSIL_H */
/** @} */
