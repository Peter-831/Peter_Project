/*
 * main.c
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */
#include "SrtRecv.h"
#include "RtpSend.h"
#include "RingBuffer.h"

pthread_mutex_t port_mutex;
pthread_cond_t port_cond;
int main(){
	if(ring_buffer_init() != 0){
		perror("ring_buffer_init()");
		return -1;
	}
	pthread_mutex_init(&port_mutex, NULL);
	pthread_cond_init(&port_cond, NULL);
	pthread_t t1, t2;
	int ret;
	ret = pthread_create(&t1, NULL, Srt_Recv, NULL);
	if(ret != 0){
		perror("pthread_create()");
		return -1;
	}

	ret = pthread_create(&t2, NULL, Rtp_Send, NULL);
	if(ret != 0){
		perror("pthread_create()");
		return -1;
	}
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}


