#include "talk_thread.h"
#include "addresslist.h"
#include "myvim.h"
#include "ait.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

extern "C"{
#include "bmploader.h"
}

#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

//simple TCP talk

TALK_BUFFER talk_buffer;
int talk_texture;
char talk_texture_file[]="Data/talking.bmp";
bool talk_init_flag=true;
int  common_sock;
double talk_alpha=1.0;
double talk_adjust=-0.02;
int  talk_hi=0;

extern int talk_flag;
extern int persons;
extern PERSON person[1];
extern Mix_Chunk *ring;
extern char talk_mybuffer[1024];
extern int  talk_mylen;


void init_talk_buffer(TALK_BUFFER *talk_buffer){
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&talk_texture,talk_texture_file);
	glDisable(GL_TEXTURE_2D);
	return ;
}

void *talk_pid(void *n){
	int com_sock;
	int len,sin_size;
	com_sock=*(int *)n;
	common_sock=com_sock;
	char buf[256];
	//fp=fopen("./talk_log","w+");
	//fputs("begin to write\n",fp);
	
//	if (fp==NULL) printf("create talk_log error!\n");
	while ((len=recv(com_sock,buf,256,0))>0){
		buf[len-2]='\n';
		buf[len-1]='\0';
	//	printf("len : %d\n",len);
		pthread_mutex_lock(&talk_buffer.lock);
		strcpy(talk_buffer.buf[talk_buffer.writepos],buf);
		strcpy(talk_buffer.name[talk_buffer.writepos],person[0].name);
		talk_buffer.writepos=(talk_buffer.writepos+1)%10;
		pthread_mutex_unlock(&talk_buffer.lock);
		talk_hi=100;
	}
	talk_flag=0;
	close(com_sock);
	printf("done!\n");
}

void *talk_main(void *){
	FILE *fp=NULL;
	void *tmp;
	int listen_sock;
	int communication_sock;
	sockaddr_in servAddr;
	sockaddr_in clntAddr;
	char buf[256];
	int len,sin_size,i;
	pthread_t tmp_id;	

	memset(&servAddr,0,sizeof(sockaddr_in));
	servAddr.sin_family=AF_INET;
	servAddr.sin_addr.s_addr=INADDR_ANY;
	servAddr.sin_port=htons(8059);

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

//	while () 循环接受 目前只是点对点
	while ((communication_sock=accept(listen_sock,(sockaddr *)&clntAddr,(socklen_t *)&sin_size))>=0){
		printf("accept client %s\n",inet_ntoa(clntAddr.sin_addr));
		len=send(communication_sock,"welcome to my server\nplease tell me your name:",46,0);
		len=recv(communication_sock,buf,256,0);
		printf("%d\n",len);
		buf[len-2]='\0';
		talk_flag=0;
		//printf("%s %s\n",buf,person[0].name);
		for (i=0;i<persons;i++)
			if (strcmp(person[i].name,buf)==0)  {talk_flag=1;break;}
		if (talk_flag){
			printf("%s\n",buf);
			len=send(communication_sock,"OK\n",4,0);
			//talk_flag=1;	
			talk_init_flag=true;
			//init_talk_buffer(&talk_buffer);
			pthread_mutex_init(&talk_buffer.lock,NULL);
			talk_buffer.readpos=0;
			talk_buffer.writepos=0;
			talk_buffer.drawpos=0;

			len=pthread_create(&tmp_id,NULL,talk_pid,(void *)&communication_sock);
		}
		else {
			printf("NO\n");
			len=send(communication_sock,"NO\n",4,0);
			close(communication_sock);
		}
	}

	//fp=fopen("./talk_log","w+");
	//fputs("begin to write\n",fp);


//	if (fp==NULL) printf("create talk_log error!\n");
/*	while ((len=recv(communication_sock,buf,256,0))>0){
		buf[len-2]='\n';
		buf[len-1]='\0';
		printf("len : %d\n",len);
		pthread_mutex_lock(&talk_buffer.lock);
		strcpy(talk_buffer.buf[talk_buffer.writepos],buf);
		talk_buffer.writepos=(talk_buffer.writepos+1)%10;
		pthread_mutex_unlock(&talk_buffer.lock);
	}
*/

//	close(communication_sock);
	close(listen_sock);
	
	printf("done!\n");
	//fclose(fp);
	return tmp;
}

void draw_talking(){
	int i;
	if (talk_mybuffer[talk_mylen-1]=='\n'){
		pthread_mutex_lock(&talk_buffer.lock);
		strcpy(talk_buffer.buf[talk_buffer.writepos],talk_mybuffer);
		strcpy(talk_buffer.name[talk_buffer.writepos],"Me");
		talk_buffer.writepos=(talk_buffer.writepos+1)%10;
	/*	talk_buffer.readpos=(talk_buffer.readpos+1)%10;
		if (talk_init_flag) {
			if (talk_buffer.readpos>=4) talk_init_flag=false;
		}
		else{
			talk_buffer.drawpos=(talk_buffer.drawpos+1)%10;
		}
*/
		pthread_mutex_unlock(&talk_buffer.lock);
		talk_mybuffer[talk_mylen]='\0';
		send(common_sock,talk_mybuffer,talk_mylen,0);
		init_Talkmybuffer();
	}
	glTranslatef(-20.0,50.0,80.0);
	//glRotatef(45.0,0.0,0.0,1.0);
	if (talk_hi>0) {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glColor4f(1.0,1.0,1.0,talk_alpha);
	}
	glEnable(GL_TEXTURE_2D);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, talk_texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0  );
		glTexCoord2f(1.0, 0.0); glVertex3f(128, 0.0, 0.0  );
		glTexCoord2f(1.0, 1.0); glVertex3f(128, 0.0, 96   );
		glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 0.0, 96   );
	glEnd();
	if (talk_hi>0) {
		if (talk_alpha>=1) talk_adjust=-0.02;
		if (talk_alpha<=0.5) talk_adjust=0.02;
		talk_alpha+=talk_adjust;
		talk_hi--;
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	glBindTexture(GL_TEXTURE_2D, person[0].head_texture);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0); glVertex3f(5.0, -2.0, 45.0  );
		glTexCoord2f(1.0, 0.0); glVertex3f(45 , -2.0, 45.0  );
		glTexCoord2f(1.0, 1.0); glVertex3f(45 , -2.0, 85    );
		glTexCoord2f(0.0, 1.0); glVertex3f(5.0, -2.0, 85    );
	glEnd();
	//50-60 写名字
	//65-130正文
	if (talk_buffer.readpos!=talk_buffer.writepos){
		pthread_mutex_lock(&talk_buffer.lock);
		Mix_PlayChannel(-1,ring,0);
		if (talk_init_flag) {
			if (talk_buffer.readpos>=4) talk_init_flag=false;
		}
		else{
			talk_buffer.drawpos=(talk_buffer.drawpos+1)%10;
		}
	//	printf("%s",talk_buffer.buf[talk_buffer.readpos]);
		talk_buffer.readpos=(talk_buffer.readpos+1)%10;
		pthread_mutex_unlock(&talk_buffer.lock);
	}
	
	glPushMatrix();
	glTranslatef(65,0.0,80);

	if (talk_init_flag){
		for (i=0;i<talk_buffer.readpos;i++){
			int tmp_len=strlen(talk_buffer.name[i]);
			//printf("%d\n",strlen(person[0].name));
			glTranslatef((tmp_len+1)*(-4.0),0.0,0.0);
			draw_String(talk_buffer.name[i],65);
			glTranslatef(4.0,0.0,0.0);
			draw_String(talk_buffer.buf[i],65);
		}
	}
	else{
		for (i=0;i<5;i++){
			int tmp_len=strlen(talk_buffer.name[(talk_buffer.drawpos+i)%10]);
			glTranslatef((tmp_len+1)*(-4.0),0.0,0.0);
			draw_String(talk_buffer.name[(talk_buffer.drawpos+i)%10],65);
			glTranslatef(4.0,0.0,0.0);
			draw_String(talk_buffer.buf[(talk_buffer.drawpos+i)%10],65);
		}
	}
	glPopMatrix();

	glTranslatef(65,0.0,10);
	int tmp_len=strlen("Me");
	//printf("%d\n",strlen(person[0].name));
	glTranslatef((tmp_len+1)*(-4.0),0.0,0.0);
	draw_String("Me",65);

	if (talk_mylen!=0){
		glTranslatef(4.0,0.0,0.0);

		draw_String(talk_mybuffer,65);
	}
	
	glDisable(GL_TEXTURE_2D);		
	return ;
}
