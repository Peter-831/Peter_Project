/*
 * Srt_Client.c
 *
 *  Created on: 2021年3月31日
 *      Author: zdp
 */


#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#define usleep(x) Sleep(x / 1000)
#else
#include <unistd.h>
#endif

#include <srt/srt.h>

#include "SrtSend.h"

int Srt_Init(){
	char *clent_IP = SRT_SERVER_IP;
    int ss, st;
    struct sockaddr_in sa;
    int yes = 1;

    printf("srt startup\n");
    srt_startup();

    printf("srt socket\n");
    ss = srt_create_socket();
    if (ss == SRT_ERROR)
    {
        fprintf(stderr, "srt_socket: %s\n", srt_getlasterror_str());
        return -1;
    }

    printf("srt remote address\n");
    sa.sin_family = AF_INET;
    sa.sin_port = htons(SRT_SERVER_PORT);
    if (inet_pton(AF_INET, clent_IP, &sa.sin_addr) != 1)
    {
        return -1;
    }

    printf("srt setsockflag\n");
//  char fec[] = "fec,cols:10,rows:10,arq:onreq,layout:even";
//
//	srt_setsockflag(ss, SRTO_PACKETFILTER, &fec, strlen(fec));
//  int arq = 1;
//	srt_setsockflag(ss, SRTO_RETRANSMITALGO, &arq, sizeof(arq));
    srt_setsockflag(ss, SRTO_SENDER, &yes, sizeof yes);

    // Test deprecated
    //srt_setsockflag(ss, SRTO_STRICTENC, &yes, sizeof yes);

    printf("srt connect\n");
    st = srt_connect(ss, (struct sockaddr*)&sa, sizeof sa);
    if (st == SRT_ERROR)
    {
        fprintf(stderr, "srt_connect: %s\n", srt_getlasterror_str());
        return -1;
    }
	return ss;
}
int Srt_Release(int ss){
	int st;
    printf("srt close\n");
    st = srt_close(ss);
    if (st == SRT_ERROR)
    {
        fprintf(stderr, "srt_close: %s\n", srt_getlasterror_str());
        return -1;
    }

    printf("srt cleanup\n");
    srt_cleanup();
    return 0;
}
void *Srt_Send()
{
	int ss, ret;
	if((ss = Srt_Init()) < 0){
//		printf("asas\n");
		perror("Srt_Init()");
		return NULL;
	}
	unsigned int packet_ind = 0;
	char Packet_Buff[MAXBUF + 1] = {0};
//	const char message [] = "This message should be sent to the other side";
	while(1){
		bzero(Packet_Buff, MAXBUF + 1);
		int packet_len = read_nalu_from_ring_buffer(Packet_Buff, &packet_ind);
	    int len;
//	        printf("srt sendmsg2 #%d >> %s\n",i,message);
		len = srt_send(ss, Packet_Buff, packet_len); // srt max packet len 1316
		if (len == SRT_ERROR)
		{
			fprintf(stderr, "srt_sendmsg: %s\n", srt_getlasterror_str());
			exit(-1);
			break;
		}
		printf("ind:%d\tsendmsg2 len:%d\n", packet_ind, len);
//	        usleep(100000);   // 20 ms


	}
	if((ret = Srt_Release(ss)) < 0){
		perror("Srt_Release()");
		return NULL;
	}

    return NULL;
}




