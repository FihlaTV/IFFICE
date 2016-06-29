#include "action.h"
#include "object.h"

#include <AR/ar.h>
#ifndef __APPLE__
#	include <GL/glut.h>
#else
#	include <OpenGL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

ACTIONS actions;
extern AR_OBJ *AR_obj;
int		time=0;
int     speed=3;
extern  int now_mode;

extern void get_buff(char *buf,int n,FILE *fp);

inline void error_find(FILE *fp){
	printf("error while loading action\n");
	fclose(fp);
	return ;
}

void action_Loading(char *filename){
	FILE *fp;
	char buf[256];
	int  i,j,k;
	ACTION_OBJ *tmp_act;

	if ((fp=fopen(filename,"r"))==NULL) {
		printf("error while open the file %s\n",filename);
		return ;
	}

	get_buff(buf,256,fp);
	if (sscanf(buf,"%d",&actions.act_obj_num)!=1) {error_find(fp);exit(1);}

	arMalloc(actions.action_obj,ACTION_OBJ,actions.act_obj_num);

	if (actions.action_obj==NULL) {printf("malloc error\n");exit(1);}
	
	for (i=0;i<actions.act_obj_num;i++){
		tmp_act=&actions.action_obj[i];
		tmp_act->action_id = -1;
		tmp_act->now_frame = 0;
		tmp_act->callback = NULL;
		tmp_act->hold=-1;
		tmp_act->unit=1;

		get_buff(buf,256,fp);
		if (sscanf(buf,"%d",&tmp_act->obj_id)!=1) {error_find(fp);exit(1);}
		get_buff(buf,256,fp);
		if (sscanf(buf,"%d",&tmp_act->act_num)!=1) {error_find(fp);exit(1);}
		
		arMalloc(tmp_act->act_sequence,MQO_SEQUENCE,tmp_act->act_num);

		for (j=0;j<tmp_act->act_num;j++){
			get_buff(buf,256,fp);
			if (sscanf(buf,"%d %lf",&tmp_act->act_frame[j],&tmp_act->act_scale[j])!=2) {error_find(fp);exit(1);}
			get_buff(buf,256,fp);
			if (sscanf(buf,"%s",tmp_act->act_file[j])!=1) {error_find(fp);exit(1);}
			tmp_act->act_sequence[j]=mqoCreateSequence(tmp_act->act_file[j],tmp_act->act_frame[j],tmp_act->act_scale[j]);
		}
	}

	return ;
}

void action_Play(){
	int i,j,k;
	ACTION_OBJ *tmp_act;
	for (i=0;i<actions.act_obj_num;i++){
		if (actions.action_obj[i].action_id!=-1){
			tmp_act=&actions.action_obj[i];
			k=tmp_act->action_id;
			glPushMatrix();
				glMultMatrixd(AR_obj[tmp_act->obj_id].mat_to_world4);
				glRotatef(90.0,1.0,0.0,0.0);
				mqoCallSequence(tmp_act->act_sequence[k],tmp_act->now_frame);
				time++;
				if (tmp_act->now_frame!=tmp_act->hold && time%speed==0) 
				tmp_act->now_frame+=tmp_act->unit;
				if (tmp_act->now_frame%tmp_act->act_frame[k]==0){
					AR_obj[tmp_act->obj_id].visible=1;
					tmp_act->now_frame=0;
					tmp_act->action_id=-1;
					if (tmp_act->callback!=NULL)
						tmp_act->callback();
				}
			glPopMatrix();
		}
	}
	return ;
}

void action_Setplay(int action_obj_id,int action_id,int hold,int start_frame,int unit){
	actions.action_obj[action_obj_id].action_id=action_id;
	actions.action_obj[action_obj_id].hold=hold;
	actions.action_obj[action_obj_id].now_frame=start_frame;
	actions.action_obj[action_obj_id].unit=unit;
	AR_obj[actions.action_obj[action_obj_id].obj_id].visible=0;
	return ;
}

void action_Setcallback(int action_obj_id,ACTION_CALLBACK action_callback){
	actions.action_obj[action_obj_id].callback=action_callback;
	return ;
}

void ninja_AI(int count){
//	if (count%400==0 && actions.action_obj[2].action_id==-1 && now_mode==0) action_Setplay(2,1,-1,1,1);
	return ;
}

void action_Free(){
	int i,j;
	for (i=0;i<actions.act_obj_num;i++){
		for (j=0;j<actions.action_obj[i].act_num;j++){
			mqoDeleteSequence(actions.action_obj[i].act_sequence[j]);
		}
	}
	free(actions.action_obj);
	actions.action_obj=NULL;
	actions.act_obj_num=0;

	return;
}
