/*****************************************************************************
 Copyright 2018-2019 Broadcom Limited.  All rights reserved.

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
 * @file bcm_wl_sdio.c
 *
 * @brief integration layer for Broadcom WIFI chipsets.
 *
 * This file implements the integration layer for Broadcom
 * wifi chipsets
 */
#include <stdint.h>
#include <bcm_utils.h>
#include <bcm_err.h>
#include <sd.h>
#include <wl_eth_osil.h>
#include <wl_sdio_osil.h>
#include <osil/bcm_osil.h>
#include "ee.h"

#define WLAN_TASK_ID                        (TaskWlan)

#define WLETH_REQ_EVENT                     (SystemEvent0)
#define WLETH_SDIO_XFER_COMPLT_EVENT        (SystemEvent1)
#define WLETH_CARD_ASYNC_EVENT              (SystemEvent2)
#define WLETH_ALARM_CB_EVENT                (SystemEvent3)
#define WLETH_EVENTS                        (WLETH_REQ_EVENT |\
                                             WLETH_SDIO_XFER_COMPLT_EVENT|\
                                             WLETH_CARD_ASYNC_EVENT|\
                                             WLETH_ALARM_CB_EVENT)

static SD_OpResultType result;

ALARMCALLBACK(WL_CreditPokeAlarmCb)
{
    SetEvent(TaskWlan, WLETH_ALARM_CB_EVENT);
}

void WLETH_SdioOpDoneInd(SD_SlotIDType aID, SD_ClientIDType aClientID,
        SD_OpResultType aResult)
{
    result = aResult;
    (void)BCM_SetEvent(TaskWlan, WLETH_SDIO_XFER_COMPLT_EVENT);
}

void WLETH_SdioCardIntrInd(SD_SlotIDType aID, SD_ClientIDType aClientID)
{
    (void)BCM_SetEvent(TaskWlan, WLETH_CARD_ASYNC_EVENT);
}

int32_t WLETH_Request(ETHER_HwIDType aID, WLETH_ReqType *const aReq)
{
    int32_t ret;

    if (NULL != aReq) {
        ret = WLETH_ReqHandler(aID, aReq);
        if (BCM_ERR_BUSY == ret) {
            ret = BCM_ERR_OK;
            BCM_SetEvent(WLAN_TASK_ID, WLETH_REQ_EVENT);
        }
    } else {
        ret = BCM_ERR_INVAL_PARAMS;
    }
    return ret;
}

TASK(TaskWlan)
{
    int32_t status = BCM_ERR_OK;
    BCM_EventMaskType mask;

    while (BCM_ERR_OK == status) {
        status = BCM_WaitEvent(WLETH_EVENTS);
        if (BCM_ERR_OK != status) {
            break;
        }
        status = BCM_GetEvent(TaskWlan, &mask);
        if (BCM_ERR_OK != status) {
            break;
        }
        if ((mask & WLETH_EVENTS) != 0UL) {
            status = BCM_ClearEvent(mask);
            if (BCM_ERR_OK != status) {
                break;
            }
            if ((mask & WLETH_CARD_ASYNC_EVENT) != 0UL) {
                WLETH_SdioCardIntrHandler(0UL);
            }
            if ((mask & WLETH_ALARM_CB_EVENT) != 0UL) {
                WLETH_TimerExpiryHandler(0UL);
            }
            if ((mask & WLETH_SDIO_XFER_COMPLT_EVENT) != 0UL) {
                if (result == SD_OP_RESULT_SUCCESS) {
                    WLETH_SdioXferCmpltHandler(0UL, result);
                }
            }
            if ((mask & WLETH_REQ_EVENT) != 0UL) {
                WLETH_ProcessRequest(0UL);
            }
        }
    }
}
