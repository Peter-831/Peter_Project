/*
 * Queue_Recv.h
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */

#ifndef QUEUERECV_H_
#define QUEUERECV_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/netfilter.h>		/* for NF_ACCEPT */
#include <errno.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

typedef struct
{
  u_int16_t source;
  u_int16_t dest;
  u_int16_t len;
  u_int16_t check;
}UDP_HEADER;
void *Queue_Recv();
static u_int32_t print_pkt (struct nfq_data *tb);
static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data);

#endif /* QUEUERECV_H_ */
