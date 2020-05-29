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

#ifndef INCLUDE_BCM8910X_COMMON_EVK_BOARD_H
#define INCLUDE_BCM8910X_COMMON_EVK_BOARD_H

#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#endif
#if defined(ENABLE_NIF)
#include <nif.h>
#endif
#if defined(ENABLE_PCA9673)
#include <pca9673.h>
#endif
#if defined(ENABLE_VTMON)
#include <vtmon.h>
#endif

extern const uint8_t DefaultMacAddr[];
#if defined(ENABLE_ETH)
extern const ETHER_CfgType                  *ETHER_Cfg;
extern const ETHER_TimeCfgType *const       ETHER_TimeCfg;
extern const uint32_t                       ETHER_TimeCfgSize;
#if defined(ENABLE_NIF)
extern const NIF_CfgType             NIF_Config;
#endif
#endif

#if defined(ENABLE_VTMON)
extern const VTMON_ConfigType VTMONConfig;
#endif

#if defined(ENABLE_PCA9673)
typedef struct {
    PCA9673_ChannelType chan;
    PCA9673_LevelType level;
} PCA9673ChanCfgType;

extern const PCA9673ChanCfgType *const  PCA9673ChanCfgTbl;
extern const uint32_t                   PCA9673ChanCfgTblSize;

extern void PCA9673Init(void);
#endif

#if defined(ENABLE_CSI_TX_PATH)
extern void ConfigMax9295A_Init(void);
#endif

#endif /* INCLUDE_BCM8910X_COMMON_EVK_BOARD_H */
