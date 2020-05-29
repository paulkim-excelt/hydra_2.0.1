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

#include "ee.h"
#include <string.h>
#include <mcu.h>
#include <clk.h>
#include <ulog.h>
#include <bcm_err.h>
#include <bcm_test.h>
#include <bcm_utils.h>
#include <audio.h>
#include <audio_osil.h>
#include <osil/bcm_osil.h>

#define GetModuleLogLevel() ULOG_LVL_INFO

/* Test IDs */
#define AUDIO_CTRL_IT1     (1UL)

#define AUDIO_PORTS_MAX	                (3UL)
#define AUDIO_TEST_PLAY_START_THRES_MUL (1UL)
#define AUDIO_TEST_PLAY_PATTERN_SIZE    (40UL)
#define AUDIO_TEST_PLAY_BUF_SIZE        (4416UL)
#define AUDIO_TEST_PLAY_BUF_COUNT       (200UL)

#define AUDIO_TEST_REC_START_THRES_MUL (2UL)
#define AUDIO_TEST_REC_PATTERN_SIZE    (40UL)
#define AUDIO_TEST_REC_BUF_SIZE        (4416UL)
#define AUDIO_TEST_REC_BUF_COUNT       (100UL)

#define AUDIO_TEST_RESULT_FAIL          (0UL)
#define AUDIO_TEST_RESULT_PASS          (1UL)
#define AUDIO_TEST_RESULT_PEND          (2UL)
#define AUDIO_TEST_RESULT_INVALID       (3UL)

extern int32_t Audio_EnableLoopback(Audio_HwIDType aID);

static uint32_t AudioTest_Result = AUDIO_TEST_RESULT_INVALID;
static int32_t AudioIT1_Status = BCM_AT_NOT_STARTED;

/** Event mask for audio driver play channel - play completion */
static const uint32_t AudioTest_PlayEvent[AUDIO_PORTS_MAX] =
{
    APLY_0_COMP_EVENT,
    APLY_1_COMP_EVENT,
    APLY_2_COMP_EVENT
};

#define AUDIO_TEST_PLAY_EVENTS	( AudioTest_PlayEvent[0]   \
                                | AudioTest_PlayEvent[1]   \
                                | AudioTest_PlayEvent[2])

#define AUDIO_TEST_PLAY_COMP_EVENT	(SystemEvent0)

/** Event mask for audio driver record channel - record completion */
static const uint32_t AudioTest_RecEvent[AUDIO_PORTS_MAX] =
{
    AREC_0_COMP_EVENT,
    AREC_1_COMP_EVENT,
    AREC_2_COMP_EVENT
};

#define AUDIO_TEST_REC_EVENTS 	( AudioTest_RecEvent[0]   \
                                | AudioTest_RecEvent[1]   \
                                | AudioTest_RecEvent[2])

#define AUDIO_TEST_REC_COMP_EVENT	(SystemEvent1)

static uint32_t AudioTest_PlayPortMask = 0UL
#if defined(ENABLE_AUDIO_PLAY_PORT0)
        | 0x1UL
#endif
#if defined(ENABLE_AUDIO_PLAY_PORT1)
        | 0x2UL
#endif
#if defined(ENABLE_AUDIO_PLAY_PORT2)
        | 0x4UL
#endif
        | 0x0UL; /* AudioTest_PlayPortMask */

static uint32_t AudioTest_RecPortMask = 0UL
#if defined(ENABLE_AUDIO_REC_PORT0)
        | 0x1UL
#endif
#if defined(ENABLE_AUDIO_REC_PORT1)
        | 0x2UL
#endif
#if defined(ENABLE_AUDIO_REC_PORT2)
        | 0x4UL
#endif
        | 0x0UL; /* RecPortMask */

#define AUDIO_TEST_PLAY_PATTERN_1       \
{   \
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,\
    0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A,\
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A,\
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A,\
}

#define AUDIO_TEST_PLAY_PATTERN_2       \
{   \
    0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA,\
    0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,\
    0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,\
    0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA,\
}

static uint8_t AudioTest_PlayPattern[2UL][AUDIO_TEST_PLAY_PATTERN_SIZE] =
{
    AUDIO_TEST_PLAY_PATTERN_1,
    AUDIO_TEST_PLAY_PATTERN_2
};

static uint8_t AudioTest_PlayBuf[AUDIO_TEST_PLAY_BUF_SIZE];

static const Audio_CfgType AudioTest_Config[] = {
#if defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC)
    {
        .hwID = 0UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S0_PLAY)
        .playBuf = &Audio_Play0Buf[0UL][0UL],
        .playXferReq = Audio_Play0DMAXfer,
        .playBufSize = AUDIO_PLAY0_BUF_SIZE,
        .playBufNum = AUDIO_PLAY0_BUF_MAX_NUM,
#endif
#if defined(ENABLE_I2S0_REC)
        .recBuf = &Audio_Rec0Buf[0UL][0UL],
        .recBufSize = AUDIO_REC0_BUF_SIZE,
        .recBufNum = AUDIO_REC0_BUF_MAX_NUM,
        .recXferReq = Audio_Rec0DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S0_PLAY) || defined(ENABLE_I2S0_REC) */
#if defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC)
    {
        .hwID = 1UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S1_PLAY)
        .playBuf = &Audio_Play1Buf[0UL][0UL],
        .playBufSize = AUDIO_PLAY1_BUF_SIZE,
        .playBufNum = AUDIO_PLAY1_BUF_MAX_NUM,
        .playXferReq = Audio_Play1DMAXfer,
#endif
#if defined(ENABLE_I2S1_REC)
        .recBuf = &Audio_Rec1Buf[0UL][0UL],
        .recBufSize = AUDIO_REC1_BUF_SIZE,
        .recBufNum = AUDIO_REC1_BUF_MAX_NUM,
        .recXferReq = Audio_Rec1DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S1_PLAY) || defined(ENABLE_I2S1_REC) */
#if defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC)
    {
        .hwID = 2UL,
        .masterClk = 24576000UL,
        .masterClkSrc = AUDIO_MCLK_SRC_NCO_0,
        .freq = 48000UL,
        .infMode = AUDIO_INF_MODE_BCLK_WS_MASTER,
        .numSlot = 2UL,
        .slotWidth = 32UL,
        .wordSelWidth = 32UL,
        .txDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .txSlotMask = 0x3UL,
        .txDataJust = AUDIO_INF_DATA_JUST_LEFT,
        .rxDataDly = AUDIO_INF_DATA_DLY_TRUE,
        .rxSlotMask = 0x3UL,
        .rxDataJust = AUDIO_INF_DATA_JUST_LEFT,
#if defined(ENABLE_I2S2_PLAY)
        .playBuf = &Audio_Play2Buf[0UL][0UL],
        .playBufSize = AUDIO_PLAY2_BUF_SIZE,
        .playBufNum = AUDIO_PLAY2_BUF_MAX_NUM,
        .playXferReq = Audio_Play2DMAXfer,
#endif
#if defined(ENABLE_I2S2_REC)
        .recBuf = &Audio_Rec2Buf[0UL][0UL],
        .recBufSize = AUDIO_REC2_BUF_SIZE,
        .recBufNum = AUDIO_REC2_BUF_MAX_NUM,
        .recXferReq = Audio_Rec2DMAXfer,
#endif
    },
#endif  /* defined(ENABLE_I2S2_PLAY) || defined(ENABLE_I2S2_REC) */
};

static int32_t AudioTest_PlayCfgStart(Audio_HwIDType aID)
{
    int32_t retVal;
    Audio_InfoType info;
    Audio_PlayCfgType cfg;

    retVal = Audio_GetPlayInfo(aID, &info);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_GetPlayInfo failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    cfg.sampleWidth = 32UL;
    cfg.samplePack = AUDIO_PACK_4B_MSB_ALIGN_LE;
    cfg.numChan = 2UL;
    cfg.startThreshold = (AUDIO_TEST_PLAY_START_THRES_MUL  * info.bufSize);

    retVal = Audio_PlayCfg(aID, &cfg);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_PlayCfg failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    retVal = Audio_PlayStart(aID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_PlayStart failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

err:
    return retVal;
}

static int32_t AudioTest_Play(Audio_HwIDType aID)
{
    int32_t retVal;
    uint8_t *buf;
    uint32_t bufLen = AUDIO_TEST_PLAY_BUF_SIZE;

    retVal = Audio_DeQueuePlayBuf(aID, bufLen, &buf);
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
        ULOG_ERR("%s: Audio_DeQueuePlayBuf failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    if (BCM_ERR_OK == retVal) {
        BCM_MemCpy(buf, AudioTest_PlayBuf, bufLen);

        retVal = Audio_QueuePlayBuf(aID, bufLen, buf);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: Audio_QueuePlayBuf failed (0x%x)\n",
                    __func__, retVal);
            goto err;
        }
    }

err:
    return retVal;
}

TASK(AudioTest_PlayTask)
{
    int32_t retVal;
    uint32_t i, j = 0UL;
    BCM_EventMaskType eventMask = AUDIO_TEST_PLAY_EVENTS;
    Audio_PlayStatsType stats;
    Audio_PlayStateType state;

    for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
        if ((AudioTest_PlayPortMask & (0x1UL << i)) != 0UL) {
            retVal = AudioTest_PlayCfgStart(i);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_PlayCfgStart failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            }
        }
        retVal = Audio_GetPlayState(i, &state);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: Audio_GetPlayState failed (0x%x)\n",
                    __func__, retVal);
            AudioIT1_Status = retVal;
            break;
        } else {
            ULOG_INFO("%s: Audio_GetPlayState (0x%x)\n",
                    __func__, state);
        }
    }

	do {
		for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
            if (((AudioTest_PlayPortMask & (0x1UL << i)) != 0UL)
                    && ((eventMask & AudioTest_PlayEvent[i]) != 0UL)) {
                /* Dequeue empty buffer and do memcpy and queue */
                retVal = AudioTest_Play(i);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
                    ULOG_ERR("%s: Audio_PlayBuf failed (0x%x)\n",
                            __func__, retVal);
                    AudioIT1_Status = retVal;
                    break;
                }
            }
        }

        if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            break;
        }

        if (BCM_ERR_OK == retVal) {
            j++;
        }
        /* Wait for event from driver */
        if (BCM_ERR_EAGAIN == retVal) {
            BCM_WaitEvent(AUDIO_TEST_PLAY_EVENTS);
            BCM_GetEvent(AudioTest_PlayTask, &eventMask);
            BCM_ClearEvent(eventMask);
        }
	} while (j < AUDIO_TEST_PLAY_BUF_COUNT);

    for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
        if ((AudioTest_PlayPortMask & (0x1UL << i)) != 0UL) {
            retVal = Audio_PlayStop(i);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_PlayStop failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            }

            do {
                retVal = Audio_GetPlayState(i, &state);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("%s: Audio_GetPlayState failed (0x%x)\n",
                            __func__, retVal);
                    AudioIT1_Status = retVal;
                    break;
                } else if (AUDIO_PLAY_STATE_STOPPED != state) {
                    BCM_WaitEvent(AUDIO_TEST_PLAY_EVENTS);
                    BCM_GetEvent(AudioTest_PlayTask, &eventMask);
                    BCM_ClearEvent(eventMask);
                }
            } while(AUDIO_PLAY_STATE_STOPPED != state);

            retVal = Audio_GetPlayState(i, &state);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_GetPlayState failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            } else {
                ULOG_INFO("%s: Audio_GetPlayState (0x%x)\n",
                        __func__, state);
            }

            retVal = Audio_GetPlayStats(i, &stats);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_GetPlayStats failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
            } else {
                ULOG_INFO("%s: Buffer Played:%u, Underflow count:%u\n",
                        __func__, stats.bufPlayed, stats.underFlowCnt);
            }
        }
    }

	BCM_SetEvent(AudioTest, AUDIO_TEST_PLAY_COMP_EVENT);

    BCM_TerminateTask();
}

static int32_t AudioTest_RecCfgStart(Audio_HwIDType aID)
{
    int32_t retVal;
    Audio_InfoType info;
    Audio_RecCfgType cfg;

    retVal = Audio_GetRecInfo(aID, &info);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_GetRecInfo failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    cfg.sampleWidth = 32UL;
    cfg.samplePack = AUDIO_PACK_4B_MSB_ALIGN_LE;
    cfg.numChan = 2UL;
    cfg.startThreshold = (AUDIO_TEST_REC_START_THRES_MUL * info.bufSize);

    retVal = Audio_RecCfg(aID, &cfg);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_RecCfg failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    retVal = Audio_RecStart(aID);
    if (BCM_ERR_OK != retVal) {
        ULOG_ERR("%s: Audio_RecStart failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

err:
    return retVal;
}

static int32_t AudioTest_Rec(Audio_HwIDType aID)
{
    int32_t retVal;
    Audio_BufInfoType bufInfo;
    uint32_t bufLen = AUDIO_TEST_REC_BUF_SIZE;
    uint32_t i;

    retVal = Audio_DeQueueRecBuf(aID, bufLen, &bufInfo);
    if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
        ULOG_ERR("%s: Audio_DeQueuePlayBuf failed (0x%x)\n",
                __func__, retVal);
        goto err;
    }

    if (BCM_ERR_OK == retVal) {
        for (i = 0UL; i < (2UL * AUDIO_TEST_PLAY_PATTERN_SIZE); i++) {
            retVal = memcmp(&AudioTest_PlayBuf[i], bufInfo.buf,
                            AUDIO_TEST_PLAY_PATTERN_SIZE);
            if (0UL == retVal) {
                break;
            }
        }

        retVal = Audio_QueueRecBuf(aID, bufLen, bufInfo.buf);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: Audio_QueueRecBuf failed (0x%x)\n",
                    __func__, retVal);
            goto err;
        } else {
            if (AUDIO_TEST_PLAY_PATTERN_SIZE == i) {
                retVal = BCM_ERR_DATA_INTEG;
            }
        }
    }

err:
    return retVal;
}

TASK(AudioTest_RecordTask)
{

    int32_t retVal;
    uint32_t i, j = 0UL;
    BCM_EventMaskType eventMask = AUDIO_TEST_REC_EVENTS;
    Audio_RecStatsType stats;
    Audio_RecStateType state;

    for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
        if ((AudioTest_RecPortMask & (0x1UL << i)) != 0UL) {
            retVal = AudioTest_RecCfgStart(i);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_RecCfgStart failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            }
            retVal = Audio_GetRecState(i, &state);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_GetRecState failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            } else {
                ULOG_INFO("%s: Audio_GetRecState (0x%x)\n",
                        __func__, state);
            }
        }
    }

	do {
		for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
            if (((AudioTest_RecPortMask & (0x1UL << i)) != 0UL)
                    && ((eventMask & AudioTest_RecEvent[i]) != 0UL)) {
                /* Dequeue filled buffer and do compare & queue */
                retVal = AudioTest_Rec(i);
                if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
                    ULOG_ERR("%s: AudioTest_Rec failed (0x%x)\n",
                            __func__, retVal);
                    AudioIT1_Status = retVal;
                    break;
                }
            }
        }

        if ((BCM_ERR_OK != retVal) && (BCM_ERR_EAGAIN != retVal)) {
            break;
        }

        if (BCM_ERR_OK == retVal) {
            j++;
        }

        /* Wait for event from driver */
        if (BCM_ERR_EAGAIN == retVal) {
            BCM_WaitEvent(AUDIO_TEST_REC_EVENTS);
            BCM_GetEvent(AudioTest_RecordTask, &eventMask);
            BCM_ClearEvent(eventMask);
        }
	} while (j < AUDIO_TEST_REC_BUF_COUNT);

    if (AUDIO_TEST_REC_BUF_COUNT == j) {
        AudioTest_Result = AUDIO_TEST_RESULT_PASS;
        AudioIT1_Status = retVal;
    } else {
        AudioTest_Result = AUDIO_TEST_RESULT_FAIL;
        AudioIT1_Status = retVal;
    }

    for (i = 0UL; i < AUDIO_PORTS_MAX; i++) {
        if ((AudioTest_RecPortMask & (0x1UL << i)) != 0UL) {

            retVal = Audio_RecStop(i);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_RecStop failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
                break;
            }

            do {
                retVal = Audio_GetRecState(i, &state);
                if (BCM_ERR_OK != retVal) {
                    ULOG_ERR("%s: Audio_GetRecState failed (0x%x)\n",
                            __func__, retVal);
                    AudioIT1_Status = retVal;
                    break;
                }
                if  (AUDIO_REC_STATE_STOPPED != state) {
                    BCM_WaitEvent(AUDIO_TEST_REC_EVENTS);
                    BCM_GetEvent(AudioTest_RecordTask, &eventMask);
                    BCM_ClearEvent(eventMask);
                }
            } while (AUDIO_REC_STATE_STOPPED != state);

            retVal = Audio_GetRecStats(i, &stats);
            if (BCM_ERR_OK != retVal) {
                ULOG_ERR("%s: Audio_GetRecStats failed (0x%x)\n",
                        __func__, retVal);
                AudioIT1_Status = retVal;
            } else {
                ULOG_INFO("%s: Buffer recorded:%u, Overflow count:%u\n",
                        __func__, stats.bufRec, stats.overFlowCnt);
            }
        }
    }

	BCM_SetEvent(AudioTest, AUDIO_TEST_REC_COMP_EVENT);

    BCM_TerminateTask();
}

TASK(AudioTest)
{
    uint32_t i = 0UL;
    BCM_EventMaskType eventMask;
	uint32_t cfgCnt = sizeof(AudioTest_Config)/sizeof(Audio_CfgType);
    uint32_t patternCnt = AUDIO_TEST_PLAY_BUF_SIZE /
                            AUDIO_TEST_PLAY_PATTERN_SIZE;

    for (i = 0UL; i < patternCnt; i++) {
        BCM_MemCpy(&(AudioTest_PlayBuf[i * AUDIO_TEST_PLAY_PATTERN_SIZE]),
                    &(AudioTest_PlayPattern[i % 2UL][0UL]),
                    AUDIO_TEST_PLAY_PATTERN_SIZE);
    }

	for (i = 0UL; i < cfgCnt; i++) {
        Audio_Init(AudioTest_Config[i].hwID, &(AudioTest_Config[i]));
        Audio_EnableLoopback(AudioTest_Config[i].hwID);
	}

    BCM_ActivateTask(AudioTest_PlayTask);
    BCM_ActivateTask(AudioTest_RecordTask);

    AudioTest_Result = AUDIO_TEST_RESULT_PEND;

	BCM_WaitEvent(AUDIO_TEST_PLAY_COMP_EVENT);
	BCM_WaitEvent(AUDIO_TEST_REC_COMP_EVENT);
	BCM_GetEvent(AudioTest, &eventMask);
	BCM_ClearEvent(eventMask);

	for (i = 0UL; i < cfgCnt; i++) {
        Audio_DeInit(AudioTest_Config[i].hwID);
	}

    switch(AudioTest_Result) {
    case AUDIO_TEST_RESULT_FAIL:
        ULOG_INFO("AudioTest Failed\n");
        break;
    case AUDIO_TEST_RESULT_PASS:
        ULOG_INFO("AudioTest Pass\n");
        break;
    case AUDIO_TEST_RESULT_PEND:
        ULOG_INFO("AudioTest Pending\n");
        break;
    default:
        ULOG_INFO("AudioTest Invalid\n");
        break;
    }

    if(BCM_AT_EXECUTING == AudioIT1_Status) {
        AudioIT1_Status = BCM_ERR_OK;
    }
    BCM_TerminateTask();
}

/* Start Test */
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case AUDIO_CTRL_IT1:
        if (BCM_AT_NOT_STARTED == AudioIT1_Status) {
            AudioIT1_Status = BCM_AT_EXECUTING;
            BCM_ActivateTask(AudioTest);
        }
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}

/* Get Test results */
int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t retVal = BCM_ERR_OK;

    switch(aIndex) {
    case AUDIO_CTRL_IT1:
        retVal = AudioIT1_Status;
        break;
    default:
        retVal = BCM_AT_NOT_AVAILABLE;
        break;
    }
    return retVal;
}


void ApplicationInit()
{
    /* Dummy App Init */
}
