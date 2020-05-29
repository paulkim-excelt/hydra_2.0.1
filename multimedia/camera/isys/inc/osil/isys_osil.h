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

/** @defgroup grp_isys_il Integration Guide
    @ingroup grp_isys

    @addtogroup grp_isys_il
    @{
    @section sec_isys_integration Integration Guide
    This subsection describes functions needed by an integrator. Some of the
    functions are implemented as part of the ISYS library, and some of the
    functions shall be implemented by the integrator.

    Tuners need a dedicated context (Task) for execution as it could take
    significant time to run based on number of tuners as well as the complexity
    of tuner algorithms.

    @note If any interfaces need to implemented by integrator, it is explicitly
    mentioned in the section

    @section sec_isys_il_seq Sequence Diagram
    Sequence diagrams in this section have SVC layer and command handler
    lifelines. If SVC exceptions are enabled in the system, SVC layer
    raises an SVC. If SVC exceptions are not used for drivers, SVC layer
    could be seen as a direct function call to Command handler Layer.

    In ISYS Library, ISP as well as UNICAM drivers uses SVC for restricting
    device access unless privileged context.

    Below diagram explains SVC sequences for #ISYS_Init.
    @image html isys_seq_il_diagram_svc_init.jpg "SVC Sequence diagram for ISYS_Init API"

    @file isys_osil.h
    @brief Integration API's of the ISYS framework
    This header file contains the ISYS framework Integration interfaces
    @version 1.82 Misra fixes
    @version 1.81 Imported from AUTOSW_ISYS_Interface_Spec_1.81.docx
*/

#ifndef ISYS_OSIL_H
#define ISYS_OSIL_H

#include <stdint.h>
#include <compiler.h>
#include <bcm_err.h>
#include <isys.h>

/**
    @name ISYS interface IDs
    @{
    @brief interface IDs for ISYS
*/
#define BRCM_SWARCH_ISYS_IL_PARSE_FRAME_NUM_PROC          (0x81U)   /**< @brief #ISYS_ParseFrameNum */
/** @} */

/** @brief Parse Camera Sensor Frame number

    This API parses the metadata received from camera sensor and return
    the filled frame number.

    @behavior Sync, Non-reentrant

    @pre CSN shall be in #CSN_STATE_STREAMING state while calling this API

    @param[in]      aBufferLong      Metadata buffer for first channel
    @param[in]      aSizeLong        Size of the buffer for first channel
    @param[in]      aBufferLong      Metadata buffer for second channel
    @param[in]      aSizeLong        Size of the buffer for second channel
    @param[out]     aFrameNum        Frame number to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Metadata parsing is successful
    @retval     #BCM_ERR_UNKNOWN        Metadata parsing failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Metadata parsing implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)
    @retval     #BCM_ERR_INVAL_STATE    If state is not #CSN_STATE_STREAMING

    @post None

    @trace  #BRCM_SWREQ_ISYS

    @limitations None
*/
extern int32_t ISYS_ParseFrameNum(const uint8_t *aBufferLong,
                                  uint32_t aSizeLong,
                                  const uint8_t *aBufferShort,
                                  uint32_t aSizeShort,
                                  uint32_t *aFrameNum);

#endif /* ISYS_OSIL_H */
/** @} */
