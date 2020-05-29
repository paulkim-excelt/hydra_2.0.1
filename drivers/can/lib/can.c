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

/**
    @defgroup grp_candrv_impl Implementation
    @ingroup grp_candrv

    @addtogroup grp_candrv_impl
    @{

    @file can.c
    @brief CAN driver interface implementation
    @version 0.1 Draft
*/

#include <stddef.h>
#include <inttypes.h>
#include <compiler.h>
#include <interrupt.h>
#include <bcm_err.h>
#include <system.h>
#include <can_osil.h>
#include <bcm_utils.h>

/**
  @name CAN Driver design IDs
  @{
  @brief Design IDs for CAN Driver
*/
#define BRCM_SWDSGN_CAN_REPORTERROR_PROC         (0x80U) /**< @brief #CAN_ReportError       */
#define BRCM_SWDSGN_CAN_GETRXMSG_PROC            (0x81U) /**< @brief #CAN_GetRxMsg          */
#define BRCM_SWDSGN_CAN_PUTRXMSG_PROC            (0x82U) /**< @brief #CAN_PutRxPkt          */
#define BRCM_SWDSGN_CAN_PROCESSRCVDPKTS_PROC     (0x83U) /**< @brief #CAN_ProcessRcvdPkts   */
#define BRCM_SWDSGN_CAN_PROCESSXMTDPKTS_PROC     (0x84U) /**< @brief #CAN_ProcessXmtdPkts   */
#define BRCM_SWDSGN_CAN_PROCESSERRPKTS_PROC      (0x85U) /**< @brief #CAN_ProcessErrPkts    */
#define BRCM_SWDSGN_CAN_PROCESSBUSOFF_PROC       (0x86U) /**< @brief #CAN_ProcessBusOff     */
#define BRCM_SWDSGN_CAN_INIT_PROC                (0x87U) /**< @brief #CAN_Init              */
#define BRCM_SWDSGN_CAN_DEINIT_PROC              (0x88U) /**< @brief #CAN_DeInit            */
#define BRCM_SWDSGN_CAN_WRITE_PROC               (0x89U) /**< @brief #CAN_Write             */
#define BRCM_SWDSGN_CAN_SLEEP_PROC               (0x8AU) /**< @brief #CAN_Sleep             */
#define BRCM_SWDSGN_CAN_WAKEUP_PROC              (0x8BU) /**< @brief #CAN_WakeUp            */
#define BRCM_SWDSGN_CAN_GETERRCNTS_PROC          (0x8CU) /**< @brief #CAN_GetErrCnts        */
#define BRCM_SWDSGN_CAN_PROCESSPKTS_PROC         (0x8DU) /**< @brief #CAN_ProcessPkts       */
/** @} */


/**
    @trace #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace #BRCM_SWARCH_CAN_INIT_PROC
    @trace #BRCM_SWARCH_CAN_DEINIT_PROC
    @trace #BRCM_SWREQ_CAN_INIT
*/
static void CAN_ReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
                            uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CAN_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace #BRCM_SWREQ_CAN_COMMUNICATION
 */
static int32_t CAN_GetRxMsg(CAN_HwIDType aID,
                            CAN_RxMsgObjType ** const aMsg)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.rxObj = NULL;

    retVal = CAN_SysCmdReq(CAN_CMD_GET_RX_PKT, &canIO);
    if (BCM_ERR_OK == retVal) {
        *aMsg = canIO.rxObj;
    }

    return retVal;
}

/**
    @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
 */
static void CAN_PutRxPkt(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;

    retVal = CAN_SysCmdReq(CAN_CMD_DEQUEUE_RX_PKT, &canIO);
    if (BCM_ERR_OK != retVal) {
        CAN_ReportError((uint8_t)aID, BRCM_SWDSGN_CAN_PUTRXMSG_PROC,
                        retVal, 0UL, 0UL, 0UL, __LINE__);
    }
}

/**
    @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessRcvdPkts(CAN_HwIDType aID)
{
    int32_t retVal;
    CAN_RxMsgObjType *pRxMsg = NULL;

    retVal = CAN_GetRxMsg(aID, &pRxMsg);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        CAN_RxInd(aID, pRxMsg);
        CAN_PutRxPkt(aID);
    } else {
        if (BCM_ERR_EAGAIN != retVal) {
            CAN_ReportError((uint8_t)aID, BRCM_SWDSGN_CAN_PROCESSRCVDPKTS_PROC,
                                retVal, (uint32_t)pRxMsg, 0UL, 0UL, __LINE__);
        }
    }
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessXmtdPkts(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;
    CAN_TxMsgObjType txMsg;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.txObj = &txMsg;

    retVal = CAN_SysCmdReq(CAN_CMD_GET_TX_PKT, &canIO);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        CAN_TxDoneInd(aID, &txMsg);
    } else {
        if (BCM_ERR_EAGAIN != retVal) {
            CAN_ReportError((uint8_t)aID, BRCM_SWDSGN_CAN_PROCESSXMTDPKTS_PROC,
                                retVal, txMsg.msgInfo.msgID, txMsg.msgInfo.length, 0UL, __LINE__);
        }
    }
    return retVal;
}


/**
    @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static int32_t CAN_ProcessErrPkts(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;
    CAN_ErrorType errCode;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.errCode = &errCode;

    retVal = CAN_SysCmdReq(CAN_CMD_GET_ERR_CODE, &canIO);
    if (BCM_ERR_OK == retVal) {
        /* Invoke Callback function */
        CAN_ErrorInd(aID, errCode);
    } else {
        if (BCM_ERR_EAGAIN != retVal) {
            CAN_ReportError((uint8_t)aID, BRCM_SWDSGN_CAN_PROCESSERRPKTS_PROC,
                                retVal, 0UL, 0UL, 0UL, __LINE__);
        }
    }

    return retVal;
}


/**
    @brief  Process Bus_off interrupt from MCAN

    @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
static void CAN_ProcessBusOff(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;
    uint32_t raiseBusOffCb = FALSE;
    uint32_t line;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.raiseCb = &raiseBusOffCb;

    retVal = CAN_SysCmdReq(CAN_CMD_BUS_OFF, &canIO);
    if (BCM_ERR_OK == retVal) {
        if (TRUE == raiseBusOffCb) {
            /* Invoke Callback function */
            CAN_BusOffInd(aID);

            /* issue Bus_off recover procedure */
            canIO.retVal = BCM_ERR_UNKNOWN;
            canIO.hwID = aID;
            retVal = CAN_SysCmdReq(CAN_CMD_BUS_OFF_RECOVERY, &canIO);
            if (BCM_ERR_OK != retVal) {
                line = __LINE__;
            }
        }
    } else {
        line = __LINE__;
    }

    if (BCM_ERR_OK != retVal) {
        CAN_ReportError((uint8_t)aID, BRCM_SWDSGN_CAN_PROCESSBUSOFF_PROC,
                retVal, 0UL, 0UL, 0UL, line);
    }
}

/**
    @trace  #BRCM_SWARCH_CAN_INIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
void CAN_Init(CAN_HwIDType aID, const CAN_ConfigType * const aConfig)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.configData = aConfig;

    retVal = CAN_SysCmdReq(CAN_CMD_INIT, &canIO);
    if (BCM_ERR_OK != retVal) {
        CAN_ReportError((uint8_t)aID, BRCM_SWARCH_CAN_INIT_PROC, retVal,
                        0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace  #BRCM_SWARCH_CAN_DEINIT_PROC
    @trace  #BRCM_SWREQ_CAN_INIT
*/
void CAN_DeInit(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;

    retVal = CAN_SysCmdReq(CAN_CMD_DEINIT, &canIO);
    if (BCM_ERR_OK != retVal) {
        CAN_ReportError((uint8_t)aID, BRCM_SWARCH_CAN_DEINIT_PROC, retVal,
                        0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace  #BRCM_SWARCH_CAN_WRITE_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
int32_t CAN_Write(CAN_HwIDType aID, CAN_TxMsgObjType * const aTxObj)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.txObj = aTxObj;

    retVal = CAN_SysCmdReq(CAN_CMD_WRITE, &canIO);
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_CAN_SLEEP_PROC
    @trace  #BRCM_SWREQ_CAN_POWER
*/
int32_t CAN_Sleep(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;

    retVal = CAN_SysCmdReq(CAN_CMD_SLEEP, &canIO);
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_CAN_WAKEUP_PROC
    @trace  #BRCM_SWREQ_CAN_POWER
*/
int32_t CAN_WakeUp(CAN_HwIDType aID)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;

    retVal = CAN_SysCmdReq(CAN_CMD_WAKEUP, &canIO);
    return retVal;
}

/**
    @trace  #BRCM_SWARCH_CAN_GETERRCNTS_PROC
    @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
int32_t CAN_GetErrCnts(CAN_HwIDType aID, CAN_ErrCntType * const aErrCntDesc)
{
    CAN_IOType canIO;
    int32_t retVal;

    canIO.retVal = BCM_ERR_UNKNOWN;
    canIO.hwID = aID;
    canIO.errCntDesc = aErrCntDesc;

    retVal = CAN_SysCmdReq(CAN_CMD_GET_ERR_CNTS, &canIO);
    return retVal;
}



/**
  @trace  #BRCM_SWARCH_CAN_PROCESSPACKETS_PROC
  @trace  #BRCM_SWREQ_CAN_COMMUNICATION
*/
void CAN_ProcessPkts(CAN_HwIDType aID)
{
    int32_t retVal;

    do {
        retVal = CAN_ProcessRcvdPkts(aID);
    } while (BCM_ERR_OK == retVal);

    do {
        retVal = CAN_ProcessXmtdPkts(aID);
    } while (BCM_ERR_OK == retVal);

    do {
        retVal = CAN_ProcessErrPkts(aID);
    } while (BCM_ERR_OK == retVal);

    CAN_ProcessBusOff(aID);
}
/** @} */
