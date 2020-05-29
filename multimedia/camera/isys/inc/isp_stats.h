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
    @addtogroup grp_isp_ifc
    @{

    @file isp_stats.h
    @brief ISP statistics types
    @version 0.1
*/

#ifndef ISP_STATS_H
#define ISP_STATS_H

/**
    @name ISP statistics interface IDs
    @{

    @brief Statistics types interface IDs for ISP
*/
#define BRCM_SWARCH_ISP_NUM_HISTOGRAMS_MACRO                  (0x0U)  /**< @brief #ISP_NUM_HISTOGRAMS                  */
#define BRCM_SWARCH_ISP_NUM_REGIONS_GROUP_MACRO               (0x1U)  /**< @brief #ISP_NUM_REGIONS_GROUP_0             */
#define BRCM_SWARCH_ISP_DEFAULT_NUM_HISTOGRAM_BINS_LOG2_MACRO (0x2U)  /**< @brief #ISP_DEFAULT_NUM_HISTOGRAM_BINS_LOG2 */
#define BRCM_SWARCH_ISP_DEFAULT_NUM_HISTOGRAM_BINS_MACRO      (0x3U)  /**< @brief #ISP_DEFAULT_NUM_HISTOGRAM_BINS      */
#define BRCM_SWARCH_ISP_NUM_HISTOGRAM_BINS_MACRO              (0x4U)  /**< @brief #ISP_NUM_HISTOGRAM_BINS              */
#define BRCM_SWARCH_ISP_REGION_STATS_TYPE                     (0x5U)  /**< @brief #ISP_RegionStatsType                 */
#define BRCM_SWARCH_ISP_STATS_OUTPUT2_CHANNEL_TYPE            (0x6U)  /**< @brief #ISP_StatsOutput2ChannelType         */
#define BRCM_SWARCH_ISP_STATS_OUTPUT2_TYPE                    (0x7U)  /**< @brief #ISP_StatsOutput2Type                */
#define BRCM_SWARCH_ISP_STATS_OUTPUT3_TYPE                    (0x8U)  /**< @brief #ISP_StatsOutput3Type                */
#define BRCM_SWARCH_ISP_STATS_OUTPUT_TYPE                     (0x9U)  /**< @brief #ISP_StatsOutputType                 */
/** @} */

/**
    @brief Number of histogram bins

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_NUM_HISTOGRAMS (2UL)

/**
    @name Macro for Number of regions in groups
    @{
    @brief Macro for Number of regions in groups

    @trace  #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_NUM_REGIONS_GROUP_0 (128UL)
#define ISP_NUM_REGIONS_GROUP_1 (16UL)
#define ISP_NUM_REGIONS_GROUP_2 (16UL)
/** @} */

/**
    @brief Number of histogram bins in log base 2

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_DEFAULT_NUM_HISTOGRAM_BINS_LOG2 (7UL)

/**
    @brief Default number of histogram bins

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_DEFAULT_NUM_HISTOGRAM_BINS (1UL << ISP_DEFAULT_NUM_HISTOGRAM_BINS_LOG2)

/**
    @brief Number of histogram bins

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
#define ISP_NUM_HISTOGRAM_BINS (ISP_DEFAULT_NUM_HISTOGRAM_BINS)

/**
    @brief Statistics output for Group 0 as in memory

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
 */
typedef struct _ISP_RegionStatsType {
   uint32_t counted;
   uint32_t rAccLo32;
   uint16_t rAccHi16;
   uint16_t gAccLo16;
   uint32_t gAccHi32;
   uint32_t bAccLo32;
   uint16_t bAccHi16;
   uint8_t pad0[10];
} ISP_RegionStatsType;

/**
    @brief Statistics output for Group 2 as in memory

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef struct _ISP_StatsOutput2ChannelType {
   uint32_t focusCount;
   uint32_t focusFOMlo32;
   uint16_t focusFOMhi16;
   uint16_t pad;
   uint16_t minimum;
   uint16_t maximum;
} ISP_StatsOutput2ChannelType;

/**
    @brief FOM Statistics output for Group 1 as in memory

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef struct _ISP_StatsOutput2Type {
   ISP_StatsOutput2ChannelType lo, hi;
} ISP_StatsOutput2Type;

/**
    @brief Histogram Statistics output for Group 2 as in memory

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef struct _ISP_StatsOutput3Type {
   uint32_t r[ISP_NUM_HISTOGRAMS];
   uint32_t g[ISP_NUM_HISTOGRAMS];
   uint32_t b[ISP_NUM_HISTOGRAMS];
   uint16_t pad;
   uint16_t rowSumLo16;
   uint32_t rowSumHi32;  /* Actually this is a 40 bit value, not a 48 */
} ISP_StatsOutput3Type;

/**
    @brief All in one statistics

    @trace #BRCM_SWREQ_ISP_KERNEL_INTERFACE
*/
typedef struct _ISP_StatsOutputType {
   ISP_RegionStatsType output0[ISP_NUM_REGIONS_GROUP_0];
   ISP_RegionStatsType output1[ISP_NUM_REGIONS_GROUP_1 + ISP_NUM_REGIONS_GROUP_2];
   ISP_StatsOutput2Type output2[24];
   ISP_StatsOutput3Type output3[ISP_NUM_HISTOGRAM_BINS];
} ISP_StatsOutputType;

#endif /* ISP_STATS_H */
/** @} */