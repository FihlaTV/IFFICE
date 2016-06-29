#ifndef _TALK_THREAD_H
#define _TALK_THREAD_H

//#include "addresslist.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//通信缓冲区结构
struct TALK_BUFFER {
	char buf[10][256];	  //实际缓存数组
	char name[10][256];    //谁说的话
	pthread_mutex_t lock; //互斥体 lock 用于对缓冲区的互斥操作
	int	readpos,writepos,drawpos; //读写指针
};

void init_talk_buffer(TALK_BUFFER *talk_buffer);
void *talk_main(void *);
void draw_talking(void);

#endif
