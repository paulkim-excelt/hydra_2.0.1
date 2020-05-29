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
 * @file cam_erika.h
 *
 * @brief Erika-OS specific data structures.
 *
 * This file exposes the erika-OS specific data structures that are
 * used by the camera sub-system.
*/

#ifndef CAM_ERIKA_H
#define CAM_ERIKA_H

#include "ee.h"

/** \def ISP_EVENT
    \brief A macro that report about event from ISP.
*/
#define ISP_EVENT       SystemEvent0

/** \def UNICAM_EVENT
    \brief A macro that report about event from UNICAM.
*/
#define UNICAM_EVENT     SystemEvent1

/** \def CSN_EVENT_IIC
    \brief A macro that report about event from IIC.
*/
#define CSN_EVENT_IIC SystemEvent2

/** \def TUNER_EVENT_ERROR
    \brief A macro that report about error from tuner.
*/
#define TUNER_EVENT_ERROR SystemEvent3

/** \def TUNER_EVENT_CMD_COMPLETE
    \brief A macro that report about command complete from tuner.
*/
#define TUNER_EVENT_CMD_COMPLETE SystemEvent4

/** \def CAM_EVENT_CQUEUE
    \brief A macro that message is posted on command queue.
*/
#define CAM_EVENT_CQUEUE    SystemEvent5

/** \def CAM_EVENT_FRAME_CAPTURE
    \brief A macro that FRAME CAPTURE is triggered (only for tuning).
*/
#define CAM_EVENT_FRAME_CAPTURE    SystemEvent6

/** \def CAM_EVENT_PTM
    \brief A macro that report about command complete from PTM.
*/
#define CAM_EVENT_PTM    SystemEvent7

/** \def CAM_EVENT_CSN_SAFE_STATE
    \brief A macro that report about safe state movement of camera.
*/
#define CAM_EVENT_CSN_SAFE_STATE    SystemEvent11

/** \def CAM_EVENT_CSN_RESET
    \brief A macro that report about CSN reset events.
*/
#define CAM_EVENT_CSN_RESET         SystemEvent12

/** \def ISP_STATS_EVENT
    \brief A macro that report about statistics event from ISP.
*/
#define ISP_STATS_EVENT       SystemEvent0

/** \def SENSOR_META_EVENT
    \brief A macro that report about metadata event from sensor.
*/
#define SENSOR_META_EVENT     SystemEvent1

/** \def SENSOR_STATS_EVENT
    \brief A macro that report about statistics event from Sensor.
*/
#define SENSOR_STATS_EVENT    SystemEvent3

/** \def TUNER_EVENT_QUIT
    \brief A macro that report tuner quit event.
*/
#define TUNER_EVENT_QUIT SystemEvent4

/** \def CAM_FW_CMD_EVENTS
    \brief A macro that abstracts camera cmd events.
*/
#define CAM_EVENTS (ISP_EVENT | \
                UNICAM_EVENT | \
                CSN_EVENT_IIC | \
                TUNER_EVENT_ERROR | \
                CAM_EVENT_CQUEUE | \
                TUNER_EVENT_CMD_COMPLETE | \
                PP_GPEvent)

#endif /* CAM_ERIKA_H */
