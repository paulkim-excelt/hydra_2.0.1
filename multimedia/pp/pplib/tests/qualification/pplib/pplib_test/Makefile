##/*++++++++++++++++++++++++++++++ FileHeaderBegin +++++++++++++++++++++++++++++++
#
# CONFIDENTIAL AND PROPRIETARY INFORMATION
# Copyright 2004 - 2016 videantis GmbH
# All Rights Reserved
#
# This document contains confidential and proprietary information of videantis
# GmbH and is protected by copyright, trade secret and other local, state,
# federal, and international laws. Its receipt or possession does not convey
# any rights to reproduce, transfer, disclose or publish its contents, or to
# manufacture, commercially or non-commercially use or sell anything it may
# describe or contain. Reproduction, disclosure or any use without specific
# written authorization of videantis GmbH or an individual license agreement
# with videantis GmbH is strictly forbidden.
#
# *++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# *
# * FILENAME: Makefile
# *
# * DESCRIPTION:  Makefile for compiling tutorial example
# *
# *++++++++++++++++++++++++++++++ FileHeaderEnd +++++++++++++++++++++++++++++++*/


include ../../../../../pplib/lowlevellib/cfg.mk

#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
# Binary running on host
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
HOST_BIN = sim/pplib_host
HOST_C_FILE = src/pplib_host.c

VMP_PROGRAM    = pplib_test
VMP_BINDIR     = bin
VMP_FINAL_DIR  = ../../pplib
#
#
# general targets
#

HOST_CFLAGS=-I$(shell readlink -f ../../../../../pplib/lowlevellib/libvidlowlevel-2.3-brcm_hydra_sim)

all:	dirs $(VMP_BINDIR)/$(VMP_PROGRAM).bin $(HOST_BIN)
#
# make bootbinary to boot from host code
#
.PRECIOUS: inc/%.h
inc/%.h:	bin/%.bin
	bin2h $< $(notdir $(basename $@)) $@
#
# compile host code
#
sim/%:
	gcc -Wall -o $@ -Iinc $(HOST_CFLAGS) $(HOST_C_FILE) $< $(HOST_LIBFLAGS)


TARGET_CPU = mp3.1
VMPCC=vmpcc #-DADDR_IMAGE_FILE_IN=0x20008000 -DADDR_QUANTTABLE_FILE_IN=0x20159800 -DADDR_IMAGE_FILE_OUT=0x20008000
VID_VMP_LLVM_STACK_SIZE ?= 128

# Add the vmp sources here
CL_FUNC_FILES = pplib.cl
ASM_VMP_FILES = \
    src/call_pplib.asm

ASM_FUNC_FILES = $(addprefix obj/,$(addsuffix .asm, $(basename $(CL_FUNC_FILES))))

VMP_LIB_DIR=$(shell readlink -f ../../../../../pplib/lowlevellib/libvidlowlevel-2.3-brcm_hydra_sim/vmp-lib)

.PRECIOUS: obj/%.asm
obj/%.asm:	src/%.cl
	$(VMPCC) $(VMPCC_FLAGS) -c -O2 -o $@ $<

VMP_LIB_FILES = \
    vid_vmp_boot_loader.asm \
    vid_vmp_mbox.asm \
	vid_vmp_lib.asm


$(VMP_BINDIR)/$(VMP_PROGRAM).bin:	$(ASM_FUNC_FILES)
	$(VMP_ASM) $(VMP_ASM_FLAGS) 				\
	--out-asm=$(basename $@).out.asm			\
	--out-code=$(basename $@).imem				\
	--out-ext-code=$(basename $@).ovl.imem		\
	--out-nice-memory-imem=$(basename $@).niceout.asm	\
	--out-memory-dmem=$(basename $@).dmem			\
	--out-memory-map=$(basename $@).nocodemap		\
	--verbose \
	$(VMP_LIB_FILES)	\
	$(ASM_FUNC_FILES)   \
	$(ASM_VMP_FILES)
	cat $(basename $@).nocodemap $(basename $@).niceout.asm > $(basename $@).map
	cat $(basename $@).imem $(basename $@).ovl.imem $(basename $@).dmem > $@
	#bin2h $@ $(notdir $(basename $@)) inc/$(notdir $(basename $@))_mp.h
	bin2h $(VMP_BINDIR)/$(VMP_PROGRAM).bin $(VMP_PROGRAM) inc/$(VMP_PROGRAM).h
	cp $(VMP_BINDIR)/$(VMP_PROGRAM).bin $(VMP_FINAL_DIR)/$(VMP_PROGRAM).bin
#
# clean target
#


.PHONY:	clean
clean:
	$(RM) $(HOST_BIN)
	$(RM) $(VMP_BINDIR)/*
	$(RM) $(VMP_FINAL_DIR)/$(VMP_PROGRAM).bin
	$(RM) inc/$(VMP_PROGRAM).h
	$(RM) obj/*
	$(RM) sim/trace/*trace sim/prof/mp* sim/*.y bin/call*

