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
    @defgroup grp_i2cdrv_bsc_impl IIC Driver Design
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_bsc_impl
    @{
*/

/* Header files */
#include <stdint.h>
#include <string.h>
#include <chip_config.h>
#include <compiler.h>
#include <bcm_err.h>
#include <i2c.h>
#include <i2c_osil.h>
#include <system.h>
#include <bcm_utils.h>
#include <lw_queue.h>
#include <atomic.h>
#include "iic_rdb.h"

/**
    @name BSC driver Design IDs
    @{
    @brief Design IDs for BSC drive
*/
#define BRCM_SWDSGN_IIC_MAX_RETRIES_MACRO           () /**< @brief #IIC_SLAVE_ADDR_RETRIES      */
#define BRCM_SWDSGN_IIC_MSG_TYPE                    () /**< @brief #IIC_MsgType                 */
#define BRCM_SWDSGN_IIC_MSG_GLOBAL                  () /**< @brief #IIC_Msg                     */
#define BRCM_SWDSGN_IIC_BSC_STATE_TYPE              () /**< @brief #IIC_BscStateType            */
#define BRCM_SWDSGN_IIC_XFER_STATE_TYPE             () /**< @brief #IIC_XferStateType           */
#define BRCM_SWDSGN_IIC_BSCRWDEV_TYPE               () /**< @brief #IIC_BscRWDevType            */
#define BRCM_SWDSGN_IIC_BSCREGS_GLOBAL              () /**< @brief #IIC_BSC_REGS                */
#define BRCM_SWDSGN_IIC_BSCRDWRDEV_GLOBAL           () /**< @brief #IIC_BscRWDev                */
#define BRCM_SWDSGN_IIC_BSC_REPORTERROR_PROC        (0x81U) /**< @brief #IIC_BscReportError     */
#define BRCM_SWDSGN_IIC_BSC_SENDCMD_PROC            (0x85U) /**< @brief #IIC_BscSendCmd         */
#define BRCM_SWDSGN_IIC_BSC_SLAVE2ADDR_PROC         (0x8DU) /**< @brief #IIC_SlaveAddr2Bytes    */
#define BRCM_SWDSGN_IIC_BSC_STATE_HANDLER_PROC      (0x8DU) /**< @brief #IIC_XferStateHandler   */
#define BRCM_SWDSGN_IIC_BSC_START_TRANSFER_PROC     (0x8DU) /**< @brief #IIC_BscStartTransfer   */
#define BRCM_SWDSGN_IIC_BSC_INIT_PROC               (0x8DU) /**< @brief #IIC_BscInit            */
#define BRCM_SWDSGN_IIC_BSC_TRANSGER_PROC           (0x8DU) /**< @brief #IIC_BscTransfer        */
#define BRCM_SWDSGN_IIC_BSC_GETSTATUS_PROC          (0x8DU) /**< @brief #IIC_BscGetStatus       */
#define BRCM_SWDSGN_IIC_BSC_DEINIT_PROC             (0x8EU) /**< @brief #IIC_BscDeInit          */
#define BRCM_SWDSGN_IIC_IRQHANDLER_PROC             (0x8FU) /**< @brief #IIC_IRQHandler         */
#define BRCM_SWDSGN_IIC_SVCIO_TYPE                  (0x90U) /**< @brief #IIC_SVCIOType          */
#define BRCM_SWDSGN_IIC_SYSCMDHANDLER_PROC          (0x91U) /**< @brief #IIC_SysCmdHandler      */
/** @} */

/** @brief Number of retries for slave address

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
 */
#define IIC_SLAVE_ADDR_RETRIES                  (3UL)

/**
    @brief IIC Message type

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

 */
typedef struct _IIC_MsgType {
    int32_t                 retVal;     /**< @brief Return value            */
    int32_t                 clientID;
    uint32_t                clientMask;
    uint32_t                numPkts;    /**< @brief Number of IIC packets   */
    IIC_PktType*            pkts;       /**< @brief Array of IIC packets    */
} IIC_MsgType;

/**
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static IIC_MsgType IIC_Msg[IIC_MAX_HW_ID][LWQ_BUFFER_MAX_LENGTH];

/** @name IIC Bsc state IDs
    @{
    @brief IIC Bsc state type

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef uint32_t IIC_BscStateType;
#define IIC_BSC_STATE_UNINIT        (0UL)
#define IIC_BSC_STATE_IDLE          (1UL)
/** @} */

/** @name IIC transfer state IDs
    @{
    @brief IIC transfer state type

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef uint32_t IIC_XferStateType;
#define IIC_XFER_STATE_START_CMD        (0UL)
#define IIC_XFER_STATE_RESTART_CMD      (1UL)
#define IIC_XFER_STATE_SLVADDR_BYTE0    (2UL)
#define IIC_XFER_STATE_SLVADDR_BYTE1    (3UL)
#define IIC_XFER_STATE_START_DATA       (4UL)
#define IIC_XFER_STATE_WRITE_BYTE       (5UL)
#define IIC_XFER_STATE_READ_CMD         (6UL)
#define IIC_XFER_STATE_READ_BYTE        (7UL)
#define IIC_XFER_STATE_STOP_CMD         (8UL)
#define IIC_XFER_STATE_END              (9UL)
/** @} */

/**
    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
typedef struct _IIC_BscRWDevType {
    IIC_BscStateType    state;
    IIC_XferStateType   xferState;
    IIC_PktType         *pkts;
    uint32_t            numPkts;
    uint8_t             slaveAddr[2UL];
    uint32_t            retryCnt;
    uint32_t            pktIdx;
    uint32_t            bufIdx;
    uint32_t            txFifoMode;
    volatile uint32_t   allocStatus;
    LWQ_BufferType      procQ;
} IIC_BscRWDevType;

/**
    @brief Global variable containing base address for I2C slave device

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static IIC_RDBType *const IIC_BSC_REGS[IIC_MAX_HW_ID] =
{
#if IIC_MAX_HW_ID >= 1UL
    (IIC_RDBType *)I2C0_BASE,
#endif
#if IIC_MAX_HW_ID >= 2UL
    (IIC_RDBType *)I2C1_BASE,
#endif
#if IIC_MAX_HW_ID >= 3UL
    (IIC_RDBType *)I2C2_BASE,
#endif
#if IIC_MAX_HW_ID == 4UL
    (IIC_RDBType *)I2C3_BASE,
#endif
#if IIC_MAX_HW_ID > 4UL
#error "IIC_MAX_HW_ID > 4"
#endif
};


/**
    @brief Global variable for I2C slave device handler

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static IIC_BscRWDevType IIC_BscRWDev[IIC_MAX_HW_ID] COMP_SECTION(".data.drivers") =
{
#if IIC_MAX_HW_ID >= 1UL
    {
        .state = IIC_BSC_STATE_UNINIT,
        .xferState = IIC_XFER_STATE_START_CMD,
        .pkts = NULL,
        .numPkts = 0UL,
        .slaveAddr = {0U, 0U},
        .retryCnt = 0UL,
        .pktIdx = 0UL,
        .bufIdx = 0UL,
        .txFifoMode = 0UL,
        .allocStatus = 0UL,
        .procQ = LWQ_BUFFER_INIT_VALUE,
    },
#endif
#if IIC_MAX_HW_ID >= 2UL
    {
        .state = IIC_BSC_STATE_UNINIT,
        .xferState = IIC_XFER_STATE_START_CMD,
        .pkts = NULL,
        .numPkts = 0UL,
        .slaveAddr = {0U, 0U},
        .retryCnt = 0UL,
        .pktIdx = 0UL,
        .bufIdx = 0UL,
        .txFifoMode = 0UL,
        .allocStatus = 0UL,
        .procQ = LWQ_BUFFER_INIT_VALUE,
    },
#endif
#if IIC_MAX_HW_ID >= 3UL
    {
        .state = IIC_BSC_STATE_UNINIT,
        .xferState = IIC_XFER_STATE_START_CMD,
        .pkts = NULL,
        .numPkts = 0UL,
        .slaveAddr = {0U, 0U},
        .retryCnt = 0UL,
        .pktIdx = 0UL,
        .bufIdx = 0UL,
        .txFifoMode = 0UL,
        .allocStatus = 0UL,
        .procQ = LWQ_BUFFER_INIT_VALUE,
    },
#endif
#if IIC_MAX_HW_ID == 4UL
    {
        .state = IIC_BSC_STATE_UNINIT,
        .xferState = IIC_XFER_STATE_START_CMD,
        .pkts = NULL,
        .numPkts = 0UL,
        .slaveAddr = {0U, 0U},
        .retryCnt = 0UL,
        .pktIdx = 0UL,
        .bufIdx = 0UL,
        .txFifoMode = 0UL,
        .allocStatus = 0UL,
        .procQ = LWQ_BUFFER_INIT_VALUE,
    },
#endif
#if IIC_MAX_HW_ID > 4UL
#error "IIC_MAX_HW_ID > 4"
#endif
};

/**
    @brief Error reporting function for the IIC BSC device

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static void IIC_BscReportError(uint8_t aInstanceID, uint8_t aApiID,
        int32_t aErrorID, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
        uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_IIC_ID, aInstanceID, aApiID, aErrorID, 4UL, values);
}

/**
    @brief Send a command to the slave device

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static void IIC_BscSendCmd(IIC_HwIDType aID, uint32_t aCmd)
{
    uint32_t reg;

    /* Disable BSC Interrupt */
    IIC_BSC_REGS[aID]->bscier = 0UL;

    /* configure BSC command and enable interrupt */
    reg =  IIC_BSC_REGS[aID]->bsccs;
    reg &= ~IIC_BSCCS_CMD_MASK;
    reg |= aCmd;
    IIC_BSC_REGS[aID]->bscier = IIC_BSCIER_BSC_INT_EN_MASK;
    IIC_BSC_REGS[aID]->bsccs = reg;
}

/**
    @brief Send the 2-byte address of the slave device

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static void IIC_SlaveAddr2Bytes(IIC_HwIDType aID, IIC_PktType *const aPkt)
{
    uint32_t slaveAddr;
    if ((aPkt->flags & IIC_PKT_FLG_SLAVE_10BIT) == 0UL) {
        if ((aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL) {
            slaveAddr = aPkt->slaveAddr << 1UL;
        } else {
            slaveAddr = (aPkt->slaveAddr << 1UL) | 0x1UL;
        }
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr;
    } else {
        if ((aPkt->flags & IIC_PKT_FLG_OP_RD) == 0UL) {
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) | 0xF0UL);
        } else {
            slaveAddr = (((aPkt->slaveAddr >> 7UL) & 0x6UL) | 0xF1UL);
        }
        IIC_BscRWDev[aID].slaveAddr[0] = (uint8_t)slaveAddr;
        IIC_BscRWDev[aID].slaveAddr[1] = (uint8_t)(aPkt->slaveAddr & 0xFFUL);
    }
}

/**
    @brief Transfer state handler

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static int32_t IIC_XferStateHandler(IIC_HwIDType aID, int32_t aError)
{
    IIC_PktType *pkt = NULL;
    IIC_BscRWDevType *stateInfo = &IIC_BscRWDev[aID];
    IIC_XferStateType state = IIC_BscRWDev[aID].xferState;
    IIC_XferStateType nextState = state;
    int32_t ret = BCM_ERR_BUSY;

    if ((IIC_BSC_REGS[aID]->bsccs & IIC_BSCCS_BUSY_MASK) == 0UL) {
        switch (state) {
        case IIC_XFER_STATE_START_CMD:
            stateInfo->retryCnt = 0UL;
            stateInfo->pktIdx = 0UL;
            stateInfo->bufIdx = 0UL;
            IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START);
            ret = BCM_ERR_BUSY;
            nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
            break;
        case IIC_XFER_STATE_RESTART_CMD:
            stateInfo->retryCnt = 0UL;
            stateInfo->bufIdx = 0UL;
            pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
            if ((pkt->flags & IIC_PKT_FLG_RESTART) != 0UL) {
                IIC_BscSendCmd(aID, IIC_BSCCS_CMD_START | BSC_ACK_GEN_REPEATED_START);
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
            }
            ret = BCM_ERR_BUSY;
            break;
        case IIC_XFER_STATE_SLVADDR_BYTE0:
            pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
            IIC_SlaveAddr2Bytes(aID, pkt);
            IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[0UL];
            if ((pkt->flags & IIC_PKT_FLG_SLAVE_10BIT) != 0UL) {
                nextState = IIC_XFER_STATE_SLVADDR_BYTE1;
            } else if ((pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL) {
                nextState = IIC_XFER_STATE_READ_CMD;
            } else {
                nextState = IIC_XFER_STATE_WRITE_BYTE;
            }
            ret = BCM_ERR_BUSY;
            break;
        case IIC_XFER_STATE_SLVADDR_BYTE1:
            pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
            if (BCM_ERR_OK == aError) {
                IIC_BSC_REGS[aID]->bscdat = stateInfo->slaveAddr[1UL];
                if ((pkt->flags & IIC_PKT_FLG_OP_RD) != 0UL) {
                    nextState = IIC_XFER_STATE_READ_CMD;
                } else {
                    nextState = IIC_XFER_STATE_WRITE_BYTE;
                }
                ret = BCM_ERR_BUSY;
            } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
                ret = BCM_ERR_EAGAIN;
                stateInfo->retryCnt++;
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
            } else {
                 ret = BCM_ERR_MAX_ATTEMPS;
            }
            break;
        case IIC_XFER_STATE_WRITE_BYTE:
            pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
            if (BCM_ERR_OK == aError) {
                stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES;
                IIC_BSC_REGS[aID]->bscdat = pkt->data[stateInfo->bufIdx++];
                if (stateInfo->bufIdx == pkt->length) {
                    stateInfo->pktIdx++;
                    stateInfo->bufIdx = 0UL;
                    if (stateInfo->pktIdx == stateInfo->numPkts) {
                        nextState = IIC_XFER_STATE_STOP_CMD;
                    } else {
                        nextState = IIC_XFER_STATE_RESTART_CMD;
                    }
                } else {
                    nextState = IIC_XFER_STATE_WRITE_BYTE;
                }
                ret = BCM_ERR_BUSY;
            } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
                ret = BCM_ERR_EAGAIN;
                stateInfo->retryCnt++;
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
            } else {
                 ret = BCM_ERR_MAX_ATTEMPS;
            }
            break;
        case IIC_XFER_STATE_READ_CMD:
            if (BCM_ERR_OK == aError) {
                stateInfo->retryCnt = IIC_SLAVE_ADDR_RETRIES;
                pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
                if (stateInfo->bufIdx == (pkt->length - 1UL)) {
                    IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE | BSC_ACK_NACK_AFTER_READ);
                } else {
                    IIC_BscSendCmd(aID, IIC_BSCCS_CMD_READ_BYTE | BSC_ACK_ACK_AFTER_READ);
                }
                ret = BCM_ERR_BUSY;
                nextState = IIC_XFER_STATE_READ_BYTE;
            } else if (stateInfo->retryCnt < IIC_SLAVE_ADDR_RETRIES) {
                ret = BCM_ERR_EAGAIN;
                stateInfo->retryCnt++;
                nextState = IIC_XFER_STATE_SLVADDR_BYTE0;
            } else {
                 ret = BCM_ERR_MAX_ATTEMPS;
            }
            break;
        case IIC_XFER_STATE_READ_BYTE:
            pkt = &IIC_BscRWDev[aID].pkts[stateInfo->pktIdx];
            if (BCM_ERR_OK == aError) {
                pkt->data[stateInfo->bufIdx++] = (uint8_t)(IIC_BSC_REGS[aID]->bscdat & IIC_BSCDAT_DAT_MASK);
                if (stateInfo->bufIdx == pkt->length) {
                    stateInfo->pktIdx++;
                    stateInfo->bufIdx = 0UL;
                    if (stateInfo->pktIdx == stateInfo->numPkts) {
                        nextState = IIC_XFER_STATE_STOP_CMD;
                    } else {
                        nextState = IIC_XFER_STATE_RESTART_CMD;
                    }
                } else {
                    nextState = IIC_XFER_STATE_READ_CMD;
                }
                ret = BCM_ERR_EAGAIN;
            } else {
                ret = aError;
            }
            break;
        case IIC_XFER_STATE_STOP_CMD:
            IIC_BscSendCmd(aID, IIC_BSCCS_CMD_STOP);
            ret = BCM_ERR_BUSY;
            nextState = IIC_XFER_STATE_END;
            break;
        case IIC_XFER_STATE_END:
            ret = BCM_ERR_OK;
            break;
        default:
            ret = BCM_ERR_INVAL_PARAMS;
            break;
        }
    }

    if ((BCM_ERR_BUSY != ret) && (BCM_ERR_EAGAIN != ret)) {
        stateInfo->xferState = IIC_XFER_STATE_START_CMD;
        stateInfo->pktIdx = 0UL;
        stateInfo->bufIdx = 0UL;
    } else {
        stateInfo->xferState = nextState;
    }
    return ret;
}

/**
    @brief Initiate the transfer for a packet

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static void IIC_BscStartTransfer(IIC_HwIDType aID)
{
    int32_t ret = BCM_ERR_OK;

    if ((IIC_XFER_STATE_START_CMD == IIC_BscRWDev[aID].xferState)
        && (LWQ_Length(IIC_BscRWDev[aID].procQ) > 0UL)){
        uint8_t jobID = LWQ_PeekFirst(IIC_BscRWDev[aID].procQ);
        /* Set i2c State to Busy */
        IIC_BscRWDev[aID].pkts = IIC_Msg[aID][jobID].pkts;
        IIC_BscRWDev[aID].numPkts = IIC_Msg[aID][jobID].numPkts;
        /* Trigger the transfer state machine */
        do {
            ret = IIC_XferStateHandler(aID, BCM_ERR_OK);
        } while (BCM_ERR_EAGAIN == ret);
    }
}

/**
    @brief Initialiaze the IIC BSC device

    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static int32_t IIC_BscInit(IIC_HwIDType aID, const IIC_ConfigType * const aConfig)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t reg;

    if ((aID >= IIC_MAX_HW_ID) || (aConfig->speed > IIC_SPEED_MAX) || (aConfig->speed < IIC_SPEED_100K)) {
        ret = BCM_ERR_INVAL_PARAMS;
        goto err_exit;
    }
    if (IIC_BscRWDev[aID].state != IIC_BSC_STATE_UNINIT) {
        ret = BCM_ERR_INVAL_STATE;
        goto err_exit;
    }

    if (TRUE == aConfig->txFifoMode) {
        ret = BCM_ERR_NOSUPPORT;
        goto err_exit;
    }


    IIC_BSC_REGS[aID]->bscfcr |= IIC_BSCFCR_FLUSH_MASK;
    IIC_BSC_REGS[aID]->bscfcr &= ~IIC_BSCFCR_FIFOEN_MASK;
    /* Enable BSC Controller clocks */
    IIC_BSC_REGS[aID]->bscclken |= IIC_BSCCLKEN_CLKEN_MASK;
    IIC_BSC_REGS[aID]->bsctout = (IIC_BSCTOUT_TOUT_MASK);

    /* configure the bus speed */
    switch (aConfig->speed) {
    case IIC_SPEED_100K:
        reg = IIC_BSC_REGS[aID]->bscclken;
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
        IIC_BSC_REGS[aID]->bscclken = reg;

        reg = IIC_BSC_REGS[aID]->bsctim;
        reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
        reg |= (0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
        reg |= (0x0UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
        IIC_BSC_REGS[aID]->bsctim = reg;
        break;
    case IIC_SPEED_400K:
        reg = IIC_BSC_REGS[aID]->bscclken;
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
        IIC_BSC_REGS[aID]->bscclken = reg;

        reg = IIC_BSC_REGS[aID]->bsctim;
        reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
        reg |= (0x2UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
        reg |= (0x3UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
        IIC_BSC_REGS[aID]->bsctim = reg;
        break;
    case IIC_SPEED_MAX:
        reg = IIC_BSC_REGS[aID]->bscclken;
        reg &= ~(IIC_BSCCLKEN_MPHASE_MASK | IIC_BSCCLKEN_NPHASE_MASK);
        IIC_BSC_REGS[aID]->bscclken = reg;

        reg = IIC_BSC_REGS[aID]->bsctim;
        reg &= ~(IIC_BSCTIM_P_MASK | IIC_BSCTIM_DIV_MASK);
        reg |= (0x0UL << IIC_BSCTIM_P_SHIFT) & IIC_BSCTIM_P_MASK;
        reg |= (0x3UL << IIC_BSCTIM_DIV_SHIFT) & IIC_BSCTIM_DIV_MASK;
        IIC_BSC_REGS[aID]->bsctim = reg;
        break;
    default:
        /* Never reach here */
        ret = BCM_ERR_UNKNOWN;
        break;
    }
    /* disable autosense by default */
    IIC_BSC_REGS[aID]->bscclken &= (~IIC_BSCCLKEN_AUTO_SENSE_MASK);
    /* Disable all the interrupts */
    IIC_BSC_REGS[aID]->bscier = 0UL;
    /* Clear all the interrupts */
    IIC_BSC_REGS[aID]->bscisr = IIC_BSCIER_ALL_INTERRUPTS_MASK;
    /* Enable BSC controller and set CMD = No Action */
    IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
    /* Enable controller */
    IIC_BSC_REGS[aID]->bsccs |= IIC_BSCCS_EN_MASK;

    IIC_BscRWDev[aID].allocStatus = (1UL << LWQ_BUFFER_MAX_LENGTH) - 1UL;
    LWQ_Init(&IIC_BscRWDev[aID].procQ);

    /* Set BSC state to Idle */
    IIC_BscRWDev[aID].state = IIC_BSC_STATE_IDLE;

err_exit:
    return ret;
}

/**
    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static int32_t IIC_BscTransfer(IIC_HwIDType aID,
                        IIC_PktType * const aPkts,
                        uint32_t aNumPkts,
                        int32_t aClientID,
                        uint32_t aClientMask,
                        uint32_t *aJobID)
{
    int32_t ret = BCM_ERR_NOMEM;
    uint32_t jobID;

    if ((aID >= IIC_MAX_HW_ID)
        || (NULL == aPkts)){
        ret =BCM_ERR_INVAL_PARAMS;
    } else {
        jobID = ATOMIC_ClrNGetBit(&IIC_BscRWDev[aID].allocStatus);
        if (jobID < LWQ_BUFFER_MAX_LENGTH) {
            IIC_Msg[aID][jobID].retVal = BCM_ERR_BUSY;
            IIC_Msg[aID][jobID].numPkts = aNumPkts;
            IIC_Msg[aID][jobID].pkts = aPkts;
            IIC_Msg[aID][jobID].clientID = aClientID;
            IIC_Msg[aID][jobID].clientMask = aClientMask;
            ret = LWQ_Push(&IIC_BscRWDev[aID].procQ, (uint8_t)jobID);
            if (BCM_ERR_OK == ret) {
                *aJobID = (aID << 4UL) | jobID;
            } else {
                ATOMIC_SetBit(&IIC_BscRWDev[aID].allocStatus, jobID);
                ret = BCM_ERR_UNKNOWN;
            }
        }
        IIC_BscStartTransfer(aID);
    }

    return ret;
}

/**
    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER
*/
static int32_t IIC_BscGetStatus(IIC_HwIDType aID,
                                int32_t aClientID,
                                const uint32_t aJobID)
{
    uint32_t jobID = aJobID & LWQ_BUFFER_MAX_LENGTH;
    int32_t retVal = BCM_ERR_BUSY;

    if (aID != (aJobID >> 4UL)) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else if (aClientID != IIC_Msg[aID][jobID].clientID) {
        retVal = BCM_ERR_AUTH_FAILED;
    } else if (BCM_ERR_BUSY != IIC_Msg[aID][jobID].retVal) {
        retVal = IIC_Msg[aID][jobID].retVal;
        ATOMIC_SetBit(&IIC_BscRWDev[aID].allocStatus, jobID);
    } else {
        retVal = BCM_ERR_BUSY;
    }

    return retVal;
}

/**
    @brief De-initialiaze the IIC BSC device

    @trace #BRCM_SWARCH_IIC_DEINIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
static int32_t IIC_BscDeInit(IIC_HwIDType aID)
{
    int32_t ret = BCM_ERR_OK;

    if (aID < IIC_MAX_HW_ID) {

        /* Check Bsc device State */
        if (IIC_BscRWDev[aID].state == IIC_BSC_STATE_UNINIT) {
            ret = BCM_ERR_INVAL_STATE;
        } else {
            if (IIC_BscRWDev[aID].state == IIC_BSC_STATE_IDLE) {
                /* Disable all the interrupts */
                IIC_BSC_REGS[aID]->bscier = ~(IIC_BSCIER_ALL_INTERRUPTS_MASK);
                /* Clear all the interrupts */
                IIC_BSC_REGS[aID]->bscisr = IIC_BSCISR_ALL_INTERRUPTS_MASK;
                /* Disable BSC clock and disable the controller */
                IIC_BSC_REGS[aID]->bscclken = ~IIC_BSCCLKEN_CLKEN_MASK;
                IIC_BSC_REGS[aID]->bsccs &= ~IIC_BSCCS_EN_MASK;
                /* Set device state to Uninit */
                IIC_BscRWDev[aID].state = IIC_BSC_STATE_UNINIT;
            } else {
                ret = BCM_ERR_INVAL_STATE;
            }
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

/**
    @brief IRQ handler for the IIC BSC device

    @trace #BRCM_SWARCH_IIC_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
IIC_ClientSignalType IIC_IRQHandler(IIC_HwIDType aID)
{
    IIC_BscStateType state;
    int32_t ret;
    int32_t err;
    IIC_ClientSignalType clientSignal;
    clientSignal.done = FALSE;
    clientSignal.clientID = 0L;
    clientSignal.clientMask = 0UL;
    uint32_t intStatus = 0UL;

    if (aID < IIC_MAX_HW_ID) {
        state = IIC_BscRWDev[aID].state;
        if (IIC_BSC_STATE_UNINIT == state) {
            IIC_BscReportError((uint8_t)aID, BRCM_SWARCH_IIC_IRQHANDLER_PROC,
                    BCM_ERR_UNINIT, 0UL, 0UL, 0UL, 0UL);
        }
        intStatus = IIC_BSC_REGS[aID]->bscisr;
        IIC_BSC_REGS[aID]->bscisr = intStatus;
        IIC_BSC_REGS[aID]->bsccs &= ~(IIC_BSCCS_CMD_MASK | IIC_BSCCS_ACK_MASK);
        if ((intStatus & IIC_BSCISR_NOACK_MASK) != 0UL) {
            err = BCM_ERR_NACK;
        } else if ((intStatus & IIC_BSCISR_BSCERR_MASK) != 0UL) {
            err = BCM_ERR_BUS_FAILURE;
        } else {
            err = BCM_ERR_OK;
        }
        do {
            ret = IIC_XferStateHandler(aID, err);
            err = BCM_ERR_OK;
        } while (BCM_ERR_EAGAIN == ret);
        if (BCM_ERR_BUSY != ret) {
            uint8_t jobID = 0U;
            (void)LWQ_Pop(&IIC_BscRWDev[aID].procQ, &jobID);
            clientSignal.done = TRUE;
            clientSignal.clientID = IIC_Msg[aID][jobID].clientID;
            clientSignal.clientMask = IIC_Msg[aID][jobID].clientMask;
            IIC_Msg[aID][jobID].retVal = ret;
            IIC_BscStartTransfer(aID);
        }
    }

    return clientSignal;
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace  #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_IIC_KERNEL_HANDLER
*/
typedef union _IIC_SVCIOType {
    uint8_t *data;
    IIC_IOType *io;
} IIC_SVCIOType;

/**
    @brief IIC command handler function

    @trace #BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_IIC_KERNEL_HANDLER

*/
void IIC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    IIC_SVCIOType i2c;
    i2c.data = aSysIO;
    int32_t retVal;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_IIC_ID == aMagicID) {
            switch(aCmd) {
            case IIC_CMD_INIT:
                retVal = IIC_BscInit(i2c.io->hwID, i2c.io->config);
                break;
            case IIC_CMD_DEINIT:
                retVal = IIC_BscDeInit(i2c.io->hwID);
                break;
            case IIC_CMD_TRANSFER_SUBMIT:
                i2c.io->jobID = 0xFFFFFFFFUL;
                retVal = IIC_BscTransfer(i2c.io->hwID, i2c.io->pkts, i2c.io->numPkts,
                                        i2c.io->clientID, i2c.io->clientMask, &i2c.io->jobID);
                break;
            case IIC_CMD_TRANSFER_STATUS:
                retVal = IIC_BscGetStatus(i2c.io->hwID,i2c.io->clientID, i2c.io->jobID);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        i2c.io->retVal = retVal;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_BUSY != retVal) &&
            (BCM_ERR_EAGAIN != retVal)) {
        IIC_BscReportError((uint8_t)0xFF, BRCM_SWARCH_IIC_SYSCMDHANDLER_PROC,
                retVal, aMagicID, aCmd, 0UL, 0UL);
    }
}
/** @} */
