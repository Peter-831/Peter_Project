
#ifndef _RING_BUFFER
#define _RING_BUFFER

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>



int ring_buffer_init();

int write_nalu_to_ring_buffer(char *frame, int frame_size,
		unsigned int frame_num);

int read_nalu_from_ring_buffer(unsigned char *, unsigned int *);

int ring_buf_left_fm();



typedef struct
{
	unsigned short int     frame_num;
	unsigned int           frame_length;
	unsigned char          is_segmentation;
	unsigned int           left_length;
	unsigned char          is_filled;
}Frame_header_send_buffer;



#endif
