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
 File Name:  pplib_host.c
 Description: This file contains the control flow and data flow code for the
 host side
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <limits.h>
#include <stdint.h>
#include <unistd.h>

#include <vid_lowlevelif.h>
#include <vid_target_if.h>
#include <call_pplib.h>
#include <pplib.h>
#include <target_consts.h>
#include <vid_dcif.h>

unsigned int mpcores[VLL_NUM_MP] = { VLL_ID_MP_0,
VLL_ID_MP_1,
VLL_ID_MP_2,
VLL_ID_MP_3,
VLL_ID_MP_4,
VLL_ID_MP_5,
VLL_ID_MP_6,
VLL_ID_MP_7 };

static int debug_mode = 0;
static int num_mpcores = NUM_MPCORES;
static int quietmode = 0;

static int XDIM = 640;
static int YDIM = 480;
static int loopmode = 1;
static char* input_filename = NULL;
static char* mat_filename = NULL;
static char* output_filename = NULL;
static CvMat dst;
static CvMat src;
static CvMat mat;
static CvMat inbuf;

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * main function
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int main(int argc, char** argv) {

    int rc;
    int i;
    uint signature;

    /* parse program arguments */
    parse_arguments(argc, argv);

    // vid_set_verbosity(0x0f);

    /* initialize videantis lowlevel lib */
    rc = vid_lowlevel_init();
    if (VMP_OK != rc) {
        printf("Error in vid_lowlevel_init init!!\n");
        exit(0);
    }

    /* initialize mailboxes to all v-MP cores we are going to use */
    for (i = 0; i < num_mpcores; i++) {
        rc = vid_mbox_init(mpcores[i]);
        if (VMP_OK != rc) {
            printf("Error in vid_mbox_init for v-MP #%d!!\n", mpcores[i]);
            return rc;
        }
    }

    /* boot v-MP cores used from host processor */
    for (i = 0; i < num_mpcores; i++) {
        printf("\nBooting v-MP coreid:%d   call_vmp_mbox\n", mpcores[i]);

        rc = vid_boot_core(call_pplib, call_pplib_size, mpcores[i]);

        if (VMP_OK != rc) {
            printf("Error in vid_boot_core for v-MP #%d!!\n", mpcores[i]);
            return rc;
        }
    }

    for (i = 0; i < num_mpcores; i++) {
          printf("\n Verifying v-MP coreid:%d   for signature\n",mpcores[i]);
          do
          {
              vid_read_reg(VMP_CTRL_BASE + ( VMP_MP_REGWIN_BASE + i * VMP_REGWIN_SIZE ) + GPDATA0_OFFSET, &signature);
          }while(signature != 0x01234567);
          printf("\n Verification of v-MP coreid:%d   successful\n",mpcores[i]);
      }

    /* process on videantis v-MP4280HDX */
    while (loopmode) {
        exec_algorithm(&src, &dst, &mat, &inbuf);
        loopmode--;
    }

    /* release lowlevel interface */
    vid_lowlevel_release();

    return 0;

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * prepare message and send to all v-MP active
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void exec_algorithm(CvMat* src, CvMat* dst, CvMat* mat, CvMat* Inbuf) {
    unsigned int i;
    unsigned int msgID;
    unsigned int handle;
    unsigned int mtype;

    vid_payload_t msgPayload;

	msgID = 0x01234567;
	for(i=0;i<56;i++)
	{
		msgPayload.pl_ui8[i] = 0xAA;
	}
	printf("\n Test started");
	vid_mbox_send(mpcores[0], msgID, (vid_payload_t *) &msgPayload,
			MBOX_BLOCKING);

	/* submit and receive buffers */
	vid_mbox_rcv(mpcores[0], &mtype, &handle, (vid_payload_t *) &msgPayload,
			MBOX_BLOCKING);

	if (mtype != (0x89ABCDEF)) {
		printf(" \n Message type failed %x \n\n",mtype);
	}

	for(i=0;i<56;i++)
	{
		if (msgPayload.pl_ui8[i] != 0xBB)
		{
			printf(" \n message payload failed %x \n\n",msgPayload.pl_ui8[i]);
			break;
		}
	}
	(void) vid_mbox_rel(mpcores[0], handle);
	printf(" \n Test Completed\n");

}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * helper functions
 *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
static void print_help(int exval) {
    printf("\nThis application executes the cvTutorial algorithm\n");

    printf("options\n\n");

    printf("  -h              print this help and exit\n");

    printf("  -l num          loop mode [0=endless, n=loop n times\n");
    printf("  -o FILE         set output file\n");
    printf("  -i FILE         set input file\n\n");
    printf("  -D [1,2]        enable profiling output\n");
    printf("  -N num          number of MP-Cores to use\n");

    exit(exval);
}

static void parse_arguments(int argc, char** argv) {
    int opt;

    // no arguments given
    if (argc == 1) {
        fprintf(stderr, "This program needs arguments....\n\n");
        print_help(1);
    }

    while ((opt = getopt(argc, argv, "hl:x:y:o:i:m:D:N:Q")) != -1) {
        switch (opt) {
        case 'h':
            print_help(0);
            break;
        case 'l':
            loopmode = atoi(optarg);
            if (!loopmode)
                loopmode = INT_MAX;
            break;
        case 'i':
            printf("Input file: %s\n", optarg);
            input_filename = optarg;
            break;
        case 'm':
            printf("Mat file: %s\n", optarg);
            mat_filename = optarg;
            break;
        case 'o':
            printf("Output file: %s\n", optarg);
            output_filename = optarg;
            break;
        case 'D':
            debug_mode = atoi(optarg);
            break;
        case 'Q':
            quietmode = 1;
            break;
        case 'x':
            XDIM = atoi(optarg);
            break;
        case 'y':
            YDIM = atoi(optarg);
            break;
        case 'N':
            num_mpcores = atoi(optarg);

            if (num_mpcores < 1) {
                num_mpcores = 1;
            }
            if (num_mpcores > 8) {
                num_mpcores = 8;
            }
            break;
        case ':':
            fprintf(stderr, "Error - Option `%c' needs a value\n\n", optopt);
            print_help(1);
            break;
        case '?':
            fprintf(stderr, "Error - No such option: `%c'\n\n", optopt);
            print_help(1);
        }
    }

    /* print all remaining options */
    for (; optind < argc; optind++) {
        printf("argument not valid: %s\n", argv[optind]);
    }
}
