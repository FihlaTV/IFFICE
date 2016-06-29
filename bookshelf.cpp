#include "bookshelf.h"
#include "mqoloader.h"
#include "config.h"
#include "myshell.h"
#include "myvim.h"
extern "C" {
#include "bmploader.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <AR/ar.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

//extern MQO_MODEL model_book;
extern int kiss;
int  paper_texture;
char paper_texture_file[]="Data/p/xinzhi0.bmp";
char path[20]="Data/book/book0/"; //第14个字符是0，
char path_of_paper[256];
char cmd[20]="ls Data/book/book0/"; //第17个字符
char *myshell="myshell.txt";
//SHELF shelf;

//读取一行文件（最多255个字符）
static void get_buff(char *buf,int n,FILE *fp){
	char *ret;
	for (;;){
		ret=fgets(buf,n,fp);
		if (ret==NULL) return ;
		if (buf[0]!='\n' && buf[0]!='#') return ; //skip blank lines and comments.
	}
}

//初始化book
BOOK	*bookInit(BOOK *book,FILE *fp,int maxn){
	char buf[256];
	char paper_name[16][256];
	int	 n,i,j,num_of_paper;
	FILE *tmp_fp;
//	shell_exec(cmd);
	arMalloc(book,BOOK,maxn);
	printf("register the book\n");
	get_buff(buf,256,fp);
	if (sscanf(buf,"%d",&n)!=1){
		fclose(fp);exit(1);
	}

	for (i=0;i<n;i++){
		get_buff(buf,256,fp);
		strcpy(book[i].name,buf);
		
		get_buff(buf,256,fp);
		if (sscanf(buf,"%d",&book[i].id)!=1){
			fclose(fp);exit(1);
		}
		book[i].firstpaperid=0;
		book[i].activepaperid=-1;
		book[i].movestate=0;
		for (j=0;j<2;j++){
			get_buff(buf,256,fp);
			if (sscanf(buf,"%lf%lf%lf",&book[i].pos[j][0],&book[i].pos[j][1],&book[i].pos[j][2])!=3){
				fclose(fp);exit(1);
			}
		}

		cmd[17]=i+'0';
		path[14]=i+'0';
	//	printf("%s\n",cmd);
		shell_exec(cmd);
		num_of_paper=0;
		if ((tmp_fp=fopen(myshell,"r"))==NULL){
			printf("Cannot open the file myshell.txt\n");
			exit(1);
		}
		while ((fscanf(tmp_fp,"%s",paper_name[num_of_paper]))!=EOF) {num_of_paper++;}
		fclose(tmp_fp);
		book[i].numofpapers=num_of_paper;
		arMalloc(book[i].paper,PAPER,num_of_paper);
		for (j=0;j<num_of_paper;j++){
			strcpy(path_of_paper,path);
			strcat(path_of_paper,paper_name[j]); //paper路径
			book[i].paper[j].id=j;
			book[i].paper[j].state=INACTIVE;
			book[i].paper[j].visible=VISIBLE;
			book[i].paper[j].pos[0]=100*j+5;
			book[i].paper[j].pos[1]=100*(j+1)-5;
			strcpy(book[i].paper[j].path,path_of_paper);
			if ((tmp_fp=fopen(path_of_paper,"r"))==NULL){
				printf("Cannot open the paper %s\n",path_of_paper);
				exit(1);
			}
			book[i].paper[j].buffer1[0]='\0';
			while ((fgets(buf,256,tmp_fp))!=NULL) {
				if (buf[0]=='#') strcpy(book[i].paper[j].buffer0,buf);
				else strcat(book[i].paper[j].buffer1,buf);
			}
			fclose(tmp_fp);
		}
	}
	return book;
}

//初始化shelf
SHELF	*init_Shelf(SHELF *shelf,char *name) {
	FILE *fp;
	char buf[256];
	//shell_exec(cmd);
	arMalloc(shelf,SHELF,1);
	printf("Opening bookshelf file %s\n",name);
	
	if ((fp=fopen(name,"r"))==NULL){ 
		printf("cannot open the file %s\n",name);
		exit(1);
	}
	get_buff(buf,256,fp);
	strcpy(shelf->name,buf);
	get_buff(buf,256,fp);

	if (sscanf(buf,"%d",&shelf->id)!=1){
		fclose(fp);exit(1);
	}
	get_buff(buf,256,fp);
	if (sscanf(buf,"%d%d",&shelf->sizeX,&shelf->sizeY)!=2){
		fclose(fp);exit(1);
	}
	shelf->numofbooks=shelf->sizeX*shelf->sizeY;
	shelf->state=INACTIVE;
	shelf->visible=VISIBLE;
	shelf->activebookid=-1;

	arMalloc(shelf->book,BOOK,shelf->numofbooks);
	shelf->book=bookInit(shelf->book,fp,shelf->numofbooks);

	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&paper_texture,paper_texture_file);
	glDisable(GL_TEXTURE_2D);

	printf("doneshelf\n");
	return shelf;
}

void move_Paper(BOOK *book){
	int pid;
	if (book->movestate>0){
		for (pid=0;pid<book->numofpapers;pid++){
			book->paper[pid].pos[0]-=5;
			book->paper[pid].pos[1]-=5;
			if (book->paper[pid].pos[1]<=-5){
				book->paper[pid].pos[0]=100*(book->numofpapers-1)+5;
				book->paper[pid].pos[1]=100*book->numofpapers-5;
			}
		}	
		book->movestate--;
	}
	if (book->movestate<0){
		for (pid=0;pid<book->numofpapers;pid++){
			if (book->paper[pid].pos[1]>=100*book->numofpapers-5) {
				book->paper[pid].pos[1]=-5;
				book->paper[pid].pos[0]=-95;
			}
			book->paper[pid].pos[0]+=5;
			book->paper[pid].pos[1]+=5;
		}
		book->movestate++;
	}
	if (book->movestate==0) kiss=0;
}

//paper展开
void draw_Paper(PAPER *paper){
	glColor3d(1.0,1.0,1.0);

	glBegin(GL_QUADS);
		glVertex3f(-45,0.0,-70.0);
		glVertex3f( 45,0.0,-70.0);
		glVertex3f( 45,0.0, 70.0);
		glVertex3f(-45,0.0, 70.0);
	glEnd();
	glPushMatrix();
		glTranslatef(-45+5,0.0,40.0);
		draw_String(paper->buffer1,80);
	glPopMatrix();
	return ;
}


//绘制book
void draw_Book(BOOK *book){
//	float rot[4]={0.0,-20.0,-140.0,-20.0};
	//float trans=2*3.1415/360.0,trot;
	int i,pid;
	double x1,x2;
	PAPER *paper;
	glTranslatef(0.0,0.0,10.0);
	glColor3d(1.0,1.0,1.0);
/*
	glBegin(GL_LINES);
		glVertex3f(0.0,-40,1.0);
		glVertex3f(0.0, 40,1.0);
	glEnd();
	glColor3d(1.0,0.0,0.0);
	for (i=0;i<4;i++){
		//trot=rot[i]*trans;
		glRotatef(rot[i],0.0,1.0,0.0);
		glBegin(GL_QUADS);
			glVertex3f(0.0 ,-40.0, 0.0);
			glVertex3f(40.0,-40.0, 0.0);
			glVertex3f(40.0, 40.0, 0.0);
			glVertex3f(0.0 , 40.0, 0.0);
		glEnd();
	}

	glRotatef(180,0.0,1.0,0.0);
*/
	glTranslatef(0.0,80.0,150.0);
	glColor3d(1.0,0.5,0.0);		
	pid=book->firstpaperid;
	if (book->movestate!=0) move_Paper(book);
	for (i=0;i<book->numofpapers;i++){
		paper=&book->paper[pid];
		pid=(pid+1)%book->numofpapers;
		if (paper->pos[1]-150<=-150) {continue;}
		if (paper->pos[0]-150>= 150) continue;
		if (paper->pos[0]-150<=-150) x1=-150;
			else x1=paper->pos[0]-150;
		if (paper->pos[1]-150>= 150) x2= 150;
			else x2=paper->pos[1]-150;
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,paper_texture);
		glBegin(GL_QUADS);
			glTexCoord2f((x1+150)/300,0.0);glVertex3f(x1,0.0,-60.0);
			glTexCoord2f((x2+150)/300,0.0);glVertex3f(x2,0.0,-60.0);
			glTexCoord2f((x2+150)/300,1.0);glVertex3f(x2,0.0, 60.0);
			glTexCoord2f((x1+150)/300,1.0);glVertex3f(x1,0.0, 60.0);
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glPushMatrix();
			glTranslatef(x1+5,0.0,40.0);
			draw_String(paper->buffer0,x2-x1);
		glPopMatrix();
	}
	if (book->movestate==0){
		glColor3f(1.0,0.0,0.0);
		glLineWidth(2.0);
		glBegin(GL_LINES);
			glVertex3f(-50.0,0.0,-70.0);
			glVertex3f( 50.0,0.0,-70.0);

			glVertex3f( 50.0,0.0,-70.0);
			glVertex3f( 50.0,0.0, 70.0);

			glVertex3f( 50.0,0.0, 70.0);
			glVertex3f(-50.0,0.0, 70.0);

			glVertex3f(-50.0,0.0, 70.0);
			glVertex3f(-50.0,0.0,-70.0);
		glEnd();
		glLineWidth(1.0);
	}
	//if (book->activepaperid!=-1) {glTranslatef(0.0,-80.0,0.0);draw_Paper(&book->paper[book->activepaperid]);}
}
			

//绘制shelf
void draw_Shelf(SHELF *shelf){
	int i;
	if (!shelf->visible) return;
	//printf("%d\n",shelf->visible);
/*	glColor3d(0.0,0.0,1.0);
	glTranslatef(80.0,100.0,0.0);
	for (i=0;i<shelf->numofbooks;i++){
		if (shelf->activebookid==i) glTranslatef(0.0,0.0,20.0);
		glBegin(GL_QUADS);
			glVertex3d(shelf->book[i].pos[0][0],shelf->book[i].pos[0][1],shelf->book[i].pos[0][2]);
			glVertex3d(shelf->book[i].pos[0][0],shelf->book[i].pos[1][1],shelf->book[i].pos[0][2]);
			glVertex3d(shelf->book[i].pos[1][0],shelf->book[i].pos[1][1],shelf->book[i].pos[1][2]);
			glVertex3d(shelf->book[i].pos[1][0],shelf->book[i].pos[0][1],shelf->book[i].pos[1][2]);
		glEnd();
		if (shelf->activebookid==i) glTranslatef(0.0,0.0,-20.0);
		//mqoCallModel(model_book);
		//glTranslatef(50.0,0.0,0.0);

	}
	glTranslatef(-80.0,-100.0,0.0);
*/
	if (shelf->activebookid!=-1 && shelf->book[shelf->activebookid].state==ACTIVE) draw_Book(&shelf->book[shelf->activebookid]);
	return;
}
	
