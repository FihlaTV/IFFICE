#ifndef _ADDRESSLIST_H
#define _ADDRESSLIST_H

//#include "panel.h"


typedef struct PERSON{
	char name[256];     
	int  id;			//从0开始
	char phone[256];	
	char email[256];
	char picture[256];	//头像图片路径
	int	 head_texture;
};



//PANEL *Peo_init_Panel(char *type,double w,double h,double interval);


//void init_Person(void);

//int persons=1;
//PERSON person[]={"lihao",123456,"13484585026","Data/picture/lihao"};

#endif
