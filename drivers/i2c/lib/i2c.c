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
    @defgroup grp_i2cdrv_impl IIC Client Design
    @ingroup grp_i2cdrv

    @addtogroup grp_i2cdrv_impl
    @{
*/

#include <stdint.h>
#include <string.h>
#include <chip_config.h>
#include <bcm_err.h>
#include <bcm_utils.h>
#include <i2c.h>
#include <i2c_osil.h>
#include <system.h>

/**
    @name IIC driver Design IDs
    @{
    @brief Design IDs for IIC driver
*/
#define BRCM_SWDSGN_IIC_INIT_PROC       (0x90U) /**< @brief #IIC_Init       */
#define BRCM_SWDSGN_IIC_DEINIT_PROC     (0x91U) /**< @brief #IIC_DeInit     */
#define BRCM_SWDSGN_IIC_TRANSFER_PROC   (0x92U) /**< @brief #IIC_Transfer   */
#define BRCM_SWDSGN_IIC_GETSTATUS_PROC  (0x93U) /**< @brief #IIC_GetStatus  */
/** @} */


/** @} */


/**
    @addtogroup grp_i2cdrv_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
void IIC_Init(IIC_HwIDType aID, const IIC_ConfigType *const aConfig)
{
    IIC_IOType iicIO;
    int32_t ret = BCM_ERR_OK;

    iicIO.retVal = BCM_ERR_UNKNOWN;
    iicIO.hwID = aID;
    iicIO.config = aConfig;
    ret = IIC_SysCmdReq(IIC_CMD_INIT, &iicIO);
    if (BCM_ERR_OK != ret) {
        const uint32_t values[4] = {0UL, 0UL, 0UL, __LINE__};
        BCM_ReportError(BCM_IIC_ID, (uint8_t)aID, BRCM_SWDSGN_IIC_INIT_PROC,
                        ret, 4UL, values);
    }
}

/**
    @brief IIC Driver Transfer Request function

    @trace #BRCM_SWARCH_IIC_TRANSFER_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

*/
int32_t IIC_Transfer(IIC_HwIDType aID,
                        IIC_PktType * const aPkts,
                        uint32_t aNumPkts,
                        uint32_t aClientMask,
                        uint32_t *aJobID)
{
    IIC_IOType iicIO;
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if (NULL != aJobID) {
        iicIO.retVal = BCM_ERR_UNKNOWN;
        iicIO.hwID = aID;
        iicIO.numPkts = aNumPkts;
        iicIO.pkts = aPkts;
        iicIO.clientMask = aClientMask;
        ret = IIC_SysCmdReq(IIC_CMD_TRANSFER_SUBMIT, &iicIO);
        if (BCM_ERR_OK == ret) {
            *aJobID = iicIO.jobID;
        }
    }

    return ret;
}

/**
    @brief IIC Driver Receive Response function

    @trace #BRCM_SWARCH_IIC_GETSTATUS_PROC
    @trace #BRCM_SWREQ_IIC_TRANSFER

*/
int32_t IIC_GetStatus(IIC_HwIDType aID, uint32_t aJobID)
{
    IIC_IOType iicIO;
    int32_t ret;

    iicIO.retVal = BCM_ERR_UNKNOWN;
    iicIO.hwID = aID;
    iicIO.jobID = aJobID;
    ret = IIC_SysCmdReq(IIC_CMD_TRANSFER_STATUS, &iicIO);

    return ret;
}

/**
    @trace #BRCM_SWARCH_IIC_INIT_PROC
    @trace #BRCM_SWREQ_IIC_INIT
*/
int32_t IIC_DeInit(IIC_HwIDType aID)
{
    IIC_IOType iicIO;
    int32_t ret = BCM_ERR_OK;

    iicIO.retVal = BCM_ERR_UNKNOWN;
    iicIO.hwID = aID;
    ret = IIC_SysCmdReq(IIC_CMD_DEINIT, &iicIO);
    return ret;
}


/** @} */
