#include "talk_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_t talk_id;
int tmp,i=30;

int main(){
	if ((tmp=pthread_create(&talk_id,NULL,talk_main,NULL))!=0){
		printf("create pthread error!\n");
		exit(1);
	}
	printf("create a pthread\n");

	while (i){
		usleep(1000);
		if(talk_buffer.readpos!=talk_buffer.writepos){
			pthread_mutex_lock(&talk_buffer.lock);
		//	Mix_PlayChannel(-1,ring,0);
			if (talk_buffer.buf[talk_buffer.readpos][0]=='q') {
				i=0;
				close(communication_sock);
				close(listen_sock);
			}
			printf("%s\n",talk_buffer.buf[talk_buffer.readpos]);
			talk_buffer.readpos=(talk_buffer.readpos+1)%10;
			pthread_mutex_unlock(&talk_buffer.lock);
		}
	}
	return 0;
}
