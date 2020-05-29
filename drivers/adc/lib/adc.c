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

/**
    @defgroup grp_adcdrv_impl Implementation
    @ingroup grp_adcdrv

    @addtogroup grp_adcdrv_impl
    @{

    @file adc.c
    @brief ADC Driver implementation
    This header file contains the interface functions for ADC Driver
    @version 1.0 Changelist1
*/

#include <stdlib.h>
#include <adc.h>
#include <adc_osil.h>
#include <bcm_err.h>

/**
    @name ADC Driver design IDs
    @{
    @brief Design IDs for ADC Driver
*/
#define BRCM_SWDSGN_ADC_REPORTERROR_PROC         (0x80U) /**< @brief #ADC_ReportError       */
#define BRCM_SWDSGN_ADC_INIT_PROC                (0x81U) /**< @brief #ADC_Init              */
#define BRCM_SWDSGN_ADC_DEINIT_PROC              (0x82U) /**< @brief #ADC_DeInit            */
#define BRCM_SWDSGN_ADC_STARTGROUPCONV_PROC      (0x83U) /**< @brief #ADC_StartGroupConv    */
#define BRCM_SWDSGN_ADC_STOPGROUPCONV_PROC       (0x84U) /**< @brief #ADC_StopGroupConv     */
#define BRCM_SWDSGN_ADC_READGROUPSAMPLE_PROC     (0x85U) /**< @brief #ADC_ReadGroupSample   */
#define BRCM_SWDSGN_ADC_ENABLEGROUPNOTIF_PROC    (0x86U) /**< @brief #ADC_EnableGroupNotif  */
#define BRCM_SWDSGN_ADC_DISABLEGROUPNOTIF_PROC   (0x87U) /**< @brief #ADC_DisableGroupNotif */
#define BRCM_SWDSGN_ADC_GETGROUPCONVSTATE_PROC   (0x88U) /**< @brief #ADC_GetGroupConvState */
/** @} */

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static void ADC_ReportError(uint32_t aInstanceID, uint8_t aApiID, int32_t aErr,
                            uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
                            uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_ADC_ID & BCM_LOGMASK_USER), (uint8_t)aInstanceID, aApiID,
                    aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
void ADC_Init(void)
{
    ADC_IOType adcIO;
    int32_t retVal;

    adcIO.retVal = BCM_ERR_UNKNOWN;

    retVal = ADC_SysCmdReq(ADC_CMD_INIT, &adcIO);
    if (BCM_ERR_OK != retVal) {
        ADC_ReportError(0UL, BRCM_SWARCH_ADC_INIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
void ADC_DeInit(void)
{
    ADC_IOType adcIO;
    int32_t retVal;

    adcIO.retVal = BCM_ERR_UNKNOWN;

    retVal = ADC_SysCmdReq(ADC_CMD_DEINIT, &adcIO);
    if (BCM_ERR_OK != retVal) {
        ADC_ReportError(0UL, BRCM_SWARCH_ADC_DEINIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
int32_t ADC_StartGroupConv(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    ADC_IOType adcIO;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;

    retVal = ADC_SysCmdReq(ADC_CMD_START_GRP_CONV, &adcIO);
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
int32_t ADC_StopGroupConv(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    ADC_IOType adcIO;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;

    retVal = ADC_SysCmdReq(ADC_CMD_STOP_GRP_CONV, &adcIO);
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_READGROUPSAMPLE_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
int32_t ADC_ReadGroupSample(ADC_ChanGrpIDType aGrpID, uint32_t *aBuf)
{
    int32_t retVal;
    ADC_IOType adcIO;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;
    adcIO.sampleBuf = aBuf;

    retVal = ADC_SysCmdReq(ADC_CMD_READ_GRP_SAMPLE, &adcIO);
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
int32_t ADC_EnableGroupNotif(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    ADC_IOType adcIO;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;

    retVal = ADC_SysCmdReq(ADC_CMD_ENABLE_GRP_NOTIF, &adcIO);
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
int32_t ADC_DisableGroupNotif(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    ADC_IOType adcIO;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;

    retVal = ADC_SysCmdReq(ADC_CMD_DISABLE_GRP_NOTIF, &adcIO);
    return retVal;
}


/**
    @trace #BRCM_SWARCH_ADC_GETGROUPCONVSTATE_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
ADC_ChanGrpConvStateType ADC_GetGroupConvState(ADC_ChanGrpIDType aGrpID)
{
    ADC_ChanGrpConvStateType state;
    ADC_IOType adcIO;
    int32_t retVal;

    adcIO.retVal = BCM_ERR_UNKNOWN;
    adcIO.grpID = aGrpID;
    adcIO.convState = ADC_CHANGRPCONVSTATE_IDLE;

    retVal = ADC_SysCmdReq(ADC_CMD_GET_GRP_CONV_STATE, &adcIO);
    if (BCM_ERR_OK == retVal) {
        state = adcIO.convState;
    } else {
        state = ADC_CHANGRPCONVSTATE_IDLE;
    }
    return state;
}

/** @} */
