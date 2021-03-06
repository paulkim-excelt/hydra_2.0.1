#
# Copyright 2016-2019 Broadcom Limited.  All rights reserved.
#
# This program is the proprietary software of Broadcom Limited and/or its
# licensors, and may only be used, duplicated, modified or distributed pursuant
# to the terms and conditions of a separate, written license agreement executed
# between you and Broadcom (an "Authorized License").
#
# Except as set forth in an Authorized License, Broadcom grants no license
# (express or implied), right to use, or waiver of any kind with respect to the
# Software, and Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED
# LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD
# IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.
#
#  Except as expressly set forth in the Authorized License,
# 1. This program, including its structure, sequence and organization,
#    constitutes the valuable trade secrets of Broadcom, and you shall use all
#    reasonable efforts to protect the confidentiality thereof, and to use this
#    information only in connection with your use of Broadcom integrated
#    circuit products.
#
# 2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
#    AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR
#    WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
#    TO THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED
#    WARRANTIES OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A
#    PARTICULAR PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS,
#    QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION.
#    YOU ASSUME THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE
#    SOFTWARE.
#
# 3. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS
#    LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
#    OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO
#    YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. \$1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
#
####################################################################################


CC = gcc
AR = ar
CFLAGS +=-D__GNU__ -DGNU

SRC = isp_hw_config_image.c
SRC += isp_tuner_config_image.c
SRC += isp_image_util.c
SRC += isp_tuner_precalac_awb.c
SRC += ../../../../isys/lib/pwl/pwl.c

OUT = libisp.a
OBJ = $(SRC:.c=.o)

INC =  -I../../../../isys/lib/fmwk/
INC += -I../../../../isys/inc/
INC += -I../../../../isys/inc/osil/
INC += -I../parser/ -I../../../../../../common/include/
INC += -I../../src/isp/ -I../image_to_array/ -I../../src/unicam/
INC += -I../../../../sensor/inc/
INC += -I../../../../tuners/fmwk/inc/
INC += -I../../../../tuners/agc/inc/
INC += -I../../../../tuners/awb/inc/
INC += -I../../../../tuners/denoise/inc/
INC += -I../../../../tuners/cc/inc/
INC += -I../../../../tuners/defpix/inc/
INC += -I../../../../tuners/sharpen/inc/
INC += -I../../../../tuners/bl/inc/
INC += -I../../../../tuners/tm/inc/
INC += -I../../../../isys/lib/isp/
INC += -I../../../../isys/lib/pwl/
INC += -I../../../../../../system/common/inc/
INC += -I../../../../../../system/utils/inc/
INC += -I../../../../../../system/bcm8910x/inc/rdb/

default: $(OUT)
.c.o:
	$(CC) $(INC) -c $< -o $@  -m32 $(CFLAGS) -Wall -Werror
$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)
	rm ../../../../isys/lib/pwl/pwl.o
