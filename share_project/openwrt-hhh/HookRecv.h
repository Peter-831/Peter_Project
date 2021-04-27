/*
 * HookRecv.h
 *
 *  Created on: 2021年4月16日
 *      Author: zdp
 */

#ifndef HOOKRECV_H_
#define HOOKRECV_H_


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include "Config.h"


typedef struct
{
    /**//* byte 0 */
    unsigned char csrc_len:4;        /**//* expect 0 */
    unsigned char extension:1;       /**//* expect 1, see RTP_OP below */
    unsigned char padding:1;         /**//* expect 0 */
    unsigned char version:2;         /**//* expect 2 */
    /**//* byte 1 */
    unsigned char payload:7;         /**//* RTP_PAYLOAD_RTSP */
    unsigned char marker:1;          /**//* expect 1 */
    /**//* bytes 2, 3 */
    unsigned short seq_no;
    /**//* bytes 4-7 */
    unsigned int timestamp;
    /**//* bytes 8-11 */
    unsigned int ssrc;               /**//* stream number is used here. */
} RTP_FIXED_HEADER;

typedef struct {
    //byte 0
	unsigned char TYPE:5;
    unsigned char NRI:2;
	unsigned char F:1;

} NALU_HEADER; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
	unsigned char NRI:2;
	unsigned char F:1;

} FU_INDICATOR; /**//* 1 BYTES */

typedef struct {
    //byte 0
    unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;
} FU_HEADER; /**//* 1 BYTES */
int Rtp_Unpackage(unsigned char *buf_in, int len, unsigned char *buf_out);
int Proc_RTP_Packet(char *packet_buf, char *fm_buf, int packet_len);
void Proc_RTP(int socket_fd, struct sockaddr_in servaddr);


int Hook_Socket_Init();
void *Hook_Recv();

#endif /* HOOKRECV_H_ */
