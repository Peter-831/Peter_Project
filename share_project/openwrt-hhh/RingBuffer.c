


#include "RingBuffer.h"

#define MAX_CONSTANT_SENDBUFFER_SIZE	5000000
pthread_mutex_t mutex;

unsigned int Frame_header_send_buffer_length;
unsigned char *send_buffer_ptr, *write_ptr_send_buffer, *copy_ptr_send_buffer;
long total_write_send, total_read_send;
pthread_cond_t ring_buf_full, ring_buf_empty;
char write_wait, read_wait;


/**
* globe_variate_init_send() -     //
*
*/
int globe_variate_init_send()
{
	total_write_send = 0;
	total_read_send = 0;
	Frame_header_send_buffer_length = sizeof(Frame_header_send_buffer);
	write_ptr_send_buffer = send_buffer_ptr;
	copy_ptr_send_buffer  = send_buffer_ptr;
	write_wait = 0;
	read_wait = 0;

	return 0;
}

/**
* send_buffer_init() -     //
*
*
*
*
*/

int ring_buffer_init()
{
	send_buffer_ptr = (unsigned char *)malloc(MAX_CONSTANT_SENDBUFFER_SIZE * sizeof(unsigned char));
	if (send_buffer_ptr == NULL) {
		printf("send_buffer_init error!\n");
		return -1;
	}
	memset(send_buffer_ptr, 0, MAX_CONSTANT_SENDBUFFER_SIZE * sizeof(unsigned char));

	if (globe_variate_init_send() == -1) {
		printf("globe_variate_init_send error!\n");
		return -1;
	}

	if (cond_ini(&ring_buf_full, NULL) < 0) {
		printf("cond ring buf init error\n");
		return -1;
	}

	if (cond_ini(&ring_buf_empty, NULL) < 0) {
		printf("cond ring buf init error\n");
		return -1;
	}

	if (mutex_ini(&mutex, NULL) < 0) {
		printf("mutex init error\n");
		return -1;
	}

	printf("send_buffer_init succeed!\n");
	return 0;
}

int ring_buf_left_fm()
{
	int left_fm;
	lock(&mutex);
	left_fm = total_write_send - total_read_send;
	unlock(&mutex);

	return left_fm;
}

/**
* write_to_send_buffer() -  //
* @frame:                 //
* @frame_size:          //
*
*
*/

int write_nalu_to_ring_buffer(char *frame, int frame_size,
		unsigned int frame_num) {
	int current_write_len, send_buffer_left_length, left_length;
	static int write_len_send = 0, overflow_count = 0;
	Frame_header_send_buffer *frame_header_write_send_buffer;

	lock(&mutex);
	send_buffer_left_length = MAX_CONSTANT_SENDBUFFER_SIZE
			- (total_write_send - total_read_send);
	//printf("====send_buffer_left:%dbyte\n", send_buffer_left_length);
	while (send_buffer_left_length
			< frame_size + Frame_header_send_buffer_length
			|| send_buffer_left_length < 0) {
		printf("===send buffer full!\n");

		write_wait = 1;
		cond_wait(&ring_buf_full, &mutex);
		write_wait = 0;
		send_buffer_left_length = MAX_CONSTANT_SENDBUFFER_SIZE
				- (total_write_send - total_read_send);
	}

	left_length = MAX_CONSTANT_SENDBUFFER_SIZE - write_len_send;
	write_len_send += (frame_size + Frame_header_send_buffer_length);
	total_write_send += (frame_size + Frame_header_send_buffer_length);

//	printf("write fm:%d left_length:%d header len:%d fm len:%d\n", frame_num, left_length, \
			Frame_header_send_buffer_length, frame_size);

	if (write_len_send < MAX_CONSTANT_SENDBUFFER_SIZE) {
		//printf("1\n");
		current_write_len = frame_size;
		memcpy(write_ptr_send_buffer + Frame_header_send_buffer_length, frame,
				current_write_len);

		frame_header_write_send_buffer =
				(Frame_header_send_buffer *) write_ptr_send_buffer;
		memset(frame_header_write_send_buffer, 0,
				sizeof(Frame_header_send_buffer));

		frame_header_write_send_buffer->frame_num = frame_num;
		frame_header_write_send_buffer->frame_length = frame_size;
		frame_header_write_send_buffer->is_filled = 1;

		write_ptr_send_buffer += (Frame_header_send_buffer_length + frame_size);
		//printf("===succeed to write send_frame %d length:%d\n", frame_num, frame_size);
	}
	else if (write_len_send == MAX_CONSTANT_SENDBUFFER_SIZE) {
		//printf("2\n");
		current_write_len = frame_size;
		write_len_send = 0;
		memcpy(write_ptr_send_buffer + Frame_header_send_buffer_length, frame,
				current_write_len);

		frame_header_write_send_buffer =
				(Frame_header_send_buffer *) write_ptr_send_buffer;
		memset(frame_header_write_send_buffer, 0,
				sizeof(Frame_header_send_buffer));

		frame_header_write_send_buffer->frame_num = frame_num;
		frame_header_write_send_buffer->frame_length = frame_size;
		frame_header_write_send_buffer->is_filled = 1;
		write_ptr_send_buffer = send_buffer_ptr;
		//printf("===succeed to write send_frame %d length:%d\n", frame_num, frame_size);
	}
	else if (left_length <= Frame_header_send_buffer_length) {
		//printf("3\n");
		current_write_len = frame_size;
		write_len_send = frame_size + Frame_header_send_buffer_length;
		write_ptr_send_buffer = send_buffer_ptr;
		memcpy(write_ptr_send_buffer + Frame_header_send_buffer_length, frame,
				current_write_len);

		frame_header_write_send_buffer =
				(Frame_header_send_buffer *) write_ptr_send_buffer;
		memset(frame_header_write_send_buffer, 0,
				sizeof(Frame_header_send_buffer));

		frame_header_write_send_buffer->frame_num = frame_num;
		frame_header_write_send_buffer->frame_length = frame_size;
		frame_header_write_send_buffer->is_filled = 1;

		write_ptr_send_buffer = send_buffer_ptr + frame_size + Frame_header_send_buffer_length;
	}
	else {
		//printf("4\n");
		current_write_len = left_length - Frame_header_send_buffer_length;
		write_len_send = frame_size - current_write_len;

		frame_header_write_send_buffer =
				(Frame_header_send_buffer *) write_ptr_send_buffer;
		memset(frame_header_write_send_buffer, 0,
				sizeof(Frame_header_send_buffer));

		frame_header_write_send_buffer->is_segmentation = 1;
		frame_header_write_send_buffer->left_length = current_write_len;
		frame_header_write_send_buffer->frame_num = frame_num;
		frame_header_write_send_buffer->frame_length = frame_size;
		frame_header_write_send_buffer->is_filled = 1;

		memcpy(write_ptr_send_buffer + Frame_header_send_buffer_length, frame,
				current_write_len);
		memcpy(send_buffer_ptr, frame + current_write_len, write_len_send);

		write_ptr_send_buffer = send_buffer_ptr + write_len_send;
		//printf("===seg===succeed to write send_frame %d length:%d\n", frame_num, frame_size);
	}

	if (read_wait == 1) {
		cond_signal(&ring_buf_empty);
	}
	unlock(&mutex);

	return 0;
}

/**
* copy_frame_from_send_buffer() -  //
* @frame:                          //
*/

int read_nalu_from_ring_buffer(unsigned char *frame, unsigned int *frame_num) {
	unsigned int frame_length;
	static int copy_len_send = 0, underflow_count = 0;
	char end_of_send_buffer = 0;
	int left_length;
	Frame_header_send_buffer *frame_header_copy_send_buffer;

	lock(&mutex);

	frame_header_copy_send_buffer =
			(Frame_header_send_buffer *) copy_ptr_send_buffer;

	if (frame_header_copy_send_buffer->is_filled == 0) {
		printf("ring buffer empty!\n");
		read_wait = 1;
		cond_wait(&ring_buf_empty, &mutex);
		read_wait = 0;
	}


	frame_length = frame_header_copy_send_buffer->frame_length;
	*frame_num = frame_header_copy_send_buffer->frame_num;

	copy_len_send += (frame_length + Frame_header_send_buffer_length);
	total_read_send += (frame_length + Frame_header_send_buffer_length);
	left_length = MAX_CONSTANT_SENDBUFFER_SIZE - copy_len_send;

//	printf("read fm:%d cp len:%d left len:%d fm len:%d\n", \
			*frame_num, copy_len_send, left_length, frame_length);

	if (copy_len_send == MAX_CONSTANT_SENDBUFFER_SIZE) {
		end_of_send_buffer = 1;
	}

	if (frame_header_copy_send_buffer->is_segmentation == 1) {
		//printf("5\n");
		memcpy(frame, copy_ptr_send_buffer + Frame_header_send_buffer_length,
				frame_header_copy_send_buffer->left_length);
		copy_ptr_send_buffer = send_buffer_ptr;
		memcpy(frame + frame_header_copy_send_buffer->left_length,
				copy_ptr_send_buffer,
				frame_length - frame_header_copy_send_buffer->left_length);
		memset(copy_ptr_send_buffer, 0,
				frame_length - frame_header_copy_send_buffer->left_length);
		copy_ptr_send_buffer = send_buffer_ptr
				+ (frame_length - frame_header_copy_send_buffer->left_length);
		copy_len_send = frame_length
				- frame_header_copy_send_buffer->left_length;
		memset(frame_header_copy_send_buffer, 0,
				Frame_header_send_buffer_length
						+ frame_header_copy_send_buffer->left_length);
		//printf("***succeed to read send_frame segment:%d length:%d\n", *frame_num, frame_length);
	}
	else {
		memcpy(frame, copy_ptr_send_buffer + Frame_header_send_buffer_length,
				frame_length);
		memset(copy_ptr_send_buffer, 0,
				Frame_header_send_buffer_length + frame_length);
		if (end_of_send_buffer == 1) {
			//printf("6\n");
			copy_ptr_send_buffer = send_buffer_ptr;
			copy_len_send = 0;
			end_of_send_buffer = 0;
		}
		else {
			if (left_length <= Frame_header_send_buffer_length) {
				//printf("7\n");
				copy_len_send = 0;
				copy_ptr_send_buffer = send_buffer_ptr;
			}
			else {
				//printf("8\n");
				copy_ptr_send_buffer += (frame_length
						+ Frame_header_send_buffer_length);
			}
		}
		//printf("***succeed to read send_frame:%d length:%d\n", *frame_num, frame_length);
	}

	if (write_wait == 1) {
		cond_signal(&ring_buf_full);
	}
	unlock(&mutex);

	return frame_length;
}

void lock(pthread_mutex_t *mutex)
{
	pthread_mutex_lock(mutex);
}

void unlock(pthread_mutex_t *mutex)
{
	pthread_mutex_unlock(mutex);
}

int mutex_ini(pthread_mutex_t *mutex, void *arg)
{
	if (pthread_mutex_init(mutex, arg) < 0) {
		printf("pthread_mutex_init error!");
		return -1;
	}
	return 0;
}

int cond_ini(pthread_cond_t *cond, void *arg)
{
	if (pthread_cond_init(cond, arg) < 0) {
		perror("pthread_cond_init");
		return -1;
	}
	return 0;
}

void cond_signal(pthread_cond_t *cond)
{
	pthread_cond_signal(cond);
}

void cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
	pthread_cond_wait(cond, mutex);
}

