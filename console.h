#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef struct {
	char	*name;
	int		id;
	int		visible;
	double	width,height;
	double  pos[2];
	double	color_R,color_G,color_B;
	double	alpha;
	int		state;
}SIGLE_CON;

typedef struct {
	char	*name;
	int		id;
	int		drawlist;
	int		state;
	int		visible;
	int		num_w,num_h;
	SIGLE_CON *con;
	double	alphe;
	int		activeid;
}CONSOLE;

//初始化控制面板
CONSOLE *init_Console(CONSOLE *tmp_CON,int num_w,int num_h,double width,double height);
// 注册一个面板绘制
void	register_Console(CONSOLE *console);
//渲染控制面板,在xz轴平面
void	draw_Console(CONSOLE *console);
//激活某个控件
void	active_Console_ID(CONSOLE *console,int id);

#endif
