#include "paint.h"
#include "hand.h"

#include <GL/glut.h>
#include <AR/ar.h>

extern HAND hand;

PAINT paint;

void init_Paint(){
	paint.num=0;
	return;
}

void draw_Paint(){
	double tmp_paint[16];

	if (is_Hand_exist() && hand.hand_to_world3[0][3]<250 && hand.hand_to_world3[0][3]>-250 && hand.hand_to_world3[1][3]<200 && hand.hand_to_world3[1][3]>-30){
		for (int i=0;i<3;i++)
				paint.dot[paint.num].Trm[i]=hand.hand_to_world3[i][3];
		paint.num++;
	}

	glColor3f(0.9,0.9,0.9);
	glBegin(GL_QUADS);
		glVertex3f(-250,-30,5.0);
		glVertex3f( 250,-30,5.0);
		glVertex3f( 250, 200,5.0);
		glVertex3f(-250, 200,5.0);
	glEnd();


	glColor3f(1.0,0.0,0.0);
	glPointSize(3.0);
	glBegin(GL_POINTS);
	for (int i=0;i<paint.num;i++)
		glVertex3f(paint.dot[i].Trm[0],paint.dot[i].Trm[1],paint.dot[i].Trm[2]);
	glEnd();
	glPointSize(1.0);
	return ;
}
