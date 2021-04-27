/*
 * RtpSend.c
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */

#include "RtpSend.h"
#include "SrtRecv.h"
extern int dest_port;
extern pthread_mutex_t port_mutex;
extern pthread_cond_t port_cond;
struct sockaddr_in stRemoteAddr = {0};

int Send_Init(){
	char *remote_IP = REMOTE_IP;
	int iSocketFD = 0;
	int iRemotAddr = 0;

	/* 创建socket */
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0)
	{
		printf("创建socket失败!\n");
		return 0;
	}
	printf("%s\n",remote_IP);
	/* 填写服务端地址 */
	stRemoteAddr.sin_family = AF_INET;

//	pthread_mutex_lock(&port_mutex);
//	if(dest_port == 0){
//		pthread_cond_wait(&port_cond, &port_mutex);
//	}
//	pthread_mutex_unlock(&port_mutex);

	stRemoteAddr.sin_port   = htons(dest_port);
	if(inet_pton(AF_INET, remote_IP, (void *)&iRemotAddr) != 1){
		perror("inet_pton");
		return -1;
	}

	stRemoteAddr.sin_addr.s_addr = iRemotAddr;
//	int ch = 0;
//	getsockopt(iSocketFD, SOL_SOCKET, SO_NO_CHECK, &ch, sizeof(int));
//	int check = 0;
//	setsockopt(iSocketFD, SOL_SOCKET, SO_NO_CHECK, &check, sizeof(int));
	return iSocketFD;
}
void *Rtp_Send(){
	int fd;
	if((fd = Send_Init()) < 0){
		perror("Send_Init()");
		return NULL;
	}
	char Packet_Buff[MAXBUF + 1] = {0};
	while(1){
		int ret = 0, read_ind = 0, read_len = 0;
		bzero(Packet_Buff, MAXBUF + 1);
		read_len = read_nalu_from_ring_buffer(Packet_Buff, &read_ind);
		UDP_HEADER* header = (UDP_HEADER *)Packet_Buff;
		stRemoteAddr.sin_port = header->dest;

		printf("len:%d\n", read_len);
		ret = sendto(fd, (char *)Packet_Buff + sizeof(UDP_HEADER), read_len - sizeof(UDP_HEADER),\
				0, (struct sockaddr *)&stRemoteAddr, sizeof(struct sockaddr_in));
	 	if(ret < 0){
	 		perror("sendto()");
	 	}
		printf("%d\n", ret);
	}
	close(fd);
	return NULL;
}


