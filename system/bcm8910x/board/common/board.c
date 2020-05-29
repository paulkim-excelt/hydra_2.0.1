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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdint.h>
#include <ulog.h>
#include <bcm_utils.h>
#include <osil/log_osil.h>
#include <board.h>
#include <chip_config.h>
#include <ee.h>
#include <osil/bcm_osil.h>
#if defined(ENABLE_NIF)
#include <nif.h>
#ifdef ENABLE_WLAN
#include <wl_eth.h>
#endif
#endif
#if defined(ENABLE_1722) && defined(ENABLE_AVB_TALKER)
#include <avb_talker.h>
#endif
#if defined(ENABLE_1722) && defined(ENABLE_AVB_LISTENER)
#include <avb_listener.h>
#endif
#if defined(ENABLE_ETS) || defined(ENABLE_ETS_TEST)
#include <ets_osil.h>
#endif
#if defined(ENABLE_INET)
#include <inet_osil.h>
#endif
#if defined(ENABLE_ETH_BRPHY)
#include <brphy_osil.h>
#endif
#if defined(ENABLE_OTP)
#include <otp.h>
#endif


#if !defined(ENABLE_UART_TEST)
#include <uart_osil.h>
#include <uconsole_osil.h>
/**
 * UART configuration
 */
const UART_ConfigType UART_Config[] = {
    {
        .txFifoLvl = UART_FIFO_LVL_1DIV8,
        .rxFifoLvl = UART_FIFO_LVL_7DIV8,
        .baud = UART_BAUD_115200,
        .stopBits = UART_STOP_BITS1,
        .parity = UART_PARITY_NONE,
        .loopBackMode = 0UL,
        .txCb = UCONSOLE_TxConfirmation,
        .rxCb = UCONSOLE_RcvIndication,
        .errCb = UCONSOLE_ErrIndication,
    },
};
#endif


#define ETHERCFG_RX_MAX_FRM_SIZE    (1522UL)

/* Use the event reserved for System_LateInit */
#define BOARD_INIT_EVENT_MASK (SystemEvent1)

#define IIC_HW_ID_0                 (0UL)

#ifdef ENABLE_PCA9673
#define IIC_HW_ID_GPIO_EXP          (IIC_HW_ID_0)
#define PCA9673_SLAVE_ADDR          (0x4AU >> 1U)
#endif

#if defined(ENABLE_MAC0)
const uint8_t DefaultMacAddr[] = {0x02, 0x01, 0x02, 0x03, 0x04, 0x05};
#elif defined(ENABLE_MAC1)
const uint8_t DefaultMacAddr[] = {0x02, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};
#else
const uint8_t DefaultMacAddr[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif

#if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST)

static const ETHER_CfgType ETHER_Config = {
    .hwID = ETHER_HW_ID_0,
    .speed = ETHER_SPEED_100MBPS,
    .duplexMode = ETHER_DUPLEX_MODE_FULL,
    .maxRxFrmLen = ETHERCFG_RX_MAX_FRM_SIZE,
    .macAddr = DefaultMacAddr,
    .prioChanMap[0] = 3UL,
    .prioChanMap[1] = 3UL,
    .prioChanMap[2] = 0UL, /* AVB traffic */
    .prioChanMap[3] = 0UL, /* AVB traffic */
    .prioChanMap[4] = 3UL,
    .prioChanMap[5] = 3UL,
    .prioChanMap[6] = 2UL, /* GPTP traffic */
    .prioChanMap[7] = 2UL,
};

const ETHER_CfgType  *ETHER_Cfg = &ETHER_Config;

const ETHER_TimeCfgType ETHER_TimeConfig[] = {
    {
        .hwIdx = 0UL,
        .portEnableMask = P1588_PORT_MASK,
        .sync = ETHER_TIMESYNC_NONE,
    },
};

const ETHER_TimeCfgType *const ETHER_TimeCfg = ETHER_TimeConfig;
const uint32_t ETHER_TimeCfgSize = sizeof(ETHER_TimeConfig)/
                                    sizeof(ETHER_TimeCfgType);

#endif /* if defined(ENABLE_ETH) && !defined(ENABLE_ETHER_TEST) */

#if defined(ENABLE_ETH)
int32_t ETHXCVR_BoardGetPhyFnTbl(const ETHXCVR_PortConfigType *const aConfig,
                                 const ETHXCVR_FuncTblType **const aPhyFnTbl)
{
    *aPhyFnTbl = NULL;
    return BCM_ERR_OK;
}
#endif /* if defined(ENABLE_ETH) */

#if defined(ENABLE_NIF)
static const NIF_CbTblType NIF_CtrlCbTbl[] = {
#if defined(ENABLE_1722)
    {
        .clientID = NIF_CLIENT_ID(BCM_AVB_ID, 0U),
        .etherType = ETHER_ETHERTYPE_1722,
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#if defined(ENABLE_AVB_LISTENER)
        .rxCb = AVBL_RxPktIndication,
#else
        .rxCb = NULL,
#endif
#if defined(ENABLE_AVB_TALKER)
        .txCb = AVBT_TxPktConfirmation,
        .bufAvailCb = AVBT_BufAvailIndication,
#else
        .txCb = NULL,
        .bufAvailCb = NULL,
#endif
        .rxTSCb = NULL,
        .txTSCb = NULL,
        .rxMgmtInfoCb = NULL,
    },
    {
        .clientID = NIF_CLIENT_ID(BCM_AVB_ID, 1U),
        .etherType = ETHER_ETHERTYPE_1722,
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#if defined(ENABLE_AVB_LISTENER)
        .rxCb = AVBL_RxPktIndication,
#else
        .rxCb = NULL,
#endif
#if defined(ENABLE_AVB_TALKER)
        .txCb = AVBT_TxPktConfirmation,
        .bufAvailCb = AVBT_BufAvailIndication,
#else
        .txCb = NULL,
        .bufAvailCb = NULL,
#endif
        .rxTSCb = NULL,
        .txTSCb = NULL,
        .rxMgmtInfoCb = NULL,
    },
    {
        .clientID = NIF_CLIENT_ID(BCM_AVB_ID, 2U),
        .etherType = ETHER_ETHERTYPE_1722,
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#if defined(ENABLE_AVB_LISTENER)
        .rxCb = AVBL_RxPktIndication,
#else
        .rxCb = NULL,
#endif
#if defined(ENABLE_AVB_TALKER)
        .txCb = AVBT_TxPktConfirmation,
        .bufAvailCb = AVBT_BufAvailIndication,
#else
        .txCb = NULL,
        .bufAvailCb = NULL,
#endif
        .rxTSCb = NULL,
        .txTSCb = NULL,
        .rxMgmtInfoCb = NULL,
    },
    {
        .clientID = NIF_CLIENT_ID(BCM_AVB_ID, 3U),
        .etherType = ETHER_ETHERTYPE_1722,
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#if defined(ENABLE_AVB_LISTENER)
        .rxCb = AVBL_RxPktIndication,
#else
        .rxCb = NULL,
#endif
#if defined(ENABLE_AVB_TALKER)
        .txCb = AVBT_TxPktConfirmation,
        .bufAvailCb = AVBT_BufAvailIndication,
#else
        .txCb = NULL,
        .bufAvailCb = NULL,
#endif
        .rxTSCb = NULL,
        .txTSCb = NULL,
        .rxMgmtInfoCb = NULL,
    },
#endif /* ENABLE_1722 */
#if defined(ENABLE_INET)
    {
        .clientID = NIF_CLIENT_ID(BCM_NET_ID, 0U),
        .etherType = ETHER_ETHERTYPE_IP,
#ifdef ENABLE_WLAN
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRELESS, 0U),
#else
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#endif
        .rxCb = INET_RxIPv4Pkt,
        .txCb = INET_TxPktConfirmation,
        .rxTSCb = NULL,
        .rxMgmtInfoCb = NULL,
        .txTSCb = NULL,
        .bufAvailCb = NULL,
    },
    {
        .clientID = NIF_CLIENT_ID(BCM_NET_ID, 0U),
        .etherType = ETHER_ETHERTYPE_ARP,
#ifdef ENABLE_WLAN
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRELESS, 0U),
#else
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#endif
        .rxCb = INET_RxARPPkt,
        .txCb = INET_TxPktConfirmation,
        .rxTSCb = NULL,
        .rxMgmtInfoCb = NULL,
        .txTSCb = NULL,
        .bufAvailCb = NULL,
    },
#endif
#if defined(ENABLE_ETS) || defined(ENABLE_ETS_TEST)
    {
        .clientID = NIF_CLIENT_ID(BCM_ETS_ID, 0U),
        .etherType = ETHER_ETHERTYPE_GPTP,
#ifdef ENABLE_WLAN
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRELESS, 0U),
#else
        .cntrlIndex = NIF_CNTRLR_ID(NIF_ETHCNTRLR_WIRED, 0U),
#endif
        .rxCb = ETS_RxPktIndication,
        .txCb = ETS_TxPktConfirmation,
        .rxTSCb = ETS_RxPktTSIndication,
        .txTSCb = ETS_TxPktTSIndication,
        .rxMgmtInfoCb = NULL,
        .linkStateChgCb = NULL,
        .bufAvailCb = NULL,
    },
#endif
};

static const NIF_ClientIDType NIF_ClientStreamsTbl[] = {
#if defined(ENABLE_1722) && defined(ENABLE_AVB_TALKER)
    NIF_CLIENT_ID(BCM_AVB_ID, 0),
    NIF_CLIENT_ID(BCM_AVB_ID, 1),
    NIF_CLIENT_ID(BCM_AVB_ID, 2),
    NIF_CLIENT_ID(BCM_AVB_ID, 3),
#endif
};

static const ETHERSWT_VLANIDType NIF_CtrlVlanArray[] = {2U /* AVB */,
                                                 1024U /* INET */,};

static const NIF_EthIntfType NIF_EthIntfTbl[] = {
    {
        .getTxBuf = ETHER_GetTxBuffer,
        .sendTxBuf = ETHER_Send,
        .getTime = ETHER_GetTime,
        .getRxTS = ETHER_GetRxTimestamp,
        .getTxTs = ETHER_GetTxTimestamp,
        .enableTs = ETHER_EnableTxTimestamp,
        .getMode = ETHER_GetMode,
        .setTime = ETHER_SetTime,
        .getMacAddr = ETHER_GetMacAddr,
        .setMode = ETHER_SetMode,
        .setCorrectionTime = ETHER_SetCorrectionTime,
        .setGPTimer = ETHER_SetGPTimer,
    },
};

static const NIF_EthIntfType NIF_WLEthIntfTbl[] = {
#ifdef ENABLE_WLAN
    {
        .getTxBuf = WLETH_GetTxBuffer,
        .sendTxBuf = WLETH_Send,
        .getTime = WLETH_GetTime,
        .getRxTS = WLETH_GetRxTimestamp,
        .getTxTs = WLETH_GetTxTimestamp,
        .enableTs = WLETH_EnableTxTimestamp,
        .getMode = WLETH_GetMode,
        .setTime = WLETH_SetTime,
        .getMacAddr = WLETH_GetMacAddr,
        .setMode = WLETH_SetMode,
        .setCorrectionTime = WLETH_SetCorrectionTime,
        .setGPTimer = WLETH_SetGPTimer,

    },
#endif
};

static const NIF_XcvrIntfType NIF_XcvrIntfTbl[] = {
    {
        .getLinkState = ETHXCVR_GetLinkState,
        .getSQIValue = ETHXCVR_GetSQIValue,
    },
};

static const NIF_XcvrIntfType NIF_WLXcvrIntfTbl[] = {
#ifdef ENABLE_WLAN
    {
        .getLinkState = WLXCVR_GetLinkState,
        .getSQIValue = WLETH_XCVR_GetSQIValue,
    },
#endif
};

static const NIF_SwtIntfType NIF_SwtIntfTbl[] = {
#ifdef ENABLE_ETH_SWITCH
    {
        .enableTxTs = ETHERSWT_EnableTxTimestamp,
    },
#endif
};

const NIF_CtrlCfgType NIF_CtrlConfig = {
    .hwIdx = 0UL,
    .vlanArray = NIF_CtrlVlanArray,
    .vlanArraySize = sizeof(NIF_CtrlVlanArray)/sizeof(ETHERSWT_VLANIDType),
    .cbTbl = NIF_CtrlCbTbl,
    .cbTblSize = sizeof(NIF_CtrlCbTbl)/sizeof(NIF_CbTblType),
    .streamTbl = NIF_ClientStreamsTbl,
    .streamTblSize = sizeof(NIF_ClientStreamsTbl)/sizeof(NIF_ClientIDType),
    /* Ethernet driver interface function table */
    .ethIntfTbl = NIF_EthIntfTbl,
    .ethIntfTblSize = sizeof(NIF_EthIntfTbl)/sizeof(NIF_EthIntfType),
    /* Wireless ethernet driver interface function table */
    .wlEthIntfTbl = NIF_WLEthIntfTbl,
    .wlEthIntfTblSize = sizeof(NIF_WLEthIntfTbl)/sizeof(NIF_EthIntfType),
    /* XCVR interface function table */
    .xcvrIntfTbl = NIF_XcvrIntfTbl,
    .xcvrIntfTblSize = sizeof(NIF_XcvrIntfTbl)/sizeof(NIF_XcvrIntfType),
    /* Wireless XCVR interface function table */
    .wlXcvrIntfTbl = NIF_WLXcvrIntfTbl,
    .wlXcvrIntfTblSize = sizeof(NIF_WLXcvrIntfTbl)/sizeof(NIF_XcvrIntfType),
    /* Switch interface function table */
    .swtIntfTbl = NIF_SwtIntfTbl,
    .swtIntfTblSize = sizeof(NIF_SwtIntfTbl)/sizeof(NIF_SwtIntfType),
};

const NIF_CfgType NIF_Config = {
    .ctrlCfg = &NIF_CtrlConfig,
};
#endif /* if defined(ENABLE_NIF) */

#if defined(ENABLE_VTMON) && !defined(ENABLE_VTMON_TEST)
const VTMON_ConfigType VTMONConfig = {
    .tempConfig = {
        .threshLow = -30,
        .threshHigh = 115,
    },
    .voltConfig = {
        {
            .ID = VTMON_VOLT_ID_1V0,
            .threshLow = 900,
            .threshHigh = 1100
        },
        {
            .ID = VTMON_VOLT_ID_1V8,
            .threshLow = 1700,
            .threshHigh = 1900
        },
        {
            .ID = VTMON_VOLT_ID_3V3,
            .threshLow = 3200,
            .threshHigh = 3400
        },
    },
};
#endif

#ifdef ENABLE_PCA9673
static inline int32_t WaitForPCA9673Completion(uint32_t aJobID)
{
    int32_t retVal = BCM_ERR_OK;
    StatusType status;
    BCM_EventMaskType eventMask = 0UL;
    BCM_TaskType taskID;

    status = GetTaskID ((TaskRefType)&taskID);
    if ((E_OK != status) || (INVALID_TASK == taskID)) {
        retVal = BCM_ERR_UNKNOWN;
        goto exit;
    }

    do {
        retVal = BCM_WaitEvent(BOARD_INIT_EVENT_MASK);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
            break;
        }
        retVal = BCM_GetEvent(taskID, &eventMask);
        if (BCM_ERR_OK != retVal) {
            retVal = BCM_ERR_UNKNOWN;
            break;
        }
        if ((eventMask & BOARD_INIT_EVENT_MASK) == BOARD_INIT_EVENT_MASK) {
            retVal = BCM_ClearEvent(BOARD_INIT_EVENT_MASK);
            if (BCM_ERR_OK != retVal) {
                retVal = BCM_ERR_UNKNOWN;
                break;
            }
            retVal = PCA9673_GetStatus(aJobID);
            if (BCM_ERR_OK == retVal) {
                ULOG_VERB("PCA9673_GetStatus succeeded\n");
                break;
            } else {
                ULOG_ERR("PCA9673_GetStatus failed(0x%x)\n", retVal);
            }
        }
    } while(BCM_ERR_OK == retVal);

exit:
    return retVal;
}

void PCA9673Init(void)
{
    int32_t retVal;
    uint32_t i;
    uint32_t jobID = 0xFFFFFFFFUL;

    if (PCA9673ChanCfgTblSize != 0UL) {
        retVal = PCA9673_Init(BOARD_INIT_EVENT_MASK, &jobID, IIC_HW_ID_GPIO_EXP,
                                PCA9673_SLAVE_ADDR);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("Skipping PCA9673 config as init failed(retVal:0x%x)\n", retVal);
        } else {
            retVal = WaitForPCA9673Completion(jobID);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("WaitForPCA9673Completion failed(0x%x) for PCA9673_Init\n",
                        retVal);
            } else {
                for (i = 0UL; i < PCA9673ChanCfgTblSize; i++) {
                    retVal = PCA9673_OutputChannel(PCA9673ChanCfgTbl[i].chan,
                            PCA9673ChanCfgTbl[i].level, BOARD_INIT_EVENT_MASK,
                            &jobID);
                    if (BCM_ERR_OK == retVal) {
                        retVal = WaitForPCA9673Completion(jobID);
                    }
                    if (BCM_ERR_OK != retVal) {
                        ULOG_ERR("PCA9673_OutputChannel failed[chan:0x%x retVal:0x%x]\n",
                                PCA9673ChanCfgTbl[i].chan, retVal);
                        break;
                    }
                }
            }
        }
    }
}
#endif

#ifdef ENABLE_CSI_TX_PATH
#define TEST_IIC_EVENT      (BOARD_INIT_EVENT_MASK)
#define IIC_MAX9295A        (0x40U)
#define IIC_MAX96752        (0x2AU)
#define IIC_HW_ID_0 (0UL)

void ConfigMax9295A_Init(void)
{
#define MAXIM_PKT_CNT  (12UL) /* 5 for IIC_MAX9295A and 7 for IIC_MAX96752*/
#define MAXIM_DATA_SZ  (4UL)  /* SlaveAddr, RegAddrHigh, RegAddrLow, Val */
    uint8_t data[MAXIM_PKT_CNT * MAXIM_DATA_SZ] = {
        IIC_MAX9295A, 0x03, 0x30, 0x00,
        IIC_MAX9295A, 0x03, 0x31, 0x30,
        IIC_MAX9295A, 0x00, 0x5B, 0x10,
        IIC_MAX9295A, 0x00, 0x53, 0x12,
        IIC_MAX9295A, 0x03, 0x70, 0x08,

        IIC_MAX96752, 0x01, 0xCE, 0x5F,
        IIC_MAX96752, 0x01, 0x00, 0x3B,
        IIC_MAX96752, 0x01, 0x09, 0x98,
        IIC_MAX96752, 0x01, 0x0A, 0x2C,
        IIC_MAX96752, 0x01, 0x0B, 0x00,
        IIC_MAX96752, 0x01, 0x0C, 0x21,
        IIC_MAX96752, 0x01, 0x0E, 0x40,
    };

    uint32_t k;
    int32_t ret;
    IIC_PktType pkt[MAXIM_PKT_CNT];
    uint32_t job = 0xFFFFFFFFUL;

    for (k = 0UL; k < MAXIM_PKT_CNT; k++)
    {
        pkt[k].flags = 0UL;
        pkt[k].slaveAddr = data[k*MAXIM_DATA_SZ];
        pkt[k].length = MAXIM_DATA_SZ - 1UL;
        pkt[k].data = &data[k*MAXIM_DATA_SZ + 1UL];

        ret = IIC_Transfer(IIC_HW_ID_0, &pkt[k], 1UL, TEST_IIC_EVENT, &job);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s err:%d\n", __func__, ret);
        }
        ret = BCM_WaitEvent(TEST_IIC_EVENT);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s Error while waiting for event, status=%d\n", __func__, ret);
        }
        ret = BCM_ClearEvent(TEST_IIC_EVENT);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s Error while clearing event, status=%d\n", __func__, ret);
        }

        ret = IIC_GetStatus(IIC_HW_ID_0, job);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("%s IIC_GetStatus[%d] returned %d %d\n", __func__, k, ret, __LINE__);
        }
    }
}
#endif

