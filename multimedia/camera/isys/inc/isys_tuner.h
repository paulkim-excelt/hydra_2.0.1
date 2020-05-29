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
    @addtogroup grp_isys
    @{
    @file isys_tuner.h
    @brief Data structures and API's of the ISYS tuner support
    This header file defines ISYS tuner support interfaces
    @version 1.82 Fixed TBDs
    @version 1.81 Imported from AUTOSW_ISYS_Interface_Spec_1.81.docx
*/

#ifndef ISYS_TUNER_H
#define ISYS_TUNER_H

#include <stdint.h>
#include <bcm_err.h>
#include <isys.h>

/**
    @name ISP API IDs
    @{
    @brief API IDs for ISP
*/
#define BRCM_SWARCH_ISYS_NUM_HISTOGRAM_BINS_MACRO                 (0x00U) /**< @brief #ISYS_NUM_HISTOGRAM_BINS              */
#define BRCM_SWARCH_ISYS_DEFAULT_NUM_HISTOGRAM_BINS_LOG2_MACRO    (0x01U) /**< @brief #ISYS_DEFAULT_NUM_HISTOGRAM_BINS_LOG2 */
#define BRCM_SWARCH_ISYS_NUM_REGIONS_GROUP_MACRO                  (0x02U) /**< @brief #ISYS_NUM_REGIONS_GROUP_0             */
#define BRCM_SWARCH_ISYS_NUM_REGIONS_MACRO                        (0x03U) /**< @brief #ISYS_NUM_REGIONS                     */
#define BRCM_SWARCH_ISYS_NUM_HISTOGRAMS_MACRO                     (0x04U) /**< @brief #ISYS_NUM_HISTOGRAMS                  */
#define BRCM_SWARCH_ISYS_REGION_STATS_TYPE                        (0x05U) /**< @brief #ISYS_RegionStatsType                 */
#define BRCM_SWARCH_ISYS_HIST_STATS_TYPE                          (0x06U) /**< @brief #ISYS_HistStatsType                   */
#define BRCM_SWARCH_ISYS_STATS_TYPE                               (0x07U) /**< @brief #ISYS_StatisticsType                  */
#define BRCM_SWARCH_ISYS_WB_PARAM_TYPE                            (0x08U) /**< @brief #ISYS_WB_ParamType                    */
#define BRCM_SWARCH_ISYS_GAIN_PARAM_TYPE                          (0x09U) /**< @brief #ISYS_GainParamType                   */
#define BRCM_SWARCH_ISYS_GET_METADATA_BUF_PROC                    (0x0AU) /**< @brief #ISYS_GetMetadataBuf                  */
#define BRCM_SWARCH_ISYS_GET_SENSOR_STATS_BUF_PROC                (0x0BU) /**< @brief #ISYS_GetSensorStatsBuf               */
#define BRCM_SWARCH_ISYS_GET_STATISTICS_PROC                      (0x0CU) /**< @brief #ISYS_GetStatistics                   */
#define BRCM_SWARCH_ISYS_GET_WB_PROC                              (0x0DU) /**< @brief #ISYS_GetWB                           */
#define BRCM_SWARCH_ISYS_SET_WB_PROC                              (0x0EU) /**< @brief #ISYS_SetWB                           */
#define BRCM_SWARCH_ISYS_GET_GAIN_PROC                            (0x0FU) /**< @brief #ISYS_GetGain                         */
#define BRCM_SWARCH_ISYS_SET_GAIN_PROC                            (0x10U) /**< @brief #ISYS_SetGain                         */
#define BRCM_SWARCH_ISYS_GET_CT_PROC                              (0x11U) /**< @brief #ISYS_GetCT                           */
#define BRCM_SWARCH_ISYS_SET_CT_PROC                              (0x12U) /**< @brief #ISYS_SetCT                           */
/** @} */

/**
    @brief Number of histogram bins

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
#define ISYS_NUM_HISTOGRAM_BINS (128UL)

/**
    @brief Log2 of Number of histograms

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
#define ISYS_DEFAULT_NUM_HISTOGRAM_BINS_LOG2 (7UL)

/**
    @name Group regions
    @{
    @brief Number of group regions

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
#define ISYS_NUM_REGIONS_GROUP_0 (128UL) /**< @brief Number of group0 regions */
#define ISYS_NUM_REGIONS_GROUP_1 (16UL)  /**< @brief Number of group1 regions */
#define ISYS_NUM_REGIONS_GROUP_2 (16UL)  /**< @brief Number of group2 regions */
/** @} */

/**
    @brief Total number of ISYS regions

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
#define ISYS_NUM_REGIONS (ISYS_NUM_REGIONS_GROUP_0 + ISYS_NUM_REGIONS_GROUP_1 + ISYS_NUM_REGIONS_GROUP_2)

/**
    @brief Number of histograms

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
#define ISYS_NUM_HISTOGRAMS     (2UL)

/**
    @brief ISYS Region statistics information

    ISYS Region statistics information structure containing region information of incoming image

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
typedef struct _ISYS_RegionStatsType {
    uint32_t counted;   /**< @brief Number of 2x2 Bayer tiles accumulated */
    uint64_t rAcc;      /**< @brief Accumulated value of R */
    uint64_t gAcc;      /**< @brief Accumulated value of G */
    uint64_t bAcc;      /**< @brief Accumulated value of B */
} ISYS_RegionStatsType;

/**
    @brief ISYS histogram statistics

    ISYS histogram statistics information structure containing 128-bin
    histogram for R, B and G channel.

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
typedef struct _ISYS_HistStatsType {
   uint32_t rHist[ISYS_NUM_HISTOGRAM_BINS];     /**< @brief R - Histogram */
   uint32_t gHist[ISYS_NUM_HISTOGRAM_BINS];     /**< @brief G - Histogram */
   uint32_t bHist[ISYS_NUM_HISTOGRAM_BINS];     /**< @brief B - Histogram */
} ISYS_HistStatsType;

/**
    @brief ISYS statistics

    ISYS statistics structure containing information about 3 programmable
    group’s region statistics as well as histogram statistics.

    @trace #BRCM_SWREQ_ISYS_AUXILIARY_DATA
*/
typedef struct _ISYS_StatisticsType {
    uint8_t isEnabled;
    uint32_t frameNum;
    ISYS_RegionStatsType group0RegionStats[ISYS_NUM_REGIONS_GROUP_0]; /**< @brief Group 0 region statistics */
    ISYS_RegionStatsType group1RegionStats[ISYS_NUM_REGIONS_GROUP_1]; /**< @brief Group 1 region statistics */
    ISYS_RegionStatsType group2RegionStats[ISYS_NUM_REGIONS_GROUP_2]; /**< @brief Group 2 region statistics */
    ISYS_HistStatsType histStats[ISYS_NUM_HISTOGRAMS];               /**< @brief Histogram statistics */
} ISYS_StatisticsType;

/**
    @brief ISYS WB parameter

    ISYS WB parameter structure containing information about Red Gain and Blue
    gain applied.

    @trace #BRCM_SWREQ_ISYS_TUNER_UPDATE
*/
typedef struct _ISYS_WB_ParamType {
    uint8_t  isEnabled;     /**< @brief Status of WB block enable in ISP */
    uint16_t gainR;         /**< @brief Gain for R channel (8.8 format) */
    uint16_t gainB;         /**< @brief Gain for B channel (8.8 format) */
} ISYS_WB_ParamType;

/**
    @brief ISYS WB parameter

    ISYS WB parameter structure containing information about Red Gain and Blue
    gain applied.

    @trace #BRCM_SWREQ_ISYS_TUNER_UPDATE
*/
typedef struct _ISYS_GainParamType {
    uint8_t  isEnabled;     /**< @brief Status of Gain block enable in ISP */
    uint16_t gain;          /**< @brief Global gain (8.8 format) */
    uint16_t threshold;     /**< @brief Threshold parameter */
    uint32_t offsetR;       /**< @brief Offset for R channel */
    uint32_t offsetG;       /**< @brief Offset for G channel */
    uint32_t offsetB;       /**< @brief Offset for B channel */
} ISYS_GainParamType;

/** @brief Retrieve meta-data buffer

    This API lets the client retrieve metadata buffer for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aExpIdx     Exposure index for which the metadata to be
                                retrieved.
    @param[out]     aP_MetaBuf  Pointer to metadata to be filled
    @param[in]      aOffset     Offset of valid data from the start of buffer
    @param[in]      aSize     Size of buffer to be copied

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Metadata retrieval has been successful
    @retval     #BCM_ERR_EAGAIN         If the data was overwritten
                                        and client need to try again
    @retval     #BCM_ERR_NOT_FOUND      If there is no metadata found in ISYS
    @retval     #BCM_ERR_NOMEM          If buffer size aP_Size is not big enough
    @retval     #BCM_ERR_INVAL_PARAMS   If passed parameters are not valid
    @retval     #BCM_ERR_INVAL_STATE    If ISYS state is not #ISYS_STATE_STREAMING

    @post None

    @trace  #BRCM_SWREQ_ISYS_AUXILIARY_DATA

    @limitations None
*/
int32_t ISYS_GetMetadataBuf(ISYS_SensorExpIDXType aExpIdx,
                            uint8_t  *aP_MetaBuf,
                            uint32_t aOffset,
                            uint32_t aSize);

/** @brief Retrieve sensor statistics buffer

    This API lets the client retrieve sensor statistics buffer for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aExpIdx     Exposure index for which the stats to be
                                retrieved.
    @param[out]     aP_StatsBuf Pointer to stats to be filled
    @param[in]      aOffset     Offset of valid data from the start of buffer
    @param[in]      aP_Size     Size of buffer to be copied
    @param[out]     aFrameNum   Pointer to frame number

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Sensor statistics retrieval has been successful
    @retval     #BCM_ERR_EAGAIN         If the data was overwritten
                                        and client need to try again
    @retval     #BCM_ERR_NOT_FOUND      If there is no sensor statistics found in ISYS
    @retval     #BCM_ERR_NOMEM          If buffer size #aP_Size is not big enough
    @retval     #BCM_ERR_INVAL_PARAMS   If passed parameters are not valid
    @retval     #BCM_ERR_INVAL_STATE    If ISYS state is not #ISYS_STATE_STREAMING

    @post None

    @trace  #BRCM_SWREQ_ISYS_AUXILIARY_DATA

    @limitations None
*/
int32_t ISYS_GetSensorStatsBuf(ISYS_SensorExpIDXType aExpIdx,
                               uint8_t  *aP_StatsBuf,
                               uint32_t aOffset,
                               uint32_t aSize,
                               uint32_t *aFrameNum);

/** @brief Retrieve statistics

    This API lets the client retrieve statistics for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[out]     aP_Stats    Pointer to statistics buffer to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Statistics retrieval has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for statistics retrieval
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_AUXILIARY_DATA

    @limitations None
*/
int32_t ISYS_GetStatistics(ISYS_StatisticsType *aP_Stats);

/** @brief Retrieve WB parameter

    This API lets the client retrieve WB parameter for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[out]     aP_WB       Pointer to WB parameters to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             WB parameter retrieval has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter retrieval
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_GetWB(ISYS_WB_ParamType *aP_WB);

/** @brief Program WB parameter

    This API lets the client program WB parameter for the next frame.

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aP_WB       Pointer to WB parameters to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             WB parameter programming has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter programming
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_SetWB(ISYS_WB_ParamType *aP_WB);

/** @brief Retrieve Gain parameters

    This API lets the client retrieve Gain parameters for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[out]     aP_Gain     Pointer to Gain parameters to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Gain parameter retrieval has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter retrieval
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_GetGain(ISYS_GainParamType *aP_Gain);

/** @brief Program Gain parameter

    This API lets the client program Gain parameter for the next frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aP_Gain     Pointer to Gain parameters to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             Gain parameter programming has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter programming
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_SetGain(ISYS_GainParamType *aP_Gain);

/** @brief Retrieve CT

    This API lets the client retrieve CT for the latest frame.

    @behavior Sync, Re-entrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[out]     aP_CT       Pointer to CT to be filled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CT retrieval has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter retrieval
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_GetCT(uint32_t *aP_CT);

/** @brief Program CT

    This API lets the client program CT for the next frame.

    @behavior Sync, Non-reentrant

    @pre ISYS shall be in #ISYS_STATE_STREAMING state while calling this API

    @param[in]      aP_CT       Pointer to CT to be set

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             CT programming has been successful
    @retval     #BCM_ERR_INVAL_STATE    Wrong state for parameter programming
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid parameter

    @post None

    @trace  #BRCM_SWREQ_ISYS_TUNER_UPDATE

    @limitations None
*/
int32_t ISYS_SetCT(uint32_t *aP_CT);

#endif /* ISYS_TUNER_H */
/** @} */
