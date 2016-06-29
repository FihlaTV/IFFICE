//simple TCP server

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
FILE *fp;

int main(int argc,char *argv[]){
	int listen_sock;
	int communication_sock;
	sockaddr_in servAddr;
	sockaddr_in clntAddr;
	char buf[256];
	int len,sin_size;

	memset(&servAddr,0,sizeof(sockaddr_in));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=INADDR_ANY;
	servAddr.sin_port=htons(8051);

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
	if ((communication_sock=accept(listen_sock,(sockaddr *)&clntAddr,(socklen_t *)&sin_size))<0){
		perror("accept");
		exit(1);
	}
	printf("accept client %s\n",inet_ntoa(clntAddr.sin_addr));
	len=send(communication_sock,"welcome to my server\n",21,0);
	fp=fopen("talk_log","w+");
	while ((len=recv(communication_sock,buf,256,0))>0){
		buf[len]='\0';
		fputs(buf,fp);
		printf("%s\n",buf);
		if (send(communication_sock,buf,len,0)<0){
			perror("write");
			exit(1);
		}
	}
	close(communication_sock);
	close(listen_sock);
	fclose(fp);
	return 0;
}


