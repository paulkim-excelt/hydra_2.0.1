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

##  @defgroup grp_mpegts_configurator Configurator
#   @ingroup grp_mpegts
#
#   @addtogroup grp_mpegts_configurator
#   @{
#   @section sec_mpegts_config_overview Overview
#
#   This file generates the binary configuration image
#   for the MPEGTS library. It has the schema for the
#   configuration elements for XML.
#
#   @limitations None
#
#   @file mpegts_config.py
#   @brief MPEGTS Component Configurator
#   This script file contains the configurator implementation
#   for MPEGTS library.
#   @version 1.0 Changelist1
#   @version 0.1 Changelist0

import sys
import getopt
import os
import struct
import binascii
import lxml

from lxml import etree
from lxml import objectify

##  @name Component API IDs
#   @{
#   @brief API IDs for MPEGTS Configutor
#

    ## @brief #mpegts_config.MPEGTS_Configurator
BRCM_SWARCH_MPEGTS_CONFIGURATOR_CLASS    = 0
    ## @brief #mpegts_config.MPEGTS_TSPidSchemaGlobal()
BRCM_SWARCH_MPEGTS_TSPIDSCHEMA_GLOBAL     = 1
    ## @brief #mpegts_config.MPEGTS_TSParamsSchemaGlobal()
BRCM_SWARCH_MPEGTS_TSPARAMSSCHEMA_GLOBAL  = 2
    ## @brief #comp_config.MPEGTS_ElementSchemaGlobal()
BRCM_SWARCH_MPEGTS_ELEMENTSCHEMA_GLOBAL  = 3
    ## @brief #mpegts_config.MPEGTS_ConfigUsage()
BRCM_SWARCH_MPEGTS_CONFIGUSAGE_PROC      = 4
    ## @brief #mpegts_config.MPEGTS_ConfigMain()
BRCM_SWARCH_MPEGTS_CONFIGMAIN_PROC       = 5
##  @}

##  @brief Global context for MPEGTS_TSPidSchemaGlobal
#
#   This is the valid range for MPEGTS PID value
#   to be assigned in the TS header.
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
MPEGTS_TSPidSchemaGlobal = """
<xs:simpleType name = "tsPIDType">
    <xs:restriction base = "xs:unsignedShort">
        <xs:minInclusive value = "16"/>
        <xs:maxInclusive value = "8190"/>
    </xs:restriction>
</xs:simpleType>"""

##  @brief Global context for MPEGTS_TSParamsSchemaGlobal
#
#   This is the scheme for the MPEGTS library configuration
#   parameters.
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
MPEGTS_TSParamsSchemaGlobal = """
<xs:complexType name = "tsParamsType">
    <xs:sequence>
        <xs:element
            name = "patTable" type = "xs:string"
            />
        <xs:element
            name = "patTableSize" type = "xs:unsignedInt"
            />
        <xs:element
            name = "pmtTable" type = "xs:string"
            />
        <xs:element
            name = "pmtTableSize" type = "xs:unsignedInt"
            />
        <xs:element
            name = "tsPID" type = "tsPIDType"
            />
        <xs:element
            name = "pcrPID" type = "tsPIDType"
            />
        <xs:element
            name = "ptsOffsetForPES" type = "xs:unsignedInt"
            />
        <xs:element
            name = "numTsPerPayload" type = "xs:unsignedInt"
            />
    </xs:sequence>
</xs:complexType>"""

##  @brief Global context for MPEGTS_ElementSchemaGlobal
#
#   The element schema for the XML file used to specify the
#   configuration values to MPEGTS library.
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
MPEGTS_ElementSchemaGlobal = """
<xs:element
        name="mpegts_cfg" type = "tsParamsType" />"""

##  @brief MPEGTS Configurator
#
#   Component configurator class for processing the schema
#   elements of MPEGTS library.
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
class MPEGTS_Configurator:
    ##  @brief MPEGTS Config Image ID
    #
    #   The image ID for MPEGTS library configuration
    MPEGTS_CFMAGIC_VALUE1 = 0xAA40
    ##  @brief Get Hex Value
    #
    #   Function that converts input value into a HEX value
    #   to be written into the configuration image.
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre None
    #
    #   @param[in]      value       Value to be converted to Hex
    #
    #   @return void
    #
    #   @post None
    #
    #   @limitations None
    #
    #   @code{.unparsed}
    #   hexVal = '{0}'.format(value)
    #   val = int(hexVal, 16)
    #   return val
    #   @endcode
    def MPEGTS_GetBitHexValue(self, value):
        hexVal = '{0}'.format(value)
        val = int(hexVal, 16)
        return val
    ##  @brief Brief description of __init__
    #
    #   Configurator initialization function
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre None
    #
    #   @param void
    #
    #   @return void
    #
    #   @post None
    #
    #   @limitations None
    #
    #   @code{.unparsed}
    #   local = XML schema from port elements
    #   schema = etree.XMLSchema(objectify.fromstring(local))
    #   self.parser = objectify.makeparser(schema = schema)
    #   @endcode
    def __init__(self):
        local =  '<?xml version="1.0"?>\
                    <xs:schema xmlns:xs="http://www.w3.org/2001/XMLSchema">'
        local += MPEGTS_TSPidSchemaGlobal + MPEGTS_TSParamsSchemaGlobal + MPEGTS_ElementSchemaGlobal
        local += '</xs:schema>'
        schema = etree.XMLSchema(objectify.fromstring(local))
        ## @brief Brief description of parser
        self.parser = objectify.makeparser(schema = schema)
    ##  @brief Brief description of configFunc
    #
    #   Function that generates the configuration image from the
    #   XML elements for the MPEGTS library.
    #
    #   @behavior Sync, Non-reentrant
    #
    #   @pre None
    #
    #   @param[in]      self        COMP_Configurator object
    #   @param[in]      file        Brief description of file
    #
    #   @return void
    #
    #   @post None
    #
    #   @limitations None
    #
    #   @code{.unparsed}
    #   fp = open input file and read
    #   port_cfg = objectify.fromstring(xml, self.parser)
    #   fil = open output file for writing
    #   write all the configuration items into output file
    #   close output file
    #   @endcode
    def configFunc(self, infile, outfile):
        fp = open(infile,'r')
        xml = fp.read()
        fp.close()
        port_cfg = objectify.fromstring(xml, self.parser)

        fil = open(outfile,'wb')
        fil.write(struct.pack('I', self.MPEGTS_CFMAGIC_VALUE1))
        fil.write(struct.pack('I', self.MPEGTS_CFMAGIC_VALUE1))

        patByteCnt = 0
        patValues = str(port_cfg.patTable).split(' ')
        for i in range (len(patValues)):
            fil.write(struct.pack('B',self.MPEGTS_GetBitHexValue(patValues[i])))
            patByteCnt+=1
        PaddingByte = 188 - patByteCnt
        while PaddingByte > 0:
            fil.write(struct.pack('B',self.MPEGTS_GetBitHexValue('0xFF')))
            PaddingByte-=1

        fil.write(struct.pack('I', port_cfg.patTableSize))

        pmtByteCnt = 0
        pmtValues = str(port_cfg.pmtTable).split(' ')
        for i in range (len(pmtValues)):
            fil.write(struct.pack('B',self.MPEGTS_GetBitHexValue(pmtValues[i])))
            pmtByteCnt+=1
        PaddingByte = 188 - pmtByteCnt
        while PaddingByte > 0:
            fil.write(struct.pack('B',self.MPEGTS_GetBitHexValue('0xFF')))
            PaddingByte-=1

        fil.write(struct.pack('I', port_cfg.pmtTableSize))
        fil.write(struct.pack('I', port_cfg.tsPID))
        fil.write(struct.pack('I', port_cfg.pcrPID))
        fil.write(struct.pack('I', port_cfg.ptsOffsetForPES))
        fil.write(struct.pack('I', port_cfg.numTsPerPayload))
        fil.close()
        print outfile+' is generated'

##  @brief Print usage for MPEGTS Configurator
#
#   @behavior Sync, Non-reentrant
#
#   @param[in]      void
#
#   @return void
#
#   @post Application exits
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
#
#   @limitations None
#
def MPEGTS_ConfigUsage():
    print("python mpegts_config.py -i <input> -o <output>")
    sys.exit(1)

##  @brief main function for MPEGTS Configurator
#
#   The main function of the configurator which invokes
#   the other sub functions to create the configuration
#   image.
#
#   @behavior Sync, Non-reentrant
#
#   @pre None
#
#   @param      void
#
#   @return void
#
#   @post None
#
#   @trace #BRCM_SWREQ_MPEGTS_CONFIGURATOR
#
#   @limitations None
#
#   @code{.unparsed}
#   check for input arguments
#   load input and output file names
#   configurator = MPEGTS_Configurator()
#   configurator.configFunc(input_file, output_file)
#   @endcode
def MPEGTS_ConfigMain():
    input_file = None
    output_file = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hi:o:",
                                    ["help=", "input=", "output="])
    except:
        print("Error in parsing arguments")
        MPEGTS_ConfigUsage()
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            MPEGTS_ConfigUsage()
        elif opt in ("-i", "--input"):
            input_file = arg
        elif opt in ("-o", "--output"):
            output_file = arg
        else:
            print("Unknown argument %s" % opt)
            MPEGTS_ConfigUsage()
    if ((input_file == None) or (output_file == None)):
        MPEGTS_ConfigUsage()
    configurator = MPEGTS_Configurator()
    configurator.configFunc(input_file, output_file)

if __name__ == '__main__':
    MPEGTS_ConfigMain()
##  @}
