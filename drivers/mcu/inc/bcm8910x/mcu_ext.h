/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_mcudrv_ifc
    @{

    @file mcu_ext.h
    @brief MCU extension interface
    This header file contains the interface functions for MCU driver

    @version 0.86 Imported from docx
*/


#ifndef INCLUDE_BCM8910X_MCU_EXT_H
#define INCLUDE_BCM8910X_MCU_EXT_H

#include <stdint.h>

/**
    @name MCU API IDs
    @{
    @brief API IDs for MCU Driver
*/
#define BRCM_SWARCH_MCU_CPUDHSID_TYPE          (0x36U) /**< @brief #MCU_CpuDHSIDType */
#define BRCM_SWARCH_MCU_CPUDHSCH_TYPE          (0x37U) /**< @brief #MCU_CpuDHSChType */
#define BRCM_SWARCH_MCU_SETDISPHSSRC_PROC      (0x38U) /**< @brief #MCU_SetDispHsSrc */
#define BRCM_SWARCH_MCU_DISPHSTRIGGER_PROC     (0x39U) /**< @brief #MCU_DispHsTrigger */
#define BRCM_SWARCH_MCU_BT656PADCFG_PROC       (0x3AU) /**< @brief #MCU_BT656PadCfg */
#define BRCM_SWARCH_MCU_SDPADCFG_PROC          (0x3BU) /**< @brief #MCU_SDPadCfg */
/** @} */

/**
    @brief Display hand-shake ID type

    @trace  #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef uint32_t MCU_CpuDHSIDType;
#define MCU_CPUDHSID_CPU        (0UL)   /**< CPU DHS src ID */
#define MCU_CPUDHSID_DISPLAY    (1UL)   /**< DISPLAY DHS src ID */
#define MCU_CPUDHSID_ISP        (2UL)   /**< ISP DHS src ID */

/**
    @brief Display handshake channel type

    @trace  #BRCM_SWREQ_MCU_KERNEL_INTERFACE
*/
typedef uint32_t MCU_CpuDHSChType;
/* DHS channels */
#define MCU_CPUDHSCH_0          (0UL)   /**< DHS Channel 0 */
#define MCU_CPUDHSCH_1          (1UL)   /**< DHS Channel 1 */

/** @brief Set the Handshake source for the input channel

    This API sets the Handshake source for the input channel

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aChannel            DHS channel ID
    @param[in]   aSrcId              Handshake source ID

    @return void

    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
extern void MCU_SetDispHsSrc(MCU_CpuDHSChType aChannel,
                                 MCU_CpuDHSIDType aSrcId);

/** @brief Trigger the Handshake for the input channel

    This API begins the Handshake for the input channel.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aChannel            DHS channel ID
    @param[in]   aBufAttr            DHS buffer attribute

    @return      void

    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
extern void MCU_DispHsTrigger(MCU_CpuDHSChType aChannel,
                                  uint32_t aBufAttr);

/** @brief Pad configuration for BT656 display

    This API will configure the PADs to send BT656 display over RGMII pins

    @behavior Sync, Non-reentrant

    @pre None

    @param       none

    @return      void

    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
extern void MCU_BT656PadCfg(void);

/** @brief Pad configuration for SD

    This API will configure the PADs required for SD

    @behavior Sync, Non-reentrant

    @pre None

    @param       none

    @return      void

    @post None

    @trace  #BRCM_SWREQ_MCU_EXTENSION_BCM8910X
*/
extern void MCU_SDPadCfg(void);

#endif /* INCLUDE_BCM8910X_MCU_EXT_H */

/** @} */
