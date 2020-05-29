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

#include "ee.h"
#include <can.h>
#include <osil/can_osil.h>
#include <bcm_err.h>
#include <ulog.h>
#include <string.h>
#include <bcm_test.h>
#include <bcm_utils.h>
#include <shell.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel()     ULOG_LVL_ERROR
#define CAN_IT1                 (1UL)

static volatile int32_t CANIT1_Result = BCM_AT_NOT_STARTED;

/* Macros */
#define MCAN_HW_ID              (0UL)
#define CAN_TX_TEST_LOOP        (20U)
#define CAN_RX_MAX_LEN          (100UL)

#define CAN_RX_IND_EVENT        (CanRxEvent)
#define CAN_TX_CONFIRM_EVENT    (CanTxConfInd)

/* Global data */
static uint8_t canRxData[CAN_RX_MAX_LEN];
static uint32_t canRxDataLen;
static CAN_TxMsgObjType canTxObj;
static uint8_t canTxData_tc2[8] = { 0xDE, 0xAD, 0xBE, 0xEF,
                             0xFE, 0xED, 0xBE, 0xEB };

static uint8_t canTxData_tc3[32] = { 0x81, 0x81, 0x81, 0x81,
                              0x91, 0x91, 0x91, 0x91,
                              0xA1, 0xA2, 0xA3, 0xA4,
                              0xB1, 0xB2, 0xB3, 0xB4,
                              0xC1, 0xC2, 0xC3, 0xC4,
                              0xD1, 0xD2, 0xD3, 0xD4,
                              0xE1, 0xE2, 0xE3, 0xE4,
                              0xF1, 0xF2, 0xF3, 0xF4 };

static uint8_t canTxData_tc4[8] = { 0x12, 0x12, 0x12, 0x12,
                             0x23, 0x23, 0x23, 0x23 };

#define MsgRAMStdFltrCfg {              \
    .startAddr = 0x0000UL,              \
    .numElements = 0x40UL,              \
}

#define MsgRAMExtFltrCfg {              \
    .startAddr = 0x0040UL,              \
    .numElements = 0x18UL,              \
}

#define MsgRAMFifo0Cfg {                \
    .startAddr = 0x0070UL,              \
    .numElements = 0x18UL,              \
    .elementSz = CAN_MSGRAMDATASZ_64,   \
    .mode = CAN_RXFIFOOPMODE_BLOCKING,  \
    .waterMark = 0x17UL,                \
}

#define MsgRAMFifo1Cfg {                \
    .startAddr = 0x0220UL,              \
    .numElements = 0x18UL,              \
    .elementSz = CAN_MSGRAMDATASZ_64,   \
    .mode = CAN_RXFIFOOPMODE_BLOCKING,  \
    .waterMark = 0x17UL,                \
}

#define MsgRAMRxBufCfg {                \
    .startAddr = 0x03D0UL,              \
    .elementSz = CAN_MSGRAMDATASZ_64,   \
}

#define MsgRAMTxEventCfg {              \
    .startAddr = 0x0580UL,              \
    .numElements = 0x20UL,              \
    .waterMark = 0x00UL,                \
}

#define MsgRAMTxBufCfg {                \
    .startAddr = 0x05C0UL,              \
    .elementSz = CAN_MSGRAMDATASZ_64,   \
    .mode = CAN_TXBUFMODE_FIFO,         \
    .fifoQueueSize = 16,                \
    .numDedicatedBufs = 16,             \
}

#define CanMsgRAMConfig {               \
    .stdFltrCfg = MsgRAMStdFltrCfg,     \
    .extFltrCfg = MsgRAMExtFltrCfg,     \
    .fifo0Cfg = MsgRAMFifo0Cfg,         \
    .fifo1Cfg = MsgRAMFifo1Cfg,         \
    .rxBufCfg = MsgRAMRxBufCfg,         \
    .txEventCfg = MsgRAMTxEventCfg,     \
    .txBufCfg = MsgRAMTxBufCfg,         \
}

#define CanBaudRateConfig {             \
    .propSeg = 2U,                      \
    .seg1 = 2U,                         \
    .seg2 = 1U,                         \
    .syncJumpWidth = 2U,                \
    .prescale = 11U,                    \
}

#define CanFDBaudRateConfig {           \
    .propSeg = 2U,                      \
    .seg1 = 2U,                         \
    .seg2 = 1U,                         \
    .syncJumpWidth = 2U,                \
    .delayCompOffset = 10U,             \
    .delayCompFilter = 0U,              \
    .prescale = 11U,                    \
}

#define CanFDCfg {                      \
    .enable = TRUE,                     \
    .bitRateSwitchEn = TRUE,            \
    .delayCompEn = FALSE,               \
    .rateCfg = CanFDBaudRateConfig,     \
}

/* Configuration of CAN Message Objects and filter elements is done here */
static const CAN_RxFilterObjType CanFilterObjects[] =
{
    {
        .msgIDType = CAN_MSGID_STANDARD,
        .bufCfg = CAN_RXFILTERBUFCFG_FIFO_0,
        .filterType = CAN_MSGFILTER_CLASSIC,
        .filterID1 = 2,
        .filterID2 = 0,
    },
    {
        .msgIDType = CAN_MSGID_EXTENDED,
        .bufCfg = CAN_RXFILTERBUFCFG_FIFO_0,
        .filterType = CAN_MSGFILTER_CLASSIC,
        .filterID1 = 2,
        .filterID2 = 0,
    },
};

static const CAN_FilterCfgType CanFilterCfg =
{
    .filterObjPtr = CanFilterObjects,
    .filterObjSz = (uint32_t)((sizeof(CanFilterObjects) / sizeof(CAN_RxFilterObjType))),
    .rxNoMatchStd = CAN_RXFLTRNOMATCHCFG_REJECT,
    .rxNoMatchExt = CAN_RXFLTRNOMATCHCFG_REJECT,
};

/* Configuration of CAN Config Structure
 * Note: Loopback mode ENABLED by default, set to CAN_LOOPBACK_DISABLED for actual bus communication */
static const CAN_ConfigType CANConfigData =
{
    .baudRateCfg = CanBaudRateConfig,
    .fdCfg = CanFDCfg,
    .filterCfgPtr = &CanFilterCfg,
    .loopbackModeEn = TRUE,
    .autoBusOffRecoveryEn = TRUE,
    .msgRAMCfg = CanMsgRAMConfig,
    .pxhdEn = FALSE,
    .rmtFrmEn = FALSE,
};

static uint32_t ConvertCanDLC(uint32_t aCanDLC)
{
    uint32_t msgLength;

    switch (aCanDLC) {
    case 9:
        msgLength = 12UL;
        break;
    case 10:
        msgLength = 16UL;
        break;
    case 11:
        msgLength = 20UL;
        break;
    case 12:
        msgLength = 24UL;
        break;
    case 13:
        msgLength = 32UL;
        break;
    case 14:
        msgLength = 48UL;
        break;
    case 15:
        msgLength = 64UL;
        break;
    default:
        msgLength = aCanDLC;
        break;
    }
    return msgLength;
}

static int32_t CanVerifyRxData(CAN_TxMsgObjType *aObj, uint8_t *aRxData,
                                uint32_t aLen)
{
    int32_t ret;
    uint32_t TxMsgLength = ConvertCanDLC(aObj->msgInfo.length);

    /* compare rx data */
    ret = memcmp(aObj->msgInfo.dataPtr, aRxData, TxMsgLength);

    if (0 != ret) {
        ULOG_ERR("RX Data mismatch");
    }
    return ret;
}

static int32_t CanWriteMsg(CAN_HwIDType aID, CAN_TxMsgObjType *aObj)
{
    int32_t ret;
    BCM_EventMaskType mask;

    ret = CAN_Write(aID, aObj);
    if (BCM_ERR_OK == ret) {
        (void)BCM_WaitEvent(CAN_TX_CONFIRM_EVENT);
        (void)BCM_GetEvent(CANIT1_Task, &mask);
        if (0UL != (mask & CAN_TX_CONFIRM_EVENT)) {
            (void)BCM_ClearEvent(CAN_TX_CONFIRM_EVENT);
            if (TRUE == CANConfigData.loopbackModeEn) {
                (void)BCM_WaitEvent(CAN_RX_IND_EVENT);
                (void)BCM_GetEvent(CANIT1_Task, &mask);
                if (0UL != (mask & CAN_RX_IND_EVENT)) {
                    (void)BCM_ClearEvent(CAN_RX_IND_EVENT);
                }
                /* verify data */
                ret = CanVerifyRxData(aObj, canRxData, canRxDataLen);
            }
        }
    } else {
        ULOG_ERR("CAN_Write failed!!\n");
    }
    return ret;
}

static int32_t CanWriteStdMsg(void)
{
    int32_t ret;
    uint8_t *dataBufPtr;

    canTxObj.msgFormat = CAN_MSGFORMAT_STANDARD;
    canTxObj.bitRateSwitchEn = FALSE;
    canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
    canTxObj.msgInfo.msgID = 0x01UL;
    canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
    canTxObj.msgInfo.length = 8UL;

    dataBufPtr = &canTxData_tc2[0];
    canTxObj.msgInfo.dataPtr = dataBufPtr;

    ret = CanWriteMsg(0UL, &canTxObj);
    return ret;
}

/* CAN FD Write with Extended Data */
static int32_t CanWriteCANFDExtData(void)
{
    int32_t ret;
    uint8_t *dataBufPtr;

    canTxObj.msgFormat = CAN_MSGFORMAT_CANFD;
    canTxObj.bitRateSwitchEn = FALSE;
    canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
    canTxObj.msgInfo.msgID = 0x02UL;
    canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
    canTxObj.msgInfo.length = 13UL;

    /* Values 9-15= CAN FD: transmit frame has 12/16/20/24/32/48/64 data bytes */
    canTxObj.msgInfo.length = 13UL;
    dataBufPtr = &canTxData_tc3[0];
    canTxObj.msgInfo.dataPtr = dataBufPtr;

    ret = CanWriteMsg(0UL, &canTxObj);
    return ret;
}

/* CAN Write with Remote Frame */
static int32_t CanWriteRemoteFrm(void)
{
    int32_t ret;

    canTxObj.msgFormat = CAN_MSGFORMAT_CANFD;
    canTxObj.bitRateSwitchEn = FALSE;
    canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
    canTxObj.msgInfo.msgID = 0x05UL;
    canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
    canTxObj.remoteTxRequestEn = TRUE;
    canTxObj.msgInfo.length = 0UL;

    ret = CanWriteMsg(0UL, &canTxObj);
    return ret;
}

/* CAN Write with Extended Frame */
static int32_t CanWriteExtendedFrm(void)
{
    int32_t ret;
    uint8_t *dataBufPtr;

    canTxObj.msgFormat = CAN_MSGFORMAT_STANDARD;
    canTxObj.bitRateSwitchEn = FALSE;
    canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
    canTxObj.msgInfo.msgID = 0x000F2800UL;
    canTxObj.msgInfo.msgIDType = CAN_MSGID_EXTENDED;
    canTxObj.msgInfo.length = 8UL;

    dataBufPtr = &canTxData_tc2[0];
    canTxObj.msgInfo.dataPtr = dataBufPtr;

    ret = CanWriteMsg(0UL, &canTxObj);
    return ret;
}

/* CAN Transmit loop */
static int32_t CanWriteTransmitMixedLoop(void)
{
    int32_t ret = BCM_ERR_OK;
    static uint8_t i;
    uint8_t * dataBufPtr;

    for (i = 0U; i < (CAN_TX_TEST_LOOP >> 1U); i++) {
        canTxObj.msgFormat = CAN_MSGFORMAT_STANDARD;
        canTxObj.bitRateSwitchEn = FALSE;
        canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
        canTxObj.msgInfo.msgID = 0x01UL;
        canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
        canTxObj.msgInfo.length = 8UL;
        dataBufPtr = &canTxData_tc4[0];
        canTxObj.msgInfo.dataPtr = dataBufPtr;

        ret = CanWriteMsg(0UL, &canTxObj);
        if (BCM_ERR_OK != ret) {
            goto err;
        }
    }

    for (i = 0U; i < (CAN_TX_TEST_LOOP >> 1U); i++) {
        canTxObj.msgFormat = CAN_MSGFORMAT_STANDARD;
        canTxObj.bitRateSwitchEn = FALSE;
        canTxObj.txBufCfg = CAN_TXBUFCFG_FIFO_QUEUE;    /* Use Tx FIFO/Queue */
        canTxObj.msgInfo.msgID = 0x02UL;
        canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
        canTxObj.msgInfo.length = 8UL;

        dataBufPtr = &canTxData_tc4[0];
        canTxObj.msgInfo.dataPtr = dataBufPtr;

        ret = CanWriteMsg(0UL, &canTxObj);
        if (BCM_ERR_OK != ret) {
            goto err;
        }
    }

err:
    return ret;
}

/* CAN Transmit loop */
static int32_t CanWriteTransmitBufferLoop(void)
{
    int32_t ret = BCM_ERR_OK;
    static uint8_t i;
    uint8_t *dataBufPtr;

    for (i = 0U; i < CAN_TX_TEST_LOOP; i++) {
        canTxObj.msgFormat = CAN_MSGFORMAT_STANDARD;
        canTxObj.bitRateSwitchEn = FALSE;
        canTxObj.txBufCfg = CAN_TXBUFCFG_BUFFER;
        canTxObj.msgInfo.msgID = 0x01UL;
        canTxObj.msgInfo.msgIDType = CAN_MSGID_STANDARD;
        canTxObj.msgInfo.length = 8UL;

        dataBufPtr = &canTxData_tc4[0];
        canTxObj.msgInfo.dataPtr = dataBufPtr;

        ret = CanWriteMsg(0UL, &canTxObj);
        if (BCM_ERR_OK != ret) {
            goto err;
        }
    }

err:
    return ret;
}

TASK(CANIT1_Task)
{
    int32_t err = BCM_ERR_OK;

    CAN_Init(MCAN_HW_ID, &CANConfigData);

    err = CanWriteStdMsg();
    if (BCM_ERR_OK != err) {
        ULOG_ERR("\nCAN error writing Standard Message");
        CANIT1_Result = BCM_ERR_UNKNOWN;
        goto err_exit;
    }

    err = CanWriteCANFDExtData();
    if (BCM_ERR_OK != err) {
        ULOG_ERR("\nCAN error writing FD Message");
        CANIT1_Result = BCM_ERR_UNKNOWN;
        goto err_exit;
    }

    if (TRUE == CANConfigData.loopbackModeEn) {
        /* Transmit RTR Frame */
        err = CanWriteRemoteFrm();
        if (BCM_ERR_OK != err) {
            ULOG_ERR("\nCAN error writing Remote Frame");
            CANIT1_Result = BCM_ERR_UNKNOWN;
            goto err_exit;
        }

        /* Transmit Extended Identifier */
        err = CanWriteExtendedFrm();
        if (BCM_ERR_OK != err) {
            ULOG_ERR("\nCAN error writing Extended Frame");
            CANIT1_Result = BCM_ERR_UNKNOWN;
            goto err_exit;
        }
    }

    /* Transmit loop using Buffers */
    err = CanWriteTransmitBufferLoop();
    if (BCM_ERR_OK != err) {
        ULOG_ERR("\nCAN error using Dedicated Tx Buffers");
        CANIT1_Result = BCM_ERR_UNKNOWN;
        goto err_exit;
    }

    err = CanWriteTransmitMixedLoop();
    if (BCM_ERR_OK != err) {
        ULOG_ERR("\nCAN error using Mixed Transmission Modes");
        CANIT1_Result = BCM_ERR_UNKNOWN;
        goto err_exit;
    }

    if (BCM_AT_EXECUTING == CANIT1_Result) {
        CANIT1_Result = BCM_ERR_OK;
    }

err_exit:
    CAN_DeInit(MCAN_HW_ID);

    (void)BCM_TerminateTask();
}

int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;
    switch (aIndex) {
    case CAN_IT1:
        CANIT1_Result = BCM_AT_EXECUTING;
        (void)BCM_ActivateTask(CANIT1_Task);
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

void ApplicationInit(void)
{
    /* Dummy Init */
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal;
    switch (aIndex) {
    case CAN_IT1:
        retVal = CANIT1_Result;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

/* Rx Callback function */
void CAN_RxInd(CAN_HwIDType aID, const CAN_RxMsgObjType * const aMsgInfo)
{
    /* Parse MsgInfo for Received data */
    uint32_t rxDLC;
    uint32_t i;
    uint32_t rxMsgLength;

    if (NULL != aMsgInfo) {
        rxDLC = aMsgInfo->msgInfo.length;
        rxMsgLength = ConvertCanDLC(rxDLC);

        if (CAN_RX_MAX_LEN > rxMsgLength) {
            for (i = 0UL; i < rxMsgLength; i++) {
                ULOG_VERB(" %04x", aMsgInfo->msgInfo.dataPtr[i]);
                canRxData[i] = (uint8_t)aMsgInfo->msgInfo.dataPtr[i];
            }
            canRxDataLen = rxMsgLength;
        } else {
            ULOG_ERR("CAN RX: invalid RxMsgLength :%d\n", rxMsgLength);
        }

        (void)BCM_SetEvent(CANIT1_Task, CAN_RX_IND_EVENT);
    } else {
        ULOG_ERR("CAN RX: aMsgInfo is NULL!!\n");
    }
}

void CAN_TxDoneInd(CAN_HwIDType aID, const CAN_TxMsgObjType * const aXmtdObj)
{
    /* Callback function for Transmit Complete */
    if (TRUE == CANConfigData.loopbackModeEn) {
        uint32_t xmtdId;     /* ID of Transmitted Message */

        xmtdId = aXmtdObj->msgInfo.msgID;

        if (xmtdId == canTxObj.msgInfo.msgID) {
        }
    }

    (void)BCM_SetEvent(CANIT1_Task, CAN_TX_CONFIRM_EVENT);
}

void CAN_BusOffInd(CAN_HwIDType aID)
{
    /* Empty stub Callback function */
    (void)BCM_SetEvent(CANIT1_Task, CAN_TX_CONFIRM_EVENT);
}

void CAN_ErrorInd(CAN_HwIDType aID, CAN_ErrorType errorCode)
{
    /* Empty stub Callback function */
}

