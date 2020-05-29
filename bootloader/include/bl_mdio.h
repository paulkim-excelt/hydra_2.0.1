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
    @defgroup grp_bl_mdio Master MDIO
    @ingroup grp_bl_drivers

    @addtogroup grp_bl_mdio
    @{
    @file bl_mdio.h
    @brief Master MDIO Interface APIs
    This file provides interface APIs for the MDIO block.

    @version 0.30 Imported from docx
*/

#ifndef BL_MDIO_H
#define BL_MDIO_H

/**
    @name MDIO API IDs
    @{
    @brief API IDs for MDIO
*/
#define BRCM_SWARCH_BL_MDIO_INIT_PROC           (0x00U) /**< @brief #BL_MDIO_Init */
#define BRCM_SWARCH_BL_MDIO_DEINIT_PROC         (0x01U) /**< @brief #BL_MDIO_DeInit */
#define BRCM_SWARCH_BL_MDIO_WRITE_PROC          (0x02U) /**< @brief #BL_MDIO_Write */
#define BRCM_SWARCH_BL_MDIO_READ_PROC           (0x03U) /**< @brief #BL_MDIO_Read */
#define BRCM_SWARCH_BL_MDIO_ACCESSMODE_TYPE     (0x04U) /**< @brief #BL_MDIO_AccessModeType */
/** @} */

/**
    @name BL_MDIO_AccessModeType
    @{
    @brief MDIO access mode
*/
typedef uint32_t BL_MDIO_AccessModeType;              /**< @brief typedef for MDIO access mode */
#define BL_MDIO_ACCESSMODE_CL22                 (1UL) /**< @brief Clause 22 access mode */
#define BL_MDIO_ACCESSMODE_CL45                 (2UL) /**< @brief Clause 45 access mode */

/** @brief Initialize MDIO block

    This API will initialize MDIO block

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_MDIO_INIT_PROC  #BRCM_REQ_BL_MDIO_INIT

    @limitations None
*/
extern void BL_MDIO_Init(void);

/** @brief DeInitialize MDIO block

    This API will de-initialize MDIO block

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  void

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_MDIO_DEINIT_PROC  #BRCM_REQ_BL_MDIO_INIT

    @limitations None
*/
extern void BL_MDIO_DeInit(void);

/** @brief Write to MDIO Slave

    This API will program a value to MDIO Slave

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aMode       Access mode
    @param[in]  aPhy        Phy address
    @param[in]  aDev        Device address
    @param[in]  aReg        Reg address
    @param[in]  aValue      Value

    @return     void

    @post None

    @trace  #BRCM_ARCH_BL_MDIO_WRITE_PROC  #BRCM_REQ_BL_MDIO_WRITE

    @limitations None
*/
extern void BL_MDIO_Write(BL_MDIO_AccessModeType aMode,
                         uint8_t aPhy,
                         uint8_t aDev,
                         uint16_t aReg,
                         uint16_t aValue);

/** @brief Read from MDIO Slave

    This API will read a register from MDIO Slave

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aMode       Access mode
    @param[in]  aPhy        Phy address
    @param[in]  aDev        Device address
    @param[in]  aReg        Reg address

    @return     Register value

    @post None

    @trace  #BRCM_ARCH_BL_MDIO_READ_PROC  #BRCM_REQ_BL_MDIO_WRITE

    @limitations None
*/

extern uint16_t BL_MDIO_Read(BL_MDIO_AccessModeType aMode,
                            uint8_t aPhy,
                            uint8_t aDev,
                            uint16_t aReg);

#endif /* BL_MDIO_H */

/** @} */
