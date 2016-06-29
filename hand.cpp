#include "hand.h"

#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/matrix.h>

#ifndef __APPLE__
#include <GL/glut.h>
#else
#include <OpenGL/glut.h>
#endif

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "object.h"

HAND hand;
extern int allstate;

extern AR_OBJ *AR_obj;

double hand_adjust[3][4]={{1.0,1.0,0.0,-15},{0.0,1.0,0.0,10.0},{0.0,0.0,1.0,25.0}};
double hand_adjust_f[3][4];

void init_Hand() {
	strcpy(hand.mqo_file,"Data/model/hand.mqo");
	strcpy(hand.pat_file,"Data/hand.pat");
	if ((hand.pat_id=arLoadPatt(hand.pat_file))<0) {
		printf("error while loading the hand pat!\n");
		exit(0);
	}
	if ((hand.model_hand=mqoCreateModel(hand.mqo_file,0.4))==NULL) {
		printf("error while loading the hand model!\n");
		exit(1);
	}
	hand.state=0;
	hand.exist=0;
	hand.pat_width=60.0;
	hand.pat_center[0]=0.0;
	hand.pat_center[1]=0.0;

	return ;
}

void check_Hand(ARMarkerInfo *marker_info,int marker_num,double world_trans[][4]) {
	int i,k;
	double iworld_trans[3][4];
	hand.exist=0;
	hand.pos[0]=hand.pos[1]=hand.pos[2]=0xFFFFFF;
	k=-1;
	for (i=0;i<marker_num;i++) {
		if (hand.pat_id==marker_info[i].id && marker_info[i].cf>=0.2) {
			if (k==-1) k=i;
			else if (marker_info[k].cf<marker_info[i].cf) k=i;
		}
	}
	if (k!=-1) {
		hand.exist=1;
		arGetTransMatCont(&marker_info[k],hand.hand_to_cam3,hand.pat_center,hand.pat_width,hand.hand_to_cam3);

//		arUtilMatInv()
		arUtilMatMul(hand.hand_to_cam3,hand_adjust,hand_adjust_f);

		argConvGlpara(hand.hand_to_cam3,hand.hand_to_cam4);

		arUtilMatInv(world_trans,iworld_trans);
//		arUtilMatMul(iworld_trans,hand.hand_to_cam3,hand.hand_to_world3);
		arUtilMatMul(iworld_trans,hand_adjust_f,hand.hand_to_world3);
		
	//	arUtilMatMul(hand_adjust,hand.hand_to_world3,hand.hand_to_world3);

		hand.pos[0]=hand.hand_to_world3[0][3];
		hand.pos[1]=hand.hand_to_world3[1][3];
		hand.pos[2]=hand.hand_to_world3[2][3];
		if (allstate==3) {
			if (AR_obj[3].pos!=1 && fabs(hand.pos[1]-AR_obj[3].mat_to_world3[1][3])<100)
			AR_obj[3].pos=1;
		}
		else AR_obj[3].pos=0;
	}
	else if (AR_obj[3].pos==1) AR_obj[3].pos=0;

	return ;
}

int is_Hand_exist(void){
	return hand.exist;
}

void draw_Hand() {
	if (hand.exist) {
		if (hand.state==0) {//选择状态
			glPushMatrix();
				glColor3f(0.8,0.8,0.8);
				glLoadMatrixd(hand.hand_to_cam4);
			/*	glBegin(GL_QUADS);
					glVertex3f(-40.0,-40.0,0.0);
					glVertex3f(40.0,-40.0,0.0);
					glVertex3f(40.0,40.0,0.0);
					glVertex3f(-40.0,40.0,0.0);
				glEnd();
			*/
				glRotatef(90.0,1.0,0.0,0.0);
				mqoCallModel(hand.model_hand);
			glPopMatrix();
		}
		if (hand.state==1) {//吸附状态
		}
		if (hand.state==2) {//翻页状态
		}
	}
	return ;
}
