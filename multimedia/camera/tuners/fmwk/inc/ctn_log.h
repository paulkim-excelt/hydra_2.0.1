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
    @defgroup grp_ctn_log_ifc Tuner Logging
    @ingroup grp_tuners

    @addtogroup grp_ctn_log_ifc
    @{
    @section sec_ctn_log_overview Overview
    @todo Add overview

    @section sec_ctn_log_fsm State Machine
    @todo Add state machine

    @section sec_ctn_log_seq Sequence Diagrams
    @todo Add sequence diagram

    @section sec_ctn_log_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   TBD                                             |
    | Memory Bandwidth  |   TBD                                             |
    | HW Utilization    |   TBD                                             |

    @todo Update performance numbers
    @includedoc ctn_log.dep

    @limitations Add component limitations here

    @file ctn_log.h
    @brief Logging APIs for Tuning algorithms
    @version 0.1 Initial version
*/

#ifndef CTN_LOG_H
#define CTN_LOG_H

#include<stdint.h>
#include<bcm_err.h>

/**
    @name CTN logging API IDs
    @{
    @brief API IDs for CTN Logging
*/
#define BRCM_SWARCH_CTN_LOG_UDP_PORT_MACRO (0x0U)  /**< @brief #CTN_LOG_UDP_PORT */
#define BRCM_SWARCH_CTN_LOG_ID_TYPE        (0x1U)  /**< @brief #CTN_LogIdType    */
#define BRCM_SWARCH_CTN_LOG_HDR_TYPE       (0x2U)  /**< @brief #CTN_LogHdrType   */
#define BRCM_SWARCH_CTN_INIT_LOG_PROC      (0x3U)  /**< @brief #CTN_InitLog      */
#define BRCM_SWARCH_CTN_DEINIT_LOG_PROC    (0x4U)  /**< @brief #CTN_DeinitLog    */
#define BRCM_SWARCH_CTN_LOG_PROC           (0x5U)  /**< @brief #CTN_Log          */
#define BRCM_SWARCH_CTN_PROCESS_LOGS       (0x6U)  /**< @brief #CTN_ProcessLogs  */
/** @} */

/**
    @brief Remote UDP port for Tuner logging

    Remote UDP Port on which log buffers will be sent to
    Remote IP address is fixed to 192.168.10.2

    @trace #BRCM_SWREQ_TUNER_LOG
*/
#define CTN_LOG_UDP_PORT (58901U)

/**
    @name Tuner Logging IDs
    @{
    @brief Camera Tuner Specific Logging ID

    First three bytes will be 'CTN' in ASCII format.
    Last byte represents tuner specific ID

    @trace #BRCM_SWREQ_CTN_LOG
*/
typedef uint32_t CTN_LogIdType; /**< @brief Tuner Log ID Type */
#define CTN_LOG_ID_AGC     (0x43544E00UL) /**< @brief AGC Tuner Log ID             */
#define CTN_LOG_ID_AWB     (0x43544E01UL) /**< @brief AWB Tuner Log ID             */
#define CTN_LOG_ID_DENOISE (0x43544E02UL) /**< @brief Denoise Tuner Log ID         */
#define CTN_LOG_ID_CC      (0x43544E03UL) /**< @brief CC Tuner Log ID              */
#define CTN_LOG_ID_DEFPIX  (0x43544E04UL) /**< @brief Defective Pixel Tuner Log ID */
#define CTN_LOG_ID_SHARPEN (0x43544E05UL) /**< @brief Sharpening Tuner Log ID      */
#define CTN_LOG_ID_BL      (0x43544E06UL) /**< @brief Black Level Tuner Log ID     */
#define CTN_LOG_ID_TM      (0x43544E07UL) /**< @brief Tone Mapping Tuner Log ID    */
/** @} */

/**
    @brief Camera Tuner Log Header

    This structure depicts Log Header. Data
    buffer will be followed

    @trace #BRCM_SWREQ_CTN_LOG
 */
typedef struct _CTN_LogHdrType {
    CTN_LogIdType  moduleId; /**< @brief CTN_LogIdType serialized as 0x43, 0x54,
                                                        0x4E, tuner type byte */
    uint8_t  dataType;      /**< @brief Individual tuner specific data type (as
                                            provided by caller of #CTN_Log)   */
    uint8_t  rsvd0[3UL];    /**< @brief Reserved space                        */
    uint32_t frameNo;       /**< @brief Frame number (as provided by caller of
                                                                  #CTN_Log)   */
    uint32_t timeSec;       /**< @brief time (seconds part in big endian format)
                                                                              */
    uint32_t timeNsec;      /**< @brief time (nanoseconds part in big endian
                                                                      format) */
    uint8_t  rsvd1[8UL];    /**< @brief Reserved space                        */
} CTN_LogHdrType;

/** @brief Initialize Tuner Logging

    @behavior Sync, Non-reentrant

    @pre None

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Success
    @retval     #BCM_ERR_UNKNOWN    Failed to initialize
    @retval     #BCM_ERR_NOSUPPORT  Tuner logging is not supported

    @post None

    @trace #BRCM_SWREQ_CTN_LOG

    @limitations None
*/
extern int32_t CTN_InitLog(void);

/** @brief De-Initialize Tuner Logging

    @behavior Sync, Non-reentrant

    @pre Tuner log is initialized

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK         Success
    @retval     #BCM_ERR_UNKNOWN    Failed to de-initialize
    @retval     #BCM_ERR_NOSUPPORT  Tuner logging is not supported

    @post None

    @trace #BRCM_SWREQ_CTN_LOG

    @limitations None
*/
extern int32_t CTN_DeinitLog(void);

/** @brief Log Tuner structure over UDP (Ethernet)

    This API queues buffer (with header) to a queue
    and triggers a UDP transport of the earliest log
    in queue. If this API fails to transport the buffer,
    client shall re-attempt transport of the buffer.
    Individual tuners will be unaware of this behavior.
    Individual tuners will just be calling this API to
    queue the buffer and rest will be handled by framework.

    @behavior Sync, Non-reentrant

    @pre Tuner logging shall be Initialized

    @param[in]      aLogId      Unique ID defined per Tuner (@ref CTN_LogIdType)
    @param[in]      aDataType   8-bit data type specific to individual tuner
    @param[in]      aFrameNo    Frame number associated with data
    @param[in]      aData       Data pointer to be logged
    @param[in]      aSize       Size of Data

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Success
    @retval     #BCM_ERR_NOMEM          Queue is full, no space to hold buffer
    @retval     #BCM_ERR_INVAL_STATE    Tuner is not in initialized state
    @retval     #BCM_ERR_INVAL_PARAMS   (aData is NULL and aSize is non-Zero)
                                        or (aLogId does not have top 24-bits
                                        matching pattern)
    @retval     #BCM_ERR_NOSUPPORT      Tuner logging is not supported

    @post None

    @trace #BRCM_SWREQ_CTN_LOG

    @limitations None
*/
extern int32_t CTN_Log(CTN_LogIdType aLogId,
                  uint8_t aDataType,
                  uint32_t aFrameNo,
                  uint8_t *aData,
                  uint16_t aSize);

/** @brief Process Tuner Logs

    @behavior Sync, Non-reentrant

    @pre Tuner logging shall be Initialized

    @param      void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK          Success
    @retval     #BCM_ERR_UNKNOWN     Failed to process logs
    @retval     #BCM_ERR_INVAL_STATE Tuner is not in initialized state
    @retval     #BCM_ERR_NOSUPPORT   Tuner logging is not supported

    @post None

    @trace #BRCM_SWREQ_CTN_LOG

    @limitations None
*/
extern int32_t CTN_ProcessLogs(void);

#endif /* CTN_LOG_H */
/** @} */