#ifndef _TALK_THREAD_H
#define _TALK_THREAD_H

//simple TCP talk
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "addresslist.h"

//通信缓冲区结构
struct TALK_BUFFER {
	char buf[10][256];	  //实际缓存数组
	pthread_mutex_t lock; //互斥体 lock 用于对缓冲区的互斥操作
	int	readpos,writepos; //读写指针
};

TALK_BUFFER talk_buffer;
int listen_sock,communication_sock;


void init_talk_buffer(TALK_BUFFER *talk_buffer){
	pthread_mutex_init(&talk_buffer->lock,NULL);
	talk_buffer->readpos=0;
	talk_buffer->writepos=0;
}

extern int talk_flag;

void *talk_pid(void *n){
	int com_sock;
	int len,sin_size;
	com_sock=*(int *)n;
	char buf[256];
	//fp=fopen("./talk_log","w+");
	//fputs("begin to write\n",fp);
	
//	if (fp==NULL) printf("create talk_log error!\n");
	while ((len=recv(com_sock,buf,256,0))>0){
		buf[len]='\0';
	//	printf("len : %d\n",len);
		pthread_mutex_lock(&talk_buffer.lock);
		strcpy(talk_buffer.buf[talk_buffer.writepos],buf);
		talk_buffer.writepos=(talk_buffer.writepos+1)%10;
		pthread_mutex_unlock(&talk_buffer.lock);
	}
	close(com_sock);
	printf("done!\n");
}


void *talk_main(void *){
	FILE *fp=NULL;
	void *tmp;
	sockaddr_in servAddr;
	sockaddr_in clntAddr;
	char buf[256];
	int len,sin_size,i;
	bool talk_flag;
	pthread_t tmp_id;

	init_talk_buffer(&talk_buffer);
	memset(&servAddr,0,sizeof(sockaddr_in));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=INADDR_ANY;
	servAddr.sin_port=htons(8058);

	if ((listen_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))<0){
		perror("socket");
		exit(1);
	}

	if (bind(listen_sock,(struct sockaddr *)&servAddr,sizeof(struct sockaddr))<0){
		perror("bind");
		exit(1);
	}
	listen(listen_sock,10);

	sin_size=sizeof(struct sockaddr_in);
	while((communication_sock=accept(listen_sock,(sockaddr *)&clntAddr,(socklen_t *)&sin_size))>=0){
		printf("accept client %s\n",inet_ntoa(clntAddr.sin_addr));
		len=send(communication_sock,"welcome to my server\n",21,0);
		usleep(100);
		len=send(communication_sock,"name:\n",6,0);
		len=recv(communication_sock,buf,256,0);
		buf[len-1]='\0';
		talk_flag=false;
		for (i=0;i<persons;i++)
			if (strcmp(person[i].name,buf)==0)  {talk_flag=true;break;}
		if (talk_flag){
			printf("%s\n",buf);
			len=send(communication_sock,"OK",3,0);
			len=pthread_create(&tmp_id,NULL,talk_pid,(void *)&communication_sock);
		}
		else {
			printf("NO\n");
			len=send(communication_sock,"NO",2,0);
			close(communication_sock);
		}
	}
	close(listen_sock);
	//fclose(fp);
	return tmp;
}

#endif
