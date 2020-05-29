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

/** @defgroup grp_unicam_il Integration Guide
    @ingroup grp_unicam
    @addtogroup grp_unicam_il
    @{

    @file unicam_osil.h
    @brief Integration API's of  UNICAM
    This header file contains the UNICAM Integration interfaces
    @version 1.82 Fixed TBDs
    @version 1.81 Imported from AUTOSW_ISYS_Interface_Spec_1.81.docx
*/

#ifndef UNICAM_OSIL_H
#define UNICAM_OSIL_H

#include <stdint.h>
#include <system.h>
#include <isys.h>

/**
    @name UNICAM interface IDs
    @{
    @brief Interface IDs for UNICAM
*/
#define BRCM_SWARCH_UNICAM_HW_EVENT_MASK_MACRO      (0x81U)  /**< @brief #UNICAM_HW_EVENT_MASK_CTRL             */
#define BRCM_SWARCH_UNICAM_INT_MACRO                (0x82U)  /**< @brief #UNICAM_INT_ERROR                      */
#define BRCM_SWARCH_UNICAM_CMD_MACRO                (0x83U)  /**< @brief #UNICAM_CMD_INIT                       */
#define BRCM_SWARCH_UNICAM_MAX_NUM_IMG_BUFS_MACRO   (0x84U)  /**< @brief #UNICAM_MAX_NUM_IMG_BUFS               */
#define BRCM_SWARCH_UNICAM_MAX_NUM_DATA_BUFS_MACRO  (0x85U)  /**< @brief #UNICAM_MAX_NUM_DATA_BUFS              */
#define BRCM_SWARCH_UNICAM_MAGIC_MACRO              (0x86U)  /**< @brief #UNICAM_MAGIC                          */
#define BRCM_SWARCH_UNICAM_MODECFG_META_LINES_MACRO (0x87U)  /**< @brief #UNICAM_MODECFG_META_LINES_START_MASK  */
#define BRCM_SWARCH_UNICAM_CSICFG_DATALANE_MACRO    (0x88U)  /**< @brief #UNICAM_CSICFG_DATALANE_NUM_LINES_MASK */
#define BRCM_SWARCH_UNICAM_MODE_TYPE                (0x89U)  /**< @brief #UNICAM_ModeType                       */
#define BRCM_SWARCH_UNICAM_OUTPUT_IDX_TYPE          (0x8bU)  /**< @brief #UNICAM_OutputIdxType                  */
#define BRCM_SWARCH_UNICAM_FORMAT_TYPE              (0x8cU)  /**< @brief #UNICAM_FormatType                     */
#define BRCM_SWARCH_UNICAM_COMMON_IFC_CONFIG_TYPE   (0x8dU)  /**< @brief #UNICAM_CommonIfcConfigType            */
#define BRCM_SWARCH_UNICAM_MODE_CONFIG_TYPE         (0x8eU)  /**< @brief #UNICAM_ModeConfigType                 */
#define BRCM_SWARCH_UNICAM_CSI_CONFIG_TYPE          (0x8fU)  /**< @brief #UNICAM_CSIConfigType                  */
#define BRCM_SWARCH_UNICAM_TG_CONFIG_TYPE           (0x90U)  /**< @brief #UNICAM_TGConfigType                   */
#define BRCM_SWARCH_UNICAM_IMAGE_TYPE               (0x91U)  /**< @brief #UNICAM_ImageType                      */
#define BRCM_SWARCH_UNICAM_BUFFER_TYPE              (0x92U)  /**< @brief #UNICAM_BufferType                     */
#define BRCM_SWARCH_UNICAM_BUF_PARAM_TYPE           (0x93U)  /**< @brief #UNICAM_BufParamType                   */
#define BRCM_SWARCH_UNICAM_CUR_ED_PARAM_TYPE        (0x94U)  /**< @brief #UNICAM_CurED_ParamType                */
#define BRCM_SWARCH_UNICAM_IO_TYPE                  (0x95U)  /**< @brief #UNICAM_IOType                         */
#define BRCM_SWARCH_UNICAM_HW_PROCESS_INT_PROC      (0x96U)  /**< @brief #UNICAM_HW_ProcessInterrupt            */
#define BRCM_SWARCH_UNICAM_HW_PROCESS_ERR_INT_PROC  (0x97U)  /**< @brief #UNICAM_HW_ProcessErrorInterrupt       */
#define BRCM_SWARCH_UNICAM_SEND_CMD_PROC            (0x98U)  /**< @brief #UNICAM_SendCmd                        */
#define BRCM_SWARCH_UNICAM_SYS_CMD_HANDLER_PROC     (0x99U)  /**< @brief #UNICAM_SysCmdHandler                  */
#define BRCM_SWARCH_UNICAM_IRQ_HANDLER_PROC         (0x9aU)  /**< @brief #UNICAM_IRQHandler                     */
#define BRCM_SWARCH_UNICAM_IRQ_ERR_HANDLER_PROC     (0x9bU)  /**< @brief #UNICAM_IRQErrHandler                  */
/** @} */

/**
    @name UNICAM event masks
    @{
    @brief Macros for UNICAM event masks

    @trace #BRCM_SWREQ_UNICAM
*/
#define UNICAM_HW_EVENT_MASK_CTRL      (0x1UL)     /**< @brief Macro that notifies UNICAM control event                */
#define UNICAM_HW_EVENT_MASK_DATA_SOF  (0x2UL)     /**< @brief Macro that notifies SOF embedded data event from UNICAM */
#define UNICAM_HW_EVENT_MASK_DATA_EOF  (0x4UL)     /**< @brief Macro that notifies EOF embedded data event from UNICAM */
/** @} */

/**
    @name UNICAM interrupt mask type
    @{
    @brief Interrupt mask type for UNICAM

    UNICAM interrupt mask type.

    @trace  #BRCM_SWREQ_UNICAM
*/
#define UNICAM_INT_ERROR    (0x10UL)  /**< @brief Error interrupt       */
#define UNICAM_INT_FS       (0x20UL)  /**< @brief Frame Start interrupt */
#define UNICAM_INT_FE       (0x40UL)  /**< @brief Frame End interrupt   */
#define UNICAM_INT_DI       (0x80UL)  /**< @brief Data interrupt        */
#define UNICAM_INT_DI_END   (0x100UL) /**< @brief Data End interrupt    */
/** @} */

/**
    @name UNICAM Driver Commands
    @{
    @brief Macro for UNICAM commands

    Commands for implementing SVC handler functions.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_CMD_INIT            (0x1UL) /**< @brief Init Command            */
#define UNICAM_CMD_DEINIT          (0x2UL) /**< @brief Deinit Command          */
#define UNICAM_CMD_CONFIGURE       (0x3UL) /**< @brief Configure Command       */
#define UNICAM_CMD_START           (0x4UL) /**< @brief Start Command           */
#define UNICAM_CMD_STOP            (0x5UL) /**< @brief Stop Command            */
#define UNICAM_CMD_GET_EVENT       (0x6UL) /**< @brief Get event Command       */
#define UNICAM_CMD_GET_CUR_BUF_IDX (0x7UL) /**< @brief Get current buffer Idx  */
#define UNICAM_CMD_GET_EMB_BUFFER  (0x9UL) /**< @brief Get embdded buffer      */
#define UNICAM_CMD_GET_EMB_BUF_PTR (0x8UL) /**< @brief Get embdded buffer ptr  */
/** @} */

/**
    @brief Maximum number of UNICAM image buffers per exposure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAX_NUM_IMG_BUFS  (2UL)

/**
    @brief Maximum number of UNICAM data buffers per exposure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAX_NUM_DATA_BUFS (2UL)

/**
    @brief Magic for UNICAM configuration

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MAGIC         (0x554e4943UL)

/** @name Mode configuration meta-lines custom mask
    @{
    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_MODECFG_META_LINES_START_MASK  (0x0fU)
#define UNICAM_MODECFG_META_LINES_START_SHIFT (0U)
#define UNICAM_MODECFG_META_LINES_END_MASK    (0xf0U)
#define UNICAM_MODECFG_META_LINES_END_SHIFT   (4U)
/** @} */

/** @name  CSI configuration data-lane custom masks
    @{

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
#define UNICAM_CSICFG_DATALANE_NUM_LINES_MASK  (0x07U)
#define UNICAM_CSICFG_DATALANE_NUM_LINES_SHIFT (0U)
#define UNICAM_CSICFG_DATALANE_DLSMN_MASK      (0x08U)
#define UNICAM_CSICFG_DATALANE_DLSMN_SHIFT     (3U)
#define UNICAM_CSICFG_DATALANE_DLTREN_MASK     (0x10U)
#define UNICAM_CSICFG_DATALANE_DLTREN_SHIFT    (4U)
/** @} */

/**
    @name UNICAM Peripheral mode types
    @{
    @brief UNICAM Peripheral mode types

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint8_t UNICAM_ModeType;   /**< @brief UNICAM_ModeType typedef   */
#define UNICAM_MODE_CPI    (0U)    /**< @brief CPI Mode                  */
#define UNICAM_MODE_CSI    (1U)    /**< @brief CSI Mode                  */
#define UNICAM_MODE_TG     (2U)    /**< @brief TG Mode                   */
#define UNICAM_MODE_HISPI  (3U)    /**< @brief HISPI Mode                */
#define UNICAM_MODE_MAX    (4U)    /**< @brief Number of supported modes */
/** @} */

/**
    @name UNICAM output index
    @{
    @brief Output index for UNICAM output

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint32_t UNICAM_OutputIdxType;
#define UNICAM_OUTPUT_IDX_LONG  (0UL)  /**< @brief Long exposure              */
#define UNICAM_OUTPUT_IDX_SHORT (1UL)  /**< @brief Short exposure             */
#define UNICAM_OUTPUT_IDX_MAX   (2UL)  /**< @brief Maximum number of exposure */
/** @} */

/** @name UNICAM_RAWFormatType
    @{
    @brief UNICAM Image format

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef uint32_t UNICAM_FormatType; /**< @brief UNICAM Input image format type */
#define UNICAM_FORMAT_RAW6  (1U)  /**< @brief RAW6  */
#define UNICAM_FORMAT_RAW7  (2U)  /**< @brief RAW7  */
#define UNICAM_FORMAT_RAW8  (3U)  /**< @brief RAW8  */
#define UNICAM_FORMAT_RAW10 (4U)  /**< @brief RAW10 */
#define UNICAM_FORMAT_RAW12 (5U)  /**< @brief RAW12 */
#define UNICAM_FORMAT_RAW14 (6U)  /**< @brief RAW14 */
#define UNICAM_FORMAT_RAW16 (7U)  /**< @brief RAW16 */
/** @} */

/**
    @brief UNICAM common interface configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_CommonIfcConfigType {
    uint8_t hdr;
    uint8_t fps;
    uint8_t metaLines[UNICAM_OUTPUT_IDX_MAX];
    uint16_t shortLinesOffset;
    uint16_t width;
    uint16_t height;
    uint16_t stride;
} UNICAM_CommonIfcConfigType;

/**
    @brief UNICAM mode configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_ModeConfigType {
    uint8_t peripheralMode;
    uint8_t imgPipelineCtl;
    uint8_t format[UNICAM_OUTPUT_IDX_MAX];
} UNICAM_ModeConfigType;

/**
    @brief UNICAM CSI configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_CSIConfigType {
    uint8_t tclkTermEnable;
    uint8_t tclkSettle;
    uint8_t tdTermEnable;
    uint8_t thsSettle;
    uint8_t trxEnable;
    uint8_t dataLaneCtl;
    uint8_t passLineSync;
    uint8_t packetFrameTimeout;
    uint8_t vcNum[UNICAM_OUTPUT_IDX_MAX];
    uint8_t pad[2];
    uint32_t hsClkFrequency;
} UNICAM_CSIConfigType;

/**
    @brief UNICAM TG configuration structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_TGConfigType {
    uint8_t blkSize;
    uint8_t axiMinBurstLength;
    uint8_t axiMaxBurstLength;
    uint8_t pad;
    uint32_t tgCtl;
    uint32_t clkFrequency;
} UNICAM_TGConfigType;

/**
    @brief UNICAM Configuration image structure

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_ImageType {
    ISYS_CommonImgHdrType imgHdr;     /**< @brief image header as #ISYS_CommonImgHdrType */
    UNICAM_ModeConfigType modeConfig; /**< @brief mode as #UNICAM_ModeType               */
    union {
        UNICAM_CSIConfigType csiConfig;
        UNICAM_TGConfigType  tgConfig;
    } InterfaceConfig;
} UNICAM_ImageType;

/**
    @brief UNICAM buffer Type

    Structure for UNICAM buffer

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
 */
typedef struct _UNICAM_BufferType {
    uint32_t startAddr;   /**< @brief Start address */
    uint32_t endAddr;     /**< @brief End address   */
} UNICAM_BufferType;

/**
    @brief UNICAM buffer parameter

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
  */
typedef struct _UNICAM_BufParamType {
    UNICAM_BufferType imgBuf[UNICAM_MAX_NUM_IMG_BUFS][UNICAM_OUTPUT_IDX_MAX];   /**< @brief Image buffer                        */
    UNICAM_BufferType dataBuf[UNICAM_MAX_NUM_DATA_BUFS][UNICAM_OUTPUT_IDX_MAX]; /**< @brief Data buffer                         */
    uint32_t numImgBufs;                                                        /**< @brief Number of image buffer per exposure */
    uint32_t numExposures;                                                      /**< @brief Number of exposures                 */
} UNICAM_BufParamType;

/**
    @brief Current ED parameter

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
  */
typedef struct _UNICAM_CurED_ParamType {
    union {
        uint8_t *ptr;
        uint32_t addr;
    } ed;                 /**< @brief Embedded data pointer      */
    uint32_t expIdx;      /**< @brief Exposure index             */
    uint32_t beforeFrame; /**< @brief Is the ed is before frame? */
    uint32_t offset;      /**< @brief offset for the data        */
    uint32_t size;        /**< @brief Size of the data           */
} UNICAM_CurED_ParamType;

/**
    @brief UNICAM IO structure

    IO structure to implement SVC handler for UNICAM

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE
*/
typedef struct _UNICAM_IOType {
    int32_t status;                /**< @brief status to be returned by server */
    const UNICAM_ImageType *cfg;   /**< @brief cfg as #UNICAM_ImageType        */
    uint32_t event;                /**< @brief event to be returned by server  */
    uint32_t bufIdx;               /**< @brief bufIdx to be returned by server */
    UNICAM_BufParamType *buf;      /**< @brief Buffer parameters to be set     */
    UNICAM_CurED_ParamType *curED; /**< @brief Embedded buffer to be set by
                                                                       server  */
    UNICAM_CommonIfcConfigType* ifcCfg;
} UNICAM_IOType;

/** @brief Process the UNICAM control interrupts

    This interface processes the UNICAM control interrupts and returns the
    notification for client.

    @behavior Sync, Re-entrant

    @pre None

    @param  void

    @return     Following possible bit-masks are returned:
                - #UNICAM_HW_EVENT_MASK_CTRL
                - #UNICAM_HW_EVENT_MASK_DATA_SOF
                - #UNICAM_HW_EVENT_MASK_DATA_EOF

    @post None

    @note This interface shall be called from UNICAM interrupt service routines

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @snippet unicam_osil_erika.c Usage of UNICAM_HW_ProcessInterrupt
*/
uint32_t UNICAM_HW_ProcessInterrupt(void);

/** @brief Process the UNICAM error interrupts

    This interface processes the UNICAM error interrupts and returns the
    notification for client.

    @behavior Sync, Re-entrant

    @pre None

    @param  void

    @return     Following possible bit-masks are returned:
                - #UNICAM_HW_EVENT_MASK_CTRL

    @post None

    @note This interface shall be called from UNICAM interrupt service routines

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

    @snippet unicam_osil_erika.c Usage of UNICAM_HW_ProcessInterrupt
*/
uint32_t UNICAM_HW_ProcessErrorInterrupt(void);

/** @brief Send UNICAM command

    This API gives provision to execute driver APIs in privileged context.
    Users may call #UNICAM_CmdHandler in privileged context to achieve the
    same.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aCmd        UNICAM command ID
    @param[inout]   aCommonIO       Pointer to internal parameter structure

    @return     void

    @post None

    @note This interface shall be implemented by integrator.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_INTERFACE

    @limitations None
*/
int32_t UNICAM_SendCmd(const uint32_t aCmd, struct _UNICAM_IOType *aIO);

/** @brief UNICAM Driver SVC command handler

    This API handles commands requested by SysCmdReq to UNICAM driver.
    This API shall be hooked to SysCmdHandler to execute in
    exception context as per SADS.

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre None

    @param[in]      aMagicID    UNICAM Magic ID #BCM_UCM_SVC_ID
    @param[in]      aCmd        Command initiated by UNICAM driver
    @param[inout]   aSysIO      #uint8_t *

    @return     void

    @post None

    @note This interface shall be implemented by the integrator and shall
    install it to SVC layer.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None
*/
void UNICAM_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO);

/** @brief UNICAM Interrupt Handler to be installed in Vector table

    This interface calls @ref UNICAM_HW_ProcessInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

*/
void UNICAM_IRQHandler(void);

/** @brief UNICAM Error Interrupt Handler to be installed in Vector table

    This interface calls @ref UNICAM_HW_ProcessInterrupt and signal CAMERA_TASK.

    @behavior Sync, Non-reentrant

    @pre To be executed in Interrupt context

    @param[in]      void

    @retval     void

    @post None

    @note To be implemented by Integrator.

    @trace #BRCM_SWREQ_UNICAM_KERNEL_HANDLER

    @limitations None

*/
void UNICAM_IRQErrHandler(void);

#endif /* UNICAM_OSIL_H */
/** @} */
