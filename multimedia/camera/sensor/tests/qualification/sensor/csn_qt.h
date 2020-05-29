/*****************************************************************************
 Copyright 2019 Broadcom Limited.  All rights reserved.

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
    WARRANTIES, EITHER EXPRESS, IPPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IPPLIED
    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COPPLETENESS,
    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
    SOFTWARE.

 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
    OR EXEPPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
******************************************************************************/

/**
    @defgroup grp_csn_qt Qualification Tests
    @ingroup grp_csn

    @addtogroup grp_csn_qt
    @{

    @file csn_qt.h
    @brief Camera Sensor Qualification Test
    This file contains test cases and test sequences for camera sensor test
    application.
    @version 0.1 Initial version
*/

#ifndef CSN_QT_H
#define CSN_QT_H

/**
    @brief Safe state notification from camera sensor

    @pre Camera sensor is streaming and induce a fault

    @test Test if CSN_SafeStateCb is invoked

    @post CSN_SafeStateCb is invoked

    @functional     Yes

    @type           Neg

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
#define BRCM_SWQTST_CSN_SAFETY_NOTIFICATION   (0UL)

/**
    @brief Recovery after safe state movement

    @pre Camera sensor entered safe state

    @test Reinitialize, configure and start camera

    @post Camera starts streaming successfully

    @functional     Yes

    @type           Neg

    @trace #BRCM_SWREQ_CSN_FUNCTIONAL_SAFETY
*/
#define BRCM_SWQTST_CSN_SAFETY_RECOVERY   (1UL)

/** @brief Safety framework integration and functional verification sequence

    Detailed description of BRCM_SWQTSEQ_CSN_SAFETY_SEQ0

    @code{.c}
    Initialize Camera Sensor
    Configure Camera Sensor
    Start Camera Sensor
    Induce Safety error in camera sensor through I2C write
    Check Expected Result for BRCM_SWQTST_CSN_SAFETY_NOTIFICATION
    Deinitialize Camera Sensor
    Initialize Camera Sensor
    Configure Camera Sensor
    Start Camera Sensor
    Check Expected Result for BRCM_SWQTST_CSN_SAFETY_RECOVERY
    Stop Camera Sensor
    Deinitialize Camera Sensor
    @endcode

    @board bcm89107_ox3a_evk
    @board bcm89103_ox3a_evk

    @auto           Yes

    @type           Sanity

    @testproc #BRCM_SWQTST_CSN_SAFETY_NOTIFICATION
    @testproc #BRCM_SWQTST_CSN_SAFETY_RECOVERY

*/
#define BRCM_SWQTSEQ_CSN_SAFETY_SEQ0   (1UL)

#endif /* CSN_QT_H */

/** @} */
