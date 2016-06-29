#include "mymove.h"
#include "object.h"

#include <AR/ar.h>
#include <AR/param.h>
#include <AR/gsub.h>

#include <GL/glut.h>

double YF_FlyWay[2000]; //存储路径

function move_f=NULL;
int  move_flag=0;

void ObjectFly(AR_OBJ *object ,int objectnum)
{
	int i,s,t;
//2 是object 数量
	for(i=0; i<objectnum; i++){
		if((object[i].state == 1) && (object[i].Locked == 0)){
			object[i].CurrentStep = object[i].StartStep;
			move_flag=1;
			for(s=0; s<3; s++)
				for(t=0; t<4; t++){
					object[i].FlyDeltaMat[s][t] = (object[i].end_mat_to_world3[s][t] - object[i].mat_to_world3[s][t]) / (double)((object[i].EndStep - object[i].StartStep) / (object[i].FlyStep));
				}

			object[i].FlyDeltaMat[0][3] = 0;

			object[i].scale_FlyWay = (object[i].end_mat_to_world3[0][3] - object[i].mat_to_world3[0][3]) / (double)( YF_FlyWay[object[i].EndStep] - YF_FlyWay[object[i].StartStep] );  

			object[i].Locked = 1;
		}
	}

	for(i=0; i<objectnum; i++){
		if((object[i].state == 1) && (object[i].Locked == 1)){
			if(object[i].CurrentStep < object[i].EndStep){
				for(s=0; s<3; s++)
					for(t=0; t<4; t++){
					object[i].mat_to_world3[s][t] = object[i].mat_to_world3[s][t] + object[i].FlyDeltaMat[s][t];
				}

				object[i].mat_to_world3[0][3] = object[i].mat_to_world3[0][3] + ( YF_FlyWay[object[i].CurrentStep + object[i].FlyStep] - YF_FlyWay[object[i].CurrentStep] ) * object[i].scale_FlyWay;


				argConvGlpara(object[i].mat_to_world3, object[i].mat_to_world4);
				object[i].CurrentStep = object[i].CurrentStep + object[i].FlyStep;
			}
			else{
				object[i].CurrentStep = object[i].EndStep;
				for(s=0; s<3; s++)
					for(t=0; t<4; t++){
					object[i].mat_to_world3[s][t] = object[i].end_mat_to_world3[s][t];
				}
				argConvGlpara(object[i].mat_to_world3, object[i].mat_to_world4);
				object[i].state = 0;
				object[i].Locked = 0;
			}
		}
	}

	if (move_flag==1){
		bool tmp_flag=true;
		for (i=0;i<objectnum;i++)
			if (object[i].state==1) {tmp_flag=false;break;}
		if (tmp_flag) {
			move_flag=0;
			if (move_f!=NULL) move_f();
		}
	}

	return ;
}

void MatSet(double s[3][4], double num0,double num1,double num2,double num3,double num4,double num5,double num6,double num7,double num8,double num9,double num10,double num11)
{	
	s[0][0] = num0;
	s[0][1] = num1;
	s[0][2] = num2;
	s[0][3] = num3;
	s[1][0] = num4;
	s[1][1] = num5;
	s[1][2] = num6;
	s[1][3] = num7;
	s[2][0] = num8;
	s[2][1] = num9;
	s[2][2] = num10;
	s[2][3] = num11;
}

void SetFunction(function ff){
	move_f=ff;
	return;
}

void SetFly(AR_OBJ *object,int start,int end,int step,double end_mat[3][4])
{
	int i,j;
	if(object->Locked == 0){
		for(i=0;i<3;i++)
			for(j=0;j<4;j++){
				object->end_mat_to_world3[i][j] = end_mat[i][j];
			}
		object->StartStep = start;
		object->EndStep = end;
		object->FlyStep = step;
		object->state = 1;
	}

}


void CalculatePath(void)
{
	double angle=0;
	double Pi=3.1415926;
	int x,y;

	angle = Pi/1000;
	for(x=0;x<250;x++){
		YF_FlyWay[x] = cosf(angle*x);
	}

	for(x=250;x<500;x++){
		YF_FlyWay[x] = -YF_FlyWay[499-x];
	}
	
	for(x=500;x<750;x++){
		YF_FlyWay[x] = -YF_FlyWay[x-500];
	}

	for(x=750;x<1000;x++){
		YF_FlyWay[x] = YF_FlyWay[999-x];
	}

	for(x=1000;x<2000;x++){
		YF_FlyWay[x] = x - 1000;
	}
}
