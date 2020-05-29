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
    @defgroup grp_csn_impl Implementation
    @ingroup grp_csn

    @addtogroup grp_csn_impl
    @{
    @section sec_csn_impl_overview Overview
    @todo Add overview

    @section sec_csn_impl_fsm State Machine
    @todo Add state diagram

    @section sec_csn_impl_seq Sequence Diagrams
    @todo Add sequence diagram

    @file camera_sensor.c
    @brief CSN Implementation
    This source file contains the implementation of functions for CSN
    @version 0.1 Initial version
*/

/* Includes */
#include <string.h>
#include <system.h>
#include <i2c.h>
#include <avt.h>
#include <clk.h>
#include <mcu.h>
#include <camera_sensor.h>
#include <camera_sensor_board.h>
#include <camera_sensor_osil.h>
#include <camera_sensor_module.h>
#include <cam_erika.h>
/**
    @name CSN Design IDs
    @{
    @brief Design IDs for CSN
*/
#define BRCM_SWDSGN_CSN_CFG_RCVD_MACRO                      (0x80U)   /**< @brief #CSN_CFG_RCVD_COMMON       */
#define BRCM_SWDSGN_CSN_MAX_SS_CMD_SIZE_MACRO               (0x81U)   /**< @brief #CSN_MAX_SS_CMD_SIZE       */
#define BRCM_SWDSGN_CSN_MAX_START_CMDS_MACRO                (0x82U)   /**< @brief #CSN_MAX_START_CMDS        */
#define BRCM_SWDSGN_CSN_MAX_STOP_CMDS_MACRO                 (0x83U)   /**< @brief #CSN_MAX_STOP_CMDS         */
#define BRCM_SWDSGN_CSN_MAX_NUM_PKTS_MACRO                  (0x84U)   /**< @brief #CSN_MAX_NUM_PKTS          */
#define BRCM_SWDSGN_CSN_MAX_SETCFG_CMD_SIZE_MACRO           (0x85U)   /**< @brief #CSN_MAX_SETCFG_CMD_SIZE   */
#define BRCM_SWDSGN_CSN_DELAY_IN_MS_MACRO                   (0x86U)   /**< @brief #CSN_DELAY_IN_MS           */
#define BRCM_SWDSGN_CSN_IIC_RETRY_COUNT_MACRO               (0x87U)   /**< @brief #CSN_IIC_RETRY_COUNT       */
#define BRCM_SWDSGN_CSN_SS_CFG_DATA_TYPE                    (0x88U)   /**< @brief #CSN_SS_CfgDataType        */
#define BRCM_SWDSGN_CSN_SET_CFG_DATA_TYPE                   (0x89U)   /**< @brief #CSN_SetCfgDataType        */
#define BRCM_SWDSGN_CSN_CLIENT_INFO_TYPE                    (0x8aU)   /**< @brief #CSN_ClientInfoType        */
#define BRCM_SWDSGN_CSN_CONTEXT_TYPE_TYPE                   (0x8bU)   /**< @brief #CSN_ContextType           */
#define BRCM_SWDSGN_CSN_CONTEXT_GLOBAL                      (0x8cU)   /**< @brief #CSN_Context               */
#define BRCM_SWDSGN_CSN_JOB_ID_GLOBAL                       (0x8dU)   /**< @brief #CSN_JobID                 */
#define BRCM_SWDSGN_CSN_REPORT_ERROR_PART_PROC              (0x8eU)   /**< @brief #CSN_ReportError           */
#define BRCM_SWDSGN_CSN_INIT_STATE_PART_PROC                (0x8fU)   /**< @brief #CSN_InitState             */
#define BRCM_SWDSGN_CSN_START_INTERNAL_PART_PROC            (0x90U)   /**< @brief #CSN_StartInternal         */
#define BRCM_SWDSGN_CSN_STOP_INTERNAL_PART_PROC             (0x91U)   /**< @brief #CSN_StopInternal          */
#define BRCM_SWDSGN_CSN_CONFIGURE_INTERNAL_PART_PROC        (0x92U)   /**< @brief #CSN_ConfigureInternal     */
#define BRCM_SWDSGN_CSN_SEND_IIC_PACKETS_PART_PROC          (0x93U)   /**< @brief #CSN_SendIICSetPackets     */
#define BRCM_SWDSGN_CSN_PROCESS_CAM_EVENT_PART_PROC         (0x94U)   /**< @brief #CSN_ProcessCamEvent       */
#define BRCM_SWDSGN_CSN_PROCESS_TUNER_EVENT_PART_PROC       (0x95U)   /**< @brief #CSN_ProcessTunerEvent     */
#define BRCM_SWDSGN_CSN_GET_CLOSEST_EXPOSURE_PART_PROC      (0x96U)   /**< @brief #CSN_GetClosestExposure    */
#define BRCM_SWDSGN_CSN_UPDATE_SENSOR_SET_PACKET_PART_PROC  (0x97U)   /**< @brief #CSN_UpdateSensorSetPacket */
#define BRCM_SWDSGN_CSN_PROCESS_EVENT_PROC                  (0x98U)   /**< @brief #CSN_ProcessEvent          */
#define BRCM_SWDSGN_CSN_GET_STATE_PROC                      (0x99U)   /**< @brief #CSN_GetState              */
#define BRCM_SWDSGN_CSN_INIT_PROC                           (0x9aU)   /**< @brief #CSN_Init                  */
#define BRCM_SWDSGN_CSN_DEINIT_PROC                         (0x9bU)   /**< @brief #CSN_Deinit                */
#define BRCM_SWDSGN_CSN_START_PROC                          (0x9cU)   /**< @brief #CSN_Start                 */
#define BRCM_SWDSGN_CSN_STOP_PROC                           (0x9dU)   /**< @brief #CSN_Stop                  */
#define BRCM_SWDSGN_CSN_CONFIGURE_PROC                      (0x9eU)   /**< @brief #CSN_Configure             */
#define BRCM_SWDSGN_CSN_GET_PARAM_PROC                      (0x9fU)   /**< @brief #CSN_GetParam              */
#define BRCM_SWDSGN_CSN_SET_PARAM_PROC                      (0xa0U)   /**< @brief #CSN_SetParam              */
#define BRCM_SWDSGN_CSN_TEST_PARAM_PROC                     (0xa1U)   /**< @brief #CSN_TestParam             */
#define BRCM_SWDSGN_CSN_PARSE_METADATA_PROC                 (0xa2U)   /**< @brief #CSN_ParseMetadata         */
#define BRCM_SWDSGN_CSN_PARSE_STATISTICS_PROC               (0xa3U)   /**< @brief #CSN_ParseStatistics       */
#define BRCM_SWDSGN_CSN_RESET_STATE_TYPE                    (0xa4U)   /**< @brief #CSN_ResetStateType        */
#define BRCM_SWDSGN_CSN_POST_RESET_INIT                     (0xa5U)   /**< @brief #CSN_PostResetInit         */
/** @} */

/**
    @name Camera Sensor programming status
    @{
    @brief Macro for Camera Sensor programming status

    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_CFG_RCVD_COMMON   (0x1UL)    /**< @brief Received common configuration */
#define CSN_CFG_RCVD_MODE     (0x2UL)    /**< @brief Received mode configuration   */
#define CSN_CFG_RCVD_START    (0x4UL)    /**< @brief Received start configuration  */
#define CSN_CFG_RCVD_STOP     (0x8UL)    /**< @brief Received stop configuration   */
#define CSN_CFG_RCVD_ALL      (0x1fUL)   /**< @brief Received all configurationa   */
/** @} */

/**
    @brief Max Start-Stop cmd size

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_MAX_SS_CMD_SIZE          (8U)

/**
    @brief Maximum number of commands in start

    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_MAX_START_CMDS           (8U)

/**
    @brief Maximum number of commands in stop

    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_MAX_STOP_CMDS            (8U)

/**
    @brief Maximum number of packets

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_MAX_NUM_PKTS             (100UL)

/**
    @brief Set config command size

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_MAX_SETCFG_CMD_SIZE      (128UL)

/**
    @brief Delay function

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION
*/
#define CSN_DELAY_IN_MS(time_ms) \
{                                \
    BCM_CpuNDelay((time_ms) * 1000UL * 1000UL);\
}

/**
    @brief Maximum number of retry in case of IIC errors

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
#define CSN_IIC_RETRY_COUNT             (0x3UL)

/**
    @brief Reset state type

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_GET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_TEST_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_METADATA_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
typedef uint32_t CSN_ResetStateType;
#define CSN_RESET_STATE_NONE        (0U)
#define CSN_RESET_STATE_IN_PROGRESS (1U)
#define CSN_RESET_STATE_COMPLETED   (2U)
#define CSN_RESET_STATE_ERROR       (3U)

/**
    @brief Start/Stop config data

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
 */
typedef struct _CSN_SS_CfgDataType {
    uint16_t cmdType;                       /**< @brief Command type    */
    uint16_t cmdLen;                        /**< @brief Command length  */
    uint8_t  cmdData[CSN_MAX_SS_CMD_SIZE];  /**< @brief Command data    */
} CSN_SS_CfgDataType;

/**
    @brief Set configuration data

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT
 */
typedef struct _CSN_SetCfgDataType {
    uint16_t numPkts;                           /**< @brief Number of packets */
    uint16_t cmdLen;                            /**< @brief Command length    */
    uint8_t  cmdData[CSN_MAX_SETCFG_CMD_SIZE];  /**< @brief Command data      */
} CSN_SetCfgDataType;

/**
    @brief Camera Client State

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_GET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_INIT
 */
typedef struct _CSN_ClientInfoType {
    uint32_t is_I2cCmdPosted;           /**< @brief Is i2c command posted? */
    uint32_t retryCount;                /**< @brief Retry count            */
    uint32_t pendingConfigs;            /**< @brief Pending configs        */
    IIC_PktType pkt[CSN_MAX_NUM_PKTS];  /**< @brief I2C packet             */
} CSN_ClientInfoType;

/**
    @brief CSN global context

    Detailed structure description.
    Structure name shall be in PascalCase

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_GET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_TEST_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_METADATA_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC
    @trace #BRCM_SWREQ_CSN_INIT
 */
typedef struct _CSN_ContextType {
    uint8_t stateChangeInProgress;
    CSN_ResetStateType resetState;
    uint32_t state;
    uint32_t prevState;
    uint32_t configRcvd;
    uint32_t slaveAddr;
    uint32_t i2cPort;
    uint32_t i2cFrequency;    /* in Hz */
    uint32_t sensorFrequency; /* in Hz */
    uint32_t addrWidth;       /* in bits */
    uint32_t dataWidth;       /* in bits */
    uint32_t endianness;
    CSN_SS_CfgDataType startCfg[CSN_MAX_START_CMDS];
    uint32_t numStartCmd;
    CSN_SS_CfgDataType stopCfg[CSN_MAX_STOP_CMDS];
    uint32_t numStopCmd;
    uint32_t lineLength;              /* In 'ns' */
    uint32_t integrationMin;          /* Min lines for integration */
    uint32_t coarseIntegrationOffset; /* Fine integration offset in
                                         percent of a full line */
    uint32_t minFrameIntegrationDiff; /* Minimum lines difference
                                         between frame length and integration
                                         length */
    uint32_t minFrameLength;
    uint32_t maxFrameLength;
    CSN_SetCfgDataType setCfg;
    uint32_t setInProgress;
    CSN_ClientInfoType cInfo[CSN_CLIENT_IDX_MAX];
    uint32_t i2cFlags;
    CSN_EmbOffsetsType embOffsets;
} CSN_ContextType;

/**
    @brief Global context for CSN

    Detailed description of the GComp_Context

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_GET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_TEST_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_METADATA_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
static CSN_ContextType CSN_Context =
{
    .state = CSN_STATE_POWEROFF,
    .resetState = CSN_RESET_STATE_NONE,
    .prevState = CSN_STATE_POWEROFF,
    .stateChangeInProgress = 0U,
    .configRcvd = 0x0UL,
    .slaveAddr = 0x0UL,
    .i2cPort = 0x0UL,
    .i2cFrequency = 0x0UL,
    .sensorFrequency = 0x0UL,
    .addrWidth = 0x0UL,
    .dataWidth = 0x0UL,
    .endianness = 0x0UL,
    .numStopCmd = 0x0UL,
    .numStartCmd = 0x0UL,
    .lineLength = 0UL,
    .integrationMin = 0UL,
    .coarseIntegrationOffset = 0UL,
    .minFrameIntegrationDiff = 0UL,
    .minFrameLength = 0UL,
    .maxFrameLength = 0UL,
    .i2cFlags = 0UL,
};

/**
    @brief Message Queue info for IIC clients of CSN

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_INIT
*/
static uint32_t CSN_JobID[CSN_CLIENT_IDX_MAX];

/** @brief CSN Report Error method

    Wrapper for BCM_ReportError.

    @behavior Sync, Re-entrant

    @pre None

    @param[in]      aApiID      API ID for the method
    @param[in]      aErr        Error type
    @param[in]      aVal1       First uint32_t data to be printed
    @param[in]      aVal2       Second uint32_t data to be printed
    @param[in]      aVal3       Third uint32_t data to be printed

    @return void

    @post None

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWARCH_CSN_GET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_TEST_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_METADATA_PROC
    @trace #BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @limitations None

    @code{.unparsed}
    Call #BCM_ReportError
    @endcode
*/
static void CSN_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_CSN_ID, 0U, aApiID, aErr, 4UL, values);
}

/** @brief Init CSN state

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static void CSN_InitState(void)
{
    uint32_t i = 0UL;

    CSN_Context.state = CSN_STATE_POWEROFF;
    CSN_Context.prevState = CSN_STATE_POWEROFF;
    CSN_Context.stateChangeInProgress = 0U;
    CSN_Context.resetState = CSN_RESET_STATE_NONE;
    for (i=0UL; i<CSN_CLIENT_IDX_MAX; i++) {
        CSN_Context.cInfo[i].is_I2cCmdPosted = 0UL;
        CSN_Context.cInfo[i].retryCount = CSN_IIC_RETRY_COUNT;
        CSN_Context.cInfo[i].pendingConfigs = 0UL;
    }
    CSN_Context.configRcvd = 0x0UL,
    CSN_Context.slaveAddr = 0x0UL,
    CSN_Context.i2cPort = 0x0UL,
    CSN_Context.i2cFrequency = 0x0UL,
    CSN_Context.sensorFrequency = 0x0UL,
    CSN_Context.addrWidth = 0x0UL,
    CSN_Context.dataWidth = 0x0UL,
    CSN_Context.endianness = 0x0UL,
    CSN_Context.numStopCmd = 0x0UL,
    CSN_Context.numStartCmd = 0x0UL,
    CSN_Context.lineLength = 0UL,
    CSN_Context.integrationMin = 0UL,
    CSN_Context.coarseIntegrationOffset = 0UL,
    CSN_Context.minFrameIntegrationDiff = 0UL,
    CSN_Context.minFrameLength = 0UL,
    CSN_Context.maxFrameLength = 0UL,
    CSN_Context.i2cFlags = 0UL,
    CSN_Context.setInProgress = 0UL;
    CSN_Context.setCfg.numPkts = 0U;
    CSN_Context.setCfg.cmdLen = 0U;
    for (i = 0; i< ISYS_SENSOR_EXP_IDX_MAX; i++) {
        CSN_Context.embOffsets.metaOffset[i] = 0U;
        CSN_Context.embOffsets.metaSize[i] = 0U;
        CSN_Context.embOffsets.statsOffset[i] = 0U;
        CSN_Context.embOffsets.statsSize[i] = 0U;
    }
}

/** @brief Internal function for CSN Start

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_START_PROC
    @trace #BRCM_SWREQ_CSN

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_StartInternal(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t numPackets = 0UL;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

    for (i = (CSN_Context.numStartCmd - cInfo->pendingConfigs); i < CSN_Context.numStartCmd; i++) {
        if (CSN_CMD_DELAY == CSN_Context.startCfg[i].cmdType) {
            uint32_t delay;

            if (16UL == CSN_Context.dataWidth) {
                if (CSN_ENDIAN_BIG == CSN_Context.endianness) {
                    delay = (uint32_t)((uint32_t)
                              CSN_Context.startCfg[i].cmdData[2U] << 8) +
                            (uint32_t)((uint32_t)
                              CSN_Context.startCfg[i].cmdData[3U]);
                } else {
                    delay = (uint32_t)((uint32_t)
                              CSN_Context.startCfg[i].cmdData[3U] << 8) +
                            (uint32_t)((uint32_t)
                              CSN_Context.startCfg[i].cmdData[2U]);
                }
            } else {
                delay = (uint32_t) CSN_Context.startCfg[i].cmdData[2U];
            }

            CSN_DELAY_IN_MS(delay);
            cInfo->pendingConfigs--;
        } else if (CSN_CMD_REG_WRITE == CSN_Context.startCfg[i].cmdType) {
            cInfo->pkt[numPackets].flags = CSN_Context.i2cFlags;
            cInfo->pkt[numPackets].slaveAddr = CSN_Context.slaveAddr;
            cInfo->pkt[numPackets].length = CSN_Context.startCfg[i].cmdLen;
            cInfo->pkt[numPackets].data = CSN_Context.startCfg[i].cmdData;
            numPackets++;
            cInfo->is_I2cCmdPosted = 1UL;
            /* IIC Write */
            ret = IIC_Transfer(CSN_Context.i2cPort,
                                  &cInfo->pkt[0],
                                  numPackets, CSN_EVENT_IIC,
                                  &CSN_JobID[CSN_CLIENT_IDX_CAMERA]);
            if (ret != BCM_ERR_OK) {
                CSN_ReportError(BRCM_SWDSGN_CSN_START_INTERNAL_PART_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                /* TODO: Retry */
            } else {
                cInfo->pendingConfigs--;
            }
            break;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWDSGN_CSN_START_INTERNAL_PART_PROC,
                    ret, CSN_Context.startCfg[i].cmdType, 0UL, 0UL, 0UL);
        }
    }

    if (BCM_ERR_OK == ret) {
        if ((0UL != cInfo->is_I2cCmdPosted) || (0UL != cInfo->pendingConfigs)) {
            ret = BCM_ERR_BUSY;
        }
    }

    return ret;
}

/** @brief Internal function for Sensor stop

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWREQ_CSN

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_StopInternal(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t numPackets = 0UL;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

    for (i = (CSN_Context.numStopCmd - cInfo->pendingConfigs); i < CSN_Context.numStopCmd; i++) {
        if (CSN_CMD_DELAY == CSN_Context.stopCfg[i].cmdType) {
            uint32_t delay;

            if (16UL == CSN_Context.dataWidth) {
                if (CSN_ENDIAN_BIG == CSN_Context.endianness) {
                    delay = (uint32_t)((uint32_t)
                               CSN_Context.stopCfg[i].cmdData[2U] << 8) +
                            (uint32_t)((uint32_t)
                               CSN_Context.stopCfg[i].cmdData[3U]);
                } else {
                    delay = (uint32_t)((uint32_t)
                               CSN_Context.stopCfg[i].cmdData[3U] << 8) +
                            (uint32_t)((uint32_t)
                               CSN_Context.stopCfg[i].cmdData[2U]);
                }
            } else {
                delay = CSN_Context.stopCfg[i].cmdData[2U];
            }

            CSN_DELAY_IN_MS(delay);
            cInfo->pendingConfigs--;
        } else if (CSN_CMD_REG_WRITE == CSN_Context.stopCfg[i].cmdType) {
            cInfo->pkt[numPackets].flags = CSN_Context.i2cFlags;
            cInfo->pkt[numPackets].slaveAddr = CSN_Context.slaveAddr;
            cInfo->pkt[numPackets].length = CSN_Context.stopCfg[i].cmdLen;
            cInfo->pkt[numPackets].data = CSN_Context.stopCfg[i].cmdData;
            numPackets++;
            cInfo->is_I2cCmdPosted = 1UL;
            /* IIC Write */
            ret = IIC_Transfer(CSN_Context.i2cPort,
                                  &cInfo->pkt[0],
                                  numPackets, CSN_EVENT_IIC,
                                  &CSN_JobID[CSN_CLIENT_IDX_CAMERA]);
            if (ret != BCM_ERR_OK) {
                CSN_ReportError(BRCM_SWDSGN_CSN_STOP_INTERNAL_PART_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                /* TODO: Retry */
            } else {
                cInfo->pendingConfigs--;
            }
            break;
        } else {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWDSGN_CSN_STOP_INTERNAL_PART_PROC,
                    ret, CSN_Context.startCfg[i].cmdType, 0UL, 0UL, 0UL);
        }
    }

    if (BCM_ERR_OK == ret) {
        if ((0UL != cInfo->is_I2cCmdPosted) || (0UL != cInfo->pendingConfigs)) {
            ret = BCM_ERR_BUSY;
        }
    }

    return ret;
}

/** @brief Internal function for Sensor configure

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_CONFIGURE_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_ConfigureInternal(const CSN_ImageType *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t dataOffset;
    uint32_t numPackets = 0UL;
    uint32_t numConfigs = aCfg->imgHdr.numConfigs;
    uint32_t cfgIndex;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

    cfgIndex = (numConfigs - cInfo->pendingConfigs);

    for (i = cfgIndex; i < numConfigs; i++)
    {
        if (CSN_CFG_COMMON == aCfg->cfgs[i].cfgType ||
                CSN_CFG_MODE == aCfg->cfgs[i].cfgType) {
            dataOffset = aCfg->cfgs[i].dataOffset;
            if (CSN_CMD_DELAY == aCfg->cfgs[i].cmdType) {
                uint32_t delay;
                uint32_t hiIdx = dataOffset + 3UL;
                uint32_t loIdx = dataOffset + 2UL;

                if (16UL == CSN_Context.dataWidth) {
                    if (CSN_ENDIAN_BIG == CSN_Context.endianness) {
                        delay = (uint32_t)((uint32_t)
                                  aCfg->cameraSensorCfgData[loIdx] << 8) +
                                (uint32_t)((uint32_t)
                                  aCfg->cameraSensorCfgData[hiIdx]);
                    } else {
                        delay = (uint32_t)((uint32_t)
                                  CSN_Context.stopCfg[i].cmdData[hiIdx] << 8) +
                                (uint32_t)((uint32_t)
                                  aCfg->cameraSensorCfgData[loIdx]);
                    }
                } else {
                    delay = (uint32_t) aCfg->cameraSensorCfgData[loIdx];
                }

                CSN_DELAY_IN_MS(delay);
                cInfo->pendingConfigs--;
            } else if (CSN_CMD_REG_WRITE == aCfg->cfgs[i].cmdType) {
                cInfo->pkt[numPackets].flags = CSN_Context.i2cFlags;
                cInfo->pkt[numPackets].slaveAddr = CSN_Context.slaveAddr;
                cInfo->pkt[numPackets].length = aCfg->cfgs[i].length;
                cInfo->pkt[numPackets].data = (((CSN_ImageType *)
                            (aCfg))->cameraSensorCfgData + dataOffset);
                numPackets++;
                cInfo->is_I2cCmdPosted = 1UL;
                cInfo->pendingConfigs--;
                break;
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWDSGN_CSN_CONFIGURE_INTERNAL_PART_PROC,
                        ret, CSN_Context.startCfg[i].cmdType, 0UL, 0UL, 0UL);
            }
        } else {
            cInfo->pendingConfigs--;
        }
    }

    /* IIC Write */
    if (0UL != cInfo->is_I2cCmdPosted) {
        ret = IIC_Transfer(CSN_Context.i2cPort,
                              &cInfo->pkt[0],
                              numPackets, CSN_EVENT_IIC,
                              &CSN_JobID[CSN_CLIENT_IDX_CAMERA]);
        if (ret != BCM_ERR_OK) {
            CSN_ReportError(BRCM_SWDSGN_CSN_CONFIGURE_INTERNAL_PART_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
    }

    if (BCM_ERR_OK == ret) {
        if ((0UL != cInfo->is_I2cCmdPosted) || (0UL != cInfo->pendingConfigs)) {
            ret = BCM_ERR_BUSY;
        }
    }

    return ret;
}

/** @brief Send I2C packets

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_SendIICSetPackets(void)
{
    int32_t ret = BCM_ERR_OK;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_TUNER];

    /* Send packets to IIC */
    if (0UL != cInfo->pendingConfigs) {
        ret = IIC_Transfer(CSN_Context.i2cPort,
                            &cInfo->pkt[CSN_Context.setCfg.numPkts -
                            cInfo->pendingConfigs],
                            1UL, CSN_EVENT_IIC, &CSN_JobID[CSN_CLIENT_IDX_TUNER]);
        if (ret != BCM_ERR_OK) {
            CSN_ReportError(BRCM_SWDSGN_CSN_SEND_IIC_PACKETS_PART_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        } else {
            cInfo->is_I2cCmdPosted = 1UL;
            cInfo->pendingConfigs--;
            CSN_Context.setInProgress = 1UL;
        }
    }

func_exit:
    return ret;
}

/** @brief Init sequence after sensor reset

    As camera sensor reset could be asyncronous, the set of operations after
    sensor reset has been organised inside this function. This function could
    be invoked either from CSN_Init or CSN_ProcessCamEvent based on the sensor
    reset behavior.

    @behavior Sync

    @pre Camera senor reset is complete

    @param void

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK       Init has been compled successfully
    @retval     #BCM_ERR_UNKNOWN  Init has failed and CSN transitioned to error state

    @post CSN has been transitioned to INIT state

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @limitations None

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_PostResetInit(void)
{
    int32_t ret;

    ret = CSN_BoardEnableSafeStateCb();
    if (BCM_ERR_OK == ret) {
        CSN_Context.state = CSN_STATE_INITIALISED;
    } else {
        CSN_Context.state = CSN_STATE_ERROR;
        ret = BCM_ERR_UNKNOWN;
    }

    return ret;
}

/** @brief Process CAM events for CSN

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static uint32_t CSN_ProcessCamEvent(CSN_EventSourceType aEventType)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t mask = CSN_STATUS_NONE;

    /* Check for IIC Response */
    if (CSN_EVENT_SOURCE_IIC == aEventType) {
        CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

        if (0UL != cInfo->is_I2cCmdPosted) {
            ret = IIC_GetStatus(CSN_Context.i2cPort, CSN_JobID[CSN_CLIENT_IDX_CAMERA]);
            if (BCM_ERR_BUSY == ret) {
                cInfo->is_I2cCmdPosted = 1UL;
                mask |= CSN_STATUS_BUSY;
                ret = BCM_ERR_OK;
                goto func_exit;
            } else if (BCM_ERR_OK != ret) {
                cInfo->is_I2cCmdPosted = 0UL;
                /* Retry for 3 times */
                if (0UL != cInfo->retryCount) {
                    cInfo->pendingConfigs++;
                    cInfo->retryCount--;
                } else {
                    CSN_ReportError(BRCM_SWDSGN_CSN_PROCESS_CAM_EVENT_PART_PROC,
                            ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                    goto func_exit;
                }
            } else {
                cInfo->is_I2cCmdPosted = 0UL;
                cInfo->retryCount = CSN_IIC_RETRY_COUNT;
                ret = BCM_ERR_OK;
            }
        }
    } else if (CSN_EVENT_SOURCE_RESET == aEventType) {
        ret = CSN_BoardResetHandler();
        if (BCM_ERR_OK == ret) {
            CSN_Context.resetState = CSN_RESET_STATE_COMPLETED;
            mask = CSN_STATUS_CMD_COMPLETE;
            ret = CSN_PostResetInit();
        } else if (BCM_ERR_BUSY == ret) {
            mask = CSN_STATUS_BUSY;
            ret = BCM_ERR_OK;
        } else {
            CSN_Context.resetState = CSN_RESET_STATE_ERROR;
            ret = BCM_ERR_UNKNOWN;
        }
    } else if (CSN_EVENT_SOURCE_SAFETY == aEventType) {
        /* Let the CSN move to error state */
        ret = BCM_ERR_UNKNOWN;
    } else {
        CSN_ReportError(BRCM_SWDSGN_CSN_PROCESS_CAM_EVENT_PART_PROC,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
    }

func_exit:
    if (BCM_ERR_OK != ret) {
        CSN_Context.prevState = CSN_Context.state;
        CSN_Context.state = CSN_STATE_ERROR;
        CSN_Context.stateChangeInProgress = 0U;
        mask |= CSN_STATUS_ERROR;
    }

    return mask;
}

/** @brief Process Tuner events for CSN

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static uint32_t CSN_ProcessTunerEvent(void)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t mask = CSN_STATUS_NONE;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_TUNER];

    /* Check for IIC Response */
    if (0UL != cInfo->is_I2cCmdPosted) {
        ret = IIC_GetStatus(CSN_Context.i2cPort, CSN_JobID[CSN_CLIENT_IDX_TUNER]);
        if (BCM_ERR_BUSY == ret) {
            cInfo->is_I2cCmdPosted = 1UL;
            mask |= CSN_STATUS_BUSY;
            ret = BCM_ERR_OK;
            goto func_exit;
        } else if ((BCM_ERR_OK != ret) && (CSN_STATE_STREAMING == CSN_Context.state)) {
            cInfo->is_I2cCmdPosted = 0UL;
            /* Retry for 3 times */
            if (0UL != cInfo->retryCount) {
                cInfo->pendingConfigs++;
                cInfo->retryCount--;
            } else {
                CSN_ReportError(BRCM_SWDSGN_CSN_PROCESS_TUNER_EVENT_PART_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                goto func_exit;
            }
        } else {
            cInfo->is_I2cCmdPosted = 0UL;
            cInfo->retryCount = CSN_IIC_RETRY_COUNT;
            ret = BCM_ERR_OK;
        }
    }

    /* State Check */
    if (0UL != CSN_Context.setInProgress) {
        if ((0UL == cInfo->pendingConfigs) || (CSN_STATE_STREAMING != CSN_Context.state)) {
            cInfo->pendingConfigs = 0UL;
            CSN_Context.setInProgress = 0UL;
            CSN_Context.setCfg.numPkts = 0U;
            CSN_Context.setCfg.cmdLen = 0U;
            mask |= CSN_STATUS_CMD_COMPLETE;
        } else {
            mask |= CSN_STATUS_BUSY;
            ret = CSN_SendIICSetPackets();
        }
    }

func_exit:
    if (BCM_ERR_OK != ret) {
        CSN_Context.setInProgress = 0UL;
        mask |= CSN_STATUS_ERROR;
    }
    return mask;
}

/** @brief Get closest exposure

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static uint32_t CSN_GetClosestExposure(uint32_t aExposure)
{
    uint32_t integrationLineWanted, exposureCorrection;
    uint32_t integrationLines;
    uint32_t exposureCurrent = aExposure;

    if (0UL != CSN_Context.lineLength) {
        aExposure++;

        exposureCorrection = (CSN_Context.coarseIntegrationOffset *
                CSN_Context.lineLength) / (100000UL);
        integrationLineWanted = (1000UL * (aExposure + exposureCorrection))/
            CSN_Context.lineLength;

        if ( integrationLineWanted <= CSN_Context.integrationMin ) {
            integrationLines = CSN_Context.integrationMin;
        } else if ( integrationLineWanted > (CSN_Context.minFrameLength -
                    CSN_Context.minFrameIntegrationDiff) ) {
            if (integrationLineWanted > (CSN_Context.maxFrameLength -
                        CSN_Context.minFrameIntegrationDiff)) {
                integrationLines = (CSN_Context.maxFrameLength -
                        CSN_Context.minFrameIntegrationDiff);
            } else {
                integrationLines = integrationLineWanted;
            }
        } else {
            integrationLines = integrationLineWanted;
        }

        exposureCurrent = ((integrationLines *
                    CSN_Context.lineLength) / (1000UL)) -
            exposureCorrection;
    }

    return exposureCurrent;
}

/** @brief Update sensor packet

    Detailed description of COMP_FunctionPart

    @behavior Sync/Async, Re-entrant/Non-reentrant

    @pre describe any pre-condition

    @param[in]      aArg1       Brief description of aArg1
    @param[inout]   aArg2       Brief description of aArg2
    @param[out]     aArg3       Brief description of aArg3

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             condition when this is returned
    @retval     #BCM_ERR_INVAL_PARAMS   condition when this is returned
    @return void (for void functions)

    @post describe post condition

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE

    @limitations any limitations for this api

    Detailed design of this function to be populated here.
    Insert flowchart or pseudocode.

    @code{.unparsed}
    Check for Null pointer
    @endcode

*/
static int32_t CSN_UpdateSensorSetPacket(CSN_MODULE_SetOpsType *aSetOps)
{
    int32_t ret = BCM_ERR_OK;
    int32_t continuePkt = 0L;
    uint32_t pktStartIdx = 0UL;
    uint32_t i = 0UL;
    uint32_t one = 1UL;
    uint32_t dataWidth = CSN_Context.dataWidth;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_TUNER];

    /* Common module driver assumes the data to be 8 bit width */
    if ((NULL == CSN_MODULE_GetModule()) ||
        (NULL == CSN_MODULE_GetModule()->getOps)) {
        dataWidth = 8UL;
    }

    for (i=0UL; i < aSetOps->count; i++) {

        if (CSN_Context.setCfg.cmdLen >= (CSN_MAX_SETCFG_CMD_SIZE - 3UL)) {
            ret = BCM_ERR_NOMEM;
            CSN_ReportError(BRCM_SWDSGN_CSN_UPDATE_SENSOR_SET_PACKET_PART_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        }

        if (CSN_Context.setCfg.numPkts >= CSN_MAX_NUM_PKTS) {
            ret = BCM_ERR_NOMEM;
            CSN_ReportError(BRCM_SWDSGN_CSN_UPDATE_SENSOR_SET_PACKET_PART_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        }

        if (0UL != i) {
            /* check for continuity */
            continuePkt = (int32_t)((dataWidth>>3UL) ==
                    (aSetOps->op[i].reg - aSetOps->op[i - one].reg));
            if (0L != continuePkt) {
                CSN_Context.setCfg.numPkts--;
            }
        }

        if (0L == continuePkt) {
            cInfo->pkt[CSN_Context.setCfg.numPkts].flags = CSN_Context.i2cFlags;
            cInfo->pkt[CSN_Context.setCfg.numPkts].slaveAddr = (uint16_t) CSN_Context.slaveAddr;
            cInfo->pkt[CSN_Context.setCfg.numPkts].data =
                &CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen];
            pktStartIdx = CSN_Context.setCfg.cmdLen;

            if (CSN_ENDIAN_BIG == CSN_Context.endianness) {
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t)((aSetOps->op[i].reg >> 8UL) & 0xffUL);
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t) (aSetOps->op[i].reg & 0xffUL);
            } else if (CSN_ENDIAN_LITTLE == CSN_Context.endianness){
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t)(aSetOps->op[i].reg & 0xffUL);
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t) ((aSetOps->op[i].reg >> 8UL) & 0xffUL);
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWDSGN_CSN_UPDATE_SENSOR_SET_PACKET_PART_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                goto func_exit;
            }
        }

        if (16UL == dataWidth) {
            if (CSN_ENDIAN_BIG == CSN_Context.endianness) {
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t)((aSetOps->op[i].val >> 8UL) & 0xffUL);
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t) (aSetOps->op[i].val & 0xffUL);
            } else if (CSN_ENDIAN_LITTLE == CSN_Context.endianness){
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t)(aSetOps->op[i].val & 0xffUL);
                CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                    (uint8_t) ((aSetOps->op[i].val >> 8UL) & 0xffUL);
            } else {
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWDSGN_CSN_UPDATE_SENSOR_SET_PACKET_PART_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                goto func_exit;
            }
        } else {
            CSN_Context.setCfg.cmdData[CSN_Context.setCfg.cmdLen++] =
                (uint8_t) (aSetOps->op[i].val & 0xffUL);
        }

        /* Update Length */
        cInfo->pkt[CSN_Context.setCfg.numPkts].length =
            (uint16_t)(CSN_Context.setCfg.cmdLen - pktStartIdx);

        /* Increment Num packtes */
        CSN_Context.setCfg.numPkts++;
    }

func_exit:
    return ret;
}
/** @} */

/**
    @addtogroup grp_csn_ifc
    @{
*/

/**
    Detailed design of CSN_ProcessEvent
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_PROCESS_EVENT_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
uint32_t CSN_ProcessEvent(uint32_t aClient,
                          CSN_EventSourceType aEventType)
{
    uint32_t mask = CSN_STATUS_NONE;

    if (CSN_CLIENT_IDX_TUNER == aClient) {
        mask = CSN_ProcessTunerEvent();
    } else {
        mask = CSN_ProcessCamEvent(aEventType);
    }

    return mask;
}

/**
    Detailed design of CSN_GetState
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_GET_STATE_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
CSN_StateType CSN_GetState(void)
{
    return CSN_Context.state;
}

/**
    Detailed design of CSN_Init
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_INIT_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_Init(void)
{
    int32_t ret = BCM_ERR_OK;

    if (CSN_STATE_POWEROFF != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_INIT_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
    } else {
        if (CSN_RESET_STATE_NONE == CSN_Context.resetState) {
            CSN_InitState();
            ret  = CSN_BoardResetReq();
            if (BCM_ERR_OK == ret) {
                CSN_Context.resetState = CSN_RESET_STATE_COMPLETED;
            } else if (BCM_ERR_BUSY == ret) {
                CSN_Context.resetState = CSN_RESET_STATE_IN_PROGRESS;
            } else {
                CSN_Context.resetState = CSN_RESET_STATE_ERROR;
            }
        }

        if (CSN_RESET_STATE_IN_PROGRESS == CSN_Context.resetState) {
            ret = BCM_ERR_BUSY;
        } else if (CSN_RESET_STATE_COMPLETED == CSN_Context.resetState) {
            ret = CSN_PostResetInit();
        } else { /* CSN_RESET_STATE_ERROR */
            CSN_Context.state = CSN_STATE_ERROR;
            ret = BCM_ERR_UNKNOWN;
        }
    }

    return ret;
}

/**
    Detailed design of CSN_Deinit
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_DEINIT_PROC
    @trace #BRCM_SWREQ_CSN_INIT

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_Deinit(void)
{
    int32_t ret = BCM_ERR_OK;

    if (CSN_STATE_POWEROFF == CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_DEINIT_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
    } else {
        ret = CSN_BoardDisableSafeStateCb();
        if (BCM_ERR_OK != ret) {
            CSN_ReportError(BRCM_SWARCH_CSN_DEINIT_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        }
        CSN_InitState();
    }

    return ret;
}

/**
    Detailed design of CSN_Start
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWREQ_CSN

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_Start(void)
{
    int32_t ret = BCM_ERR_OK;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

    if (CSN_STATE_READY != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_START_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        if (0U == CSN_Context.stateChangeInProgress) {
            CSN_Context.stateChangeInProgress = 1U;
            cInfo->pendingConfigs = CSN_Context.numStartCmd;
        }
        /* Call Sensor Start */
        ret = CSN_StartInternal();
        if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
            ret = BCM_ERR_UNKNOWN;
        } else {
            if (BCM_ERR_OK == ret) {
                CSN_Context.prevState = CSN_Context.state;
                CSN_Context.state = CSN_STATE_STREAMING;
                CSN_Context.stateChangeInProgress = 0U;
            }
        }
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_Stop
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_STOP_PROC
    @trace #BRCM_SWREQ_CSN

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_Stop(void)
{
    int32_t ret = BCM_ERR_OK;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_STOP_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    } else {
        if (0U == CSN_Context.stateChangeInProgress) {
            CSN_Context.stateChangeInProgress = 1U;
            cInfo->pendingConfigs = CSN_Context.numStopCmd;
        }
        /* Call Sensor Stop */
        ret = CSN_StopInternal();
        if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
            ret = BCM_ERR_UNKNOWN;
        } else {
            if (BCM_ERR_OK == ret) {
                CSN_Context.prevState = CSN_Context.state;
                CSN_Context.state = CSN_STATE_READY;
                CSN_Context.stateChangeInProgress = 0U;
            }
        }
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_Configure
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_CONFIGURATION

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_Configure(const CSN_ImageType *aCfg)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t i;
    uint32_t j;
    uint8_t startCfgIndex = 0U;
    uint16_t startDataIndex = 0U;
    uint8_t stopCfgIndex = 0U;
    uint16_t stopDataIndex = 0U;
    uint32_t ncoFrequency = 0UL;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_CAMERA];
    CSN_MODULE_ConfigType mConfig;

    if ((CSN_STATE_INITIALISED != CSN_Context.state) &&
            (CSN_STATE_READY != CSN_Context.state)) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    /* Check Parameter */
    if (NULL == aCfg) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }
    if (CSN_CONFIG != aCfg->imgHdr.commonHdr.cfgType) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.commonHdr.cfgType, 0UL, 0UL);
        goto func_exit;
    }
    if (CSN_MAGIC !=  aCfg->imgHdr.commonHdr.cfgMagic) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.commonHdr.cfgType, 0UL, 0UL);
        goto func_exit;
    }
    if (aCfg->imgHdr.numConfigs > CSN_MAX_CFGS) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t) __LINE__, aCfg->imgHdr.commonHdr.cfgType, 0UL, 0UL);
        goto func_exit;
    }
    if (0UL ==  aCfg->imgHdr.lineLength) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t)(__LINE__), 0UL, 0UL, 0UL);
        goto func_exit;
    }
    if (0UL ==  aCfg->imgHdr.fpsHigh) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t)(__LINE__), 0UL, 0UL, 0UL);
        goto func_exit;
    }
    if (0UL ==  aCfg->imgHdr.fpsLow) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                ret, (uint32_t)(__LINE__), 0UL, 0UL, 0UL);
        goto func_exit;
    }

    /* Save to Global Data */
    CSN_Context.slaveAddr = aCfg->imgHdr.slaveAddr;
    CSN_Context.i2cPort = aCfg->imgHdr.i2cPort;
    CSN_Context.i2cFrequency = aCfg->imgHdr.i2cFrequency;
    CSN_Context.sensorFrequency = aCfg->imgHdr.sensorFrequency;
    CSN_Context.addrWidth = aCfg->imgHdr.addrWidth;
    CSN_Context.dataWidth = aCfg->imgHdr.dataWidth;
    CSN_Context.endianness = aCfg->imgHdr.endianness;
    CSN_Context.lineLength = aCfg->imgHdr.lineLength;
    CSN_Context.integrationMin = aCfg->imgHdr.integrationMin;
    CSN_Context.coarseIntegrationOffset = aCfg->imgHdr.coarseIntegrationOffset;
    CSN_Context.minFrameIntegrationDiff = aCfg->imgHdr.minFrameIntegrationDiff;
    CSN_Context.minFrameLength = BCM_NS_PER_SEC/(CSN_Context.lineLength *
            aCfg->imgHdr.fpsHigh);
    CSN_Context.maxFrameLength = BCM_NS_PER_SEC/(CSN_Context.lineLength *
            aCfg->imgHdr.fpsLow);

    for (i = 0; i< ISYS_SENSOR_EXP_IDX_MAX; i++) {
        CSN_Context.embOffsets.metaOffset[i] = aCfg->imgHdr.metaOffset[i];
        CSN_Context.embOffsets.metaSize[i] = aCfg->imgHdr.metaSize[i];
        CSN_Context.embOffsets.statsOffset[i] = aCfg->imgHdr.statsOffset[i];
        CSN_Context.embOffsets.statsSize[i] = aCfg->imgHdr.statsSize[i];
    }

    /* Form IIC default flags */
    CSN_Context.i2cFlags = 0UL;
    if (0UL != aCfg->imgHdr.is_RestartRequired) {
        CSN_Context.i2cFlags = IIC_PKT_FLG_RESTART;
    }
    if (0UL != aCfg->imgHdr.is_10bitSlave) {
        CSN_Context.i2cFlags |= IIC_PKT_FLG_SLAVE_10BIT;
    }

    /* Configure Sensor module driver */
    mConfig.bpp = (uint8_t) aCfg->imgHdr.bpp;
    mConfig.isHDR = (uint8_t) aCfg->imgHdr.isHDR;
    mConfig.imgWidth = (uint16_t) aCfg->imgHdr.imgWidth;

    /* Configure specific module driver if exists */
    if ((NULL != CSN_MODULE_GetModule()) &&
        (NULL != CSN_MODULE_GetModule()->sensorConfigure)) {
        ret = CSN_MODULE_GetModule()->sensorConfigure(&mConfig);
        if (BCM_ERR_OK != ret) {
            CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                    ret, (__LINE__), 0UL, 0UL, 0UL);
            goto func_exit;
        }
    }

    /* Configure common module driver if exists */
    if ((NULL != CSN_MODULE_GetCommonModule()) &&
        (NULL != CSN_MODULE_GetCommonModule()->sensorConfigure)) {
        ret = CSN_MODULE_GetCommonModule()->sensorConfigure(&mConfig);
        if (BCM_ERR_OK != ret) {
            CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                    ret, (__LINE__), 0UL, 0UL, 0UL);
            goto func_exit;
        }
    }

    /* Check NCO Clock */
    ncoFrequency = MCU_GetClkFreq(MCU_CLK_CFG_ID_CAM);
    if ((CSN_Context.sensorFrequency != ncoFrequency) &&
                    (0UL != CSN_Context.sensorFrequency)) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                                 ret,
                                 (uint32_t) __LINE__,
                                 ncoFrequency,
                                 CSN_Context.sensorFrequency, 0UL);
        goto func_exit;
    }

    /* Check for Configs */
    for (i = 0UL; i < (aCfg->imgHdr.numConfigs); i++) {
        switch (aCfg->cfgs[i].cfgType) {
            case CSN_CFG_COMMON:
                CSN_Context.configRcvd |= CSN_CFG_RCVD_COMMON;
                break;
            case CSN_CFG_MODE:
                CSN_Context.configRcvd |= CSN_CFG_RCVD_MODE;
                break;
            case CSN_CFG_START:
                if (startCfgIndex >= CSN_MAX_START_CMDS) {
                    ret = BCM_ERR_INVAL_PARAMS;
                    CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                            ret, (uint32_t) __LINE__, startCfgIndex, 0UL, 0UL);
                } else if (aCfg->cfgs[i].length > CSN_MAX_SS_CMD_SIZE) {
                    ret = BCM_ERR_INVAL_PARAMS;
                    CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                            ret, (uint32_t) __LINE__, aCfg->cfgs[i].length, 0UL, 0UL);
                } else {
                    CSN_Context.startCfg[startCfgIndex].cmdType =  aCfg->cfgs[i].cmdType;
                    CSN_Context.startCfg[startCfgIndex].cmdLen  =  aCfg->cfgs[i].length;
                    startDataIndex =  aCfg->cfgs[i].dataOffset;
                    for (j = 0UL; j < CSN_Context.startCfg[startCfgIndex].cmdLen; j++) {
                        CSN_Context.startCfg[startCfgIndex].cmdData[j] = aCfg->cameraSensorCfgData[startDataIndex];
                        startDataIndex++;
                    }
                    startCfgIndex++;
                    CSN_Context.configRcvd |= CSN_CFG_RCVD_START;
                }
                break;
            case CSN_CFG_STOP:
                if (stopCfgIndex >= CSN_MAX_STOP_CMDS) {
                    ret = BCM_ERR_INVAL_PARAMS;
                    CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                            ret, (uint32_t) __LINE__, stopCfgIndex, 0UL, 0UL);
                } else if (aCfg->cfgs[i].length > CSN_MAX_SS_CMD_SIZE) {
                    ret = BCM_ERR_INVAL_PARAMS;
                    CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                            ret, (uint32_t) __LINE__, aCfg->cfgs[i].length, 0UL, 0UL);
                } else {
                    CSN_Context.stopCfg[stopCfgIndex].cmdType =  aCfg->cfgs[i].cmdType;
                    CSN_Context.stopCfg[stopCfgIndex].cmdLen = aCfg->cfgs[i].length;
                    stopDataIndex = aCfg->cfgs[i].dataOffset;
                    for (j = 0UL; j < CSN_Context.stopCfg[stopCfgIndex].cmdLen; j++) {
                        CSN_Context.stopCfg[stopCfgIndex].cmdData[j] = aCfg->cameraSensorCfgData[stopDataIndex];
                        stopDataIndex++;
                    }
                    stopCfgIndex++;
                    CSN_Context.configRcvd |= CSN_CFG_RCVD_STOP;
                }
                break;
            case CSN_CFG_CALIBRATION:
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                break;
            case CSN_CFG_DYNAMIC:
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                break;
            default:
                ret = BCM_ERR_INVAL_PARAMS;
                CSN_ReportError(BRCM_SWARCH_CSN_CONFIGURE_PROC,
                        ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
                break;
        }
        if (BCM_ERR_OK != ret) {
            break;
        }
    }

    if (BCM_ERR_OK != ret) {
        goto func_exit;
    }

    /* Update Start/Stop command count */
    if ((0UL != startCfgIndex) && (0UL != stopCfgIndex)) {
        CSN_Context.numStartCmd = startCfgIndex;
        CSN_Context.numStopCmd = stopCfgIndex;
    }

    if (0U == CSN_Context.stateChangeInProgress) {
        CSN_Context.stateChangeInProgress = 1U;

        /* Update Num Configs */
        cInfo->pendingConfigs = aCfg->imgHdr.numConfigs;
    }

    ret = CSN_ConfigureInternal(aCfg);
    if ((BCM_ERR_OK != ret) && (BCM_ERR_BUSY != ret)) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        if (BCM_ERR_OK == ret) {
            CSN_Context.prevState = CSN_Context.state;
            CSN_Context.state = CSN_STATE_READY;
            CSN_Context.stateChangeInProgress = 0U;
        }
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_GetParam
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_GetParam(CSN_ParamsType *aParam)
{
    /* TODO: Implement if required */
    int32_t ret = BCM_ERR_NOSUPPORT;

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_GET_PARAM_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if (NULL == aParam) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_GET_PARAM_PROC,
                ret, (uint32_t)(__LINE__), 0UL, 0UL, 0UL);
        goto func_exit;
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_SetParam
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_SET_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_SetParam(const CSN_ParamsType *aParam)
{
    int32_t ret = BCM_ERR_OK;
    CSN_MODULE_ExpParamType expP;
    CSN_MODULE_AWB_ParamType wb;
    CSN_ClientInfoType *cInfo = &CSN_Context.cInfo[CSN_CLIENT_IDX_TUNER];
    uint32_t opsType = CSN_MODULE_OP_NONE;
    uint32_t oldPktCnt = CSN_Context.setCfg.numPkts;

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if (NULL == aParam) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if (0UL != (aParam->type & (CSN_PARAM_EXPOSURE_LONG |
                                CSN_PARAM_EXPOSURE_SHORT |
                                CSN_PARAM_GAIN_LONG |
                                CSN_PARAM_GAIN_SHORT))) {
        uint32_t shutter;

        if (NULL == aParam->expParam) {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                    ret, (uint32_t) __LINE__, __LINE__, 0UL, 0UL);
            goto func_exit;
        }

        if (0UL == CSN_Context.lineLength) {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                    ret, (uint32_t)(__LINE__), 0UL, 0UL, 0UL);
            goto func_exit;
        }

        /* Fill all parameters */
        shutter = aParam->expParam->shutter[CSN_MODULE_EXP_IDX_LONG];
        /* Round off */
        shutter++;
        expP.expLines[CSN_MODULE_EXP_IDX_LONG] =
                (uint16_t)((shutter * 1000UL) / (CSN_Context.lineLength));
        expP.gain[CSN_MODULE_EXP_IDX_LONG] =
                aParam->expParam->gain[CSN_MODULE_EXP_IDX_LONG];

        shutter = aParam->expParam->shutter[CSN_MODULE_EXP_IDX_SHORT];
        /* Round off */
        shutter++;
        expP.expLines[CSN_MODULE_EXP_IDX_SHORT] =
                (uint16_t)((shutter * 1000UL) / (CSN_Context.lineLength));
        expP.gain[CSN_MODULE_EXP_IDX_SHORT] =
                aParam->expParam->gain[CSN_MODULE_EXP_IDX_SHORT];

        expP.expRatio = (uint16_t) aParam->expParam->expRatio;
        /* Update flicker period */
        expP.minFlickerLines = (uint16_t)
                              ((aParam->expParam->flickerPeriod * 1000UL) /
                              (CSN_Context.lineLength));
    }

    if (0UL != (aParam->type & CSN_PARAM_EXPOSURE_LONG)) {
        opsType |= CSN_MODULE_OP_EXPOSURE_LONG;
    }

    if (0UL != (aParam->type & CSN_PARAM_EXPOSURE_SHORT)) {
        opsType |= CSN_MODULE_OP_EXPOSURE_SHORT;
    }

    if (0UL != (aParam->type & CSN_PARAM_GAIN_LONG)) {
        opsType |= CSN_MODULE_OP_GAIN_LONG;
    }

    if (0UL != (aParam->type & CSN_PARAM_GAIN_SHORT)) {
        opsType |= CSN_MODULE_OP_GAIN_SHORT;
    }

    if (0UL != (aParam->type & CSN_PARAM_WB)) {
        if (NULL == aParam->wbParam) {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                ret, (uint32_t) __LINE__, __LINE__, 0UL, 0UL);
            goto func_exit;
        } else {
            wb.gainR = aParam->wbParam->gainR;
            wb.gainB = aParam->wbParam->gainB;
        }
        opsType |= CSN_MODULE_OP_WB;
    }

    if (CSN_MODULE_OP_NONE != opsType) {
        CSN_MODULE_OpsParamType opsParam;
        CSN_MODULE_SetOpsType ops;

        ops.count = 0UL;
        opsParam.expParam = &expP;
        opsParam.wbParam = &wb;
        if ((NULL != CSN_MODULE_GetModule()) &&
            (NULL != CSN_MODULE_GetModule()->getOps)) {
            ret = CSN_MODULE_GetModule()->getOps(opsType, &opsParam, &ops);
        } else {
            if ((NULL != CSN_MODULE_GetCommonModule()) &&
                (NULL != CSN_MODULE_GetCommonModule()->getOps)) {
                ret = CSN_MODULE_GetCommonModule()->getOps(opsType, &opsParam, &ops);
            } else {
                ret = BCM_ERR_NOSUPPORT;
            }
        }

        /* Process status and process if OK */
        if ((BCM_ERR_OK != ret) || ops.count > CSN_MODULE_MAX_SET_OPS) {
            CSN_ReportError(BRCM_SWARCH_CSN_SET_PARAM_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        } else {
            ret = CSN_UpdateSensorSetPacket(&ops);
            if (BCM_ERR_OK != ret) {
                goto func_exit;
            }
        }
    }

    cInfo->pendingConfigs += (CSN_Context.setCfg.numPkts - oldPktCnt);

    if (0UL == cInfo->is_I2cCmdPosted) {
        ret = CSN_SendIICSetPackets();
    }

func_exit:
    if((BCM_ERR_OK != ret) && ((BCM_ERR_INVAL_PARAMS != ret) &&
       (BCM_ERR_INVAL_STATE != ret) && (BCM_ERR_NOSUPPORT != ret))){
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
}

/**
    Detailed design of CSN_TestParam
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_TEST_PARAM_PROC
    @trace #BRCM_SWREQ_CSN_PARAM_UPDATE

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_TestParam(CSN_ParamsType *aParam)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t exposure = 0UL;

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_TEST_PARAM_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if (NULL == aParam) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_TEST_PARAM_PROC,
                ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if (0UL != (aParam->type & (CSN_PARAM_EXPOSURE_LONG |
                                CSN_PARAM_EXPOSURE_SHORT |
                                CSN_PARAM_GAIN_LONG |
                                CSN_PARAM_GAIN_SHORT))) {
        if (NULL == aParam->expParam) {
            ret = BCM_ERR_INVAL_PARAMS;
            CSN_ReportError(BRCM_SWARCH_CSN_TEST_PARAM_PROC,
                    ret, (uint32_t) __LINE__, __LINE__, 0UL, 0UL);
            goto func_exit;
        }
    }

    if (0UL != (aParam->type & CSN_PARAM_EXPOSURE_LONG)) {
        exposure = CSN_GetClosestExposure(aParam->expParam->shutter[CSN_MODULE_EXP_IDX_LONG]);
        aParam->expParam->shutter[CSN_MODULE_EXP_IDX_LONG] = exposure;
    }

    if (0UL != (aParam->type & CSN_PARAM_EXPOSURE_SHORT)) {
        exposure = CSN_GetClosestExposure(aParam->expParam->shutter[CSN_MODULE_EXP_IDX_SHORT]);
        aParam->expParam->shutter[CSN_MODULE_EXP_IDX_SHORT] = exposure;
    }

    if (0UL != (aParam->type & CSN_PARAM_GAIN_LONG)) {
        if ((NULL != CSN_MODULE_GetModule()) &&
            (NULL != CSN_MODULE_GetModule()->getClosestGain)) {
            ret = CSN_MODULE_GetModule()->getClosestGain(CSN_MODULE_OP_GAIN_LONG,
                    &(aParam->expParam->gain[CSN_MODULE_EXP_IDX_LONG]));
        } else {
            if ((NULL != CSN_MODULE_GetCommonModule()) &&
                (NULL != CSN_MODULE_GetCommonModule()->getClosestGain)) {
                ret = CSN_MODULE_GetCommonModule()->getClosestGain(CSN_MODULE_OP_GAIN_LONG,
                    &(aParam->expParam->gain[CSN_MODULE_EXP_IDX_LONG]));
            } else {
                ret = BCM_ERR_NOSUPPORT;
            }
        }
        if (BCM_ERR_OK != ret) {
            CSN_ReportError(BRCM_SWARCH_CSN_TEST_PARAM_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        }
    }

    if (0UL != (aParam->type & CSN_PARAM_GAIN_SHORT)) {
        if ((NULL != CSN_MODULE_GetModule()) &&
            (NULL != CSN_MODULE_GetModule()->getClosestGain)) {
            ret = CSN_MODULE_GetModule()->getClosestGain(CSN_MODULE_OP_GAIN_SHORT,
                    &(aParam->expParam->gain[CSN_MODULE_EXP_IDX_SHORT]));
        } else {
            if ((NULL != CSN_MODULE_GetCommonModule()) &&
                (NULL != CSN_MODULE_GetCommonModule()->getClosestGain)) {
                ret = CSN_MODULE_GetCommonModule()->getClosestGain(CSN_MODULE_OP_GAIN_SHORT,
                        &(aParam->expParam->gain[CSN_MODULE_EXP_IDX_SHORT]));
            } else {
                ret = BCM_ERR_NOSUPPORT;
            }
        }
        if (BCM_ERR_OK != ret) {
            CSN_ReportError(BRCM_SWARCH_CSN_TEST_PARAM_PROC,
                    ret, (uint32_t) __LINE__, 0UL, 0UL, 0UL);
            goto func_exit;
        }
    }

func_exit:
    if((BCM_ERR_OK != ret) && ((BCM_ERR_INVAL_PARAMS != ret) &&
       (BCM_ERR_INVAL_STATE != ret) && (BCM_ERR_NOSUPPORT != ret))){
        ret = BCM_ERR_UNKNOWN;
    }
    return ret;
}

/**
    Detailed design of CSN_ParseMetadata
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_PARSE_METADATA_PROC
    @trace #BRCM_SWREQ_CSN_PARSE_ED

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
int32_t CSN_ParseMetadata(const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                          uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                          CSN_MODULE_MetadataType *aMetadata)
{
    int32_t ret = BCM_ERR_OK;

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_PARSE_METADATA_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if ((NULL == aBuffer) || (NULL == aMetadata)) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_PARSE_METADATA_PROC,
                ret, 0UL, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if ((NULL != CSN_MODULE_GetModule()) &&
        (NULL != CSN_MODULE_GetModule()->parseMetadata)) {
        ret = CSN_MODULE_GetModule()->parseMetadata(aBuffer,
                                                    aSize,
                                                    aMetadata);
    } else {
        if ((NULL != CSN_MODULE_GetCommonModule()) &&
            (NULL != CSN_MODULE_GetCommonModule()->parseMetadata)) {
            ret = CSN_MODULE_GetCommonModule()->parseMetadata(aBuffer,
                                                              aSize,
                                                              aMetadata);
        } else {
            ret = BCM_ERR_NOSUPPORT;
        }
    }

    if (BCM_ERR_OK != ret) {
        ret = BCM_ERR_UNKNOWN;
    } else {
        /* Convert exposure value from number of lines to micro seconds */
        aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG] = (aMetadata->shutter[CSN_MODULE_EXP_IDX_LONG]
                * CSN_Context.lineLength) / 1000UL;
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_ParseStatistics
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC
    @trace #BRCM_SWREQ_CSN_PARSE_ED

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
extern int32_t CSN_ParseStatistics(const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                                   uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                   CSN_MODULE_StatsType * const (aStats)[CSN_MODULE_EXP_IDX_MAX],
                                   uint32_t *aParsedMask)
{
    int32_t ret = BCM_ERR_OK;

    if (CSN_STATE_STREAMING != CSN_Context.state) {
        ret = BCM_ERR_INVAL_STATE;
        CSN_ReportError(BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC,
                ret, CSN_Context.state, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if ((NULL == aBuffer) || (NULL == aStats)) {
        ret = BCM_ERR_INVAL_PARAMS;
        CSN_ReportError(BRCM_SWARCH_CSN_PARSE_STATISTICS_PROC,
                ret, 0UL, 0UL, 0UL, 0UL);
        goto func_exit;
    }

    if ((NULL != CSN_MODULE_GetModule()) &&
        (NULL != CSN_MODULE_GetModule()->parseStatistics)) {
        ret = CSN_MODULE_GetModule()->parseStatistics(aBuffer,
                                                      aSize,
                                                      aStats,
                                                      aParsedMask);
    } else {
        if ((NULL != CSN_MODULE_GetCommonModule()) &&
            (NULL != CSN_MODULE_GetCommonModule()->parseStatistics)) {
            ret = CSN_MODULE_GetCommonModule()->parseStatistics(aBuffer,
                                                                aSize,
                                                                aStats,
                                                                aParsedMask);
        } else {
            ret = BCM_ERR_NOSUPPORT;
        }
    }
    if (BCM_ERR_OK != ret) {
        ret = BCM_ERR_UNKNOWN;
    }

func_exit:
    return ret;
}

/**
    Detailed design of CSN_GetEmbOffsets
    Insert flowchart or pseudocode.

    @trace #BRCM_SWARCH_CSN_GET_EMB_OFFSETS_PROC
    @trace #BRCM_SWREQ_CSN_PARSE_ED

    @code{.c}
    ret = ERR_OK
    if Arguments are NULL
        ret = ERR_INVAL_PARAMS
    else if state is not COMP_STATE1
        ret = ERR_INVAL_STATE
    else
        ret = COMP_FunctionPart
    return ret
    @endcode
*/
void CSN_GetEmbOffsets(CSN_EmbOffsetsType *aEmbOffsets)
{
    if (NULL != aEmbOffsets) {
        *aEmbOffsets = CSN_Context.embOffsets;
    }
}

/** @} */
