/*****************************************************************************
Copyright 2019 Broadcom Limited.  All rights reserved.

This program is the proprietary software of Broadcom Corporation and/or its
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
******************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SERVERPORT                    (5000)
#define CLIENTPORT                    (5000)
#define MAX_UDP_PAYLOAD_SIZE          (64000UL)
#define MAX_NUM_NAL_PER_PAYLOAD       (5UL)
#define RTP_PADDING                   (0U)
#define RTP_EXTENSION_HDR             (0U)
#define RTP_PAYLOAD_TYPE              (96U)
#define RTP_CSRC_COUNT                (0U)
#define FU_A_PACKET_TYPE              (28U)
#define FRAGMENTATION_PAYLOAD_LEN     (1386UL)
#define RTP_VERSION                   (2U)

/**
 * The RTP Payload data used for RTP Header addition
 */
typedef struct _RTPPayloadInfo {
    /** Pointer to H264 Payload data */
    uint8_t *data;

    /** Length of H264 Payload */
    uint32_t dataLen;

    /** Frame Number applicable to H264 Payload */
    uint32_t frameNum;

    /** Is this start of a new H264 Frame */
    uint32_t isNewFrame;

    /** Is this start of a new H264 Frame */
    uint32_t isLastNalOfFrame;

    /** Is Metadata - SPS/PPS */
    uint32_t isMetaData;

    /** Payload Format */
    uint32_t payloadFmt;
#define SINGLE_NAL_RTP_PACKET 1UL
#define FU_A_RTP_PACKET       2UL

    /** Num of NAL's in payload (used in STAP only) */
    uint32_t numNALUnit;

    /** NAL type of payload (>1 used in STAP only) */
    uint32_t nalTypeValue[MAX_NUM_NAL_PER_PAYLOAD];
}RTPPayloadInfo;

typedef void (*RtpPacketizedDataCb) (uint8_t *payload, uint32_t len);

/** Internal structure of RTP Library */
typedef struct _RTPHandle {
    uint16_t packetSeqNum;
    uint32_t lastTimeStamp;
    uint32_t ssrcIdentifier;
    RtpPacketizedDataCb payloadCB;
}RTPHandle;

int32_t RtpInit (RtpPacketizedDataCb func);
int32_t RtpPacketize (RTPPayloadInfo *data);
int32_t RtpDeInit ();
uint32_t AddRtpHeader (RTPPayloadInfo *data);
uint32_t SingleNalPayload (RTPPayloadInfo *data);
int32_t FU_A_Header (RTPPayloadInfo *data, uint8_t *buf,
                        uint8_t start, uint8_t end);
uint32_t FragmentedNalPayload (RTPPayloadInfo *data);

uint8_t RtpPayload [64 * 1024 * 1024];
RTPHandle rtpLibHandle;

int32_t RtpInit (RtpPacketizedDataCb func)
{
    memset (&rtpLibHandle, 0, sizeof(RTPHandle));
    rtpLibHandle.ssrcIdentifier = 0x102;
    rtpLibHandle.payloadCB = func;
    return 0;
}

int32_t RtpPacketize (RTPPayloadInfo *data)
{
    uint32_t payloadLen = 0;

    if (data->payloadFmt == SINGLE_NAL_RTP_PACKET) {

        payloadLen = SingleNalPayload (data);
        rtpLibHandle.payloadCB (&(RtpPayload[0]),payloadLen);

    } else if (data->payloadFmt == FU_A_RTP_PACKET) {

        FragmentedNalPayload(data);

    } else {
        /* TBD */
    }

    return 0;
}

uint32_t AddRtpHeader (RTPPayloadInfo *data)
{
    uint8_t *buf = &(RtpPayload[0]);
    uint8_t markerBit = 0U;
    uint16_t sequenceNum = 0U;
    uint32_t timestamp = 0UL;
    uint32_t filledLen = 0UL;
    uint32_t srcID = 0UL;

    buf[filledLen] = (uint8_t)
                         /* Version=2, 2 bits */
                        (((((uint8_t)RTP_VERSION & 0x3U) << 6U) |
                         /* Padding, 1 bit */
                         (((uint8_t)RTP_PADDING & 0x1U) << 5U) |
                         /* Extension Header, 1 bit */
                         (((uint8_t)RTP_EXTENSION_HDR & 0x1U) << 4U) |
                         /* Count of CSRC, 4 bit */
                         ((uint8_t)RTP_CSRC_COUNT & 0xFU)) & 0xFF);
    filledLen += 1;

    if (1 == data->isLastNalOfFrame)
        markerBit = 1U;
    else
        markerBit = 0U;

    buf[filledLen] = (uint8_t)
                         /* Marker, 1 bit */
                        (((((uint8_t)markerBit & 0x1U) << 7U) |
                         /* Payload Type, 7 bit */
                         ((uint8_t)RTP_PAYLOAD_TYPE & 0x7FU)) & 0xFF);
    filledLen += 1;

    sequenceNum = rtpLibHandle.packetSeqNum++;
    /* Sequence number, 16 bit */
    buf[filledLen] = ((uint8_t)(sequenceNum >> 8U)) & 0xFFU;
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)(sequenceNum)) & 0xFFU;
    filledLen += 1UL;

    /* SPS/PPS and first frame should carry same TS */
    if (data->frameNum == 0) {
        timestamp = 3600;
    } else {
        /* 90khz increments @25 fps for each frame */
        /* TBD: Time increment based on command line configuration */
        timestamp = 3600 + (3600 * data->frameNum);
    }

    /* Timestamp, 32 bit */
    buf[filledLen] = ((uint8_t)(timestamp >> 24U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)(timestamp >> 16U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)(timestamp >> 8U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)timestamp & 0xFFU);
    filledLen += 1UL;

    srcID = rtpLibHandle.ssrcIdentifier;
    /* SSRC ID, 32 bit */
    buf[filledLen] = ((uint8_t)(srcID >> 24U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)(srcID >> 16U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)(srcID >> 8U) & 0xFFU);
    filledLen += 1UL;
    buf[filledLen] = ((uint8_t)srcID & 0xFFU);
    filledLen += 1UL;

    return filledLen;
}

uint32_t SingleNalPayload (RTPPayloadInfo *data)
{
    uint32_t numBytes = 0;
    uint8_t forbiddenZero = 0;
    uint8_t nalRefIdc = 0;
    uint8_t nalType = 0;
    uint8_t skipBytes = 4;
    uint8_t *buf = &(RtpPayload[0]);

    /* Add the generic RTP Header */
    numBytes = AddRtpHeader(data);
    if (0 == numBytes)
        printf("RTP > Error Addding RTP Header\n");

    /* Add the single NAL Unit Header */
    forbiddenZero = 0; /* 1 bit, set to 0 */
    nalRefIdc = 0x3; /* 2 bits, Set to 11 as all are IDR units */
    nalType = data->nalTypeValue[0]; /* 5 bits */

    /* SPS/PPS should not have the Payload Header */
    if (0 == data->isMetaData) {
        buf[numBytes] = (uint8_t)
                            (((((uint8_t)forbiddenZero & 0x1U) << 7U) |
                              (((uint8_t)nalRefIdc & 0x3U) << 5U) |
                               ((uint8_t)nalType & 0x1FU)) & 0xFF);
        numBytes += 1;
        skipBytes = 5; /* In case of NAL data, additional byte after
                          start code 00 00 00 01 XX needs to be skipped */
    }

    /* Copy the Payload data - NAL start code 00000001 should not be copied */
    memcpy (&buf[numBytes], data->data + skipBytes, data->dataLen - skipBytes);
    numBytes += (data->dataLen - skipBytes);

    return numBytes;
}

uint32_t FragmentedNalPayload (RTPPayloadInfo *data)
{
    uint32_t numBytes = 0;
    uint8_t *buf = &(RtpPayload[0]);
    uint32_t dataPending = data->dataLen;
    uint32_t fragSize = 0UL;
    uint32_t totalSent = 0UL;
    uint32_t startFrag = 1UL;
    uint32_t endFrag = 0UL;
    uint32_t skipBytes = 0UL;

    /* For NAL Data */
    if (0 == data->isMetaData) {
        /* In case of NAL data, additional byte after    */
        /* start code 00 00 00 01 XX needs to be skipped */
        dataPending = data->dataLen - 5;
        skipBytes = 5;
    } else {
        /* NAL start code should not be sent for SPS and PPS */
        dataPending = data->dataLen - 4;
        skipBytes = 4;
    }

    while (dataPending)
    {
        /* Unit of fragmented data to be packetized */
        if (dataPending > FRAGMENTATION_PAYLOAD_LEN) {
            fragSize = FRAGMENTATION_PAYLOAD_LEN;
        } else {
            fragSize = dataPending;
            endFrag = 1;
        }

        /* Remaining data to be processed */
        dataPending -= fragSize;

        /* Add the generic RTP Header */
        numBytes = AddRtpHeader(data);
        if (0 == numBytes)
            printf("RTP > Error Addding RTP Header\n");

        /* SPS/PPS should not have the FU Header */
        if (0 == data->isMetaData) {
            /* Add the FU-A Header */
            numBytes += FU_A_Header(data, buf+numBytes, startFrag, endFrag);
            startFrag = 0UL;
        }

        /* Copy the Payload - NAL start code 00000001 should not be copied */
        memcpy (&buf[numBytes], data->data + skipBytes + totalSent, fragSize);
        numBytes += fragSize;

        /* Invoke callback to send out data */
        rtpLibHandle.payloadCB (&(RtpPayload[0]),numBytes);

        totalSent += fragSize;
    }

    return numBytes;
}

int32_t FU_A_Header (RTPPayloadInfo *data, uint8_t *buf,
                        uint8_t start, uint8_t end)
{
    uint32_t numBytes = 0;
    uint8_t forbiddenZero = 0;
    uint8_t nalRefIdc = 0;
    uint8_t nalType = 0;
    uint8_t resrv = 0;

    /* Add the FU-A Indicator */
    forbiddenZero = 0; /* 1 bit, set to 0 */
    nalRefIdc = 0x3; /* 2 bits, Set to 10 as all are IDR units */
    nalType = FU_A_PACKET_TYPE; /* 5 bits */

    buf[numBytes] = (uint8_t)
                        (((((uint8_t)forbiddenZero & 0x1U) << 7U) |
                          (((uint8_t)nalRefIdc & 0x3U) << 5U) |
                           ((uint8_t)nalType & 0x1FU)) & 0xFF);
    numBytes += 1;

    /* Add the FU-A Header */
    resrv = 0; /* Reserved bit */
    nalType = data->nalTypeValue[0]; /* 5 bits, NAL Type */

    buf[numBytes] = (uint8_t)
                        (((((uint8_t)start & 0x1U) << 7U) |
                          (((uint8_t)end & 0x1U) << 6U) |
                          (((uint8_t)resrv & 0x1U) << 5U) |
                           ((uint8_t)nalType & 0x1FU)) & 0xFF);
    numBytes += 1;

    return numBytes;
}

int32_t RtpDeInit ()
{
    memset (&rtpLibHandle, 0, sizeof(RTPHandle));
    return 0;
}

unsigned int file_size = 0;
FILE         *read_fd;
unsigned int first = 0;
int          data_sock = -1;
uint8_t      inp_data[300*1024*1024];
uint32_t     inp_data_consumed = 0;
uint8_t      nal_unit[1*1024*1024];
uint32_t     nal_type = 0;
uint32_t     nal_unit_len = 0;
uint32_t     is_new_frame = 0;
uint32_t     is_end_of_cur_frame = 0;

/** Function to load entire file to memory (for ease of framing) */
int load_file_data(int file_size)
{
    int count = 0;
    count = fread(inp_data, 1, file_size, read_fd);
    printf("APP > %d bytes read from file\n",count);
    return count;
}

/** Function to return a single H264 NAL Frame */
void get_nal_unit ()
{
    unsigned char *start_ptr = NULL;
    unsigned char *end_ptr = NULL;

    while(inp_data_consumed < file_size)
    {
        /* Check NAL Start Code (4 bytes) */
        if((inp_data[inp_data_consumed]   == 0x0) &&
           (inp_data[inp_data_consumed+1] == 0x0) &&
           (inp_data[inp_data_consumed+2] == 0x0) &&
           (inp_data[inp_data_consumed+3] == 0x1))
        {
            /* If NAL is I Frame or I/P Frame (1 byte) */
            if((0x1 == (inp_data[inp_data_consumed+4] & 0x1F)) ||
               (0x5 == (inp_data[inp_data_consumed+4] & 0x1F)))
            {
                nal_type = inp_data[inp_data_consumed+4] & 0x1F;

                /* A high on the MSB means start of a new frame (1 MSB Bit) */
                if((0x80 == (inp_data[inp_data_consumed+5] & 0x80)) &&
                   (NULL == start_ptr))
                {
                    is_new_frame = 1;
                }

                /* Identify the last NAL of a frame */
                if((0x80 == (inp_data[inp_data_consumed+5] & 0x80)) &&
                   (NULL != start_ptr)) {
                    is_end_of_cur_frame = 1;
                }
            }

            if (start_ptr == NULL)
            {
                start_ptr = &inp_data[inp_data_consumed];
            }
            else
            {
                end_ptr = &inp_data[inp_data_consumed];
                break;
            }
        }
        inp_data_consumed++;
    }

    nal_unit_len = end_ptr-start_ptr;
    if ((end_ptr > start_ptr) && (nal_unit_len > 0UL)) {
        memcpy(&nal_unit, start_ptr, nal_unit_len);
    } else {
        nal_unit_len = 0UL;
    }
    start_ptr = NULL;
    end_ptr = NULL;

    return;
}

void ClientrecvRtpData (uint8_t *payload, uint32_t len)
{
    uint32_t sendto_count = 0;
    struct sockaddr_in clientAddr;
    uint32_t *ts = payload+4;

    memset(&clientAddr, 0x00, sizeof(struct sockaddr_in));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(CLIENTPORT);
    clientAddr.sin_addr.s_addr = inet_addr("192.168.10.1");

    if ((sendto_count = sendto(data_sock, payload, len, 0,
            (struct sockaddr *)&clientAddr, sizeof(clientAddr))) == -1 ) {
        printf("Error sending over AVTP socket\n");
    }
    //usleep(250);
    return;
}

int32_t main(int argc, char* argv[])
{
    char *interface = NULL;
    char *input_file = NULL;
    double frame_rate = 0;
    unsigned int slicesPerFrame = 0;
    struct stat st;
    int n;
    struct ifreq ifr;
    struct sockaddr_in serveraddr, clientAddr;
    uint32_t frame_count = 0;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size, client_addr_size;
    char buffer[1024];
    long long frame_duration;
    long long slice_wait_duration;
    long long sleep_time = 0;
    long elapsed = 0;
    struct timeval t0, t1, t2;

    if (argc < 9) {
        printf("Insufficient arguments, Usage: ./a.out -f <file name> \
                -i <interface> -r <frame_rate> -s <num_slice_per_frame>\n");
        return -1;
    }

    /* Parse command line options to obtain input file name
     * and target ethernet device
     */
    while ((n = getopt(argc, argv, "f:i:r:s:")) != -1) {
        switch (n) {
            case 'i':
                interface = strdup(optarg);
                break;
            case 'f':
                input_file = strdup(optarg);
                break;
            case 'r':
                frame_rate = atoi(optarg);
                break;
            case 's':
                slicesPerFrame = atoi(optarg);
                break;
            default:
                printf("Usage: ./a.out -f <file name> -i <interface> \
                            -r <frame_rate> -s <num_slice_per_frame>\n");
                return -1;
        }
    }

    fprintf(stderr, "input_file %s\n", input_file);
    read_fd = fopen(input_file, "rb");
    fprintf(stderr, "Frame rate %u\n", frame_rate);
    fprintf(stderr, "Slices per frame %u\n", slicesPerFrame);

    /* Create the UDP socket */
    data_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (data_sock < 0)
    {
        fprintf(stderr, "Error in socket() creation - %s", strerror(errno));
    }

    /* Bind to network interface specified */
    memset(&ifr, 0, sizeof(ifr));
    snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), interface);
    if ((n = setsockopt(data_sock, SOL_SOCKET, SO_BINDTODEVICE,
                         (void *)&ifr, sizeof(ifr))) < 0)
    {
        fprintf(stderr, "Server-setsockopt() error for SO_BINDTODEVICE\n");
        fprintf(stderr, "%s\n", strerror(errno));
        close(data_sock);
        exit(-1);
    }

    /* bind to an address */
    memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    serveraddr.sin_addr.s_addr = inet_addr("192.168.10.1");

    int rc = bind(data_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));

    lseek(read_fd, 0, SEEK_SET);

    stat(input_file, &st);
    file_size = st.st_size;

    if(file_size > 300*1024*1024) {
        printf("File size exceeded limit\n");
        exit(0);
    }
    else {
        printf("File size %d bytes \n",file_size);
    }

    load_file_data(file_size);

    frame_duration = (long long)((double)(1/frame_rate)*1000*1000);
    slice_wait_duration = frame_duration/slicesPerFrame;

    /* Initialize the RTP Library */
    RtpInit(ClientrecvRtpData);

    printf("RTP Initialized\n");

    /* Get individual H264 NAL units, Get RTP header added and stream */
    while (1)
    {
        RTPPayloadInfo h264_data;
        memset(&h264_data,0,sizeof(RTPPayloadInfo));

        /* Get a new NAL unit */
        get_nal_unit();
        if (0UL == nal_unit_len) {
            break;
        }
        /* Space out (in time) the slices of the frame */
        //usleep(slice_wait_duration);

        /* Check if this NAL marks start of a new Frame */
        if(1 == is_new_frame) {
            gettimeofday(&t2, 0);
            printf("APP > Frame Number %d \n",frame_count);
            is_new_frame = 0;
            frame_count++;

            /* Manage frame rate control */
            if ((t0.tv_sec != 0) || (t0.tv_usec != 0)) {
                gettimeofday(&t1, 0);
                elapsed = ((1000000 * t1.tv_sec) + t1.tv_usec) -
                          ((1000000 * t0.tv_sec) + t0.tv_usec);
                sleep_time = frame_duration - elapsed;
                if( sleep_time > 0)
                {
                    usleep(sleep_time);
                }
                gettimeofday(&t0, 0);
            } else {
                gettimeofday(&t0, 0);
            }

        }

        /* Update the RTP Payload information */
        h264_data.data = &(nal_unit[0]);
        h264_data.dataLen = nal_unit_len;
        if (frame_count > 0) {
            /* FIXME: After SPS/PPS Frame num should be 0 and not 1 */
            h264_data.frameNum = frame_count-1;
        } else {
            h264_data.frameNum = frame_count;
        }

        h264_data.nalTypeValue[0] = nal_type;
        h264_data.numNALUnit = 1;

        /* SPS or PPS */
        if (50 > nal_unit_len)
            h264_data.isMetaData = 1;

        if (1 == is_end_of_cur_frame) {
            is_end_of_cur_frame = 0;
            h264_data.isLastNalOfFrame = 1;
        } else {
            h264_data.isLastNalOfFrame = 0;
        }

        /* Configure for Single NAL or FUA mode of RTP Packetization */
        h264_data.payloadFmt = FU_A_RTP_PACKET;

        /* Get RTP Header added */
        RtpPacketize (&h264_data);
    }

    /* Deinitialize the RTP Library */
    RtpDeInit();

    if (read_fd > 0) {
        fclose(read_fd);
    }

    if (data_sock > 0) {
        close(data_sock);
    }

    printf("*#*# %s[%d]\n", __FUNCTION__, __LINE__);
    return 0;
}
