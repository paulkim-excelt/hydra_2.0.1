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
    @defgroup grp_vgc_qt Qualification Tests
    @ingroup grp_vgc

    @addtogroup grp_vgc_qt
    @{

    @file lvds_test.c
    @brief VGC Component Integration Test
    This source file contains the qualification tests for VGC

    @version 0.53 Derived from AUTOSW_VGC_Driver_Interface_Specification_0.53.docx
*/

/**
    @brief LVDS Integration test

    This test simulates a VGC Client and enables LVDS display.
    APIs Covered:
    -# VGC_DispInit
    -# VGC_SetBackgroundColor
    -# VGC_Start
    -# VGC_EnableSurface
    -# VGC_DequeueBuffer
    -# VGC_QueueBuffer
    -# VGC_Stop
    -# VGC_Deinit
    -# VGC_DispGetStats
    -# VGC_DispDrvSysCmdReq
    -# VGC_DispDrvSysCmdHandler
    -# VGC_DispDrvCmdHandler
    -# VGC_DispDrvPVInterruptHandler

    @trace #LVDSTest_IT0 #BRCM_ARCH_VGC_FUNCTION_PROC
*/
#define BRCM_INTP_VGC_FUNCTION_CASE0   (0UL)

/**
    @brief Logging level for the test application

    @trace #BRCM_ARCH_VGC_LOGLVL
*/
#define GetModuleLogLevel() ULOG_LVL_INFO

/* Includes */
#include <string.h>
#include "ee.h"
#include <bcm_err.h>
#include "ulog.h"
#include "bcm_time.h"
#include "console.h"
#include "vgc.h"
#include "vgc_osil.h"
#include "bcm_utils.h"
#include "i2c.h"
#include "cache.h"
#include <bcm_test.h>
#include "bcm_time.h"
#include <osil/bcm_osil.h>

/** @brief This test simulates a VGC Client and enables LVDS display.

    @testproc  #LVDSTest_IT0  #BRCM_INTP_VGC_FUNCTION_CASE0

    @code{.unparsed}
    Initialize VGC through VGC_DispInit API
    Set background through VGC_SetBackgroundColor API
    Start VGC through VGC_Start API
    Enable Video Surface through VGC_EnableSurface API
    De-queue video buffer through VGC_DequeueBuffer API
    Create a small rectangle with predefined color, position and size on the de-queued buffer
    Queue the buffer back to VGC through VGC_QueueBuffer API
    Wait for vsync event from VGC
    Upon receiving vsync signal (VGC_VSYNC_EVENT), continue the test.
    Dequeue video buffer through VGC_DequeueBuffer API
    Create a big rectangle(with a different color than what was used earlier), predefined position and size on the de-queued buffer
    Queue the buffer back to VGC through VGC_QueueBuffer API
    Wait for few seconds of time
    Repeat Steps 12, 13, 14 & 15 thrice in the same order.
    Get statistics information through VGC_DispGetStats API
    Print the same on console
    Stop VGC through VGC_Stop API
    Wait for Stop completion signal(VGC_STOP_EVENT) from VGC
    Clear Stop completion event
    De-initialize VGC_Deinit API

    Run this (for BRCM_INTP_VGC_FUNCTION_CASE0)
    @endcode

*/
static int32_t LVDSTest_IT0(void);

/** @} */

/* defines */
/* Test events mapping */
#define TEST_EVENT_START      (SystemEvent4)
#define TEST_EVENT_STOP       (SystemEvent5)
#define TEST_EVENT_POST       (SystemEvent6)
#define TEST_EVENT_STATS      (SystemEvent7)

#define WIDTH                 (1280UL)
#define HEIGHT                (720UL)

#define MAX_COLOURS           (5UL)

/*****************************************************************************/
const static VGC_DispCfgType VGCCfg = {
    .misc = {
        .output = VGC_OUTPUT_LVDS,
        .yuvToRgbBypass = TRUE,
        .lvdsCrCbSwap = TRUE,
        .iifCrCbSwap  = TRUE,
    },
    .cmp = {
        .bg = {
            .alpha = 0x00,
            .y_r   = 0x00,
            .cb_g  = 0xff,
            .cr_b  = 0x00,
            .fmt = VGC_PIXFMT_YUV444P,
        },
        .panel = {
            .width = WIDTH,
            .height = HEIGHT,
        },
    },
    .pv = {
        .isCont = TRUE,
        .horz = {
            .sync = 11,
            .bp = 61,
            .act = WIDTH,
            .fp = 61,
        },
        .vert = {
            .sync = 8,
            .bp = 15,
            .act = HEIGHT,
            .fp = 15,
        },
    },
    .lvds = {
        .mode = VGC_DISP_LVDSMODE_24_1,
        .de = VGC_DISP_LVDSDE_HACT_VACT,
        .isSyncEnabled = 0U,
    },
};

static VGC_PixelType Pix[MAX_COLOURS] = {
    [0] = {
        .alpha = 0x0,
        .y_r = 0xff,
        .cb_g = 0x0,
        .cr_b = 0x0,
        .fmt = VGC_PIXFMT_YUV444P,
    },
    [1] = {
        .alpha = 0x0,
        .y_r = 0x0,
        .cb_g = 0xff,
        .cr_b = 0x0,
        .fmt = VGC_PIXFMT_YUV444P,
    },
    [2] = {
        .alpha = 0x0,
        .y_r = 0x0,
        .cb_g = 0x0,
        .cr_b = 0xff,
        .fmt = VGC_PIXFMT_YUV444P,
    },
    [3] = {
        .alpha = 0x0,
        .y_r = 0x25,
        .cb_g = 0x45,
        .cr_b = 0x65,
        .fmt = VGC_PIXFMT_YUV444P,
    },
    [4] = {
        .alpha = 0x0,
        .y_r = 0x45,
        .cb_g = 0x34,
        .cr_b = 0x14,
        .fmt = VGC_PIXFMT_YUV444P,
    },
};

static uint32_t ColourIdx = 0UL;

static VGC_BufferType buffer;
static VGC_SurfAttrType surfAttr = {
    .window = {
        .buffRect = {
            .width = WIDTH,
            .height = HEIGHT,
        },
        .surfaceOffset = {
            .x = 0,
            .y = 0,
        },
        .dispRect = {
            .width = WIDTH,
            .height = HEIGHT,
        },
        .canvasOffset = {
            .x = 0,
            .y = 0,
        },
    },
    .fmt = VGC_PIXFMT_YUV444P,
    .blend = {
        .back = VGC_BLENDFACT_ZERO,
        .front = VGC_BLENDFACT_ONE,
        .alpha = 0UL,
    },
};

static VGC_SurfAttrType gSurfAttr = {
    .window = {
        .buffRect = {
            .width = WIDTH,
            .height = HEIGHT,
        },
        .surfaceOffset = {
            .x = 0,
            .y = 0,
        },
        .dispRect = {
            .width = WIDTH,
            .height = HEIGHT,
        },
        .canvasOffset = {
            .x = 0,
            .y = 0,
        },
    },
    .fmt = VGC_PIXFMT_ARGB8888,
    .blend = {
        .back = VGC_BLENDFACT_ONE_MINUS_FRONT,
        .front = VGC_BLENDFACT_FRONT,
        .alpha = 0UL,
    },
};

static VGC_DispStatsType stats;

static volatile int32_t LVDSTestRes = BCM_AT_NOT_STARTED;

/*****************************************************************************/
void fill_rect(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2,
        VGC_PixelType *aPix, uint8_t *aBuff, VGC_PixFmtType aFmt);
void EnableGPIOExpander(void);

typedef struct _YUV422 {
    uint8_t y[HEIGHT][WIDTH];
    uint8_t u[HEIGHT][WIDTH/2];
    uint8_t v[HEIGHT][WIDTH/2];
} YUV422;

typedef struct _YUV444 {
    uint8_t y[HEIGHT][WIDTH];
    uint8_t u[HEIGHT][WIDTH];
    uint8_t v[HEIGHT][WIDTH];
} YUV444;

typedef struct _ARGB8888 {
    struct {
        uint8_t a;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    } pix[HEIGHT][WIDTH];
} ARGB8888;


/*****************************************************************************/
void fill_rect_yuv422(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2, VGC_PixelType *aPix, uint8_t *aBuff)
{
    YUV422 *yuv = (YUV422 *)aBuff;
    int32_t i,j;

    ULOG_ERR("@%p (%d,%d) to (%d,%d) with (Y4,U2,V2)=(%d,%d,%d)\n", aBuff, aX1, aY1, aX2, aY2, aPix->y_r, aPix->cb_g, aPix->cr_b);
    for (i =0; i < WIDTH*HEIGHT*2; i++)
        *aBuff++ = 0;
    /* For each line in the window */
    for (i = aY1; i <= aY2; i++) {
        for (j = aX1; j <= aX2; j++) {
            yuv->y[i][j] = aPix->y_r;
            yuv->u[i][j>>1UL] = aPix->cb_g;
            yuv->v[i][j>>1UL] = aPix->cr_b;
        }
    }
}
void fill_rect_yuv444(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2, VGC_PixelType *aPix, uint8_t *aBuff)
{
    YUV444 *yuv = (YUV444 *)aBuff;
    int32_t i,j;

    ULOG_ERR("@%p (%d,%d) to (%d,%d) with (Y4,U4,V4)=(%d,%d,%d)\n", aBuff, aX1, aY1, aX2, aY2, aPix->y_r, aPix->cb_g, aPix->cr_b);
    for (i =0; i < WIDTH*HEIGHT*4; i++)
        *aBuff++ = 0;
    /* For each line in the window */
    for (i = aY1; i <= aY2; i++) {
        for (j = aX1; j <= aX2; j++) {
            yuv->y[i][j] = aPix->y_r;
            yuv->u[i][j] = aPix->cb_g;
            yuv->v[i][j] = aPix->cr_b;
        }
    }
}
void fill_rect_argb8888(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2, VGC_PixelType *aPix, uint8_t *aBuff)
{
    ARGB8888 *argb = (ARGB8888 *)aBuff;
    int32_t i,j;

    ULOG_ERR("@%p (%d,%d) to (%d,%d) with (A8,R8,G8,B8)=(%d,%d,%d,%d)\n", aBuff,
            aX1, aY1, aX2, aY2, aPix->alpha, aPix->y_r, aPix->cb_g, aPix->cr_b);
    for (i =0; i < WIDTH*HEIGHT*4; i++)
        *aBuff++ = 0;
    /* For each line in the window */
    for (i = aY1; i <= aY2; i++) {
        for (j = aX1; j <= aX2; j++) {
            argb->pix[i][j].a = aPix->alpha;
            argb->pix[i][j].r = aPix->y_r;
            argb->pix[i][j].g = aPix->cb_g;
            argb->pix[i][j].b = aPix->cr_b;
        }
    }
}

void fill_rect(uint32_t aX1, uint32_t aY1, uint32_t aX2, uint32_t aY2,
        VGC_PixelType *aPix, uint8_t *aBuff, VGC_PixFmtType aFmt)
{
    if (VGC_PIXFMT_YUV422P == aFmt)
        fill_rect_yuv422(aX1, aY1, aX2, aY2, aPix, aBuff);
    else if (VGC_PIXFMT_YUV444P == aFmt)
        fill_rect_yuv444(aX1, aY1, aX2, aY2, aPix, aBuff);
    else if (VGC_PIXFMT_ARGB8888 == aFmt)
        fill_rect_argb8888(aX1, aY1, aX2, aY2, aPix, aBuff);

    DCACHE_CleanAll();
}

int32_t LVDSTest_Stats()
{
    int i;
    int32_t ret;

    ret = VGC_DispGetStats(&stats);
    ULOG_ERR("Start: %lu:%lu\n", stats.start.s, stats.start.ns);
    ULOG_ERR("End  : %lu:%lu\n", stats.end.s, stats.end.ns);
    ULOG_ERR("peakISRTime     %lu\n", stats.peakISRTime    );
    ULOG_ERR("pwrDwnIRQCnt    %lu\n", stats.pwrDwnIRQCnt   );
    ULOG_ERR("bvbIRQCnt       %lu\n", stats.bvbIRQCnt      );
    ULOG_ERR("pvIRQCnt        %lu\n", stats.pvIRQCnt       );
    ULOG_ERR("pvErrors        %lu\n", stats.pvErrors       );
    ULOG_ERR("framesDisplayed %lu\n", stats.framesDisplayed);
    ULOG_ERR("queueVid        %lu\n", stats.queueVid       );
    ULOG_ERR("dequeueVid      %lu\n", stats.dequeueVid     );
    ULOG_ERR("queueGfx        %lu\n", stats.queueGfx       );
    ULOG_ERR("dequeueGfx      %lu\n", stats.dequeueGfx     );
    ULOG_ERR("repeat1CntVid   %lu\n", stats.repeat1CntVid  );
    ULOG_ERR("repeat2CntVid   %lu\n", stats.repeat2CntVid  );
    ULOG_ERR("repeat3CntVid   %lu\n", stats.repeat3CntVid  );
    ULOG_ERR("repeat4CntVid   %lu\n", stats.repeat4CntVid  );
    ULOG_ERR("repeatNCntVid   %lu\n", stats.repeatNCntVid  );
    ULOG_ERR("repeatMaxVid    %lu\n", stats.repeatMaxVid   );
    ULOG_ERR("repeat1CntGfx   %lu\n", stats.repeat1CntGfx  );
    ULOG_ERR("repeat2CntGfx   %lu\n", stats.repeat2CntGfx  );
    ULOG_ERR("repeat3CntGfx   %lu\n", stats.repeat3CntGfx  );
    ULOG_ERR("repeat4CntGfx   %lu\n", stats.repeat4CntGfx  );
    ULOG_ERR("repeatNCntGfx   %lu\n", stats.repeatNCntGfx  );
    ULOG_ERR("repeatMaxGfx    %lu\n", stats.repeatMaxGfx   );
    ULOG_ERR("Vid Repeat 3 TimeStamps\n");
    for (i = 0; (i < stats.repeat3CntVid) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu\n", stats.repeat3TsVid[i].s, stats.repeat3TsVid[i].ns);
    ULOG_ERR("Vid Repeat 4 TimeStamps\n");
    for (i = 0; (i < stats.repeat4CntVid) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu\n", stats.repeat4TsVid[i].s, stats.repeat4TsVid[i].ns);
    ULOG_ERR("Vid Repeat >4 TimeStamps\n");
    for (i = 0; (i < stats.repeatNCntVid) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu Cnt:%lu\n", stats.repeatNTsVid[i].s, stats.repeatNTsVid[i].ns, stats.repeatNValVid[i]);
    ULOG_ERR("Gfx Repeat 3 TimeStamps\n");
    for (i = 0; (i < stats.repeat3CntGfx) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu\n", stats.repeat3TsGfx[i].s, stats.repeat3TsGfx[i].ns);
    ULOG_ERR("Gfx Repeat 4 TimeStamps\n");
    for (i = 0; (i < stats.repeat4CntGfx) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu\n", stats.repeat4TsGfx[i].s, stats.repeat4TsGfx[i].ns);
    ULOG_ERR("Gfx Repeat >4 TimeStamps\n");
    for (i = 0; (i < stats.repeatNCntGfx) && (i < 20UL); i++)
        ULOG_ERR("%lu %lu Cnt:%lu\n", stats.repeatNTsGfx[i].s, stats.repeatNTsGfx[i].ns, stats.repeatNValGfx[i]);
    return ret;
}

int32_t LVDSTest_Post()
{
    int32_t ret;
    ret = VGC_DequeueBuffer(VGC_DISP_SURFACE_VID, WIDTH*HEIGHT*2, &buffer);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to dequeue buffer from VGC, ret=%d\n", ret);
        goto err_dq;
    }

    ret = VGC_QueueBuffer(&surfAttr, &buffer, NULL, NULL);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to queue filled buffer to VGC, ret=%d\n", ret);
        goto err_q;
    }
err_q:
err_dq:
    return ret;
}

int32_t LVDSTest_PostG()
{
    int32_t ret;
    ret = VGC_DequeueBuffer(VGC_DISP_SURFACE_GFX, WIDTH*HEIGHT*4, &buffer);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to graphics dequeue buffer from VGC, ret=%d\n", ret);
        goto err_dq;
    }

    ret = VGC_QueueBuffer(NULL, NULL, &gSurfAttr, &buffer);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to queue graphics filled buffer to VGC, ret=%d\n", ret);
        goto err_q;
    }
err_q:
err_dq:
    return ret;
}

int32_t LVDSTest_Start()
{
    int32_t ret;
    int i;
    const VGC_PixelType bg = {
            .alpha = 0x00,
            .y_r   = 0xff,
            .cb_g  = 0x00,
            .cr_b  = 0x00,
            .fmt = VGC_PIXFMT_YUV444P,
    };

    VGC_DispInit(&VGCCfg);
    VGC_SetBackgroundColor(&bg);

    ret = VGC_EnableSurface(VGC_DISP_SURFACE_VID);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to enable surface, ret=%d\n", ret);
        goto err_en_surf;
    }
    ret = VGC_EnableSurface(VGC_DISP_SURFACE_GFX);
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to enable graphics surface, ret=%d\n", ret);
        goto err_en_g_surf;
    }

    /****************V-FRAME*******************/
    for (i=0; i < 3UL; i++) {
        ret = VGC_DequeueBuffer(VGC_DISP_SURFACE_VID, WIDTH*HEIGHT*3, &buffer);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Failed to dequeue v-buffer from VGC, ret=%d\n", ret);
            goto err_dq;
        }

        fill_rect(10, 10, 150, 150, &Pix[ColourIdx++], buffer.addr, VGC_PIXFMT_YUV444P);
        if (ColourIdx == MAX_COLOURS) {
            ColourIdx = 0UL;
        }
        ret = VGC_QueueBuffer(&surfAttr, &buffer, NULL, NULL);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Failed to queue filled v-buffer to VGC, ret=%d\n", ret);
            goto err_q;
        }
    }
    /****************G-FRAME*******************/
    for (i=0; i < 3UL; i++) {
        ret = VGC_DequeueBuffer(VGC_DISP_SURFACE_GFX, WIDTH*HEIGHT*4, &buffer);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Failed to dequeue g-buffer from VGC, ret=%d\n", ret);
            goto err_g_dq;
        }

        fill_rect(100, 100, 200, 200, &Pix[ColourIdx++], buffer.addr, VGC_PIXFMT_ARGB8888);
        if (ColourIdx == MAX_COLOURS) {
            ColourIdx = 0UL;
        }

        ret = VGC_QueueBuffer(NULL, NULL, &gSurfAttr, &buffer);
        if (BCM_ERR_OK != ret) {
            ULOG_ERR("Failed to queue filled g-buffer to VGC, ret=%d\n", ret);
            goto err_g_q;
        }
    }

    ret = VGC_Start();
    if (BCM_ERR_OK != ret) {
        ULOG_ERR("Failed to start VGC, ret=%d\n", ret);
        goto err_start;
    }

err_start:
err_g_q:
err_g_dq:
err_q:
err_dq:
err_en_g_surf:
err_en_surf:
    return ret;
}

int32_t LVDSTest_Stop()
{
    int32_t ret;
    BCM_EventMaskType mask;
    int32_t retVal;

    ret = VGC_Stop();
    if (BCM_ERR_OK == ret) {
        /* wait for completion signal */
        retVal = BCM_WaitEvent(VGC_STOP_EVENT);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_WaitEvent failed \n", __FUNCTION__);
        }
        retVal = BCM_GetEvent(LVDSTestTask, &mask);
        if (BCM_ERR_OK != retVal) {
            ULOG_ERR("%s: BCM_GetEvent failed\n", __FUNCTION__);
        }
        if (0UL != (mask & VGC_STOP_EVENT)) {
            BCM_ClearEvent(VGC_STOP_EVENT);
        }

        /* Proceed to de-initialise all subcomponents */
        ret = VGC_Deinit();
        if (BCM_ERR_OK != ret)
            ULOG_ERR("Failed to deinitialise VGC, ret=%d\n", ret);

    } else {
        ULOG_ERR("Failed to stop VGC, ret=%d\n", ret);
    }
    return ret;
}

int32_t LVDSTest_IT0(void)
{
    BCM_EventMaskType mask;
    int32_t ret = BCM_ERR_OK;

    ret = LVDSTest_Start();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_Start passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_Start failed\n", __FUNCTION__);
    }

    BCM_WaitEvent(VGC_VSYNC_EVENT);
    BCM_GetEvent(LVDSTestTask, &mask);
    if (0UL != (mask & VGC_VSYNC_EVENT)) {
        ULOG_VERB("Vsync received\n");
    }
    ret = LVDSTest_Post();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_Post passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_Post failed\n", __FUNCTION__);
    }

    BCM_CpuNDelay(1000 * 1000 * 1000); /* wait for a second */
    ret = LVDSTest_PostG();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_PostG passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_PostG failed\n", __FUNCTION__);
    }

    BCM_CpuNDelay(1000 * 1000 * 1000); /* wait for a second */
    ret = LVDSTest_Post();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_Post passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_Post failed\n", __FUNCTION__);
    }

    BCM_CpuNDelay(1000 * 1000 * 1000); /* wait for a second */
    ret = LVDSTest_Stats();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_Stats passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_Stats failed\n", __FUNCTION__);
    }
    ret = LVDSTest_Stop();
    if (BCM_ERR_OK == ret){
        ULOG_INFO("%s: LVDSTest_Stop passed\n", __FUNCTION__);
    } else {
        goto done;
        ULOG_ERR("%s: LVDSTest_Stop failed\n", __FUNCTION__);
    }

done:
    return ret;
}

/*****************************************************************************/
TASK(LVDSTestTask)
{
    int32_t ret;

    LVDSTestRes = LVDSTest_IT0();

    ret = BCM_TerminateTask();
    if(BCM_ERR_OK != ret) {
        ULOG_ERR("BCM_TerminateTask Failed \n");
    }
}

/*****************************************************************************/
int32_t BCM_ExecuteAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = BCM_ERR_OK;
        LVDSTestRes = BCM_AT_EXECUTING;
        BCM_ActivateTask(LVDSTestTask);
    }
    return ret;
}

int32_t BCM_GetResultAT(uint32_t aIndex)
{
    int32_t ret = BCM_AT_NOT_AVAILABLE;
    if (1UL == aIndex) {
        ret = LVDSTestRes;
    }
    return ret;
}

/* ApplicationInit */
void ApplicationInit()
{
}
