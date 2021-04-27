/*
 * Srt_Client.h
 *
 *  Created on: 2021年3月31日
 *      Author: zdp
 */

#ifndef SRTSEND_H_
#define SRTSEND_H_

#include <errno.h>
#include <stdio.h>


#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include "RingBuffer.h"
#include "Config.h"

void *Srt_Send();
int Srt_Init();
int Srt_Release();

#endif /* SRTSEND_H_ */
