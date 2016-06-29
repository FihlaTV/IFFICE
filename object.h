#ifndef _OBJECT_H
#define _OBJECT_H

#include "mqoloader.h"

typedef struct AR_OBJ {
	char	name[256];
	int		id;
	double	mat_to_cam4[16];
	double	mat_to_cam3[3][4];
	double	mat_to_world4[16];
	double  mat_to_world3[3][4];
	double  mat_to_hand4[16];
	double  mat_to_hand3[3][4];
	double	mat_to_world_reset[3][4];
	
	double  mat_to_hide[3][4];

	double  mat_from[3][4];	//变换
	double  mat_to[3][4];
	//int		step;			//当前动作完成到第一步
	
	double  end_mat_to_world3[3][4];
	double	FlyDeltaMat[3][4];			//每次移动的矩阵值

	int		Locked;						//飞行路径是否已被锁定

	int		StartStep;			//起始
	int		EndStep;			//终止
	int		CurrentStep;		//当前
	int		FlyStep;			//每次移动的步数
	double	scale_FlyWay;


	double	dis[3];
	int		is_can_move;	//是否可以被手板吸附
	int		pos;			//位置 0 : 世界 1：手板
	int		state;			//0:静止状态，可以接受命令 1：动画状态，不能接受指令；
	int		style;			//当state=1时，确定调动的特效函数
	int		visible;		//是否绘制
	int		active;			//0:功能未被激活  1：功能被激活，进入功能状态，此时底下显示激活贴图
	double	scale;			//mqo载入比例
	char	mqo_file[256];
	MQO_MODEL model;		//mqo句柄

	double  ActiveRange1[3],ActiveRange2[3]; // 左下近，右上远	
};

AR_OBJ *reg_object(AR_OBJ *AR_obj,char *filename,int &num);

//void object_init_test(AR_OBJECT *object)

#endif 
