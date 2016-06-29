// a simple UDP client
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc,char *argv[]){
	int s,len;
	sockaddr_in remote_addr;
	char buf[256];

	memset(&remote_addr,0,sizeof(remote_addr));
	remote_addr.sin_family=AF_INET;
	remote_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	remote_addr.sin_port=htons(8050);

	if ((s=socket(AF_INET,SOCK_DGRAM,0))<0){
			perror("socket");
			exit(1);
	}
	strcpy(buf,"You got it");
	if ((len=sendto(s,buf,strlen(buf),0,(sockaddr *)&remote_addr,sizeof(sockaddr)))<0){
		perror("sendto");
		exit(1);
	}
	printf("sending : '%s'\n",buf);
	close(s);
	return 0;
}
