/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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

#include <string.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <system.h>
#include <chip_config.h>
#include <osil/lin_osil.h>
#include <circ_queue.h>
#include "uart.h"
#include "pl011.h"
#include <osil/lin_osil.h>
#include "lin.h"
#include "lin_drv.h"

typedef uint32_t LINDrv_StateType;
#define LIN_DRV_IDLE                           (0UL)
#define LIN_DRV_TX_SYNC                        (1UL)
#define LIN_DRV_TX_PID                         (2UL)
#define LIN_DRV_TX_DATA                        (3UL)
#define LIN_DRV_PID_TRANSMITTED                (4UL)
#define LIN_DRV_RECEIVE_DATA                   (5UL)
#define LIN_DRV_RECEIVE_CHECKSUM               (6UL)
#define LIN_DRV_CHECKSUM_TRANSMITTED           (7UL)
#define LIN_DRV_PREP_WAKEUP                    (8UL)
#define LIN_DRV_DETECT_WAKEUP                  (9UL)

typedef uint32_t  LINDrv_ErrReportType;
#define LIN_DRV_ERR_REPORT_DISABLED      (0UL)
#define LIN_DRV_ERR_REPORT_ENABLED       (1UL)

typedef struct {
    LIN_BaudrateType baud;
    LIN_WakeupDetectType wakeUpCfg;
    LIN_StatusType linStatus;
    LINDrv_StateType state;
    LINDrv_ErrReportType reportErr;
    LIN_ErrorType errStatus;
    uint32_t errTimeout;
    uint32_t timeoutEnabled;
    const LIN_FrmObjType * txFrmPtr;
    uint32_t txInProgress;
    uint32_t brkInProgress;
    uint32_t txWakeup;
    uint32_t brkCnt;
    uint32_t txDataSize;
    uint8_t checksum;
    uint32_t txActive;
    uint32_t txBufPutIdx;
    uint32_t txBufGetIdx;
    uint32_t rxBufIdx;
    uint8_t dataBuf[LIN_DRV_TX_BUF_SIZE];
} LINDrv_RWDevType;

static LINDrv_RWDevType COMP_SECTION(".data.drivers")
    LINDrv_RWDev[LIN_MAX_HW_ID] =
{
#if (LIN_MAX_HW_ID == 0)
    #error "LIN_MAX_HW_ID == 0"
#endif
    {.reportErr = LIN_DRV_ERR_REPORT_DISABLED,
     .errStatus = 0U,
     .errTimeout = 0UL,
     .timeoutEnabled = FALSE,
     .txFrmPtr = NULL,
     .txInProgress = FALSE,
     .brkInProgress = FALSE,
     .txWakeup = FALSE,
     .brkCnt = 0UL,
     .txActive = FALSE,
     .txBufPutIdx = 0UL,
     .txBufGetIdx = 0UL,
     .rxBufIdx = 0UL,
     .linStatus = LIN_STATUS_UNINIT},
#if (LIN_MAX_HW_ID > 1)
    {.reportErr = LIN_DRV_ERR_REPORT_DISABLED,
     .errStatus = 0U,
     .errTimeout = 0UL,
     .timeoutEnabled = FALSE,
     .txFrmPtr = NULL,
     .txInProgress = FALSE,
     .brkInProgress = FALSE,
     .txWakeup = FALSE,
     .brkCnt = 0UL,
     .txActive = FALSE,
     .txBufPutIdx = 0UL,
     .txBufGetIdx = 0UL,
     .rxBufIdx = 0UL,
     .linStatus = LIN_STATUS_UNINIT},
#endif
#if (LIN_MAX_HW_ID > 2)
    {.reportErr = LIN_DRV_ERR_REPORT_DISABLED,
     .errStatus = 0U,
     .errTimeout = 0UL,
     .timeoutEnabled = FALSE,
     .txFrmPtr = NULL,
     .txInProgress = FALSE,
     .brkInProgress = FALSE,
     .txWakeup = FALSE,
     .brkCnt = 0UL,
     .txActive = FALSE,
     .txBufPutIdx = 0UL,
     .txBufGetIdx = 0UL,
     .rxBufIdx = 0UL,
     .linStatus = LIN_STATUS_UNINIT},
#endif
#if (LIN_MAX_HW_ID > 3)
    #error "LIN_MAX_HW_ID > 3 is not supported by the driver"
#endif
};

static const uint8_t sleepData[] = { 0x00UL, 0xFFUL, 0xFFUL, 0xFFUL, 0xFFUL, 0xFFUL, 0xFFUL, 0xFFUL };
static const LIN_FrmObjType sleepFrm =
{
    .frmId = LIN_DRV_DIAG_MASTER_REQ,
    .frmDataLen = 8UL,
    .frmDataPtr = (uint8_t *)sleepData,
    .frmChecksumModel = LIN_CHECKSUM_CLASSIC,
    .frmResponse = LIN_FRM_TX,
};

static void LINDrv_ReportError(uint32_t aInstanceID, uint8_t aApiID,
        int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
        uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_LIN_ID, (uint8_t)aInstanceID, aApiID, aErr, 4UL, values);
}

COMP_INLINE void LINDrv_StartTransmit(LIN_HwIDType aHwID)
{
    LINDrv_RWDev[aHwID].brkInProgress = TRUE;
    LINDrv_RWDev[aHwID].txInProgress = TRUE;
    LINDrv_RWDev[aHwID].txActive = TRUE;
    UART_SendBreak(aHwID);
}

void LINDrv_Init(LIN_HwIDType aHwID, const LIN_ConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    UART_ConfigType uartConfig;

    if ((NULL == aConfig) || (aHwID >= LIN_MAX_HW_ID) ||
        (aConfig->baudrate > LIN_BAUD_19200) ||
        (aConfig->wakeUpCfg > LIN_WAKEUP_DETECT_ENABLED)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (LIN_STATUS_UNINIT != LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    uartConfig.baud = aConfig->baudrate;
    uartConfig.txFifoLvl = UART_TX_FIFO_LVL_1DIV8;
    uartConfig.rxFifoLvl = UART_RX_FIFO_LVL_1DIV8;
    uartConfig.stopBits = UART_STOP1;
    uartConfig.parity = UART_PARITY_NONE;
    uartConfig.errReport = UART_ERR_REPORT_ENABLED;
    uartConfig.loopBackMode = (uint32_t)aConfig->loopbackMode;

    LINDrv_RWDev[aHwID].wakeUpCfg = aConfig->wakeUpCfg;

    UART_Init((UART_HwIDType)aHwID, (const UART_ConfigType * const)&uartConfig);

    LINDrv_RWDev[aHwID].baud = aConfig->baudrate;
    LINDrv_RWDev[aHwID].linStatus = LIN_NORMAL_OPERATION;

err:
    if (BCM_ERR_OK != retVal) {
        LINDrv_ReportError(aHwID, LIN_DRV_INIT_API_ID, retVal, 0UL, 0UL, 0UL,
                0UL);
    }
    return;
}

static uint8_t LINDrv_CalcChecksum(const uint8_t *const aBuffer,
                                   uint32_t aSize,
                                   uint8_t aPID,
                                   LIN_FrmChecksumType aChksumModel){
    uint16_t chkSum = 0;
    uint8_t chkSumFinal;
    uint32_t i;

    for(i = 0UL; i < aSize; i++) {
        chkSum = chkSum + (uint16_t)aBuffer[i];
        if (chkSum >= 256U) {
            chkSum = chkSum - 255U;
        }
    }

    if(aChksumModel == LIN_CHECKSUM_ENHANCED){
        chkSum = chkSum + (uint16_t)aPID;
        if (chkSum >= 256U) {
            chkSum = chkSum - 255U;
        }
    }

    chkSumFinal = (uint8_t)chkSum;

    return ~chkSumFinal;
}

static void LINDrv_AbortTransfer(LIN_HwIDType aHwID)
{
    if ((LIN_DRV_RECEIVE_DATA == LINDrv_RWDev[aHwID].state) ||
            (LIN_DRV_RECEIVE_CHECKSUM == LINDrv_RWDev[aHwID].state)) {
        if ((TRUE == LINDrv_RWDev[aHwID].errTimeout) && (LINDrv_RWDev[aHwID].rxBufIdx == 0UL)) {
            LINDrv_RWDev[aHwID].linStatus = LIN_RX_NO_RESPONSE;
        } else {
            LINDrv_RWDev[aHwID].linStatus = LIN_RX_ERROR;
        }
    } else {
        if (LINDrv_RWDev[aHwID].txBufGetIdx <= LIN_DRV_MID_FIELD) {
            LINDrv_RWDev[aHwID].linStatus = LIN_TX_HEADER_ERR;
        } else {
            LINDrv_RWDev[aHwID].linStatus = LIN_TX_RESPONSE_ERR;
        }
    }

    if ((LIN_STATUS_SLEEP == LINDrv_RWDev[aHwID].linStatus) &&
        (LIN_WAKEUP_DETECT_ENABLED == LINDrv_RWDev[aHwID].wakeUpCfg)) {
        LINDrv_RWDev[aHwID].state = LIN_DRV_PREP_WAKEUP;
    } else {
        LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
    }

    LINDrv_RWDev[aHwID].txInProgress = FALSE;
    LINDrv_RWDev[aHwID].txActive = FALSE;
    LINDrv_RWDev[aHwID].txBufPutIdx = 0UL;
    LINDrv_RWDev[aHwID].txBufGetIdx = 0UL;
    LINDrv_RWDev[aHwID].rxBufIdx = 0UL;
    LINDrv_RWDev[aHwID].errTimeout = FALSE;
    LINDrv_RWDev[aHwID].timeoutEnabled = FALSE;
    LINDrv_RWDev[aHwID].brkInProgress = FALSE;
    LINDrv_RWDev[aHwID].brkCnt = 0UL;
}

void LINDrv_Deinit(LIN_HwIDType aHwID)
{
    int32_t retVal = BCM_ERR_OK;

    if (aHwID < LIN_MAX_HW_ID) {
        /* Deinitialize LIN/UART */
        retVal = UART_DeInit(aHwID);
        if (BCM_ERR_OK != retVal) {
            goto err;
        }

        BCM_MemSet(&(LINDrv_RWDev[aHwID]), 0x0, sizeof(LINDrv_RWDevType));
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

err:
    if (BCM_ERR_OK != retVal) {
        LINDrv_ReportError(aHwID, LIN_DRV_INIT_API_ID, retVal, 0UL, 0UL, 0UL,
                0UL);
    }
    return;
}

int32_t LINDrv_Sleep(LIN_HwIDType aHwID)
{
    /* Send Sleep command on LIN bus */
    int32_t retVal = BCM_ERR_OK;

    if (aHwID >= LIN_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (LIN_STATUS_UNINIT == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (LIN_STATUS_SLEEP == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (LIN_NORMAL_OPERATION != LINDrv_RWDev[aHwID].linStatus) {
        LINDrv_AbortTransfer(aHwID);
    }

    retVal = LINDrv_TransmitFrame(aHwID, (const LIN_FrmObjType * const)&sleepFrm);

    LINDrv_RWDev[aHwID].linStatus = LIN_STATUS_SLEEP;

err:
    return retVal;
}

int32_t LINDrv_WakeUp(LIN_HwIDType aHwID)
{
    int32_t retVal = BCM_ERR_OK;

    /* Send Wakeup on LIN bus */
    if (aHwID >= LIN_MAX_HW_ID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (LIN_STATUS_UNINIT == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (LIN_STATUS_SLEEP != LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* Wakeup pulse is between 250us-5ms long, transmit the Break field */
    LINDrv_RWDev[aHwID].brkInProgress = TRUE;
    LINDrv_RWDev[aHwID].txWakeup = TRUE;
    UART_SendBreak(aHwID);

err:
    return retVal;
}

int32_t LINDrv_TransmitFrame(LIN_HwIDType aHwID, const LIN_FrmObjType * const aFrmObj)
{
    int32_t retVal = BCM_ERR_OK;

    if ((NULL == aFrmObj) || (aHwID >= LIN_MAX_HW_ID)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (LIN_STATUS_UNINIT == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (LIN_STATUS_SLEEP == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (LIN_NORMAL_OPERATION != LINDrv_RWDev[aHwID].linStatus) {
        LINDrv_AbortTransfer(aHwID);
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    LINDrv_RWDev[aHwID].txFrmPtr = aFrmObj;

    if (LIN_FRM_TX == aFrmObj->frmResponse) {
        LINDrv_RWDev[aHwID].linStatus = LIN_TX_BUSY;
        LINDrv_RWDev[aHwID].checksum = LINDrv_CalcChecksum((const uint8_t *const)LINDrv_RWDev[aHwID].txFrmPtr->frmDataPtr,
                                                    LINDrv_RWDev[aHwID].txFrmPtr->frmDataLen,
                                                    LINDrv_RWDev[aHwID].txFrmPtr->frmId,
                                                    LINDrv_RWDev[aHwID].txFrmPtr->frmChecksumModel);
    } else if (LIN_FRM_MON == aFrmObj->frmResponse) {
        LINDrv_RWDev[aHwID].linStatus = LIN_TX_BUSY;
    } else {
        LINDrv_RWDev[aHwID].linStatus = LIN_RX_BUSY;
    }

    LINDrv_RWDev[aHwID].txBufPutIdx = 0UL;
    LINDrv_RWDev[aHwID].txBufGetIdx = 0UL;
    LINDrv_RWDev[aHwID].rxBufIdx = 0UL;

    LINDrv_RWDev[aHwID].state = LIN_DRV_TX_SYNC;
    LINDrv_StartTransmit(aHwID);

err:
    return retVal;
}

uint32_t LINDrv_TxCb(LIN_HwIDType aHwID, char * const aData, uint32_t aSize)
{
    uint32_t nBytes = 0UL;
    uint32_t i;

    for (i = 0UL; i < aSize; i++) {
        if (TRUE == LINDrv_RWDev[aHwID].txActive) {
            switch (LINDrv_RWDev[aHwID].state)
            {
            case LIN_DRV_TX_SYNC:
                LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufPutIdx] = LIN_DRV_SYNC_DATA;
                LINDrv_RWDev[aHwID].state = LIN_DRV_TX_PID;
                break;

            case LIN_DRV_TX_PID:
                LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufPutIdx] = LINDrv_RWDev[aHwID].txFrmPtr->frmId;

                if (LIN_FRM_TX != LINDrv_RWDev[aHwID].txFrmPtr->frmResponse) {
                    LINDrv_RWDev[aHwID].txActive = FALSE;
                } else {
                    LINDrv_RWDev[aHwID].state = LIN_DRV_TX_DATA;
                }
                break;

            case LIN_DRV_TX_DATA:
                if (LINDrv_RWDev[aHwID].txBufPutIdx < (LINDrv_RWDev[aHwID].txFrmPtr->frmDataLen
                                                       + LIN_DRV_DATA_FIELD)) {
                    LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufPutIdx] =
                         LINDrv_RWDev[aHwID].txFrmPtr->frmDataPtr[LINDrv_RWDev[aHwID].txBufPutIdx - LIN_DRV_DATA_FIELD];
                } else {
                    LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufPutIdx] = LINDrv_RWDev[aHwID].checksum;

                    if ((LIN_STATUS_SLEEP == LINDrv_RWDev[aHwID].linStatus) &&
                       (LIN_WAKEUP_DETECT_ENABLED == LINDrv_RWDev[aHwID].wakeUpCfg)) {
                        LINDrv_RWDev[aHwID].state = LIN_DRV_PREP_WAKEUP;
                    } else {
                    LINDrv_RWDev[aHwID].state = LIN_DRV_CHECKSUM_TRANSMITTED;
                    }

                    LINDrv_RWDev[aHwID].txActive = FALSE;
                }
                break;

            default:
                LINDrv_RWDev[aHwID].txActive = FALSE;
                break;
            }

            aData[i] = (char)LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufPutIdx];
            LINDrv_RWDev[aHwID].txBufPutIdx++;
            nBytes = i + 1UL;
        }
    }

    return nBytes;
}

void LINDrv_ErrCb(LIN_HwIDType aHwID, LIN_ErrorType aErr, char aData)
{
    if ((0UL != (aErr & LIN_TIMEOUT_ERR_MASK)) && (TRUE == LINDrv_RWDev[aHwID].timeoutEnabled)) {
        /* Store Timeout error */
        LINDrv_RWDev[aHwID].errTimeout = TRUE;
    } else {
        if (TRUE == LINDrv_RWDev[aHwID].brkInProgress) {
            UART_ClrBreak(aHwID);
            LINDrv_RWDev[aHwID].brkInProgress = FALSE;

            if (FALSE == LINDrv_RWDev[aHwID].txWakeup) {
                /* Start of transmission */
                UART_ProcessTx(aHwID);
            } else {
                LINDrv_RWDev[aHwID].txWakeup = FALSE;
            }
        } else {
            if (LIN_DRV_ERR_REPORT_ENABLED == LINDrv_RWDev[aHwID].reportErr) {
                /* Store error */
                LINDrv_RWDev[aHwID].errStatus = (LIN_ErrorType)aErr;
                LINDrv_RxCb(aHwID, (char * const)&aData, 1UL);
            }
        }
    }
}

void LINDrv_RxCb(LIN_HwIDType aHwID, char * const aData, uint32_t aSize)
{
    uint32_t i;

    if (TRUE == LINDrv_RWDev[aHwID].txInProgress) {
        /* Check Rx data from Tx data and Unpop from Tx Queue */
        for (i = 0UL; i < aSize; i++) {
            if ((((uint8_t)aData[i] != LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].txBufGetIdx])) ||
                 ((0U != LINDrv_RWDev[aHwID].errStatus) && (LIN_DRV_DETECT_WAKEUP != LINDrv_RWDev[aHwID].state)) ||
                 (TRUE == LINDrv_RWDev[aHwID].errTimeout)) {
                LINDrv_AbortTransfer(aHwID);
                LINDrv_RWDev[aHwID].errStatus = 0U;
                LINDrv_RWDev[aHwID].reportErr = LIN_DRV_ERR_REPORT_DISABLED;
                break;
            }
            LINDrv_RWDev[aHwID].txBufGetIdx++;
        }
    }

    switch (LINDrv_RWDev[aHwID].state)
    {
    case LIN_DRV_TX_SYNC:
        /* Do nothing, Tx callback will handle processing */
        break;

    case LIN_DRV_TX_PID:
        if ((LIN_FRM_TX != LINDrv_RWDev[aHwID].txFrmPtr->frmResponse) &&
            (LIN_DRV_MID_FIELD == LINDrv_RWDev[aHwID].txBufGetIdx)) {
            LINDrv_RWDev[aHwID].state = LIN_DRV_PID_TRANSMITTED;
        }
        break;

    case LIN_DRV_TX_DATA:
        /* Do nothing, Tx callback will handle processing */
        break;

    case LIN_DRV_PID_TRANSMITTED:
        LINDrv_RWDev[aHwID].txInProgress = FALSE;
        if (LIN_FRM_RX == LINDrv_RWDev[aHwID].txFrmPtr->frmResponse) {
            LINDrv_RWDev[aHwID].timeoutEnabled = TRUE;
            LINDrv_RWDev[aHwID].state = LIN_DRV_RECEIVE_DATA;
        } else {
            LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
            LINDrv_RWDev[aHwID].linStatus = LIN_TX_OK;
        }
        break;

    case LIN_DRV_RECEIVE_DATA:
        for (i = 0UL; i < aSize; i++) {
            LINDrv_RWDev[aHwID].dataBuf[LINDrv_RWDev[aHwID].rxBufIdx] = (uint8_t)aData[i];
            LINDrv_RWDev[aHwID].rxBufIdx++;
            if (LINDrv_RWDev[aHwID].rxBufIdx == LINDrv_RWDev[aHwID].txFrmPtr->frmDataLen) {
                LINDrv_RWDev[aHwID].timeoutEnabled = FALSE;
                LINDrv_RWDev[aHwID].state = LIN_DRV_RECEIVE_CHECKSUM;
                break;
            }
        }
        break;

    case LIN_DRV_RECEIVE_CHECKSUM:
        LINDrv_RWDev[aHwID].checksum = LINDrv_CalcChecksum((const uint8_t *const)&LINDrv_RWDev[aHwID].dataBuf,
                                                      LINDrv_RWDev[aHwID].txFrmPtr->frmDataLen,
                                                      LINDrv_RWDev[aHwID].txFrmPtr->frmId,
                                                      LINDrv_RWDev[aHwID].txFrmPtr->frmChecksumModel);
        if ((uint8_t)*aData != LINDrv_RWDev[aHwID].checksum) {
            LINDrv_AbortTransfer(aHwID);
        } else {
            LINDrv_RWDev[aHwID].linStatus = LIN_RX_OK;
            LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
        }
        break;

    case LIN_DRV_CHECKSUM_TRANSMITTED:
        if (LINDrv_RWDev[aHwID].txBufPutIdx == LINDrv_RWDev[aHwID].txBufGetIdx) {
            LINDrv_RWDev[aHwID].txInProgress = FALSE;
            LINDrv_RWDev[aHwID].linStatus = LIN_TX_OK;
            LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
            LINDrv_RWDev[aHwID].timeoutEnabled = FALSE;
        }
        break;

    case LIN_DRV_IDLE:
        break;

    case LIN_DRV_PREP_WAKEUP:
        if (LINDrv_RWDev[aHwID].txBufPutIdx == LINDrv_RWDev[aHwID].txBufGetIdx) {
            LINDrv_RWDev[aHwID].state = LIN_DRV_DETECT_WAKEUP;
            LINDrv_RWDev[aHwID].reportErr = LIN_DRV_ERR_REPORT_ENABLED;
        }
        break;

    case LIN_DRV_DETECT_WAKEUP:
        if (LIN_DRV_WAKEUP_DATA == (uint8_t)aData[0]) {
            LINDrv_RWDev[aHwID].errStatus = 0U;    /* Reset error status */
            LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
            LINDrv_RWDev[aHwID].linStatus = LIN_NORMAL_OPERATION;
            LINDrv_RWDev[aHwID].reportErr = LIN_DRV_ERR_REPORT_DISABLED;
            LIN_WakeUpNotification(aHwID);      /* Invoke WakeUp Callback */
        }
        break;

    default:
        LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
        break;
    }
}

LIN_StatusType LINDrv_ReadStatus(LIN_HwIDType aHwID)
{
    LIN_StatusType ret = LIN_ERR_UNKNOWN;

    if (aHwID >= LIN_MAX_HW_ID) {
        ret = LIN_ERR_UNKNOWN;
        goto err;
    }

    if (LIN_STATUS_UNINIT == LINDrv_RWDev[aHwID].linStatus) {
        ret = LIN_STATUS_UNINIT;
        goto err;
    }

    ret = LINDrv_RWDev[aHwID].linStatus;

    if (LINDrv_RWDev[aHwID].linStatus >= LIN_TX_OK)
    {
        LINDrv_RWDev[aHwID].linStatus = LIN_NORMAL_OPERATION;
    }

err:
    return ret;
}

int32_t LINDrv_GetData(LIN_HwIDType aHwID, uint8_t *const aDataPtr)
{
    int32_t retVal = BCM_ERR_OK;

    if ((aHwID >= LIN_MAX_HW_ID) || (NULL == aDataPtr)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (LIN_STATUS_UNINIT == LINDrv_RWDev[aHwID].linStatus) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (LINDrv_RWDev[aHwID].linStatus != LIN_RX_OK) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    } else {
        LINDrv_RWDev[aHwID].linStatus = LIN_NORMAL_OPERATION;
    }

    BCM_MemCpy(aDataPtr, LINDrv_RWDev[aHwID].dataBuf, LINDrv_RWDev[aHwID].txFrmPtr->frmDataLen);
    LINDrv_RWDev[aHwID].state = LIN_DRV_IDLE;
err:
    return retVal;
}
/* Nothing past this line */
