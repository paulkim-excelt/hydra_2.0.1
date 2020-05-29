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
    @defgroup grp_svc_handler_impl SVC Handler Implementation
    @ingroup grp_svc

    @addtogroup grp_svc_handler_impl
    @{

    @file svc_handlers.c

    @version 0.1 Initial version
*/

#include <stdlib.h>
#include <system.h>
#include <bcm_time.h>
#include <svc.h>
#include <osil/mcu_osil.h>

#include <uart_osil.h>

#if defined(ENABLE_FLASH)
#include <osil/flash_osil.h>
#endif  /* ENABLE_FLASH */

#if defined(ENABLE_GPIO_GIO_V2)
#include <osil/gpio_osil.h>
#include <osil/pinmux_osil.h>
#endif  /* ENABLE_GPIO_GIO_V2 */

#if defined(ENABLE_AVT)
#include <osil/avt_osil.h>
#endif  /* ENABLE_AVT */

#if defined(ENABLE_UART_CONSOLE)
#include <osil/uconsole_osil.h>
#endif  /* ENABLE_UART_CONSOLE */

#if defined(ENABLE_ETH)
#include <osil/eth_osil.h>
#include <osil/ethxcvr_osil.h>
#endif  /* ENABLE_ETH */

#if defined(ENABLE_MSG_QUEUE)
#include <osil/msg_queue_osil.h>
#endif  /* ENABLE_MSG_QUEUE */

#if defined(ENABLE_PP)
#include <osil/pp_osil.h>
#endif  /* ENABLE_PP */

#if defined(ENABLE_CAN_MCAN)
#include <osil/can_osil.h>
#endif  /* ENABLE_CAN_MCAN */

#if defined(ENABLE_ULOG)
#include <osil/log_osil.h>
#endif  /* ENABLE_ULOG */

#if defined(ENABLE_ISYS)
#include <osil/isp_osil.h>
#include <osil/unicam_osil.h>
#endif  /* ENABLE_ISYS */

#if defined(ENABLE_WATCHDOG_SP805)
#include <osil/wdt_osil.h>
#endif  /* ENABLE_WATCHDOG_SP805 */

#if defined(ENABLE_VTMON)
#include <osil/vtmon_osil.h>
#endif  /* ENABLE_VTMON */

#if defined(ENABLE_CCT)
#include <osil/ccu_osil.h>
#endif  /* ENABLE_CCT */

#if defined(ENABLE_PWM)
#include <osil/pwm_osil.h>
#endif  /* ENABLE_PWM */

#if defined(ENABLE_DMA330)
#include <osil/dma_osil.h>
#endif  /* ENABLE_DMA330 */

#if defined(ENABLE_IIC_BSC)
#include <osil/i2c_osil.h>
#endif  /* ENABLE_IIC_BSC */

#if defined(ENABLE_I2S)
#include <osil/audio_osil.h>
#endif  /* ENABLE_I2S */

#if defined(ENABLE_TIMER_SP804)
#include <osil/gpt_osil.h>
#endif  /* ENABLE_TIMER_SP804 */

#if defined(ENABLE_ADC)
#include <osil/adc_osil.h>
#endif  /* ENABLE_ADC */

#if defined(ENABLE_SPI_PL022)
#include <osil/spi_osil.h>
#endif  /* ENABLE_SPI_PL022 */

#if defined(ENABLE_LIN)
#include <osil/lin_osil.h>
#endif /* ENABLE_LIN */

#if defined(ENABLE_SD)
#include <osil/sd_osil.h>
#endif

#if defined(ENABLE_VGC)
#include <osil/vgc_osil.h>
#endif

#if defined(ENABLE_OTP)
#include <osil/otp_osil.h>
#endif /* ENABLE_OTP */

#ifdef ENABLE_DBGMEM
#include <osil/dbgmem_osil.h>
#endif

#include <osil/bcm_osil_svc.h>
#include <osil/cache_osil.h>

#include "ee_internal.h"

/**
    @name BCM8910X SVC Handelr Design IDs
    @{
    @brief Design IDs for BCM8910X SVC Handler
*/
#define BRCM_SWDSGN_SVC_DEFAULT_HANDLER_PROC  (0x80U)   /**< @brief #SVC_DefaultHandler  */
#define BRCM_SWDSGN_SVC_HANDLER_TABLE_GLOBAL  (0x81U)   /**< @brief #SVC_HandlerFuncTbl  */
#define BRCM_SWDSGN_SVC_HANDLER_PROC          (0x82U)   /**< @brief #SVC_CmdHandler      */
/** @} */


/**
    @trace #BRCM_SWARCH_SVC_HANDLER_TYPE
    @trace #BRCM_SWARCH_SVC_HANDLER_PROC
    @trace #BRCM_SWREQ_SVC_REQUEST_BCM8910X_HANDLER
*/
static void SVC_DefaultHandler(uint32_t aMagicID, uint32_t aCmd, uint8_t * aSysIO)
{
    (void)aMagicID;
    (void)aCmd;
    (void)aSysIO;
}

/**
    @trace #BRCM_SWARCH_SVC_HANDLER_TYPE
    @trace #BRCM_SWARCH_SVC_HANDLER_PROC
    @trace #BRCM_SWREQ_SVC_REQUEST_BCM8910X_HANDLER
*/
static const SVC_ReqHandlerType SVC_HandlerFuncTbl[(SVC_CMD_END - SVC_CMD_START) + 1UL] = {
    MCU_SysCmdHandler,      /* 0x80 */
#if defined(ENABLE_TIMER_SP804)
    TIM_SysCmdHandler,          /* 0x81 */
#else
    SVC_DefaultHandler,          /* 0x81 */
#endif
#if defined(ENABLE_CCT)
    CCU_SysCmdHandler,          /* 0x82 */
#else
    SVC_DefaultHandler,          /* 0x82 */
#endif
    UART_SysCmdHandler,          /* 0x83 */
#if defined(ENABLE_IIC_BSC)
    IIC_SysCmdHandler,          /* 0x84 */
#else
    SVC_DefaultHandler,          /* 0x84 */
#endif
#if defined(ENABLE_SPI_PL022)
    SPI_SysCmdHandler,          /* 0x85 */
#else
    SVC_DefaultHandler,          /* 0x85 */
#endif
#if defined(ENABLE_FLASH)
    FLASH_SysCmdHandler,        /* 0x86 */
#else
    SVC_DefaultHandler,          /* 0x86 */
#endif
#if defined(ENABLE_WATCHDOG_SP805)
    WDT_SysCmdHandler,          /* 0x87 */
#else
    SVC_DefaultHandler,          /* 0x87 */
#endif
#if defined(ENABLE_GPIO_GIO_V2)
    GPIO_SysCmdHandler,         /* 0x88 */
    PINMUX_SysCmdHandler,       /* 0x89 */
#else
    SVC_DefaultHandler,          /* 0x88 */
    SVC_DefaultHandler,          /* 0x89 */
#endif
#if defined(ENABLE_SD)
    SD_SysCmdHandler,            /* 0x8A */
#else
    SVC_DefaultHandler,          /* 0x8A */
#endif
#if defined(ENABLE_PWM)
    PWM_SysCmdHandler,          /* 0x8B */
#else
    SVC_DefaultHandler,          /* 0x8B */
#endif
#if defined(ENABLE_ADC)
    ADC_SysCmdHandler,          /* 0x8C */
#else
    SVC_DefaultHandler,          /* 0x8C */
#endif
#if defined(ENABLE_DMA330)
    DMA_SysCmdHandler,
#else
    SVC_DefaultHandler,          /* 0x8D */
#endif
#if defined(ENABLE_OTP)
    OTP_SysCmdHandler,          /* 0x8E */
#else
    SVC_DefaultHandler,          /* 0x8E */
#endif
#if defined(ENABLE_VTMON)
    VTMON_SysCmdHandler,        /* 0x8F */
#else
    SVC_DefaultHandler,          /* 0x8F */
#endif
    SVC_DefaultHandler,          /* 0x90 */
    SVC_DefaultHandler,          /* 0x91 */
    SVC_DefaultHandler,          /* 0x92 */
    SVC_DefaultHandler,          /* 0x93 */
    SVC_DefaultHandler,          /* 0x94 */
    SVC_DefaultHandler,          /* 0x95 */
    SVC_DefaultHandler,          /* 0x96 */
    SVC_DefaultHandler,          /* 0x97 */
    SVC_DefaultHandler,          /* 0x98 */
    SVC_DefaultHandler,          /* 0x99 */
    SVC_DefaultHandler,          /* 0x9A */
    SVC_DefaultHandler,          /* 0x9B */
    SVC_DefaultHandler,          /* 0x9C */
    SVC_DefaultHandler,          /* 0x9D */
    SVC_DefaultHandler,          /* 0x9E */
    SVC_DefaultHandler,          /* 0x9F */
#if defined(ENABLE_PP)
    PP_DrvCmdHandler,            /* 0xA0 */
#else
    SVC_DefaultHandler,          /* 0xA0 */
#endif  /* ENABLE_PP */
#if defined(ENABLE_I2S)
    Audio_SysCmdHandler,           /* 0xA1 */
#else
    SVC_DefaultHandler,          /* 0xA1 */
#endif
    SVC_DefaultHandler,          /* 0xA2 */
#if defined(ENABLE_ISYS)
    UNICAM_SysCmdHandler,       /* 0xA3 */
    ISP_SysCmdHandler,          /* 0xA4 */
#else
    SVC_DefaultHandler,          /* 0xA3 */
    SVC_DefaultHandler,          /* 0xA4 */
#endif  /* ENABLE_ISYS */
    SVC_DefaultHandler,          /* 0xA5 */
    SVC_DefaultHandler,          /* 0xA6 */
#if defined(ENABLE_VGC)
    VGC_SysCmdHandler,           /* 0xA7 */
#else
    SVC_DefaultHandler,          /* 0xA7 */
#endif  /* ENABLE_VGC */
    SVC_DefaultHandler,          /* 0xA8 */
#if defined (ENABLE_AVT)
    AVT_SysCmdHandler,          /* 0xA9 */
#else
    SVC_DefaultHandler,          /* 0xA9 */
#endif  /* ENABLE_AVT */
    SVC_DefaultHandler,          /* 0xAA */
    SVC_DefaultHandler,          /* 0xAB */
    SVC_DefaultHandler,          /* 0xAC */
    SVC_DefaultHandler,          /* 0xAD */
    SVC_DefaultHandler,          /* 0xAE */
    SVC_DefaultHandler,          /* 0xAF */
#if defined(ENABLE_ETH)
    ETHER_SysCmdHandler,         /* 0xB0 */
#else
    SVC_DefaultHandler,          /* 0xB0 */
#endif
#if defined(ENABLE_CAN_MCAN)
    CAN_SysCmdHandler,           /* 0xB1 */
#else
    SVC_DefaultHandler,          /* 0xB1 */
#endif
#if defined(ENABLE_LIN)
    LIN_SysCmdHandler,          /* 0xB2 */
#else
    SVC_DefaultHandler,          /* 0xB2 */
#endif
    SVC_DefaultHandler,          /* 0xB3 */
    SVC_DefaultHandler,          /* 0xB4 */
#if defined(ENABLE_ETH)
    ETHXCVR_SysCmdHandler,       /* 0xB5 */
#else
    SVC_DefaultHandler,          /* 0xB5 */
#endif
#if defined(ENABLE_ETH)
    ETHER_TimeSysCmdHandler,     /* 0xB6 */
#else
    SVC_DefaultHandler,          /* 0xB6 */
#endif
    SVC_DefaultHandler,          /* 0xB7 */
    SVC_DefaultHandler,          /* 0xB8 */
    SVC_DefaultHandler,          /* 0xB9 */
    SVC_DefaultHandler,          /* 0xBA */
    SVC_DefaultHandler,          /* 0xBB */
    SVC_DefaultHandler,          /* 0xBC */
    SVC_DefaultHandler,          /* 0xBD */
    SVC_DefaultHandler,          /* 0xBE */
    SVC_DefaultHandler,          /* 0xBF */
#if defined(ENABLE_MSG_QUEUE)
    MSGQ_CmdHandler,            /* 0xC0 */
#else
    SVC_DefaultHandler,          /* 0xC0 */
#endif  /* ENABLE_MSG_QUEUE */
#if defined(ENABLE_UART_CONSOLE)
    UCONSOLE_SysCmdHandler,      /* 0xC1 */
#else
    SVC_DefaultHandler,          /* 0xC1 */
#endif /*ENABLE_UART_CONSOLE */
    SVC_DefaultHandler,          /* 0xC2 */
#ifdef ENABLE_DBGMEM
    DBGMEM_SysCmdHandler,        /* 0xC3 */
#else
    SVC_DefaultHandler,          /* 0xC3 */
#endif
    BCM_OsSysCmdHandler,         /* 0xC4 */
    DCACHE_SysCmdHandler,        /* 0xC5 */
    SVC_DefaultHandler,          /* 0xC6 */
    SVC_DefaultHandler,          /* 0xC7 */
    SVC_DefaultHandler,          /* 0xC8 */
    SVC_DefaultHandler,          /* 0xC9 */
    SVC_DefaultHandler,          /* 0xCA */
    SVC_DefaultHandler,          /* 0xCB */
    SVC_DefaultHandler,          /* 0xCC */
    SVC_DefaultHandler,          /* 0xCD */
    SVC_DefaultHandler,          /* 0xCE */
    SVC_DefaultHandler,          /* 0xCF */
};

/**
    Handle the command requested by unprivileged software.
    @trace #BRCM_SWARCH_SVC_HANDLER_PROC
    @trace #BRCM_SWREQ_SVC_REQUEST_BCM8910X_HANDLER
*/
void SVC_CmdHandler(SVC_RequestType *aSysReqIO, uint32_t aSysReqID)
{
    if (NULL == aSysReqIO) {
        /* TODO: Force crash */
    } else if ((SVC_CMD_START > aSysReqID)
               || (SVC_CMD_END < aSysReqID)
               || (aSysReqID != aSysReqIO->sysReqID)){
        aSysReqIO->response = BCM_ERR_INVAL_PARAMS;
    } else {
        SVC_HandlerFuncTbl[aSysReqIO->sysReqID - SVC_CMD_START](aSysReqIO->magicID,
                                                                aSysReqIO->cmd,
                                                                aSysReqIO->svcIO);
        aSysReqIO->response = BCM_ERR_OK;
    }
}

/** @} */
