/*
 * Main.c
 *
 *  Created on: 2021年3月31日
 *      Author: zdp
 */
#include <stdio.h>
#include <pthread.h>
#include "HookRecv.h"
#include "SrtSend.h"
#include "QueueRecv.h"
int main(){
	if(ring_buffer_init() != 0){
		perror("ring_buffer_init");
		return -1;
	}
	pthread_t t1, t2;
	int ret;
//	ret = pthread_create(&t1, NULL, Hook_Recv, NULL);
	ret = pthread_create(&t1, NULL, Queue_Recv, NULL);
	if(ret != 0){
		perror("pthread_create");
		return -1;
	}
	ret = pthread_create(&t2, NULL, Srt_Send, NULL);
	if(ret != 0){
		perror("pthread_create");
		return -1;
	}
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}



