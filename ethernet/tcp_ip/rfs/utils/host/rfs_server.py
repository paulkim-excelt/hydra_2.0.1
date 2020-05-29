import threading
import socket
import logging
import struct
import sys
import os.path
import getopt
import time
import signal
import sys

RFS_RETRY_TIMEOUT_IN_SEC        = 0.4
RFS_SERVER_RECV_TIMEOUT_IN_SEC  = 0.1

RFS_MAX_RETRY_COUNT   = 30
RFS_MAX_CONNECTIONS   = 16

RFS_TFTP_OPCODE_RRQ   = 1
RFS_TFTP_OPCODE_WRQ   = 2
RFS_TFTP_OPCODE_DATA  = 3
RFS_TFTP_OPCODE_ACK   = 4
RFS_TFTP_OPCODE_ERROR = 5
RFS_TFTP_MAX_PKT_SZ = 516
RFS_TFTP_DATA_SZ = 512

class RFS_Connection():
    def __init__(self, aRequestType = None, aFname = None, aClientIPPort = None):
        self.requestType = aRequestType
        self.fileName = aFname
        self.clientIP = aClientIPPort
        self.socketProcess = None
        self.isAlive = 1
        self.curRetryCount = 0
        self.thread = None

    def start(self, aRequestType, aFname, aClientIPPort):
        self.requestType = aRequestType
        self.fileName = aFname
        self.clientIP = aClientIPPort
        self.socketProcess = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socketProcess.settimeout(RFS_RETRY_TIMEOUT_IN_SEC)
        self.isAlive = 1
        if aRequestType == RFS_TFTP_OPCODE_RRQ:
            self.thread = threading.Thread(target=self.readRequestHandler, args=())
        elif aRequestType == RFS_TFTP_OPCODE_WRQ:
            self.thread = threading.Thread(target=self.writeRequestHandler, args=())

        if self.thread is not None:
            self.thread.start()

    def stop(self):
        self.socketProcess.settimeout(0)
        self.socketProcess.close()
        self.isAlive = 0

    def sendErrorPacket(self, errCode, errMsg):
        b = bytearray()
        b += struct.pack("<H", socket.htons(RFS_TFTP_OPCODE_ERROR))
        b += struct.pack("<H", socket.htons(errCode))
        b.extend(errMsg)
        b += struct.pack("<B", 0)
        self.socketProcess.sendto(b, self.clientIP)
        del b

    def readRequestHandler(self):
        # Pick the file name, offset and numbytes
        y = self.fileName.split('+')
        fileName = y[0]
        offset = int(y[1], 16)
        numBytes = int(y[2], 16)

        if os.path.isfile(fileName) == False:
            print fileName, "not found"
            # Send TFTP error packet
            self.sendErrorPacket(1, "File not found.")
            self.isAlive = 0
            return

        # Open the file for reading
        fp  = open(fileName, 'rb')
        fsize = os.path.getsize(fileName)
        # Check if read request (offset, numBytes) is valid
        if fsize < offset:
            print fileName, "Request bound exceeded"
            # Send TFTP error packet and kill thread
            self.sendErrorPacket( 2, "Access violation.")
            self.isAlive = 0
            return
        elif fsize < (offset + numBytes):
            numBytes = fsize - offset
            print "updated numBytes ", numBytes

        fp.seek(offset)
        fileData = fp.read(numBytes)
        blockNo = 1

        while (True):
            isLastPacket = 0
            # Send the file data to TFTP client
            if numBytes >= (blockNo - 1) * RFS_TFTP_DATA_SZ:
                b = bytearray()
                b += struct.pack("<H", socket.htons(RFS_TFTP_OPCODE_DATA))
                b += struct.pack("<H", socket.htons(blockNo))
                if numBytes == ((blockNo-1)*RFS_TFTP_DATA_SZ):
                    isLastPacket = 1
                elif numBytes > (((blockNo-1)*RFS_TFTP_DATA_SZ)+512):
                    b.extend(fileData[(blockNo-1)*RFS_TFTP_DATA_SZ:((blockNo-1)*RFS_TFTP_DATA_SZ)+512])
                elif numBytes < (((blockNo-1)*RFS_TFTP_DATA_SZ)+512):
                    b.extend(fileData[(blockNo-1)*RFS_TFTP_DATA_SZ:numBytes])
                    isLastPacket = 1
                else:
                    b.extend(fileData[(blockNo-1)*RFS_TFTP_DATA_SZ:numBytes])

                self.socketProcess.sendto(b, self.clientIP)
                del b

            # Wait for the ACK from the TFTP client
            try:
                data, addr = self.socketProcess.recvfrom(RFS_TFTP_MAX_PKT_SZ)
            except:
                # On timeout transmit the last packet
                if self.curRetryCount < RFS_MAX_RETRY_COUNT:
                    self.curRetryCount += 1
                    continue
                elif self.curRetryCount == RFS_MAX_RETRY_COUNT:
                    print "Maximum retry done, exiting"
                    break

            # On last Tx to client getting lost, we receive an old ACK from client. Resend last Tx packet again.
            if (RFS_TFTP_OPCODE_ACK == int(data[0:2].encode('hex'), 16)) and ((blockNo-1) == int(data[2:4].encode('hex'), 16)):
                continue
            # Send next data packet
            elif (RFS_TFTP_OPCODE_ACK == int(data[0:2].encode('hex'), 16)) and (blockNo == int(data[2:4].encode('hex'), 16)):
                self.curRetryCount = 0
                blockNo += 1
            # Abort connection if client reports error
            elif RFS_TFTP_OPCODE_ERROR == int(data[0:2].encode('hex'), 16):
                print "Received Error packet from client, closing connection."
                break
            else:
                print "Received invalid packet, closing connection."
                self.sendErrorPacket(4, "Illegal TFTP operation.")
                break

            if isLastPacket == 1:
                break
        # Close the file
        fp.close()
        self.isAlive = 0

    def writeRequestHandler(self):
        # Pick the file name, offset and numbytes
        y = self.fileName.split('+')
        fileName = y[0]
        offset = int(y[1], 16)
        numBytes = int(y[2], 16)

        # Open file for writing/appending based on specified offset
        if (offset == 0xFFFFFFFF) or (offset != 0x0):
            if os.path.isfile(fileName) == False:
                self.sendErrorPacket( 1, "File not found.")
                print "File to be appended does not exist ", fileName
                self.isAlive = 0
                return
            else:
                fp  = open(fileName, 'ab')
        elif offset == 0x0:
            fp  = open(fileName, 'wb')
            fsize = 0

        blockNo = 0
        islastPacket = 0

        while (True):
            # Send ACK for received data request or packet
            b = bytearray()
            b += struct.pack("<H", socket.htons(RFS_TFTP_OPCODE_ACK))
            b += struct.pack("<H", socket.htons(blockNo))
            self.socketProcess.sendto(b, self.clientIP)
            del b

            if islastPacket == 0:
                try:
                    data, addr = self.socketProcess.recvfrom(RFS_TFTP_MAX_PKT_SZ)
                except:
                    if self.curRetryCount < RFS_MAX_RETRY_COUNT:
                        self.curRetryCount += 1
                        continue
                    elif self.curRetryCount == RFS_MAX_RETRY_COUNT:
                        print "Maximum retry done, exiting"
                        break

                # When last ACK was missed and same data packet is received, resend last ACK again
                if (RFS_TFTP_OPCODE_DATA == int(data[0:2].encode('hex'), 16)) and (blockNo == int(data[2:4].encode('hex'), 16)):
                    continue
                # Check if ACK is got for the last transmitted write packet
                elif (RFS_TFTP_OPCODE_DATA == int(data[0:2].encode('hex'), 16)) and ((blockNo+1) == int(data[2:4].encode('hex'), 16)):
                    self.curRetryCount = 0
                    blockNo += 1
                elif RFS_TFTP_OPCODE_ERROR == int(data[0:2].encode('hex'), 16):
                    print "Received Error packet from client, closing connection."
                    break
                else:
                    self.sendErrorPacket(4, "Illegal TFTP operation.")
                    print "Block number of data packet is invalid"
                    break

                # Copy packet data to file
                fp.write(data[4:])
                if (len(data) != RFS_TFTP_MAX_PKT_SZ):
                    islastPacket = 1
            else:
                break;
        # Close the file
        fp.close()
        self.isAlive = 0

if __name__ == '__main__':
    freeClientList = []
    activeClientList = []
    isServerRun = 1
    # Create objects for all connections
    for i in range(RFS_MAX_CONNECTIONS):
        connection = RFS_Connection(None, None, None)
        freeClientList.append(connection)
    serverSock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    serverSock.settimeout(RFS_SERVER_RECV_TIMEOUT_IN_SEC)
    serverSock.bind(('', 69))

    def signal_handler(sig, frame):
        isServerRun = 0
        serverSock.settimeout(0)
        serverSock.close()
        for conn in activeClientList:
            conn.stop()
            conn.thread.join()
            activeClientList.remove(conn)
            freeClientList.append(conn)
        print('Application closed successfully')
        sys.exit(0)

    # register for interrupt
    signal.signal(signal.SIGINT, signal_handler)
    print('Press Ctrl+C to stop application')

    while isServerRun == 1:
        try:
            request, clientIP = serverSock.recvfrom(1024)
        except socket.timeout:
            pass
        else:
            requestType = int(request[0:2].encode('hex'), 16)
            x = request[2:].split('\0')
            fileName = x[0]
            mode = x[1]
            if mode != "octet":
                print mode,"is unsupported"
                continue
            elif RFS_TFTP_OPCODE_RRQ != requestType and  RFS_TFTP_OPCODE_WRQ != requestType:
                print "Invalid packet got at TFTP server port 69"
                continue

            if len(freeClientList) != 0:
                client = freeClientList.pop()
                activeClientList.append(client)
                client.start(requestType, fileName, clientIP)
            else:
                print "Maximum connection reached, ignoring current request from", client
        for conn in activeClientList:
            if conn.isAlive == 0:
                conn.stop()
                conn.thread.join()
                activeClientList.remove(conn)
                freeClientList.append(conn)
