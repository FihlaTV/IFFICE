#ifndef _PANEL_H
#define _PANEL_H

#include "ait.h"
#include "addresslist.h"


typedef struct PANEL {
	char 	panel_name[256];	//面板的类型（音乐，视频,联系列表）
	int		num;				//联系人的数量
	int		file_work_id;		//当前工作联系人的位置
	int		file_on_id;			//当前移动到的联系人位置
	int		visible;			//面板可见或不可见
	int		pre_visible;		//之前的可见性 
	int		animation; 			//动画状态
	int		animation1;			//滚轮状态
	int		animation2;			//面板总标识状态
	int		noFind;				//列表容错参数
	int		top;				//最上面列表条的id
	int		rec_work_id;		
	double  list_long;
	double  w;					//列表长
	double  h;					//列表宽
	double  interval;			//列表间隔
	double  person_w;			//个人表长
	double  person_h;			//个人表宽
	double  person_interval;	//个人表间隔
	int		sub_file_on_id;
	int		sub_file_work_id;
	double  new_b;
	double  new_b1;
	double	DIST;
	MUSIC * music;				//各music文件
	PERSON* person;				//各联系人文件
}PANEL;							

PANEL *init_Panel(char *filename,char *type,char path[],double w,double h,double interval);
void  draw_Panel(PANEL *panel,double pos[]);
PANEL *free_Panel(PANEL *panel);

#endif
