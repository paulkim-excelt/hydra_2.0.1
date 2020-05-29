#! python2
##############################################################################
# Copyright 2017-2019 Broadcom Limited.  All rights reserved.
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
from PyQt4.QtGui import *
from PyQt4.QtCore import pyqtSlot,SIGNAL,SLOT,QThread
from PyQt4.QtCore import *
from ctypes import *
from shutil import copyfile

import zlib
import struct
import binascii
import tarfile
import threading
import netifaces
import socket
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

import ConfigParser

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

COMMAND_STATUS_PORT = 58900
TFTP_PORT = 58901

TFTP_TRANSFER_SUCCESS = 0
TFTP_CONNECTION_TIMEOUT = 1
TFTP_TRANSFER_TIMEOUT = 2
TFTP_TRANSFER_ERROR = 3

PERCENTAGE_TFTP = 20
PERCENTAGE_FLASH = 80

OPERATION_NONE=0
OPERATION_ERASE=1
OPERATION_FLASH=2

FLASHER_EXIT = 0
FLASHER_REFRESH_IP_COMMAND = 1
FLASHER_ERASE_COMMAND = 2
FLASHER_FLASH_COMMAND = 3

FLASH_ALL = 1
FLASH_BL_CONFIG = 2
FLASH_CONFIG = 3
FLASH_CUSTOM = 4

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
class RAMClient(QObject):
    def __init__(self):
        super(RAMClient, self).__init__()
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
    def openSession(self, dev = None):
        if dev != None:
            self.iJLink.JLINKARM_EMU_SelectByUSBSN(dev)
        self.iJLink.JLINKARM_Open()
    def closeSession(self):
        self.iJLink.JLINKARM_Close()
    def cleanUp(self):
        self.closeSession()
    def listDevices(self, trigger):
        if self.iJLink.JLINKARM_EMU_GetNumDevices() > 0:
            self.openSession()
            if (self.iJLink.JLINKARM_GetSN() >= 0):
                print str(self.iJLink.JLINKARM_GetSN())
                if trigger != None:
                    trigger.emit(str(self.iJLink.JLINKARM_GetSN()))
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
    def erase(self, msg):
        folder = None
        try:
            zip = zipfile.ZipFile(msg['filename'])
            folder = tempfile.mkdtemp()
            print folder
            zip.extractall(folder)
            zip.close()
            zip = None
        except:
            print 'Erase: File not found:'+ str(msg['filename'])
            return 1
        error = 0
        trigger = msg['trigger']
        self.openSession(msg['device'])
        if self.enterDwnldMode(folder, 0x9) == 1:
            self.iSeqNum = 1 - self.iSeqNum
            flag = (msg['type'] | (self.iSeqNum << HOST_SEQNUM_SHIFT) | (2 << CMD_SHIFT))
            self.iJLink.JLINKARM_WriteU16(self.iHostAddr, flag)
            timeout = 10
            while timeout:
                time.sleep(0.1)
                self.iJLink.JLINKARM_ReadMemU16(self.iTargetAddr, 1, byref(self.iRegVal), None)
                percent = ((self.iRegVal.value & PERCENT_MASK) >> PERCENT_SHIFT)
                target_seq = ((self.iRegVal.value & TARGET_SEQNUM_MASK) >> TARGET_SEQNUM_SHIFT)
                if not ((target_seq != self.iSeqNum) and (percent == 100)):
                    if (trigger != None):
                        trigger.emit(OPERATION_ERASE, percent, -1)
                else:
                    timeout = timeout - 1
                if (target_seq == self.iSeqNum):
                    error = 0
                    break
            if ((self.iRegVal.value & STATUS_MASK) == STATUS_MASK):
                error = 1
                print 'Erasing of %s failed' % file
            else:
                print 'Erasing done'
        else:
            error = 1
        self.closeSession()
        shutil.rmtree(folder)
        return error

    def update(self, msg, folder):
        error = 1
        flashID = msg['type']
        trigger = msg['trigger']
        self.openSession(msg['device'])
        if self.enterDwnldMode(folder, 0x9) == 1:
            print 'loading %s at 0x%x' % (folder+'/'+'temp.bin', self.iLoadAddr)
            self.iSeqNum = 1 - self.iSeqNum
            flag = (flashID | (self.iSeqNum << HOST_SEQNUM_SHIFT) | (0 << CMD_SHIFT))
            self.iStdCalliJLink.JLINK_DownloadFile(folder+'/'+'temp.bin', self.iLoadAddr)
            self.iJLink.JLINKARM_WriteU16(self.iHostAddr, flag)
            timeout = 10
            while timeout:
                time.sleep(0.1)
                self.iJLink.JLINKARM_ReadMemU16(self.iTargetAddr, 1, byref(self.iRegVal), None)
                percent = ((self.iRegVal.value & PERCENT_MASK) >> PERCENT_SHIFT)
                target_seq = ((self.iRegVal.value & TARGET_SEQNUM_MASK) >> TARGET_SEQNUM_SHIFT)
                if not ((target_seq != self.iSeqNum) and (percent == 100)):
                    if (trigger != None):
                        trigger.emit(OPERATION_FLASH, percent, -1)
                else:
                    timeout = timeout - 1
                if (target_seq == self.iSeqNum):
                    break
            if ((self.iRegVal.value & STATUS_MASK) == STATUS_MASK):
                error = 1
                print 'Flashing of %s failed' % file
            else:
                error = 0
                print 'Flashing done'
        else:
            error = 1
        self.closeSession()
        return error

def generateBLCmd(cmd):
    key = bytearray()
    key.append(0x4D)
    key.append(0x43)
    key.append(0x52)
    key.append(0x42)
    key.append(cmd)
    key.append(0x00)
    key.append(0x00)
    key.append(0x00)
    return key

def processSocketData(recvSocket, op, timeout, trigger):
    recvSocket.settimeout(timeout)
    while 1:
        try:
            data, addr = recvSocket.recvfrom(1024) # buffer size is 1024 bytes
        except:
            return 1
        b = bytearray()
        b.extend(data)
        if ((b[0]==0x4D) and (b[1]==0x43) and (b[2]==0x52) and (b[3]==0x42)):
            cmd = b[5]*256 + b[4]
            if (cmd == 0):
                if (trigger != None):
                    trigger.emit(str(list(addr)[0]))
                print str(list(addr)[0])
            elif (cmd == 1):
                status = b[7]*256 + b[6]
                if (len(b) > 8):
                    if op == OPERATION_FLASH:
                        percent = PERCENTAGE_TFTP + (b[8]*PERCENTAGE_FLASH/100)
                        if trigger != None:
                            try:
                                trigger.emit(op, percent, -1)
                            except:
                                pass
                    else:
                        if trigger != None:
                            try:
                                trigger.emit(op, b[8], -1)
                            except :
                                pass
                if (status == 1):
                    return 0
                elif (status != 4):
                    return 1
            elif (cmd == 2):
                return 0
            else:
                print 'ResponseThread unknown message cmd(' + str(cmd) + ') from ' + str(addr)
                return 1


class TFTPClient(QObject):
    def __init__(self):
        super(TFTPClient, self).__init__()
        self.iSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.iSocket.bind(('', TFTP_PORT))
        self.iSocket.settimeout(1)

    def sendData(self, fd, addr, block, type):
        fd.seek((block-1)*512)
        buffer = bytearray()
        buffer.append(0)
        buffer.append(3)
        buffer.append(int(block/256))
        buffer.append(block%256)
        s = fd.read(512)
        buffer.extend(s)
        if block == 1:
            if type == 1:
                buffer[12] = buffer[12] | 4
        self.iSocket.sendto(buffer,addr)
        if (len(s) < 512):
            self.iLast = block

    def sendFile(self, ip, folder,file, type, trigger):
        if (tarfile.is_tarfile(folder + '/' + file) == True):
            downloadImage = DownloadImage(folder + '/' + file, folder+'/'+'temp.bin')
            downloadImage.writeImage(type, EDC_CRC)
            file = 'temp.bin'
        nblocks = (os.stat(folder+'/'+file).st_size + 511)/512
        fd = open(folder+'/'+file,'rb')
        key = bytearray()
        key.append(0x00)
        key.append(0x02)
        key.append('f')
        key.append('i')
        key.append('l')
        key.append(0x00)
        key.append('o')
        key.append('c')
        key.append('t')
        key.append('e')
        key.append('t')
        key.append(0x00)
        self.iLast = -1
        self.iSocket.sendto(key, (ip, 69))
        connected = 0
        retry = 0
        ack = 0
        remoteAddr = None
        while 1:
            try:
                exception_occured = 1
                data, addr = self.iSocket.recvfrom(1024) # buffer size is 1024 bytes
                exception_occured = 0
            except socket.timeout:
                if connected == 0:
                    return TFTP_CONNECTION_TIMEOUT
                else:
                    if (retry < 4):
                        retry = retry + 1
                        self.sendData(fd, remoteAddr, ack + 1, type)
                    else:
                        return TFTP_TRANSFER_TIMEOUT
            except:
                return TFTP_TRANSFER_ERROR
            if (exception_occured == 0):
                b = bytearray()
                b.extend(data)
                op = b[0]*256 + b[1]
                if op == 5 :
                    error_code = b[2]*256 + b[3]
                    print 'error ' + str(error_code)
                    print data
                    return TFTP_TRANSFER_ERROR
                elif op == 4:
                    connected = 1
                    remoteAddr = addr
                    ack = b[2]*256 + b[3]
                    percent = int(ack*PERCENTAGE_TFTP/nblocks)
                    if trigger != None:
                        trigger.emit(OPERATION_FLASH, percent, -1)
                    if (ack == self.iLast):
                        fd.close()
                        return TFTP_TRANSFER_SUCCESS
                    else:
                        self.sendData(fd, remoteAddr, ack + 1, type)

    def erase(self, msg, socket):
        key = generateBLCmd(2)
        key.append(msg['type'])
        socket.sendto(key, (msg['device'], COMMAND_STATUS_PORT))
        error = processSocketData(socket, OPERATION_ERASE, 1, msg['trigger'])
        return error

    def update(self, msg, folder, socket):
        error = 0
        if (TFTP_TRANSFER_SUCCESS == self.sendFile(msg['device'], folder , 'temp.bin', msg['type'], msg['trigger'])):
            if processSocketData(socket, OPERATION_FLASH, 60, msg['trigger']) == 1:
                error = 1
        else:
            error = 1
        return error

class BCM8910X_FlasherBackend(QThread):
    def __init__(self):
        super(BCM8910X_FlasherBackend, self).__init__()
        self.iFileName = None
        self.iFiles = []
        self.iTarFile = None
        self.iCommandQueue = Queue.Queue()
        self.iResponseQueue = Queue.Queue()
        self.iProcessSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.iProcessSocket.bind(('', COMMAND_STATUS_PORT))
        self.iTFTPClient = TFTPClient()
        self.iRAMClient = RAMClient()
        self.iProcessThread = threading.Thread(None, self)
    def cleanUp(self):
        self.iRAMClient.cleanUp()
    def start(self):
        self.iProcessThread.start()
    def stop(self):
        msg = {}
        msg['command'] = FLASHER_EXIT
        msg['trigger'] = None
        self.iCommandQueue.put(msg)
    def refresh(self, trigger=None):
        msg = {}
        msg['command'] = FLASHER_REFRESH_IP_COMMAND
        msg['trigger'] = trigger
        self.iCommandQueue.put(msg)
    def erase(self, device, type, trigger=None):
        msg = {}
        msg['command'] = FLASHER_ERASE_COMMAND
        msg['trigger'] = trigger
        msg['device'] = device
        msg['filename'] = self.iFileName
        msg['type'] = type
        self.iCommandQueue.put(msg)
    def update(self, device, type, mode, selectedFile = None, trigger=None):
        msg = {}
        msg['command'] = FLASHER_FLASH_COMMAND
        msg['trigger'] = trigger
        msg['device'] = device
        msg['filename'] = self.iFileName
        msg['type'] = type
        msg['file'] = selectedFile
        msg['mode'] = mode
        self.iCommandQueue.put(msg)
    def updateFile(self, filename):
        self.iFileName = filename
        try:
            tmpFolder = tempfile.mkdtemp()
            zip = zipfile.ZipFile(filename)
            self.iTarFile = [each for each in zip.namelist() if each.endswith('.tar')]
            zip.extract(self.iTarFile[0], tmpFolder)
            zip.extract(self.iTarFile[0] + '.cfg', tmpFolder)
            tar = tarfile.open(tmpFolder + '/' + self.iTarFile[0])
            self.iFiles = tar.getnames()
            config = ConfigParser.RawConfigParser()
            config.read(tmpFolder + '/' + self.iTarFile[0] + '.cfg')
            for (idx,file) in enumerate(self.iFiles):
                self.iFiles[idx] = file[:-4] + ':' + config.get('File_Links',file[:-4])
            tar.close()
            zip.close()
            zip = None
            shutil.rmtree(tmpFolder)
        except Exception as err:
            print err
            return None
        print self.iFiles
        return self.iFiles
    def getFileList(self):
        return self.iFiles

    def processErase(self, msg):
        trigger = msg['trigger']
        if trigger != None:
            trigger.emit(OPERATION_ERASE, 0, -1)
        if ip_prefix not in msg['device']: # RAM Device
            error = self.iRAMClient.erase(msg)
        else:
            error = self.iTFTPClient.erase(msg, self.iProcessSocket)
        if trigger != None:
            trigger.emit(OPERATION_ERASE, 100, error)
        return error

    def processUpdate(self, msg):
        folder = None
        trigger = msg['trigger']
        mode = msg['mode']
        if trigger != None:
            trigger.emit(OPERATION_FLASH, 0, -1)
        try:
            zip = zipfile.ZipFile(msg['filename'])
            folder = tempfile.mkdtemp()
            print folder
            zip.extractall(folder)
            zip.close()
            zip = None
        except:
            print 'File not found:'+ str(msg['filename'])
            return 1
        inTar = tarfile.open(folder+'/'+ self.iTarFile[0], 'r')
        inTarConfig = ConfigParser.RawConfigParser()
        inTarConfig.read(folder + '/' + self.iTarFile[0] + '.cfg')
        outTar = tarfile.open(folder+'/'+'temp.tar', 'w')
        inTar.extractall(folder)
        for each in inTar.getnames():
            actFileName = inTarConfig.get('File_Links',each[:-4])
            if mode == FLASH_ALL:
                outTar.add(folder + '/' + each, each)
            elif mode == FLASH_BL_CONFIG:
                if (actFileName.endswith('erika.text.bin') == False) and \
                (actFileName.endswith('erika.data.privileged.bin') == False) and \
                (actFileName.endswith('erika.data.bin') == False):
                    outTar.add(folder + '/' + each, each)
            elif mode == FLASH_CONFIG:
                if (actFileName.endswith('bl.bin') == False) and \
                (actFileName.endswith('erika.text.bin') == False) and \
                (actFileName.endswith('erika.data.privileged.bin') == False) and \
                (actFileName.endswith('erika.data.bin') == False):
                    outTar.add(folder + '/' + each, each)
            else:
                if (each == msg['file']):
                    outTar.add(folder + '/' + each, each)
        outTar.close()
        inTar.close()
        downloadImage = DownloadImage(folder + '/' + 'temp.tar', folder+'/'+'temp.bin')
        downloadImage.writeImage(msg['type'], EDC_CRC)
        if ip_prefix not in msg['device']: # RAM Device
            error = self.iRAMClient.update(msg, folder)
        else:
            error = self.iTFTPClient.update(msg, folder, self.iProcessSocket)
        if trigger != None:
            trigger.emit(OPERATION_FLASH, 100, error)
        shutil.rmtree(folder)
        return error

    def processRefresh(self, trigger):
        print 'Finding Devices'
        self.iRAMClient.listDevices(trigger)
        key = generateBLCmd(0)
        for i in range(15,0,-1):
            for j in range(len(ip_search_range)):
               self.iProcessSocket.sendto(key, (ip_prefix + '.' + ip_search_range[j] + '.' + str(i), COMMAND_STATUS_PORT))
        processSocketData(self.iProcessSocket, OPERATION_NONE, 1, trigger)
        print 'Finding Devices Complete'

    def __call__(self):
        while 1:
            msg = self.iCommandQueue.get()
            cmd = msg['command']
            trigger = msg['trigger']
            resp = {}
            error = 1
            if (cmd == FLASHER_EXIT):
                resp['status'] = 0
                self.iResponseQueue.put(resp)
                break
            elif (cmd == FLASHER_REFRESH_IP_COMMAND):
                self.processRefresh(trigger)
                error = 0
            elif (cmd == FLASHER_ERASE_COMMAND):
                print 'Erase command'
                self.processErase(msg)
                print 'Erase command complete'
                #self.deviceRefresh(trigger)
            elif (cmd == FLASHER_FLASH_COMMAND):
                print 'Flash command'
                self.processUpdate(msg)
                print 'Flash command complete'
                #self.deviceRefresh(trigger)
            else:
                print 'Unknown command'
            resp['status'] = error
            self.iResponseQueue.put(resp)

class FlaserComboBox(QComboBox):
    def __init__(self, window, font, width, height, xoff, yoff):
        super(FlaserComboBox, self).__init__(window)
        super(FlaserComboBox, self).setDuplicatesEnabled(False)
        self.setFixedWidth(width)
        self.setFixedHeight(height)
        self.move(xoff,yoff)
        self.setFont(font)

class FlasherButton(QPushButton):
    def __init__(self, name, window, font, width, height, xoff, yoff):
        super(FlasherButton, self).__init__(name, window)
        self.setFixedWidth(width)
        self.setFixedHeight(height)
        self.move(xoff,yoff)
        self.setFont(font)
        self.setEnabled(False)

class FlasherLabel(QLabel):
    def __init__(self, name, window, font, width, height, xoff, yoff):
        super(FlasherLabel, self).__init__(name, window)
        self.setFixedWidth(width)
        self.setFixedHeight(height)
        self.move(xoff,yoff)
        self.setFont(font)
        self.setEnabled(False)

class FlasherProgressBar(QProgressBar):
    def __init__(self, window, font, width, height, xoff, yoff):
        super(FlasherProgressBar, self).__init__(window)
        self.setFixedWidth(width)
        self.setFixedHeight(height)
        self.move(xoff,yoff)
        self.setValue(0)
        self.setTextVisible(False)
        self.setFont(font)
        self.iValue = 0
        self.iLabel = QLabel(window)
        self.iLabel.setFont(font)
        self.iLabel.setAlignment(Qt.AlignCenter)
        self.iLabel.setFixedWidth(width)
        self.iLabel.setFixedHeight(height)
        self.iLabel.move(xoff,yoff)

    def setText(self, name):
        self.iLabel.setText(name)

class FlasherMainWindow(QMainWindow):
    def __init__(self, flasher, width, height, title):
        super(FlasherMainWindow, self).__init__()
        self.setFixedSize(width, height)
        self.setWindowTitle(title)
        self.iFlasher = flasher

    def closeEvent(self, evnt):
        print 'closeEvent'
        self.iFlasher.iBackend.stop()
        super(FlasherMainWindow, self).closeEvent(evnt)

class BCM8910X_FlasherUI(QObject):
    iUpdateTrigger = pyqtSignal(int,int,int)
    iAddDeviceTrigger = pyqtSignal(str)
    def __init__(self, backend):
        super(BCM8910X_FlasherUI, self).__init__()
        self.iBackend = backend
        self.iApp = QApplication(sys.argv)
        font = QFont()
        font.setPointSize(10)
        self.iWindow = FlasherMainWindow(self, 640, 480, 'BCM8910X-Flasher')

        self.iFlashFileButton = FlasherButton('File', self.iWindow, font, 110, 40, 510, 10)
        self.iFlashFileButton.setEnabled(True)
        self.iFlashFileButton.connect(self.iFlashFileButton, SIGNAL("clicked()"), self, SLOT("fileButtonClick()"))
        self.iFileName = FlasherLabel('No file selected', self.iWindow, font, 460, 40, 20, 10)

        self.iRefreshButton = FlasherButton('Refresh Devices', self.iWindow, font, 220, 40, 400, 60)
        self.iRefreshButton.setEnabled(True)
        self.iRefreshButton.connect(self.iRefreshButton, SIGNAL("clicked()"), self, SLOT("refreshDeviceList()"))

        self.iDeviceAddressCombo = FlaserComboBox(self.iWindow, font, 220, 40, 400, 100)
        self.iDeviceAddressCombo.connect(self.iDeviceAddressCombo, SIGNAL("currentIndexChanged(int)"), self, SLOT("deviceChanged()"))

        self.iDeviceLabel = FlasherLabel('Device Configuration', self.iWindow, font, 220, 40, 400, 180)
        self.iFlashTypeCombo = FlaserComboBox(self.iWindow, font, 110, 40, 400, 220)
        self.iFlashTypeCombo.addItem('Internal Flash')
        self.iFlashTypeCombo.addItem('External Flash')
        self.iTypeCombo = FlaserComboBox(self.iWindow, font, 100, 40, 520, 220)
        self.iTypeCombo.addItem('All')
        self.iTypeCombo.addItem('BL+Config')
        self.iTypeCombo.addItem('Config')
        self.iTypeCombo.addItem('Custom')
        """
        self.iDumpTypeCombo = FlaserComboBox(self.iWindow, font, 110, 40, 400, 300)
        self.iDumpTypeCombo.addItem('SRAM')
        self.iDumpTypeCombo.addItem('DRAM')
        self.iDumpSaveButton = FlasherButton('Save', self.iWindow, font, 110, 40, 510, 300)
        self.iDumpSaveButton.connect(self.iDumpSaveButton, SIGNAL("clicked()"), self, SLOT("saveButtonClick()"))
        """
        self.iOperationLabel = FlasherLabel('Device Update', self.iWindow, font, 220, 40, 400, 340)
        self.iEraseButton = FlasherButton('Erase', self.iWindow, font, 110, 40, 400, 380)
        self.iEraseButton.connect(self.iEraseButton, SIGNAL("clicked()"), self, SLOT("eraseButtonClick()"))
        self.iFlashButton = FlasherButton('Flash', self.iWindow, font, 100, 40, 520, 380)
        self.iFlashButton.connect(self.iFlashButton, SIGNAL("clicked()"), self, SLOT("flashButtonClick()"))

        self.iFileList = QListWidget(self.iWindow)
        self.iFileList.setFixedWidth(360)
        self.iFileList.setFixedHeight(340)
        self.iFileList.move(20,60)

        self.iOverallProgressBar = FlasherProgressBar(self.iWindow, font, 360, 40, 20, 420)
        self.iOverallProgressBar.setText('Overall Progress')
        self.iOverallProgressBar.setValue(0)

        self.iUpdateTrigger.connect(self.statusUpdate)
        self.iAddDeviceTrigger.connect(self.addDevice)

    def statusUpdate(self, operation, overallp, result = -1):
        self.iOverallProgressBar.setValue(overallp)
        if operation == OPERATION_ERASE:
            name = 'Erase '
        elif operation == OPERATION_FLASH:
            name = 'Flash '
        else:
            name = 'None'
        if result == 0:
            self.iOverallProgressBar.setText(name + 'Success')
        elif result == 1:
            self.iOverallProgressBar.setText(name + 'Failed')
        else:
            self.iOverallProgressBar.setText(name + 'in Progress')

    def disableButtons(self):
        self.iEraseButton.setEnabled(False)
        self.iFlashButton.setEnabled(False)
        self.iFlashFileButton.setEnabled(False)

    def addDevice(self, addr):
        if -1 == self.iDeviceAddressCombo.findText(addr):
            print 'adding '+ addr
            self.iDeviceAddressCombo.addItem(addr)

    @pyqtSlot()
    def refreshDeviceList(self):
        print 'refreshDeviceList'
        self.iDeviceAddressCombo.clear()
        self.iBackend.refresh(self.iAddDeviceTrigger)

    @pyqtSlot()
    def deviceChanged(self):
        print 'deviceChanged: ' + self.iDeviceAddressCombo.currentText()
        if not str(self.iDeviceAddressCombo.currentText()):
            print 'Disable Erase Button'
            self.iEraseButton.setEnabled(False)
            self.iFlashButton.setEnabled(False)
            self.iFlashFileButton.setEnabled(False)
        else:
            self.iEraseButton.setEnabled(False)
            if ip_prefix in self.iDeviceAddressCombo.currentText():
                self.iEraseButton.setEnabled(True)
            self.iFlashFileButton.setEnabled(True)
            if (len(self.iBackend.getFileList()) != 0):
                self.iFlashButton.setEnabled(True)
                self.iEraseButton.setEnabled(True)

    @pyqtSlot()
    def fileButtonClick(self):
        filename = str(QFileDialog.getOpenFileName(None, 'Open file', '.', "Image files (*.zip)"))
        print filename
        self.iFileName.clear()
        self.iFileName.setText(filename)
        files = self.iBackend.updateFile(filename)
        self.iFileList.clear()
        self.iFileList.addItems(files)
        if (len(files) == 0):
            self.iFlashButton.setEnabled(False)
        else:
            self.iFlashButton.setEnabled(True)
            self.iEraseButton.setEnabled(True)
    """
    @pyqtSlot()
    def saveButtonClick(self):
        filename = str(QFileDialog.getSaveFileName(None, 'Save file', '.', "Ramdump files (*.bin)"))
        print filename
        print 'Not Supported'
    """
    @pyqtSlot()
    def eraseButtonClick(self):
        self.disableButtons()
        if self.iFlashTypeCombo.currentText() == 'Internal Flash':
            self.iBackend.erase(str(self.iDeviceAddressCombo.currentText()), 0, self.iUpdateTrigger)
        else:
            self.iBackend.erase(str(self.iDeviceAddressCombo.currentText()), 1, self.iUpdateTrigger)

    @pyqtSlot()
    def flashButtonClick(self):
        print 'flashButtonClick'
        self.disableButtons()
        if self.iFlashTypeCombo.currentText() == 'Internal Flash':
            type = 0
        else:
            type = 1
        mode = FLASH_ALL
        selectedFile = None
        if self.iTypeCombo.currentText() == 'All':
            mode = FLASH_ALL
        elif self.iTypeCombo.currentText() == 'BL+Config':
            mode = FLASH_BL_CONFIG
        elif self.iTypeCombo.currentText() == 'Config':
            mode = FLASH_CONFIG
        else:
            mode = FLASH_CUSTOM
            selectedFile = self.iFileList.currentItem().text()
            selectedFile = selectedFile.split(':')[0] + '.bin'
        self.iBackend.update(str(self.iDeviceAddressCombo.currentText()), type, mode, selectedFile, self.iUpdateTrigger)

    def execute(self):
        self.iWindow.show()
        self.iBackend.start()
        self.iApp.exec_()

def backend_cleanup():
    if (backend != None):
        backend.cleanUp()


def usage():
    print '\'flasher.py -l \' => List flashable devices '
    print '\'flasher.py -d <device> -e 0 \' => Erase Internal Flash '
    print '\'flasher.py -d <device> -f 0 -i <zip_file> \' => Internal Flash '
    sys.exit(2)

atexit.register(backend_cleanup)
try:
    os.environ["BRCM_FLASHER_SEGGER_PATH"]
except KeyError:
    print "Please set the environment variable BRCM_FLASHER_SEGGER_PATH"
    sys.exit()
backend = BCM8910X_FlasherBackend()
try:
    opts, args = getopt.getopt(sys.argv[1:],"i:d:e:f:l")
except getopt.GetoptError:
    print 'exception'
    usage()

if len(opts) == 0:
    flasher = BCM8910X_FlasherUI(backend)
    flasher.execute()
else:
    command = FLASHER_EXIT
    internal = 0
    input = None
    for opt, arg in opts:
        if opt == "-h":
            usage()
        elif opt == "-i":
            input = arg
        elif opt == "-d":
            device = arg
        elif opt == "-e":
            command = FLASHER_ERASE_COMMAND
            internal = int(arg)
        elif opt == "-f":
            command = FLASHER_FLASH_COMMAND
            internal = int(arg)
        elif opt == "-l":
            command = FLASHER_REFRESH_IP_COMMAND
        else:
            print("Unknown argument %s" % opt)
            usage()
    backend.start()
    if (command == FLASHER_FLASH_COMMAND):
        backend.updateFile(input)
        backend.update(device, internal, FLASH_ALL)
    elif (command == FLASHER_ERASE_COMMAND):
        backend.updateFile(input)
        backend.erase(device, internal)
    elif (command == FLASHER_REFRESH_IP_COMMAND):
        backend.refresh()
    backend.stop()
    resp = backend.iResponseQueue.get()
    print 'result = ' + str(resp['status'])

sys.exit()
