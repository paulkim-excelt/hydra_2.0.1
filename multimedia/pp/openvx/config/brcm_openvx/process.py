#!/usr/bin/python2
# -*- coding: utf-8 -*-
#
# Copyright 2018 Broadcom Limited.  All rights reserved.
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

import brcm_openvx
import image
import struct
import os
import defn
import logging

license_txt = """/*****************************************************************************
 Copyright 2018 Broadcom Limited.  All rights reserved.

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
******************************************************************************/"""
import numbers
import inspect
import hashlib
import brcm_openvx



def vxGenerateConfig_int(context,directory,numcores,dump_header=False):
    logger = logging.getLogger(__name__)
    if (isinstance(numcores,numbers.Number) == False):
        logger.error('vxGenerateConfig: numcores is not an integer')
        raise TypeError
    if (isinstance(directory,basestring) == False):
        logger.error('vxGenerateConfig: directory is not a string')
        raise TypeError
    ret = defn.verifyContext(context)
    if ret is not True:
        logger.error('vxGenerateConfig: Contect cannot be verified')
        raise AttributeError
    if (numcores <= 0) :
        logger.error('vxGenerateConfig: numcores is less than zero')
        raise ValueError
    if (numcores > 8) :
        logger.error('vxGenerateConfig: numcores is greater than 8')
        raise ValueError

    prefix = context['VX_NAME']
    #write binary file
    if not os.path.exists(directory):
        os.makedirs(directory)
    directory = directory + '/'
    print directory

    hash_md5 = hashlib.md5()
    for graph in context['VX_GRAPH']:
        for node in graph['VX_DATA'].node:
            msg = node['VX_DATA'].createMailBox(numcores)
            if msg is not None:
                for item in msg:
                    hash_md5.update(item)
    uniqueID = int((hash_md5.hexdigest())[0:8],16) + int(float(brcm_openvx.__version__)*100)

    for graph in context['VX_GRAPH']:
        try:
            fp = open(directory + prefix + '_' + graph['VX_NAME']+'.bin','wb')
            fp.write(struct.pack('>I',0x56580006))
            fp.write(struct.pack('I',uniqueID))
        except:
            print'vxGenerateConfig:Cannot open the bin file to write'
            raise EnvironmentError
        num_jobs = 0
        for node in graph['VX_DATA'].node:
            num_jobs += node['VX_DATA'].getNumJobs()
        fp.write(struct.pack('I',num_jobs))
        fp.write(struct.pack('I',numcores))
        fp.write(struct.pack('I',graph['VX_DATA'].getTotalTempSize()))

        for node in graph['VX_DATA'].node:
            msg = node['VX_DATA'].createMailBox(numcores)
            if msg is not None:
                for item in msg:
                    fp.write(item)
        for node in graph['VX_DATA'].node:
            tbl = node['VX_DATA'].getTableData()
            for item in tbl:
                fp.write(item)
        fp.close()
        if dump_header == True:
            try:
                ldr = open(directory + prefix + '_' + graph['VX_NAME'] +'.bin','rb')
                dump = open(directory + prefix + '_' + graph['VX_NAME'] +'.h','wb')
                dump.write(license_txt)
                dump.write('\n/******************************************************************************\nFile Name  : config.h\nDescription: This file contains config data\n ******************************************************************************/')
                dump.write('\n#ifndef GRAPH_'+graph['VX_NAME']+'_H\n#define GRAPH_'+graph['VX_NAME']+'_H\nstatic unsigned char graph_'+graph['VX_NAME']+'[] = {\n')
                isContinue = True
            except:
                print'vxGenerateConfig:Cannot open the bin file to write'
                raise EnvironmentError
            while isContinue == True:
                s = ldr.read(8)
                if len(s) == 0:
                    isContinue = False
                for c in s:
                    dump.write(" "+"0x{:02x}".format(ord(c)) + ",")
                dump.write('\n')
            dump.write('};\n')
            dump.write('#endif\n')
            dump.close()
            ldr.close()

    ## generate the config file
    try:
        fp = open(directory + prefix + '_' +'internal.h','wb')
    #wrire header
        fp.write(license_txt)

        fp.write('\n#ifndef '+prefix.upper()+'_INTERNAL_H\n#define '+prefix.upper()+'_INTERNAL_H\n\n')
        fp.write('\n')

        fp.write('#include <openvx_lib.h>\n\n')

        fp.write('#define '+prefix.upper()+'_UNIQUE_ID ('+str(uniqueID)+'UL)\n\n')

        fp.write('#define '+prefix.upper()+'_NUM_PP_CORES ('+str(numcores)+'UL)\n\n')

        fp.write('#define '+prefix.upper()+'_MAX_DATA_ENTRIES ('+str(context['VX_DATA'].getMaxDataEntries())+'UL)\n\n')
    except:
        print 'vxGenerateConfig:Cannot open the internal.h file to write'
        raise EnvironmentError
    tempSize = 0
    for graph in context['VX_GRAPH']:
        graphTempSize = graph['VX_DATA'].getTotalTempSize()
        if tempSize < graphTempSize:
            tempSize = graphTempSize

    totalScalarsSize = len(context['VX_SCALAR']) * 8

    fp.write('#define '+prefix.upper()+'_TEMP_STORAGE_IDX'+' (0UL)\n')
    fp.write('#define '+prefix.upper()+'_TEMP_STORAGE_SIZE'+' ('+str(tempSize + totalScalarsSize)+'UL)\n')
    fp.write('\n')

    for image in context['VX_IMAGE']:
        (name, size, offset) = image['VX_DATA'].getStorageDetails()
        fp.write('#define '+prefix.upper()+'_'+image['VX_NAME'].upper()+'_IMG_IDX'+' ('+str(name)+'UL)\n')
        fp.write('#define '+prefix.upper()+'_'+image['VX_NAME'].upper()+'_IMG_SIZE'+' ('+str(size)+'UL)\n')
        fp.write('\n')
    fp.write('\n')

    for pyramid in context['VX_PYRAMID']:
        (name, size, offset) = pyramid['VX_DATA'].getStorageDetails()
        fp.write('#define '+prefix.upper()+'_'+pyramid['VX_NAME'].upper()+'_PYRAMID_IDX'+' ('+str(name)+'UL)\n')
        fp.write('#define '+prefix.upper()+'_'+pyramid['VX_NAME'].upper()+'_PYRAMID_SIZE'+' ('+str(size)+'UL)\n')
        fp.write('\n')
    fp.write('\n')

    for array in context['VX_ARRAY']:
        (name, size, offset) = array['VX_DATA'].getStorageDetails()
        fp.write('#define '+prefix.upper()+'_'+array['VX_NAME'].upper()+'_ARRAY_IDX'+' ('+str(name)+'UL)\n')
        (type, maxCount) = array['VX_DATA'].getDataType()
        fp.write('typedef '+type+' '+prefix.upper()+'_'+array['VX_NAME'].upper()+'_ARRAY_TYPE;\n')
        fp.write('#define '+prefix.upper()+'_'+array['VX_NAME'].upper()+'_MAX_COUNT'+' ('+str(maxCount)+'UL)\n')
        fp.write('#define DEFINE_'+prefix.upper()+'_'+array['VX_NAME'].upper()+'(aName)\\\n')
        fp.write('    '+type+' aName['+str(maxCount)+'UL]')
        fp.write('\n\n')
    fp.write('\n')

    for scalar in context['VX_SCALAR']:
        (name, size, offset) = scalar['VX_DATA'].getStorageDetails()
        fp.write('#define '+prefix.upper()+'_'+scalar['VX_NAME'].upper()+'_SCALAR_IDX'+' ('+str(name)+'UL)\n')
        #fp.write('#define '+prefix.upper()+'_'+scalar['VX_NAME'].upper()+'_SCALAR_SIZE'+' ('+str(size)+'UL)\n')
        type = scalar['VX_DATA'].getDataType()
        fp.write('typedef '+type+' '+prefix.upper()+'_'+scalar['VX_NAME'].upper()+'_SCALAR_TYPE;')
        fp.write('\n')
    fp.write('\n')

    fp.write('#endif /* '+prefix.upper()+'_INTERNAL_H */\n')
    fp.close()
