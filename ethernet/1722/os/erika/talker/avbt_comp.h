/*****************************************************************************
Copyright 2017-2019 Broadcom Limited.  All rights reserved.

This program is the proprietary software of Broadcom Corporation and/or its
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
  @addtogroup grp_avbt_ifc
  @{
  @file avbt_comp.h
  @brief Data internal data structures of the AVBT Component
  This file provides the internal data structures and macros
  used by the AVBT component.
*/

#ifndef AVBT_COMP_H
#define AVBT_COMP_H

#include "avb_talker_cfg.h"
#include <msg_queue.h>

/**
    @name AVBT Architecture IDs
    @{
    @brief Architecture IDs for AVB Talker

    This is the list of IDs for the data elements which are part
    of the AVBT component.
*/
#define BRCM_SWARCH_AVBT_INPUTPORTNUM_TYPE              (0x86U)   /**< @brief #AVBT_InputportNumType */
#define BRCM_SWARCH_AVBT_AVTP_PAYLOAD_SIZE_MACRO        (0x87U)   /**< @brief #AVBT_AVTP_PAYLOAD_SIZE */
#define BRCM_SWARCH_AVBT_NUM_PORT_BUFFERS_MACRO         (0x88U)   /**< @brief #AVBT_PORT_NUM_BUFFERS */
#define BRCM_SWARCH_AVBT_PORT_BUFSIZE_MACRO             (0x89U)   /**< @brief #AVBT_PORT_BUF_SIZE */
#define BRCM_SWARCH_AVBT_MIN_ETHBUF_MACRO               (0x8AU)   /**< @brief #AVBT_MIN_ETH_BUF_SIZE */
#define BRCM_SWARCH_AVBT_PORTIDX_OFFSET_MACRO           (0x8BU)   /**< @brief #AVBT_PORT_IDX_OFFSET */
#define BRCM_SWARCH_AVBT_CIRCQ_SIZE_MACRO               (0x8CU)   /**< @brief #AVBT_COMP_CIRCQ_SIZE */
#define BRCM_SWARCH_AVBT_1722CFG_STR_MACRO              (0x8DU)   /**< @brief #AVBT_CONFIG_1722_STR */
#define BRCM_SWARCH_AVBT_COMPSTATE_TYPE                 (0x8EU)   /**< @brief #AVBT_CompStateType */
#define BRCM_SWARCH_AVBT_TLKR_STATS_TYPE                (0x8FU)   /**< @brief #AVBT_Tlkr_StatsType */
#define BRCM_SWARCH_AVBT_BUFINFO_TYPE                   (0x90U)   /**< @brief #AVBT_BufInfoType */
#define BRCM_SWARCH_AVBT_STACKHANDLE_TYPE               (0x91U)   /**< @brief #AVBT_StackHandleType */
#define BRCM_SWARCH_AVBT_FILLBUFFERDONEPROCESS_PROC     (0x92U)   /**< @brief #AVBT_FillBufferDoneProcess */
/** @} */

/**
    @name Port index IDs
    @{
    @brief Port indexes for AVBT component

    @trace #BRCM_SWREQ_AVBT_INIT
 */
typedef uint32_t AVBT_InputportNumType; /**< @brief Port index number  */
#define AVBT_INPUTPORTNUM_0     0UL     /**< @brief Port index 0 value */
#define AVBT_INPUTPORTNUM_1     1UL     /**< @brief Port index 1 value */
#define AVBT_INPUTPORTNUM_2     2UL     /**< @brief Port index 2 value */
#define AVBT_INPUTPORTNUM_3     3UL     /**< @brief Port index 3 value */
/** @} */

/**
    @brief Size of each payload for transmit

    This macro defines the size in bytes of the payload used and is used
    to compute the input buffer size.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_AVTP_PAYLOAD_SIZE    (1440UL)

/**
    @brief number of input port buffers

    This macro defines the number of port buffers used by each
    input port of the AVBT component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_PORT_NUM_BUFFERS     (4UL)

/**
    @brief Minimum size of ethernet buffer needed

    This macro defines the value for the minimum size of ethernet buffer
    for us to transmit. Anything less than this means that there is too
    much header overhead making it inefficient.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_MIN_ETH_BUF_SIZE     (100UL)

/**
    @brief Index for storing Port index value

    This macro defines the index in MSYS_BufferType structure used to
    store port index.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_PORT_IDX_OFFSET      (MSYS_CHANNEL_ATTRIB_RSVD1)

/**
    @brief Size of port circular queue

    This macro defines the number of entries in the each input port
    circular queue of this component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_COMP_CIRCQ_SIZE      (8UL)

/**
    @brief Macro for AVBT Talker Configuration

    This macro contains the value of the configuration image for the
    AVB Talker component. The application shall pass the configuration
    image along with the configuration image during initialization.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#define AVBT_CONFIG_1722_STR  (MSYS_CONFIG_AVB_TLKR)

/**
    @brief Size of input port buffer

    This macro defines the size in bytes of the port buffer
    used by the input ports of the AVBT component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
#ifndef AVBT_PORT_BUF_SIZE
#define AVBT_PORT_BUF_SIZE         (AVBT_AVTP_PAYLOAD_SIZE * 5UL)
#endif

/**
    @{
    @brief Component state of the component

    This group has all the states of the AVBT component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef uint32_t AVBT_CompStateType;     /**< @brief Component state */
#define AVBT_COMPSTATE_RESET  0UL        /**< @brief Reset state */
#define AVBT_COMPSTATE_INIT   1UL        /**< @brief Initialized state */
#define AVBT_COMPSTATE_READY  2UL        /**< @brief Ready state */
#define AVBT_COMPSTATE_START  3UL        /**< @brief Start state */
/** @} */

/**
    @brief Statistics maintained by the AVBT component

    This structure has all the statistics that are maintained for
    operation of AVBT component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_Tlkr_StatsType{
    uint32_t pktTxCount;           /**< @brief Number of 1722 Tx packets transmitted */
    uint32_t sliceEndCount;        /**< @brief Number of slice end markers encountered */
    uint32_t pktTxHdrAddnErrors;   /**< @brief Number of errors noted when creating the 1722 header */
    uint32_t pktTxDiscard;         /**< @brief Number of Tx packets discarded (error in sending) */
    uint32_t pktGetTxBufFailure;   /**< @brief Number of Get ethernet Tx buffer failure */
    uint32_t fillBufFailCount;     /**< @brief Number of MSYS_FillBuffer API failure */
} AVBT_Tlkr_StatsType;

/**
    @brief Input port circular queue structure

    This structure is used to maintain an entry in the port circular
    queue of each input port.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_BufInfoType {
    uint32_t bufIdx;             /**< @brief Index of memory pool */
    uint32_t actualLen;          /**< @brief Buffer/Data length */
    uint32_t procLen;            /**< @brief Size of processed data len */
    uint32_t rsv;                /**< @brief Reserved */
    uint64_t timestamp;          /**< @brief Timestamp to be filled/passed */
    uint32_t syncStart;          /**< @brief Start of slice flag */
    uint32_t syncEnd;            /**< @brief End of slice flag */
    uint32_t frameEnd;           /**< @brief End of frame flag */
    uint32_t isDataUnderProcess; /**< @brief Flag to track is current buffer is under process */
    uint32_t strInfo;            /**< @brief Stream information */
} AVBT_BufInfoType;

/**
    @brief AVBT component context structure

    Internal structure used to manage the AVBT component.

    @trace #BRCM_SWREQ_AVBT_INIT
*/
typedef struct _AVBT_StackHandleType{
    AVBT_CompStateType        compState;                                /**< @brief State of the AVB talker component */
    AVBT_Stream1722CfgType    compCfg[AVBT_MAX_NUM_INPUT_PORTS];        /**< @brief Configuration for the component */
    AVB_1722TlkrBufParamsType bufparam[AVBT_MAX_NUM_INPUT_PORTS];       /**< @brief Stream configuration for 1722 header addition */
    uint32_t                  numInpPort;                               /**< @brief Number of input ports activated */
    AVBT_BufInfoType          circQData[AVBT_MAX_NUM_INPUT_PORTS];      /**< @brief The current data from circularQ that is being processed
                                                                             It is used only when ethernet Tx buffersare unavailable
                                                                             for processing */
    AVBT_Tlkr_StatsType       talkerStats[AVBT_MAX_NUM_INPUT_PORTS];    /**< @brief The data structure used to maintain stats */
    NIF_CntrlIDType           ethCtrlIdx[AVBT_MAX_NUM_INPUT_PORTS];     /**< @brief NIF controller index */
    uint32_t                  isDataAvailable[AVBT_MAX_NUM_INPUT_PORTS];/**< @brief Flag indicating whether data is still available
                                                                             in the circular queue */
} AVBT_StackHandleType;

/**
   @brief AVBT Fill Buffer Done Handler function

   API to receive the data payload from upstream component, and
   add it to port circular queue from which it will be processed
   by the AVBT component task.

   @behavior Sync, Non-reentrant

   @pre None

   @param[in]   aMediaBuffer   Pointer to Framework's MSYS_BufferType which
                               contains the filled payload from the
                               upstream component.

   @retval  None

   @post None

   @trace #BRCM_SWREQ_AVBT_INIT

   @limitations None
*/
void AVBT_FillBufferDoneProcess (MSYS_BufferType *const aMediaBuffer);

#endif /* AVBT_COMP_H */

/** @} */
