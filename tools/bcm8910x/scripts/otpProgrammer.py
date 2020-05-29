#! python2
##############################################################################
# Copyright 2018-2019 Broadcom Limited.  All rights reserved.
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
#    OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER
#    IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF
#    ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
################################################################################

import sys
from ctypes import *
from shutil import copyfile

import zlib
import struct
import binascii
import tarfile
import threading
import ctypes
import platform
import sys
import getopt
import Queue
import os
import shutil
import atexit
import zipfile
import tempfile
import time

global backend
backend = None

global ip_search_range
global ip_prefix
ip_prefix = '192.168'
ip_search_range = ['101', '103', '105', '106', '107', '109']

TARGET_SEQNUM_MASK = 0x1E00
TARGET_SEQNUM_SHIFT = 9
HOST_SEQNUM_MASK = 0x0F00
HOST_SEQNUM_SHIFT = 8
BL_READY_MASK = 0x1
STATUS_MASK = 0x100
PERCENT_MASK = 0xFE
PERCENT_SHIFT = 1
CMD_MASK = 0xF000
CMD_SHIFT = 12

RAM_DWNLD_CMD_FLASH          = 0
RAM_DWNLD_CMD_EXECUTE        = 1
RAM_DWNLD_CMD_ERASE          = 2
RAM_DWNLD_CMD_FLASH_WRITE    = 3
RAM_DWNLD_CMD_OTP_RD         = 4
RAM_DWNLD_CMD_OTP_WR_SEC_KEY = 5
RAM_DWNLD_CMD_OTP_ENABLE_SEC = 6
RAM_DWNLD_CMD_OTP_WRITE_CUST = 7
RAM_DWNLD_CMD_OTP_WR_MAC_ADDR = 8
RAM_DWNLD_CMD_OTP_RD_MAC_ADDR = 9


ARG_UINT32 = 0
ARG_FILE = 1
# *****************************************************************************
EDC_CRC = 1
def getByteArray(entry, size):
    if size == 64:
        data = struct.pack('Q', entry)
    elif size == 16:
        data = struct.pack('H', entry)
    else:
        data = struct.pack('I', entry)
    return bytearray(data)

class DownloadImage:
    def __init__(self, infile, outfile):
        self.magic_number = 0x54415248 #'T''A''R''H'
        self.img_size = 0
        self.flags = 0
        self.err_det_code = 0
        self.input = infile
        self.outfile = outfile
    def appendImage(self, output):
        with open(self.input, "rb") as image:
            output.write(image.read())
            image.close()
    def writeImage(self, flash_id, edc):
        flash_id_shift = 2
        output = open(self.outfile, 'wb')
        if EDC_CRC == edc:
            crc = 0xFFFFFFFF
            total_size = 0
            with open(self.input, 'rb') as image:
                buffer = image.read(65536)
                while len(buffer) > 0 :
                    crc = zlib.crc32(buffer, crc)
                    total_size += len(buffer)
                    buffer = image.read(65536)
                image.close()
            self.err_det_code = (crc & 0xFFFFFFFF)
        img_stat = os.stat(self.input)
        self.img_size = img_stat.st_size
        self.flags = (flash_id << flash_id_shift) | edc
        output.write(getByteArray(self.magic_number, 32))
        output.write(getByteArray(self.img_size, 32))
        output.write(getByteArray(self.flags, 32))
        output.write(getByteArray(self.err_det_code, 64))
        self.appendImage(output)


# *****************************************************************************
class RAMClient:
    def __init__(self):
        print 'RAMClient init'
        loader = ctypes.cdll
        if sys.platform.startswith('win'):
            library = os.environ['BRCM_FLASHER_SEGGER_PATH'] + '\jlinkarm.dll'
            stdcallLoader =  ctypes.windll
        elif sys.platform.startswith('linux'):
            library = os.environ['BRCM_FLASHER_SEGGER_PATH'] + '/libjlinkarm.so.6'
            stdcallLoader = ctypes.cdll
        else:
            print ('%s OS is not supported' % sys.platform)
        self.iJLink, self.iBackendInfo = loader.LoadLibrary(library), library
        self.iStdCalliJLink = stdcallLoader.LoadLibrary(library)
        self.iRegVal = ctypes.c_int16()
        self.iSeqNum = 0
        self.respData = ctypes.c_int32()
    def openSession(self, dev = None):
        if dev != None:
            self.iJLink.JLINKARM_EMU_SelectByUSBSN(dev)
        self.iJLink.JLINKARM_Open()
    def closeSession(self):
        self.iJLink.JLINKARM_Close()
    def cleanUp(self):
        self.closeSession()
    def listDevices(self):
        if self.iJLink.JLINKARM_EMU_GetNumDevices() > 0:
            self.openSession()
            if (self.iJLink.JLINKARM_GetSN() >= 0):
                print str(self.iJLink.JLINKARM_GetSN())
            self.closeSession()

    def findAddr(self, fname, txt):
        with open(fname) as searchfile:
            for line in searchfile:
                if txt in line:
                    addr = line.split()
                    if (addr[1] == txt):
                        searchfile.close()
                        return addr[0]

    def enterDwnldMode(self, folder, flag):
        bl_found = 0
        mapfile_found = 0
        timeout = 100
        print 'Configuring iJLink'
        self.iJLink.JLINKARM_SetSpeed(5000) # 5000 kHz
        self.iJLink.JLINKARM_CORE_Select(0x0E0100FF) # JLINK_CORE_CORTEX_M7
        self.iJLink.JLINKARM_TIF_Select(0) # JLINKARM_TIF_JTAG
        if self.iJLink.JLINKARM_Connect() >= 0:
            print 'JLink connected successfully'
        else:
            print 'JLink connection failed'
            return 0
        self.iJLink.JLINKARM_Reset()
        print 'Reset SRAM'
        self.iJLink.JLINKARM_WriteU32(0xe0103010, 0x10)
        self.iJLink.JLINKARM_WriteU32(0xe0103410, 0x10)
        self.iJLink.JLINKARM_WriteU32(0xe0103810, 0x10)
        self.iJLink.JLINKARM_WriteU32(0xe0103c10, 0x10)
        print 'Disable watchdog'
        self.iJLink.JLINKARM_WriteU32(0xe0100008, 0x0)
        self.iJLink.JLINKARM_WriteU32(0xe0102100, 0x80000000)
        time.sleep(1)
        for file in os.listdir(folder):
            if file.endswith("bl.map"):
                mapfile_found = 1
                self.iHostAddr = int(self.findAddr(folder+'/'+file, "BL_HostInfo"), 16)
                self.iTargetAddr = int(self.findAddr(folder+'/'+file, "BL_TargetInfo"), 16)
                self.iLoadAddr = int(self.findAddr(folder+'/'+file, "BL_DWNLD_StartAddr"), 16)
                self.iBLLoadAddr = int(self.findAddr(folder+'/'+file, "__text_start__"), 16)
                break
        if mapfile_found == 0:
            print 'BL map file is not found'
            return 0
        for file in os.listdir(folder):
            if file.endswith("evk_bl.bin"):
                bl_found = 1
                self.iStdCalliJLink.JLINK_DownloadFile(folder+'/'+file, self.iBLLoadAddr)
                break
        if bl_found == 0:
            print 'Bootloader binary is not found in the temp folder. Select the file again'
            return 0
        self.iJLink.JLINKARM_WriteU32(0xe010200c, flag)
        self.iJLink.JLINKARM_WriteReg(15, self.iBLLoadAddr)
        self.iJLink.JLINKARM_Go()
        while timeout:
            timeout = timeout - 1
            self.iJLink.JLINKARM_ReadMemU16(self.iTargetAddr, 1, byref(self.iRegVal), None)
            if ((self.iRegVal.value & BL_READY_MASK) == BL_READY_MASK):
                self.iSeqNum = 0
                return 1
        return 0
    def sendCmd(self, device, input, cmdID, argType, cmdArg, respList):
        folder = None
        try:
            zip = zipfile.ZipFile(input)
            folder = tempfile.mkdtemp()
            print folder
            zip.extractall(folder)
            zip.close()
            zip = None
        except:
            print 'File not found:'+ str(input)
            return 1
        error = 0
        self.openSession(device)
        if self.enterDwnldMode(folder, 0x9) == 1:
            self.iSeqNum = 1 - self.iSeqNum
            flag = (self.iSeqNum << HOST_SEQNUM_SHIFT) | (cmdID << CMD_SHIFT)
            if argType == ARG_UINT32:
                for i in range(len(cmdArg)):
                    print("write arg: %x to addr: %x" %(cmdArg[i], self.iLoadAddr + i * 4))
                    self.iJLink.JLINKARM_WriteU32(self.iLoadAddr + (i * 4), cmdArg[i])
            if argType == ARG_FILE:
                imgName = folder + '/' +'_dwnld.bin'
                dwnldimg = DownloadImage(cmdArg[0], imgName)
                dwnldimg.writeImage(0, EDC_CRC)
                self.iStdCalliJLink.JLINK_DownloadFile(imgName, self.iLoadAddr)
            self.iJLink.JLINKARM_WriteU16(self.iHostAddr, flag)
            timeout = 10
            while timeout:
                time.sleep(0.1)
                self.iJLink.JLINKARM_ReadMemU16(self.iTargetAddr, 1, byref(self.iRegVal), None)
                percent = ((self.iRegVal.value & PERCENT_MASK) >> PERCENT_SHIFT)
                target_seq = ((self.iRegVal.value & TARGET_SEQNUM_MASK) >> TARGET_SEQNUM_SHIFT)
                if ((target_seq != self.iSeqNum) or (percent == 100)):
                    timeout = timeout - 1
                if (target_seq == self.iSeqNum):
                    error = 0
                    break
            if ((self.iRegVal.value & STATUS_MASK) == STATUS_MASK):
                error = 1
                print("sendCmd[%d] failed" % cmdID)
            else:
                print("sendCmd[%d] success" % cmdID)
                if cmdID == RAM_DWNLD_CMD_OTP_RD_MAC_ADDR:
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 0, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 4, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 8, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 12, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 16, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
                else:
                    self.iJLink.JLINKARM_ReadMemU32(self.iLoadAddr + 4, 1, byref(self.respData), None)
                    respList.append(self.respData.value)
        else:
            error = 1
        self.closeSession()
        shutil.rmtree(folder)
        return error

# *****************************************************************************
class OTPProgrammer:
    def __init__(self):
        self.command = -1
        self.otpAddr = -1
        self.otpData = -1
        self.sec_key = None
        self.wr_mac_addr = None
        self.loc = None
        self.device = 0
        self.zipFile = None
        self.argList = []
        self.respList = []
        self.ramClient = RAMClient()
        try:
            os.environ["BRCM_FLASHER_SEGGER_PATH"]
        except KeyError:
            print "Please set the environment variable BRCM_FLASHER_SEGGER_PATH"
            sys.exit()
    def execCmd(self):
        if self.command == -1:
            return
        if self.command == RAM_DWNLD_CMD_OTP_RD:
            self.argList.append(self.otpAddr)
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, ARG_UINT32, self.argList, self.respList)
            print("OTP Read: row: %x data: %x" %(self.otpAddr, self.respList[0]))
        elif self.command == RAM_DWNLD_CMD_OTP_WRITE_CUST:
            self.argList.append(self.otpAddr)
            self.argList.append(self.otpData)
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, ARG_UINT32, self.argList, self.respList)
        elif self.command == RAM_DWNLD_CMD_OTP_ENABLE_SEC:
            self.argList.append(0)
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, ARG_UINT32, self.argList, self.respList)
        elif self.command == RAM_DWNLD_CMD_OTP_WR_SEC_KEY:
            self.argList.append(self.sec_key)
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, ARG_FILE, self.argList, self.respList)
        elif self.command == RAM_DWNLD_CMD_OTP_WR_MAC_ADDR:
#            self.argList = self.wr_mac_addr
            self.argList.append(self.loc)
            self.argList.append((self.wr_mac_addr[0] << 16) | (self.wr_mac_addr[1] << 8) | self.wr_mac_addr[2])
            self.argList.append((self.wr_mac_addr[3] << 16) | (self.wr_mac_addr[4] << 8) | self.wr_mac_addr[5])
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, ARG_UINT32, self.argList, self.respList)
        elif self.command == RAM_DWNLD_CMD_OTP_RD_MAC_ADDR:
            self.ramClient.sendCmd(self.device, self.zipFile, self.command, None, self.argList, self.respList)
            if len(self.respList) != 0:
                print ("-" * 50)
                print ("Location\t MAC address\t\t Valid")
                print ("-" * 50)
                macAddrOct1 = ((self.respList[1] >> 16) & 0xFF)
                macAddrOct2 = ((self.respList[1] >> 8) & 0xFF)
                macAddrOct3 = (self.respList[1] & 0xFF)
                macAddrOct4 = ((self.respList[2] >> 16) & 0xFF)
                macAddrOct5 = ((self.respList[2] >> 8) & 0xFF)
                macAddrOct6 = (self.respList[2] & 0xFF)
                print ("0\t\t {0:02x}:{1:02x}:{2:02x}:{3:02x}:{4:02x}:{5:02x}\t {6:x}".format(macAddrOct1, macAddrOct2, macAddrOct3, macAddrOct4, macAddrOct5, macAddrOct6, self.respList[0] & 0x1))
                macAddrOct1 = ((self.respList[3] >> 16) & 0xFF)
                macAddrOct2 = ((self.respList[3] >> 8) & 0xFF)
                macAddrOct3 = (self.respList[3] & 0xFF)
                macAddrOct4 = ((self.respList[4] >> 16) & 0xFF)
                macAddrOct5 = ((self.respList[4] >> 8) & 0xFF)
                macAddrOct6 = (self.respList[4] & 0xFF)
                print ("1\t\t {0:02x}:{1:02x}:{2:02x}:{3:02x}:{4:02x}:{5:02x}\t {6:x}".format(macAddrOct1, macAddrOct2, macAddrOct3, macAddrOct4, macAddrOct5, macAddrOct6, (self.respList[0] & 0x2) >> 1))
                print ("-" * 50)

        else:
            print("OTPProgrammer: invalid command")
            return
    def usage(self):
        print ("otpProgrammer.py -l => List flashable devices")
        print ("otpProgrammer.py -d <device> -i <zip_file> --read <otp row in hex> => read an OTP row")
        print ("otpProgrammer.py -d <device> -i <zip_file> --write <otp row in hex> --data <32-bit hex data> => Write to a custom OTP row")
        print ("otpProgrammer.py -d <device> -i <zip_file> --sec_enable 1 => Enable secure OTP bit")
        print ("otpProgrammer.py -d <device> -i <zip_file> --write_key <key_file> => Write secure key")
        print ("otpProgrammer.py -d <device> -i <zip_file> --mac_addr_write <mac address(xx:xx:xx:xx:xx:xx)>  --loc <location 0/1>=> Write MAC address")
        print ("otpProgrammer.py -d <device> -i <zip_file> --mac_addr_read 1 => Read MAC address")
        sys.exit(2)

    def parse(self, args):
        try:
            opts, args = getopt.getopt(sys.argv[1:],"hi:d:l",["read=", "write=", "data=", "sec_enable=", "write_key=", "mac_addr_read=", "mac_addr_write=", "loc="])
        except getopt.GetoptError:
            print("Error in parsing arguments")
            self.usage()
            sys.exit(2)

        for opt, arg in opts:
            if opt == "-h":
                self.usage()
                sys.exit(1)
            elif opt == "-i":
                self.zipFile = arg
            elif opt == "-d":
                self.device = arg
            elif opt == "-l":
                self.ramClient.listDevices()
            elif opt == "--read":
                self.command = RAM_DWNLD_CMD_OTP_RD
                self.otpAddr = int(arg, 16)
                print("otp read: addr: %d" %self.otpAddr)
            elif opt == "--write":
                self.command = RAM_DWNLD_CMD_OTP_WRITE_CUST
                self.otpAddr = int(arg, 16)
            elif opt == "--data":
                self.otpData = int(arg, 16)
            elif opt == "--sec_enable":
                self.command = RAM_DWNLD_CMD_OTP_ENABLE_SEC
            elif opt == "--write_key":
                self.command = RAM_DWNLD_CMD_OTP_WR_SEC_KEY
                self.sec_key = arg
            elif opt == "--loc":
                self.command = RAM_DWNLD_CMD_OTP_WR_MAC_ADDR
                self.loc = int(arg, 16)
            elif opt == "--mac_addr_write":
                self.command = RAM_DWNLD_CMD_OTP_WR_MAC_ADDR
                self.wr_mac_addr = arg.split(":")
                self.wr_mac_addr = [int(oct, 16) for oct in self.wr_mac_addr]
            elif opt == "--mac_addr_read":
                self.command = RAM_DWNLD_CMD_OTP_RD_MAC_ADDR
            else:
                print("invalid command")
                self.usage()

    def validateArgs(self):
        if (((self.command == RAM_DWNLD_CMD_OTP_WRITE_CUST) and (self.otpData == -1)) or
            ((self.command == RAM_DWNLD_CMD_OTP_WR_SEC_KEY) and (self.sec_key == None)) or
            ((self.command == RAM_DWNLD_CMD_OTP_WR_MAC_ADDR) and (self.wr_mac_addr == None)) or
            ((self.command == RAM_DWNLD_CMD_OTP_WR_MAC_ADDR) and (self.loc == None))):
            print("invalid argument for command: %d" %self.command)
            sys.exit(1)

# *****************************************************************************
otpProgrammer = OTPProgrammer()
otpProgrammer.parse(sys.argv[1:])
otpProgrammer.validateArgs()
otpProgrammer.execCmd()
sys.exit()
