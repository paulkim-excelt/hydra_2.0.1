#!/usr/bin/python2

#
# $Id$
# $Copyright: Copyright 2018 Broadcom Limited.
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
import binascii
import socket

from lxml import etree
from lxml import objectify

cfgMagic = 0xAA20
rtpCfgType = 0xAA20
rsv = 0

FORMAT_UNDEFINED = 0
FORMAT_FU_A_SLICE = 1
FORMAT_MPEGTS = 2

H264_VIDEO = 0
PCM_AUDIO = 1

ID_0 = 0
ID_1 = 1
ID_2 = 2
ID_3 = 3
ID_4 = 4
ID_5 = 5
ID_6 = 6
ID_7 = 7

RTCP_ENABLED = 1
RTCP_DISABLED = 0

def getBitHexValue(value):
    hexVal = '{0}'.format(value)
    val = int(hexVal, 16)
    return val

def getpayloadFormat(value):
    if 'FORMAT_UNDEFINED' == value:
        val = FORMAT_UNDEFINED
    elif 'FORMAT_FU_A_SLICE' == value:
        val = FORMAT_FU_A_SLICE
    else:
        val = FORMAT_MPEGTS
    return val

def getMediaFormat(value):
    if 'H264_VIDEO' == value:
        val = H264_VIDEO
    else:
        val = PCM_AUDIO
    return val

def getIfRTCPEnabled(value):
    if 'RTCP_ENABLED' == value:
        val = RTCP_ENABLED
    else:
        val = RTCP_DISABLED
    return val

def getUDPPayloadID(value):
    if 'ID_0' == value:
        val = ID_0
    elif 'ID_1' == value:
        val = ID_1
    elif 'ID_2' == value:
        val = ID_2
    elif 'ID_3' == value:
        val = ID_3
    elif 'ID_4' == value:
        val = ID_4
    elif 'ID_5' == value:
        val = ID_5
    elif 'ID_6' == value:
        val = ID_6
    else:
        val = ID_7
    return val

def getIPaddrAsHexU32(value):
    ip = binascii.hexlify(socket.inet_aton(str(value)))
    return getBitHexValue(ip)

class rtps_config_t:
    def write(self, outfile, cfg):
        fil = open(outfile,'wb')
        fil.write(struct.pack('I', cfgMagic))
        fil.write(struct.pack('I', rtpCfgType))
        fil.write(struct.pack('I', len(cfg.findall('rtp_port'))))
        fil.write(struct.pack('I', rsv))

        if hasattr(cfg, 'rtp_port'):
            for each in cfg.rtp_port:
                fil.write(struct.pack('I', getMediaFormat(each.rtpParams.mediaFormat)))
                fil.write(struct.pack('I', getBitHexValue(each.rtpParams.SSRC)))
                fil.write(struct.pack('I', getpayloadFormat(each.rtpParams.payloadFormat)))
                fil.write(struct.pack('I', getUDPPayloadID(each.rtpParams.rtpPayloadID)))
                fil.write(struct.pack('H', each.rtpParams.localPort))
                fil.write(struct.pack('H', each.rtpParams.remotePort))
                fil.write(struct.pack('I', getIPaddrAsHexU32(each.rtpParams.remoteIP)))
                fil.write(struct.pack('I', getIfRTCPEnabled(each.rtcpParams.isRTCPEnabled)))
                fil.write(struct.pack('256s', '{0}'.format(each.rtcpParams.cnameStr)))
                fil.write(struct.pack('4s', '{0}'.format(each.rtcpParams.name1733Str)))
                fil.write(struct.pack('Q', getBitHexValue(each.rtcpParams.streamID)))
                fil.write(struct.pack('I', getUDPPayloadID(each.rtcpParams.rtcpPayloadID)))
                fil.write(struct.pack('H', each.rtcpParams.localPort))
                fil.write(struct.pack('H', each.rtcpParams.remotePort))
                fil.write(struct.pack('I', getIPaddrAsHexU32(each.rtcpParams.remoteIP)))
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
            print objectify.fromstring(xml, self.parser)
            return None
        return brcm_cfg
    def processCompXML(self, compfile, outfile):
        cfg = self.loadXML(compfile)
        if cfg is None:
            print compfile + ' is not a valid xml file'
            return

        rtps = rtps_config_t()
        rtps.write(outfile, cfg)
        print outfile+' is generated'

# ******************************************************************************
def usage():
    print("python rtps_config.py -i <input> -o <output> -s <schema>")
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
    print 'schema file is needed.. Exiting'
    sys.exit()
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
