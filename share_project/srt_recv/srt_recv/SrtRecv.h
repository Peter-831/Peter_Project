/*
 * SrtRecv.h
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */

#ifndef SRTRECV_H_
#define SRTRECV_H_
#include <stdio.h>
#include <srt/srt.h>
#include <pthread.h>
#include "Config.h"
typedef struct
{
	u_int16_t source;
	u_int16_t dest;
	u_int16_t len;
	u_int16_t check;
}UDP_HEADER;

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


void *Srt_Recv();
int Srt_Init(int *ss_ret);
int Srt_Release(int ss);

#endif /* SRTRECV_H_ */
