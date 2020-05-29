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

#include <board.h>
#include <avt.h>
#include <chip_config.h>
#if defined(ENABLE_DMA330)
#include <dma.h>
#endif
#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#endif
#if defined(ENABLE_NIF)
#include <nif.h>
#endif
#ifdef ENABLE_ETS
#include "ets_osil.h"
#endif
#include <syscfg.h>
#include <ulog.h>
#include <bcm_utils.h>
#if defined(ENABLE_IIC_BSC)
#include <osil/i2c_osil.h>
#endif
#if defined(ENABLE_WLAN)
#include <wl_eth.h>
#include <wl_eth_xcvr.h>
#endif

#include <osil/ptm_osil.h>
#include <osil/flsmgr_osil.h>
#include <init.h>
#include <osil/bcm_osil.h>
#if defined(ENABLE_OTP)
#include <otp.h>
#endif

#include <imgl.h>

#include "ee_internal.h"
#include "ee.h"

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define SYS_CFG_EVENT           (SystemEvent1)
#define WLETH_INIT_EVENT        (SystemEvent1)

/*
 * Externs
 */
/** TODO: Extern declaration of COMMS_GetMacAddr should be removed
 * after movement of WLAN Init to comms_osil.c
 */
extern void COMMS_GetMacAddr(uint8_t *const aMacAddr);

#if defined(ENABLE_WLAN) && !defined(ENABLE_WLAN_TEST)
#define WL_FW_SIZE_MAX      (0xA0000)
/* Area where WIFI firmware is loaded from flash */
static uint8_t COMP_SECTION(".wifi.fw") WL_FWLoadRegion[WL_FW_SIZE_MAX];
/** TODO: Reduce this allocation in wifi driver and use small area
 * for verification
 */
static uint8_t COMP_SECTION(".wifi.fw") WL_FWVerifyRegion[WL_FW_SIZE_MAX];

static uint32_t WlCntrlMode = ETHER_MODE_DOWN;
static uint32_t WlXcvrMode= ETHXCVR_MODE_DOWN;
static TaskType InitWlantaskID;
static uint32_t WlFwSize = 0UL;
static uint32_t WlFwLoaded = FALSE;
WLXCVR_CfgType wlEthXcvrCfg;
WLETH_CfgType wlEthCfg;
void WLETH_ModeChangeInd(ETHER_HwIDType aID,
        ETHER_ModeType aMode)
{
    WlCntrlMode = aMode;
    (void)BCM_SetEvent(InitWlantaskID, WLETH_INIT_EVENT);
}

void WLXCVR_ModeChangeInd(ETHXCVR_IDType aID,
        ETHXCVR_ModeType aMode)
{
    WlXcvrMode = aMode;
    (void)BCM_SetEvent(InitWlantaskID, WLETH_INIT_EVENT);
}

int32_t WLETH_GetFirmwareInfo(ETHXCVR_IDType aID,
        uint8_t const ** aAddr, uint8_t **aScratch, uint32_t *const aSize)
{
    int ret = BCM_ERR_OK;

    if ((aAddr == NULL) || (aSize == NULL)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    if (TRUE != WlFwLoaded) {
        ret = BCM_ERR_UNINIT;
        goto err_exit;
    }

    *aAddr = WL_FWLoadRegion;
    *aScratch = WL_FWVerifyRegion;
    *aSize = WlFwSize;

err_exit:
    return ret;
}

int32_t WLETH_GetNvramInfo(ETHXCVR_IDType aID,
        uint8_t const ** aAddr, uint32_t *const aSize)
{
    int ret = BCM_ERR_OK;
    if ((aAddr == NULL) || (aSize == NULL)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }

    *aAddr = (uint8_t *)WLAN_NvramCfg;
    *aSize = WLAN_NvramCfgSz;
err_exit:
    return ret;
}

static void BCM8910X_InitWlan(void)
{
    BCM_EventMaskType mask;
    StatusType status;
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;
    int32_t ret = BCM_ERR_OK;
    uint8_t macAddr[6UL] = {(uint8_t)0};

    /* Retrieve MAC address */
    COMMS_GetMacAddr(macAddr);

    status = GetTaskID(&InitWlantaskID);
    if (status == E_OK) {
        BCM_MemCpy(&wlEthXcvrCfg, &WLETH_XcvrCfg,
                sizeof(WLXCVR_CfgType));
        BCM_MemCpy(&wlEthCfg, &WLETH_Cfg, sizeof(WLETH_CfgType));
        wlEthCfg.macAddr = macAddr;

        ret = IMGL_LoadImg(0xBC30, WL_FWLoadRegion, 0UL,
                sizeof(WL_FWLoadRegion),
                WLETH_INIT_EVENT, &imglStatus);
        if (ret !=  BCM_ERR_OK) {
            ULOG_ERR("IMGL_LoadImg() failed\n");
            goto err_exit;
        }

        BCM_WaitEvent(WLETH_INIT_EVENT);
        BCM_GetEvent(InitWlantaskID, &mask);
        if ((mask & WLETH_INIT_EVENT) != 0UL) {
            BCM_ClearEvent(WLETH_INIT_EVENT);
        }
        ptmStatus = IMGL_GetStatus(&imglStatus);
        if (ptmStatus == BCM_ERR_OK) {
            WlFwSize = imglStatus.size;
            ULOG_ERR("FW data loaded@0x%x\n", WL_FWLoadRegion);
            WlFwLoaded = TRUE;
        } else {
            ULOG_ERR("IMGL_GetStatus() failed\n");
            goto err_exit;
        }
        WLETH_Init(wlEthCfg.hwID, &wlEthCfg);
        ret = WLETH_SetMode(wlEthCfg.hwID, ETHER_MODE_ACTIVE);
        /* wait for the controller to become active */
        if (BCM_ERR_OK == ret) {
            ret = BCM_WaitEvent(WLETH_INIT_EVENT);
            if (ret == BCM_ERR_OK) {
                ret = BCM_GetEvent(InitWlantaskID, &mask);
                if ((BCM_ERR_OK == ret) && ((mask & WLETH_INIT_EVENT) != 0UL)) {
                    (void)BCM_ClearEvent(WLETH_INIT_EVENT);
                    if (WlCntrlMode != ETHER_MODE_ACTIVE) {
                        ret = BCM_ERR_UNKNOWN;
                    }
                }
            }
        }
        if ((E_OK == status) && (BCM_ERR_OK == ret)) {
            /* wait for the controller to become active */
            WLXCVR_Init(wlEthXcvrCfg.xcvrID, &wlEthXcvrCfg);
            ret = WLXCVR_SetMode(wlEthXcvrCfg.xcvrID, ETHXCVR_MODE_ACTIVE);
            if (BCM_ERR_OK == ret) {
                ret = BCM_WaitEvent(WLETH_INIT_EVENT);
                if (ret == BCM_ERR_OK) {
                    ret = BCM_GetEvent(InitWlantaskID, &mask);
                    if ((ret == BCM_ERR_OK) && ((mask & WLETH_INIT_EVENT) != 0UL)) {
                        (void)BCM_ClearEvent(WLETH_INIT_EVENT);
                    }
                }
            }
        }
    }
err_exit:
    if ((E_OK != status) || (BCM_ERR_OK != ret)) {
        ULOG_ERR("%s: failed\n", __func__);
    }
}
#endif

#if defined (ENABLE_IIC_BSC)
#define LATE_INIT_EVENT         (SystemEvent1)

static void InitIIC(void)
{
    uint32_t i;

    for (i = 0UL; i < IIC_ConfigTblSz; i++) {
        IIC_Init(IIC_Config[i].id, &IIC_Config[i]);
    }
}
#endif  /* ENABLE_IIC_BSC */

#if defined(ENABLE_I2S) && !defined(ENABLE_AUDIO_TEST)
static void InitAudio(void)
{
    uint32_t i;

    for (i = 0UL; i < Audio_CfgTblSz; i++) {
        Audio_Init(Audio_CfgTbl[i].hwID, &Audio_CfgTbl[i]);
    }
}
#endif


#if defined(ENABLE_PP) && !defined(ENABLE_PP_TEST)
#include <pp.h>
extern uint8_t __fast_dma_start__[];
#ifndef PP_CODEC_MODE
#define PP_CODEC_MODE (0UL)
#endif

#define MAX_SIZE_BRCM_PP_MP      (32UL * 1024UL)
#define MAX_SIZE_BRCM_PP_SP      (64UL * 1024UL)
#define MAX_SIZE_BRCM_PP_CODEC   (64UL * 1024UL)

#define OFFSET_BRCM_PP_MP    (0UL)
#define OFFSET_BRCM_PP_SP    (MAX_SIZE_BRCM_PP_MP)
#define OFFSET_BRCM_PP_CODEC (MAX_SIZE_BRCM_PP_MP + MAX_SIZE_BRCM_PP_SP)
int32_t BCM8910X_InitPP()
{
    IMGL_LoadStatusType imglStatus;
    int32_t ptmStatus;
    int32_t retVal;
    uint8_t* mpBinary = NULL;
    uint8_t* spBinary = NULL;
    uint8_t* codecBinary = NULL;


    retVal = IMGL_LoadImg((uint16_t)BRCM_PP_MP_CONFIG,
                         &__fast_dma_start__[OFFSET_BRCM_PP_MP], 0UL,
                         MAX_SIZE_BRCM_PP_MP,
                         SystemEvent1,
                         &imglStatus);
    if (BCM_ERR_OK == retVal) {
        (void)BCM_WaitEvent(SystemEvent1);
        (void)BCM_ClearEvent(SystemEvent1);
        ptmStatus = IMGL_GetStatus(&imglStatus);
        if (ptmStatus != BCM_ERR_OK) {
            ULOG_ERR("BCM8910X_InitPP PTM load MP failed with %d\n", ptmStatus);
        } else {
            mpBinary = &__fast_dma_start__[OFFSET_BRCM_PP_MP];
        }
    }

    retVal = IMGL_LoadImg((uint16_t)BRCM_PP_SP_CONFIG,
                         &__fast_dma_start__[OFFSET_BRCM_PP_SP], 0UL,
                         MAX_SIZE_BRCM_PP_SP,
                         SystemEvent1,
                         &imglStatus);
    if (BCM_ERR_OK == retVal) {
        (void)BCM_WaitEvent(SystemEvent1);
        (void)BCM_ClearEvent(SystemEvent1);
        ptmStatus = IMGL_GetStatus(&imglStatus);
        if (ptmStatus != BCM_ERR_OK) {
            if (BCM_ERR_NOT_FOUND != ptmStatus) {
                ULOG_ERR("BCM8910X_InitPP PTM load SP failed with %d\n", ptmStatus);
            }
        } else {
            spBinary = &__fast_dma_start__[OFFSET_BRCM_PP_SP];
        }
    }

    retVal = IMGL_LoadImg((uint16_t)BRCM_PP_CODEC_CONFIG,
                         &__fast_dma_start__[OFFSET_BRCM_PP_CODEC], 0UL,
                         MAX_SIZE_BRCM_PP_CODEC,
                         SystemEvent1,
                         &imglStatus);
    if (BCM_ERR_OK == retVal) {
        (void)BCM_WaitEvent(SystemEvent1);
        (void)BCM_ClearEvent(SystemEvent1);
        ptmStatus = IMGL_GetStatus(&imglStatus);
        if (ptmStatus != BCM_ERR_OK) {
            ULOG_ERR("BCM8910X_InitPP PTM load CODEC failed with %d\n", ptmStatus);
        } else {
            codecBinary = &__fast_dma_start__[OFFSET_BRCM_PP_CODEC];
        }
    }

    retVal = PP_Init(PP_CODEC_MODE, mpBinary, spBinary, codecBinary);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("PP_Init failed %d\n",retVal);
    }

    return retVal;
}
#endif

#if defined(ENABLE_TIMER_SP804) && !defined(ENABLE_TIMER_TEST)
#if defined(ENABLE_HRTIMER)
static void BCM8910X_HRTimerCb(void)
{
    int ret;
#if !defined(__OO_NO_ALARMS__) && defined(HRTimer)
    ret = IncrementCounterHardware(HRTimer);
    if (E_OK != ret) {
    }
#endif
}
#endif

static void TIM_OneShotExpiryCb(void)
{
}

const TIM_ConfigType TIM_Config[] = {
#if defined(ENABLE_HRTIMER)
    {
        .prescale = TIM_PRESCALE_DIV_1,
        .chanID = 0UL,
        .chanMode = TIM_CHAN_MODE_PERIODIC,
        .width = TIM_WIDTH_32_BIT,
        .cb = BCM8910X_HRTimerCb,
    },
#endif
    {
        .prescale = TIM_PRESCALE_DIV_1,
        .chanID = 1UL,
        .chanMode = TIM_CHAN_MODE_ONE_SHOT,
        .width = TIM_WIDTH_32_BIT,
        .cb = TIM_OneShotExpiryCb,
    },
};

const uint32_t TIM_ConfigTblSize = (sizeof(TIM_Config) / sizeof(TIM_ConfigType));

static void BCM8910X_InitTimer(void)
{
    uint32_t i;

    for (i = 0UL; i < TIM_ConfigTblSize; i++) {
        TIM_Init(TIM_Config[i].chanID, &TIM_Config[i]);
    }
}
#endif

static int32_t BcmCfg_Init(void)
{
    int32_t retVal = BCM_ERR_OK;

#if defined(ENABLE_OTP) && !defined(ENABLE_OTP_TEST)
    OTP_Init(OTP_HW_ID_0);
#endif

#if defined(ENABLE_TIMER_SP804) && !defined(ENABLE_TIMER_TEST)
    BCM8910X_InitTimer();
#endif

#if defined(ENABLE_SD) && !defined(ENABLE_SD_TEST)
    SD_Init(SD_Cfg.slotID, &SD_Cfg);
#endif

#if defined(ENABLE_PP) && !defined(ENABLE_PP_TEST)
    retVal = BCM8910X_InitPP();
#endif

#if defined(ENABLE_WLAN) && !defined (ENABLE_WLAN_TEST)
    BCM8910X_InitWlan();
#endif

#if defined(ENABLE_DMA330) && !defined(ENABLE_DMA_TEST)
    DMA_Init();
#endif

#if defined(ENABLE_IIC_BSC) && !defined(ENABLE_IIC_TEST)
    InitIIC();
#endif

#if defined(ENABLE_I2S) && !defined(ENABLE_AUDIO_TEST)
    InitAudio();
#endif

#if defined(ENABLE_VTMON) && !defined(ENABLE_VTMON_TEST)
    VTMON_Init(&VTMONConfig);
#endif

    return retVal;
}

int32_t BCM8910X_LateInit(void)
{
    int32_t retVal = BCM_ERR_OK;

    retVal = BcmCfg_Init();
    Board_Init();

#if defined(ENABLE_VGCC_DISPLAY)
    Display_Init();
#endif  /* ENABLE_VGCC_DISPLAY */

    return retVal;
}
