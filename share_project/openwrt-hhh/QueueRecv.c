/*
 * Queue_Recv.c
 *
 *  Created on: 2021年4月19日
 *      Author: zdp
 */


#include "QueueRecv.h"

/* returns packet id */
static u_int32_t print_pkt (struct nfq_data *tb)
{
	int id = 0;
	struct nfqnl_msg_packet_hdr *ph;
	struct nfqnl_msg_packet_hw *hwph;
	u_int32_t mark,ifi;
	int ret;
	unsigned char *data;
	static unsigned int ind = 0;
	ind++;
  // 提取数据包头信息,包括id,协议和hook点信息
	ph = nfq_get_msg_packet_hdr(tb);
	if (ph) {
		id = ntohl(ph->packet_id);
		printf("hw_protocol=0x%04x hook=%u id=%u ",ntohs(ph->hw_protocol), ph->hook, id);
	}

	hwph = nfq_get_packet_hw(tb);
	if (hwph) {
		int i, hlen = ntohs(hwph->hw_addrlen);
		printf("hw_src_addr=");
		for (i = 0; i < hlen-1; i++)
			printf("%02x:", hwph->hw_addr[i]);
		printf("%02x ", hwph->hw_addr[hlen-1]);
	}


	mark = nfq_get_nfmark(tb);
	if (mark)
		printf("mark=%u ", mark);

	ifi = nfq_get_indev(tb);
	if (ifi)
		printf("indev=%u ", ifi);

	ifi = nfq_get_outdev(tb);
	if (ifi)
		printf("outdev=%u ", ifi);

	ifi = nfq_get_physindev(tb);
	if (ifi)
		printf("physindev=%u ", ifi);

	ifi = nfq_get_physoutdev(tb);
	if (ifi)
		printf("physoutdev=%u ", ifi);

  // 获取数据包载荷,data指针指向载荷,从实际的IP头开始
	ret = nfq_get_payload(tb, &data);
	UDP_HEADER *udp_header = NULL;
//	int i = 0;
	if (ret > 28)
	{
//		printf("payload_len=%d\t", ret);
//		for(i = 0; i < ret; i++)//udp header
//			printf("%02x ", data[i]);
//		printf("\n\n");
		udp_header = (UDP_HEADER*)(data + 20);
		printf("udp info:%u %u %u %u\n", ntohs(udp_header->source), ntohs(udp_header->dest), ntohs(udp_header->len), ntohs(udp_header->check));
		write_nalu_to_ring_buffer((char *)data + 20, ret - 20, ind);
	}

	fputc('\n', stdout);
	return id;
}



static int cb(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg,
	      struct nfq_data *nfa, void *data)
{
	printf("entering callback\n");//进入到回调函数
	u_int32_t id = print_pkt(nfa);
	/**
	 * 函数功能：
		对一个数据包发表裁决。
	 * 函数参数：
		qh:通过调用nfq_create_queue（）获得的Netfilter队列句柄。
		id:由netfilter分配给数据包的ID
		verdict:决定返回到netfilter
		data_len: buf缓冲区的字节数
		buf:包含数据包数据的缓冲区
	 * 函数返回值
		出错返回-1，否则返回值大于等于0。
	*/
	return nfq_set_verdict(qh, id, NF_DROP, 0, NULL);
}


void *Queue_Recv()
{
	struct nfq_handle *h;
	struct nfq_q_handle *qh;
//	struct nfnl_handle *nh;
	int fd;
	int rv;
	char buf[4096] __attribute__ ((aligned));

	printf("opening library handle\n");
	h = nfq_open();//创建 netfilter_queue
	if (!h) {//创建失败
		perror("nfq_open");
		fprintf(stderr, "error during nfq_open()\n");
		exit(1);
	}

	printf("unbinding existing nf_queue handler for AF_INET (if any)\n");//解绑已经存在的队列

	if (nfq_unbind_pf(h, AF_INET) < 0) {
		perror("nfq_unbind_bf");
		fprintf(stderr, "error during nfq_unbind_pf()\n");
		exit(1);
	}

	printf("binding nfnetlink_queue as nf_queue handler for AF_INET\n");//绑定上我们创建的队列

	if (nfq_bind_pf(h, AF_INET) < 0) {
		fprintf(stderr, "error during nfq_bind_pf()\n");
		exit(1);
	}

	printf("binding this socket to queue '0'\n");//cb是回调函数
	qh = nfq_create_queue(h,  0, &cb, NULL);
	if (!qh) {
		fprintf(stderr, "error during nfq_create_queue()\n");
		exit(1);
	}

	printf("setting copy_packet mode\n");
	if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {//设置的包处理模式
		fprintf(stderr, "can't set packet_copy mode\n");
		exit(1);
	}

	fd = nfq_fd(h);

	for (;;) {
		printf("1111\n");
		if ((rv = recv(fd, buf, sizeof(buf), 0)) >= 0) {
			printf("pkt received\n");
			nfq_handle_packet(h, buf, rv);
			printf("2222\n");
			continue;
		}
		/* if your application is too slow to digest the packets that
		 * are sent from kernel-space, the socket buffer that we use
		 * to enqueue packets may fill up returning ENOBUFS. Depending
		 * on your application, this error may be ignored. Please, see
		 * the doxygen documentation of this library on how to improve
		 * this situation.
		 */
		if (rv < 0 && errno == ENOBUFS) {
			printf("losing packets!\n");
			continue;
		}
		perror("recv failed");
		break;
	}

	printf("unbinding from queue 0\n");
	nfq_destroy_queue(qh);//摧毁队列，退出

#ifdef INSANE
	/* normally, applications SHOULD NOT issue this command, since
	 * it detaches other programs/sockets from AF_INET, too ! */
	printf("unbinding from AF_INET\n");
	nfq_unbind_pf(h, AF_INET);
#endif

	printf("closing library handle\n");
	nfq_close(h);
	exit(0);
	return NULL;
}



