#!/usr/bin/python
################################################################################
#
# Copyright 2016-2018 Broadcom Limited.  All rights reserved.
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

import argparse
import sys
import subprocess
import os
from time import gmtime, strftime
import datetime
import re

def write_to_log_file(filename, data):
	f = open(filename, 'ab+')
	f.write(data)
	f.close()

def runAndlog(cmd, cmd_type, logfile):
	print ("Running Coverity {}: {}".format(cmd_type, cmd))
	cov_proc = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE)
	cmdlog = cov_proc.communicate()[0]
	cmd_rc = cov_proc.returncode
	write_to_log_file(logfile, "\n{}".format(cmd))
	write_to_log_file(logfile, "\n{}".format(cmdlog))
	if (cmd_rc == 0):
		print ("Coverity {} Completed Successfully".format(cmd_type))
	else:
		print ("Coverity {} Failed, check logs... {}".format(cmd_type, logfile))
#		sys.exit(1)

#Coverity output directory structure under BRCM_SDK_ROOT
#
#[cov-out
#	-> EMIT/<comp>/<date-time>
#	-> HTML_MISRA/<comp>/<date-time
#	-> log/<comp>_<date-time>
#	-> log/MISRA_<comp>-<date-time>.html]

def main():
	cov_version = "2019.03"
	cov_bin_path = "/tools/coverity/" + cov_version + "/Linux-64/bin/"
	#cov_config_file = "/projects/BLR_SW_AUTOMOTIVE/tools/Coverity_" + cov_version + "/config_template_gcc/coverity_config.xml"
	#cov_misra_config = "/tools/coverity/" + cov_version + "/Linux-64/config/MISRA/MISRA_c2012_7.config"
	cov_misra_config = "/tools/coverity/" + cov_version + "/Linux-64/config/coding-standards/misrac2012/misrac2012-mandatory-required.config"
	Date = datetime.datetime.now().strftime("%d-%m-%Y")
	Time = datetime.datetime.today().strftime("%H-%M-%S")
	Date_Time = Date + '_' + Time
	script_path = sys.path[0]
	repo_path = script_path + '/../..'
	cov_out = repo_path + '/cov_out'
	build_dir = repo_path + '/build'
	cov_config_file = repo_path + "/process/repository/scripts/coverity_templates/" + cov_version + "/coverity_config.xml"
	#logfile = repo_path + '/cov_log-' + Date_Time
	parser = argparse.ArgumentParser()
	parser.add_argument('-b', '--board', help='Board name for Hydra, e.g. bcm89107_evk', required=True)
	parser.add_argument('-c', '--comp', default=None, help='component name, e.g. vidxmtr vidrcvr', nargs='*')
	parser.add_argument('-d', '--date', default=None, help='date_time string to be used for creating folder name')
	parser.add_argument('--cov_only', action='store_true', help='Generate only Coverity report, exclude MISRA')
	parser.add_argument('--noclean', action='store_true', help='Do not run make clean')
	args = vars(parser.parse_args())
	board, comp, date_arg = args['board'], args['comp'], args['date']
	print ("Board:", board)
	print ("Comp:", comp)
	print ("Type:", args['cov_only'])
	print ("Make Clean:", args['noclean'])

	if date_arg is None:
		print ("Date input not given, using date from this script: {}".format(Date_Time))
	else:
		print ("Date input given: {}".format(date_arg))
		Date_Time = date_arg
	log_dir = cov_out + '/log/'
	emit_path = cov_out + '/EMIT/' + board + '/' + Date_Time
	html_path = cov_out + '/HTML_MISRA/' + board + '/' + Date_Time
	html_app_cov_path = html_path + '/app_cov'
	html_app_misra_path = html_path + '/app_misra'
	logfile = log_dir + board + '-' + Date_Time
	make_clean_cmd = 'make clean'

	mk_emit_path = subprocess.Popen("mkdir -p {}".format(emit_path), shell=True, stdout=subprocess.PIPE).communicate()[0]
	mk_html_path = subprocess.Popen("mkdir -p {}".format(html_app_cov_path), shell=True, stdout=subprocess.PIPE).communicate()[0]
	mk_html_path = subprocess.Popen("mkdir -p {}".format(html_app_misra_path), shell=True, stdout=subprocess.PIPE).communicate()[0]
	mk_log_dir = subprocess.Popen("mkdir -p {}".format(log_dir), shell=True, stdout=subprocess.PIPE).communicate()[0]
	write_to_log_file(logfile, "Board: {}".format(board))
	write_to_log_file(logfile, "\nComponent: {}".format(comp))
	write_to_log_file(logfile, "\nScript Path: {}".format(script_path))
	print ("Path:", script_path)
	print ("Repo:", repo_path)

	os.chdir(build_dir)
	if args['noclean'] is True:
		print ("Skipping make clean, running Incremental Coverity")
	else:
		write_to_log_file(logfile, "\nRunning Make clean: {}".format(make_clean_cmd))
		print ("Running Make clean: {}".format(make_clean_cmd))
		clean_proc = subprocess.Popen(make_clean_cmd, shell=True, stdout=subprocess.PIPE)
		clean_data = clean_proc.communicate()[0]
		clean_rc = clean_proc.returncode
		write_to_log_file(logfile, "\n{}".format(clean_data))


	if comp is None:
		cov_build_cmd = cov_bin_path + 'cov-build --emit-complementary-info -c ' + cov_config_file + ' --dir ' + emit_path + ' make board=' + board + ' LIB_GNU_CC=1'
		#print cov_build_cmd
		runAndlog(cov_build_cmd, "Build", logfile)
	else:
		for each_comp in comp:
			cov_build_cmd = cov_bin_path + 'cov-build --emit-complementary-info -c ' + cov_config_file + ' --dir ' + emit_path + ' make board=' + board + \
						' comp=' + each_comp + ' LIB_GNU_CC=1'
			#print cov_build_cmd
			runAndlog(cov_build_cmd, "Build", logfile)

	cov_analyze_cmd = cov_bin_path + 'cov-analyze -c ' + cov_config_file + ' --dir ' + emit_path + ' --all --aggressiveness-level high'
	cov_analyze_misra_cmd = cov_bin_path + 'cov-analyze -c ' + cov_config_file + ' --dir ' + emit_path + ' --coding-standard-config ' + cov_misra_config

	print "Running Coverity Analysis"
#Analyze and create report for App Coverity
	runAndlog(cov_analyze_cmd, "Analyze", logfile)
	cov_report_cmd = getreportcmd(cov_bin_path, cov_config_file, emit_path, html_app_cov_path)
	runAndlog(cov_report_cmd, "Report", logfile)

	if args['cov_only'] is False:
		print "Running MISRA Analysis"
#Analyze and create report for App MISRA
		runAndlog(cov_analyze_misra_cmd, "Analyze", logfile)
		cov_report_cmd = getreportcmd(cov_bin_path, cov_config_file, emit_path, html_app_misra_path)
		runAndlog(cov_report_cmd, "Report", logfile)


	#Get relative path of coverity html report folder wrt "log" folder. This is required in html report generated by this script.
	#"html_path" contains absolute path of html output directory, from this parse out "cov_out" directory
	html_relative_rx = r'^{0}(.*)'.format(cov_out)
	html_search_obj = re.search(html_relative_rx, html_path, re.M)
	html_relative_path = html_search_obj.group(1)

	cov_html_report_file = html_path + '/index.html'
	cov_html_summary = html_path + '/summary.html'
	os.chdir(html_path)

	print ("Coverity build Done.")
	print ("Coverity App Report: {}".format(html_app_cov_path))
	print ("MISRA App Report: {}".format(html_app_misra_path))
	print ("Coverity Log: {}".format(logfile))

def print_help():
	""" print help information and exit """
	print ("Usage:")
	print (sys.argv[0] + ' -b|--board {board} -c|--comp {component}')

def getreportcmd(cov_bin_path, cov_config_file, emit, html):
	cov_report_cmd = cov_bin_path + 'cov-format-errors -c ' + cov_config_file + ' --dir ' + emit + \
					' --exclude-files \'/projects/ntsw-tools/gcc-arm-none-eabi-4_9-2015q3.*\' ' + '--html-output ' + html
	return cov_report_cmd

VERSION = 'v0.2'
if __name__ == "__main__":
	print ("Starting Hydra Coverity")
	main()
