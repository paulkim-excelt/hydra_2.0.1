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
#include <stdint.h>
#include <bl_chip_config.h>
#include <bl_utils.h>
#include <bl_log.h>
#include <bl_bcm_err.h>
#include <mcu.h>
#include <bl_ethernet.h>
#include <bl_eth_xcvr.h>
#include <bl_cntrl.h>
#include <eth_xcvr.h>
#include "bl_chip_misc.h"
#include "bl_brphy.h"
#include <afe_rdb.h>

#define BL_GetModuleLogLevel() (BL_LOG_LVL_INFO)

#define BL_ETHERCFG_RX_MAX_FRM_SIZE    (1522UL)

extern void BL_BCM8910X_SetXcvrConfig(ETHXCVR_ConfigType *aXcvrConfig,
                                        uint8_t *const aMacAddr);

extern int32_t BL_PIOCFG_SetEthIOCntrl(uint32_t aRGMIIEnable,
                                       BL_ETHER_XcvrSpeedType aSpeed);

static BL_CHIPMISC_RegsType* const BL_CHIPMISC_REGS = (BL_CHIPMISC_RegsType *)BL_CHIP_MISC_BASE;
static BL_BRPHY_RegsType * const BL_BRPHY_REGS = (BL_BRPHY_RegsType *)BL_BRPHY0_BR_CL22_IEEE_BASE;
static AFE_RDBType * const BL_AFE_RDB_REGS = (AFE_RDBType *)AFE_BASE;

void BL_BCM8910X_SetXcvrConfig(ETHXCVR_ConfigType *aXcvrConfig,
                               uint8_t *const aMacAddr)
{
    int32_t ret;
    BL_ETHER_XcvrCfgType xcvrConfig;
    uint32_t rgmiiEn = BL_FALSE;
    uint8_t macAddr[6UL] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
    uint32_t speed = BL_ETHER_XCVR_SPEED_100MBPS;

    BL_AFE_RDB_REGS->tx_config_1 = 0x00000800;

    if (ETHXCVR_CONFIG_MAGIC == aXcvrConfig->magic) {
        switch (aXcvrConfig->port[0UL].busMode) {
        case ETHXCVR_BUSMODE_RGMII:
            BL_CHIPMISC_REGS->PAD_CFG &= ~(PAD_CFG_RGMII_MODEHV_MASK);
            BL_BRPHY_REGS->miiCtrl |= BL_BRPHY0_BR_CL22_IEEE_MII_CTRL_POWER_DOWN_MASK;
            speed = BL_ETHER_XCVR_SPEED_1000MBPS;
            BL_LOG_INFO("Configured as RGMII at 1Gbps\n");
            rgmiiEn = BL_TRUE;
        break;
        case ETHXCVR_BUSMODE_INTG:
            if (ETHXCVR_BOOLEAN_TRUE == aXcvrConfig->port[0UL].phy.masterMode) {
                BL_LOG_INFO("BRPHY Configured as MASTER\n");
                *((volatile uint32_t*)0x410FFFC0) |= 0x8;
            } else {
                BL_LOG_INFO("BRPHY Configured as SLAVE\n");
                *((volatile uint32_t*)0x410FFFC0) &= ~(0x8);
            }
        break;
        default:
        break;
        }
    }

    if (NULL != aMacAddr) {
        macAddr[0UL] = aMacAddr[0UL];
        macAddr[1UL] = aMacAddr[1UL];
        macAddr[2UL] = aMacAddr[2UL];
        macAddr[3UL] = aMacAddr[3UL];
        macAddr[4UL] = aMacAddr[4UL];
        macAddr[5UL] = aMacAddr[5UL];
    }

    BL_LOG_ERR("mac = %02x:%02x:%02x:%02x:%02x:%02x\n", macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
    ret = BL_PIOCFG_SetEthIOCntrl(rgmiiEn, speed);
    if (ret != BL_BCM_ERR_OK) {
        BL_LOG_ERR("BL_PIOCFG_SetEthIOCntrl failed!!\n");
    }
    xcvrConfig.speed = speed;
    xcvrConfig.mode = BL_ETHER_XCVR_DUPLEX_MODE_FULL;
    xcvrConfig.maxRxFrmLen = BL_ETHERCFG_RX_MAX_FRM_SIZE;
    xcvrConfig.macAddr = macAddr;

    BL_ETHER_XcvrInit(0UL, &xcvrConfig);
}
