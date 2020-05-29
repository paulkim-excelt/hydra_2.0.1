#
# Copyright 2016-2017 Broadcom Limited.  All rights reserved.
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

PARSER_MAKEFILE = parser.mk
PARSER_PATH = src/parser

MODE_MAKEFILE = mode_image_util.mk
MODE_PATH = src/mode

ISP_MAKEFILE = isp_image_util.mk
ISP_PATH = src/isp

WRITER_APP_MAKEFILE = image_writer.mk
WRITER_APP_PATH = src/app/image_writer
WRITER_APP_BIN_NAME = image_writer

READER_APP_MAKEFILE = image_reader.mk
READER_APP_PATH = src/app/image_reader
READER_APP_BIN_NAME = image_reader

COMBINE_APP_MAKEFILE = image_combine.mk
COMBINE_APP_PATH = src/app/image_combine
COMBINE_APP_BIN_NAME = image_combine

IMAGETOC_MAKEFILE = image_to_array.mk
IMAGETOC_PATH = src/image_to_array

UNICAM_MAKEFILE = unicam_image_util.mk
UNICAM_PATH = src/unicam

SENSOR_MAKEFILE = sensor_image_util.mk
SENSOR_PATH = src/sensor

#Output floders
LIBS = lib
OBJS = obj
BINS = binaries

all:
	mkdir -p $(LIBS)/$(OBJS)
	mkdir -p $(BINS)

	#make parser
	cd $(PARSER_PATH) && $(MAKE) -f $(PARSER_MAKEFILE)
	mv $(PARSER_PATH)/*.a $(LIBS)/.
	mv $(PARSER_PATH)/*.o $(LIBS)/$(OBJS)

	#make mode
	cd $(MODE_PATH) && $(MAKE) -f $(MODE_MAKEFILE)
	mv $(MODE_PATH)/*.a $(LIBS)/.
	mv $(MODE_PATH)/*.o $(LIBS)/$(OBJS)

	#make isp
	cd $(ISP_PATH) && $(MAKE) -f $(ISP_MAKEFILE) $1
	mv $(ISP_PATH)/*.a $(LIBS)/.
	mv $(ISP_PATH)/*.o $(LIBS)/$(OBJS)

	#make unicam
	cd $(UNICAM_PATH) && $(MAKE) -f $(UNICAM_MAKEFILE)
	mv $(UNICAM_PATH)/*.a $(LIBS)/.
	mv $(UNICAM_PATH)/*.o $(LIBS)/$(OBJS)

	#make sensor
	cd $(SENSOR_PATH) && $(MAKE) -f $(SENSOR_MAKEFILE)
	mv $(SENSOR_PATH)/*.a $(LIBS)/.
	mv $(SENSOR_PATH)/*.o $(LIBS)/$(OBJS)

	#make image_to_c
	cd $(IMAGETOC_PATH) && $(MAKE) -f $(IMAGETOC_MAKEFILE)
	mv $(IMAGETOC_PATH)/*.a $(LIBS)/.
	mv $(IMAGETOC_PATH)/*.o $(LIBS)/$(OBJS)

	#Main Writer Application
	cd $(WRITER_APP_PATH) && $(MAKE) -f $(WRITER_APP_MAKEFILE) $1
	mv $(WRITER_APP_PATH)/$(WRITER_APP_BIN_NAME) $(BINS)

	#Main Reader Application
	cd $(READER_APP_PATH) && $(MAKE) -f $(READER_APP_MAKEFILE) $1
	mv $(READER_APP_PATH)/$(READER_APP_BIN_NAME) $(BINS)

	#Main Combine Application
	cd $(COMBINE_APP_PATH) && $(MAKE) -f $(COMBINE_APP_MAKEFILE)
	mv $(COMBINE_APP_PATH)/$(COMBINE_APP_BIN_NAME) $(BINS)

clean:
	rm -rf $(LIBS) rm -rf $(BINS)
