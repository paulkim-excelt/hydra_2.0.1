/*****************************************************************************
 Copyright 2016-2018 Broadcom Limited.  All rights reserved.

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
    @defgroup grp_avtdrv_ifc AVT
    @ingroup grp_avtdrv

    @addtogroup grp_avtdrv_ifc
    @{
    @section sec_avt_overview Overview
    AVT driver provides Media Clock Generator and Time-stamping units, which
    serve as a tool to replicate and track multiple independent media clocks
    across the network regardless of the variation of the crystal oscillator
    clocks used in the nodes. It provides the following features:
    - Three independently tunable media clocks
    - Frequency tracking with the resolution of 1ppm and jitter <200ps RMS
    - Audio, Display and Camera clock generation and tracking
    - Audio and Video synchronization for low-latency media streaming
    - Accuracy of time synchronization better than 50ns
    - Four concurrent time-stampers for system hardware events
    - Time-stamper for software event
    - Periodic trigger and bit clock generation from generated media clocks
    - 64-bit System and Media wall clocks and timestamps

    @section sec_avt_fsm State Machine
    NA

    @section sec_avt_seq Sequence Diagrams
    @subsection subsec_avt_seq_mts Media Time-Stamper usage
    @image html avt_seq_diagram_mts.jpg "Sequence diagram"

    @section sec_avt_perf Performance & Memory

    | Performance       | Description                                       |
    |:-----------------:|:-------------------------------------------------:|
    | CPU Load          |   NA                                              |
    | Memory Bandwidth  |   NA                                              |
    | HW Utilization    |   NA                                              |

    @limitations None

    @file avt.h
    @brief AVT Application interface
    This header file contains the APIs for AVT driver.
    @version 0.50 Imported from docx
*/

#ifndef AVT_H
#define AVT_H

#include <bcm_time.h>

/**
    @name AVT API IDs
    @{
    @brief API IDs for AVT driver
*/
#define BRCM_SWARCH_AVT_MTSCHANID_TYPE          (0x01U) /**< @brief #AVT_MTSChanIDType      */
#define BRCM_SWARCH_AVT_MTSPRESCALER_TYPE       (0x02U) /**< @brief #AVT_MTSPrescalerType   */
#define BRCM_SWARCH_AVT_MTSCLOCKSEL_TYPE        (0x03U) /**< @brief #AVT_MTSClockSelType    */
#define BRCM_SWARCH_AVT_MTSTRIG_TYPE            (0x04U) /**< @brief #AVT_MTSTrigType        */
#define BRCM_SWARCH_AVT_MTSCONFIG_TYPE          (0x05U) /**< @brief #AVT_MTSConfigType      */
#define BRCM_SWARCH_AVT_NCO_CHANID_TYPE         (0x06U) /**< @brief #AVT_NCOChanIDType      */
#define BRCM_SWARCH_AVT_EN_MTS_PROC             (0x07U) /**< @brief #AVT_EnableMTS          */
#define BRCM_SWARCH_AVT_DIS_MTS_PROC            (0x08U) /**< @brief #AVT_DisableMTS         */
#define BRCM_SWARCH_AVT_GET_MTS_VAL_PROC        (0x09U) /**< @brief #AVT_GetMTSVal          */
#define BRCM_SWARCH_AVT_SET_PERIODIC_TIMER_PROC (0x0AU) /**< @brief #AVT_SetPeriodicTimer   */
#define BRCM_SWARCH_AVT_GET_SYS_TIME_PROC       (0x0BU) /**< @brief #AVT_GetSystemTime      */
#define BRCM_SWARCH_AVT_GET_TIME_PROC           (0x0CU) /**< @brief #AVT_GetTime            */
#define BRCM_SWARCH_AVT_RESET_1588_PROC         (0x0DU) /**< @brief #AVT_Reset1588          */
#define BRCM_SWARCH_AVT_SETNCOFREQ_PROC         (0x0EU) /**< @brief #AVT_SetNCOFreq         */
#define BRCM_SWARCH_AVT_TUNENCOFREQ_PROC        (0x0FU) /**< @brief #AVT_TuneNCOFreq        */
/** @} */

/**
    @name AVT_MTSChanIDType
    @{
    @brief IDs of MTS channels in the AVT driver

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
*/
typedef uint32_t AVT_MTSChanIDType;   /**< @brief MTS channel ID type               */
#define AVT_MTSCHANID_0       (0UL)   /**< @brief MTS Channel 0                     */
#define AVT_MTSCHANID_1       (1UL)   /**< @brief MTS Channel 1                     */
#define AVT_MTSCHANID_2       (2UL)   /**< @brief MTS Channel 2                     */
#define AVT_MTSCHANID_3       (3UL)   /**< @brief MTS Channel 3 (intended for
                                            display and camera time-stamping and
                                            control)                                */
#define AVT_MTSCHANID_MAX     (4UL)   /**< @brief Maximum number of MTS
                                            channels                                */
/** @} */

/**
    @name AVT_MTSPrescalerType
    @{
    @brief Supported Media Clock Pre-scalers

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
*/
typedef uint32_t AVT_MTSPrescalerType;      /**< @brief MTS pre-scaler type     */
#define AVT_MTSPRESCALER_1      (0x00UL)    /**< @brief Divide media clock by 1 */
#define AVT_MTSPRESCALER_2      (0x10UL)    /**< @brief Divide media clock by 2 */
#define AVT_MTSPRESCALER_4      (0x20UL)    /**< @brief Divide media clock by 4 */
#define AVT_MTSPRESCALER_8      (0x30UL)    /**< @brief Divide media clock by 8 */
/** @} */

/**
    @name AVT_MTSClockSelType
    @{
    @brief Media Clock Select for Media Clock Counter

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
*/
typedef uint32_t AVT_MTSClockSelType;           /**< @brief MTS clock select type           */
#define AVT_MTSCLOCKSEL_I2S_BITCLK      (0UL)   /**< @brief I2S Bit Clock (Only for
                                                     #AVT_MTSCHANID_0, #AVT_MTSCHANID_1 and
                                                     #AVT_MTSCHANID_2)                      */
#define AVT_MTSCLOCKSEL_I2S_WORDSEL     (1UL)   /**< @brief I2S Word-Select (Only for
                                                     #AVT_MTSCHANID_0, #AVT_MTSCHANID_1 and
                                                     #AVT_MTSCHANID_2)                      */
#define AVT_MTSCLOCKSEL_I2S_GPIO        (2UL)   /**< @brief I2S GPIO input                  */
#define AVT_MTSCLOCKSEL_PIXEL_CLK       (AVT_MTSCLOCKSEL_I2S_BITCLK)
                                                /**< @brief Pixel Clock (Only
                                                     for #AVT_MTSCHANID_3. Muxed
                                                     at top level between Camera
                                                     Pixel Clock and Display
                                                     Pixel Clock)                           */
#define AVT_MTSCLOCKSEL_SEL_DISABLE     (AVT_MTSCLOCKSEL_I2S_WORDSEL)
                                                /**< @brief Media clock disable
                                                     (Only for #AVT_MTSCHANID_3)            */
/** @} */

/**
    @name AVT_MTSTrigType
    @{
    @brief Media Trigger select

    Media Trigger select used to take media timestamp.

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
*/
typedef uint32_t AVT_MTSTrigType;      /**< @brief MTS trigger type                 */
#define AVT_MTSTRIG_PRESENT    (0x0UL) /**< @brief Presentation trigger             */
#define AVT_MTSTRIG_PERIODIC   (0x4UL) /**< @brief Media periodic trigger           */
#define AVT_MTSTRIG_SNAPSHOT   (0x8UL) /**< @brief Media snapshot. Muxed at top
                                             level between Camera VSYNC and
                                             Display VSYNC.                         */
#define AVT_MTSTRIG_SOFTWARE   (0xCUL) /**< @brief Software snapshot                */
/** @} */

/**
    @brief MTS configuration

    @trace #BRCM_SWREQ_AVT_KERNEL_INTERFACE
 */
typedef struct _AVT_MTSConfigType {
    AVT_MTSChanIDType       channelID;      /**< @brief MTS channel ID                  */
    AVT_MTSPrescalerType    preScaler;      /**< @brief Media clock pre-scaler select   */
    AVT_MTSClockSelType     clockSel;       /**< @brief Media clock select              */
    AVT_MTSTrigType         trigger;        /**< @brief Media trigger select            */
} AVT_MTSConfigType;

/**
    @name NCO Channel ID Type
    @{
    @brief NCO Channel ID Type

    @trace #BRCM_SWREQ_AVT_NCO
*/
typedef uint32_t AVT_NCOChanIDType;        /**< @brief NCO channel number               */
#define AVT_NCO_CHANID_0             (0UL) /**< @brief NCO channel 0                    */
#define AVT_NCO_CHANID_1             (1UL) /**< @brief NCO channel 1                    */
#define AVT_NCO_CHANID_2             (2UL) /**< @brief NCO channel 2                    */
#define AVT_NCO_CHANID_MAX           (3UL) /**< @brief Maximum number of nco channels   */
/** @} */

/** @brief Configure and enable MTS channel

    This API configures and enables the given MTS channel (as part of MTS
    configuration data).

    @behavior Sync, Re-entrant if MTS channels are different

    @pre TBD

    @param[in]      aMTSCfg     Pointer to MTS configuration

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MTS channel configured and enabled
                                        successfully
    @retval     #BCM_ERR_INVAL_PARAMS   AVT_MTSConfigType::channelID is not
                                        #AVT_MTSCHANID_3
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid MTS configuration
    @retval     #BCM_ERR_INVAL_PARAMS   @a aMTSCfg is NULL

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations In the current version only #AVT_MTSCHANID_3 is supported. If any
    other channel is passed as part of configuration data, the API shall return
    #BCM_ERR_INVAL_PARAMS.
*/
extern int32_t AVT_EnableMTS(const AVT_MTSConfigType *const aMTSCfg);

/** @brief Disable MTS channel

    This API disables the given MTS channel.

    @behavior Sync, Re-entrant if @a aChannelID is different

    @pre TBD

    @param[in]      aChannelID  ID of the MTS channel to be disabled

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MTS channel disabled successfully
    @retval     #BCM_ERR_INVAL_PARAMS   aChannelID is not #AVT_MTSCHANID_3

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations In the current version only #AVT_MTSCHANID_3 is supported. If any
    other channel is passed as the channel to be disabled, the API shall return
    #BCM_ERR_INVAL_PARAMS.
*/
extern int32_t AVT_DisableMTS(AVT_MTSChanIDType aChannelID);

/** @brief Retrieve MTS time-stamp value

    API to obtain the MTS time-stamp value (in nanoseconds) for the given MTS
    channel.

    @behavior Sync, Re-entrant if @a aChannelID is different

    @pre TBD

    @param[in]      aChannelID  ID of the MTS channel to be disabled
    @param[out]     aTimeStamp  Time-stamp value (in nanoseconds)

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK             MTS time-stamp value obtained
                                        successfully
    @retval     #BCM_ERR_INVAL_PARAMS   Invalid MTS Channel
    @retval     #BCM_ERR_INVAL_PARAMS   aTimeStamp is NULL

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations If this API is called on an MTS channel that was not configured
    and enabled (using #AVT_EnableMTS API), the time-stamp value shall be zero.
    @limitations In the current version only #AVT_MTSCHANID_3 is supported. If any
    other channel is passed as @a aChannelID, the API shall return
    #BCM_ERR_INVAL_PARAMS.
*/
extern int32_t AVT_GetMTSVal(AVT_MTSChanIDType aChannelID, uint64_t *aTimeStamp);

/** @brief Set the AVT periodic timer counter value

    This API sets the periodic timer value and enables the Periodic timer
    interrupt.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[in]      aTimeUS     Periodic timer value (in microseconds) to be set

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations None
*/
extern int32_t AVT_SetPeriodicTimer(uint32_t aTimeUS);

/** @brief Retrieve the current system time

    API to retrieve the current system time.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[out]     aTime       Pointer to #Time structure

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations This API shall only be called from privileged context.
    @limitations This API is expensive in terms of system performance and hence
    shall be used carefully.
*/
extern uint64_t AVT_GetSystemTime(void);

/** @brief Retrieve the current system time

    API to retrieve the current system time.

    @behavior Sync, Re-entrant

    @pre TBD

    @param[out]     aTime       Pointer to #Time structure

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AVT_TIME

    @limitations This API shall only be called from unprivileged context.
*/
extern uint64_t AVT_GetTime(void);

/** @brief Reset 1588 block in the BRPHY

    API to reset the 1588 block in the BRPHY.

    @behavior Sync, Non-reentrant

    @pre TBD

    @param[out]     aSysSnapshot    Pointer to system time snapshot. Contains
                                    the system time snapshot of the moment at
                                    which the 1588 block resets

    @return     void

    @post TBD

    @trace #BRCM_SWREQ_AVT_1588_RESET

    @limitations None
*/
extern int32_t AVT_Reset1588(uint64_t *const aSysSnapshot);

/** @brief Set output frequency for the given NCO channel

    This API will set the output frequency for the given NCO channel.

    @pre TBD

    @param[in]   aChannel            NCO ChannelID
    @param[in]   aFreq               Frquency to be set in Hz

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Frequency set successfully
    @retval      BCM_ERR_INVAL_PARAMS    Invalid NCO channel
    @retval      BCM_ERR_INVAL_PARAMS    aFreq is beyond the supported range

    @post TBD

    @trace #BRCM_SWREQ_AVT_NCO
*/
extern int32_t AVT_SetNCOFreq(AVT_NCOChanIDType aChannel, uint32_t aFreq);

/** @brief Tune already set output frequency of the given NCO channel

    This API tunes the already set output frequency for the the given NCO
    channel.

    @pre TBD

    @param[in]   aChannel            NCO Channel ID
    @param[in]   aCorrection         Correction value (in Hz)

    Return values are documented in reverse-chronological order
    @retval      BCM_ERR_OK              Frequency tuned succcessfully
    @retval      BCM_ERR_INVAL_PARAMS    Invalid NCO channel
    @retval      BCM_ERR_INVAL_PARAMS    Invalid correction value

    @post TBD

    @trace #BRCM_SWREQ_AVT_NCO
*/
extern int32_t AVT_TuneNCOFreq(AVT_NCOChanIDType aChannel,
                                  int32_t aCorrection);

#endif /* AVT_H */
/** @} */
