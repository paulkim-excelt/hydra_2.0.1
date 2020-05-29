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
    @defgroup grp_ccudrv_drv_impl CCU Driver Design
    @ingroup grp_ccudrv

    @addtogroup grp_ccudrv_drv_impl
    @{
*/

#include <string.h>
#include <bcm_err.h>
#include <interrupt.h>
#include <system.h>
#include <cct_rdb.h>
#include <ccu.h>
#include <ccu_osil.h>
#include <bcm_utils.h>

/**
    @name CCU Design IDs
    @{
    @brief Design IDs for CCU driver
*/
#define BRCM_SWDSGN_CCU_DRV_CHANSTATE_TYPE                  (0xA0U) /**< @brief #CCU_DrvChanStateType */
#define BRCM_SWDSGN_CCU_DRV_SUBCHANSTATE_TYPE               (0xA1U) /**< @brief #CCU_DrvSubChanStateType */
#define BRCM_SWDSGN_CCU_DRV_CHANCBEN_TYPE                   (0xA2U) /**< @brief #CCU_DrvChanCBEnType */
#define BRCM_SWDSGN_CCU_DRV_SUBCHAN_IDX_MACRO               (0xA3U) /**< @brief #CCU_DRV_SUBCHAN_IDX_MAX */
#define BRCM_SWDSGN_CCU_DRV_CHANDEV_TYPE                    (0xA4U) /**< @brief #CCU_DrvChanDevType */
#define BRCM_SWDSGN_CCU_DRV_RWDEV_TYPE                      (0xA5U) /**< @brief #CCU_DrvRWDevType */
#define BRCM_SWDSGN_CCU_DRV_RWDEV_GLOBAL                    (0xA6U) /**< @brief #CCU_DrvRWDev */
#define BRCM_SWDSGN_CCU_DRV_REPORTERROR_PROC                (0xA7U) /**< @brief #CCU_DrvReportError */
#define BRCM_SWDSGN_CCU_DRV_SETCAPDEFSIGNALPROP_PROC        (0xA8U) /**< @brief #CCU_DrvSetCapDefSignalProp */
#define BRCM_SWDSGN_CCU_DRV_INITDEV_PROC                    (0xA9U) /**< @brief #CCU_DrvInitDev */
#define BRCM_SWDSGN_CCU_DRV_INTSETCAPSIGNALPROP_PROC        (0xAAU) /**< @brief #CCU_DrvIntSetCapSignalProp */
#define BRCM_SWDSGN_CCU_DRV_INIT_PROC                       (0xABU) /**< @brief #CCU_DrvInit */
#define BRCM_SWDSGN_CCU_DRV_DEINIT_PROC                     (0xACU) /**< @brief #CCU_DrvDeInit */
#define BRCM_SWDSGN_CCU_DRV_GETCHANMODE_PROC                (0xADU) /**< @brief #CCU_DrvGetChanMode */
#define BRCM_SWDSGN_CCU_DRV_STARTCOMPCHANNEL_PROC           (0xAEU) /**< @brief #CCU_DrvStartCompChannel */
#define BRCM_SWDSGN_CCU_DRV_STOPCOMPCHANNEL_PROC            (0xAFU) /**< @brief #CCU_DrvStopCompChannel */
#define BRCM_SWDSGN_CCU_DRV_SETCOMPPINPROPERTY_PROC         (0xB0U) /**< @brief #CCU_DrvSetCompPinProperty */
#define BRCM_SWDSGN_CCU_DRV_SETCOMPABSTHRESHOLD_PROC        (0xB1U) /**< @brief #CCU_DrvSetCompAbsThreshold */
#define BRCM_SWDSGN_CCU_DRV_ENABLECOMPCALLBACK_PROC         (0xB2U) /**< @brief #CCU_DrvEnableCompCallback */
#define BRCM_SWDSGN_CCU_DRV_DISABLECOMPCALLBACK_PROC        (0xB3U) /**< @brief #CCU_DrvDisableCompCallback */
#define BRCM_SWDSGN_CCU_DRV_SETCAPSIGNALPROP_PROC           (0xB4U) /**< @brief #CCU_DrvSetCapSignalProp */
#define BRCM_SWDSGN_CCU_DRV_ENABLECAPCALLBACK_PROC          (0xB5U) /**< @brief #CCU_DrvEnableCapCallback */
#define BRCM_SWDSGN_CCU_DRV_DISABLECAPCALLBACK_PROC         (0xB6U) /**< @brief #CCU_DrvDisableCapCallback */
#define BRCM_SWDSGN_CCU_DRV_GETCAPINPUTSTATE_PROC           (0xB7U) /**< @brief #CCU_DrvGetCapInputState */
#define BRCM_SWDSGN_CCU_DRV_STARTCAPTIMESTAMP_PROC          (0xB8U) /**< @brief #CCU_DrvStartCapTimestamp */
#define BRCM_SWDSGN_CCU_DRV_STOPCAPTIMESTAMP_PROC           (0xB9U) /**< @brief #CCU_DrvStopCapTimestamp */
#define BRCM_SWDSGN_CCU_DRV_GETCAPTIMESTAMPINDEX_PROC       (0xBAU) /**< @brief #CCU_DrvGetCapTimestampIndex */
#define BRCM_SWDSGN_CCU_DRV_RESETCAPEDGECOUNT_PROC          (0xBBU) /**< @brief #CCU_DrvResetCapEdgeCount */
#define BRCM_SWDSGN_CCU_DRV_ENABLECAPEDGECOUNT_PROC         (0xBCU) /**< @brief #CCU_DrvEnableCapEdgeCount */
#define BRCM_SWDSGN_CCU_DRV_DISABLECAPEDGECOUNT_PROC        (0xBDU) /**< @brief #CCU_DrvDisableCapEdgeCount */
#define BRCM_SWDSGN_CCU_DRV_ENABLECAPEDGEDETECTION_PROC     (0xBEU) /**< @brief #CCU_DrvEnableCapEdgeDetection */
#define BRCM_SWDSGN_CCU_DRV_DISABLECAPEDGEDETECTION_PROC    (0xBFU) /**< @brief #CCU_DrvDisableCapEdgeDetection */
#define BRCM_SWDSGN_CCU_DRV_GETCAPEDGENUMBERS_PROC          (0xC0U) /**< @brief #CCU_DrvGetCapEdgeNumbers */
#define BRCM_SWDSGN_CCU_DRV_STARTCAPSIGNALMEASUREMENT_PROC  (0xC1U) /**< @brief #CCU_DrvStartCapSignalMeasurement */
#define BRCM_SWDSGN_CCU_DRV_STOPCAPSIGNALMEASUREMENT_PROC   (0xC2U) /**< @brief #CCU_DrvStopCapSignalMeasurement */
#define BRCM_SWDSGN_CCU_DRV_GETCAPTIMEELAPSED_PROC          (0xC3U) /**< @brief #CCU_DrvGetCapTimeElapsed */
#define BRCM_SWDSGN_CCU_DRV_GETCAPDUTYCYCLEVALUES_PROC      (0xC4U) /**< @brief #CCU_DrvGetCapDutyCycleValues */
#define BRCM_SWDSGN_CCU_DRV_CAPTIMESTAMPHANDLER_PROC        (0xC5U) /**< @brief #CCU_DrvCapTimestampHandler */
#define BRCM_SWDSGN_CCU_DRV_CAPSIGMEASHANDLER_PROC          (0xC6U) /**< @brief #CCU_DrvCapSigMeasHandler */
#define BRCM_SWDSGN_CCU_DRV_CHANHANDLER_PROC                (0xC7U) /**< @brief #CCU_DrvChanHandler */
#define BRCM_SWDSGN_CCU_DRV_UDCHANDLER_PROC                 (0xC8U) /**< @brief #CCU_DrvUDCHandler */
#define BRCM_SWDSGN_CCU_DRV_IRQHANDLER_PROC                 (0xC9U) /**< @brief #CCU_DrvIRQHandler */
#define BRCM_SWDSGN_CCU_DRV_CMDHANDLER_PROC                 (0xCAU) /**< @brief #CCU_DrvCmdHandler */
#define BRCM_SWDSGN_CCU_SVCIO_TYPE                          (0xCBU) /**< @brief #CCU_SVCIOType */
#define BRCM_SWDSGN_CCU_SYSCMDHANDLER_PROC                  (0xCCU) /**< @brief #CCU_SysCmdHandler */
#define BRCM_SWDSGN_CCU_CCTCHAN_TYPE                        (0xCDU) /**< @brief #CCU_CCTChanType */
#define BRCM_SWDSGN_CCU_CCTSUBCHAN_TYPE                     (0xCEU) /**< @brief #CCU_CCTSubChanType */
#define BRCM_SWDSGN_CCU_CCTMODE_TYPE                        (0xCFU) /**< @brief #CCU_CCTModeType */
#define BRCM_SWDSGN_CCU_CCTCOMPOUTPOL_TYPE                  (0xD0U) /**< @brief #CCU_CCTCompOutPolType */
#define BRCM_SWDSGN_CCU_CCTINPINNUM_TYPE                    (0xD1U) /**< @brief #CCU_CCTInPinNumType */
#define BRCM_SWDSGN_CCU_CCTEDGE_TYPE                        (0xD2U) /**< @brief #CCU_CCTEdgeType */
#define BRCM_SWDSGN_CCU_CCTCONFIG_TYPE                      (0xD3U) /**< @brief #CCU_CCTConfigType */
#define BRCM_SWDSGN_CCU_UDC_CTRL_ALL_EVENT_MACRO            (0xD0U) /**< @brief #CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK */
#define BRCM_SWDSGN_CCU_PSC_CTRL_ALL_EVENTS_MACRO           (0xD1U) /**< @brief #CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK */
#define BRCM_SWDSGN_CCU_CCTREGS_GLOBAL                      (0xD2U) /**< @brief #CCU_CCTRegs */
#define BRCM_SWDSGN_CCU_CCTINIT_PROC                        (0xD3U) /**< @brief #CCU_CCTInit */
#define BRCM_SWDSGN_CCU_CCTDEINIT_PROC                      (0xD4U) /**< @brief #CCU_CCTDeInit */
#define BRCM_SWDSGN_CCU_CCTSETCOMPAREVALUE_PROC             (0xD5U) /**< @brief #CCU_CCTSetCompareValue */
#define BRCM_SWDSGN_CCU_CCTSELECTMODE_PROC                  (0xD6U) /**< @brief #CCU_CCTSelectMode */
#define BRCM_SWDSGN_CCU_CCTSETCOMPAREOUTPOLARITY_PROC       (0xD7U) /**< @brief #CCU_CCTSetCompareOutPolarity */
#define BRCM_SWDSGN_CCU_CCTENABLECOMPAREOUT_PROC            (0xD8U) /**< @brief #CCU_CCTEnableCompareOut */
#define BRCM_SWDSGN_CCU_CCTTRIGGERCOMPARE_PROC              (0xD9U) /**< @brief #CCU_CCTTriggerCompare */
#define BRCM_SWDSGN_CCU_CCTGETCAPTUREVALUE_PROC             (0xDAU) /**< @brief #CCU_CCTGetCaptureValue */
#define BRCM_SWDSGN_CCU_CCTSELCAPTUREINPUT_PROC             (0xDBU) /**< @brief #CCU_CCTSelCaptureInput */
#define BRCM_SWDSGN_CCU_CCTTRIGGERCAPTURE_PROC              (0xDCU) /**< @brief #CCU_CCTTriggerCapture */
#define BRCM_SWDSGN_CCU_CCTENABLECAPCOMPINTERRUPT_PROC      (0xDDU) /**< @brief #CCU_CCTEnableCapCompInterrupt */
#define BRCM_SWDSGN_CCU_CCTDISABLECAPCOMPINTERRUPT_PROC     (0xDEU) /**< @brief #CCU_CCTDisableCapCompInterrupt */
#define BRCM_SWDSGN_CCU_CCTENABLEUDCINTERRUPT_PROC          (0xDFU) /**< @brief #CCU_CCTEnableUDCInterrupt */
#define BRCM_SWDSGN_CCU_CCTDISABLEUDCINTERRUPT_PROC         (0xE0U) /**< @brief #CCU_CCTDisableUDCInterrupt */
#define BRCM_SWDSGN_CCU_CCTDISABLEETRINTERRUPT_PROC         (0xE1U) /**< @brief #CCU_CCTDisableETRInterrupt */
#define BRCM_SWDSGN_CCU_CCTIRQHANDLER_PROC                  (0xE2U) /**< @brief #CCU_CCTIRQHandler */
/** @} */

/**
    @name CCU_DrvChanStateType
    @{
    @brief CCU_DrvChanStateType

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvChanStateType;        /**< @brief CCU_DrvChanStateType */
#define CCU_DRV_CHANSTATE_STOPPED      (0x1UL) /**< @brief Stopped */
#define CCU_DRV_CHANSTATE_RUNNING      (0x2UL) /**< @brief Running */
/** @} */

/**
    @name CCU_DrvSubChanStateType
    @{
    @brief CCU_DrvSubChanStateType

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvSubChanStateType;    /**< @brief CCU_DrvSubChanStateType */
#define CCU_DRV_SUBCHANSTATE_IN_USE   (0x1UL) /**< @brief In use */
#define CCU_DRV_SUBCHANSTATE_NOT_USE  (0x2UL) /**< @brief Not use */
/** @} */

/**
    @name CCU_DrvChanCBEnType
    @{
    @brief CCU_DrvChanCBEnType

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_DrvChanCBEnType; /**< @brief CCU_DrvChanCBEnType */
/** @} */

/**
    @name CCU Driver Sub Channel Index
    @{
    @brief CCU Sub channel Index

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
#define CCU_DRV_SUBCHAN_IDX_MAX        (2)     /**< @brief Maximum Index */
#define CCU_DRV_SUBCHAN_IDX_A          (0x0UL) /**< @brief Index 0 */
#define CCU_DRV_SUBCHAN_IDX_B          (0x1UL) /**< @brief Index 1 */
/** @} */

/**
    @brief CCT channel type

    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef uint32_t CCU_CCTChanType;

/**
    @{
    @brief CCT  type

    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTSubChanType;
#define CCU_CCTSUBCHAN_A          (1UL) /**< @brief Subchannel A */
#define CCU_CCTSUBCHAN_B          (2UL) /**< @brief Subchannel B */
#define CCU_CCTSUBCHAN_BOTH       (3UL) /**< @brief Subchannel A and B */
/** @} */

/**
    @{
    @brief CCT mode type
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTModeType;
#define CCU_CCTMODE_CAPTURE        (1UL) /**< @brief Capture */
#define CCU_CCTMODE_COMPARE        (2UL) /**< @brief Compare */
/** @} */

/**
    @{
    @brief CCT polarity type
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTCompOutPolType;
#define CCU_CCTCOMPOUTPOL_HIGH   (1UL) /**< @brief High */
#define CCU_CCTCOMPOUTPOL_LOW    (2UL) /**< @brief Low */
/** @} */

/**
    @{
    @brief CCT input pin type
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTInPinNumType;
#define CCU_CCTINPINNUM_0            (0UL) /**< @brief 0 */
#define CCU_CCTINPINNUM_1            (1UL) /**< @brief 1 */
#define CCU_CCTINPINNUM_2            (2UL) /**< @brief 2 */
#define CCU_CCTINPINNUM_3            (3UL) /**< @brief 3 */
/** @} */

/**
    @{
    @brief CCT edge type
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef uint32_t CCU_CCTEdgeType;
#define CCU_CCTEDGE_RISING         (0UL) /**< @brief Rising Edge */
#define CCU_CCTEDGE_FALLING        (1UL) /**< @brief Falling Edge */
/** @} */

/**
    @{
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
typedef struct _CCU_CCTConfigType {
    uint16_t    cntrMaxValue;   /**< Up-down counter max value */
    uint16_t    preScaleRatio;  /**< Pre-scale ratio */
} CCU_CCTConfigType;
/** @} */

/**
    @name CCU_DrvChanDevType
    @{
    @brief CCU Channel Device Type

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_DrvChanDevType{                                /**< @brief CCU_DrvChanDevType */
    CCU_ChanModeType        mode;                                   /**< @brief mode                */
    CCU_DrvChanStateType    state;                                 /**< @brief state               */
    CCU_DrvSubChanStateType subChanState[CCU_DRV_SUBCHAN_IDX_MAX]; /**< @brief subChanState        */
    uint32_t                isCapSigPropSet;                        /**< @brief isCapSigPropSet     */
    /* Capture related parameters */
    CCU_CapModeType         capDefMode;                             /**< @brief capDefMode          */
    CCU_InPinSelType        capDefPin;                              /**< @brief capDefPin           */
    CCU_EdgeType            capDefEdge;                             /**< @brief capDefEdge          */
    CCU_CapModeType         capMode;                                /**< @brief capMode             */
    CCU_InPinSelType        capPin;                                 /**< @brief capPin              */
    CCU_EdgeType            capEdge;                                /**< @brief capEdge             */
    CCU_InputStateType      inputState;                             /**< @brief inputState          */
    CCU_DrvChanCBEnType     isCbEn;                                /**< @brief isCbEn              */
    CCU_ValueType *         TSBuf;                                  /**< @brief TSBuf               */
    uint32_t                bufSize;                                /**< @brief bufSize             */
    uint32_t                notifyInterval;                         /**< @brief notifyInterval      */
    CCU_TSIndexType         tsWrIndex;                              /**< @brief tsWrIndex           */
    CCU_EdgeNumberType      edgeCnt;                                /**< @brief edgeCnt             */
    CCU_TimeStampBufType    TSBufProp;                              /**< @brief TSBufProp           */
    CCU_SignalPropertyType  sigMeasProp;                            /**< @brief sigMeasProp         */
    CCU_DutyCycleType       dutyCycle;                              /**< @brief dutyCycle           */
    CCU_CallbackType        timestampCB;                            /**< @brief timestampCB         */
    CCU_CallbackType        edgeDetectCB;                           /**< @brief edgeDetectCB        */
    uint32_t                fallingEdgeIdx;                         /**< @brief fallingEdgeIdx      */
    uint32_t                risingEdgeIdx;                          /**< @brief risingEdgeIdx       */
    uint64_t                fallingEdgeTS[2UL];                     /**< @brief fallingEdgeTS       */
    uint64_t                risingEdgeTS[2UL];                      /**< @brief risingEdgeTS        */
    /* Compare related parametes */
    CCU_CallbackType        compCB;                                 /**< @brief compCB              */
    CCU_OutPinSelType       compOutPin;                             /**< @brief compOutPin          */
    CCU_PinActionType       pinAction;                              /**< @brief pinAction           */
    CCU_ValueType           compThreshold;                          /**< @brief compThreshold       */
    CCU_ValueType           compDefThreshold;                       /**< @brief compDefThreshold    */
    CCU_ValueType           elapsedTime;                            /**< @brief elapsedTime         */
} CCU_DrvChanDevType;
/** @} */

/**
    @name CCU_DrvRWDevType
    @{
    @brief CCU read-write device structure

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
typedef struct _CCU_DrvRWDevType {                         /**< @brief CCU_DrvRWDevType */
    uint32_t isInit;                                        /**< @brief isInit */
    uint32_t cntrWrapCnt;                                   /**< @brief cntrWrapCnt */
    CCU_DrvChanDevType         chan[CCT_MAX_CHAN_CNT];     /**< @brief chan */
} CCU_DrvRWDevType;
/** @} */

/**
    @brief CCT UDC CTRL All Event Mask

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
#define CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK        (0x0000003FUL)

/**
    @brief CCT PSC CTRL All Event Mask

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
#define CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK   (0x003FFFFFUL)

/**
    @brief CCT Register base address

    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static CCT_RDBType* const CCU_CCTRegs = (CCT_RDBType * const)CCT_BASE;

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTInit(CCU_CCTConfigType *const aConfig)
{
    /* Reset and Enable CCT
     * NOTE: Disable-Enable CCT doesn't reset preconfigured register
     * values */
    CCU_CCTRegs->ctrla &= ~CCT_CTRLA_EN_MASK;
    CCU_CCTRegs->ctrla |= CCT_CTRLA_EN_MASK;

    /* Disable UDC trigger events, Set UDC value, unidirectional
     * counter and trigger it */
    CCU_CCTRegs->udc_ctrl |= CCU_UDC_CTRL_CCT_MASK_ALL_EVENT_MASK;
    CCU_CCTRegs->udc_ctrl &= ~CCT_UDC_CTRL_ARR_MASK;
    CCU_CCTRegs->udc_ctrl |= ((((uint32_t)aConfig->cntrMaxValue)
                << CCT_UDC_CTRL_ARR_SHIFT) &  CCT_UDC_CTRL_ARR_MASK);
    CCU_CCTRegs->udc_ctrl &= ~CCT_UDC_CTRL_UDEN_MASK;
    CCU_CCTRegs->udc_trig |= CCT_UDC_TRIG_TRIG_MASK;

    /* Disable PSC trigger events */
    CCU_CCTRegs->psc_ctrl = CCU_PSC_CTRL_CCT_PSC_MASK_ALL_EVENTS_MASK;

    /* Set PSC ratio */
    CCU_CCTRegs->psc_ratio &= ~CCT_PSC_RATIO_RATIO_MASK;
    CCU_CCTRegs->psc_ratio |= ((((uint32_t)aConfig->preScaleRatio)
                << CCT_PSC_RATIO_RATIO_SHIFT) & CCT_PSC_RATIO_RATIO_MASK);

    /* Trigger to load new PSC ratio */
    CCU_CCTRegs->psc_trig |= CCT_PSC_TRIG_TRIG_MASK;
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDeInit(void)
{
    /* Disable CCT */
    CCU_CCTRegs->ctrla &= ~CCT_CTRLA_EN_MASK;

}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSetCompareValue(CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    uint16_t aValue)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val &= ~CCT_CCR_VAL_A_VAL_MASK;
        CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val |=
            ((((uint32_t)aValue) << CCT_CCR_VAL_A_VAL_SHIFT) & CCT_CCR_VAL_A_VAL_MASK);
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val &= ~CCT_CCR_VAL_B_VAL_MASK;
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val |=
                    ((((uint32_t)aValue) << CCT_CCR_VAL_B_VAL_SHIFT)
                     & CCT_CCR_VAL_B_VAL_MASK);
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSelectMode(CCU_CCTChanType aChanNum,
                               CCU_CCTSubChanType aSubChan,
                               CCU_CCTModeType aMode)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_CCSEL_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_CCSEL_MASK;
        }
    }

    if (0UL != mask) {
        switch (aMode) {
        case CCU_CCTMODE_CAPTURE:
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
            break;
        case CCU_CCTMODE_COMPARE:
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        default:
            break;
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSetCompareOutPolarity(CCU_CCTChanType aChanNum,
                                          CCU_CCTSubChanType aSubChan,
                                          CCU_CCTCompOutPolType aPolarity)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_POL_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_POL_MASK;
        }
    }

    if (0UL != mask) {
        switch (aPolarity){
        case CCU_CCTCOMPOUTPOL_HIGH:
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
            break;
        case CCU_CCTCOMPOUTPOL_LOW:
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
            break;
        default:
            break;
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTEnableCompareOut(CCU_CCTChanType aChanNum,
                                     CCU_CCTSubChanType aSubChan,
                                     uint32_t aEnable)
{
    uint32_t mask = 0UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        mask = CCT_CCR_CTRL_A_OUT_OEB_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            mask = CCT_CCR_CTRL_B_OUT_OEB_MASK;
        }
    }

    if (0UL != mask) {
        if (TRUE == aEnable) {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl &= ~mask;
        } else {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl |= mask;
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTTriggerCompare(CCU_CCTChanType aChanNum,
                                   CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_A_TRIG_MASK;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_B_TRIG_MASK;
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTGetCaptureValue(CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    uint16_t *const aValue)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        *aValue = (uint16_t)(((CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val) & CCT_CCR_VAL_A_VAL_MASK)
            >> CCT_CCR_VAL_A_VAL_SHIFT);
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            *aValue =
                (uint16_t)(((CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_val) & CCT_CCR_VAL_B_VAL_MASK)
                >> CCT_CCR_VAL_B_VAL_SHIFT);
        } else {
            *aValue = 0U;
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTSelCaptureInput(CCU_CCTChanType aChanNum,
                                    CCU_CCTSubChanType aSubChan,
                                    CCU_CCTInPinNumType aInPinNum,
                                    CCU_CCTEdgeType aEdge,
                                    uint32_t aEnable)
{
    uint32_t shift = 32UL;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        shift = CCT_CCR_CTRL_A_MASK_IN0_RE_SHIFT;
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            shift = CCT_CCR_CTRL_B_MASK_IN0_RE_SHIFT;
        }
    }

    if (32UL > shift) {
        if (TRUE == aEnable) {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl &=
                ~(0x1UL << (shift + (2UL * aInPinNum) + aEdge));
        } else {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_ctrl |=
                (0x1UL << (shift + (2UL * aInPinNum) + aEdge));
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTTriggerCapture(CCU_CCTChanType aChanNum,
                                   CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_BOTH == aSubChan) {
        CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_trig |=
            (CCT_CCR_TRIG_A_TRIG_MASK | CCT_CCR_TRIG_B_TRIG_MASK);
    } else {
        if (CCU_CCTSUBCHAN_A == aSubChan) {
            CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_A_TRIG_MASK;
        } else {
            if (CCU_CCTSUBCHAN_B == aSubChan) {
                CCU_CCTRegs->cct_ccr_regs[aChanNum].ccr0_trig |= CCT_CCR_TRIG_B_TRIG_MASK;
            }
        }
    }

}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTEnableCapCompInterrupt(CCU_CCTChanType aChanNum,
                                           CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs->irq_ctrl |=
                    (CCT_IRQ_CTRL_CCR0_A_EN_MASK << (2UL * aChanNum));
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs->irq_ctrl |=
                    (CCT_IRQ_CTRL_CCR0_B_EN_MASK << (2UL * aChanNum));
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDisableCapCompInterrupt(CCU_CCTChanType aChanNum,
                                            CCU_CCTSubChanType aSubChan)
{
    if (CCU_CCTSUBCHAN_A == aSubChan) {
        CCU_CCTRegs->irq_ctrl &=
                    ~(CCT_IRQ_CTRL_CCR0_A_EN_MASK << (2UL * aChanNum));
    } else {
        if (CCU_CCTSUBCHAN_B == aSubChan) {
            CCU_CCTRegs->irq_ctrl &=
                    ~(CCT_IRQ_CTRL_CCR0_B_EN_MASK << (2UL * aChanNum));
        }
    }
}

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static  void CCU_CCTEnableUDCInterrupt(void)
{
    CCU_CCTRegs->irq_ctrl |= CCT_IRQ_CTRL_UDC_EN_MASK;
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDisableUDCInterrupt(void)
{
    CCU_CCTRegs->irq_ctrl &= ~CCT_IRQ_CTRL_UDC_EN_MASK;
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_CCTDisableETRInterrupt(CCU_CCTEdgeType aEdge)
{
    switch (aEdge) {
    case CCU_CCTEDGE_RISING:
        CCU_CCTRegs->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_RE_EN_MASK;
        break;
    case CCU_CCTEDGE_FALLING:
        CCU_CCTRegs->irq_ctrl &= ~CCT_IRQ_CTRL_ETR_FE_EN_MASK;
        break;
    default:
        break;
    }
}

/**
    @name CCU_DrvRWDev

    @brief CCU read-write device

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
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU_CONFIGURATION
*/
static CCU_DrvRWDevType CCU_DrvRWDev COMP_SECTION(".data.drivers") =
    {
        .isInit = FALSE,
#if (CCT_MAX_CHAN_CNT == 0)
#error "CCT_MAX_CHAN_CNT == 0"
#endif
#if (CCT_MAX_CHAN_CNT > 0)
        .chan[0UL].mode = CCU_CHANMODE_NONE,
        .chan[0UL].state = CCU_DRV_CHANSTATE_STOPPED,
        .chan[0UL].inputState = CCU_INPUTSTATE_IDLE,
#endif
#if (CCT_MAX_CHAN_CNT > 1)
        .chan[1UL].mode = CCU_CHANMODE_NONE,
        .chan[1UL].state = CCU_DRV_CHANSTATE_STOPPED,
        .chan[1UL].inputState = CCU_INPUTSTATE_IDLE,
#endif
#if (CCT_MAX_CHAN_CNT > 2)
        .chan[2UL].mode = CCU_CHANMODE_NONE,
        .chan[2UL].state = CCU_DRV_CHANSTATE_STOPPED,
        .chan[2UL].inputState = CCU_INPUTSTATE_IDLE,
#endif
#if (CCT_MAX_CHAN_CNT > 3)
        .chan[3UL].mode = CCU_CHANMODE_NONE,
        .chan[3UL].state = CCU_DRV_CHANSTATE_STOPPED,
        .chan[3UL].inputState = CCU_INPUTSTATE_IDLE,
#endif
#if (CCT_MAX_CHAN_CNT > 4)
#error "CCT_MAX_CHAN_CNT > 4"
#endif
    };

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
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
static void CCU_DrvReportError(uint8_t aInstanceID, uint8_t aApiID,
                                int32_t aErr, uint32_t aVal0, uint32_t aVal1, uint32_t aVal2,
                                uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CCT_ID, aInstanceID, aApiID, aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvSetCapDefSignalProp(CCU_ChanType aChanNum)
{
    CCU_InPinSelType inPin = CCU_DrvRWDev.chan[aChanNum].capDefPin;
    CCU_EdgeType edge = CCU_DrvRWDev.chan[aChanNum].capDefEdge;

    /* Set capture signal and edge */
    switch (edge) {
    case CCU_EDGE_RISING:
        CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, TRUE);
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev.chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    case CCU_EDGE_FALLING:
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev.chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_RISING, TRUE);
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        } else {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    case CCU_EDGE_BOTH:
        CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, TRUE);
        if ((CCU_CAPMODE_EDGE_COUNT == CCU_DrvRWDev.chan[aChanNum].capMode)
           || (CCU_CAPMODE_EDGE_DETECT == CCU_DrvRWDev.chan[aChanNum].capMode)) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        } else {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    default:
        break;
    }
}

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvInitDev(void)
{
    CCU_DrvRWDev.cntrWrapCnt = 0UL;
#if (CCT_MAX_CHAN_CNT == 0)
#error "CCT_MAX_CHAN_CNT == 0"
#endif
#if (CCT_MAX_CHAN_CNT > 0)
    CCU_DrvRWDev.chan[0UL].isCapSigPropSet = FALSE;
    CCU_DrvRWDev.chan[0UL].edgeCnt = 0UL;
    CCU_DrvRWDev.chan[0UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[0UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev.chan[0UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev.chan[0UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[0UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev.chan[0UL].fallingEdgeTS[1UL] = 0ULL;
#endif
#if (CCT_MAX_CHAN_CNT > 1)
    CCU_DrvRWDev.chan[1UL].isCapSigPropSet = FALSE;
    CCU_DrvRWDev.chan[1UL].edgeCnt = 0UL;
    CCU_DrvRWDev.chan[1UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[1UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev.chan[1UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev.chan[1UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[1UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev.chan[1UL].fallingEdgeTS[1UL] = 0ULL;
#endif
#if (CCT_MAX_CHAN_CNT > 2)
    CCU_DrvRWDev.chan[2UL].isCapSigPropSet = FALSE;
    CCU_DrvRWDev.chan[2UL].edgeCnt = 0UL;
    CCU_DrvRWDev.chan[2UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[2UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev.chan[2UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev.chan[2UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[2UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev.chan[2UL].fallingEdgeTS[1UL] = 0ULL;
#endif
#if (CCT_MAX_CHAN_CNT > 3)
    CCU_DrvRWDev.chan[3UL].isCapSigPropSet = FALSE;
    CCU_DrvRWDev.chan[3UL].edgeCnt = 0UL;
    CCU_DrvRWDev.chan[3UL].risingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[3UL].risingEdgeTS[0UL] = 0UL;
    CCU_DrvRWDev.chan[3UL].risingEdgeTS[1UL] = 0UL;
    CCU_DrvRWDev.chan[3UL].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[3UL].fallingEdgeTS[0UL] = 0ULL;
    CCU_DrvRWDev.chan[3UL].fallingEdgeTS[1UL] = 0ULL;
#endif
#if (CCT_MAX_CHAN_CNT > 4)
#error "CCT_MAX_CHAN_CNT > 4"
#endif
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvIntSetCapSignalProp(CCU_ChanType aChanNum)
{
    CCU_EdgeType edge;
    CCU_InPinSelType inPin;

    inPin = CCU_DrvRWDev.chan[aChanNum].capPin;
    edge = CCU_DrvRWDev.chan[aChanNum].capEdge;

    /* Set capture signal and edge */
    switch (edge) {
    case CCU_EDGE_RISING:
        CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, TRUE);
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev.chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    case CCU_EDGE_FALLING:
        if (CCU_CAPMODE_SIGNAL_MEASURE == CCU_DrvRWDev.chan[aChanNum].capMode) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_RISING, TRUE);
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        } else {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    default:
        CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                            inPin, CCU_CCTEDGE_RISING, TRUE);
        if ((CCU_CAPMODE_EDGE_COUNT == CCU_DrvRWDev.chan[aChanNum].capMode)
           || (CCU_CAPMODE_EDGE_DETECT == CCU_DrvRWDev.chan[aChanNum].capMode)) {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_A,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        } else {
            CCU_CCTSelCaptureInput(aChanNum, CCU_CCTSUBCHAN_B,
                    inPin, CCU_CCTEDGE_FALLING, TRUE);
        }
        break;
    }
}

/**
    @trace #BRCM_SWARCH_CCU_INIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvInit(const CCU_ConfigType *const aConfig)
{
    int32_t retVal = BCM_ERR_OK;
    uint32_t i;
    CCU_CCTConfigType cctCfg;
    CCU_ChanCfgTblType * chanCfgTbl;
    uint32_t chanNum = 0UL;

    if (NULL == aConfig) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (TRUE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    CCU_DrvInitDev();

    cctCfg.cntrMaxValue = aConfig->cntrMaxValue;
    cctCfg.preScaleRatio = aConfig->preScaleRatio;
    CCU_CCTInit(&cctCfg);
    CCU_CCTEnableUDCInterrupt();

    chanCfgTbl = aConfig->chanCfgTbl;

    if (NULL != chanCfgTbl) {
        for (i = 0UL; i < aConfig->chanCfgTblSz; i++) {
            if (chanCfgTbl[i].chanNum >= CCT_MAX_CHAN_CNT) {
                continue;
            }
            chanNum = chanCfgTbl[i].chanNum;
            switch (chanCfgTbl[i].chanMode) {
            case CCU_CHANMODE_CAPTURE:
                CCU_DrvRWDev.chan[chanNum].mode = chanCfgTbl[i].chanMode;
                CCU_DrvRWDev.chan[chanNum].capDefMode = chanCfgTbl[i].capCfg.mode;
                CCU_DrvRWDev.chan[chanNum].capDefPin = chanCfgTbl[i].capCfg.pin;
                CCU_DrvRWDev.chan[chanNum].capDefEdge = chanCfgTbl[i].capCfg.edge;
                CCU_DrvRWDev.chan[chanNum].TSBufProp = chanCfgTbl[i].capCfg.TSBufProp;
                CCU_DrvRWDev.chan[chanNum].sigMeasProp = chanCfgTbl[i].capCfg.sigMeasProp;
                CCU_DrvRWDev.chan[chanNum].timestampCB = chanCfgTbl[i].capCfg.timestampCB;
                CCU_DrvRWDev.chan[chanNum].edgeDetectCB = chanCfgTbl[i].capCfg.edgeDetectCB;
                break;
            case CCU_CHANMODE_COMPARE:
                CCU_DrvRWDev.chan[chanNum].mode = chanCfgTbl[i].chanMode;
                CCU_DrvRWDev.chan[chanNum].compCB = chanCfgTbl[i].compCfg.compCB;
                CCU_DrvRWDev.chan[chanNum].compDefThreshold =
                                    chanCfgTbl[i].compCfg.compThreshold;

                break;
            default:
                break;
            }
        }
    }

    CCU_DrvRWDev.isInit = TRUE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)chanNum, BRCM_SWDSGN_CCU_DRV_INIT_PROC, retVal, 0UL, 0UL, 0UL,
                0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_DEINIT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvDeInit(void)
{
    uint32_t i;

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        CCU_DrvReportError(0U, BRCM_SWDSGN_CCU_DRV_DEINIT_PROC, BCM_ERR_UNINIT, 0UL,
                0UL, 0UL, 0UL);
        goto err;
    }

    /* Check state of all the channels */
    for (i = 0UL; i < CCT_MAX_CHAN_CNT; i++) {
        if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[i].state) {
            CCU_DrvReportError(0U, BRCM_SWDSGN_CCU_DRV_DEINIT_PROC,
                    BCM_ERR_INVAL_STATE, 0UL, 0UL, 0UL, 0UL);
            goto err;
        }
    }

    /* Disable all the interrupts */
    for (i = 0UL; i < CCT_MAX_CHAN_CNT; i++) {
        CCU_CCTDisableCapCompInterrupt(i, CCU_CCTSUBCHAN_A);
        CCU_CCTDisableCapCompInterrupt(i, CCU_CCTSUBCHAN_B);
    }
    CCU_CCTDisableUDCInterrupt();
    CCU_CCTDisableETRInterrupt(CCU_CCTEDGE_RISING);
    CCU_CCTDisableETRInterrupt(CCU_CCTEDGE_FALLING);

    /* De-intialize CCT */
    CCU_CCTDeInit();

    CCU_DrvRWDev.isInit = FALSE;

err:
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_GETCHANMODE_PROC
    @trace #BRCM_SWREQ_CCU
*/
int32_t CCU_DrvGetChanMode(CCU_ChanType aChanNum,
                            CCU_ChanModeType *const aChanMode)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CCU_CHAN_MAX < aChanNum)
            || (NULL == aChanMode)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (TRUE != CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    *aChanMode = CCU_DrvRWDev.chan[aChanNum].mode;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCHANMODE_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStartCompChannel(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan = 0UL;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (TRUE != CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A]
            == CCU_DRV_SUBCHANSTATE_IN_USE) {
        subChan = CCU_CCTSUBCHAN_A;
    } else {
        if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B]
                == CCU_DRV_SUBCHANSTATE_IN_USE) {
            subChan = CCU_CCTSUBCHAN_B;
        } else {
            subChan = CCU_CCTSUBCHAN_A;
            CCU_CCTSetCompareValue(aChanNum, subChan,
                    CCU_DrvRWDev.chan[aChanNum].compDefThreshold);
        }
    }

    /* Enable interupt for the channel */
    CCU_CCTEnableCapCompInterrupt(aChanNum, subChan);

    /* Set Compare mode */
    CCU_CCTSelectMode(aChanNum, subChan, CCU_CCTMODE_COMPARE);

    /* Set Compare trigger bit */
    CCU_CCTTriggerCompare(aChanNum, subChan);

    /* mark the channel state to running */
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STARTCOMPCHANNEL_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCOMPCHANNEL_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStopCompChannel(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan = 0UL;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A]
            == CCU_DRV_SUBCHANSTATE_IN_USE) {
        subChan = CCU_CCTSUBCHAN_A;
    } else {
        if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B]
                == CCU_DRV_SUBCHANSTATE_IN_USE) {
            subChan = CCU_CCTSUBCHAN_B;
        }
    }

    if (0UL != subChan) {
        /* Disable interrupt for the channel */
        CCU_CCTDisableCapCompInterrupt(aChanNum, subChan);

        /* Halt the compare operation by changing mode */
        CCU_CCTSelectMode(aChanNum, subChan, CCU_CCTMODE_CAPTURE);
    }

    /* Mark sub-channel as not in use */
    CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] =
                                                CCU_DRV_SUBCHANSTATE_NOT_USE;
    CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] =
                                                CCU_DRV_SUBCHANSTATE_NOT_USE;

    /* mark the channel state to stop */
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STOPCOMPCHANNEL_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPPINPROPERTY_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvSetCompPinProperty(CCU_ChanType aChanNum,
                                       CCU_PinActionType aPinAction,
                                       CCU_OutPinSelType aOutPin)
{
    int32_t retVal = BCM_ERR_OK;
    CCU_CCTSubChanType subChan;

    if ((CCU_CHAN_MAX < aChanNum)
            || (CCU_PINACTION_SET_LOW > aPinAction)
            || (CCU_PINACTION_DISABLE < aPinAction)
            || (CCU_OUTPINSEL_OUT > aOutPin)
            || (CCU_OUTPINSEL_OUT_N < aOutPin)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    if (CCU_OUTPINSEL_OUT == aOutPin) {
        subChan = CCU_CCTSUBCHAN_A;
        CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] =
                                                CCU_DRV_SUBCHANSTATE_IN_USE;
    } else {
        subChan = CCU_CCTSUBCHAN_B;
        CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] =
                                                CCU_DRV_SUBCHANSTATE_IN_USE;
    }

    switch (aPinAction) {
    case CCU_PINACTION_SET_LOW:
        CCU_CCTSetCompareOutPolarity(aChanNum, subChan, CCU_CCTCOMPOUTPOL_LOW);
        CCU_CCTEnableCompareOut(aChanNum, subChan, TRUE);
        break;
    case CCU_PINACTION_SET_HIGH:
        CCU_CCTSetCompareOutPolarity(aChanNum, subChan, CCU_CCTCOMPOUTPOL_HIGH);
        CCU_CCTEnableCompareOut(aChanNum, subChan, TRUE);
        break;
    case CCU_PINACTION_TOGGLE:
        /* TODO: What to do for this state? */
        break;
    default:
        /* Disable out enable, Pin will be HIGH */
        CCU_CCTEnableCompareOut(aChanNum, subChan, FALSE);
        break;
    }

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_SETCOMPPINPROPERTY_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_SETCOMPABSTHRESHOLD_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvSetCompAbsThreshold(CCU_ChanType aChanNum,
                                        CCU_ValueType aAbsValue)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    /* Setting absolute threshold value */
    if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_A] ==
                                            CCU_DRV_SUBCHANSTATE_IN_USE) {
        CCU_CCTSetCompareValue(aChanNum, CCU_CCTSUBCHAN_A, aAbsValue);
    } else {
        if (CCU_DrvRWDev.chan[aChanNum].subChanState[CCU_DRV_SUBCHAN_IDX_B] ==
                                            CCU_DRV_SUBCHANSTATE_IN_USE) {
            CCU_CCTSetCompareValue(aChanNum, CCU_CCTSUBCHAN_B, aAbsValue);
        }
    }

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_SETCOMPABSTHRESHOLD_PROC,
                        retVal, 0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvEnableCompCallback(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].isCbEn = TRUE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_ENABLECOMPCALLBACK_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECOMPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvDisableCompCallback(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_COMPARE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].isCbEn = FALSE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_DISABLECOMPCALLBACK_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_SETCAPSIGNALPROP_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvSetCapSignalProp(CCU_ChanType aChanNum,
                                     CCU_InPinSelType aInPin,
                                     CCU_EdgeType aEdge)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CCU_CHAN_MAX < aChanNum)
        || (CCU_INPINSEL_IN0 >= aInPin)
        || (CCU_INPINSEL_IN3 <= aInPin)
        || (CCU_EDGE_RISING > aEdge)
        || (CCU_EDGE_BOTH < aEdge)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capPin = aInPin;
    CCU_DrvRWDev.chan[aChanNum].capEdge = aEdge;
    CCU_DrvRWDev.chan[aChanNum].isCapSigPropSet = TRUE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_SETCAPSIGNALPROP_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvEnableCapCallback(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].isCbEn = TRUE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_ENABLECAPCALLBACK_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPCALLBACK_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvDisableCapCallback(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].isCbEn = FALSE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_DISABLECAPCALLBACK_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
    return;
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPINPUTSTATE_PROC
    @trace #BRCM_SWREQ_CCU
*/
static int32_t CCU_DrvGetCapInputState(CCU_ChanType aChanNum,
                                        CCU_InputStateType *const aInputState)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    *aInputState = CCU_DrvRWDev.chan[aChanNum].inputState;
    CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCAPINPUTSTATE_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStartCapTimestamp(CCU_ChanType aChanNum,
                                      CCU_ValueType * const aTSBuf,
                                      uint32_t aBufSize,
                                      uint32_t aNotifyInterval)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CCU_CHAN_MAX < aChanNum) || (NULL == aTSBuf)
        || (0UL == aBufSize)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_TIMESTAMP;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
    CCU_DrvRWDev.chan[aChanNum].tsWrIndex = 0UL;
    CCU_DrvRWDev.chan[aChanNum].TSBuf = aTSBuf;
    CCU_DrvRWDev.chan[aChanNum].bufSize = aBufSize;
    CCU_DrvRWDev.chan[aChanNum].notifyInterval = aNotifyInterval;

    if (TRUE == CCU_DrvRWDev.chan[aChanNum].isCapSigPropSet) {
        CCU_DrvIntSetCapSignalProp(aChanNum);
    } else {
        CCU_DrvSetCapDefSignalProp(aChanNum);
    }

    switch (CCU_DrvRWDev.chan[aChanNum].capEdge) {
    case CCU_EDGE_RISING:
        CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
        CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_A);
        break;
    case CCU_EDGE_FALLING:
        CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
        CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_A);
        break;
    case CCU_EDGE_BOTH:
        CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
        CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_B);
        CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
        CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_CAPTURE);
        CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_BOTH);
        break;
    default:
        break;
    }
err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STARTCAPTIMESTAMP_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCAPTIMESTAMP_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStopCapTimestamp(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_TIMESTAMP != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_NONE;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
    CCU_DrvRWDev.chan[aChanNum].TSBuf = NULL;
    CCU_DrvRWDev.chan[aChanNum].bufSize = 0UL;

    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_B);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_COMPARE);
err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STOPCAPTIMESTAMP_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
}


/**
    @trace #BRCM_SWARCH_CCU_GETCAPTIMESTAMPINDEX_PROC
    @trace #BRCM_SWREQ_CCU
*/
static int32_t CCU_DrvGetCapTimestampIndex(CCU_ChanType aChanNum,
                                            CCU_TSIndexType *const aTSIndex)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_TIMESTAMP != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    *aTSIndex = CCU_DrvRWDev.chan[aChanNum].tsWrIndex;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCAPTIMESTAMPINDEX_PROC,
                retVal, 0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CCU_RESETCAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvResetCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].edgeCnt = 0UL;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_RESETCAPEDGECOUNT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvEnableCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_EDGE_COUNT;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
    if (TRUE == CCU_DrvRWDev.chan[aChanNum].isCapSigPropSet) {
        CCU_DrvIntSetCapSignalProp(aChanNum);
    } else {
        CCU_DrvSetCapDefSignalProp(aChanNum);
    }
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
    CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_A);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_ENABLECAPEDGECOUNT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGECOUNT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvDisableCapEdgeCount(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_EDGE_COUNT != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_NONE;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_DISABLECAPEDGECOUNT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_ENABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvEnableCapEdgeDetection(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_EDGE_DETECT;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
    if (TRUE == CCU_DrvRWDev.chan[aChanNum].isCapSigPropSet) {
        CCU_DrvIntSetCapSignalProp(aChanNum);
    } else {
        CCU_DrvSetCapDefSignalProp(aChanNum);
    }
    CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
    CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_A);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_ENABLECAPEDGEDETECTION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DISABLECAPEDGEDETECTION_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvDisableCapEdgeDetection(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_EDGE_DETECT != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_NONE;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_DISABLECAPEDGEDETECTION_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPEDGENUMBERS_PROC
    @trace #BRCM_SWREQ_CCU
*/
static int32_t CCU_DrvGetCapEdgeNumbers(CCU_ChanType aChanNum,
                                         CCU_EdgeNumberType *const aEdgeNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if (CCU_CAPMODE_EDGE_COUNT != CCU_DrvRWDev.chan[aChanNum].capMode) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    *aEdgeNum = CCU_DrvRWDev.chan[aChanNum].edgeCnt;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCAPEDGENUMBERS_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CCU_STARTCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStartCapSignalMeasurement(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_NONE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_SIGNAL_MEASURE;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_RUNNING;
    CCU_DrvRWDev.chan[aChanNum].fallingEdgeIdx = 0UL;
    CCU_DrvRWDev.chan[aChanNum].risingEdgeIdx = 0UL;
    if (TRUE == CCU_DrvRWDev.chan[aChanNum].isCapSigPropSet) {
        CCU_DrvIntSetCapSignalProp(aChanNum);
    } else {
        CCU_DrvSetCapDefSignalProp(aChanNum);
    }
    CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTEnableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_B);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_CAPTURE);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_CAPTURE);
    CCU_CCTTriggerCapture(aChanNum, CCU_CCTSUBCHAN_BOTH);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STARTCAPSIGNALMEASUREMENT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_STOPCAPSIGNALMEASUREMENT_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvStopCapSignalMeasurement(CCU_ChanType aChanNum)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_STOPPED == CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    CCU_DrvRWDev.chan[aChanNum].capMode = CCU_CAPMODE_NONE;
    CCU_DrvRWDev.chan[aChanNum].state = CCU_DRV_CHANSTATE_STOPPED;
    CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_A);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_A, CCU_CCTMODE_COMPARE);
    CCU_CCTDisableCapCompInterrupt(aChanNum, CCU_CCTSUBCHAN_B);
    CCU_CCTSelectMode(aChanNum, CCU_CCTSUBCHAN_B, CCU_CCTMODE_COMPARE);

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_STOPCAPSIGNALMEASUREMENT_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPTIMEELAPSED_PROC
    @trace #BRCM_SWREQ_CCU
*/
static int32_t CCU_DrvGetCapTimeElapsed(CCU_ChanType aChanNum,
                                         CCU_ValueType *const aTimeElapsed)
{
    int32_t retVal = BCM_ERR_OK;

    if (CCU_CHAN_MAX < aChanNum) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    *aTimeElapsed = CCU_DrvRWDev.chan[aChanNum].elapsedTime;
    if (CCU_DrvRWDev.chan[aChanNum].sigMeasProp != CCU_SIGNALPROPERTY_PERIOD_TIME) {
        CCU_DrvRWDev.chan[aChanNum].elapsedTime = 0U;
    }

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCAPTIMEELAPSED_PROC, retVal,
                0UL, 0UL, 0UL, 0UL);
    }

    return retVal;
}

/**
    @trace #BRCM_SWARCH_CCU_GETCAPDUTYCYCLEVALUES_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvGetCapDutyCycleValues(CCU_ChanType aChanNum,
                                          CCU_DutyCycleType *const aDutyCycle)
{
    int32_t retVal = BCM_ERR_OK;

    if ((CCU_CHAN_MAX < aChanNum) || (NULL == aDutyCycle)) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Check the controller status */
    if (FALSE == CCU_DrvRWDev.isInit) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE != CCU_DrvRWDev.chan[aChanNum].mode) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    if ((CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev.chan[aChanNum].state)
        || (CCU_CAPMODE_SIGNAL_MEASURE != CCU_DrvRWDev.chan[aChanNum].capMode)) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }

    aDutyCycle->activeTime = CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime;
    aDutyCycle->periodTime = CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime;
    CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime = 0U;
    CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime = 0U;

err:
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError((uint8_t)aChanNum, BRCM_SWDSGN_CCU_DRV_GETCAPDUTYCYCLEVALUES_PROC, retVal, 0UL,
                0UL, 0UL, 0UL);
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvCapTimestampHandler(CCU_ChanType aChanNum,
                                        CCU_CCTSubChanType aSubChan)
{
    uint32_t tsWrIndex;

    CCU_DrvRWDev.chan[aChanNum].edgeCnt++;
    tsWrIndex = CCU_DrvRWDev.chan[aChanNum].tsWrIndex;
    if ((CCU_TIMESTAMPBUF_LINEAR == CCU_DrvRWDev.chan[aChanNum].TSBufProp)
            && (tsWrIndex < CCU_DrvRWDev.chan[aChanNum].bufSize)) {
        CCU_CCTGetCaptureValue(aChanNum, aSubChan,
                &(CCU_DrvRWDev.chan[aChanNum].TSBuf[tsWrIndex]));
        CCU_DrvRWDev.chan[aChanNum].tsWrIndex++;
    }

    if (CCU_TIMESTAMPBUF_CIRCULAR == CCU_DrvRWDev.chan[aChanNum].TSBufProp) {
        if (tsWrIndex == CCU_DrvRWDev.chan[aChanNum].bufSize) {
            tsWrIndex = 0UL;
            CCU_DrvRWDev.chan[aChanNum].tsWrIndex = 0UL;
        }
        CCU_CCTGetCaptureValue(aChanNum, aSubChan,
                &(CCU_DrvRWDev.chan[aChanNum].TSBuf[tsWrIndex]));
        CCU_DrvRWDev.chan[aChanNum].tsWrIndex++;
    }

    if ((TRUE == CCU_DrvRWDev.chan[aChanNum].isCbEn)
            && (NULL != CCU_DrvRWDev.chan[aChanNum].timestampCB)
            && (0UL != CCU_DrvRWDev.chan[aChanNum].notifyInterval)
            && ((CCU_DrvRWDev.chan[aChanNum].edgeCnt
                    % CCU_DrvRWDev.chan[aChanNum].notifyInterval) == 0UL)) {
        CCU_DrvRWDev.chan[aChanNum].timestampCB();
    }
}

/**
    @trace #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace #BRCM_SWREQ_CCU
*/
static void CCU_DrvCapSigMeasHandler(CCU_ChanType aChanNum,
                                      CCU_CCTSubChanType aSubChan)
{
    uint32_t idx;
    CCU_ValueType TS;
    uint64_t TS1, TS2, TS3, TS4;

    if (CCU_CCTSUBCHAN_A == aSubChan) {
        idx = (CCU_DrvRWDev.chan[aChanNum].risingEdgeIdx) % 2UL;
        CCU_CCTGetCaptureValue(aChanNum, aSubChan, &TS);
        CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[idx] =
            (((uint64_t)CCU_DrvRWDev.cntrWrapCnt << 16UL) + (uint64_t)TS);
        CCU_DrvRWDev.chan[aChanNum].risingEdgeIdx++;
    } else {
        idx = (CCU_DrvRWDev.chan[aChanNum].fallingEdgeIdx) % 2UL;
        CCU_CCTGetCaptureValue(aChanNum, aSubChan, &TS);
        CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[idx] =
            (((uint64_t)CCU_DrvRWDev.cntrWrapCnt << 16UL) + (uint64_t)TS);
        CCU_DrvRWDev.chan[aChanNum].fallingEdgeIdx++;
    }

    switch (CCU_DrvRWDev.chan[aChanNum].sigMeasProp) {
        case CCU_SIGNALPROPERTY_LOW_TIME:
        TS1 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[0UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        } else {
            TS1 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[0UL];
            TS2 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[1UL];
            if (TS2 > TS1) {
                CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev.chan[aChanNum].elapsedTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        }
        break;
    case CCU_SIGNALPROPERTY_HIGH_TIME:
        TS1 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[0UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        } else {
            TS1 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[0UL];
            TS2 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[1UL];
            if (TS2 > TS1) {
                CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev.chan[aChanNum].elapsedTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        }
        break;
    case CCU_SIGNALPROPERTY_PERIOD_TIME:
        CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
        TS1 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[1UL];
        if (TS2 > TS1) {
            CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS2 - TS1);
        } else {
            CCU_DrvRWDev.chan[aChanNum].elapsedTime =
                                        (CCU_ValueType)(TS1 - TS2);
        }
        break;
    case CCU_SIGNALPROPERTY_DUTY_CYCLE:
        TS1 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[0UL];
        TS2 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[0UL];
        TS3 = CCU_DrvRWDev.chan[aChanNum].risingEdgeTS[1UL];
        TS4 = CCU_DrvRWDev.chan[aChanNum].fallingEdgeTS[1UL];
        if (TS3 > TS1) {
            CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime =
                (CCU_ValueType)(TS3 - TS1);
            if (TS2 > TS1) {
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime =
                    (CCU_ValueType)(TS2 - TS1);
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        } else if (TS4 > TS2) {
            CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime =
                (CCU_ValueType)(TS4 - TS2);
            if (TS4 > TS3) {
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime =
                    (CCU_ValueType)(TS4 - TS3);
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            } else {
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime = 0U;
                CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
            }
        } else {
            CCU_DrvRWDev.chan[aChanNum].dutyCycle.periodTime = 0U;
            CCU_DrvRWDev.chan[aChanNum].dutyCycle.activeTime = 0U;
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_IDLE;
        }
        break;
    default:
        break;
    }
}

/**
    @trace  #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER

    @limitations None
*/
static void CCU_DrvChanHandler(CCU_ChanType aChanNum,
                                CCU_CCTSubChanType aSubChan)
{
    if (CCU_CHAN_MAX <  aChanNum) {
        goto err;
    }

    if (FALSE == CCU_DrvRWDev.isInit) {
        goto err;
    }

    if (CCU_DRV_CHANSTATE_RUNNING != CCU_DrvRWDev.chan[aChanNum].state) {
        goto err;
    }

    if (CCU_CHANMODE_CAPTURE == CCU_DrvRWDev.chan[aChanNum].mode) {
        switch(CCU_DrvRWDev.chan[aChanNum].capMode) {
        case CCU_CAPMODE_EDGE_COUNT:
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            CCU_DrvRWDev.chan[aChanNum].edgeCnt++;
            break;
        case CCU_CAPMODE_EDGE_DETECT:
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            if ((TRUE == CCU_DrvRWDev.chan[aChanNum].isCbEn) &&
                    (NULL != CCU_DrvRWDev.chan[aChanNum].edgeDetectCB)) {
                CCU_DrvRWDev.chan[aChanNum].edgeDetectCB();
            }
            break;
        case CCU_CAPMODE_SIGNAL_MEASURE:
            CCU_DrvCapSigMeasHandler(aChanNum, aSubChan);
            break;
        case CCU_CAPMODE_TIMESTAMP:
            CCU_DrvRWDev.chan[aChanNum].inputState = CCU_INPUTSTATE_ACTIVE;
            CCU_DrvCapTimestampHandler(aChanNum, aSubChan);
            break;
        default:
            break;
        }
    }

    if (CCU_CHANMODE_COMPARE == CCU_DrvRWDev.chan[aChanNum].mode) {
        if ((TRUE == CCU_DrvRWDev.chan[aChanNum].isCbEn)
                && (NULL != CCU_DrvRWDev.chan[aChanNum].compCB)) {
            CCU_DrvRWDev.chan[aChanNum].compCB();
        }
    }

err:
    return;
}

/**
    @trace  #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER

    @limitations None
*/
static void CCU_DrvUDCHandler(void)
{
    if (TRUE == CCU_DrvRWDev.isInit) {
        CCU_DrvRWDev.cntrWrapCnt++;
    }
}

/** @} */

/**
    @addtogroup grp_ccudrv_ifc
    @{
*/

/**

    @trace  #BRCM_SWARCH_CCU_DRV_IRQHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
void CCU_DrvIRQHandler(void)
{
    uint32_t regVal = CCU_CCTRegs->irq_stat;

    /* Clear interrupt status */
    CCU_CCTRegs->irq_stat = regVal;

    /* Process interrupts */
    if ((regVal & CCT_IRQ_STAT_CCR0_A_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_0, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR0_B_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_0, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR1_A_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_1, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR1_B_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_1, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR2_A_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_2, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR2_B_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_2, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_CCR3_A_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_3, CCU_CCTSUBCHAN_A);
    }

    if ((regVal & CCT_IRQ_STAT_CCR3_B_STAT_MASK) != 0UL) {
        CCU_DrvChanHandler(CCU_CHAN_3, CCU_CCTSUBCHAN_B);
    }

    if ((regVal & CCT_IRQ_STAT_UDC_STAT_MASK) != 0UL) {
        CCU_DrvUDCHandler();
    }

}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace  #BRCM_SWARCH_CCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWARCH_CCU_SYSCMDREQ_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_INTERFACE
*/
typedef union _CCU_SVCIOType {
    uint8_t *data;
    CCU_IOType *io;
} CCU_SVCIOType;

/**
    @code{.c}
    if aSysIO.ccuIO is not NULL {
        if aMagicID is SVC_MAGIC_CCT_ID
            aSysIO.ccuIO->retVal = CCU_DrvCmdHandler(aCmd, aSysIO.ccuIO)
        else
            aSysIO.ccuIO->retVal = BCM_ERR_INVAL_MAGIC
    }
    @endcode

    @trace  #BRCM_SWARCH_CCU_SYSCMDHANDLER_PROC
    @trace  #BRCM_SWREQ_CCU_KERNEL_HANDLER
*/
void CCU_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    CCU_SVCIOType ccu;
    ccu.data = aSysIO;
    int32_t retVal = BCM_ERR_OK;

    if (aSysIO != NULL) {
        if (aMagicID == SVC_MAGIC_CCT_ID) {
            switch (aCmd) {
            case CCU_CMD_INIT:
                CCU_DrvInit(ccu.io->config);
                break;
            case CCU_CMD_DEINIT:
                CCU_DrvDeInit();
                break;
            case CCU_CMD_GET_CHAN_MODE:
                retVal = CCU_DrvGetChanMode(ccu.io->chanNum, ccu.io->chanMode);
                break;
            case CCU_CMD_START_COMP_CHANNEL:
                CCU_DrvStartCompChannel(ccu.io->chanNum);
                break;
            case CCU_CMD_STOP_COMP_CHANNEL:
                CCU_DrvStopCompChannel(ccu.io->chanNum);
                break;
            case CCU_CMD_SET_COMP_PIN_PROPERTY:
                CCU_DrvSetCompPinProperty(ccu.io->chanNum, ccu.io->pinAction,
                        ccu.io->outPin);
                break;
            case CCU_CMD_SET_COMP_ABS_THRESHOLD:
                CCU_DrvSetCompAbsThreshold(ccu.io->chanNum, ccu.io->absValue);
                break;
            case CCU_CMD_ENABLE_COMP_CALLBACK:
                CCU_DrvEnableCompCallback(ccu.io->chanNum);
                break;
            case CCU_CMD_DISABLE_COMP_CALLBACK:
                CCU_DrvDisableCompCallback(ccu.io->chanNum);
                break;
            case CCU_CMD_SET_CAP_SIGNAL_PROP:
                CCU_DrvSetCapSignalProp(ccu.io->chanNum, ccu.io->inPin, ccu.io->edge);
                break;
            case CCU_CMD_ENABLE_CAP_CALLBACK:
                CCU_DrvEnableCapCallback(ccu.io->chanNum);
                break;
            case CCU_CMD_DISABLE_CAP_CALLBACK:
                CCU_DrvDisableCapCallback(ccu.io->chanNum);
                break;
            case CCU_CMD_GET_CAP_INPUT_STATE:
                retVal = CCU_DrvGetCapInputState(ccu.io->chanNum, ccu.io->inputState);
                break;
            case CCU_CMD_START_CAP_TIMESTAMP:
                CCU_DrvStartCapTimestamp(ccu.io->chanNum, ccu.io->tsBuf, ccu.io->tsBufSize,
                        ccu.io->notifyInterval);
                break;
            case CCU_CMD_STOP_CAP_TIMESTAMP:
                CCU_DrvStopCapTimestamp(ccu.io->chanNum);
                break;
            case CCU_CMD_GET_CAP_TIMESTAMP_INDEX:
                retVal = CCU_DrvGetCapTimestampIndex(ccu.io->chanNum, ccu.io->tsIndex);
                break;
            case CCU_CMD_RESET_CAP_EDGE_COUNT:
                CCU_DrvResetCapEdgeCount(ccu.io->chanNum);
                break;
            case CCU_CMD_ENABLE_CAP_EDGE_COUNT:
                CCU_DrvEnableCapEdgeCount(ccu.io->chanNum);
                break;
            case CCU_CMD_DISABLE_CAP_EDGE_COUNT:
                CCU_DrvDisableCapEdgeCount(ccu.io->chanNum);
                break;
            case CCU_CMD_ENABLE_CAP_EDGE_DETECTION:
                CCU_DrvEnableCapEdgeDetection(ccu.io->chanNum);
                break;
            case CCU_CMD_DISABLE_CAP_EDGE_DETECTION:
                CCU_DrvDisableCapEdgeDetection(ccu.io->chanNum);
                break;
            case CCU_CMD_GET_CAP_EDGE_NUMBERS:
                retVal = CCU_DrvGetCapEdgeNumbers(ccu.io->chanNum, ccu.io->edgeNum);
                break;
            case CCU_CMD_START_CAP_SIGNAL_MEASUREMENT:
                CCU_DrvStartCapSignalMeasurement(ccu.io->chanNum);
                break;
            case CCU_CMD_STOP_CAP_SIGNAL_MEASUREMENT:
                CCU_DrvStopCapSignalMeasurement(ccu.io->chanNum);
                break;
            case CCU_CMD_GET_CAP_TIME_ELAPSED:
                retVal = CCU_DrvGetCapTimeElapsed(ccu.io->chanNum, ccu.io->timeElapsed);
                break;
            case CCU_CMD_GET_CAP_DUTY_CYCLE_VALUES:
                CCU_DrvGetCapDutyCycleValues(ccu.io->chanNum, ccu.io->dutyCycle);
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
            ccu.io->retVal = retVal;
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }
    if (BCM_ERR_OK != retVal) {
        CCU_DrvReportError(0xFFU, BRCM_SWARCH_CCU_SYSCMDHANDLER_PROC, retVal,
                            aMagicID, aCmd, (uint32_t)aSysIO, 0UL);
    }
}
/** @} */
