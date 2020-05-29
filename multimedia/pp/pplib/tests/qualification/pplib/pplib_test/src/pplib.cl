/*****************************************************************************
 Copyright 2015 Broadcom Corporation.  All rights reserved.

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
/******************************************************************************
 File Name:  pplib.cl
 Description: This file contains DMA calls and control flow for dearping.
 ******************************************************************************/

/* llvm interface to videantis-C */
#ifdef __VMP__
# include "vmp_cl/vmp_cl.h"
#endif
/* emulation library for videantis-C */
#ifndef __VMP__
#include "vmp_cl/vid_videantis-C.hpp"
#endif

__kernel void pplib_main() {
	int i;
    __local struct vid_mbox_t mbox;
    uint msg_type;
    uint msg_type_out = 0;
    volatile uint *gpdata0 = (volatile uint *) 0x3DA;

    vid_mbox_init(&mbox, VLL_ID_HOST);
    *gpdata0 = 0x01234567;
    msg_type = vid_mbox_rcv(&mbox);
    vid_mbox_rel(&mbox);
    if(msg_type == 0x01234567)
    {
    	for(i=0;i<7;i++)
    	{
    		mbox.payload.pl_ui32[i] = (uint2)0xBBBBBBBB;
    	}
    	vid_mbox_send(&mbox, 0x89ABCDEF);
    }
    while(1);
}
