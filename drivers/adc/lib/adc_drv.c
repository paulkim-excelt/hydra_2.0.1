/*****************************************************************************
 Copyright 2017-2019 Broadcom Limited.  All rights reserved.

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
    @addtogroup grp_adcdrv_impl
    @{

    @file adc_drv.c
    @brief ADC Driver implementation
    This header file contains the functions for ADC Driver
    @version 1.0 Changelist1
*/

#include <compiler.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <interrupt.h>
#include <stdlib.h>
#include <atomic.h>
#include <adc.h>
#include <adc_osil.h>
#include <adc_rdb.h>

/**
    @name ADC Driver design IDs
    @{
    @brief Design IDs for ADC Driver
*/
#define BRCM_SWDSGN_ADC_DRVINVALIDIDX_MACRO             (0x90U) /**< @brief #ADC_DRV_INVALID_IDX        */
#define BRCM_SWDSGN_ADC_INITGRPINFO_MACRO               (0x91U) /**< @brief #ADC_INIT_GRP_INFO          */
#define BRCM_SWDSGN_ADC_DRVSTATE_TYPE                   (0x92U) /**< @brief #ADC_DrvStateType           */
#define BRCM_SWDSGN_ADC_GRPINFO_TYPE                    (0x93U) /**< @brief #ADC_GrpInfoType            */
#define BRCM_SWDSGN_ADC_RWDEV_TYPE                      (0x94U) /**< @brief #ADC_RWDevType              */
#define BRCM_SWDSGN_ADC_REGS_GLOBAL                     (0x95U) /**< @brief #ADC_Regs                   */
#define BRCM_SWDSGN_ADC_RWDEV_GLOBAL                    (0x96U) /**< @brief #ADC_RWDev                  */
#define BRCM_SWDSGN_ADC_DRV_REPORTERROR_PROC            (0x97U) /**< @brief #ADC_DrvReportError         */
#define BRCM_SWDSGN_ADC_DRV_DISABLEHW_PROC              (0x98U) /**< @brief #ADC_DrvDisableHw           */
#define BRCM_SWDSGN_ADC_DRV_ENABLEHW_PROC               (0x99U) /**< @brief #ADC_DrvEnableHw            */
#define BRCM_SWDSGN_ADC_DRV_ISRESULTINRANGE_PROC        (0x9AU) /**< @brief #ADC_DrvIsResultInRange     */
#define BRCM_SWDSGN_ADC_DRV_GETGRPIDX_PROC              (0x9BU) /**< @brief #ADC_DrvGetGrpCfgIdx        */
#define BRCM_SWDSGN_ADC_DRV_VALIDATECHANNCFG_PROC       (0x9CU) /**< @brief #ADC_DrvValidateChannCfg    */
#define BRCM_SWDSGN_ADC_DRV_VALIDATEGRPCFG_PROC         (0x9DU) /**< @brief #ADC_DrvValidateGrpCfg      */
#define BRCM_SWDSGN_ADC_DRV_VALIDATECFG_PROC            (0x9EU) /**< @brief #ADC_DrvValidateCfg         */
#define BRCM_SWDSGN_ADC_DRV_INIT_PROC                   (0x9FU) /**< @brief #ADC_DrvInit                */
#define BRCM_SWDSGN_ADC_DRV_DEINIT_PROC                 (0xA0U) /**< @brief #ADC_DrvDeInit              */
#define BRCM_SWDSGN_ADC_DRV_STARTGROUPCONV_PROC         (0xA1U) /**< @brief #ADC_DrvStartGroupConv      */
#define BRCM_SWDSGN_ADC_DRV_STOPGROUPCONV_PROC          (0xA2U) /**< @brief #ADC_DrvStopGroupConv       */
#define BRCM_SWDSGN_ADC_DRV_READGROUPSAMPLE_PROC        (0xA3U) /**< @brief #ADC_DrvReadGroupSample     */
#define BRCM_SWDSGN_ADC_DRV_ENABLEGROUPNOTIF_PROC       (0xA4U) /**< @brief #ADC_DrvEnableGroupNotif    */
#define BRCM_SWDSGN_ADC_DRV_DISABLEGROUPNOTIF_PROC      (0xA5U) /**< @brief #ADC_DrvDisableGroupNotif   */
#define BRCM_SWDSGN_ADC_DRV_GETGROUPCONVSTATE_PROC      (0xA6U) /**< @brief #ADC_DrvGetGroupConvState   */
#define BRCM_SWDSGN_ADC_IRQHANDLERINTERNAL_PROC         (0xA7U) /**< @brief #ADC_IRQHandlerInternal     */
#define BRCM_SWDSGN_ADC_SVCIO_TYPE                      (0xA8U) /**< @brief #ADC_SVCIOType              */
#define BRCM_SWDSGN_ADC_SYSCMDHANDLER_PROC              (0xA9U) /**< @brief #ADC_SysCmdHandler          */
/** @} */

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
#define ADC_DRV_INVALID_IDX         (0xFFFFFFFFUL) /**< @brief Invalid index */

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
#define ADC_INIT_GRP_INFO(aIdx)                     \
    .grpInfo[(aIdx)] = {                            \
        .currGrpID = ADC_DRV_INVALID_IDX,           \
        .currChanIdx = 0UL,                         \
        .convState = ADC_CHANGRPCONVSTATE_IDLE,     \
        .bufWrIdx = 0UL,                            \
        .enableNotif = FALSE,                       \
    }

/**
    @name ADC_DRV_RegValType
    @{

    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
typedef uint32_t ADC_DrvStateType;
#define ADC_DRVSTATE_UNINIT        (0UL)   /**< @brief Uninitialized state  */
#define ADC_DRVSTATE_INIT          (1UL)   /**< @brief Initialized state    */
/** @} */

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
typedef struct _ADC_GrpInfoType {
    uint32_t                    currGrpID;
    uint32_t                    currChanIdx;
    ADC_ChanGrpConvStateType    convState;      /**< @brief Conversion state                */
    ADC_ValType                 sampleBuf[ADC_MAX_NUM_CHANN * 2UL];
    uint32_t                    bufWrIdx;       /**< @brief Last completed sampling round   */
    uint32_t                    enableNotif;
} ADC_GrpInfoType;

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
typedef  struct _ADC_RWDevType {
    ADC_DrvStateType state;                  /**< @brief Device state   */
    ADC_GrpInfoType  grpInfo[ADC_MAX_HW_ID]; /**< @brief Hardware index */
} ADC_RWDevType;


/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static ADC_RDBType * const ADC_Regs[ADC_MAX_HW_ID] = {
#if (ADC_MAX_HW_ID == 0)
#error "ADC_MAX_HW_ID = 0"
#endif  /* ADC_MAX_HW_ID == 0 */
    (ADC_RDBType *) ADC_BASE,
#if (ADC_MAX_HW_ID > 1)
#error "ADC_MAX_HW_ID > 1"
#endif  /* ADC_MAX_HW_ID > 1 */
};

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static ADC_RWDevType ADC_RWDev COMP_SECTION(".data.drivers") = {
    .state = ADC_DRVSTATE_UNINIT,
#if (ADC_MAX_HW_ID == 0)
#error "ADC_MAX_HW_ID = 0"
#endif  /* ADC_MAX_HW_ID == 0 */
#if (ADC_MAX_HW_ID > 0)
    ADC_INIT_GRP_INFO(0UL),
#endif
#if (ADC_MAX_HW_ID > 1)
#error "ADC_MAX_HW_ID > 1"
#endif  /* ADC_MAX_HW_ID > 1 */
};

/**
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWARCH_ADC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_ADC_KERNEL_HANDLER
*/
static void ADC_DrvReportError(uint32_t aInstanceID, uint8_t aApiID,
                               int32_t aErr, uint32_t aVal0, uint32_t aVal1,
                               uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError((BCM_ADC_ID & BCM_LOGMASK_KERNEL), (uint8_t)aInstanceID, aApiID,
                    aErr, 4UL, values);
}

/**
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static void ADC_DrvDisableHw(ADC_HwIDType aHwID, uint32_t aPwrDn)
{
    uint32_t regVal = ADC_Regs[aHwID]->ctrl;
    regVal |= ADC_CTRL_ADC_IRQ_MASK_MASK;
    regVal &= ~ADC_CTRL_ENABLE_MASK;
    if (TRUE == aPwrDn) {
        regVal &= ~ADC_CTRL_ADCPWRUP_MASK;
    }
    ADC_Regs[aHwID]->ctrl = regVal;

    /* clear the interrupt */
    ADC_Regs[aHwID]->irq_stat = ADC_IRQ_STAT_IRQ_MASK;
}

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static void ADC_DrvEnableHw(ADC_HwIDType aHwID, uint32_t aChann)
{
    uint32_t reg;

    reg = ADC_Regs[aHwID]->ctrl;
    reg &= ~ADC_CTRL_AMUX_MASK;
    reg |= (aChann << ADC_CTRL_AMUX_SHIFT);
    reg |= ADC_CTRL_ADCPWRUP_MASK;
    reg |= ADC_CTRL_ENABLE_MASK;
    reg &= ~ADC_CTRL_ADC_IRQ_MASK_MASK;
    ADC_Regs[aHwID]->ctrl = reg;
}

/**
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_KERNEL_HANDLER
*/
static uint32_t ADC_DrvIsResultInRange(ADC_HwIDType aHwID,
                                       uint32_t aResult,
                                       const ADC_ChannCfgType *const aChannCfg) {
    uint32_t inRange = FALSE;

    switch (aChannCfg->limitCheck) {
    case ADC_LIMITCHECK_ALWAYS:
        inRange = TRUE;
        break;
    case ADC_LIMITCHECK_BETWEEN:
        if ((aResult > aChannCfg->limitLow)
                && (aResult <= aChannCfg->limitHigh)) {
            inRange = TRUE;
        }
        break;
    case ADC_LIMITCHECK_NOT_BETWEEN:
        if ((aResult <= aChannCfg->limitLow)
                || (aResult > aChannCfg->limitHigh)) {
            inRange = TRUE;
        }
        break;
    case ADC_LIMITCHECK_NOT_OVER_HIGH:
        if (aResult <= aChannCfg->limitHigh) {
            inRange = TRUE;
        }
        break;
    case ADC_LIMITCHECK_NOT_UNDER_LOW:
        if (aResult > aChannCfg->limitLow) {
            inRange = TRUE;
        }
        break;
    case ADC_LIMITCHECK_OVER_HIGH:
        if (aResult > aChannCfg->limitHigh) {
            inRange = TRUE;
        }
        break;
    case ADC_LIMITCHECK_UNDER_LOW:
        if (aResult <= aChannCfg->limitLow) {
            inRange = TRUE;
        }
        break;
    default:
        /*
         * Won't reach here since configuration is already validated
         * at initialization.
         */

        ADC_DrvReportError(aHwID, BRCM_SWDSGN_ADC_DRV_ISRESULTINRANGE_PROC,
                            BCM_ERR_INVAL_PARAMS, aResult, aChannCfg->limitCheck,0UL,
                            __LINE__);
        break;
    } /* End of switch(aChannCfg->range) */
    return inRange;
}

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvGetGrpCfgIdx(ADC_ChanGrpIDType aGrpID,
                                    uint32_t *const aHwCfgIdx,
                                    uint32_t *const aGrpCfgIdx)
{
    uint32_t hwIdx, grpIdx;
    int32_t retVal = BCM_ERR_NOT_FOUND;

    for (hwIdx = 0UL; hwIdx < ADC_ConfigSz; ++hwIdx) {
        for (grpIdx = 0UL; grpIdx < ADC_Config[hwIdx].grpCfgListSz; ++grpIdx) {
            if (aGrpID == ADC_Config[hwIdx].grpCfgList[grpIdx].grpID) {
                *aHwCfgIdx = hwIdx;
                *aGrpCfgIdx = grpIdx;
                retVal = BCM_ERR_OK;
                break;
            }
        }
        if (BCM_ERR_OK == retVal) {
            break;
        }
    }

    return retVal;
}
/**
    @brief Helper function to validate ADC channel configuration

    @param[in]   aChannCfg      `        Pointer to ADC Hw configuration

    @retval      BCM_ERR_OK              aChannCfg is valid
    @retval      BCM_ERR_INVAL_PARAMS    aChannCfg is NULL
    @retval      BCM_ERR_INVAL_PARAMS    aChannCfg is invalid

    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
static int32_t ADC_DrvValidateChannCfg(const ADC_ChannCfgType *const aChannCfg)
{
    int32_t retVal;

    if (ADC_MAX_NUM_CHANN < aChannCfg->channID) {
        retVal = BCM_ERR_INVAL_PARAMS;
    } else {
        if (ADC_LIMITCHECK_UNDER_LOW < aChannCfg->limitCheck) {
            retVal = BCM_ERR_INVAL_PARAMS;
        } else {
            if (ADC_LIMITCHECK_ALWAYS !=aChannCfg->limitCheck) {
                if (aChannCfg->limitLow <= aChannCfg->limitHigh) {
                    retVal = BCM_ERR_OK;
                } else {
                    retVal = BCM_ERR_INVAL_PARAMS;
                }
            } else {
                retVal = BCM_ERR_OK;
            }
        }
    }
    return retVal;
}

/**
    @brief Helper function to validate ADC group configuration

    @param[in]   aGrpCfg                 Pointer to ADC Hw configuration

    @retval      BCM_ERR_OK              aGrpCfg is valid
    @retval      BCM_ERR_INVAL_PARAMS    aGrpCfg is NULL
    @retval      BCM_ERR_INVAL_PARAMS    aGrpCfg is invalid

    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
static int32_t ADC_DrvValidateGrpCfg(uint32_t aChannCfgSz, const ADC_GrpCfgType *const aGrpCfg)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if ((0UL < aGrpCfg->channCfgIdxListSz) && (aGrpCfg->channCfgIdxListSz <= aChannCfgSz)) {
        for (i = 0UL; i < aGrpCfg->channCfgIdxListSz; i++) {
            if (aGrpCfg->channCfgIdxList[i] >= aChannCfgSz) {
                break;
            }
        }
        if (i == aGrpCfg->channCfgIdxListSz) {
            retVal = BCM_ERR_OK;
        }
    }
    return retVal;
}

/**
    @brief Helper function to validate ADC configuration.

    @param[in]   aCfg    Pointer to ADC configuration

    @retval      BCM_ERR_OK              aCfg is valid
    @retval      BCM_ERR_INVAL_PARAMS    aCfg is NULL
    @retval      BCM_ERR_INVAL_PARAMS    aCfg is invalid

    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
 */
static int32_t ADC_DrvValidateCfg(const ADC_ConfigType * const aCfg)
{
    uint32_t i;
    int32_t retVal = BCM_ERR_INVAL_PARAMS;

    if (ADC_MAX_HW_ID > aCfg->hwID) {
        if ((NULL != aCfg->channCfgList) && (0UL < aCfg->channCfgListSz)
                && (ADC_MAX_NUM_CHANN >= aCfg->channCfgListSz)) {
            /* Validate channel configuration */
            for (i = 0UL; i < aCfg->channCfgListSz; i++) {
                retVal = ADC_DrvValidateChannCfg(&aCfg->channCfgList[i]);
                if (BCM_ERR_OK != retVal) {
                    break;
                }
            }
            if (BCM_ERR_OK == retVal) {
                /* validate group configuration */
                if ((NULL != aCfg->grpCfgList) && (0UL < aCfg->grpCfgListSz)) {
                    for (i = 0UL; i < aCfg->grpCfgListSz; i++) {
                        retVal = ADC_DrvValidateGrpCfg(aCfg->channCfgListSz,
                                &aCfg->grpCfgList[i]);
                        if (BCM_ERR_OK != retVal) {
                            break;
                        }
                    }
                } else {
                    retVal = BCM_ERR_INVAL_PARAMS;
                }
            }
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_INIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static int32_t ADC_DrvInit(void)
{
    int32_t retVal = BCM_ERR_INVAL_STATE;
    uint32_t hwCfgIdx;

    if (ADC_DRVSTATE_UNINIT == ADC_RWDev.state) {
        for (hwCfgIdx = 0UL; hwCfgIdx < ADC_ConfigSz; hwCfgIdx++) {
            retVal = ADC_DrvValidateCfg(&ADC_Config[hwCfgIdx]);
            if (BCM_ERR_OK != retVal) {
                break;
            }
        }
        if (BCM_ERR_OK == retVal) {
            ADC_RWDev.state = ADC_DRVSTATE_INIT;
        }
    }
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_DEINIT_PROC
    @trace #BRCM_SWREQ_ADC_INIT
*/
static int32_t ADC_DrvDeInit(void)
{
    uint32_t idx;
    int32_t retVal = BCM_ERR_OK;

    /* Check state of the driver */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Check if all the ADC channel groups are in idle or completed state */
    for (idx = 0UL; idx < ADC_MAX_HW_ID; ++idx) {
        if (ADC_DRV_INVALID_IDX != ADC_RWDev.grpInfo[idx].currGrpID) {
            retVal = BCM_ERR_BUSY;
            break;
        }
    }

    if (BCM_ERR_OK != retVal) {
        goto err;
    }


    /* Move ADC_RWDev to reset state */
    for (idx = 0UL; idx < ADC_MAX_HW_ID; ++idx) {
        ADC_RWDev.grpInfo[idx].currGrpID = ADC_DRV_INVALID_IDX;
        ADC_RWDev.grpInfo[idx].currChanIdx = 0UL;
        ADC_RWDev.grpInfo[idx].convState = ADC_CHANGRPCONVSTATE_IDLE;
        ADC_RWDev.grpInfo[idx].bufWrIdx = 0UL;
        ADC_RWDev.grpInfo[idx].enableNotif = FALSE;
        ADC_DrvDisableHw(idx, TRUE);
    }

    /* Move the driver to uninit state */
    ADC_RWDev.state = ADC_DRVSTATE_UNINIT;
err:
    return retVal;
}


/**
    @trace #BRCM_SWARCH_ADC_STARTGROUPCONV_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvStartGroupConv(ADC_ChanGrpIDType aGrpID)
{
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;
    ADC_GrpInfoType *grpInfo;
    const ADC_GrpCfgType  *grpCfg;
    ADC_ChannelIDType  channID;
    uint32_t currChannCfgIdx;
    int32_t retVal = BCM_ERR_OK;

    /* Validate driver state */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Validate group ID */
    retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    grpInfo = &ADC_RWDev.grpInfo[hwCfgIdx];

    if (ADC_DRV_INVALID_IDX != grpInfo->currGrpID) {
        retVal = BCM_ERR_BUSY;
        goto err;
    }

    grpCfg = &ADC_Config[hwCfgIdx].grpCfgList[grpCfgIdx];
    grpInfo->currGrpID = aGrpID;
    grpInfo->bufWrIdx = 0UL;
    grpInfo->currChanIdx = 0UL;
    currChannCfgIdx = grpCfg->channCfgIdxList[0UL];
    BCM_MemSet(grpInfo->sampleBuf, 0x0, sizeof(grpInfo->sampleBuf));
    channID = ADC_Config[hwCfgIdx].channCfgList[currChannCfgIdx].channID;
    grpInfo->convState = ADC_CHANGRPCONVSTATE_BUSY;
    ADC_DrvEnableHw(hwCfgIdx, channID);

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_STOPGROUPCONV_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvStopGroupConv(ADC_ChanGrpIDType aGrpID)
{
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;
    ADC_GrpInfoType *grpInfo;
    int32_t retVal = BCM_ERR_OK;

    /* Validate driver state */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Validate group ID */
    retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    grpInfo = &ADC_RWDev.grpInfo[hwCfgIdx];

    if (aGrpID != grpInfo->currGrpID) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Stop the hardware and reset the vars */
    ADC_DrvDisableHw(hwCfgIdx, TRUE);
    grpInfo->currGrpID = ADC_DRV_INVALID_IDX;
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_READGROUPSAMPLE_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvReadGroupSample(ADC_ChanGrpIDType aGrpID, uint32_t *aBuf)
{
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;
    ADC_GrpInfoType *grpInfo;
    const ADC_GrpCfgType  *grpCfg;
    uint32_t rdIdx;
    uint32_t i;
    int32_t retVal = BCM_ERR_OK;

    if (NULL != aBuf) {
        /* Validate driver state */
        if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
            retVal = BCM_ERR_UNINIT;
            goto err;
        }
        /* Validate group ID */
        retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
        if (BCM_ERR_OK != retVal) {
            goto err;
        }
        grpInfo = &ADC_RWDev.grpInfo[hwCfgIdx];
        if (aGrpID != grpInfo->currGrpID) {
            retVal = BCM_ERR_INVAL_PARAMS;
            goto err;
        }
        if (ADC_CHANGRPCONVSTATE_AVAILABLE != grpInfo->convState) {
            retVal = BCM_ERR_INVAL_STATE;
            goto err;
        }
        grpCfg = &ADC_Config[hwCfgIdx].grpCfgList[grpCfgIdx];
        if (grpInfo->bufWrIdx <= grpCfg->channCfgIdxListSz) {
            rdIdx = grpCfg->channCfgIdxListSz;
        } else {
            rdIdx = 0UL;
        }
        for (i = 0UL; i < grpCfg->channCfgIdxListSz; i++) {
            aBuf[i] = grpInfo->sampleBuf[rdIdx++];
        }
    }
err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_ENABLEGROUPNOTIF_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvEnableGroupNotif(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;

    /* Validate driver state */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Validate channel group ID */
    retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    /* Validate callback function pointer */
    if (NULL == ADC_Config[hwCfgIdx].grpCfgList[grpCfgIdx].callback) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Enable channel group notification */
    ADC_RWDev.grpInfo[hwCfgIdx].enableNotif = TRUE;
    retVal = BCM_ERR_OK;

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_DISABLEGROUPNOTIF_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvDisableGroupNotif(ADC_ChanGrpIDType aGrpID)
{
    int32_t retVal;
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;

    /* Validate driver state */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Validate channel group ID */
    retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    /* Validate callback function pointer */
    if (NULL == ADC_Config[hwCfgIdx].grpCfgList[grpCfgIdx].callback) {
        retVal = BCM_ERR_INVAL_PARAMS;
        goto err;
    }

    /* Disable channel group notification */
    ADC_RWDev.grpInfo[hwCfgIdx].enableNotif = FALSE;
    retVal = BCM_ERR_OK;

err:
    return retVal;
}

/**
    @trace #BRCM_SWARCH_ADC_GETGROUPCONVSTATE_PROC
    @trace #BRCM_SWREQ_ADC_SERVICES
*/
static int32_t ADC_DrvGetGroupConvState(ADC_ChanGrpIDType aGrpID,
                                        ADC_ChanGrpConvStateType * const aConvState)
{
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;
    ADC_GrpInfoType *grpInfo;
    int32_t retVal = BCM_ERR_OK;

    if (NULL == aConvState) {
        retVal = BCM_ERR_UNKNOWN;
        goto err;
    }

    *aConvState = ADC_CHANGRPCONVSTATE_IDLE;
    /* Validate driver state */
    if (ADC_DRVSTATE_INIT != ADC_RWDev.state) {
        retVal = BCM_ERR_UNINIT;
        goto err;
    }

    /* Validate channel group ID */
    retVal = ADC_DrvGetGrpCfgIdx(aGrpID, &hwCfgIdx, &grpCfgIdx);
    if (BCM_ERR_OK != retVal) {
        goto err;
    }

    grpInfo = &ADC_RWDev.grpInfo[hwCfgIdx];
    if (aGrpID != grpInfo->currGrpID) {
        retVal = BCM_ERR_INVAL_STATE;
        goto err;
    }
    /* Get channel group conversion state */
    *aConvState = grpInfo->convState;

err:
    return retVal;
}


/**
    @trace #BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC
    @trace #BRCM_SWREQ_ADC_KERNEL_HANDLER
*/
void ADC_IRQHandlerInternal(ADC_HwIDType aHwID)
{
    uint32_t hwCfgIdx;
    uint32_t grpCfgIdx;
    ADC_GrpInfoType *grpInfo;
    const ADC_GrpCfgType  *grpCfg;
    const ADC_ChannCfgType *channCfg;
    uint16_t adcSample;
    uint32_t channCfgIdx;
    ADC_ChannelIDType nextChannID;
    uint32_t line = __LINE__;
    int32_t retVal = BCM_ERR_OK;

    if (ADC_MAX_HW_ID > aHwID) {
        grpInfo = &ADC_RWDev.grpInfo[aHwID];
        /* read the sample, clear the interrupt and disable the hardware
         * check the sample range; if its not valid, restart the covertion
         * of the whole group again
         */
        adcSample = (uint16_t)(ADC_Regs[aHwID]->stat & ADC_STAT_OUT_MASK);
        ADC_DrvDisableHw(aHwID, FALSE);
        if (ADC_DRV_INVALID_IDX != grpInfo->currGrpID) {
            retVal = ADC_DrvGetGrpCfgIdx(grpInfo->currGrpID, &hwCfgIdx,
                    &grpCfgIdx);
            if (BCM_ERR_OK == retVal) {
                grpCfg = &ADC_Config[hwCfgIdx].grpCfgList[grpCfgIdx];
                channCfgIdx = grpCfg->channCfgIdxList[grpInfo->currChanIdx];
                channCfg = &ADC_Config[hwCfgIdx].channCfgList[channCfgIdx];

                if (TRUE == ADC_DrvIsResultInRange(aHwID, adcSample, channCfg)) {
                    grpInfo->sampleBuf[grpInfo->bufWrIdx] = adcSample;
                    grpInfo->bufWrIdx = BCM_IncMod(grpInfo->bufWrIdx,
                            grpCfg->channCfgIdxListSz * 2UL);

                    if (grpInfo->currChanIdx == (grpCfg->channCfgIdxListSz - 1UL)) {
                        /* one round of sampling completed */
                        grpInfo->convState = ADC_CHANGRPCONVSTATE_AVAILABLE;
                        if ((TRUE == grpInfo->enableNotif) &&
                                (NULL != grpCfg->callback)) {
                            grpCfg->callback(grpCfg->grpID);
                        }
                    }
                    grpInfo->currChanIdx = BCM_IncMod(grpInfo->currChanIdx,
                            grpCfg->channCfgIdxListSz);
                } else {
                    if (grpInfo->bufWrIdx < grpCfg->channCfgIdxListSz) {
                        grpInfo->bufWrIdx = 0UL;
                    } else {
                        grpInfo->bufWrIdx = grpCfg->channCfgIdxListSz;
                    }
                    grpInfo->currChanIdx = 0UL;
                }
                channCfgIdx = grpCfg->channCfgIdxList[grpInfo->currChanIdx];
                channCfg = &ADC_Config[hwCfgIdx].channCfgList[channCfgIdx];
                nextChannID = channCfg->channID;
                ADC_DrvEnableHw(aHwID, nextChannID);
            } else {
                line = __LINE__;
            }
        } else {
            retVal = BCM_ERR_INVAL_STATE;
            line = __LINE__;
        }
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
        line = __LINE__;
    }
    if (BCM_ERR_OK != retVal) {
        ADC_DrvReportError(aHwID, BRCM_SWARCH_ADC_IRQHANDLERINTERNAL_PROC, retVal,
                            0UL, 0UL, 0UL, line);
    }
}

/**
    @brief Union to avoid MISRA Required error
    for Type conversion

    @trace #BRCM_SWARCH_ADC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_ADC_KERNEL_HANDLER
*/
typedef union _ADC_SVCIOType {
    uint8_t     *data;
    ADC_IOType  *io;
} ADC_SVCIOType;

/**
    @trace #BRCM_SWARCH_ADC_SYSCMDHANDLER_PROC
    @trace #BRCM_SWREQ_ADC_KERNEL_HANDLER
*/
void ADC_SysCmdHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t *aSysIO)
{
    int32_t retVal;
    ADC_SVCIOType adcIO;
    adcIO.data = aSysIO;

    if (NULL != aSysIO) {
        if (SVC_MAGIC_ADC_ID == aMagicID) {
            switch (aCmd) {
            case ADC_CMD_INIT:
                retVal = ADC_DrvInit();
                break;
            case ADC_CMD_DEINIT:
                retVal = ADC_DrvDeInit();
                break;
            case ADC_CMD_START_GRP_CONV:
                retVal  = ADC_DrvStartGroupConv(adcIO.io->grpID);
                break;
            case ADC_CMD_STOP_GRP_CONV:
                retVal = ADC_DrvStopGroupConv(adcIO.io->grpID);
                break;
            case ADC_CMD_READ_GRP_SAMPLE:
                retVal = ADC_DrvReadGroupSample(adcIO.io->grpID,
                        adcIO.io->sampleBuf);
                break;
            case ADC_CMD_ENABLE_GRP_NOTIF:
                retVal = ADC_DrvEnableGroupNotif(adcIO.io->grpID);
                break;
            case ADC_CMD_DISABLE_GRP_NOTIF:
                retVal = ADC_DrvDisableGroupNotif(adcIO.io->grpID);
                break;
            case ADC_CMD_GET_GRP_CONV_STATE:
                retVal = ADC_DrvGetGroupConvState(adcIO.io->grpID,
                        &(adcIO.io->convState));
                break;
            default:
                retVal = BCM_ERR_INVAL_PARAMS;
                break;
            }
        } else {
            retVal = BCM_ERR_INVAL_MAGIC;
        }
        adcIO.io->retVal = retVal;
    } else {
        retVal = BCM_ERR_INVAL_PARAMS;
    }

    if (BCM_ERR_OK != retVal) {
        ADC_DrvReportError(0xFFUL, BRCM_SWARCH_ADC_SYSCMDHANDLER_PROC, retVal,
                aMagicID, aCmd, (uint32_t)aSysIO, __LINE__);
    }
}
/** @} */
