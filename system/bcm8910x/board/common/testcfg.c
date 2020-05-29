/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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
#include <stdlib.h>
#include <string.h>
#include "ee.h"
#include <ulog.h>
#include <bcm_test.h>
#include <bcm_err.h>
#include <gpio.h>
#include <pinmux.h>
#include <testcfg.h>

#if defined(ENABLE_GPIO_TEST) || defined(ENABLE_PINMUX_TEST)
static const PINMUX_PinModeCfgType UARTPinModeCfg[] = {
    {PINMUX_PIN_MODE_UART0, 0UL},
};

static const PINMUX_PinModeCfgType GPIO_PinModeCfg[] = {
    {PINMUX_PIN_MODE_GPIO, 0UL}
};
#endif

#if defined(ENABLE_GPIO_TEST)
static const PINMUX_PinCfgType GIO0PinMuxPins[] = {
    /* UART0 Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART0 Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* Pin for testing */
    {PINMUX_PIN_DIRECTION_OUT, PINMUX_PIN_DIRECTION_CHANGEABLE, GPIO_TEST_CHANNEL0, PINMUX_PIN_MODE_GPIO, PINMUX_PIN_LEVEL_LOW, 1UL, &GPIO_PinModeCfg[0], PINMUX_PIN_MODE_CHANGEABLE,},
    /* Pin for testing */
    {PINMUX_PIN_DIRECTION_OUT, PINMUX_PIN_DIRECTION_CHANGEABLE, GPIO_TEST_CHANNEL1, PINMUX_PIN_MODE_GPIO, PINMUX_PIN_LEVEL_LOW, 1UL, &GPIO_PinModeCfg[0], PINMUX_PIN_MODE_CHANGEABLE},
};

const GPIO_ChannelGroupType GPIO_TestChannGrp = {
    ((1UL << GPIO_TEST_CHANNEL0) | (1UL << GPIO_TEST_CHANNEL1)),
    (uint8_t)GPIO_TEST_CHANNEL0,
    GPIO_TEST_PORT0,
};
#endif

#if defined(ENABLE_PINMUX_TEST)
static const PINMUX_PinModeCfgType TestPinModeCfg[] = {
    {PINMUX_PIN_MODE_PWM, 0UL},
    {PINMUX_PIN_MODE_GPIO,0UL},
};

static const PINMUX_PinCfgType GIO0PinMuxPins[] = {
    /* UART0 Tx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_6, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* UART0 Rx Pin */
    {0U, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, GPIO_CHANNEL_7, PINMUX_PIN_MODE_UART0, 0U, 1UL, &UARTPinModeCfg[0], PINMUX_PIN_MODE_NOT_CHANGEABLE},
    /* Pin 20 for Testing */
    {PINMUX_PIN_DIRECTION_IN, PINMUX_PIN_DIRECTION_CHANGEABLE, PINMUX_TESTPIN0, PINMUX_PIN_MODE_PWM, PINMUX_PIN_LEVEL_LOW, 0x00000002UL, &TestPinModeCfg[0], PINMUX_PIN_MODE_CHANGEABLE},
    /* Pin 21 for Testing */
    {PINMUX_PIN_DIRECTION_IN, PINMUX_PIN_DIRECTION_NOT_CHANGEABLE, PINMUX_TESTPIN1, PINMUX_PIN_MODE_PWM, PINMUX_PIN_LEVEL_LOW, 0x00000002UL, &TestPinModeCfg[0], PINMUX_PIN_MODE_CHANGEABLE}
};
#endif /* ENABLE_PINMUX_TEST */

#if defined(ENABLE_GPIO_TEST) || defined(ENABLE_PINMUX_TEST)
const PINMUX_ConfigType PINMUX_Config[] = {
    {
        sizeof(GIO0PinMuxPins) / sizeof(PINMUX_PinCfgType),
        &GIO0PinMuxPins[0],
    },
};
#endif
