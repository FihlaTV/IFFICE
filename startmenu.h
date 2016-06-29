#ifndef _STARTMENU_H
#define _STARTMENU_H

#include "button.h"
#include "mqoloader.h"

typedef struct STARTMENU {
	char		name[256];
	int			active;
	int			state;				//0:静止 1:up -1 down
	int			complete;			//每次旋转60度
	int			application;
	double		radius;
	int			rank;				//菜单等级，主菜单为0级
	BUTTON		*button[10];
	MQO_MODEL	menu[7];
};

void init_Startmenu(void);		//init 调用，暂时使用固定模式
void draw_Startmenu(void);		//draw

#endif
