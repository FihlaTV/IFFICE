#include "star.h"
#include <GL/glut.h>
#include <AR/ar.h>

#include <stdio.h>
#include <stdlib.h>

NEBULA nebula,nebula_helper;
double  angle_nebula=0;
double  speed_nebula=5;

extern void get_buff(char *buf,int n,FILE *fp);

void init_Nebula() {
	int ver,i;
	double rx,ry,rz;
	char buf[256];
	char *filename="Data/sphere_test";
	FILE *fp;
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}
	get_buff(buf,256,fp);
	sscanf(buf,"%d",&ver);
	nebula.num_of_stars=ver;
	arMalloc(nebula.star,STAR,ver);
	for (i=0;i<ver;i++){
		get_buff(buf,256,fp);
		sscanf(buf,"%lf %lf %lf",&rx,&rz,&ry); //交换y 和 z
		nebula.star[i].x=rx*0.8;
		nebula.star[i].y=ry*0.8;
		nebula.star[i].z=rz*0.8;
	}
	fclose(fp);
	return ;
}

void draw_Nebula(int scale) {
	int i;
	glPushMatrix();
		glRotatef(angle_nebula,0,0,1);
		angle_nebula+=speed_nebula;
		glColor3f(0.0,1.0,1.0);
		glBegin(GL_POINTS);
			for (i=0;i<nebula.num_of_stars;i++){
				glVertex3d(nebula.star[i].x*scale,nebula.star[i].y*scale,nebula.star[i].z*scale);
			}
		glEnd();
	glPopMatrix();
	return;
}

void destroy_Nebula() {
	free(nebula.star);
	return;
}
