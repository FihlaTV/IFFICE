#include "myvim.h"
#include "font.h"
#include "bookshelf.h"
#include "myshell.h"
//#include "talk_thread.h"
//#include "addresslist.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

extern int kiss;
FILE *myvim_fp=NULL;
char myvim_buffer[2048];
char talk_mybuffer[256];
int  talk_mylen;
int  myvim_len;
int  myvim_step;
int  read_flag=false;
extern int paper_texture;

int isPaper_exist(){
	if (myvim_fp==NULL) return 0;
	else return 1;
}

void creat_Paper(char *filename){
	if ((myvim_fp=fopen(filename,"w+"))==NULL) {
		printf("File connot be created!\n");
		return;
	}
	myvim_len=0;
	memset(myvim_buffer,'\0',sizeof(myvim_buffer));
	glTranslatef(0.0,100.0,100.0);
	myvim_step=30;
	return;
}

void init_Talkmybuffer(){
	memset(talk_mybuffer,'\0',sizeof(talk_mybuffer));
	talk_mylen=0;
	return ;
}
void write_Talkmybuffer(char ch){
	talk_mybuffer[talk_mylen++]=ch;
	return ;
}
void erase_Talkmybuffer(){
	if (talk_mylen>0){
		talk_mylen--;
		talk_mybuffer[talk_mylen]='\0';
	}
	return;
}

void read_Paper(PAPER *paper){
	if ((myvim_fp=fopen(paper->path,"r"))==NULL){
		printf("File connot be loaded!\n");
		exit(1);
	}
	memset(myvim_buffer,'\0',sizeof(myvim_buffer));
	strcpy(myvim_buffer,paper->buffer1);
	myvim_len=strlen(myvim_buffer);
	fclose(myvim_fp);
	myvim_fp=fopen(paper->path,"a");
	read_flag=true;
	return ;
}

void write_Paper(char ch){
	myvim_buffer[myvim_len++]=ch;
	return;
}

void write_Paper_string(char *s){
	strcat(myvim_buffer,s);
	myvim_len+=strlen(s);
	return ;
}

void print_Paper(char *s){
	char cmd[32]="lp ";
	strcat(cmd,s);
	shell_exec(cmd);
	return ;
}

void erase_Paper(){
	myvim_len--;
	myvim_buffer[myvim_len]='\0';
	return;
}

void close_Paper(){
	myvim_buffer[myvim_len]='\0';
	if (!read_flag) fputs(myvim_buffer,myvim_fp);
	else read_flag=false;
	fclose(myvim_fp);
	myvim_fp=NULL;
	return;
}

void move_paper(){
	int tmp=30-myvim_step;
	glTranslatef(0.0,myvim_step*4-40.0,100.0);
	glColor4f(0.5,0.5,0.5,0.0);
	glBegin(GL_QUADS);
		glVertex3f(-tmp*2.0,0.0,-tmp*3.0);
		glVertex3f( tmp*2.0,0.0,-tmp*3.0);
		glVertex3f( tmp*2.0,0.0, tmp*3.0);
		glVertex3f(-tmp*2.0,0.0, tmp*3.0);
	glEnd();
	myvim_step--;
	return;
}

void draw_Paper(){
	int i;
	if (myvim_step!=0) {move_paper();return;}
	else kiss=0;
	glTranslatef(0.0,3.0,150.0);
	glColor3f(1.0,0.5,0.0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,paper_texture);
	glBegin(GL_QUADS);	
		glTexCoord2f(0.0,0.0);glVertex3f( -150.0,0.0,-70.0);
		glTexCoord2f(1.0,0.0);glVertex3f(  150.0,0.0,-70.0);
		glTexCoord2f(1.0,1.0);glVertex3f( 150.0,0.0, 70.0);
		glTexCoord2f(0.0,1.0);glVertex3f(-150.0,0.0, 70.0);
	glEnd();
	glTranslatef(-100.0,-2.0,40.0);
/*	int j=0;
	for (i=0;i<myvim_len;i++){
		if (myvim_buffer[i]=='\n') {
			glTranslatef((j-i)*6.0,0.0,-8.0); //每个字符占6mm
			j=i+1;
			continue;
		}
		print_Font(myvim_buffer[i],0);
		glTranslatef(-4.0,0.0,0.0);
	}
*/
	draw_String(myvim_buffer,200);
}

void draw_String(char *buf,double len){
	int i,j,k,len_of_buf;
	len_of_buf=strlen(buf);
	j=0;
	k=(int)(len/4-2);

	//glPushMatrix();
	
	for (i=0;i<len_of_buf;i++){
		if (buf[i]=='\n' || i-j==k) {
			glTranslatef((j-i)*4.0,0.0,-8.0); //每个字符占4mm
			if (i-j==k && buf[i]!='\n') {print_Font(buf[i],0);glTranslatef(-6.0,0.0,0.0);j=i;}
			else
			j=i+1;
			continue;
		}
		if (buf[i]!='\n'){
			print_Font(buf[i],0);
			glTranslatef(-6.0,0.0,0.0);
		}
	}

	//glPopMatrix();
	return ;
}

