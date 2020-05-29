/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_cortex_rx Cortex-R Functionality
    @ingroup grp_cortex

    @addtogroup grp_cortex_rx
    @{

    @file cortex_rx.h
    @brief ARM Cortex-R Special functionality
    This header file contains the interface functions/macros for ARM Cortex-R
    Processor architecture special instructions

    @version 0.1 Initial Version
*/

#ifndef CORTEX_RX_H
#define CORTEX_RX_H

#include <compiler.h>
#include <cortex.h>
#include <inttypes.h>
#include <bcm_err.h>

/**
    @name Cortex Rx Architecture IDs
    @{
    @brief Architecture IDs for Cortex Rx Proecessor Specifics
*/
#define BRCM_SWARCH_CORTEX_RX_MPU_SIZE_TYPE         (0x00U)   /**< @brief #CORTEX_RX_MPUSizeType       */
#define BRCM_SWARCH_CORTEX_RX_MPU_ATTRIB_TYPE       (0x01U)   /**< @brief #CORTEX_RX_MPUAttribType     */
#define BRCM_SWARCH_CORTEX_RX_MPU_DISABLE_PROC      (0x02U)   /**< @brief #CORTEX_RX_MPUDisable        */
#define BRCM_SWARCH_CORTEX_RX_MPU_ENABLE_PROC       (0x03U)   /**< @brief #CORTEX_RX_MPUEnable         */
#define BRCM_SWARCH_CORTEX_RX_MPU_CONFIG_PROC       (0x04U)   /**< @brief #CORTEX_RX_MPUConfigure      */
/** @} */

/**
    @name Cortex Rx MPU Region Size
    @{
    @brief Macros for Cortex Rx MPU region size

    Type and Macros for valid region sizes for Cortex Rx MPU

    @trace #BRCM_SWREQ_CORTEX_RX
*/
typedef uint32_t CORTEX_RX_MPUSizeType;
#define CORTEX_RX_MPU_SIZE_32           (0x08UL)
#define CORTEX_RX_MPU_SIZE_64           (0x0AUL)
#define CORTEX_RX_MPU_SIZE_128          (0x0CUL)
#define CORTEX_RX_MPU_SIZE_256          (0x0EUL)
#define CORTEX_RX_MPU_SIZE_512          (0x10UL)
#define CORTEX_RX_MPU_SIZE_1K           (0x12UL)
#define CORTEX_RX_MPU_SIZE_2K           (0x14UL)
#define CORTEX_RX_MPU_SIZE_4K           (0x16UL)
#define CORTEX_RX_MPU_SIZE_8K           (0x18UL)
#define CORTEX_RX_MPU_SIZE_16K          (0x1AUL)
#define CORTEX_RX_MPU_SIZE_32K          (0x1CUL)
#define CORTEX_RX_MPU_SIZE_64K          (0x1EUL)
#define CORTEX_RX_MPU_SIZE_128K         (0x20UL)
#define CORTEX_RX_MPU_SIZE_256K         (0x22UL)
#define CORTEX_RX_MPU_SIZE_512K         (0x24UL)
#define CORTEX_RX_MPU_SIZE_1M           (0x26UL)
#define CORTEX_RX_MPU_SIZE_2M           (0x28UL)
#define CORTEX_RX_MPU_SIZE_4M           (0x2AUL)
#define CORTEX_RX_MPU_SIZE_8M           (0x2CUL)
#define CORTEX_RX_MPU_SIZE_16M          (0x2EUL)
#define CORTEX_RX_MPU_SIZE_32M          (0x30UL)
#define CORTEX_RX_MPU_SIZE_64M          (0x32UL)
#define CORTEX_RX_MPU_SIZE_128M         (0x34UL)
#define CORTEX_RX_MPU_SIZE_256M         (0x36UL)
#define CORTEX_RX_MPU_SIZE_512M         (0x38UL)
#define CORTEX_RX_MPU_SIZE_1G           (0x3AUL)
#define CORTEX_RX_MPU_SIZE_2G           (0x3CUL)
#define CORTEX_RX_MPU_SIZE_4G           (0x3EUL)
#define CORTEX_RX_MPU_SIZE_SR0_DISABLE  (0x01UL << 8UL)
#define CORTEX_RX_MPU_SIZE_SR1_DISABLE  (0x01UL << 9UL)
#define CORTEX_RX_MPU_SIZE_SR2_DISABLE  (0x01UL << 10UL)
#define CORTEX_RX_MPU_SIZE_SR3_DISABLE  (0x01UL << 11UL)
#define CORTEX_RX_MPU_SIZE_SR4_DISABLE  (0x01UL << 12UL)
#define CORTEX_RX_MPU_SIZE_SR5_DISABLE  (0x01UL << 13UL)
#define CORTEX_RX_MPU_SIZE_SR6_DISABLE  (0x01UL << 14UL)
#define CORTEX_RX_MPU_SIZE_SR7_DISABLE  (0x01UL << 15UL)
/** @} */

/**
    @name Cortex Rx MPU Region Attributes
    @{
    @brief Macros for Cortex Rx MPU region attributes

    Type and Macros for valid region attributes for Cortex Rx MPU

    @trace #BRCM_SWREQ_CORTEX_RX
*/
typedef uint32_t CORTEX_RX_MPUAttribType;
#define CORTEX_RX_MPU_ATTRIB_TEX_0           (0x0UL)
#define CORTEX_RX_MPU_ATTRIB_TEX_1           (0x1UL << 3UL)
#define CORTEX_RX_MPU_ATTRIB_TEX_2           (0x2UL << 3UL)
#define CORTEX_RX_MPU_ATTRIB_TEX_3           (0x3UL << 3UL)
#define CORTEX_RX_MPU_ATTRIB_SHAREABLE       (0x1UL << 2UL)
#define CORTEX_RX_MPU_ATTRIB_CACHEABLE       (0x1UL << 1UL)
#define CORTEX_RX_MPU_ATTRIB_BUFFERABLE      (0x1UL)
#define CORTEX_RX_MPU_ATTRIB_NO_ACCESS       (0x000UL)
#define CORTEX_RX_MPU_ATTRIB_PRIVILAGE       (0x100UL)
#define CORTEX_RX_MPU_ATTRIB_USER_READ       (0x200UL)
#define CORTEX_RX_MPU_ATTRIB_FULL_ACCESS     (0x300UL)
#define CORTEX_RX_MPU_ATTRIB_PRIVILAGE_READ  (0x500UL)
#define CORTEX_RX_MPU_ATTRIB_READ_ONLY       (0x600UL)
#define CORTEX_RX_MPU_ATTRIB_NEVER_EXEC      (0x1000UL)
/** @} */

/** @brief Disable MPU for Cortex Rx processor

    @behavior Sync, Non-reentrant

    @pre Caches are disabled, if there is a change in cache attributes for
    any memory region

    @param[in]      void

    @return void

    @post MPU is disabled

    @trace #BRCM_SWREQ_CORTEX_RX
*/
void CORTEX_RX_MPUDisable();

/** @brief Enable MPU for Cortex Rx processor

    @behavior Sync, Non-reentrant

    @pre MPU is disabled using #CORTEX_RX_MPU_Disable

    @param[in]      void

    @return void

    @post MPU is enabled

    @trace #BRCM_SWREQ_CORTEX_RX
*/
void CORTEX_RX_MPUEnable();

/** @brief Configure a specific region entry in MPU

    Apply user provided configuration to the user specified region in
    Cortex Rx MPU

    @behavior Sync, Non-reentrant

    @pre MPU is disabled using #CORTEX_RX_MPU_Disable

    @param[in]      aRegionNum      Region to which configuration need to be applied
    @param[in]      aBaseAddr       MPU Region Base address
    @param[in]      aRegionSize     MPU Region Size Configuration to be applied
    @param[in]      aRegionAttrib   MPU Region Attributes to be applied

    @return void

    @post MPU Region is updated with provided configuration

    @trace #BRCM_SWREQ_CORTEX_MX
*/
void CORTEX_RX_MPUConfigure(uint32_t aRegionNum, uint32_t aBaseAddr,
                                CORTEX_RX_MPUSizeType aRegionSize,
                                CORTEX_RX_MPUAttribType aRegionAttrib);


#endif  /* CORTEX_RX_H */

/** @} */
