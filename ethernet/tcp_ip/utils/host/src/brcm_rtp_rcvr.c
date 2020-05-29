/*****************************************************************************
 Copyright 2017 Broadcom Limited.  All rights reserved.

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
******************************************************************************/

/** @file brcm_rtp_rcvr.c
    @brief Implementation of RTP stream dump application

    This is an independent Linux PC application which receives data
    from UDP socket, extracts the RTP header and dumps the payload to
    the mentioned dump file.
*/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<unistd.h>

/* Maximum size of data that is dumped to file - Debug */
#define MAX_FDUMP_SIZE (100 * 1024 * 1024)
uint8_t buffer_enc[MAX_FDUMP_SIZE];
uint32_t buf_offset = 0;

/* Receive buffer length */
#define BUFLEN (20*1024)

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char* argv[])
{
    struct sockaddr_in si_me, si_other;
    uint16_t prevSeqNum = 0UL;
    uint32_t ssrcID = 0x102UL;
    uint16_t portNum;
    char *outFileName = NULL;
    FILE *fp = NULL;
    int32_t n = 0;
    uint8_t first = 1U;
    int32_t s, i, slen = sizeof(si_other) , recv_len;
    uint8_t buf[BUFLEN];

    /* Check the count of input arguments */
    if (argc < 5) {
        printf("Insufficient arguments, Usage: ./a.out -p <udp_port_num> -f <out_file_name>\n");
        return -1;
    }

    /* Parse command line options to obtain input arguments */
    while ((n = getopt(argc, argv, "p:f:")) != -1) {
        switch (n) {
            case 'p':
                portNum = atoi(optarg);
                break;
            case 'f':
                outFileName = strdup(optarg);
                break;
            default:
                printf("Usage: Usage: ./a.out -p <udp_port_num> -f <out_file_name>\n");
                return -1;
        }
    }

    /* Create a UDP socket for receiving data */
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    /* Zero out the structure */
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(portNum);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bind socket to port */
    if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        die("bind");
    }

    /* keep listening for data */
    while(1)
    {
        uint16_t seqNum = 0U;
        uint32_t ssrc = 0UL;
        uint8_t pktType = 0U;
        uint8_t isStartOfNal = 0U;
        uint8_t nalHdrLen = 0U;
        uint8_t tmp8 = 0U;
        uint8_t nalData[5] = {0x0U,0x0U,0x0U,0x1U,0x65U};
        uint32_t fSize = 0UL;
        int32_t write_len = 0;
        int32_t copyBufSize = 0UL;
        uint32_t offset = 0UL;
        int32_t prev;
        uint32_t isFileRewrite = 0UL;
        uint32_t isSPSSliceStart = 0UL;

        /* Try to receive some data, this is a blocking call */
        if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
        {
            die("recvfrom()");
        }

        /* Log in case of receive error */
        if (0 > recv_len){
            printf("Error >> Receive length error %d \n",recv_len);
        }

        /* Check the RTP stream ID (0x102) */
        ssrc = be32toh(*((uint32_t*)(buf+8UL)));
        if(memcmp(&ssrc, &ssrcID, 4UL)){
            printf("Info >> ssrc %x mismatch (expected 0x102)\n",ssrc);
            continue;
        }

        /* Check for any packet losses */
        seqNum = ntohs(*((uint16_t *)(buf + 2)));
        if ((seqNum != prevSeqNum+1) && (0U == first)){
            printf("Error >> Packet drop detected [prev %d, cur %d]",prevSeqNum,seqNum);
        }
        prevSeqNum = seqNum;
        first = 0U;

        /* Check for the payload data type */
        pktType = *(buf + 12);

        /* For FUA packets - Has H264 IDR data */
        if (0x7C == pktType){
            /* FU Header byte */
            tmp8 = *(buf + 13);

            /* Check if slice start flag set */
            if (1U == ((tmp8 >> 7U) & 1U)){
                isStartOfNal = 1U;
                nalHdrLen = 5U;
            }

            /* Compute h264 payload size */
            offset = 12U /* RTP header */ + 2U /* FUA Header */;
            copyBufSize = recv_len - offset;

        /* Single NAL packet - has sps/pps slice */
        } else if ((0x67 == pktType) || (0x68 == pktType)){
            /* Set flag to add the NAL start code */
            isStartOfNal = 1U;
            nalHdrLen = 4U;

            /* Compute sps/pps payload size */
            offset = 12U /* RTP header */;
            copyBufSize = recv_len - offset;

            if (0x67 == pktType){
                isSPSSliceStart = 1UL;
            }
        } else {
            printf("Error >> Undetected payload type\n");
        }

        if (fp == NULL) {
            fp = fopen(outFileName,"ab");
            if (NULL == fp){
                printf("Error >> File open failed\n");
            }
        }

        /* Get the output file size for overwrite decision */
        fSize = ftell(fp);

        /* Restore file pointer based on the output file size */
        if (fSize < ((MAX_FDUMP_SIZE) - BUFLEN)) {
            /* Previous file location less than the MAX_FDUMP_SIZE */
        } else {
            /* Close and reopen file to start dump from offset 0 */
            fclose(fp);
            fp = fopen(outFileName,"wb");
            if (NULL == fp){
                printf("Error >> File open failed\n");
            }
        }

        /* Check if start of a file */
        if (0UL == ftell(fp)) {
            /* This flag is set to write file starting with an SPS */
            isFileRewrite = 1UL;
        }

        /* In case if its file start/overwrite, then start data dump from SPS only */
        if (((1UL == isFileRewrite) && (1UL == isSPSSliceStart)) || (0UL == isFileRewrite)){
            /* Write NAL start code which is not present in RTP payload */
            /* but needed for h264 playback on VLC                      */
            if (1U == isStartOfNal){
                write_len = fwrite(nalData,1UL, nalHdrLen, fp);
                if(write_len != nalHdrLen) {
                    printf("Error 1 >> File Dump Failed %d \n",write_len);
                }
            }

            /* Dump payload to file */
            write_len = fwrite(buf+offset,1UL, copyBufSize, fp);
            if(write_len != copyBufSize) {
                printf("Error 2 >> File Dump Failed %d actual size %d \n",write_len,copyBufSize);
            }
        }

        fclose(fp);
        fp = NULL;
    }

    close(s);
    return 0;
}
