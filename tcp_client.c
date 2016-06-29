//simple tcp client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc ,char *argv[]){
	int communication_sock;
	int len;
	sockaddr_in servAddr;
	char buf[256];

	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=inet_addr("222.25.135.22");
	servAddr.sin_port=htons(8058);
	if ((communication_sock=socket(AF_INET,SOCK_STREAM,0))<0){
		perror("socket");
		exit(1);
	}
	if (connect(communication_sock,(sockaddr *)&servAddr,sizeof(sockaddr))<0){
		perror("connect");
		exit(1);
	}
	printf ("connected to server\n");

	len=recv(communication_sock,buf,256,0);
	buf[len]='\0';
	printf("%s",buf);
	len=recv(communication_sock,buf,256,0);
	buf[len]='\0';
	printf("%s",buf);
	fgets(buf,256,stdin);
	buf[strlen(buf)]='\0';
	len = send(communication_sock,buf,strlen(buf),0);
	len=recv(communication_sock,buf,256,0);
	if (buf[0]=='O' && buf[1]=='K'){
	while (1){
		printf("Enter string to send:");
		fgets(buf,256,stdin);
		buf[strlen(buf)-1]='\0';
		if (!strcmp(buf,"quit")) break;
		len = send(communication_sock,buf,strlen(buf),0);
		//len=recv(communication_sock,buf,256,0);
		//buf[len]='\0';
		//printf(" received: %s\n",buf);
	}
	}
	close(communication_sock);
	return 0;
}
