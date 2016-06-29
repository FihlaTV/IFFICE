// a simple UDP server
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc,char *argv[]){
	int s,len;
	sockaddr_in my_addr;
	sockaddr_in remote_addr;
	int sin_size;
	char buf[256];

	memset(&my_addr,0,sizeof(my_addr));
	my_addr.sin_family=AF_INET;
	my_addr.sin_addr.s_addr=INADDR_ANY;
	my_addr.sin_port=htons(8050);

	if ((s=socket(AF_INET,SOCK_DGRAM,0))<0){
		perror("socket");
		exit(1);
	}
	if (bind(s,(sockaddr *)&my_addr,sizeof(sockaddr))<0){
		perror("bind");
		exit(1);
	}
	sin_size=sizeof(sockaddr_in);
	printf("waiting for a packet..\n");
	if (!(len=recvfrom(s,buf,256,0,(sockaddr *)&remote_addr,(socklen_t *)&sin_size))<0){
		perror("recvfrom");
		exit(1);
	}
	printf("received\n");
	buf[len]='\0';
	printf("contents:%s\n",buf);
	close(s);
	return 0;
}
