/*
 * Hook_Recv.c
 *
 *  Created on: 2021年4月16日
 *      Author: zdp
 */

#include "HookRecv.h"
FILE *fp;
int Proc_RTP_Packet(char *packet_buf, char *fm_buf, int packet_len){
	static int fm = 0, tmp_fm_size = 0, after_stamp = 0;
	int RTP_head_len = sizeof(RTP_FIXED_HEADER);
	RTP_FIXED_HEADER *rtp_header = (RTP_FIXED_HEADER *)packet_buf;

	int cur_stamp = rtp_header->timestamp;
//	printf("seq:%d\t stamp:%d\n",rtp_header->seq_no, rtp_header->timestamp);
	if(after_stamp != cur_stamp){ // new fm
//		printf("fm:%d\tfm_size:%d\n", fm, fm_size);
		tmp_fm_size = 0;
		fm++;
	}
	tmp_fm_size += packet_len - RTP_head_len;
	after_stamp = cur_stamp;

	memcpy(fm_buf + tmp_fm_size, packet_buf, packet_len);
	return 0;
}
void Proc_RTP(int socket_fd, struct sockaddr_in servaddr){
	int n;
	socklen_t  addr_len;
	char buff[MAXBUF + 1] = {0};

	static unsigned int ind = 0;
	memset(buff, 0, MAXBUF);
	addr_len = sizeof(struct sockaddr_in);
//	printf("222\n");
    n = recvfrom(socket_fd, buff, MAXBUF, 0,(struct sockaddr*)&servaddr, &addr_len);
    if(n == -1){
        perror("recvfrom()");
    }
    printf("recvform: %d\n", n);
	ind++;
	write_nalu_to_ring_buffer(buff, n, ind);
	unsigned char buf_out[MAXBUF] = {0};
	int ret = Rtp_Unpackage((unsigned char*)buff, n, buf_out);
//	printf("ind:%d\tret:%d\n",ind,ret);
	if(ind <= 2000){
//		fwrite(buf_out, 1, ret, fp);
//		fflush(fp);
	}
	if(ind == 2001){
		fclose(fp);
	}
	printf("@@@@@ind:%d\tRecv %d Byte RTP message from server\n",ind, n);

}
int Rtp_Unpackage(unsigned char *buf_in, int len, unsigned char *buf_out)
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
	printf("seq:%d\t stamp:%d\n",rtp_header->seq_no, rtp_header->timestamp);
	if(after_stamp != stamp){
		printf("fm:%d\tfm_size:%d\n", fm, fm_size);
		fm_size = 0;
		fm++;
	}
	after_stamp = stamp;
	int i = 0;
	nalu_hdr = (NALU_HEADER*)(buf_in + RTP_head_len);
	printf("TYPE:%d\n",nalu_hdr->TYPE);
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
		if(nalu_hdr->TYPE == 29){
			printf("bbbbbbbbb\n");
		}
		//FU-A或FU-B分片包，解码顺序和传输顺序相同
		fu_ind = (FU_INDICATOR*)(buf_in + RTP_head_len);
		fu_hdr = (FU_HEADER*)(buf_in + RTP_head_len + 1);
		printf("E:%d\tS:%d\tR:%d\n", fu_hdr->E, fu_hdr->S, fu_hdr->R);
		if (fu_hdr->E == 1)	{
			//分片包最后一个包
			memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);
			i += len - RTP_head_len - 2;
		}
		else {
			//分片包，但不是最后一个包
			if (fu_hdr->S == 1) {
				//分片的第一个包
				//写起始字节
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
				memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);
				i += len - RTP_head_len - 2;
			}
			else {
				//如果不是第一个包
				memcpy(&buf_out[i], buf_in + RTP_head_len + 2, len - RTP_head_len - 2);
				i += len - RTP_head_len - 2;
			}
		}
	}else{
		printf("aaaaaaaaa\n");
	}
	fm_size += i;

	return i;
}



int Hook_Socket_Init(){

	int iSocketFD = 0;
	struct sockaddr_in stLocalAddr = {0};

	/* 创建socket */
	iSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	if(iSocketFD < 0)
	{
		printf("创建socket失败!\n");
		return -1;
	}

	/* 填写地址 */
	stLocalAddr.sin_family = AF_INET;
	stLocalAddr.sin_port   = htons(RECV_PORT);
	stLocalAddr.sin_addr.s_addr = 0;
	//htonl(INADDR_ANY)
	/* 绑定地址 */
	if(0 > bind(iSocketFD, (void *)&stLocalAddr, sizeof(stLocalAddr)))
	{
		printf("绑定地址失败!\n");
		close(iSocketFD);
		return -1;
	}
	return iSocketFD;
}


void *Hook_Recv(){

	int iSocketFD = 0;
	if((iSocketFD = Hook_Socket_Init()) < 0){
		perror("Hook_Socket_Init\n");
	}
	fp = fopen("recv.h264", "wb");
	if(fp == NULL){perror("recv.h264");}
	struct sockaddr_in stRemoteAddr = {0};
//	socklen_t iRemoteAddrLen = 0;
//	int iRecvLen = 0;
//	char acBuf[4096] = {0};
	while(1)     //实现了循环监听
	{
		printf("1111\n");
//		iRecvLen = recvfrom(iSocketFD, acBuf, sizeof(acBuf), 0, (void *)&stRemoteAddr, &iRemoteAddrLen);
		Proc_RTP(iSocketFD, stRemoteAddr);


	}
	close(iSocketFD);

	return NULL;
}


