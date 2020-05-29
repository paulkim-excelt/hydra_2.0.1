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

#define GetModuleLogLevel()     ULOG_LVL_INFO

#include <stdlib.h>
#include <string.h>
#include <ulog.h>
#include "ee.h"
#include <bcm_utils.h>
#include <vgc_display_service.h>
#include <board.h>
#include <init.h>
#include <osil/bcm_osil.h>

#define VGCD_CTRL_RESP_EVENT    SystemEvent1

static int32_t Disp_Enable()
{
    const MSGQ_MsgHdrType* hdr = NULL;
    int32_t retVal;

    retVal = VGCD_Enable(VGCD_CTRL_RESP_EVENT, &hdr);
    if (BCM_ERR_OK == retVal) {
        retVal = BCM_WaitEvent(VGCD_CTRL_RESP_EVENT);
        if (BCM_ERR_OK == retVal) {
            retVal = BCM_ClearEvent(VGCD_CTRL_RESP_EVENT);
            if (BCM_ERR_OK == retVal) {
                retVal = VGCD_GetCtrlResponse(hdr);
                if (BCM_ERR_OK == retVal) {
                    ULOG_ERR("Display enabled successfully\n");
                } else {
                    ULOG_ERR("Failed to enable display err=%d\n", retVal);
                }
            } else {
                retVal = BCM_ERR_UNKNOWN;
                ULOG_ERR("Failed to clear event for Enable Resp err=%u\n", retVal);
            }
        } else {
            retVal = BCM_ERR_UNKNOWN;
            ULOG_ERR("Error while waiting for Enable Resp err=%u\n", retVal);
        }
    } else {
        ULOG_ERR("Failed to enable display ret=%d\n", retVal);
    }
    return retVal;
}

void Display_Init()
{
    const MSGQ_MsgHdrType* hdr = NULL;
    int32_t retVal;

    retVal = VGCD_Init(&VGC_Cfg, VGCD_CTRL_RESP_EVENT, &hdr);
    if (BCM_ERR_OK == retVal) {
        retVal = BCM_WaitEvent(VGCD_CTRL_RESP_EVENT);
        if (BCM_ERR_OK == retVal) {
            retVal = BCM_ClearEvent(VGCD_CTRL_RESP_EVENT);
            if (BCM_ERR_OK == retVal) {
                retVal = VGCD_GetCtrlResponse(hdr);
                if (BCM_ERR_OK == retVal) {
                    ULOG_ERR("Display Init successful\n");
                } else {
                    ULOG_ERR("Failed to initialise display ret=%d\n",
                            retVal);
                }
#ifndef ENABLE_VGC_LL
                retVal = Disp_Enable();
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("Failed to enable display ret=%d\n", retVal);
                }
#endif
            } else {
                ULOG_ERR("Failed to clear event for Init Resp err=%u\n",
                        retVal);
            }
        } else {
            ULOG_ERR("Error while waiting for Init Resp err=%u\n", retVal);
        }
    } else {
        ULOG_ERR("Failed to initialise display ret=%d\n", retVal);
    }
}
