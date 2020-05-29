/*****************************************************************************
 Copyright 2017-2018 Broadcom Limited.  All rights reserved.

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
#include <osil/lin_osil.h>
#include "lin_drv.h"

/* LIN driver commands for SVC */
#define LIN_CMD_INIT                            (1UL)
#define LIN_CMD_DEINIT                          (2UL)
#define LIN_CMD_TRANSMIT_FRAME                  (3UL)
#define LIN_CMD_SLEEP                           (4UL)
#define LIN_CMD_WAKEUP                          (5UL)
#define LIN_CMD_READ_STATUS                     (6UL)
#define LIN_CMD_GET_DATA                        (7UL)

static void LIN_ReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
        uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_LIN_ID & BCM_LOGMASK_USER), aInstanceID, aApiID, aErr,
            4UL, values);
}

void LIN_Init(LIN_HwIDType aHwID, const LIN_ConfigType *const aConfig)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    linIO.config = aConfig;
    retVal = LIN_SysCmdReq(LIN_CMD_INIT, &linIO);

    if (BCM_ERR_OK != retVal) {
        LIN_ReportError((uint8_t)aHwID, BRCM_SWARCH_LIN_INIT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

void LIN_DeInit(LIN_HwIDType aHwID)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    retVal = LIN_SysCmdReq(LIN_CMD_DEINIT, &linIO);
    if (BCM_ERR_OK != retVal) {
        LIN_ReportError((uint8_t)aHwID, BRCM_SWARCH_LIN_DEINIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

int32_t LIN_TransmitFrame(LIN_HwIDType aHwID, const LIN_FrmObjType * const aFrmObj)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    linIO.frmObj = aFrmObj;
    retVal = LIN_SysCmdReq(LIN_CMD_TRANSMIT_FRAME, &linIO);

    return retVal;
}

int32_t LIN_SendSleep(LIN_HwIDType aHwID)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    retVal = LIN_SysCmdReq(LIN_CMD_SLEEP, &linIO);

    return retVal;
}

int32_t LIN_SendWakeup(LIN_HwIDType aHwID)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    retVal = LIN_SysCmdReq(LIN_CMD_WAKEUP, &linIO);

    return retVal;
}

int32_t LIN_ReadStatus(LIN_HwIDType aHwID)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    retVal = LIN_SysCmdReq(LIN_CMD_READ_STATUS, &linIO);

    return retVal;
}

int32_t LIN_GetData(LIN_HwIDType aHwID, uint8_t *const aDataPtr)
{
    int32_t retVal;
    LINIO linIO;

    linIO.retVal = BCM_ERR_UNKNOWN;
    linIO.hwID = aHwID;
    linIO.dataPtr = aDataPtr;
    retVal = LIN_SysCmdReq(LIN_CMD_GET_DATA, &linIO);

    return retVal;
}


int32_t LIN_CmdHandler(LIN_CmdType aCmd,
                       LINIO *const aIO)
{
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aIO) {
        switch (aCmd) {
        case LIN_CMD_INIT:
            LINDrv_Init(aIO->hwID, aIO->config);
            break;
        case LIN_CMD_DEINIT:
            LINDrv_Deinit(aIO->hwID);
            break;
        case LIN_CMD_TRANSMIT_FRAME:
            retVal = LINDrv_TransmitFrame(aIO->hwID, aIO->frmObj);
            break;
        case LIN_CMD_SLEEP:
            retVal = LINDrv_Sleep(aIO->hwID);
            break;
        case LIN_CMD_WAKEUP:
            retVal = LINDrv_WakeUp(aIO->hwID);
            break;
        case LIN_CMD_READ_STATUS:
            retVal = LINDrv_ReadStatus(aIO->hwID);
            break;
        case LIN_CMD_GET_DATA:
            retVal = LINDrv_GetData(aIO->hwID, aIO->dataPtr);
            break;
        default:
            retVal = BCM_ERR_INVAL_PARAMS;
            break;
        }
    } else {
        retVal = BCM_ERR_UNKNOWN;
    }

    return retVal;
}
