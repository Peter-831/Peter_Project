/*
 * RtpSend.h
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */

#ifndef RTPSEND_H_
#define RTPSEND_H_
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Config.h"
#include "RingBuffer.h"
int Send_Init();
void *Rtp_Send();


#endif /* RTPSEND_H_ */
