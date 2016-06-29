#include "console.h"
extern "C"{
#include "bmploader.h"
}
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <AR/ar.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif


int texture[12];
int	test1;
char file[20]="Data/number/a.bmp";
//初始化一个控制面板
CONSOLE *init_Console(CONSOLE *tmp_CON,int num_w,int num_h,double width,double height){
	//CONSOLE *tmp_CON;
	if (tmp_CON==NULL)
		arMalloc(tmp_CON,CONSOLE,1);
	int		i,j,tmp;
	if (tmp_CON->con==NULL)
		arMalloc(tmp_CON->con,SIGLE_CON,num_w*num_h);
	//tmp_CON->name="";
	tmp_CON->id=0;
	tmp_CON->visible=INVISIBLE;
	tmp_CON->num_w = num_w;
	tmp_CON->num_h = num_h;
	tmp_CON->state=INACTIVE;
	tmp_CON->drawlist=-1;
	tmp_CON->activeid=-1;
	for (i=0;i<num_h;i++)
		for (j=0;j<num_w;j++) {
			tmp=j+num_w*i;
			tmp_CON->con[tmp].name=(char *)('a'+tmp);
			tmp_CON->con[tmp].id=tmp;
			tmp_CON->con[tmp].visible=TRUE;
			tmp_CON->con[tmp].width=width;
			tmp_CON->con[tmp].height=height;
			tmp_CON->con[tmp].color_R=0;
			tmp_CON->con[tmp].color_G=0;
			tmp_CON->con[tmp].color_B=0;
			tmp_CON->con[tmp].pos[0]=j*width;
			tmp_CON->con[tmp].pos[1]=i*height;
			tmp_CON->con[tmp].state=0;
		}
	return tmp_CON;
}

//注册一个面板绘制
void register_Console(CONSOLE *console){
	int i,j,tmp;
	char ch;
	SIGLE_CON *sigle_con;
	glEnable(GL_TEXTURE_2D);
	for (i=0;i<12;i++){
		//file[12]++;
		ch='a'+i;
		file[12]=ch;
		loadGLTexture(&texture[i],file);
	}
	loadGLTexture(&test1,"Data/bumps.bmp");
	console->drawlist=glGenLists(1);
	glNewList(console->drawlist,GL_COMPILE);
	for (i=0;i<console->num_h;i++)
		for (j=0;j<console->num_w;j++) {
			tmp=j+console->num_w*i;
			glBindTexture(GL_TEXTURE_2D,texture[tmp]);
			sigle_con=&console->con[tmp];
			glBegin(GL_QUADS);					
				glColor3f(sigle_con->color_R,sigle_con->color_G,sigle_con->color_B);
				glTexCoord2f(0.0,0.0);
				glVertex3f(sigle_con->pos[0],					0,	sigle_con->pos[1]);
				glTexCoord2f(1.0,0.0);
				glVertex3f(sigle_con->pos[0]+sigle_con->width-5,0,	sigle_con->pos[1]);
				glTexCoord2f(1.0,1.0);
				glVertex3f(sigle_con->pos[0]+sigle_con->width-5,0,	sigle_con->pos[1]+sigle_con->height-5);
				glTexCoord2f(0.0,1.0);
				glVertex3f(sigle_con->pos[0],					0,	sigle_con->pos[1]+sigle_con->height-5);
			glEnd();
		}
		glDisable(GL_TEXTURE_2D);
		glEndList();
}

//渲染控制面板,在xz轴平面
void draw_Console(CONSOLE *console){
	if (console->drawlist==-1) register_Console(console);
	glEnable(GL_TEXTURE_2D);
	if (console->activeid==-1)
		glCallList(console->drawlist);
	else
	   active_Console_ID(console,console->activeid);
	glDisable(GL_TEXTURE_2D);	
}

void active_Console_ID(CONSOLE *console,int id) {
	SIGLE_CON *sigle_con;
	if (id<0 || id >11) {printf("No id matched\n");return ;}
	glCallList(console->drawlist);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture[id]);
	sigle_con=&console->con[id];
	glPushMatrix();
	glTranslatef(0.0,-10.0,0.0);
	glBegin(GL_QUADS);					
		//glColor3f(1.0,0.0,0.0);
		glTexCoord2f(0.0,0.0);
		glVertex3f(sigle_con->pos[0]-5,					0,	sigle_con->pos[1]-5);
		glTexCoord2f(1.0,0.0);
		glVertex3f(sigle_con->pos[0]+sigle_con->width,0,	sigle_con->pos[1]-5);
		glTexCoord2f(1.0,1.0);
		glVertex3f(sigle_con->pos[0]+sigle_con->width,0,	sigle_con->pos[1]+sigle_con->height+5);
		glTexCoord2f(0.0,1.0);
		glVertex3f(sigle_con->pos[0]-5,					0,	sigle_con->pos[1]+sigle_con->height+5);
	glEnd();
	glPopMatrix();

/*****************************************
	glBindTexture(GL_TEXTURE_2D,test1);
	glPushMatrix();
	glTranslatef(0.0,30.0,0.0);
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);					
		//glColor3f(1.0,0.0,0.0);
		glTexCoord2f(0.0,0.0);
		glVertex3f(0.0,0.0,0.0);
		glTexCoord2f(1.0,0.0);
		glVertex3f(100.0,0.0,0.0);
		glTexCoord2f(1.0,1.0);
		glVertex3f(100.0,0.0,100.0);
		glTexCoord2f(0.0,1.0);
		glVertex3f(0.0,0.0,100.0);
	glEnd();
	glPopMatrix();

*****************************************/

	glDisable(GL_TEXTURE_2D);
	if (console->activeid!=id) console->activeid=id;
}

		
