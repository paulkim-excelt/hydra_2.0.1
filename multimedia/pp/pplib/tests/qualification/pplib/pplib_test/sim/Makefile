#++++++++++++++++++++++++++++ FileHeaderBegin +++++++++++++++++++++++++++++#
# (c) videantis GmbH
#++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++#
#                                                                          #
# FILENAME:             Makefile
#                                                                          #
#--------------------------------------------------------------------------#
#                                                                          #
# RESPONSIBLE:         Andreas Dehnhardt                                   #
#                                                                          #
# DESCRIPTION:         Makefile for simulating v-MP3 project with host code#
#                                                                          #
#                                                                          #
#+++++++++++++++++++++++++++++ FileHeaderEnd ++++++++++++++++++++++++++++++#

#
#
#

SIMCFG_FILE = call_pplib
MP_PROGRAM = pplib_test

VID_BIN            ?= ./pplib_host
VID_NUM_MP         ?= 1
VID_OUTFILE        ?= pplib.out.y
VID_LOOP           ?= 1

#simulator breakpoint files
VMP_SIM_BP_FILES=\
	bp/$(MP_PROGRAM)_mp0.simbp \
	bp/$(MP_PROGRAM)_mp1.simbp \
	bp/$(MP_PROGRAM)_mp2.simbp \
	bp/$(MP_PROGRAM)_mp3.simbp \
	bp/$(MP_PROGRAM)_mp4.simbp \
	bp/$(MP_PROGRAM)_mp5.simbp \
	bp/$(MP_PROGRAM)_mp6.simbp \
	bp/$(MP_PROGRAM)_mp7.simbp

sim: vid_sim_config_link.cfg $(VMP_SIM_BP_FILES)
	$(VID_BIN) -x 1280 -y 720 -o $(VID_OUTFILE) -N $(VID_NUM_MP) -l $(VID_LOOP) &
	-vidsim -cores mp3.1 1 -mp0: -bp stop=10 trace=1 vid_vmp_exit._exit -f vid_sim_config_link.cfg $(VID_SIM_ARGS)

vid_sim_config_link.cfg:
	ln -s $(SIMCFG_FILE).simcfg vid_sim_config_link.cfg

#simulator breakpoint file: create emtpy file when not in use, do not delete intermediate file, ecplipse might change this file
.PRECIOUS: %.simbp
%.simbp:
	touch $@




# end of file
