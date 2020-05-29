/*****************************************************************************
 Copyright 2018 Broadcom Limited. All rights reserved.

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
    @defgroup grp_swr_cfg SWR Configuration
    @ingroup grp_bcm8910x_init

    @addtogroup grp_swr_cfg
    @{
    @file swr_cfg.h
    @brief Interface for SWR Configuration

    @version 0.30 Imported from docx
*/

#ifndef SWR_CFG_H
#define SWR_CFG_H

#include <stdint.h>

/**
    @name SWR Configuration API IDs
    @{
    @brief API IDs for SWR Configuration
 */
#define BRCM_ARCH_SYSCFG_MMI_WRITE_TYPE     (0x00U) /**< @brief #SYSCFG_MMIWriteType */
#define BRCM_ARCH_SYSCFG_SWR_CONFIG_TYPE    (0x01U) /**< @brief #SYSCFG_SWRConfigType */
/** @} */

/**
    @brief MMI write structure

    @trace #BRCM_ARCH_SYSCFG_MMI_WRITE_TYPE #BRCM_REQ_SYSCFG_MMI_WRITE_STRUCTURE
 */
typedef struct {
    uint8_t pAddr;  /**< @brief PHY address */
    uint8_t rAddr;  /**< @brief Register address */
    uint16_t value; /**< @brief Value to be programmed */
} SYSCFG_MMIWriteType;

/**
    @brief SWR Config structure

    @trace #BRCM_ARCH_SYSCFG_SWR_CONFIG_TYPE #BRCM_REQ_SYSCFG_SWR_CONFIG_STRUCTURE
 */
typedef struct {
    uint32_t magic;                            /**< @brief Magic number */
#define SWR_CONFIG_MAGIC        (0x43525753UL)
    uint32_t version;                          /**< @brief Switching Regulator configuration version */
    uint32_t count;                            /**< @brief Number of valid configurations in mmi (between 1-32) */
    uint32_t reserved;                         /**< @brief Reserved field */
#define SWR_CONFIG_COUNT        (32UL)
    SYSCFG_MMIWriteType mmi[SWR_CONFIG_COUNT]; /**< @brief Array of MMI write configurations */
    uint8_t file[128UL];                       /**< @brief File name from which this configuration is generated */
} SYSCFG_SWRConfigType;

#endif /* SWR_CFG_H */

/** @} */
