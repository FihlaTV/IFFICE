#ifndef _MESSAGE_H
#define _MESSAGE_H

#define		MESSAGE_POOL_MAXSIZE	30

//message为bmp贴图
typedef struct {
	int		id;
	int		texture_id;
	int		state;		//0:未激活 1：激活
	double  pos[3];		//先不支持旋转 以后改成[3][4]的
	double  w,h;
	int		current_frame;
	int		twinkle_frame;
	double	alpha;
	double  adjust;
}MESSAGE;

int register_Message(double pos[],double w,double h,char *file);  //return message id
void call_Message(int id);
void over_Message(int id);
void draw_Message();

#endif 
