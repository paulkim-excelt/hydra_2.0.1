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

# Imports
import sys
import os
import socket
import time

# Command layout
'''
CmdPktType layout
4 Bytes of hdr
4 Bytes of addr
1 Byte of data
'''

'''
CmdPktHdrType layout

4 bytes of Magic
2 Bytes of seqNo
2 Bytes of status
4 Bytes of totalTuples
CmdPktType cmdPkts[]
'''
# Constants
# Command IDs
CMD_ID_READ          = 0x0001
CMD_ID_WRITE         = 0x0002
CMD_ID_START         = 0x0003
CMD_ID_STOP          = 0x0004
CMD_ID_FRAME_CAPTURE = 0x0005
CMD_ID_FRAME_STATUS  = 0x0006
CMD_ID_GET_VERSION   = 0x0007
CMD_ID_SLICE_CAPTURE = 0x0008
CMD_ID_SLICE_STATUS  = 0x0009

# Device type
DEV_TYPE_IO          = 0x0001
DEV_TYPE_I2C         = 0x0002

# Command status
STATUS_SUCCESS       = 0x0001
STATUS_INVALID_CMD   = 0x0002
STATUS_UNKNOWN_ERR   = 0x0003
STATUS_BUSY          = 0x0004

# Magic
CMD_MAGIC_NUM  = 0x4252434D

# Masks and shifts for command header
CMD_MASK       = 0x3F
CMD_SHIFT      = 0
WIDTH_MASK     = 0xC0
WIDTH_SHIFT    = 6
DEV_TYPE_MASK  = 0x300
DEV_TYPE_SHIFT = 8
SLAVE_ID_MASK  = 0x1FFC00
SLAVE_ID_SHIFT = 10
LEN_MASK       = 0xFFE00000
LEN_SHIFT      = 21

# Data Width
WIDTH_8  = 0
WIDTH_16 = 1
WIDTH_32 = 2
WIDTH_64 = 3

# Results
RES_SUCCESS = 0
RES_CANCELLED = 1
RES_FAILED = 2

# Port details
BRCM_CMD_LISTEN_PORT = 58900
BRCM_CMD_HOST_IP = "192.168.10.1"
REMOTE_IP  = (BRCM_CMD_HOST_IP, BRCM_CMD_LISTEN_PORT)

# Maximum lenght per packet
MAX_LEN_PER_PACKET = 1400

# Frame capture type
FC_NONE = 0
FC_FRAME = 1
FC_SLICE = 2

# Frame capture state
FC_STATE_NONE = 0
FC_STATE_REQUESTED = 1
FC_STATE_CAPTURING = 2
FC_STATE_COMPLETED = 5

# Globals dicts
options_pair = {1 : 'Read', 2 : 'Write', 3 : 'Frame Capture', 4 : 'Enable Tuner', 5 : 'Disable Tuner', 6 : 'Version', 7 : 'Exit' }
dev_options_pair = {1 : 'IO', 2 : 'I2C', 3 : 'Back' }
tuner_options_pair = {1 : 'AWB', 2 : 'AGC'}
width_pair = {8 : WIDTH_8, 16 : WIDTH_16, 32 : WIDTH_32, 64 : WIDTH_64}
cmd_res_pair = {STATUS_SUCCESS : 'Success', STATUS_INVALID_CMD : 'Invalid command', STATUS_UNKNOWN_ERR : 'Unknown error', STATUS_BUSY : 'Busy'}
tuple_options_pair = {1 : 'Yes', 2 : 'No' }
cmd_id_pair = {CMD_ID_READ : 'READ', CMD_ID_WRITE : 'WRITE', CMD_ID_START : 'START', CMD_ID_STOP : 'STOP', CMD_ID_FRAME_CAPTURE : 'FRAME_CAPTURE', CMD_ID_FRAME_STATUS : 'FRAME_STATUS', CMD_ID_GET_VERSION : 'GET_VERSION', CMD_ID_SLICE_CAPTURE : 'SLICE_CAPTURE', CMD_ID_SLICE_STATUS : 'SLICE_STATUS'}

# Cmd class to manage user's requests
class Cmd :
    # Constructor
    def __init__(self):
        self.udp_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
        self.seq_num = 0
        self.fc_state = FC_STATE_NONE
        self.dump_file_name = ''
        self.y_addr = 0
        self.y_size = 0
        self.u_addr = 0
        self.u_size = 0
        self.v_addr = 0
        self.v_size = 0
        self.width = 1280
        self.height = 720
        self.format = '420P'
        self.mapfile = ''
        self.num_slices = 45

    # Construct command packer header
    def __construct_cmd_pkt_hdr(self) :
        # Create message
        # Add magic
        for counter in range(0, 4):
            self.buffer.append((CMD_MAGIC_NUM  >> (counter*8)) & 0xff)
        self.buffer_len += 4
        # Add sequence number
        for counter in range(0, 2):
            self.buffer.append((self.seq_num  >> (counter*8)) & 0xff)
        self.buffer_len += 2
        # Increment sequence number
        self.seq_num += 1
        # Skip status field
        for counter in range(0, 2):
            self.buffer.append(0)
        self.buffer_len += 2
        # total_tuples field - start with 0
        total_tuples = 0
        for counter in range(0, 4):
            self.buffer.append((total_tuples  >> (counter*8)) & 0xff)
        self.buffer_len += 4

    # Increment tuple count
    def __inc_tuple_count(self) :
        # Retreive tuple count
        tuple_count = 0
        for counter in range(0, 4):
            tuple_count |= self.buffer[8+counter] << (counter * 8)
        tuple_count += 1
        # Update
        for counter in range(0, 4):
            self.buffer[8+counter] = ((tuple_count  >> (counter*8)) & 0xff)

    # Construct command
    def __construct_cmd_pkt(self, command, dev_type, address, slave_id, width, num_ops, in_buffer, length) :
        # Form header
        cmd_pkt_hdr = (command | (width << WIDTH_SHIFT) | (dev_type << DEV_TYPE_SHIFT) | (slave_id << SLAVE_ID_SHIFT) | (num_ops << LEN_SHIFT))
        # Write pkt (1) Header
        for counter in range(0, 4):
            self.buffer.append((cmd_pkt_hdr  >> (counter*8)) & 0xff)
        self.buffer_len += 4
        # Write pkt addr
        for counter in range(0, 4):
            self.buffer.append((address  >> (counter*8)) & 0xff)
        self.buffer_len += 4
        # Write data bytes
        for counter in range(0, length):
            self.buffer.append(in_buffer[counter])
        self.buffer_len += length
        # Increment tuple count
        self.__inc_tuple_count()

    # Construct read command
    def __construct_read_cmd(self, dev_type, address, slave_id, width, num_reads) :
        in_buffer = bytearray();
        width_bits = 0
        for key in width_pair.iterkeys():
            if width_pair[key] == width :
                width_bits = key
        length = (width_bits/8) * num_reads
        for counter in range(0, length):
            in_buffer.append(0)
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_READ, dev_type, address, slave_id, width, num_reads, in_buffer, length)

    # Construct write command
    def __construct_write_cmd(self, dev_type, address, slave_id, width, num_writes) :
        in_buffer = bytearray();

        # Update the write data
        while 1 :
            print 'Enter the data(in hex) to write separated by space!'
            line = sys.stdin.readline().strip('\n')
            content_list = line.split()
            if len(content_list) < num_writes :
                print 'Not enough inputs, try again !'
            else :
                try :
                    for counter in range(0, num_writes) :
                        hex(int((content_list[counter]), 16))
                    break
                except :
                    print 'Invalid Input, Try again !'

        # Calculate width in bytes
        width_bytes = 0
        for key in width_pair.iterkeys():
            if width_pair[key] == width :
                width_bytes = key/8
        # Add elements to the list now
        for counter in range(0, num_writes) :
            data = int(content_list[counter], 16)
            for byte in range(0, width_bytes) :
                in_buffer.append((data >> ((width_bytes-byte-1) * 8)) & 0xff)
        length = width_bytes * num_writes
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_WRITE, dev_type, address, slave_id, width, num_writes, in_buffer, length)

    # Construct frame capture
    def __construct_frame_capture_cmd(self) :
        # Take inputs
        print 'Input: <yuv_file_name>'
        while 1 :
            line = sys.stdin.readline().strip('\n')
            try:
                content_list = line.split()
                if len(content_list) != 1 :
                    print 'Invalid Input, Try again !'
                    continue
                else :
                    break
            except KeyError:
                print 'Invalid Input, Try again !'
                continue

        # Save values
        self.dump_file_name = content_list[0]
        self.fc_state = FC_STATE_REQUESTED
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_FRAME_CAPTURE, 0, 0, 0, 0, 0, 0, 0)

    # Construct frame status
    def __construct_frame_status_cmd(self) :
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_FRAME_STATUS, 0, 0, 0, 0, 0, 0, 0)

    # Construct frame capture
    def __construct_enable_disable_tuner_cmd(self, enable) :
        ret = RES_SUCCESS
        # Take inputs
        print 'Select Tuner:'
        tuner = ''
        tuner_state_name = ''
        tuner_state_offset = 0
        for key in tuner_options_pair.keys():
            print (str(key) + ': ' + tuner_options_pair[key])
        while 1 :
            line = sys.stdin.readline().strip('\n')
            if line.isdigit() == False :
                print 'Invalid Input, Try again !'
            else :
                if int(line) in tuner_options_pair :
                    tuner = tuner_options_pair[int(line)]
                    break
                else :
                    print 'Invalid Input, Try again !'

        if tuner == 'AGC' :
            tuner_state_name = "gAGC_Config"
        else :
            tuner_state_name = "gAWB_Config"

        # Find offset
        try :
            # Read map file
            mapfile = open(self.mapfile, "r")
            for line in mapfile:
                if tuner_state_name in line:
                    content_list = line.split()
                    try :
                        int((content_list[0]), 16)
                        tuner_state_offset = int((content_list[0]), 16)
                        break
                    except :
                        continue
            mapfile.close()
        except :
            print 'Map file read failed'
            ret = RES_FAILED

        # construct cmd
        if ret == RES_SUCCESS :
            in_buffer = bytearray();
            in_buffer.append(enable)
            self.__construct_cmd_pkt(CMD_ID_WRITE, DEV_TYPE_IO, (tuner_state_offset+8), 0, WIDTH_8, 1, in_buffer, 1)
        else :
            print 'Tuner Enable/Disable failed !!!'

    # Construct get version
    def __construct_get_version_cmd(self) :
        in_buffer = bytearray(50);
        length = 50
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_GET_VERSION, 0, 0, 0, WIDTH_8, length, in_buffer, length)

    # Construct slice capture
    def __construct_slice_capture_cmd(self, slice_num) :
        in_buffer = bytearray();
        in_buffer.append(int(slice_num))
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_SLICE_CAPTURE, 0, 0, 0, WIDTH_8, 1, in_buffer, 1)

    # Construct slice status
    def __construct_slice_status_cmd(self) :
        # construct cmd
        self.__construct_cmd_pkt(CMD_ID_SLICE_STATUS, 0, 0, 0, 0, 0, 0, 0)

    # Constuct Read-Write
    def __construct_read_write_cmd(self, input) :
        ret = RES_SUCCESS
        slave_id = 0
        line = '\n'
        dev_type = 0
        device = ''

        print 'Select Device:'
        for key in dev_options_pair.keys():
            print (str(key) + ': ' + dev_options_pair[key])
        while 1 :
            line = sys.stdin.readline().strip('\n')
            if line.isdigit() == False :
                print 'Invalid Input, Try again !'
            else :
                if int(line) in dev_options_pair :
                    if dev_options_pair[int(line)] == 'Back' :
                        ret = RES_CANCELLED
                    else :
                        device = dev_options_pair[int(line)]
                    break
                else :
                    print 'Invalid Input, Try again !'

        if ret == RES_SUCCESS :
            if device == 'I2C' :
                while 1 :
                    print 'Input: <slave_id>'
                    line = sys.stdin.readline().strip('\n')
                    try :
                        slave_id = int(line, 16)
                        break
                    except :
                        print 'Invalid Input!'
                        continue
                dev_type = DEV_TYPE_I2C
            else :
                dev_type = DEV_TYPE_IO

            if input == 'Read' :
                print 'Input: <address_in_hex> <width> <number_of_reads>: or say Back'
            else :
                print 'Input: <address_in_hex> <width> <number_of_writes>: or say Back'

            while 1 :
                line = sys.stdin.readline().strip('\n')
                try:
                    content_list = line.split()
                    if content_list[0] == 'Back' :
                        ret = RES_CANCELLED
                        break
                    elif len(content_list) != 3 :
                        print 'Invalid Input, Try again !'
                        continue
                    else :
                        hex(int((content_list[0]), 16))
                        int(content_list[1])
                        int(content_list[2])
                        width_pair[int(content_list[1])]
                        break
                except KeyError:
                    print 'Invalid Input, Try again !'
                    continue

        if ret == RES_SUCCESS :
            content_list = line.split()
            print('Address: ' + hex(int((content_list[0]), 16)))
            print('Width  : ' + content_list[1])
            if input == 'Read' :
                print('Reads  : ' + content_list[2])
                self.__construct_read_cmd(dev_type, int((content_list[0]), 16), slave_id, width_pair[int(content_list[1])], int(content_list[2]))
            else :
                print('Writes  : ' + content_list[2])
                self.__construct_write_cmd(dev_type, int((content_list[0]), 16), slave_id, width_pair[int(content_list[1])], int(content_list[2]))
        return ret

    # Process input
    def __process_input_cmd(self, input) :
        ret = RES_SUCCESS

        if ((input == 'Read') or (input == 'Write')):
            ret = self.__construct_read_write_cmd(input)
        elif input == 'Frame Capture' :
            self.__construct_frame_capture_cmd()
        elif input == 'Enable Tuner' :
            self.__construct_enable_disable_tuner_cmd(1)
        elif input == 'Disable Tuner' :
            self.__construct_enable_disable_tuner_cmd(0)
        elif input == 'Version' :
            self.__construct_get_version_cmd()
        else :
            print 'We are going to support it!'
            ret = RES_FAILED

        return ret

    # Process read command output
    def __process_read_ouput(self, buffer, data_start_idx, width, length, addr) :
        # Print read data
        width_bits = 0
        for key in width_pair.iterkeys():
            if width_pair[key] == width :
                width_bits = key
        addr_inc = width_bits/8
        data_base = data_start_idx
        for counter in range(0, length):
            data_w1 = 0
            data_w2 = 0
            data = 0
            if width >= 0 :
                data_w1 = buffer[data_base]
                data_base += 1
                if width == 0 :
                    data = data_w1
            if width >= 1 :
                data_w1 |= (buffer[data_base] << 8)
                data_base += 1
                if width == 1 :
                    data = socket.ntohs(data_w1)
            if width >= 2 :
                data_w1 |= (buffer[data_base] << 16)
                data_base += 1
                data_w1 |= (buffer[data_base] << 24)
                data_base += 1
                if width == 2 :
                    data = socket.ntohl(data_w1)
            if width == 3 :
                data_w2 |= (buffer[data_base] << 0)
                data_base += 1
                data_w2 |= (buffer[data_base] << 8)
                data_base += 1
                data_w2 |= (buffer[data_base] << 16)
                data_base += 1
                data_w2 |= (buffer[data_base] << 24)
                data_base += 1
                data_w1 = socket.ntohl(data_w1)
                data_w2 = socket.ntohl(data_w2)
                data = data_w1 | (data_w2 << 32)

            print ('Data @' + hex(addr + addr_inc * counter) + ': ' + hex(data))

    # Process get version output
    def __process_get_version_output(self, buffer, data_start_idx, width, length, addr) :
        data_base = data_start_idx
        version = bytearray(50)
        for counter in range(0, 50) :
            version[counter] = buffer[data_base+counter]
        print ('Version: ' + version.decode())

    # Init
    def init(self):
        self.buffer = bytearray()
        self.buffer_len = 0
        self.__construct_cmd_pkt_hdr()

    # Setup
    def setup(self, width, height, format, mapfile, num_slices) :
        ret = RES_SUCCESS
        try :
            self.width = width
            self.height = height
            self.format = format
            self.mapfile = mapfile
            self.num_slices = num_slices
            self.udp_socket.bind(('', BRCM_CMD_LISTEN_PORT))
            self.udp_socket.settimeout(1)
        except socket.error, msg:
            print ('Socket bind failed, ' + str(msg))
            ret = RES_FAILED
        return ret

    # Teardown
    def teardown(self):
        ret = RES_SUCCESS
        try :
            self.udp_socket.close()
        except socket.error, msg:
            ret = RES_FAILED
        return ret

    # Send command packet
    def send(self) :
        ret = RES_SUCCESS
        try :
            self.udp_socket.sendto(self.buffer, REMOTE_IP)
        except socket.error, msg:
            ret = RES_FAILED
        return ret;

    # Receive command packet
    def recv(self, buffer) :
        ret = RES_SUCCESS
        # Copy self buffer to buffer
        buffer[:] = self.buffer[:]
        try :
            size, send_addr = self.udp_socket.recvfrom_into(buffer)
        except :
            print 'recv timeout'
            ret = RES_FAILED
        # print ('Received '+str(size)+' bytes from '+ str(send_addr))
        status = buffer[6] | (buffer[7] << 8)
        if status != STATUS_SUCCESS :
            # print ('Command status: ' + cmd_res_pair[status])
            ret = RES_FAILED
        return ret;

    # Check Frame Capture type
    def __check_fc_type(self) :
        type = FC_NONE
        # Delay for FC completion
        time.sleep(1)
        self.init()
        self.__construct_frame_status_cmd()
        while True :
            ret = self.send()
            if ret == RES_SUCCESS :
                buffer = bytearray(1024)
                # Receive Packet
                ret = self.recv(buffer)
                if ret == RES_SUCCESS :
                    type = FC_FRAME
                else :
                    type = FC_SLICE
                break
            else :
                print 'self.send() failed !, retrying'

        return type

    # Save YUV frame
    def __save_frame(self) :
        ret = RES_SUCCESS
        frame = bytearray()

        # Construct read packet for Y-U-V
        for channel in range(0, 3) :
            if channel == 0 :
                channel_size = self.y_size
                channel_addr = self.y_addr
            elif channel == 1 :
                channel_size = self.u_size
                channel_addr = self.u_addr
            else :
                channel_size = self.v_size
                channel_addr = self.v_addr

            num_pkts = channel_size/MAX_LEN_PER_PACKET;
            if (channel_size % MAX_LEN_PER_PACKET != 0) :
                num_pkts += 1
            size_remaining = channel_size
            read_address = channel_addr
            for i in range(0, num_pkts) :
                self.init()
                if size_remaining > MAX_LEN_PER_PACKET :
                    read_size = MAX_LEN_PER_PACKET
                else :
                    read_size = size_remaining
                in_buffer = bytearray(read_size)
                # Construct read packet
                self.__construct_cmd_pkt(CMD_ID_READ, DEV_TYPE_IO, read_address, 0, WIDTH_8, read_size, in_buffer, read_size)
                # Send
                while True :
                    ret = self.send()
                    if ret == RES_SUCCESS :
                        buffer = bytearray(4096)
                        # Receive Packet
                        ret = self.recv(buffer)
                        if ret == RES_SUCCESS :
                            for j in range(0, read_size) :
                                frame.append(buffer[20+j])
                            break
                        else :
                            print 'recv_cmd_pkt Failed!, retrying'
                # Add to byte array
                size_remaining -= read_size
                read_address += read_size
        # dump frame to file
        if ret == RES_SUCCESS :
            fo = open(self.dump_file_name, "wb")
            fo.write(frame)
            fo.close()
        else :
            print 'save_frame: failed !'

        return ret

    # Save YUV slices as frame
    def __save_slices(self) :
        ret = RES_SUCCESS
        yFrame = bytearray()
        uFrame = bytearray()
        vFrame = bytearray()

        # Construct read packet for Y-U-V
        for slice in range(0, self.num_slices) :
            # Trigger Slice capture for nth slice
            self.init()
            self.__construct_slice_capture_cmd(slice)
            while True :
                ret = self.send()
                if ret == RES_SUCCESS :
                    buffer = bytearray(1024)
                    # Receive Packet
                    ret = self.recv(buffer)
                    if ret == RES_SUCCESS :
                        break
                else :
                    print '__save_slices: self.send() failed !, retrying'

            # Check status of slice capture
            while True :
                self.init()
                self.__construct_slice_status_cmd()
                ret = self.send()
                if ret == RES_SUCCESS :
                    buffer = bytearray(1024)
                    # Receive Packet
                    ret = self.recv(buffer)
                else :
                    print '__save_slices: self.send() failed !, retrying'
                if ret == RES_SUCCESS :
                    break

            # Slice has been captured, save it now
            for channel in range(0, 3) :
                if channel == 0 :
                    channel_size = self.y_size
                    channel_addr = self.y_addr
                    frame = yFrame
                elif channel == 1 :
                    channel_size = self.u_size
                    channel_addr = self.u_addr
                    frame = uFrame
                else :
                    channel_size = self.v_size
                    channel_addr = self.v_addr
                    frame = vFrame

                if ret == RES_SUCCESS :
                    num_pkts = channel_size/MAX_LEN_PER_PACKET;
                    if (channel_size % MAX_LEN_PER_PACKET != 0) :
                        num_pkts += 1
                    size_remaining = channel_size
                    read_address = channel_addr
                    for i in range(0, num_pkts) :
                        self.init()
                        if size_remaining > MAX_LEN_PER_PACKET :
                            read_size = MAX_LEN_PER_PACKET
                        else :
                            read_size = size_remaining
                        in_buffer = bytearray(read_size)
                        # Construct read packet
                        self.__construct_cmd_pkt(CMD_ID_READ, DEV_TYPE_IO, read_address, 0, WIDTH_8, read_size, in_buffer, read_size)
                        # Send
                        while True :
                            ret = self.send()
                            if ret == RES_SUCCESS :
                                buffer = bytearray(4096)
                                # Receive Packet
                                ret = self.recv(buffer)
                                if ret == RES_SUCCESS :
                                    for j in range(0, read_size) :
                                        frame.append(buffer[20+j])
                                    break
                            else :
                                print '__save_slices: self.send() failed, retrying !'

                        # Add to byte array
                        size_remaining -= read_size
                        read_address += read_size
                        if ret != RES_SUCCESS :
                            break
                if ret != RES_SUCCESS :
                    break
            if ret != RES_SUCCESS :
                break
        # dump frame to file
        if ret == RES_SUCCESS :
            fo = open(self.dump_file_name, "wb")
            fo.write(yFrame)
            fo.write(uFrame)
            fo.write(vFrame)
            fo.close()
        else :
            print 'save_slices: failed !'

        return ret

    def add(self, input) :
        ret = self.__process_input_cmd(input)
        return ret

    # Process output
    def process(self, buffer) :
        ret = RES_SUCCESS
        # Retreive tuple count
        tuple_count = 0
        for counter in range(0, 4):
            tuple_count |= buffer[8+counter] << (counter * 8)
        buffer_pkt_offset = 12
        for i in range(0, tuple_count) :
            # Retreive required data
            hdr = 0
            for j in range(0, 4):
                hdr |= buffer[buffer_pkt_offset+j] << (j*8)
            cmd = (hdr & CMD_MASK) >> CMD_SHIFT
            width = (hdr & WIDTH_MASK) >> WIDTH_SHIFT
            length = (hdr & LEN_MASK) >> LEN_SHIFT
            addr = 0
            for j in range(0, 4):
                addr |= buffer[buffer_pkt_offset+4+j] << (j*8)
            data_start_idx = buffer_pkt_offset + 8
            # Process based on command
            print ('rxd: tuple : ' + str(i) + ', cmd: ' + cmd_id_pair[cmd])
            if cmd == CMD_ID_READ :
                self.__process_read_ouput(buffer, data_start_idx, width, length, addr)
            elif cmd == CMD_ID_WRITE :
                print 'Write command Succeeded!'
            elif cmd == CMD_ID_FRAME_CAPTURE :
                self.fc_state = FC_STATE_CAPTURING
            elif cmd == CMD_ID_GET_VERSION :
                self.__process_get_version_output(buffer, data_start_idx, width, length, addr)
            else :
                print 'Unsupported command in output'
                ret = RES_FAILED
            # Increment buffer_pkt_offset
            width_bytes = 0
            for key in width_pair.iterkeys():
                if width_pair[key] == width :
                    width_bytes = key/8
            buffer_pkt_offset += (8 + length * width_bytes)

        # Check if FC was started
        if self.fc_state == FC_STATE_CAPTURING :
            try :
                # Read map file
                mapfile = open(self.mapfile, "r")
                for line in mapfile:
                    if "CAMERA_YFrameGlobal" in line:
                        content_list = line.split()
                        y_frame_addr = int((content_list[0]), 16)

                    if "CAMERA_UFrameGlobal" in line:
                        content_list = line.split()
                        u_frame_addr = int((content_list[0]), 16)

                    if "CAMERA_VFrameGlobal" in line:
                        content_list = line.split()
                        v_frame_addr = int((content_list[0]), 16)

                    if "CAMERA_YSliceGlobal" in line:
                        content_list = line.split()
                        y_slice_addr = int((content_list[0]), 16)

                    if "CAMERA_USliceGlobal" in line:
                        content_list = line.split()
                        u_slice_addr = int((content_list[0]), 16)

                    if "CAMERA_VSliceGlobal" in line:
                        content_list = line.split()
                        v_slice_addr = int((content_list[0]), 16)
                mapfile.close()
            except :
                print 'Map file read failed'
                ret = RES_FAILED

            if self.format != '420P' :
                print 'Unsupported format for frame capture!'
                ret = RES_FAILED

            if ret == RES_SUCCESS :
                type = self.__check_fc_type()
                if type == FC_FRAME :
                    self.y_addr = y_frame_addr
                    self.u_addr = u_frame_addr
                    self.v_addr = v_frame_addr
                    self.y_size = self.width * self.height
                    self.u_size = (self.width * self.height)/4
                    self.v_size = (self.width * self.height)/4
                    ret = self.__save_frame()
                elif type == FC_SLICE :
                    self.y_addr = y_slice_addr
                    self.u_addr = u_slice_addr
                    self.v_addr = v_slice_addr
                    self.y_size = self.width * 16
                    self.u_size = (self.width * 16)/4
                    self.v_size = (self.width * 16)/4
                    ret = self.__save_slices()
                else :
                    ret = RES_FAILED

            if ret == RES_SUCCESS :
                print 'FC success!!!'
            else :
                print 'FC failed!!!'

        self.fc_state = FC_STATE_NONE
        return ret

# Main
def main():
    print 'Welcome to Camera Tuning !!!'
    tuple_count = 0
    cmd = Cmd()

    if len(sys.argv) == 5 :
        try :
            width = int(sys.argv[1])
            height = int(sys.argv[2])
            format = sys.argv[3]
            mapfile = sys.argv[4]
            num_slices = ((height + 15) & ~15)/16
            ret = cmd.setup(width, height, format, mapfile, num_slices)
        except :
            print 'Invalid command line args!'
            ret = RES_FAILED
    else :
        print 'Command line args missed! <width> <height> <format> <mapfile>'
        print 'Only (420P) format supported'
        ret = RES_FAILED

    if ret == RES_SUCCESS :
        while 1 :
            ret = RES_SUCCESS
            if tuple_count == 0 :
                cmd.init()
                print ('Adding first tuple')
            else :
                print ('Adding another tuple, no: ' + str(tuple_count+1))

            print 'Here are the options :'
            for key in options_pair.keys():
                print (str(key) + ': ' + options_pair[key])
            # Scan for input
            line = sys.stdin.readline().strip('\n')
            if line.isdigit() == False :
                print 'Invalid Input, Try again !'
                continue
            if int(line) in options_pair :
                print ('Selected ' + ' - ' + options_pair[int(line)] + '!')
                if options_pair[int(line)]  == 'Exit' :
                    break
                else :
                    ret = cmd.add(options_pair[int(line)])
                    if ret == RES_SUCCESS :
                        print 'Success'
                    elif ret == RES_CANCELLED :
                        print 'Cancelled'
                    else :
                        print 'Failed'
            else :
                print 'Invalid Input, Try again !'

            while (ret == RES_SUCCESS) :
                print 'Add tuple ?'
                for key in tuple_options_pair.keys():
                    print (str(key) + ': ' + tuple_options_pair[key])
                try :
                    # Scan for input
                    line = sys.stdin.readline().strip('\n')
                    if int(line) in tuple_options_pair :
                        print ('Selected ' + ' - ' + tuple_options_pair[int(line)] + '!')
                    else :
                        tuple_options_pair[int(line)]

                except KeyError :
                    print 'Invalid Input, Try again !'
                    continue

                if tuple_options_pair[int(line)]  == 'Yes' :
                    tuple_count += 1
                else :
                    print 'Sending constructed packet!'
                    buffer = bytearray(4096)
                    while True :
                        ret = cmd.send()
                        if ret == RES_SUCCESS :
                            # Receive Packet
                            ret = cmd.recv(buffer)
                            if ret == RES_SUCCESS :
                                break
                            else :
                                print 'cmd.recv() Failed!, retrying'
                        else :
                            print 'cmd.send() Failed!, retrying'
                    ret = cmd.process(buffer)
                    if ret != RES_SUCCESS :
                        print 'cmd.process() Failed!'
                    tuple_count = 0
                break

    ret = cmd.teardown()
    return ret

if __name__ == '__main__':
    try :
        main()
    except KeyboardInterrupt :
        print 'Interrupted'
        try :
            sys.exit(0)
        except SystemExit :
            os._exit(0)
