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
    @defgroup grp_csn_module_ifc Interface
    @ingroup grp_csn_module

    @addtogroup grp_csn_module_ifc
    @{
    @section sec_csn_overview Overview
    @image html csn_overview.jpg "Component Overview"
    Detailed Overview to be populated here

    @subsection subsec_csn_example Sample
    Details here

    @section sec_csn_fsm State Machine
    @todo Add state machine

    @section sec_csn_seq Sequence Diagrams
    @todo Add sequence diagram

    @section sec_csn_module_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   Update CPU load here                            |
    | Memory Bandwidth  |   Update Memory Bandwidth here                    |
    | HW Utilization    |   Update Hardware utilization here                |

    @limitations Add component limitations here

    @file camera_sensor.h
    @brief Camera Sensor Interface
    This header file contains the interface functions for CSN
    @version 0.1 Initial Version
*/
#ifndef CAMERA_SENSOR_MODULE_H
#define CAMERA_SENSOR_MODULE_H

#include <stdint.h>

/**
    @name CSN Module Architecture IDs
    @{
    @brief Architecture IDs for CSN Module
*/
#define BRCM_SWARCH_CSN_MODULE_MAX_SET_OPS_MACRO             (0x00U)   /**< @brief #CSN_MODULE_MAX_SET_OPS             */
#define BRCM_SWARCH_CSN_MODULE_NUM_EMB_HIST_BINS_MACRO       (0x01U)   /**< @brief #CSN_MODULE_NUM_EMB_HIST_BINS       */
#define BRCM_SWARCH_CSN_MODULE_OP_MACRO                      (0x02U)   /**< @brief #CSN_MODULE_OP_NONE                 */
#define BRCM_SWARCH_CSN_MODULE_STATS_PARSED_MACRO            (0x03U)   /**< @brief #CSN_MODULE_STATS_PARSED_NONE       */
#define BRCM_SWARCH_CSN_MODULE_EXP_IDX_TYPE                  (0x04U)   /**< @brief #CSN_MODULE_ExpIDX_Type             */
#define BRCM_SWARCH_CSN_MODULE_EXP_TIME_IDX_MACRO            (0x05U)   /**< @brief #CSN_MODULE_EXP_TIME_IDX_T1         */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_BYTES_FN_MACRO        (0x06U)   /**< @brief #CSN_MODULE_MAX_NUM_BYTES_FN        */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_BYTES_EXP_MACRO       (0x07U)   /**< @brief #CSN_MODULE_MAX_NUM_BYTES_EXP       */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_BYTES_GAIN_MACRO      (0x08U)   /**< @brief #CSN_MODULE_MAX_NUM_BYTES_GAIN      */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_BYTES_WB_OFFSET_MACRO (0x09U)   /**< @brief #CSN_MODULE_MAX_NUM_BYTES_WB_OFFSET */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_GAIN_TABLES_MACRO     (0x0aU)   /**< @brief #CSN_MODULE_MAX_NUM_GAIN_TABLES     */
#define BRCM_SWARCH_CSN_MODULE_MAX_GAIN_TABLE_SIZE _MACRO    (0x0bU)   /**< @brief #CSN_MODULE_MAX_GAIN_TABLE_SIZE     */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_REGS_EXP_MACRO        (0x0cU)   /**< @brief #CSN_MODULE_MAX_NUM_REGS_EXP        */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_REGS_WB_GAIN _MACRO   (0x0dU)   /**< @brief #CSN_MODULE_MAX_NUM_REGS_WB_GAIN    */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_REGS_WB_OFFSET_MACRO  (0x0eU)   /**< @brief #CSN_MODULE_MAX_NUM_REGS_WB_OFFSET  */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_REGS_GAIN_MACRO       (0x0fU)   /**< @brief #CSN_MODULE_MAX_NUM_REGS_GAIN       */
#define BRCM_SWARCH_CSN_MODULE_WG_GAIN_IDX_MACRO             (0x10U)   /**< @brief #CSN_MODULE_WG_GAIN_IDX_R           */
#define BRCM_SWARCH_CSN_MODULE_EXP_IDX_MACRO                 (0x11U)   /**< @brief #CSN_MODULE_MAX_GAIN_COMPS          */
#define BRCM_SWARCH_CSN_MODULE_GROUP_HOLD_IDX_MACRO          (0x12U)   /**< @brief #CSN_MODULE_GROUP_HOLD_IDX_START    */
#define BRCM_SWARCH_CSN_MODULE_MAX_GAIN_COMPS_MACRO          (0x13U)   /**< @brief #CSN_MODULE_MAX_NUM_GR_HLD_REGS     */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_GAIN_STAGES_MACRO     (0x14U)   /**< @brief #CSN_MODULE_MAX_NUM_GAIN_STAGES     */
#define BRCM_SWARCH_CSN_MODULE_INVALID_USHORT_MACRO          (0x15U)   /**< @brief #CSN_MODULE_INVALID_USHORT          */
#define BRCM_SWARCH_CSN_MODULE_UP8_UNITY_MACRO               (0x16U)   /**< @brief #CSN_MODULE_UP8_UNITY               */
#define BRCM_SWARCH_CSN_MODULE_UP12_UNITY_MACRO              (0x17U)   /**< @brief #CSN_MODULE_UP12_UNITY              */
#define BRCM_SWARCH_CSN_MODULE_CFG_SHIFT_MACRO               (0x18U)   /**< @brief #CSN_MODULE_CFG_SHIFT_NONE          */
#define BRCM_SWARCH_CSN_MODULE_SHIFT_DIR_MASK_MACRO          (0x19U)   /**< @brief #CSN_MODULE_SHIFT_DIR_MASK          */
#define BRCM_SWARCH_CSN_MODULE_SHIFT_COUNT_MASK_MACRO        (0x1aU)   /**< @brief #CSN_MODULE_SHIFT_COUNT_MASK        */
#define BRCM_SWARCH_CSN_MODULE_RATIO_PRE_MASK_MACRO          (0x1bU)   /**< @brief #CSN_MODULE_RATIO_PRE_MASK          */
#define BRCM_SWARCH_CSN_MODULE_RATIO_VAL_MASK_MACRO          (0x1cU)   /**< @brief #CSN_MODULE_RATIO_VAL_MASK          */
#define BRCM_SWARCH_CSN_METADATA_TYPE                        (0x1dU)   /**< @brief #CSN_MODULE_MetadataType            */
#define BRCM_SWARCH_CSN_STATS_MACRO                          (0x1eU)   /**< @brief #CSN_MODULE_StatsType               */
#define BRCM_SWARCH_CSN_MODULE_OP_TYPE                       (0x1fU)   /**< @brief #CSN_MODULE_OpType                  */
#define BRCM_SWARCH_CSN_MODULE_SET_OP_TYPE                   (0x20U)   /**< @brief #CSN_MODULE_SetOpsType              */
#define BRCM_SWARCH_CSN_MODULE_EXP_PARAM_TYPE                (0x21U)   /**< @brief #CSN_MODULE_ExpParamType            */
#define BRCM_SWARCH_CSN_MODULE_AWB_PARAM_TYPE                (0x22U)   /**< @brief #CSN_MODULE_AWB_ParamType           */
#define BRCM_SWARCH_CSN_MODULE_OPS_PARAM_TYPE                (0x23U)   /**< @brief #CSN_MODULE_OpsParamType            */
#define BRCM_SWARCH_CSN_MODULE_PARSE_BYTE_CFG_TYPE           (0x24U)   /**< @brief #CSN_MODULE_ParseByteCfgType        */
#define BRCM_SWARCH_CSN_MODULE_PARSE_FN_CFG_TYPE             (0x25U)   /**< @brief #CSN_MODULE_ParseFnCfgType          */
#define BRCM_SWARCH_CSN_MODULE_GAIN_TABLE_TYPE               (0x26U)   /**< @brief #CSN_MODULE_GainTableType           */
#define BRCM_SWARCH_CSN_MODULE_PARSE_EXP_CFG_TYPE            (0x27U)   /**< @brief #CSN_MODULE_ParseExpCfgType         */
#define BRCM_SWARCH_CSN_MODULE_PARSE_WB_GAIN_CFG_TYPE        (0x28U)   /**< @brief #CSN_MODULE_ParseWBGainCfgType      */
#define BRCM_SWARCH_CSN_MODULE_PARSE_WB_OFFSET_CFG_TYPE      (0x29U)   /**< @brief #CSN_MODULE_ParseWBOffsetCfgType    */
#define BRCM_SWARCH_CSN_MODULE_PARSE_WB_CFG_TYPE             (0x2aU)   /**< @brief #CSN_MODULE_ParseWBCfgType          */
#define BRCM_SWARCH_CSN_MODULE_PARSE_GAIN_CFG_TYPE           (0x2bU)   /**< @brief #CSN_MODULE_ParseGainCfgType        */
#define BRCM_SWARCH_CSN_MODULE_PARSE_CFG_TYPE                (0x2cU)   /**< @brief #CSN_MODULE_ParseCfgType            */
#define BRCM_SWARCH_CSN_MODULE_GROUP_HOLD_CFG_TYPE           (0x2dU)   /**< @brief #CSN_MODULE_GroupHoldCfgType        */
#define BRCM_SWARCH_CSN_MODULE_WB_GAIN_CFG_TYPE              (0x2eU)   /**< @brief #CSN_MODULE_WBGainRegCfgType        */
#define BRCM_SWARCH_CSN_MODULE_WB_OFFSET_CFG_TYPE            (0x2fU)   /**< @brief #CSN_MODULE_WBOffsetRegCfgType      */
#define BRCM_SWARCH_CSN_MODULE_WB_REG_CFG_TYPE               (0x30U)   /**< @brief #CSN_MODULE_WBRegCfgType            */
#define BRCM_SWARCH_CSN_MODULE_EXP_REG_CFG_TYPE              (0x31U)   /**< @brief #CSN_MODULE_ExpRegCfgType           */
#define BRCM_SWARCH_CSN_MODULE_GAIN_REG_CFG_TYPE             (0x32U)   /**< @brief #CSN_MODULE_GainRegCfgType          */
#define BRCM_SWARCH_CSN_MODULE_GAIN_PARAM_CFG_TYPE           (0x33U)   /**< @brief #CSN_MODULE_GainParamCfgType        */
#define BRCM_SWARCH_CSN_MODULE_GET_OPS_CFG_TYPE              (0x34U)   /**< @brief #CSN_MODULE_GetOpsCfgType           */
#define BRCM_SWARCH_CSN_MODULE_TUNER_CFG_TYPE                (0x35U)   /**< @brief #CSN_MODULE_TunerCfgType            */
#define BRCM_SWARCH_CSN_MODULE_CONFIG_TYPE                   (0x36U)   /**< @brief #CSN_MODULE_ConfigType              */
#define BRCM_SWARCH_CSN_MODULE_PARSE_METADATA_TYPE           (0x37U)   /**< @brief #CSN_MODULE_ParseMetadataType       */
#define BRCM_SWARCH_CSN_MODULE_GET_OPS_TYPE                  (0x38U)   /**< @brief #CSN_MODULE_GetOpsType              */
#define BRCM_SWARCH_CSN_MODULE_GET_CLOSEST_GAIN_TYPE         (0x39U)   /**< @brief #CSN_MODULE_GetClosestGainType      */
#define BRCM_SWARCH_CSN_MODULE_PARSE_STATISTICS_TYPE         (0x3aU)   /**< @brief #CSN_MODULE_ParseStatisticsType     */
#define BRCM_SWARCH_CSN_MODULE_CONFIGURE_TYPE                (0x3bU)   /**< @brief #CSN_MODULE_ConfigureType           */
#define BRCM_SWARCH_CSN_MODULE_TYPE                          (0x3cU)   /**< @brief #CSN_MODULE_Type                    */
#define BRCM_SWARCH_CSN_MODULE_GET_MODULE_PROC               (0x3dU)   /**< @brief #CSN_MODULE_GetModule               */
#define BRCM_SWARCH_CSN_MODULE_GET_COMMON_MODULE_PROC        (0x3eU)   /**< @brief #CSN_MODULE_GetCommonModule         */
#define BRCM_SWARCH_CSN_MODULE_ED_LAYOUT_TYPE                (0x3fU)   /**< @brief #CSN_MODULE_EDLayoutType            */
#define BRCM_SWARCH_CSN_MODULE_CRC_ORDER_TYPE                (0x40U)   /**< @brief #CSN_MODULE_CRCOrderType            */
#define BRCM_SWARCH_CSN_MODULE_CRC_LAYOUT_TYPE               (0x41U)   /**< @brief #CSN_MODULE_CRCLayoutType           */
#define BRCM_SWARCH_CSN_MODULE_ED_CRC_LAYOUT_TYPE            (0x42U)   /**< @brief #CSN_MODULE_EDCRCLayoutType         */
#define BRCM_SWARCH_CSN_MODULE_MAX_NUM_ED_CRCS_MACRO         (0x43U)   /**< @brief #CSN_MODULE_MAX_NUM_ED_CRCS         */
#define BRCM_SWARCH_CSN_MODULE_ED_FORMAT_TYPE                (0x44U)   /**< @brief #CSN_MODULE_EDFormatType            */
#define BRCM_SWARCH_CSN_MODULE_ED_CRC_FORAMT_TYPE            (0x45U)   /**< @brief #CSN_MODULE_EDCRCFormatType         */
#define BRCM_SWARCH_CSN_MODULE_PARSE_ED_CRC_TYPE             (0x46U)   /**< @brief #CSN_MODULE_ParseEDCRCType          */
/** @} */

/**
    @brief Maximum number of register operations

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_SET_OPS (50UL)

/**
    @brief Maximum number of histogram bins

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_NUM_EMB_HIST_BINS      (256U)

/**
    @name CSN operation type Mask
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_OP_NONE           (0x0UL)
#define CSN_MODULE_OP_EXPOSURE_LONG  (0x1UL)
#define CSN_MODULE_OP_EXPOSURE_SHORT (0x2UL)
#define CSN_MODULE_OP_GAIN_LONG      (0x4UL)
#define CSN_MODULE_OP_GAIN_SHORT     (0x8UL)
#define CSN_MODULE_OP_WB             (0x10UL)
/** @} */

/**
    @name CSN stats parsed mask
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_STATS_PARSED_NONE  (0x0UL)
#define CSN_MODULE_STATS_PARSED_LONG  (0x1UL)
#define CSN_MODULE_STATS_PARSED_SHORT (0x2UL)
/** @} */

/** @name CSN_MODULE_ExpIDX_Type
    @{
    @brief Camera sensor exposure index

    @trace #BRCM_SWREQ_CSN_MODULE
*/
typedef uint32_t CSN_MODULE_ExpIDX_Type;
#define CSN_MODULE_EXP_IDX_LONG  (0UL)  /**< @brief Long exposure */
#define CSN_MODULE_EXP_IDX_SHORT (1UL)  /**< @brief Short exposre */
#define CSN_MODULE_EXP_IDX_MAX   (2UL)  /**< @brief Maximum number of exposre */
/** @} */

/**
    @name CSN exposure time indexes
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_EXP_TIME_IDX_T1         (0U)
#define CSN_MODULE_EXP_TIME_IDX_T2         (1U)
#define CSN_MODULE_EXP_TIME_IDX_T3         (2U)
#define CSN_MODULE_EXP_TIME_IDX_MAX        (3U)
/** @} */

/**
    @brief Maximum number of bytes for frame number

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_BYTES_FN        (4U)

/**
    @brief Maximum number of bytes for  exposures

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_BYTES_EXP       (2U)

 /**
    @brief Maximum number of bytes for gain

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_BYTES_GAIN      (4U)

  /**
    @brief Maximum number of bytes for WB offset

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_BYTES_WB_OFFSET (4U)

/**
    @brief Maximum number of gain tables

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_GAIN_TABLES     (2U)

/**
    @brief Maximum size of gain table

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_GAIN_TABLE_SIZE     (8U)

/**
    @brief Maximum number of register to be programmed per exposure

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_REGS_EXP        (2U)

/**
    @brief Maximum number of register to be programmed per WB gain

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_REGS_WB_GAIN    (2U)

/**
    @brief Maximum number of register to be programmed per WB offset

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_REGS_WB_OFFSET  (3U)

/**
    @brief Maximum number of register to be programmed per gain

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_REGS_GAIN       (3U)

/**
    @name CSN WG gain indexes
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_WG_GAIN_IDX_R           (0U)
#define CSN_MODULE_WG_GAIN_IDX_GR          (1U)
#define CSN_MODULE_WG_GAIN_IDX_GB          (2U)
#define CSN_MODULE_WG_GAIN_IDX_B           (3U)
#define CSN_MODULE_WG_GAIN_IDX_MAX         (4U)
/** @} */

/**
    @brief Maximum number of gain components (ag, cg, dg)

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_GAIN_COMPS          (3U)

/**
    @name CSN Group hold indexes
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_GROUP_HOLD_IDX_START    (0U)
#define CSN_MODULE_GROUP_HOLD_IDX_END      (1U)
#define CSN_MODULE_GROUP_HOLD_IDX_MAX      (2U)
/** @} */

/**
    @brief Maximum number of registers per group

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_GR_HLD_REGS     (4UL)

/**
    @brief Maximum number of gain stages

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_GAIN_STAGES     (2U)

/**
    @brief Invalid short

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_INVALID_USHORT          (0xffffU)
/**
    @brief Unity value for 8p8 type

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_UP8_UNITY               (256U)
/**
    @brief Unity value for 4p12 type

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_UP12_UNITY              (4096U)

/**
    @name Shift type
    @{

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_CFG_SHIFT_NONE (0x00U)
#define CSN_MODULE_CFG_SHIFT_L_1  (0x01U)
#define CSN_MODULE_CFG_SHIFT_L_2  (0x02U)
#define CSN_MODULE_CFG_SHIFT_L_3  (0x03U)
#define CSN_MODULE_CFG_SHIFT_L_4  (0x04U)
#define CSN_MODULE_CFG_SHIFT_L_5  (0x05U)
#define CSN_MODULE_CFG_SHIFT_L_6  (0x06U)
#define CSN_MODULE_CFG_SHIFT_L_7  (0x07U)
#define CSN_MODULE_CFG_SHIFT_L_8  (0x08U)
#define CSN_MODULE_CFG_SHIFT_R_1  (0x81U)
#define CSN_MODULE_CFG_SHIFT_R_2  (0x82U)
#define CSN_MODULE_CFG_SHIFT_R_3  (0x83U)
#define CSN_MODULE_CFG_SHIFT_R_4  (0x84U)
#define CSN_MODULE_CFG_SHIFT_R_5  (0x85U)
#define CSN_MODULE_CFG_SHIFT_R_6  (0x86U)
#define CSN_MODULE_CFG_SHIFT_R_7  (0x87U)
#define CSN_MODULE_CFG_SHIFT_R_8  (0x88U)
/** @} */

/**
    @brief Direction mask for shift type

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_SHIFT_DIR_MASK   (0x80U)
/**
    @brief Mask for shift count

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_SHIFT_COUNT_MASK (0x7fU)

/**
    @brief Pre/Post mask for ratio

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_RATIO_PRE_MASK (0x8000U)
/**
    @brief Value mask for ratio

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_RATIO_VAL_MASK (0x7fffU)

/**
    @brief CSN Metadata structure

    @trace #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_MetadataType {
    uint32_t frameNum;                      /**< @brief Frame number                        */
    uint32_t shutter[CSN_MODULE_EXP_IDX_MAX];      /**< @brief Shutter in microseconds      */
    uint16_t analogGain[CSN_MODULE_EXP_IDX_MAX];   /**< @brief Analog gain in 8.8 formats   */
    uint16_t gainR;                         /**< @brief Gain for R channel                  */
    uint16_t gainB;                         /**< @brief Gain for B channel                  */
    uint32_t wbEnabled;                     /**< @brief Status of WB block enable in sensor.
                                                 This indicates the values gainR & gainB is
                                                 applied in the sensor or not               */
} CSN_MODULE_MetadataType;

/**
    @brief CSN Sensor Statistics structure

    @trace #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_StatsType {
    uint32_t frameNum;                              /**< @brief Frame number         */
    uint32_t histAbcissae[CSN_MODULE_NUM_EMB_HIST_BINS];   /**< @brief Histogram Abscissae  */
    uint32_t histOrdinates[CSN_MODULE_NUM_EMB_HIST_BINS];  /**< @brief Histogram Ordinates  */
    uint32_t numValidBins;                          /**< @brief Number of valid bins */
    uint32_t blBin;                                 /**< @brief Black level bin      */
} CSN_MODULE_StatsType;

/**
    @brief CSN Module register operation structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_OpType {
    uint32_t reg;  /**< @brief Register address */
    uint32_t val;  /**< @brief Value */
} CSN_MODULE_OpType;

/**
    @brief CSN Module "set" operations structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_SetOpsType {
    CSN_MODULE_OpType op[CSN_MODULE_MAX_SET_OPS]; /**< @brief op as in #CSN_MODULE_OpType */
    uint32_t count;                               /**< @brief Number of operations */
} CSN_MODULE_SetOpsType;

/**
    @brief CSN Module exposure parameter structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ExpParamType {
    uint16_t expLines[CSN_MODULE_EXP_IDX_MAX]; /**< @brief Exposure lines        */
    uint16_t gain[CSN_MODULE_EXP_IDX_MAX];     /**< @brief Gain                  */
    uint16_t expRatio;                          /**< @brief Exposure ratio        */
    uint16_t minFlickerLines;                   /**< @brief Minimum flicker lines */
} CSN_MODULE_ExpParamType;

/**
    @brief CSN Module AWB parameter structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_AWB_ParamType {
    uint16_t gainR;         /**< @brief Gain for R channel (8.8 format) */
    uint16_t gainB;         /**< @brief Gain for B channel (8.8 format) */
} CSN_MODULE_AWB_ParamType;

/**
    @brief CSN Module operations param structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_OpsParamType {
    CSN_MODULE_ExpParamType *expParam;        /**< @brief Exposure parameters */
    CSN_MODULE_AWB_ParamType   *wbParam;      /**< @brief WB parameters       */
} CSN_MODULE_OpsParamType;

/**
    @brief CSN Parse byte configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseByteCfgType {
    uint8_t mask;
    uint8_t shift;
    uint16_t offset;
} CSN_MODULE_ParseByteCfgType;

/**
    @brief CSN Parse frame number configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseFnCfgType {
    uint8_t  bufIdx;
    uint8_t  numBytes;
    uint8_t  shift;
    CSN_MODULE_ParseByteCfgType bytes[CSN_MODULE_MAX_NUM_BYTES_FN];
} CSN_MODULE_ParseFnCfgType;

/**
    @brief Gain map table structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_GainTableType {
    uint16_t code[CSN_MODULE_MAX_GAIN_TABLE_SIZE];
    uint16_t val[CSN_MODULE_MAX_GAIN_TABLE_SIZE];
    uint16_t mask;
    uint16_t numVals;
} CSN_MODULE_GainTableType;

/**
    @brief CSN Parse exposure configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseExpCfgType {
    uint8_t  bufIdx;
    uint8_t  numBytes;
    uint8_t  shift;
    CSN_MODULE_ParseByteCfgType bytes[CSN_MODULE_MAX_NUM_BYTES_EXP];
} CSN_MODULE_ParseExpCfgType;

/**
    @brief CSN Parse WB gain configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseWBGainCfgType {
    uint8_t  bufIdx;
    uint8_t  numBytes;
    uint8_t  shift;
    CSN_MODULE_ParseByteCfgType bytes[CSN_MODULE_MAX_NUM_BYTES_GAIN];
} CSN_MODULE_ParseWBGainCfgType;

/**
    @brief CSN Parse WB offset configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseWBOffsetCfgType {
    uint8_t  bufIdx;
    uint8_t  numBytes;
    uint8_t  shift;
    CSN_MODULE_ParseByteCfgType bytes[CSN_MODULE_MAX_NUM_BYTES_WB_OFFSET];
} CSN_MODULE_ParseWBOffsetCfgType;

/**
    @brief CSN Parse WB configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseWBCfgType {
    uint8_t enByteBufIdx;
    CSN_MODULE_ParseByteCfgType enByte;
    CSN_MODULE_ParseWBGainCfgType parseWBGainCfg[CSN_MODULE_WG_GAIN_IDX_MAX];
    CSN_MODULE_ParseWBOffsetCfgType parseWBOffsetCfg;
} CSN_MODULE_ParseWBCfgType;

/**
    @brief CSN Parse gain configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseGainCfgType {
    uint8_t  bufIdx;
    uint8_t  numBytes;
    uint8_t  shift;
    uint8_t  useTable; // "0" if no table/ "index+1" if table used
    CSN_MODULE_ParseByteCfgType bytes[CSN_MODULE_MAX_NUM_BYTES_GAIN];
} CSN_MODULE_ParseGainCfgType;

/**
    @brief Embedded Data Layout configurations

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
typedef struct _CSN_MODULE_EDLayoutType {
    uint16_t startOffset; /**< @brief Offset of first "data" in ED buffer    */
    uint16_t jumpOffset;  /**< @brief Offset between "data" in ED buffer     */
    uint16_t numBytes;    /**< @brief Number of "data" elements in ED buffer */
    uint16_t reserved;    /**< @brief Reserved for future use                */
} CSN_MODULE_EDLayoutType;

/** @name CSN_MODULE_CRCOrderType
    @{
    @brief Endianness of CRC bytes

    @trace #BRCM_SWREQ_CSN_MODULE
*/
typedef uint8_t CSN_MODULE_CRCOrderType;
#define CSN_MODULE_CRC_ORDER_LE (0U)  /**< @brief Little Endian */
#define CSN_MODULE_CRC_ORDER_BE (1U)  /**< @brief Big Endian    */
/** @} */

/**
    @brief CRC layout configurations

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
typedef struct _CSN_MODULE_CRCLayoutType {
    CSN_MODULE_CRCOrderType order; /**< @brief Byte order as in #CSN_MODULE_CRCOrderType  */
    uint16_t startOffset;          /**< @brief Offset of first byte of CRC                */
    uint16_t jumpOffset;           /**< @brief Offset beteen CRC bytes                    */
    uint16_t numBytes;             /**< @brief Number of CRC bytes                        */
    uint32_t poly;                 /**< @brief Polynomial for CRC                         */
    uint32_t crcInitValue;         /**< @brief Initial Value for CRC                      */
} CSN_MODULE_CRCLayoutType;

/**
    @brief Embedded data and CRC layout configurations

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
typedef struct _CSN_MODULE_EDCRCLayoutType {
    CSN_MODULE_ExpIDX_Type channelIdx;  /**< @brief Exposure channel index       */
    CSN_MODULE_EDLayoutType edLayout;   /**< @brief Embedded data layout configs */
    CSN_MODULE_CRCLayoutType crcLayout; /**< @brief CRC layout configs for ED    */
} CSN_MODULE_EDCRCLayoutType;

/**
    @brief Maximum number of CRCs for Embedded data

    @trace   #BRCM_SWREQ_CSN_MODULE
*/
#define CSN_MODULE_MAX_NUM_ED_CRCS (0x4U)

/** @name CSN_MODULE_EDFormatType
    @{
    @brief Embedded data format

    @trace #BRCM_SWREQ_CSN_MODULE
*/
typedef uint8_t CSN_MODULE_EDFormatType;
#define CSN_MODULE_ED_FORMAT_TAGGED (0U) /**< @brief Format where bytes seprated
                                                               with fixed offsets */
#define CSN_MODULE_ED_FORMAT_MAX    (1U) /**< @brief Invalid format               */
/** @} */

/** @name CSN_MODULE_EDCRCFormatType
    @{
    @brief CRC type for Embedded Data

    @trace #BRCM_SWREQ_CSN_MODULE
*/
typedef uint8_t CSN_MODULE_EDCRCFormatType;
#define CSN_MODULE_ED_CRC_FORMAT_32C (0U)  /**< @brief 32C Type         */
#define CSN_MODULE_ED_CRC_FORMAT_MAX (1U)  /**< @brief Invalid CRC type */
/** @} */

/**
    @brief CSN Parse Embedded CRC configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseEDCRCType {
    CSN_MODULE_EDFormatType edFormat;   /**< @brief ED format as in #CSN_MODULE_EDFormatType   */
    CSN_MODULE_EDCRCFormatType crcType; /**< @brief CRC type as in #CSN_MODULE_EDCRCFormatType */
    uint8_t numCRCs;                    /**< @brief Number of CRCs in Embedded data buffer     */
    CSN_MODULE_EDCRCLayoutType edCrcLayout[CSN_MODULE_MAX_NUM_ED_CRCS];
} CSN_MODULE_ParseEDCRCType;

/**
    @brief CSN Parse metadata configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ParseCfgType {
    CSN_MODULE_ParseFnCfgType parseFnCfg;
    CSN_MODULE_ParseExpCfgType parseExpCfg;
    CSN_MODULE_ParseWBCfgType parseWBCfg;
    CSN_MODULE_ParseGainCfgType parseGainCfg[CSN_MODULE_MAX_GAIN_COMPS];
    CSN_MODULE_GainTableType gainTable[CSN_MODULE_MAX_NUM_GAIN_TABLES];
    CSN_MODULE_ParseEDCRCType parseEECRCCfg;
} CSN_MODULE_ParseCfgType;

/**
    @brief CSN Group Hold register configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_GroupHoldCfgType {
    uint16_t addr[CSN_MODULE_MAX_NUM_GR_HLD_REGS];
    uint16_t val[CSN_MODULE_MAX_NUM_GR_HLD_REGS];
} CSN_MODULE_GroupHoldCfgType;

/**
    @brief CSN WB gain register configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_WBGainRegCfgType {
    uint16_t addr[CSN_MODULE_MAX_NUM_REGS_WB_GAIN];
    uint16_t min;
    uint16_t max;
} CSN_MODULE_WBGainRegCfgType;

/**
    @brief CSN WB offset register configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_WBOffsetRegCfgType {
    uint16_t addr[CSN_MODULE_MAX_NUM_REGS_WB_OFFSET];
    uint16_t sub;
    uint16_t mul;
} CSN_MODULE_WBOffsetRegCfgType;

/**
    @brief CSN WB configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_WBRegCfgType {
    CSN_MODULE_WBGainRegCfgType wbGainCfg[CSN_MODULE_WG_GAIN_IDX_MAX];
    CSN_MODULE_WBOffsetRegCfgType wbOffsetCfg[CSN_MODULE_WG_GAIN_IDX_MAX];
} CSN_MODULE_WBRegCfgType;

/**
    @brief CSN exposure register configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ExpRegCfgType {
    uint8_t linesMin;
    uint16_t linesMax;
    uint16_t addr[CSN_MODULE_MAX_NUM_REGS_EXP];
} CSN_MODULE_ExpRegCfgType;

/**
    @brief CSN Gain register configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_GainRegCfgType {
    uint16_t addr[CSN_MODULE_MAX_NUM_REGS_GAIN];
} CSN_MODULE_GainRegCfgType;

/**
    @brief CSN Gain parameter configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_GainParamCfgType {
    uint16_t min[CSN_MODULE_MAX_NUM_GAIN_STAGES];
    uint16_t max[CSN_MODULE_MAX_NUM_GAIN_STAGES];
    uint16_t ratio; /* 3p12 format */
} CSN_MODULE_GainParamCfgType;

/**
    @brief CSN GetOps configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_GetOpsCfgType {
    CSN_MODULE_GroupHoldCfgType ghCfg[CSN_MODULE_GROUP_HOLD_IDX_MAX];
    CSN_MODULE_WBRegCfgType wbCfg[CSN_MODULE_EXP_TIME_IDX_MAX];
    CSN_MODULE_ExpRegCfgType expCfg[CSN_MODULE_EXP_TIME_IDX_MAX];
    CSN_MODULE_GainRegCfgType gainRegCfg[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_MAX_GAIN_COMPS];
    CSN_MODULE_GainParamCfgType gainParamCfg[CSN_MODULE_EXP_TIME_IDX_MAX][CSN_MODULE_MAX_GAIN_COMPS];
} CSN_MODULE_GetOpsCfgType;

/**
    @brief CSN Tuner configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_TunerCfgType {
    CSN_MODULE_ParseCfgType parseCfg;
    CSN_MODULE_GetOpsCfgType getOpsCfg;
} CSN_MODULE_TunerCfgType;

/**
    @brief CSN Module configuration structure

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_ConfigType {
    uint8_t bpp;                         /**< @brief Bits per pixel      */
    uint8_t isHDR;                       /**< @brief Is HDR              */
    uint16_t imgWidth;                   /**< @brief image width         */
} CSN_MODULE_ConfigType;

/** @brief Parse Camera Sensor metadata

    This API parses the metadata received from camera sensor and return
    the filled #CSN_MODULE_MetadataType structure.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aBuffer      Metadata buffer for all exposures
    @param[in]      aSize        Metadata buffer sizes for all exposures
    @param[out]     aMetadata    #CSN_MODULE_MetadataType structure to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Metadata parsing is successful
    @retval     #BCM_ERR_UNKNOWN        Metadata parsing failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Metadata parsing implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)
    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
typedef int32_t (*CSN_MODULE_ParseMetadataType) (const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                                                 uint32_t (aSize) [CSN_MODULE_EXP_IDX_MAX],
                                                 CSN_MODULE_MetadataType * const aMetadata);

/** @brief Get operations for an operation
    This API fills operations structure for an sensor set operation.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aOpsType  Operation type
    @param[in]      opsParam  Operation params
    @param[out]     aOps      Ops structure to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             GetOps is successful
    @retval     #BCM_ERR_UNKNOWN        GetOps failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      GetOps implementation is not available
                                        in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)
    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
typedef int32_t (*CSN_MODULE_GetOpsType) (uint32_t aOpsType,
                                          CSN_MODULE_OpsParamType * const opsParam,
                                          CSN_MODULE_SetOpsType * const aOps);

 /** @brief Get closest gain for an input gain

    This API returns closest possible gain for a sensor module for an input gain.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aOpsType  Operation type
    @param[inout]   aGain     Gain passed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             GetClosestGain is successful
    @retval     #BCM_ERR_UNKNOWN        GetClosestGain failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      GetClosestGain implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aGain is NULL)

    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
typedef int32_t (*CSN_MODULE_GetClosestGainType) (uint32_t aOpsType,
                                                  uint16_t * const aGain);

/** @brief Parse Camera Sensor statistics

    This API parses the statistics data received from camera sensor and return
    the filled #CSN_MODULE_StatsType structure.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aBuffer      Statistics buffers for all exposures
    @param[in]      aSize        Statistics buffer sizes for all exposures
    @param[out]     aStats       #CSN_MODULE_StatsType structures to be filled
    @param[out]     aParsedMask  Status of parsed statistics parsed

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Statistics parsing is successful
    @retval     #BCM_ERR_UNKNOWN        Statistics parsing failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Statistics parsing implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aBuffer is NULL) or
                                        (aStats is NULL)

    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
typedef int32_t (*CSN_MODULE_ParseStatisticsType) (const uint8_t * const (aBuffer) [CSN_MODULE_EXP_IDX_MAX],
                                      uint32_t (aSize)[CSN_MODULE_EXP_IDX_MAX],
                                      CSN_MODULE_StatsType * const (aSensorStats)[CSN_MODULE_EXP_IDX_MAX],
                                      uint32_t *aParsedMask);

/** @brief Configure CSN Module

    This interface configures CSN Module with module specific configuration.

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aConfig       Configuration to be applied

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Module configuration is successful
    @retval     #BCM_ERR_UNKNOWN        Module configuration failed from CSN Module
    @retval     #BCM_ERR_NOSUPPORT      Module configuration implementation is not
                                        available in CSN Module
    @retval     #BCM_ERR_INVAL_PARAMS   (aConfig is NULL)

    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
typedef int32_t (*CSN_MODULE_ConfigureType) (CSN_MODULE_ConfigType *aConfig);

/**
    @brief CSN Module structure

    Module structure containing function pointers

    @trace   #BRCM_SWREQ_CSN_MODULE
 */
typedef struct _CSN_MODULE_Type {
    CSN_MODULE_ParseMetadataType parseMetadata;      /**< @brief parseMetadata as #CSN_MODULE_ParseMetadataType     */
    CSN_MODULE_GetOpsType getOps;                    /**< @brief getOps as #CSN_MODULE_GetOpsType                   */
    CSN_MODULE_GetClosestGainType getClosestGain;    /**< @brief getClosestGain as #CSN_MODULE_GetClosestGainType   */
    CSN_MODULE_ParseStatisticsType parseStatistics;  /**< @brief parseStatistics as #CSN_MODULE_ParseStatisticsType */
    CSN_MODULE_ConfigureType sensorConfigure;        /**< @brief sensorConfigure as #CSN_MODULE_ConfigureType       */
} CSN_MODULE_Type;

/** @brief Get CSN Module

    This interface Gets the CSN module implemented in the platform

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval     CSN_MODULE_Type* Pointer to CSN Module

    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
const CSN_MODULE_Type * CSN_MODULE_GetModule(void);

/** @brief Get CSN Module

    This interface Gets the common CSN module implemented in the platform

    @behavior Sync, Non-reentrant

    @pre None

    @param void

    Return values are documented in reverse-chronological order
    @retval     CSN_MODULE_Type* Pointer to CSN Module

    @post None

    @trace   #BRCM_SWREQ_CSN_MODULE

    @limitations None
*/
const CSN_MODULE_Type * CSN_MODULE_GetCommonModule(void);

#endif /* CAMERA_SENSOR_MODULE_H */
/** @} */
