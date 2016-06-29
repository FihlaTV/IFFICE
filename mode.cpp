#include "mode.h"
#include "mqoloader.h"
#include "mymove.h"
#include "object.h"
extern "C" {
#include "bmploader.h"
}

#include <stdio.h>

#include <AR/ar.h>

#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

extern AR_OBJ *AR_obj;
extern int obj_num;
extern int kiss;


char *mqo_floor="Data/floor.mqo";
double x_scale_f=0.1,y_scale_f=0.1,z_scale_f=0.1;
MQO_MODEL floor_model;
bool floor_flag=false;
int  floor_mode=-1;
int  now_mode=0;  //0 work 1 fun
mode_function mode_f=NULL;
//int  desktop_texture;
//char desktop_texture_file[]="Data/p/desk.bmp";

void mode_Init(){
	if ((floor_model=mqoCreateModel(mqo_floor,1))==NULL){
		printf("cannot find the model %s\n",mqo_floor);
		exit(1);
	}
//	glEnable(GL_TEXTURE_2D);
//	loadGLTexture(&desktop_texture,desktop_texture_file);
//	glDisable(GL_TEXTURE_2D);

	return ;
}

void mode_SetFunction(mode_function mode_f_) {
	mode_f=mode_f_;
	return ;
}

void move_callback0(){
	int i;
	for (i=0;i<obj_num;i++){
		if (AR_obj[i].visible){
			AR_obj[i].visible=0;
		}
		else AR_obj[i].visible=1;
	}
	mode_Translation(2);
	return ;
}

void move_callback1(){
	floor_mode=1;
	return ;
}

void mode_Translation(int mode){
	now_mode=1-now_mode;
	int i;
	if (mode==0){
		y_scale_f=0.1;
		floor_mode=0;
		floor_flag=true;
	}
	if (mode==1){
		for (i=0;i<obj_num;i++){
			if (AR_obj[i].visible){
				SetFly(&AR_obj[i],1000,1500,20,AR_obj[i].mat_to_hide);
			}
		}
		SetFunction(move_callback0);
	}
	if (mode==2){
		for (i=0;i<obj_num;i++){
			if (AR_obj[i].visible){
				SetFly(&AR_obj[i],1000,1500,20,AR_obj[i].mat_to_hand3);
			}
		}
		SetFunction(move_callback1);
	}

	return ;
}

void mode_Drawfloor(){
	if (floor_flag){
		if (floor_mode==0){
			if (y_scale_f<1.0) y_scale_f+=0.3;
			else {
				y_scale_f=1.0;
				floor_mode=-1;
				mode_Translation(1);
			}
		}
		if (floor_mode==1){
			if (y_scale_f>0.1) y_scale_f-=0.3;
			else {
				y_scale_f=0.1;
				floor_mode=-1;
				floor_flag=false; //over
				if (mode_f!=NULL) {mode_f();mode_f=NULL;}
			}
		}
		glRotatef(90.0,1.0,0.0,0.0);
		glScaled(y_scale_f*8,y_scale_f*3,y_scale_f*6);
		//glEnable(GL_LIGHTING)
		mqoCallModel(floor_model);
	}
	return ;
}
