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
    @addtogroup grp_isp_ifc
    @{

    @file isp_image.h
    @brief ISP image layout definitions
    @version 0.1
*/

#ifndef ISP_IMAGE_H
#define ISP_IMAGE_H
#include <isys.h>

/**
    @name ISP image interface IDs
    @{

    @brief Image interface IDs for ISP
*/
#define BRCM_SWARCH_ISP_MAGIC_MACRO           (0x0U)  /**< @brief #ISP_MAGIC            */
#define BRCM_SWARCH_ISP_MAX_CONFIGS_MACRO     (0x1U)  /**< @brief #ISP_MAX_CONFIGS      */
#define BRCM_SWARCH_ISP_MAX_CONFIG_SIZE_MACRO (0x2U)  /**< @brief #ISP_MAX_CONFIG_SIZE  */
#define BRCM_SWARCH_ISP_CFG_VERSION_TYPE      (0x3U)  /**< @brief #ISP_CfgVersionType   */
#define BRCM_SWARCH_ISP_CFG_PASS_TYPE         (0x4U)  /**< @brief #ISP_CfgPassType      */
#define BRCM_SWARCH_ISP_CFG_TYPE              (0x5U)  /**< @brief #ISP_CfgType          */
#define BRCM_SWARCH_ISP_PASS_ENABLE_TYPE      (0x6U)  /**< @brief #ISP_PassEnableType   */
#define BRCM_SWARCH_ISP_CONFIG_INFO_TYPE      (0x7U)  /**< @brief #ISP_ConfigInfoType   */
#define BRCM_SWARCH_ISP_IMAGE_HDR_TYPE        (0x8U)  /**< @brief #ISP_ImageHdrType     */
#define BRCM_SWARCH_ISP_IMAGE_TYPE            (0x9U)  /**< @brief #ISP_ImageType        */
/** @} */

/**
    @brief ISP Magic
    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_MAGIC           (0x49530421UL)

/**
    @brief Maximum number of configurations for ISP

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_MAX_CONFIGS     (82UL)

/**
    @brief Maximum size for ISP configuration

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_MAX_CONFIG_SIZE (5376UL) /* 21504 bytes */

/**
    @name ISP_CfgVersionType
    @{
    @brief ISP Configuration version

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_CfgVersionType;
#define ISP_CFG_VERSION_A0     (0x1U)
#define ISP_CFG_VERSION_B0     (0x2U)
#define ISP_CFG_VERSION_COMMON (0x3U)
/** @} */

/**
    @name ISP_CfgPassType
    @{
    @brief ISP configuration pass type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint8_t ISP_CfgPassType;
#define ISP_CFG_PASS_HV     (1U)
#define ISP_CFG_PASS_CV     (2U)
#define ISP_CFG_PASS_COMMON (3U)
/** @} */

/**
    @name ISP_CfgType
    @{
    @brief ISP configuration type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint16_t ISP_CfgType;
#define ISP_CFG_BLK (1U)
#define ISP_CFG_TCB (2U)
/** @} */

/**
    @name ISP_PassEnableType
    @{
    @brief Pass enable type

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef uint32_t ISP_PassEnableType;
#define ISP_PASS_ENABLE_HV   (0x1UL)
#define ISP_PASS_ENABLE_CV   (0x2UL)
/** @} */

/**
    @brief ISP Sub Image information

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ConfigInfoType {
    ISP_CfgVersionType version;
    ISP_CfgPassType pass;
    ISP_CfgType type;
    uint32_t deviceOffset;
    uint32_t dataOffset;
    uint32_t count; /* in words */
} ISP_ConfigInfoType;

/**
    @brief ISP Image header

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ImageHdrType {
    ISYS_CommonImgHdrType commonHdr;
    ISP_PassEnableType passEnable;
    uint32_t numConfigs;
} ISP_ImageHdrType;

/**
    @brief ISP Image Layout

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_ImageType {
    ISP_ImageHdrType imgHdr;
    ISP_ConfigInfoType cfgs[ISP_MAX_CONFIGS];
    uint32_t ispCfgData[ISP_MAX_CONFIG_SIZE];
} ISP_ImageType;

#endif /* ISP_IMAGE_H */
/** @} */