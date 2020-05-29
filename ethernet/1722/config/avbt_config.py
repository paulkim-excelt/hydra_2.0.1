#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2018-2019 Broadcom Limited.
# This program is the proprietary software of Broadcom Limited
# and/or its licensors, and may only be used, duplicated, modified
# or distributed pursuant to the terms and conditions of a separate,
# written license agreement executed between you and Broadcom
# (an "Authorized License").  Except as set forth in an Authorized
# License, Broadcom grants no license (express or implied), right
# to use, or waiver of any kind with respect to the Software, and
# Broadcom expressly reserves all rights in and to the Software
# and all intellectual property rights therein.  IF YOU HAVE
# NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
# IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
# ALL USE OF THE SOFTWARE.
#
# Except as expressly set forth in the Authorized License,
#
# 1.     This program, including its structure, sequence and organization,
# constitutes the valuable trade secrets of Broadcom, and you shall use
# all reasonable efforts to protect the confidentiality thereof,
# and to use this information only in connection with your use of
# Broadcom integrated circuit products.
#
# 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
# PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
# REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
# OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
# DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
# NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
# ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
# CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
# OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
#
# 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
# BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
# INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
# ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
# TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
# THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
# WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
# ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
#

import sys
import getopt
import os
import struct
import binascii
import lxml

from lxml import etree
from lxml import objectify

cfgMagic = 0xAA00
AvbCfgType = 0xAA00
rsv = 0

H264 = 0
PCM = 1

PCP_0 = 0
PCP_1 = 1
PCP_2 = 2
PCP_3 = 3
PCP_4 = 4
PCP_5 = 5
PCP_6 = 6
PCP_7 = 7

CLASS_A = 1
CLASS_B = 0

USERSPECIFIED = 0
FLOAT32 = 1
INT32 = 2
INT24 = 3
INT16 = 4

USER_SPEC = 0
FREQ_8KHZ = 1
FREQ_16KHZ = 2
FREQ_32KHZ = 3
FREQ_44_1KHZ = 4
FREQ_48KHZ = 5

PCM_MONO = 0
PCM_STEREO = 1

TIMESTAMP_EACH_PACKET = 0
TIMESTAMP_EVERY_8TH_PACKET = 1

def getHexValue(value):
    hexVal = '{0}'.format(value)
    val = int(hexVal, 16)
    return val

def getpayloadFormat(value):
    if 'H264' == value:
        val = H264
    else:
        val = PCM
    return val

def getpcpValue(value):
    if value == 'PCP_0':
        val = PCP_0
    elif value == 'PCP_1':
        val = PCP_1
    elif value == 'PCP_2':
        val = PCP_2
    elif value == 'PCP_3':
        val = PCP_3
    elif value == 'PCP_4':
        val = PCP_4
    elif value == 'PCP_5':
        val = PCP_5
    elif value == 'PCP_6':
        val = PCP_6
    else:
        val = PCP_7
    return val

def gettrafficClass(value):
    if 'CLASS_A' == value:
        val = CLASS_A
    else:
        val = CLASS_B
    return val

def getaudChannelsPerFrame(value):
    if 'PCM_MONO' == value:
        val = PCM_MONO
    else:
        val = PCM_STEREO
    return val

def getaudTimestampMode(value):
    if 'TIMESTAMP_EACH_PACKET' == value:
        val = TIMESTAMP_EACH_PACKET
    else:
        val = TIMESTAMP_EVERY_8TH_PACKET
    return val

def getaudioSampleFormat(value):
    if value == 'USERSPECIFIED':
        val = USERSPECIFIED
    elif value == 'FLOAT32':
        val = FLOAT32
    elif value == 'INT32':
        val = INT32
    elif value == 'INT24':
        val = INT24
    else:
        val = INT16
    return val

def getaudioSampleFrequency(value):
    if value == 'USER_SPEC':
        val = USER_SPEC
    elif value == '8KHZ':
        val = FREQ_8KHZ
    elif value == '16KHZ':
        val = FREQ_16KHZ
    elif value == '32KHZ':
        val = FREQ_32KHZ
    elif value == '44_1KHZ':
        val = FREQ_44_1KHZ
    else:
        val = FREQ_48KHZ
    return val

class avbt_config_t:
    def write(self, outfile, cfg):
        fil = open(outfile,'wb')
        fil.write(struct.pack('I', cfgMagic))
        fil.write(struct.pack('I', AvbCfgType))
        fil.write(struct.pack('I', len(cfg.findall('avb_port'))))
        fil.write(struct.pack('I', rsv))

        if hasattr(cfg, 'avb_port'):
            for each in cfg.avb_port:
                fil.write(struct.pack('Q', getHexValue(each.streamID)))
                if hasattr(each.payloadFormat, 'videoFormat'):
                    fil.write(struct.pack('I', getpayloadFormat(each.payloadFormat.videoFormat)))
                else:
                    fil.write(struct.pack('I', getpayloadFormat(each.payloadFormat.audioFormat)))
                if hasattr(each.payloadFormat, 'audioFormat'):
                    fil.write(struct.pack('I', getaudioSampleFormat(each.payloadFormat.audioSampleFormat)))
                    fil.write(struct.pack('I', getaudioSampleFrequency(each.payloadFormat.audioSampleFrequency)))
                    fil.write(struct.pack('I', getaudChannelsPerFrame(each.payloadFormat.audChannelsPerFrame)))
                    fil.write(struct.pack('I', each.payloadFormat.numBitsAudSample))
                    fil.write(struct.pack('I', getaudTimestampMode(each.payloadFormat.audTimestampMode)))
                else:
                    fil.write(struct.pack('I', 0))
                    fil.write(struct.pack('I', 0))
                    fil.write(struct.pack('I', 0))
                    fil.write(struct.pack('I', 0))
                    fil.write(struct.pack('I', 0))
                fil.write(struct.pack('I', each.maxTransitTimeinNS))
                fil.write(struct.pack('I', each.talkerProcDelayNs))
                fil.write(struct.pack('I', each.listenerProcDelayNs))
                fil.write(struct.pack('I', rsv))
                fil.write(struct.pack('Q', getHexValue(each.mcStreamAddr)))
                fil.write(struct.pack('I', getpcpValue(each.pcpValue)))
                fil.write(struct.pack('I', each.vlanValue))
                fil.write(struct.pack('I', each.percentRsvdBW))
                fil.write(struct.pack('I', gettrafficClass(each.trafficClass)))
                fil.write(struct.pack('I', each.headerLenOffset))
                fil.write(struct.pack('I', rsv))
        fil.close()

class XMLParser:
    def __init__(self, schemafile):
        self.repo = None
        self.dirs = []
        self.outdir = None
        self.schema = etree.XMLSchema(file = schemafile)
        self.parser = objectify.makeparser(schema = self.schema)
    def loadXML(self, file):
        fp = open(str(file),'r')
        xml = fp.read()
        fp.close
        try:
            brcm_cfg = objectify.fromstring(xml, self.parser)
        except:
            brcm_cfg = objectify.fromstring(xml, self.parser)
            print objectify.dump(brcm_cfg)
            return None
        return brcm_cfg
    def processCompXML(self, compfile, outfile):
        cfg = self.loadXML(compfile)
        if cfg is None:
            print compfile + ' is not a valid xml file'
            return

        avbt = avbt_config_t()
        avbt.write(outfile, cfg)
        print outfile+' is generated'

# ******************************************************************************
def usage():
    print("python avbt_config.py -i <input> -o <output> -s <schema>")
    sys.exit(1)

compfiles = []
compOutfiles = []
schemafile = None
try:
    opts, args = getopt.getopt(sys.argv[1:], "hi:o:s:",["help=", "input=", "output=", "schema="])
except:
    print("Error in parsing arguments")
    usage()
for opt, arg in opts:
    if opt in ("-h", "--help"):
        usage()
    elif opt in ("-i", "--input"):
        compfiles.append(arg)
    elif opt in ("-o", "--output"):
        compOutfiles.append(arg)
    elif opt in ("-s", "--schema"):
        schemafile = arg
    else:
        print("Unknown argument %s" % opt)
        usage()
if schemafile is None:
    usage()
xmlParser = XMLParser(schemafile)
if len(compfiles) is not 0 and len(compOutfiles) is 0:
    pre, ext = os.path.splitext(compfiles[0])
    outfile = pre +'.bin'
    compOutfiles.append(outfile)
if len(compfiles) is 0:
    compOutfiles = []
    #parse the current folder for xml files
    for filename in os.listdir('.'):
        if not filename.endswith('.xml'): continue
        compfiles.append(filename)
        pre, ext = os.path.splitext(filename)
        outfile = pre +'.bin'
        compOutfiles.append(outfile)
for i,compfile in enumerate(compfiles):
    xmlParser.processCompXML(compfile,compOutfiles[i])
