/*****************************************************************************
 Copyright 2016-2019 Broadcom Limited.  All rights reserved.

 This program is the proprietary software of Broadcom Corporation and/or its
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
    @defgroup grp_ccudrv_impl CCu Client Design
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_impl
    @{
*/

#include <stdint.h>
#include <osil/ccu_osil.h>

/**
    @name CCU API IDs
    @{
    @brief API IDs for CCU driver
*/
#define BRCM_SWDSGN_CCU_REPORTERROR_PROC                (0x80U) /**< @brief #CCU_ReportError */
#define BRCM_SWDSGN_CCU_INIT_PROC                       (0x81U) /**< @brief #CCU_Init */
#define BRCM_SWDSGN_CCU_DEINIT_PROC                     (0x82U) /**< @brief #CCU_DeInit */
#define BRCM_SWDSGN_CCU_GETCHANMODE_PROC                (0x83U) /**< @brief #CCU_GetChanMode */
#define BRCM_SWDSGN_CCU_STARTCOMPCHANNEL_PROC           (0x84U) /**< @brief #CCU_StartCompChannel */
#define BRCM_SWDSGN_CCU_STOPCOMPCHANNEL_PROC            (0x85U) /**< @brief #CCU_StopCompChannel */
#define BRCM_SWDSGN_CCU_SETCOMPPINPROPERTY_PROC         (0x86U) /**< @brief #CCU_SetCompPinProperty */
#define BRCM_SWDSGN_CCU_SETCOMPABSTHRESHOLD_PROC        (0x87U) /**< @brief #CCU_SetCompAbsThreshold */
#define BRCM_SWDSGN_CCU_ENABLECOMPCALLBACK_PROC         (0x88U) /**< @brief #CCU_EnableCompCallback */
#define BRCM_SWDSGN_CCU_DISABLECOMPCALLBACK_PROC        (0x89U) /**< @brief #CCU_DisableCompCallback */
#define BRCM_SWDSGN_CCU_SETCAPSIGNALPROP_PROC           (0x8AU) /**< @brief #CCU_SetCapSignalProp */
#define BRCM_SWDSGN_CCU_ENABLECAPCALLBACK_PROC          (0x8BU) /**< @brief #CCU_EnableCapCallback */
#define BRCM_SWDSGN_CCU_DISABLECAPCALLBACK_PROC         (0x8CU) /**< @brief #CCU_DisableCapCallback */
#define BRCM_SWDSGN_CCU_GETCAPINPUTSTATE_PROC           (0x8DU) /**< @brief #CCU_GetCapInputState */
#define BRCM_SWDSGN_CCU_STARTCAPTIMESTAMP_PROC          (0x8EU) /**< @brief #CCU_StartCapTimestamp */
#define BRCM_SWDSGN_CCU_STOPCAPTIMESTAMP_PROC           (0x8FU) /**< @brief #CCU_StopCapTimestamp */
#define BRCM_SWDSGN_CCU_GETCAPTIMESTAMPINDEX_PROC       (0x90U) /**< @brief #CCU_GetCapTimestampIndex */
#define BRCM_SWDSGN_CCU_RESETCAPEDGECOUNT_PROC          (0x91U) /**< @brief #CCU_ResetCapEdgeCount */
#define BRCM_SWDSGN_CCU_ENABLECAPEDGECOUNT_PROC         (0x92U) /**< @brief #CCU_EnableCapEdgeCount */
#define BRCM_SWDSGN_CCU_DISABLECAPEDGECOUNT_PROC        (0x93U) /**< @brief #CCU_DisableCapEdgeCount */
#define BRCM_SWDSGN_CCU_ENABLECAPEDGEDETECTION_PROC     (0x94U) /**< @brief #CCU_EnableCapEdgeDetection */
#define BRCM_SWDSGN_CCU_DISABLECAPEDGEDETECTION_PROC    (0x95U) /**< @brief #CCU_DisableCapEdgeDetection */
#define BRCM_SWDSGN_CCU_GETCAPEDGENUMBERS_PROC          (0x96U) /**< @brief #CCU_GetCapEdgeNumbers */
#define BRCM_SWDSGN_CCU_STARTCAPSIGNALMEASUREMENT_PROC  (0x97U) /**< @brief #CCU_StartCapSignalMeasurement */
#define BRCM_SWDSGN_CCU_STOPCAPSIGNALMEASUREMENT_PROC   (0x98U) /**< @brief #CCU_StopCapSignalMeasurement */
#define BRCM_SWDSGN_CCU_GETCAPTIMEELAPSED_PROC          (0x99U) /**< @brief #CCU_GetCapTimeElapsed */
#define BRCM_SWDSGN_CCU_GETCAPDUTYCYCLEVALUES_PROC      (0x9AU) /**< @brief #CCU_GetCapDutyCycleValues */
/** @} */

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_ReportError(uint8_t aInstanceID, uint8_t aApiID, int32_t aErr,
                            uint32_t aVal0, uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_CCT_ID & BCM_LOGMASK_USER), aInstanceID, aApiID, aErr,
            4UL, values);
}

/** @} */

/**
    @addtogroup grp_ccudrv_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_Init(const CCU_ConfigType *const aConfig)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        ccuIO.retVal = BCM_ERR_UNKNOWN;
        ccuIO.config = aConfig;
        retVal = CCU_SysCmdReq(CCU_CMD_INIT, &ccuIO);
    }
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError(0U, BRCM_SWARCH_CCU_INIT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_DeInit(void)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    retVal = CCU_SysCmdReq(CCU_CMD_DEINIT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError(0U, BRCM_SWARCH_CCU_DEINIT_PROC, retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWREQ_CCU
*/
CCU_ChanModeType CCU_GetChanMode(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;
    CCU_ChanModeType chanMode;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.chanMode = &chanMode;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CHAN_MODE, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCHANMODE_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        chanMode = CCU_CHANMODE_NONE;
    }

    return chanMode;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StartCompChannel(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_START_COMP_CHANNEL, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StopCompChannel(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_STOP_COMP_CHANNEL, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_SetCompPinProperty(CCU_ChanType aChanNum,
        CCU_PinActionType aPinAction,
        CCU_OutPinSelType aOutPin)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.pinAction = aPinAction;
    ccuIO.outPin = aOutPin;
    retVal = CCU_SysCmdReq(CCU_CMD_SET_COMP_PIN_PROPERTY, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_SetCompAbsThreshold(CCU_ChanType aChanNum,
                             CCU_ValueType aAbsValue)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.absValue = aAbsValue;
    retVal = CCU_SysCmdReq(CCU_CMD_SET_COMP_ABS_THRESHOLD, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_EnableCompCallback(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_ENABLE_COMP_CALLBACK, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_DisableCompCallback(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_DISABLE_COMP_CALLBACK, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_SetCapSignalProp(CCU_ChanType aChanNum,
                          CCU_InPinSelType aInPin,
                          CCU_EdgeType aEdge)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.inPin = aInPin;
    ccuIO.edge = aEdge;
    retVal = CCU_SysCmdReq(CCU_CMD_SET_CAP_SIGNAL_PROP, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_EnableCapCallback(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_ENABLE_CAP_CALLBACK, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_DisableCapCallback(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_DISABLE_CAP_CALLBACK, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWREQ_CCU
*/
CCU_InputStateType CCU_GetCapInputState(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;
    CCU_InputStateType inputState;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.inputState = &inputState;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CAP_INPUT_STATE, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        inputState = CCU_INPUTSTATE_IDLE;
    }

    return inputState;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StartCapTimestamp(CCU_ChanType aChanNum,
                           CCU_ValueType *const aTSBuf,
                           uint32_t aBufSize,
                           uint32_t aNotifyInterval)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.tsBuf = aTSBuf;
    ccuIO.tsBufSize = aBufSize;
    ccuIO.notifyInterval = aNotifyInterval;
    retVal = CCU_SysCmdReq(CCU_CMD_START_CAP_TIMESTAMP, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StopCapTimestamp(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_STOP_CAP_TIMESTAMP, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWREQ_CCU
*/
CCU_TSIndexType CCU_GetCapTimestampIndex(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;
    CCU_TSIndexType tsIndex;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.tsIndex = &tsIndex;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CAP_TIMESTAMP_INDEX, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        tsIndex = 0UL;
    }

    return tsIndex;
}

/**
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_ResetCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_RESET_CAP_EDGE_COUNT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_EnableCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_ENABLE_CAP_EDGE_COUNT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_DisableCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_DISABLE_CAP_EDGE_COUNT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_EnableCapEdgeDetection(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_ENABLE_CAP_EDGE_DETECTION, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_DisableCapEdgeDetection(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_DISABLE_CAP_EDGE_DETECTION, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWREQ_CCU
*/
CCU_EdgeNumberType CCU_GetCapEdgeNumbers(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;
    CCU_EdgeNumberType edgeNum;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.edgeNum = &edgeNum;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CAP_EDGE_NUMBERS, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        edgeNum = 0U;
    }

    return edgeNum;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StartCapSignalMeasurement(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_START_CAP_SIGNAL_MEASUREMENT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_StopCapSignalMeasurement(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    retVal = CCU_SysCmdReq(CCU_CMD_STOP_CAP_SIGNAL_MEASUREMENT, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_CCU
*/
CCU_ValueType CCU_GetCapTimeElapsed(CCU_ChanType aChanNum)
{
    int32_t retVal;
    CCU_IOType ccuIO;
    CCU_ValueType timeElapsed;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.timeElapsed = &timeElapsed;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CAP_TIME_ELAPSED, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
        timeElapsed = 0U;
    }

    return timeElapsed;
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWREQ_CCU
*/
void CCU_GetCapDutyCycleValues(CCU_ChanType aChanNum,
        CCU_DutyCycleType *const aDutyCycle)
{
    int32_t retVal;
    CCU_IOType ccuIO;

    ccuIO.retVal = BCM_ERR_UNKNOWN;
    ccuIO.chanNum = aChanNum;
    ccuIO.dutyCycle = aDutyCycle;
    retVal = CCU_SysCmdReq(CCU_CMD_GET_CAP_DUTY_CYCLE_VALUES, &ccuIO);
    if (BCM_ERR_OK != retVal) {
        CCU_ReportError((uint8_t)aChanNum, BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}


/** @} */
