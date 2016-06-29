#ifndef _ACTION_H
#define _ACTION_H

#define MAX_ACTION_NUM 5    //每个动作物体最大动作数量  （后面记得整理到config）

#include "mqoloader.h"

typedef void (*ACTION_CALLBACK)(void); //动作完成时的回调函数

typedef struct {
	int		action_id;   //动作编号 -1 时不动 
	int		obj_id;      //对应object编号,调用obj的坐标位置
	int		now_frame;   //当前帧
	int		unit;		 //
	int		act_num;	 //动作数量
	int		hold;		 //停在某帧
	MQO_SEQUENCE *act_sequence; //容器
	int		act_frame[MAX_ACTION_NUM];		//动作i的帧数 
	double  act_scale[MAX_ACTION_NUM];		//动作i的scale
	char    act_file[MAX_ACTION_NUM][256];  //动作文件路径
	ACTION_CALLBACK callback;   //动作完成的回调函数，初始化为NULL
}ACTION_OBJ;

typedef struct {
	ACTION_OBJ *action_obj; //动画物体
	int			act_obj_num;//动画物体数量
}ACTIONS;					//动作

void action_Loading(char *filename); //loading
void action_Play(void);     //状态检测机制
void action_Setplay(int action_obj_id,int action_id,int hold,int start_frame,int unit); //启动动作
void action_Setcallback(int action_obj_id,ACTION_CALLBACK action_callback);//注册动画物体action_obj_id的回调函数
void ninja_AI(int count);
void action_Free(void);		//释放内存

#endif
