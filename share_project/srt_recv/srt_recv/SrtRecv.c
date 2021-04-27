/*
 * SrtRecv.c
 *
 *  Created on: 2021年4月4日
 *      Author: zdp
 */


#include "SrtRecv.h"
#include <stdbool.h>

FILE *fp;

int dest_port;
extern pthread_mutex_t port_mutex;
extern pthread_cond_t port_cond;
int rtsp_unpackage(unsigned char *buf_in, int len, unsigned char *buf_out)
{
	static int fm = 0;
	static int fm_size = 0;
	int RTP_head_len = sizeof(RTP_FIXED_HEADER);
	RTP_FIXED_HEADER *rtp_header = NULL;
	NALU_HEADER *nalu_hdr = NULL;
	FU_INDICATOR *fu_ind = NULL;
	FU_HEADER *fu_hdr = NULL;
	rtp_header = (RTP_FIXED_HEADER *)buf_in;
	static int after_stamp = 0;
	int stamp = rtp_header->timestamp;
//	printf("seq:%d\t stamp:%d\n",rtp_header->seq_no, rtp_header->timestamp);
	if(after_stamp != stamp){
//		printf("fm:%d\tfm_size:%d\n", fm, fm_size);
		fm_size = 0;
		fm++;
	}
	after_stamp = stamp;
	int i = 0;
	nalu_hdr = (NALU_HEADER*)(buf_in + RTP_head_len);
//	printf("TYPE:%d\n",nalu_hdr->TYPE);
	if (nalu_hdr->TYPE > 0 && nalu_hdr->TYPE < 24)
	{
		//NALU单包
		//写起始字节
		buf_out[i++] = 0x00;
		buf_out[i++] = 0x00;
		buf_out[i++] = 0x00;
		buf_out[i++] = 0x01;
		//写NAL_HEADER
		memcpy(&buf_out[i++], nalu_hdr, 1);
		memcpy(&buf_out[i], buf_in + RTP_head_len + 1, len - RTP_head_len - 1);
		i += len - RTP_head_len - 1;
	}
	else if (nalu_hdr->TYPE == 28 || nalu_hdr->TYPE == 29)
	{
//		if(nalu_hdr->TYPE == 29){
//			printf("bbbbbbbbb\n");
//		}
		//FU-A或FU-B分片包，解码顺序和传输顺序相同
		fu_ind = (FU_INDICATOR*)(buf_in + RTP_head_len);
		fu_hdr = (FU_HEADER*)(buf_in + RTP_head_len + 1);
		printf("E:%d\tS:%d\tR:%d\n", fu_hdr->E, fu_hdr->S, fu_hdr->R);
		if (fu_hdr->E == 1)	{
			//分片包最后一个包
//			printf("1111\n");
			memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);
//			printf("2222\n");
			i += len - RTP_head_len - 2;
		}
		else {
			//分片包，但不是最后一个包
			if (fu_hdr->S == 1) {
				//分片的第一个包
				//写起始字节
//				printf("3333\n");
				buf_out[i++] = 0x00;
				buf_out[i++] = 0x00;
				buf_out[i++] = 0x00;
				buf_out[i++] = 0x01;
				unsigned char F;
				unsigned char NRI;
				unsigned char TYPE;
				unsigned char nh;
				F = fu_ind->F << 7;
				NRI = fu_ind->NRI << 5;
				TYPE = fu_hdr->TYPE;
				nh = F | NRI | TYPE;
				//写NAL_HEADER
				buf_out[i++] = nh;
//				printf("aaaa:%d\n",len - RTP_head_len - 2);
				memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);

				i += len - RTP_head_len - 2;
//				printf("5555\n");
			}
			else {
//			printf("4444\n");
				//如果不是第一个包
				memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);
				i += len - RTP_head_len - 2;
//						printf("6666\n");
			}
		}
	}else{
//		printf("aaaaaaaaa\n");
	}
	fm_size += i;

	return i;
}
int Srt_Init(int *ss_ret){

	int ss, st;
	struct sockaddr_in sa;
	int yes = 1;
	struct sockaddr_storage their_addr;

	srt_startup();

	ss = srt_create_socket();
	if (ss == SRT_ERROR)
	{
		fprintf(stderr, "srt_socket: %s\n", srt_getlasterror_str());
		return -1;
	}
	printf("srt_create_socket ok\n");
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SRT_LOCAL_PORT);
	if (inet_pton(AF_INET, SRT_LOCAL_IP, &sa.sin_addr) != 1)
	{
		return -1;
	}
	char fec[] = "fec,cols:10,rows:10,arq:never,layout:even";
	srt_setsockflag(ss, SRTO_PACKETFILTER, &fec, strlen(fec));
	srt_setsockflag(ss, SRTO_RCVSYN, &yes, sizeof yes);
//	int arq = 1;
//	srt_setsockflag(ss, SRTO_RETRANSMITALGO, &arq, sizeof arq);
//	bool a = false;
//	srt_setsockflag(ss, SRTO_NAKREPORT, &a, sizeof(bool));
	st = srt_bind(ss, (struct sockaddr*)&sa, sizeof sa);
	if (st == SRT_ERROR)
	{
		fprintf(stderr, "srt_bind: %s\n", srt_getlasterror_str());
		return -1;
	}
	printf("srt_bind ok\n");

	st = srt_listen(ss, 2);
	if (st == SRT_ERROR)
	{
		fprintf(stderr, "srt_listen: %s\n", srt_getlasterror_str());
		return -1;
	}
	printf("srt_listen ok\n");
	int addr_size = sizeof their_addr;
	int their_fd = srt_accept(ss, (struct sockaddr *)&their_addr, &addr_size);
	*ss_ret = ss;
	printf("srt_accept ok\n");
	return their_fd;
}

int Srt_Release(int ss){
	int ret;
	printf("srt close\n");
	ret = srt_close(ss);
	if (ret == SRT_ERROR)
	{
		fprintf(stderr, "srt_close: %s\n", srt_getlasterror_str());
		return -1;
	}

	printf("srt cleanup\n");
	srt_cleanup();
	return 0;
}
 void *Srt_Recv()
{

	fp = fopen("recv.h264", "wb");
	if(fp == NULL)	{perror("recv.h264"); return 0;}

	int their_fd, len, ss;
	if((their_fd = Srt_Init(&ss)) < 0){
		perror("Srt_Init()");
		return NULL;
	}


    while (1)
    {
    	static int ind = 0;
    	unsigned char buf_out[MAXBUF + 1] = {0};
        char msg[2048];
        len = srt_recv(their_fd, msg, sizeof msg);
        if (len == SRT_ERROR)
        {
            fprintf(stderr, "srt_recv: %s\n", srt_getlasterror_str());

        }
        ind++;
//        UDP_HEADER *udp_header = (UDP_HEADER *)msg;
//        if(ind == 1){
//        	pthread_mutex_lock(&port_mutex);
//        	dest_port = ntohs(udp_header->dest);
//        	printf("port:%d\n", dest_port);
//
//        	pthread_mutex_unlock(&port_mutex);
//           	pthread_cond_signal(&port_cond);
//
//        }
//        printf("udp info:%u %u %u %u\n", ntohs(udp_header->source),\
//        		ntohs(udp_header->dest), ntohs(udp_header->len), ntohs(udp_header->check));
        write_nalu_to_ring_buffer((unsigned char*)msg, len, ind);
//        write_nalu_to_ring_buffer((unsigned char*)msg + sizeof(UDP_HEADER), len - sizeof(UDP_HEADER), ind);
//		int ret = rtsp_unpackage((unsigned char*)msg + sizeof(UDP_HEADER), len - sizeof(UDP_HEADER), buf_out);
//		printf("ind:%d\tret:%d\n",i,ret);
//		if(i <= 20000){
//			//display(buf_out, ret);
//			fwrite(buf_out, 1, ret, fp);
//			fflush(fp);
//		}
//		if(i == 20001){
//			fclose(fp);
//			exit(-1);
//		}
//        printf("Got msg of len %d << %s\n", st, msg);
    }
	if(Srt_Release(ss) < 0){
		perror("Srt_Relase()");
		return NULL;
	}

    return 0;
}

