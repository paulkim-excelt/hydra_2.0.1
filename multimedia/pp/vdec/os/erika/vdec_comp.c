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
    @defgroup grp_vdec_comp_impl Implementation
    @ingroup grp_vdec

    @addtogroup grp_vdec_comp_impl
    @{
    Vendor decoder component implementation

    @file vdec_comp.c
    @brief Vendor decoder component implementation
    This source file contains the implementation of functions for vdec component
    @version 0.1 Initial version
*/
#include <string.h>
#include <media_port.h>
#include <component.h>
#include <msg_queue.h>
#include <system.h>
#include <bcm_err.h>
#include <ee.h>
#include <vdec_lib.h>
#include <vdec.h>
#include <bcm_time.h>
#include <osil/bcm_osil.h>
#include <imgl.h>
#include <bcm_utils.h>

/**
    @name VDEC Component Design IDs
    @{
    @brief Design IDs for VDEC Component
*/

#define BRCM_SWDSGN_VDEC_COMP_STATE_TYPE                (0xC0U)   /**< @brief #VDEC_CompStateType                  */
#define BRCM_SWDSGN_VDEC_COMP_BINARY_MACRO              (0xC1U)   /**< @brief #VDEC_COMP_BINARY_MP_SIZE_8_BIT      */
#define BRCM_SWDSGN_VDEC_INT_MEM_DATA_SIZE_MACRO        (0xC2U)   /**< @brief #VDEC_INT_MEM_DATA_SIZE              */
#define BRCM_SWDSGN_VDEC_IN_PORT_IDX_TYPE               (0xC3U)   /**< @brief #VDEC_InPortIdxType                  */
#define BRCM_SWDSGN_VDEC_OUT_PORT_IDX_TYPE              (0xC4U)   /**< @brief #VDEC_OutPortIdxType                 */
#define BRCM_SWDSGN_VDEC_EVENT_MACRO                    (0xC5U)   /**< @brief #VDEC_EVENT_CMDQ_SERVER              */
#define BRCM_SWDSGN_VDEC_BUF_INFO_TYPE                  (0xC6U)   /**< @brief #VDEC_BufInfoType                    */
#define BRCM_SWDSGN_VDEC_COMP_CONTEXT_TYPE              (0xC7U)   /**< @brief #VDEC_CompContexType                 */
#define BRCM_SWDSGN_VDEC_CONFIG_DATA_TYPE               (0xC8U)   /**< @brief #VDEC_ConfigDataType                 */
#define BRCM_SWDSGN_VDEC_IN_PORT_EVENT_GLOBAL           (0xC9U)   /**< @brief #VDEC_InPortEvent                    */
#define BRCM_SWDSGN_VDEC_COMP_CONTEXT_GLOBAL            (0xCAU)   /**< @brief #VDEC_CompContext                    */
#define BRCM_SWDSGN_VDEC_STREAM_CONFIG_GLOBAL           (0xCBU)   /**< @brief #VDEC_StreamConfigData               */
#define BRCM_SWDSGN_VDEC_SP_BINARY_CAVLC_GLOBAL         (0xCCU)   /**< @brief #VDEC_SPBinaryCavlc                  */
#define BRCM_SWDSGN_VDEC_SP_BINARY_CABAC_GLOBAL         (0xCDU)   /**< @brief #VDEC_SPBinaryCabac                  */
#define BRCM_SWDSGN_VDEC_FW_ID_GLOBAL                   (0xCEU)   /**< @brief #VDEC_FwID                           */
#define BRCM_SWDSGN_VDEC_BINARY_ADDR_GLOBAL             (0xCFU)   /**< @brief #VDEC_BinaryAddr                     */
#define BRCM_SWDSGN_VDEC_INT_MEMORY_GLOBAL              (0xD0U)   /**< @brief #VDEC_IntMem                         */
#define BRCM_SWDSGN_VDEC_REF_MEMORY_GLOBAL              (0xD1U)   /**< @brief #VDEC_RefMem                         */
#define BRCM_SWDSGN_VDEC_IN_DATA_GLOBAL                 (0xD2U)   /**< @brief #VDEC_InData                         */
#define BRCM_SWDSGN_VDEC_REPORT_ERROR_PROC              (0xD3U)   /**< @brief #VDEC_ReportError                    */
#define BRCM_SWDSGN_VDEC_FILL_MEDIA_BUFFER_PROC         (0xD4U)   /**< @brief #VDEC_FillMediaBuffer                */
#define BRCM_SWDSGN_VDEC_FILL_BUFFER_DONE_PROC          (0xD5U)   /**< @brief #VDEC_FillBufferDone                 */
#define BRCM_SWDSGN_VDEC_IN_PORT0_GLOBAL                (0xD6U)   /**< @brief #VDEC_InPort0                        */
#define BRCM_SWDSGN_VDEC_IN_PORT1_GLOBAL                (0xD7U)   /**< @brief #VDEC_InPort1                        */
#define BRCM_SWDSGN_VDEC_IN_PORT2_GLOBAL                (0xD8U)   /**< @brief #VDEC_InPort2                        */
#define BRCM_SWDSGN_VDEC_IN_PORT3_GLOBAL                (0xD9U)   /**< @brief #VDEC_InPort3                        */
#define BRCM_SWDSGN_VDEC_IN_PORTS_GLOBAL                (0xDAU)   /**< @brief #VDEC_MediaInPortPtr                 */
#define BRCM_SWDSGN_VDEC_CMD_MSG_Q_GLOBAL               (0xDBU)   /**< @brief #VDEC_CmdMsgQ                        */
#define BRCM_SWDSGN_VDEC_GET_CMD_MSG_PROC               (0xDCU)   /**< @brief #VDEC_GetCmdMsg                      */
#define BRCM_SWDSGN_VDEC_CMD_Q_GLOBAL                   (0xDDU)   /**< @brief #VDEC_CmdQ                           */
#define BRCM_SWDSGN_VDEC_MEDIA_BUFFER_GLOBAL            (0xDEU)   /**< @brief #VDEC_MediaBuffer                    */
#define BRCM_SWDSGN_VDEC_GET_DATA_MSG0_PROC             (0xDFU)   /**< @brief #VDEC_GetDataMsg0                    */
#define BRCM_SWDSGN_VDEC_OUT_PORT0_GLOBAL               (0xE0U)   /**< @brief #VDEC_OutPort0                       */
#define BRCM_SWDSGN_VDEC_GET_DATA_MSG1_PROC             (0xE1U)   /**< @brief #VDEC_GetDataMsg1                    */
#define BRCM_SWDSGN_VDEC_OUT_PORT1_GLOBAL               (0xE2U)   /**< @brief #VDEC_OutPort1                       */
#define BRCM_SWDSGN_VDEC_GET_DATA_MSG2_PROC             (0xE3U)   /**< @brief #VDEC_GetDataMsg2                    */
#define BRCM_SWDSGN_VDEC_OUT_PORT2_GLOBAL               (0xE4U)   /**< @brief #VDEC_OutPort2                       */
#define BRCM_SWDSGN_VDEC_GET_DATA_MSG3_PROC             (0xE5U)   /**< @brief #VDEC_GetDataMsg3                    */
#define BRCM_SWDSGN_VDEC_OUT_PORT3_GLOBAL               (0xE6U)   /**< @brief #VDEC_OutPort3                       */
#define BRCM_SWDSGN_VDEC_OUT_PORTS_GLOBAL               (0xE7U)   /**< @brief #VDEC_MediaOutPortPtr                */
#define BRCM_SWDSGN_VDEC_SEND_CMD_PROC                  (0xE8U)   /**< @brief #VDEC_SendCmd                        */
#define BRCM_SWDSGN_VDEC_RECV_RESP_PROC                 (0xE9U)   /**< @brief #VDEC_RecvResp                       */
#define BRCM_SWDSGN_VDEC_GET_MEDIA_OUTPORT_PROC         (0xEAU)   /**< @brief #VDEC_GetMediaOutPort                */
#define BRCM_SWDSGN_VDEC_GET_MEDIA_INPORT_PROC          (0xEBU)   /**< @brief #VDEC_GetMediaInPort                 */
#define BRCM_SWDSGN_VDEC_COMP_FUNC_TABLE_GLOBAL         (0xECU)   /**< @brief #VDEC_CompFnTable                    */
#define BRCM_SWDSGN_VDEC_RESET_STATE_HANDLER_PROC       (0xEDU)   /**< @brief #VDEC_ResetStateHandler              */
#define BRCM_SWDSGN_VDEC_CONFIG_LIB_PROC                (0xEEU)   /**< @brief #VDEC_ConfigLib                      */
#define BRCM_SWDSGN_VDEC_LOAD_IMG_PROC                  (0xEFU)   /**< @brief #VDEC_LoadImg                        */
#define BRCM_SWDSGN_VDEC_INIT_STATE_HANDLER_PROC        (0xF0U)   /**< @brief #VDEC_InitStateHandler               */
#define BRCM_SWDSGN_VDEC_READY_STATE_HANDLER_PROC       (0xF1U)   /**< @brief #VDEC_ReadyStateHandler              */
#define BRCM_SWDSGN_VDEC_PROCESS_FLAGS_PROC             (0xF2U)   /**< @brief #VDEC_ProcessFlags                   */
#define BRCM_SWDSGN_VDEC_COPY_FRAME_DATA_REQ_PROC       (0xF3U)   /**< @brief #VDEC_CopyFrameDataReq                */
#define BRCM_SWDSGN_VDEC_TRY_SUBMIT_DECODE_PROC         (0xF4U)   /**< @brief #VDEC_TrySubmitDecode                */
#define BRCM_SWDSGN_VDEC_PROCESS_IN_PORT_PROC           (0xF5U)   /**< @brief #VDEC_ProcessInPort                  */
#define BRCM_SWDSGN_VDEC_STOP_CMD_HANDLER_PROC          (0xF6U)   /**< @brief #VDEC_StopCmdHandler                 */
#define BRCM_SWDSGN_VDEC_RUN_STATE_HANDLER_PROC         (0xF7U)   /**< @brief #VDEC_RunStateHandler                */
#define BRCM_SWDSGN_VDEC_TASK_PROC                      (0xF8U)   /**< @brief #VDEC_Task                           */
#define BRCM_SWDSGN_VDEC_GET_COMP_FN_TABLE_PROC         (0xF9U)   /**< @brief #VDEC_GetCompFnTable                 */
#define BRCM_SWDSGN_VDEC_GET_REF_BUF_PROC               (0xFAU)   /**< @brief #VDEC_GetRefBuf                      */
#define BRCM_SWDSGN_VDEC_MP_BINARY_8BIT_GLOBAL          (0xFBU)   /**< @brief #VDEC_MPBinary8Bit                   */
#define BRCM_SWDSGN_VDEC_MP_BINARY_NBIT_GLOBAL          (0xFCU)   /**< @brief #VDEC_MPBinaryNBit                   */
#define BRCM_SWDSGN_VDEC_MP_BINARY_COPY_GLOBAL          (0xFDU)   /**< @brief #VDEC_MPBinaryCopy                   */

/** @} */

/**
    @name VDEC_CompStateType IDs
    @{
    @brief State ID for component

    State of the component

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
typedef uint32_t VDEC_CompStateType;        /**< @brief VDEC_CompStateType typedef */
#define VDEC_COMP_STATE_RESET    (0UL)      /**< @brief Reset state */
#define VDEC_COMP_STATE_INIT_PRE (1UL)      /**< @brief Pre Init state */
#define VDEC_COMP_STATE_INIT     (2UL)      /**< @brief Initialized */
#define VDEC_COMP_STATE_READY    (3UL)      /**< @brief Ready state */
#define VDEC_COMP_STATE_RUNNING  (4UL)      /**< @brief Running state */
/** @} */

/**
    @name Binary details
    @{
    @brief Binary ID and size

    Binary IDs and size details in VDEC

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
#define VDEC_COMP_BINARY_MP_SIZE_8_BIT   (0x5000UL)
#define VDEC_COMP_BINARY_MP_SIZE_N_BIT   (0x4400UL)
#define VDEC_COMP_BINARY_MP_SIZE_COPY    (0x2800UL)
#define VDEC_COMP_BINARY_MP_COUNT        (3UL)
#define VDEC_COMP_BINARY_SP_SIZE_CABAC   (0x10000UL)
#define VDEC_COMP_BINARY_SP_SIZE_CAVLC   (0x10000UL)
#define VDEC_COMP_BINARY_SP_COUNT        (2UL)
/** @} */

/**
    @brief Internal data memory size

    Size of the buffer needed in SRAM

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
#define VDEC_INT_MEM_DATA_SIZE           (13UL * 1024UL)

/**
    @name VDEC_InPortIdxType IDs
    @{
    @brief In port ID

    In port indicies

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
typedef uint32_t VDEC_InPortIdxType;       /**< @brief VDEC_InPortIdxType typedef */
#define VDEC_IN_PORT_IDX0      (0UL)       /**< @brief In port 0 */
#define VDEC_IN_PORT_IDX1      (1UL)       /**< @brief In port 1 */
#define VDEC_IN_PORT_IDX2      (2UL)       /**< @brief In port 2 */
#define VDEC_IN_PORT_IDX3      (3UL)       /**< @brief In port 3 */
/** @} */


/**
    @name VDEC_OutPortIdxType IDs
    @{
    @brief In port ID

    In port indicies

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
typedef uint32_t VDEC_OutPortIdxType;       /**< @brief VDEC_OutPortIdxType typedef */
#define VDEC_OUT_PORT_IDX0      (0UL)       /**< @brief Out port 0 */
#define VDEC_OUT_PORT_IDX1      (1UL)       /**< @brief Out port 1 */
#define VDEC_OUT_PORT_IDX2      (2UL)       /**< @brief Out port 2 */
#define VDEC_OUT_PORT_IDX3      (3UL)       /**< @brief Out port 3 */
/** @} */

/**
    @name Event details
    @{
    @brief Event macros

    Event details

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#define VDEC_EVENT_CMDQ_SERVER     (SystemEvent0)
#define VDEC_EVENT_IMGL            (SystemEvent1)
#define VDEC_EVENT_INPORT          (SystemEvent4 | SystemEvent5 | SystemEvent6 | SystemEvent7)
#define VDEC_EVENT_OUTPORT         (SystemEvent8 | SystemEvent9 | SystemEvent10 | SystemEvent11)
/** @} */

/**
    @brief Buffer details

    Buffer information structure

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
typedef struct _VDEC_BufInfoType {
    uint64_t timestamp;               /**< @brief Time stamp */
    uint32_t size;                    /**< @brief Buffer Size */
    uint32_t bufIdx;                  /**< @brief Buffer index */
    uint32_t portIdx;                 /**< @brief In Port index */
    uint32_t frameNum;                /**< @brief Frame number */
    uint32_t frameStart;              /**< @brief Frame start flag */
    uint32_t frameEnd;                /**< @brief Frame end flag */
    VDEC_SliceInfoType sliceInfo;     /**< @brief Slice info as #VDEC_SliceInfoType */
} VDEC_BufInfoType;

/**
    @brief Component context

    Component context structure

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
 */
typedef struct _VDEC_CompContexType {
    VDEC_CompStateType state;        /**< @brief Component state */
    uint32_t stopInProgress;         /**< @brief Stop in progress flag */
    uint64_t timestamp[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief Time stamp for each stream */
    VDEC_StreamInfoType streamInfo[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief Stream info as #VDEC_StreamInfoType */
    uint32_t spsSize[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief SPS size for each stream */
    uint32_t ppsSize[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief PPS size for each stream */
    VDEC_BufInfoType bufInfo[VDEC_COMP_NUM_STREAMS][VDEC_NUM_IN_BUFFERS_PER_PORT];
                                     /**< @brief Buffer info as #VDEC_BufInfoType */
    IMGL_LoadStatusType imglStatus;  /**< @brief Image loader status as #IMGL_LoadStatusType */
    int32_t preInitError;            /**< @brief Pre init error status */
    int32_t preInitLoadIdx;          /**< @brief Pre init image load index */
    uint32_t binarySize[VDEC_COMP_BINARY_MP_COUNT + VDEC_COMP_BINARY_SP_COUNT];
                                     /**< @brief Actual binary size */
    LWQ_BufferType fillQ[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief Queue to hold filled buffers as in # LWQ_BufferType */
    LWQ_BufferType procQ[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief Queue to hold processing buffers as in # LWQ_BufferType */
    uint32_t copyFrameDataReq[VDEC_COMP_NUM_STREAMS];
                                     /**< @brief Flag indicating buffer submitted to copy decoded data */
    VDEC_ContextType context;        /**< @brief Library context as #VDEC_ContextType */
} VDEC_CompContexType;

/**
    @brief Configuration union

    Configuration data

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
 */
typedef union _VDEC_ConfigDataType {
    const uint8_t* hdr;         /**< @brief Pointer to the configuration data */
    const VDEC_ConfigType *cfg; /**< @brief Pointer to the configuration structure as in #VDEC_ConfigType */
} VDEC_ConfigDataType;

/**
    @brief VDEC_InPortEvent

    In port events

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static const
BCM_EventMaskType VDEC_InPortEvent[VDEC_COMP_NUM_STREAMS] = {
#if (VDEC_IN_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
SystemEvent4,
#endif
#if (VDEC_IN_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
SystemEvent5,
#endif
#if (VDEC_IN_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
SystemEvent6,
#endif
#if (VDEC_IN_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
SystemEvent7
#endif
};

/**
    @brief VDEC_CompContext

    Component context

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static VDEC_CompContexType VDEC_CompContext COMP_SECTION(".bss.vdec");

/**
    @brief VDEC_StreamConfigData

    Stream configuration data

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static uint8_t
VDEC_StreamConfigData[VDEC_COMP_NUM_STREAMS][2UL * VDEC_CONTEXT_MAX_STREAM_CONFIG_SIZE]
COMP_SECTION(".vdec.uncached.VDEC_StreamConfigData") COMP_ALIGN(256);

/**
    @brief VDEC_MPBinary8Bit

    MP binary 8 bit

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t VDEC_MPBinary8Bit[VDEC_COMP_BINARY_MP_SIZE_8_BIT]
COMP_SECTION(".vdec.slowmem.VDEC_PPBinary") COMP_ALIGN(256);

/**
    @brief VDEC_MPBinaryNBit

    MP binary N bit

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t VDEC_MPBinaryNBit[VDEC_COMP_BINARY_MP_SIZE_N_BIT]
COMP_SECTION(".vdec.slowmem.VDEC_PPBinary") COMP_ALIGN(256);

/**
    @brief VDEC_MPBinaryCopy

    MP binary copy

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t VDEC_MPBinaryCopy[VDEC_COMP_BINARY_MP_SIZE_COPY]
COMP_SECTION(".vdec.slowmem.VDEC_PPBinary") COMP_ALIGN(256);

/**
    @brief VDEC_SPBinaryCavlc

    SP binary cavlc

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t
VDEC_SPBinaryCavlc[VDEC_COMP_BINARY_SP_SIZE_CAVLC]
COMP_SECTION(".vdec.slowmem.VDEC_PPBinary") COMP_ALIGN(256);

/**
    @brief VDEC_SPBinaryCabac

    SP binary cabac

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t
VDEC_SPBinaryCabac[VDEC_COMP_BINARY_SP_SIZE_CABAC]
COMP_SECTION(".vdec.slowmem.VDEC_PPBinary") COMP_ALIGN(256);

/**
    @brief VDEC_FwID

    FW ID

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static const uint16_t
VDEC_FwID[VDEC_COMP_BINARY_MP_COUNT + VDEC_COMP_BINARY_SP_COUNT] =
{
    MSYS_CONFIG_VDEC_MP_8BIT,
    MSYS_CONFIG_VDEC_MP_NBIT,
    MSYS_CONFIG_VDEC_MP_COPY,
    MSYS_CONFIG_VDEC_SP_CAVLC,
    MSYS_CONFIG_VDEC_SP_CABAC,
};

/**
    @brief VDEC_BinaryAddr

    Binary data addresses

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static uint8_t* const
VDEC_BinaryAddr[VDEC_COMP_BINARY_MP_COUNT + VDEC_COMP_BINARY_SP_COUNT] =
{
    VDEC_MPBinary8Bit,
    VDEC_MPBinaryNBit,
    VDEC_MPBinaryCopy,
    VDEC_SPBinaryCavlc,
    VDEC_SPBinaryCabac
};

/**
    @brief VDEC_IntMem

    Internal memory for each stream

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static uint8_t VDEC_IntMem[VDEC_COMP_NUM_STREAMS][VDEC_INT_MEM_DATA_SIZE]
COMP_SECTION(".vdec.uncached.VDEC_IntMem") COMP_ALIGN(256);

/**
    @brief VDEC_RefMem

    Reference memory

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT
*/
static uint8_t VDEC_RefMem[VDEC_NUM_REF_BUFFERS][VDEC_REF_MEM_DATA_SIZE]
COMP_SECTION(".vdec.slowmem.VDEC_RefMem") COMP_ALIGN(256);

/**
    @brief VDEC_InData

    In port buffers

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static uint8_t
VDEC_InData[VDEC_COMP_NUM_STREAMS][VDEC_NUM_IN_BUFFERS_PER_PORT][VDEC_IN_BUFFER_SIZE]
COMP_SECTION(".vdec.slowmem.VDEC_InData") COMP_ALIGN(256);

/** @brief Report error

    Report error

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]      aApiID API ID
    @param[in]      aErr Error ID
    @param[in]      aVal0 Value 0
    @param[in]      aVal1 Value 1
    @param[in]      aVal2 Value 2
    @param[in]      aVal3 Value 3

    @return void

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}
    BCM_ReportError(#BCM_VDE_ID,#aApiID,#aErr, #aVal0, #aVal1, #aVal2, #aVal3)

    @endcode

*/
static void VDEC_ReportError(uint8_t aApiID, int32_t aErr, uint32_t aVal0,
                        uint32_t aVal1, uint32_t aVal2, uint32_t aVal3)
{
    const uint32_t values[4UL] = {aVal0, aVal1, aVal2, aVal3};
    BCM_ReportError(BCM_VDE_ID, 0U, aApiID, aErr, 4UL, values);
}

/** @brief Fill media buffer

    This submits the given buffer to the in port for filling

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]      aInPort Pointer to the in port
    @param[in]         aBufIdx Buffer index

    @return void

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}
    if component stop is not in progress
        create #MSYS_BufferType
        #MSYS_BufferType format = #MSYS_BUFFER_FORMAT_RAW_DATA
        #MSYS_BufferType numChannels = 1
        #MSYS_BufferType seqNum = 0
        Encode #aBufIdx and port index to #MSYS_BufferType bufIdx
        #MSYS_BufferType buffer 0 channelType = 0
        #MSYS_BufferType buffer 0 size = VDEC_IN_BUFFER_SIZE
        #MSYS_BufferType buffer 0 data = VDEC_InData[#aInPort portIndex][aBufIdx]

        MSYS_FillBuffer(#aInPort, #MSYS_BufferType)
        if error
            Report error

    @endcode

*/
static void VDEC_FillMediaBuffer(const MSYS_InPortType * const aInPort,
                                 uint8_t aBufIdx)
{
    int32_t  ret;

    if (0UL == VDEC_CompContext.stopInProgress) {
        const MSYS_BufferType mediaBuffer = {
            .frameNum = 0UL,
            .frameStart = 0UL,
            .frameEnd = 0UL,
            .timestamp = 0ULL,
            .version = sizeof(MSYS_BufferType),
            .response = BCM_ERR_UNKNOWN,
            .format = MSYS_BUFFER_FORMAT_RAW_DATA,
            .numChannels = 1UL,
            .seqNum = 0UL,
            .bufIdx = (uint32_t)aBufIdx | ((uint32_t)(aInPort->portIndex) << 16UL),

            .buffer[0UL].channelType = 0UL,
            .buffer[0UL].size = VDEC_IN_BUFFER_SIZE,
            .buffer[0UL].data = &VDEC_InData[aInPort->portIndex][aBufIdx][0UL],
        };

        ret = MSYS_FillBuffer(aInPort, &mediaBuffer);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_FILL_MEDIA_BUFFER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @brief Fill buffer done callback

    Fill buffer done callback

    @behavior Async, Non-reentrant

    @pre None

    @param[in]  aMediaBuffer Pointer to media buffer filled

    @return void

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aMediaBuffer filled size is not zero
        populate #VDEC_CompContext bufInfo with #aMediaBuffer contents
        ret = VDEC_GetStreamInfo(#aMediaBuffer data, bufInfo sliceInfo, min(#VDEC_CONTEXT_MAX_STREAM_CONFIG_SIZE, size))
        if ret is not ERR_OK
            report error
        if slice is SPS or PPS and config data not updated
            update config data in #VDEC_CompContext streamInfo
        else if slice is video
            Push the buffer to #VDEC_CompContext fillQ
        else
            report error
            VDEC_FillMediaBuffer(port, bufIdx)
    else
        VDEC_FillMediaBuffer(port, bufIdx)
    @endcode

*/
void VDEC_FillBufferDone(const MSYS_InPortType * const aPort,
                            MSYS_BufferType *const aMediaBuffer)
{
    int32_t ret;
    uint32_t portIdx;

    portIdx = (aMediaBuffer->bufIdx) >> 16UL;
    /* pop from submit Q */
    if (0UL != aMediaBuffer->buffer[0UL].size) {
        VDEC_BufInfoType *bufInfo;
        uint8_t bufIdx = (uint8_t) (aMediaBuffer->bufIdx & 0xFFFFUL);
        uint32_t size;

        bufInfo =  &VDEC_CompContext.bufInfo[portIdx][bufIdx];

        bufInfo->size = aMediaBuffer->buffer[0UL].size;
        bufInfo->bufIdx = aMediaBuffer->bufIdx & 0xFFFFUL;
        bufInfo->timestamp = aMediaBuffer->timestamp;
        bufInfo->portIdx = portIdx;
        bufInfo->frameNum = aMediaBuffer->frameNum;
        bufInfo->frameStart = aMediaBuffer->frameStart;
        bufInfo->frameEnd = aMediaBuffer->frameEnd;

        if (VDEC_CONTEXT_MAX_STREAM_CONFIG_SIZE < aMediaBuffer->buffer[0UL].size) {
            size = VDEC_CONTEXT_MAX_STREAM_CONFIG_SIZE;
        } else {
            size = aMediaBuffer->buffer[0UL].size;
        }
        ret = VDEC_GetStreamInfo(&VDEC_CompContext.context,
                                 &VDEC_CompContext.streamInfo[portIdx],
                                 VDEC_InData[portIdx][bufIdx],
                                 size,
                                 &bufInfo->sliceInfo);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_FILL_BUFFER_DONE_PROC,
                             ret,
                             __LINE__,
                             size,
                             portIdx,
                             bufIdx);
        }

        if ((VDEC_SLICE_SPS == bufInfo->sliceInfo.sliceType)
             || (VDEC_SLICE_PPS == bufInfo->sliceInfo.sliceType)) {
            uint32_t* configSize;
            if (VDEC_SLICE_SPS == bufInfo->sliceInfo.sliceType) {
                configSize = &VDEC_CompContext.spsSize[portIdx];
            } else {
                configSize = &VDEC_CompContext.ppsSize[portIdx];
            }

            if (0UL == *configSize) {
                /* Store the SPS */
                BCM_MemCpy(&VDEC_StreamConfigData[portIdx][VDEC_CompContext.streamInfo[portIdx].configDataSize],
                           VDEC_InData[portIdx][bufIdx],
                           aMediaBuffer->buffer[0UL].size);
                VDEC_CompContext.streamInfo[portIdx].configDataSize += aMediaBuffer->buffer[0UL].size;
                *configSize = aMediaBuffer->buffer[0UL].size;
            }

            /* call fill buffer */
            VDEC_FillMediaBuffer(aPort, bufIdx);
        } else if (VDEC_SLICE_VIDEO == bufInfo->sliceInfo.sliceType) {
            ret = LWQ_Push(&VDEC_CompContext.fillQ[portIdx], bufIdx);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_FILL_BUFFER_DONE_PROC,
                                 ret,
                                 __LINE__,
                                 portIdx,
                                 bufIdx,
                                 aMediaBuffer->buffer[0UL].size);
            }
        } else {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_FILL_BUFFER_DONE_PROC,
                             BCM_ERR_UNKNOWN,
                             __LINE__,
                             portIdx,
                             bufIdx,
                             aMediaBuffer->buffer[0UL].size);
            VDEC_FillMediaBuffer(aPort,bufIdx);
        }
    } else {
        VDEC_FillMediaBuffer(aPort, aMediaBuffer->bufIdx & 0xFFFFUL);
    }
}

/**
    @brief VDEC InPort0

    In port 0

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_IN_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_IN_PORT_V2(VDEC_InPort0,
                        VDEC_IN_PORT_IDX0,
                        MSYS_COMP_ID_VDEC,
                        SystemEvent4,
                        VDEC_InPort0RW,
                        VDEC_FillBufferDone,
                        ".data.vdec");
#endif

/**
    @brief VDEC InPort1

    In port 1

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_IN_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_IN_PORT_V2(VDEC_InPort1,
                        VDEC_IN_PORT_IDX1,
                        MSYS_COMP_ID_VDEC,
                        SystemEvent5,
                        VDEC_InPort1RW,
                        VDEC_FillBufferDone,
                        ".data.vdec");
#endif

/**
    @brief VDEC InPort2

    In port 2

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_IN_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_IN_PORT_V2(VDEC_InPort2,
                        VDEC_IN_PORT_IDX2,
                        MSYS_COMP_ID_VDEC,
                        SystemEvent6,
                        VDEC_InPort2RW,
                        VDEC_FillBufferDone,
                        ".data.vdec");
#endif

/**
    @brief VDEC InPort3

    In port 3

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_IN_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_IN_PORT_V2(VDEC_InPort3,
                        VDEC_IN_PORT_IDX3,
                        MSYS_COMP_ID_VDEC,
                        SystemEvent7,
                        VDEC_InPort3RW,
                        VDEC_FillBufferDone,
                        ".data.vdec");
#endif

/**
    @brief VDEC_MediaInPortPtr

    In ports

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static const
MSYS_InPortType* VDEC_MediaInPortPtr[VDEC_COMP_NUM_STREAMS] = {
#if (VDEC_IN_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
    &VDEC_InPort0,
#endif
#if (VDEC_IN_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
    &VDEC_InPort1,
#endif
#if (VDEC_IN_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
    &VDEC_InPort2,
#endif
#if (VDEC_IN_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
    &VDEC_InPort3
#endif
};

/**
    @brief VDEC_CmdMsgQ

    Command message queue buffer

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static MSYS_CmdType VDEC_CmdMsgQ[MSGQ_SIZE] COMP_SECTION(".bss.vdec");

/** @brief Get command message

    Get command message

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIndex Index of the message

    @return Pointer to the message

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aIndex is valid
        return VDEC_CmdMsgQ #aIndex
    @endcode

*/
static void* VDEC_GetCmdMsg(uint32_t aIndex)
{
    void * cmdMsg = NULL;
    if(MSGQ_SIZE > aIndex) {
        cmdMsg = (void *)&VDEC_CmdMsgQ[aIndex];
    }

    return cmdMsg;
}

/**
    @brief VDEC_CmdQ

    Command message queue

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
MSGQ_DEFINE_V2(VDEC_CmdQ,
               VDEC_Task,
               VDEC_EVENT_CMDQ_SERVER,
               MSGQ_CLIENT_SIGNAL_ENABLE,
               MSYS_CmdType,
               VDEC_CmdMsgHdrQ,
               VDEC_GetCmdMsg,
               ".data.vdec");

/**
    @brief VDEC_MediaBuffer

    Media buffers

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static MSYS_BufferType VDEC_MediaBuffer[VDEC_COMP_NUM_STREAMS][MSGQ_SIZE]
COMP_SECTION(".bss.vdec");

/** @brief Get data message0

    Get data message for port 0

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIndex Index of the message

    @return Pointer to the message

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aIndex is valid
        return VDEC_MediaBuffer #VDEC_OUT_PORT_IDX0 #aIndex
    @endcode

*/
#if (VDEC_OUT_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
static void* VDEC_GetDataMsg0(uint32_t aIndex)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&VDEC_MediaBuffer[VDEC_OUT_PORT_IDX0][aIndex];
    }

    return dataMsg;
}
#endif

/**
    @brief VDEC_OutPort0

    Out port 0

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_OUT_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_OUT_PORT_V2(VDEC_OutPort0,
                         VDEC_OUT_PORT_IDX0,
                         VDEC_DataMsgHdrQ0,
                         VDEC_Task,
                         VDEC_GetDataMsg0,
                         MSYS_COMP_ID_VDEC,
                         SystemEvent8,
                         ".data.vdec");
#endif

/** @brief Get data message1

    Get data message for port 1

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIndex Index of the message

    @return Pointer to the message

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aIndex is valid
        return VDEC_MediaBuffer #VDEC_OUT_PORT_IDX1 #aIndex
    @endcode

*/
#if (VDEC_OUT_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
static void* VDEC_GetDataMsg1(uint32_t aIndex)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&VDEC_MediaBuffer[VDEC_OUT_PORT_IDX1][aIndex];
    }

    return dataMsg;
}
#endif

/**
    @brief VDEC_OutPort1

    Out port 1

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_OUT_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_OUT_PORT_V2(VDEC_OutPort1,
                         VDEC_OUT_PORT_IDX1,
                         VDEC_DataMsgHdrQ1,
                         VDEC_Task,
                         VDEC_GetDataMsg1,
                         MSYS_COMP_ID_VDEC,
                         SystemEvent9,
                         ".data.vdec");
#endif

/** @brief Get data message2

    Get data message for port 2

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIndex Index of the message

    @return Pointer to the message

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aIndex is valid
        return VDEC_MediaBuffer #VDEC_OUT_PORT_IDX2 #aIndex
    @endcode

*/
#if (VDEC_OUT_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
static void* VDEC_GetDataMsg2(uint32_t aIndex)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&VDEC_MediaBuffer[VDEC_OUT_PORT_IDX2][aIndex];
    }

    return dataMsg;
}
#endif

/**
    @brief VDEC_OutPort2

    Out port 2

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_OUT_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_OUT_PORT_V2(VDEC_OutPort2,
                         VDEC_OUT_PORT_IDX2,
                         VDEC_DataMsgHdrQ2,
                         VDEC_Task,
                         VDEC_GetDataMsg2,
                         MSYS_COMP_ID_VDEC,
                         SystemEvent10,
                         ".data.vdec");
#endif

/** @brief Get data message3

    Get data message for port 3

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aIndex Index of the message

    @return Pointer to the message

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if #aIndex is valid
        return VDEC_MediaBuffer #VDEC_OUT_PORT_IDX3 #aIndex
    @endcode

*/
#if (VDEC_OUT_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
static void* VDEC_GetDataMsg3(uint32_t aIndex)
{
    void * dataMsg = NULL;
    if (MSGQ_SIZE > aIndex) {
        dataMsg = (void *)&VDEC_MediaBuffer[VDEC_OUT_PORT_IDX3][aIndex];
    }

    return dataMsg;
}
#endif

/**
    @brief VDEC_OutPort3

    Out port 3

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
#if (VDEC_OUT_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
MSYS_DECLARE_OUT_PORT_V2(VDEC_OutPort3,
                         VDEC_OUT_PORT_IDX3,
                         VDEC_DataMsgHdrQ3,
                         VDEC_Task,
                         VDEC_GetDataMsg3,
                         MSYS_COMP_ID_VDEC,
                         SystemEvent11,
                         ".data.vdec");
#endif

/**
    @brief VDEC_MediaOutPortPtr

    Out ports

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static const
MSYS_OutPortType* VDEC_MediaOutPortPtr[VDEC_COMP_NUM_STREAMS] = {
#if (VDEC_OUT_PORT_IDX0 < VDEC_COMP_NUM_STREAMS)
    &VDEC_OutPort0,
#endif
#if (VDEC_OUT_PORT_IDX1 < VDEC_COMP_NUM_STREAMS)
    &VDEC_OutPort1,
#endif
#if (VDEC_OUT_PORT_IDX2 < VDEC_COMP_NUM_STREAMS)
    &VDEC_OutPort2,
#endif
#if (VDEC_OUT_PORT_IDX3 < VDEC_COMP_NUM_STREAMS)
    &VDEC_OutPort3
#endif
};

/** @brief Post a command to the command queue of the component

    Post a command to the command queue of the component

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aMSYS_CmdType    Pointer to Cmd structure
    @param[in]   aMask   Mask to signal client task
    @param[out]  aHdr    Handle to get response, valid only when return value
                         is BCM_ERR_OK

    @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
    @retval      BCM_ERR_INVAL_PARAMS   Invalid magic ID of aCmd
    @retval      BCM_ERR_INVAL_PARAMS   aMask is zero
    @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
    @retval      others as retured by MSGQ_SendMsg()

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if arguments are valid
        if command is #MSYS_COMP_CMD_INIT
            Activate the VDEC task

        Post the command to #VDEC_CmdQ
        return status
    else
        report error
        return ERR_INVAL_PARAMS
    @endcode

*/
static int32_t VDEC_SendCmd(const MSYS_CmdType *const aCmd,
                            const uint32_t aMask,
                            const MSGQ_MsgHdrType** const aHdr)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aCmd)
        && (MSYS_COMP_ID_VDEC == aCmd->compID)
        && (NULL != aHdr)) {
        if (aCmd->cmd == MSYS_COMP_CMD_INIT) {
            ret = BCM_ActivateTask(VDEC_Task);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_SEND_CMD_PROC,
                                 ret,
                                 __LINE__,
                                 0UL,
                                 0UL,
                                 0UL);
            }
        }
        /* Post the command to the component's Command Queue */
        ret = MSGQ_SendMsg(&VDEC_CmdQ, (const void * const)aCmd, aMask, aHdr);
    } else {
        VDEC_ReportError(BRCM_SWDSGN_VDEC_SEND_CMD_PROC,
                         ret,
                         __LINE__,
                         0UL,
                         0UL,
                         0UL);
    }

    return ret;
}

/** @brief Receive response for the command posted previously

    Receive response to the command posted previously to the
                command queue of the component

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aHdr    Handle returned by SendCmd API
    @param[out]  aCmd    Response from the component for posted command
                         (command corresponding to handle in hdr)

    @retval      BCM_ERR_INVAL_PARAMS   aCmd is NULL
    @retval      BCM_ERR_INVAL_PARAMS   aHdr is NULL
    @retval      others as retured by MSGQ_RecvMsg()

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if arguments are valid

        Receive the response from #VDEC_CmdQ
        return status
    else
        report error
        return ERR_INVAL_PARAMS
    @endcode

*/
static int32_t VDEC_RecvResp(const MSGQ_MsgHdrType* const aHdr,
                             MSYS_CmdType *const aCmd)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;

    if ((NULL != aHdr) && (NULL != aCmd)) {
        ret = MSGQ_RecvMsg(&VDEC_CmdQ, aHdr, (void* const) aCmd);
    } else {
        VDEC_ReportError(BRCM_SWDSGN_VDEC_RECV_RESP_PROC,
                         ret,
                         __LINE__,
                         0UL,
                         0UL,
                         0UL);
    }

    return ret;
}

/** @brief Get the MSYS_OutPortType where the message needs to be posted to

    Get the MSYS_OutPortType where the message needs to be posted to (triggered
    when the sink component requests vdec component to fill the media buffer)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex          Port Index

    @retval      NULL aIndex is not valid
    @retval      pointer to the media outport

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if arguments are valid
        return #VDEC_MediaOutPortPtr #aIndex
    else
        report error
        return NULL
    @endcode

*/
static const MSYS_OutPortType* VDEC_GetMediaOutPort(const uint32_t aIndex)
{
    const MSYS_OutPortType *outPort = NULL;

    if (aIndex < VDEC_COMP_NUM_STREAMS) {
        outPort = VDEC_MediaOutPortPtr[aIndex];
    } else {
        VDEC_ReportError(BRCM_SWDSGN_VDEC_GET_MEDIA_OUTPORT_PROC,
                         BCM_ERR_INVAL_PARAMS,
                        __LINE__,
                        0UL,
                        0UL,
                        0UL);
    }
    return outPort;
}

/** @brief Get the MSYS_InPortType where the message needs to be posted to

    Get the MSYS_InPortType where the message needs to be posted to (triggered
    when the sink component requests this component to fill the media buffer)

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex          Port Index

    @retval      NULL aIndex is not valid
    @retval      pointer to the media inport

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Decode the port index and buffer index from #aMediaBuffer bufIdx
    if arguments are valid
        return #VDEC_MediaInPortPtr #aIndex
    else
        report error
        return NULL
    @endcode

*/
static const MSYS_InPortType* VDEC_GetMediaInPort(const uint32_t aIndex)
{
    const MSYS_InPortType *inPort = NULL;

    if (aIndex < VDEC_COMP_NUM_STREAMS) {
        inPort = VDEC_MediaInPortPtr[aIndex];
    } else {
        VDEC_ReportError(BRCM_SWDSGN_VDEC_GET_MEDIA_INPORT_PROC,
                         BCM_ERR_INVAL_PARAMS,
                        __LINE__,
                         0UL,
                         0UL,
                         0UL);
    }

    return inPort;
}

/**
    @brief VDEC_CompFnTable

    Component function table

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT
*/
static const MSYS_CompFnTblType VDEC_CompFnTable =
{
    VDEC_SendCmd,
    VDEC_RecvResp,
    VDEC_GetMediaOutPort,
    VDEC_GetMediaInPort,
};

/** @brief Reset state handler

    Reset state handler

    @behavior Sync, Non-reentrant

    @pre None

    @param   None

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    if #VDEC_CmdQ has a message
        Get the message handle
        Set message response to ERR_INVAL_PARAMS
        if the message is #MSYS_COMP_CMD_INIT
            update the library state to #VDEC_STATE_RESET
            Initialize the library context
            if error
                Report error

            Initialize component context
            For each stream
                Initialize the fill queue and process queue
                Reset the stream info
                Initialize the out port
                if error
                    report error
            Set message response to ERR_OK
            update component state to #VDEC_COMP_STATE_INIT_PRE

        Complete the message
    @endcode

*/
static void VDEC_ResetStateHandler(void)
{
    uint32_t i;
    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret=BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&VDEC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VDEC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if (processCmd->cmd == MSYS_COMP_CMD_INIT) {
            VDEC_CompContext.context.state = VDEC_STATE_RESET;
            ret = VDEC_Init(&VDEC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_RESET_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
            }

            VDEC_CompContext.stopInProgress = 0UL;
            VDEC_CompContext.state = VDEC_COMP_STATE_INIT_PRE;
            VDEC_CompContext.imglStatus.hdr = NULL;
            VDEC_CompContext.preInitError = BCM_ERR_OK;
            VDEC_CompContext.preInitLoadIdx = 0UL;
            VDEC_CompContext.binarySize[0UL] = VDEC_COMP_BINARY_MP_SIZE_8_BIT;
            VDEC_CompContext.binarySize[1UL] = VDEC_COMP_BINARY_MP_SIZE_N_BIT;
            VDEC_CompContext.binarySize[2UL] = VDEC_COMP_BINARY_MP_SIZE_COPY;
            VDEC_CompContext.binarySize[3UL] = VDEC_COMP_BINARY_SP_SIZE_CAVLC;
            VDEC_CompContext.binarySize[4UL] = VDEC_COMP_BINARY_SP_SIZE_CABAC;
            for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
                LWQ_Init(&VDEC_CompContext.fillQ[i]);
                LWQ_Init(&VDEC_CompContext.procQ[i]);
                VDEC_CompContext.timestamp[i] = 0ULL;
                BCM_MemSet(&VDEC_CompContext.streamInfo[i], 0U, sizeof(VDEC_StreamInfoType));
                ret = MSYS_OutPortInit(VDEC_MediaOutPortPtr[i]);
                if(BCM_ERR_OK != ret) {
                    VDEC_ReportError(BRCM_SWDSGN_VDEC_RESET_STATE_HANDLER_PROC, ret, __LINE__, i, 0UL, 0UL);
                }

            }
            processCmd->response = BCM_ERR_OK;
        }
        ret = MSGQ_CompleteMsgIdx(&VDEC_CmdQ, cmdMsgIdx);
        if(BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_RESET_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @brief Configure the library

    Send configuration to the library

    @behavior Sync, Non-reentrant

    @pre None

    @param[inout]   aCmd Pointer to the command

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Read #aCmd cfgData as #VDEC_ConfigDataType
    if #aCmd configID is MSYS_CONFIG_VDEC_MOD:
        if configuration data is valid
            Configure the library
            ret = VDEC_Configure(library context, #VDEC_ConfigDataType, Reference buffers)
            if ret is not ERR_OK
                report error
            Update #aCmd response as ret
    else
        Update #aCmd response as ERR_INVAL_PARAMS
    @endcode
*/
static void VDEC_ConfigLib(MSYS_CmdType *aCmd)
{
    int32_t ret;
    VDEC_ConfigDataType config;

    config.hdr = aCmd->cfgData;

    switch ((aCmd->configID & 0xFFF0UL)) {
    case MSYS_CONFIG_VDEC_MOD:
        if ((config.cfg->width <= ((MEDIA_SYSTEM_MAX_VIDEO_WIDTH + 15UL) & (~15UL)))
            && (config.cfg->height <= ((MEDIA_SYSTEM_MAX_VIDEO_HEIGHT + 15UL) & (~15UL)))) {
            ret = VDEC_Configure(&VDEC_CompContext.context, config.cfg, VDEC_NUM_REF_BUFFERS, VDEC_REF_MEM_DATA_SIZE);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_CONFIG_LIB_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            aCmd->response = ret;
        } else {
            aCmd->response = BCM_ERR_INVAL_PARAMS;
        }
    break;
    default:
        aCmd->response = BCM_ERR_INVAL_PARAMS;
    break;
    }
}

/** @brief Reset state handler

    Reset state handler

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]    aConfigID Configuration ID
    @param[inout] aBuffer   Pointer to the buffer to load into
    @param[inout] aSize     Pointer to the Store the size

    Return values are documented in reverse-chronological order
    @retval     #BCM_ERR_OK                 Image load completed
    @retval     #BCM_ERR_BUSY               Image load in progress
    @retval     #BCM_ERR_UNKNOWN            Image load failed

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    if no image is under loading
        Start the next image load as #IMGL_LoadImg
        if request successful
            ret = ERR_BUSY
            Set image under load flag
        else
            ret = ERR_UNKNOWN
    else
        Check the status of the image load as #IMGL_GetStatus
        if sucessful
            ret = ERR_OK
            update the size in #aSize
            Clear the image under load flag
        else if it is ERR_NOMEM
            ret = ERR_NOMEM
            Clear the image under load flag
        else if it is not busy
            ret = ERR_UNKNOWN
            Clear the image under load flag

    @endcode

*/
static int32_t VDEC_LoadImg(uint16_t aConfigID, uint8_t *aBuffer, uint32_t *aSize)
{
    int32_t ret = BCM_ERR_INVAL_PARAMS;
    if (NULL == VDEC_CompContext.imglStatus.hdr) {
        /* No load is in progress */
        ret = IMGL_LoadImg(aConfigID,
                           aBuffer,
                           0UL,
                           *aSize,
                           VDEC_EVENT_IMGL,
                           &VDEC_CompContext.imglStatus);
        if (BCM_ERR_OK == ret) {
            ret = BCM_ERR_BUSY;
        } else {
            ret = BCM_ERR_UNKNOWN;
        }
    } else {
        ret = IMGL_GetStatus(&VDEC_CompContext.imglStatus);
        if (BCM_ERR_NOMEM == ret) {
            ret = BCM_ERR_NOMEM;
        } else if (BCM_ERR_BUSY == ret) {
            /* Do Nothing */
            ret = BCM_ERR_BUSY;
        } else if (BCM_ERR_OK == ret) {
            *aSize = VDEC_CompContext.imglStatus.size;
            VDEC_CompContext.imglStatus.hdr = NULL;
            ret = BCM_ERR_OK;
        } else {
            VDEC_CompContext.imglStatus.hdr = NULL;
            ret = BCM_ERR_UNKNOWN;
        }
    }

    return ret;
}

/** @brief Init state handler

    Pre init and init state handler

    @behavior Sync, Non-reentrant

    @pre None

    @param   None

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    if component state is #VDEC_COMP_STATE_INIT_PRE
        Load the firmware images as #VDEC_LoadImg
        if all the images are loaded
            update component state to #VDEC_COMP_STATE_INIT
        if any error
            update pre init fail flag
    if component state is #VDEC_COMP_STATE_INIT
        Read command message
        if the command is #MSYS_COMP_CMD_CONFIG
            if pre init fail flag is set
                update command response to ERR_UNKNOWN
            else
                Configure the library as #VDEC_ConfigLib
                if library state is #VDEC_STATE_IDLE
                    Update component state to #VDEC_COMP_STATE_READY
        Complete message
        if error
            report error

    @endcode
*/
static void VDEC_InitStateHandler(void)
{
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;

    if (VDEC_COMP_STATE_INIT_PRE == VDEC_CompContext.state) {
        while ((BCM_ERR_OK == VDEC_CompContext.preInitError)
                || (BCM_ERR_BUSY == VDEC_CompContext.preInitError)) {
            VDEC_CompContext.preInitError
                = VDEC_LoadImg(VDEC_FwID[VDEC_CompContext.preInitLoadIdx],
                               VDEC_BinaryAddr[VDEC_CompContext.preInitLoadIdx],
                               &VDEC_CompContext.binarySize[VDEC_CompContext.preInitLoadIdx]);
            if (BCM_ERR_OK == VDEC_CompContext.preInitError) {
                VDEC_CompContext.preInitLoadIdx++;
                if (VDEC_CompContext.preInitLoadIdx >= (VDEC_COMP_BINARY_MP_COUNT + VDEC_COMP_BINARY_SP_COUNT)) {
                    VDEC_CompContext.state = VDEC_COMP_STATE_INIT;
                    break;
                }
            } else if (BCM_ERR_BUSY == VDEC_CompContext.preInitError) {
                break;
            } else {
                VDEC_CompContext.preInitError = BCM_ERR_UNKNOWN;
                VDEC_CompContext.state = VDEC_COMP_STATE_INIT;
                break;
            }
        }
    }

    if ((VDEC_COMP_STATE_INIT == VDEC_CompContext.state)
        || (BCM_ERR_UNKNOWN == VDEC_CompContext.preInitError)) {
        ret = MSGQ_GetMsgIdx(&VDEC_CmdQ, &cmdMsgIdx);
        if (BCM_ERR_OK == ret) {
            processCmd = &VDEC_CmdMsgQ[cmdMsgIdx];

            /* validate the command */
            processCmd->response = BCM_ERR_INVAL_PARAMS;
            if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
                if (BCM_ERR_OK == VDEC_CompContext.preInitError) {
                    VDEC_ConfigLib(processCmd);
                    if (VDEC_STATE_IDLE == VDEC_CompContext.context.state) {
                        VDEC_CompContext.state = VDEC_COMP_STATE_READY;
                    }
                } else {
                    processCmd->response = BCM_ERR_UNKNOWN;
                }
            }

            if (BCM_ERR_BUSY != processCmd->response) {
                ret = MSGQ_CompleteMsgIdx(&VDEC_CmdQ, cmdMsgIdx);
                if (BCM_ERR_OK != ret) {
                    VDEC_ReportError(BRCM_SWDSGN_VDEC_INIT_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
                }
            }
        }
    }
}

/** @brief Ready state handler

    Ready state handler

    @behavior Sync, Non-reentrant

    @pre None

    @param   None

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}


    Read command message
    if the command is #MSYS_COMP_CMD_START
        Set all inport and outport events to VDEC task
        for each stream
            update the stream info details
            Update binary details
            Update memory details
            Update Configuration data details

            for each buffer per stream
                VDEC_FillMediaBuffer(port index, buffer index)

        Update component state to #VDEC_COMP_STATE_RUNNING
        Update command response to ERR_OK
    else if command is #MSYS_COMP_CMD_DEINIT
        De initialize the library
        Update component state to #VDEC_COMP_STATE_RESET
        Update command response to ERR_OK
    else if command is #MSYS_COMP_CMD_CONFIG
        Update command response to ERR_OK
    else
        Update command response to ERR_INVAL_PARAMS
    Complete message
    if error
        report error

    @endcode
*/
static void VDEC_ReadyStateHandler(void)
{
    uint32_t i;
    uint32_t j;

    uint32_t cmdMsgIdx=0UL;
    MSYS_CmdType *processCmd = NULL;
    int32_t ret = BCM_ERR_OK;
    int32_t retVal = BCM_ERR_OK;

    ret = MSGQ_GetMsgIdx(&VDEC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VDEC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        if (processCmd->cmd == MSYS_COMP_CMD_START) {
            retVal = BCM_SetEvent(VDEC_Task, VDEC_EVENT_INPORT | VDEC_EVENT_OUTPORT);
            if (BCM_ERR_OK != retVal) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_READY_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
            }

            for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
                VDEC_CompContext.streamInfo[i].streamID = VDEC_STREAMID_0 + i;
                VDEC_CompContext.streamInfo[i].intMemory = VDEC_IntMem[i];
                VDEC_CompContext.streamInfo[i].intMemorySize = VDEC_INT_MEM_DATA_SIZE;

                VDEC_CompContext.streamInfo[i].mpBinary8 = VDEC_BinaryAddr[0UL];
                VDEC_CompContext.streamInfo[i].mpBinary8Size = VDEC_CompContext.binarySize[0UL];
                VDEC_CompContext.streamInfo[i].mpBinaryN = VDEC_BinaryAddr[1UL];
                VDEC_CompContext.streamInfo[i].mpBinaryNSize = VDEC_CompContext.binarySize[1UL];
                VDEC_CompContext.streamInfo[i].mpBinaryCopy = VDEC_BinaryAddr[2UL];
                VDEC_CompContext.streamInfo[i].mpBinaryCopySize = VDEC_CompContext.binarySize[2UL];
                VDEC_CompContext.streamInfo[i].spBinaryCavlc = VDEC_BinaryAddr[3UL];
                VDEC_CompContext.streamInfo[i].spBinaryCavlcSize = VDEC_CompContext.binarySize[3UL];
                VDEC_CompContext.streamInfo[i].spBinaryCabac = VDEC_BinaryAddr[4UL];
                VDEC_CompContext.streamInfo[i].spBinaryCabacSize = VDEC_CompContext.binarySize[4UL];

                VDEC_CompContext.streamInfo[i].configData = VDEC_StreamConfigData[i];
                VDEC_CompContext.streamInfo[i].configDataSize = 0UL;
            }

            /* submit the buffers */
            for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
                for(j = 0UL; j < VDEC_NUM_IN_BUFFERS_PER_PORT; j++) {
                    VDEC_FillMediaBuffer(VDEC_MediaInPortPtr[i], j);
                }
            }

            VDEC_CompContext.state = VDEC_COMP_STATE_RUNNING;
            processCmd->response = BCM_ERR_OK;
        } else if(processCmd->cmd == MSYS_COMP_CMD_DEINIT) {
            ret = VDEC_DeInit(&VDEC_CompContext.context);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_READY_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
            }
            VDEC_CompContext.state = VDEC_COMP_STATE_RESET;
            processCmd->response = BCM_ERR_OK;
        } else if (processCmd->cmd == MSYS_COMP_CMD_CONFIG) {
            processCmd->response = BCM_ERR_OK;
        } else {
            processCmd->response = BCM_ERR_INVAL_PARAMS;
        }
        ret = MSGQ_CompleteMsgIdx(&VDEC_CmdQ, cmdMsgIdx);
        if(BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_READY_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }
}

/** @brief Process library flags

    Process the libraty flags per port

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex Port index
    @param[in]   aFlags Library flags

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    if #aFlags is not 0
        if #VDEC_FLAGS_ERROR_DETECTED is set in #aFlags at #aIndex
            Report error
        if #VDEC_FLAGS_LIBRARY_ERROR is set in #aFlags at #aIndex
            Report error
        if #VDEC_FLAGS_DECODE_COMPLETED is set in #aFlags at #aIndex
            Pop all under processing buffers form procQ
            for each poped buffer
                VDEC_FillMediaBuffer(port index, buffer index)
        if #VDEC_FLAGS_FRAME_DATA_COPY_DONE is set in #aFlags at #aIndex
            Get media buffer from outport
            Update media buffer response to ERR_OK
            Update media buffer properties
            Complete message
            Reset copyFrameDataReq flag for #aIndex
    @endcode

*/
static void VDEC_ProcessFlags(uint32_t aIndex,
                               uint32_t aFlags)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;


    if (0UL != aFlags) {
        if (0UL != (aFlags & (VDEC_FLAGS_ERROR_DETECTED << aIndex))) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_PROCESS_FLAGS_PROC,
                             BCM_ERR_UNKNOWN,
                             __LINE__,
                             aIndex,
                             aFlags,
                             0UL);
        }

        if (0UL != (aFlags & (VDEC_FLAGS_LIBRARY_ERROR << aIndex))) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_PROCESS_FLAGS_PROC,
                             BCM_ERR_UNKNOWN,
                             __LINE__,
                             aIndex,
                             aFlags,
                             0UL);
        }

        if (0UL != (aFlags & (VDEC_FLAGS_DECODE_COMPLETED << aIndex))) {
            uint8_t bufIdx;
            do {
                ret = LWQ_Pop(&VDEC_CompContext.procQ[aIndex] , &bufIdx);
                if (BCM_ERR_OK == ret) {
                    VDEC_FillMediaBuffer(VDEC_MediaInPortPtr[aIndex], bufIdx);
                }
            } while (BCM_ERR_OK == ret);
        }

        if (0UL != (aFlags & (VDEC_FLAGS_FRAME_DATA_COPY_DONE << aIndex))) {
            /* get filled size */
            ret = MSGQ_GetMsgIdx(&VDEC_MediaOutPortPtr[aIndex]->dataQ, &mediaBufferIdx);
            if (BCM_ERR_OK == ret) {
                mediaBuffer = &VDEC_MediaBuffer[aIndex][mediaBufferIdx];
                mediaBuffer->timestamp = VDEC_CompContext.timestamp[aIndex];
                mediaBuffer->response = BCM_ERR_OK;
                mediaBuffer->seqNum = 0UL;
                mediaBuffer->frameStart = 1U;
                mediaBuffer->frameEnd = 1U;

                ret = MSGQ_CompleteMsgIdx(&VDEC_MediaOutPortPtr[aIndex]->dataQ, mediaBufferIdx);
                if(ret != BCM_ERR_OK) {
                    VDEC_ReportError(BRCM_SWDSGN_VDEC_PROCESS_FLAGS_PROC,
                             ret,
                             __LINE__,
                             aIndex,
                             aFlags,
                             0UL);
                }

                VDEC_CompContext.copyFrameDataReq[aIndex] = 0UL;

            } else {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_PROCESS_FLAGS_PROC,
                                 ret,
                                 __LINE__,
                                 aIndex,
                                 aFlags,
                                 0UL);
            }
        }
    }
}

/** @brief Submit fill buffer request

    Submit fill buffer request to library

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex Port index

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    if copyFrameDataReq flag for #aIndex is not set and
       Library copy buffers queue is not empty and
       Library copy state is #VDEC_COPYSTATE_INIT or #VDEC_COPYSTATE_READY
        Try to get a media buffer from out port
        if succesful
            if media buffer address and size are not 0
                Populate #VDEC_YUVBufferType from media buffer fields
                Submit the request to library as #VDEC_CopyFrameData
                if succesful
                    Set copyFrameDataReq flag for #aIndex
                else
                    report error
            else
                media buffer response = ERR_INVAL_PARAMS
                Complete message
                if error
                    report error
    @endcode
*/
static void VDEC_CopyFrameDataReq(uint32_t aIndex)
{
    int32_t ret = BCM_ERR_OK;
    uint32_t mediaBufferIdx;
    MSYS_BufferType *mediaBuffer;

    if ((0UL == VDEC_CompContext.copyFrameDataReq[aIndex])
        && (0UL != LWQ_Length(VDEC_CompContext.context.streamContext[aIndex].copyBufs))
        && ((VDEC_COPYSTATE_INIT == VDEC_CompContext.context.copyState)
             || (VDEC_COPYSTATE_READY == VDEC_CompContext.context.copyState))) {
        /* No buffer under processing */
        ret = MSGQ_GetMsgIdx(&VDEC_MediaOutPortPtr[aIndex]->dataQ, &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            VDEC_YUVBufferType yuvBuffer;
            mediaBuffer = &VDEC_MediaBuffer[aIndex][mediaBufferIdx];
            mediaBuffer->response = BCM_ERR_INVAL_PARAMS;
            /* validate mediabuffer */
            if ((NULL != mediaBuffer->buffer[0UL].data) &&
                (0UL != mediaBuffer->buffer[0UL].size)) {
                yuvBuffer.yAddr = mediaBuffer->buffer[0UL].data;
                yuvBuffer.width = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_WIDTH];
                yuvBuffer.yStride = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                yuvBuffer.height = (uint16_t)mediaBuffer->buffer[0UL].attribs[MSYS_CHANNEL_ATTRIB_HEIGHT];
                yuvBuffer.uAddr = mediaBuffer->buffer[1UL].data;
                yuvBuffer.vAddr = mediaBuffer->buffer[2UL].data;
                yuvBuffer.cStride = (uint16_t)mediaBuffer->buffer[1UL].attribs[MSYS_CHANNEL_ATTRIB_STRIDE];
                yuvBuffer.format = VDEC_YUVFORMAT_420;
                yuvBuffer.bitDepth = VDEC_BITDEPTH_8;
                mediaBuffer->frameNum = 0UL;

                ret = VDEC_CopyFrameData(&VDEC_CompContext.context,
                                      &VDEC_CompContext.streamInfo[aIndex],
                                      VDEC_BUFMODE_FRAME,
                                      &yuvBuffer);
                if (BCM_ERR_OK == ret) {
                    VDEC_CompContext.copyFrameDataReq[aIndex] = 1UL;
                } else {
                    VDEC_ReportError(BRCM_SWDSGN_VDEC_COPY_FRAME_DATA_REQ_PROC,
                                     ret,
                                     __LINE__,
                                     aIndex,
                                     0UL,
                                     0UL);
                }

            } else {
                ret = MSGQ_CompleteMsgIdx(&VDEC_MediaOutPortPtr[aIndex]->dataQ, mediaBufferIdx);
                if(ret != BCM_ERR_OK) {
                    VDEC_ReportError(BRCM_SWDSGN_VDEC_COPY_FRAME_DATA_REQ_PROC,
                                     ret,
                                     __LINE__,
                                     aIndex,
                                     0UL,
                                     0UL);
                }
            }
        }
    }
}

/** @brief Submit decode request

    Submit decode request to library

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex Port index

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    loop
        if fillQ length is not zero
            Peek the buffer
            submit = 0
            if buffer slice is new frame and stream state is #VDEC_STREAMSTATE_READY
                submit = 1
            else buffer slice is new slice and stream state is #VDEC_STREAMSTATE_BUSY
                submit = 1
            else
                exit loop

            if submit is 1
                submit buffer to library
                ret = VDEC_Decode(buffer)
                if ret is ERR_OK
                    pop the buffer from fillQ
                    Push the buffer to ProcQ
                else if ret is not ERR_EAGAIN
                    Report error
                    exit loop
                else
                    exit loop
        else
            exit loop

    @endcode
*/
static void VDEC_TrySubmitDecode(uint32_t aIndex)
{
    int32_t ret;

    while (0UL != LWQ_Length(VDEC_CompContext.fillQ[aIndex])) {
        uint8_t bufIdx = LWQ_PeekFirst(VDEC_CompContext.fillQ[aIndex]);

        if (((0U != VDEC_CompContext.bufInfo[aIndex][bufIdx].sliceInfo.newFrame) &&
             (VDEC_STREAMSTATE_READY == VDEC_CompContext.context.streamContext[aIndex].state)) ||
            ((0U == VDEC_CompContext.bufInfo[aIndex][bufIdx].sliceInfo.newFrame) &&
            (VDEC_STREAMSTATE_BUSY == VDEC_CompContext.context.streamContext[aIndex].state))) {
            ret = VDEC_Decode(&VDEC_CompContext.context,
                              &VDEC_CompContext.streamInfo[aIndex],
                              VDEC_InData[aIndex][bufIdx],
                              VDEC_CompContext.bufInfo[aIndex][bufIdx].size);
            if (BCM_ERR_OK == ret) {
                (void)LWQ_Pop(&VDEC_CompContext.fillQ[aIndex], &bufIdx);
                (void)LWQ_Push(&VDEC_CompContext.procQ[aIndex], bufIdx);
            } else if (BCM_ERR_EAGAIN != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_TRY_SUBMIT_DECODE_PROC,
                                 BCM_ERR_INVAL_STATE,
                                 __LINE__,
                                 VDEC_CompContext.context.streamContext[aIndex].state,
                                 aIndex,
                                 0UL);
                break;
            } else {
                break;
            }
        } else {
            break;
        }
    }
}

/** @brief Process events on ports

    Process events on inport and outport

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]   aIndex Port index
    @param[in]   aFlags Library flags
    @param[in]   aMask  Event mask

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    VDEC_ProcessFlags(aIndex, aFlags)

    VDEC_CopyFrameDataReq(aIndex)

    if #VDEC_InPortEvent #aIndex is set in #aMask
        MSYS_InPortProcess(#VDEC_MediaInPortPtr #aIndex);
        if error
            report error

    VDEC_TrySubmitDecode(aIndex)

    @endcode
*/
static void VDEC_ProcessInPort(uint32_t aIndex,
                               uint32_t aFlags,
                               BCM_EventMaskType aMask)
{
    VDEC_ProcessFlags(aIndex, aFlags);

    VDEC_CopyFrameDataReq(aIndex);

    if (0UL != (aMask & VDEC_InPortEvent[aIndex])) {
        int32_t ret = MSYS_InPortProcess(VDEC_MediaInPortPtr[aIndex]);
        if ((BCM_ERR_OK != ret)
            &&  (BCM_ERR_BUSY != ret)) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_PROCESS_IN_PORT_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
    }

    VDEC_TrySubmitDecode(aIndex);
}

/** @brief Stop command handler

    Stop command handler

    @behavior Sync, Non-reentrant

    @pre None

    @param  None

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    for each stream
        Cancel submitted buffers
        if error
            report error

    Reset stop in progress flag

    for each stream
        Complete pending out port buffers

    Complete stop command
    if error
        report error

    @endcode
*/
static void VDEC_StopCmdHandler(void)
{
    uint32_t i;
    int32_t ret;
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;

    for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
        ret = MSYS_InPortCancelAll(VDEC_MediaInPortPtr[i]);
        if ((BCM_ERR_OK != ret) && (BCM_ERR_CANCELLED != ret)) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_STOP_CMD_HANDLER_PROC,
                             ret,
                             __LINE__,
                             i,
                             0UL,
                             0UL);
        }
    }

    /* This should be cleared after cancelling all the media buffers */
    VDEC_CompContext.stopInProgress = 0UL;
    for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
        uint32_t mediaBufferIdx;
        MSYS_BufferType *mediaBuffer;
        ret = MSGQ_GetMsgIdx(&VDEC_MediaOutPortPtr[i]->dataQ,
                             &mediaBufferIdx);
        if (BCM_ERR_OK == ret) {
            mediaBuffer = &VDEC_MediaBuffer[VDEC_MediaOutPortPtr[i]->portIndex][mediaBufferIdx];
            mediaBuffer->buffer[0].size = 0UL;
            mediaBuffer->response = BCM_ERR_OK;
            ret = MSGQ_CompleteMsgIdx(&VDEC_MediaOutPortPtr[i]->dataQ,
                                      mediaBufferIdx);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_STOP_CMD_HANDLER_PROC,
                                 ret,
                                 __LINE__,
                                 i,
                                 0UL,
                                 0UL);
            }
        }
    }

    ret = MSGQ_GetMsgIdx(&VDEC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VDEC_CmdMsgQ[cmdMsgIdx];
        processCmd->response = BCM_ERR_OK;
        ret = MSGQ_CompleteMsgIdx(&VDEC_CmdQ, cmdMsgIdx);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_STOP_CMD_HANDLER_PROC,
                             ret,
                             __LINE__,
                             0UL,
                             0UL,
                             0UL);
        }
    }

}

/** @brief Run state handler

    Run state handler

    @behavior Sync, Non-reentrant

    @pre None

    @param[in]  aMask  Event mask

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    Try to read command
    if succesful
        if command is #MSYS_COMP_CMD_STOP
            set stop in progress flag
        else
            update command response to ERR_INVAL_PARAMS
            Complete message
            if error
                report error

    if #PP_GPEvent is set in #aMask
        flags = VDEC_Process(library context)
    else
        flags = 0

    for each port
        VDEC_ProcessInPort(port index, flags, aMask)

    if stop in progress and library state is VDEC_STATE_IDLE
        VDEC_StopCmdHandler()
        Update component state to #VDEC_COMP_STATE_READY

    @endcode
*/
static void VDEC_RunStateHandler(BCM_EventMaskType aMask)
{
    uint32_t i;
    int32_t ret = BCM_ERR_OK;
    VDEC_FlagsType flags = 0UL;
    uint32_t cmdMsgIdx = 0UL;
    MSYS_CmdType *processCmd = NULL;

    ret = MSGQ_GetMsgIdx(&VDEC_CmdQ, &cmdMsgIdx);
    if (BCM_ERR_OK == ret) {
        processCmd = &VDEC_CmdMsgQ[cmdMsgIdx];
        /* validate the command */
        processCmd->response = BCM_ERR_INVAL_PARAMS;
        if(MSYS_COMP_CMD_STOP == processCmd->cmd) {
            VDEC_CompContext.stopInProgress = 1UL;
        } else {
            ret = MSGQ_CompleteMsgIdx(&VDEC_CmdQ, cmdMsgIdx);
            if (BCM_ERR_OK != ret) {
                VDEC_ReportError(BRCM_SWDSGN_VDEC_RUN_STATE_HANDLER_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
            }
        }
    }

    if (0UL != (aMask & PP_GPEvent)) {
        flags = VDEC_Process(&VDEC_CompContext.context);
    }

    for (i = 0UL; i < VDEC_COMP_NUM_STREAMS; i++) {
        VDEC_ProcessInPort(i, flags, aMask);
    }

    if ((1UL == VDEC_CompContext.stopInProgress) &&
        (VDEC_STATE_IDLE == VDEC_CompContext.context.state)) {
        VDEC_StopCmdHandler();
        VDEC_CompContext.state = VDEC_COMP_STATE_READY;
    }
}

/** @brief Component task

    Component task

    @behavior Sync, Non-reentrant

    @pre None

    @param None

    @return None

    @post None

    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @limitations None

    @code{.unparsed}

    loop
        Wait(VDEC_EVENT_CMDQ_SERVER or
             VDEC_EVENT_IMGL or
             PP_GPEvent or
             VDEC_EVENT_INPORT or
             VDEC_EVENT_OUTPORT)

        mask = getEvent()

        if component state is #VDEC_COMP_STATE_RESET
            VDEC_ResetStateHandler()
        else if component state is #VDEC_COMP_STATE_INIT_PRE or #VDEC_COMP_STATE_INIT
            VDEC_InitStateHandler()
        else if component state is #VDEC_COMP_STATE_READY
            VDEC_ReadyStateHandler()
        else if component state is #VDEC_COMP_STATE_RUNNING
            VDEC_RunStateHandler(mask)
        else
            report error

        if component state is #VDEC_COMP_STATE_RESET
            break loop

    Terminate task
    @endcode
*/
TASK(VDEC_Task)
{
    int32_t ret;
    BCM_EventMaskType mask = 0UL;

    do {
        ret = BCM_WaitEvent(VDEC_EVENT_CMDQ_SERVER
                            | VDEC_EVENT_IMGL
                            | PP_GPEvent
                            | VDEC_EVENT_INPORT
                            | VDEC_EVENT_OUTPORT);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_GetEvent(VDEC_Task,&mask);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }
        ret = BCM_ClearEvent(mask);
        if (BCM_ERR_OK != ret) {
            VDEC_ReportError(BRCM_SWDSGN_VDEC_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
        }

        switch(VDEC_CompContext.state) {
        case VDEC_COMP_STATE_RESET:
            VDEC_ResetStateHandler();
            break;
        case VDEC_COMP_STATE_INIT_PRE:
        case VDEC_COMP_STATE_INIT:
            VDEC_InitStateHandler();
            break;
        case VDEC_COMP_STATE_READY:
            VDEC_ReadyStateHandler();
            break;
        case VDEC_COMP_STATE_RUNNING:
            VDEC_RunStateHandler(mask);
            break;
        default:
            VDEC_ReportError(BRCM_SWDSGN_VDEC_TASK_PROC, BCM_ERR_UNKNOWN, __LINE__, 0UL, 0UL, 0UL);
            break;
        }
    } while(VDEC_COMP_STATE_RESET != VDEC_CompContext.state);

    ret = BCM_TerminateTask();
    VDEC_ReportError(BRCM_SWDSGN_VDEC_TASK_PROC, ret, __LINE__, 0UL, 0UL, 0UL);
}
/** @} */

/**
    @addtogroup grp_vdec_comp
    @{
*/

/**
    @trace #BRCM_SWARCH_VDEC_GETFNTABLE_PROC
    @trace #BRCM_SWREQ_VDEC_MEDIA_COMPONENT

    @code{.c}
    return VDEC_CompFnTable
    @endcode
*/
const MSYS_CompFnTblType* VDEC_GetCompFnTable(void)
{
    return (&VDEC_CompFnTable);
}
/** @} */

/**
    @addtogroup grp_vdec_lib_ifc
    @{
*/

/**
    @trace #BRCM_SWARCH_VDEC_GET_REF_BUF_PROC
    @trace #BRCM_SWREQ_VDEC_MEMORY_MANAGEMENT

    @code{.c}
    if aBufIdx is valid
        return #VDEC_RefMem #aBufIdx
    else
        return 0
    @endcode
*/
uint32_t VDEC_GetRefBuf(uint8_t aBufIdx)
{
    uint32_t addr = 0UL;
    if ((uint8_t)VDEC_NUM_REF_BUFFERS > aBufIdx) {
        addr = (uint32_t)VDEC_RefMem[aBufIdx];
    }
    return addr;
}
/** @} */
