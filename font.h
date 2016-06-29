#ifndef _FONT_H
#define _FONT_H

void build_Font(void);			  //导入字体，在init里调用
void print_Font(char ch,int set); //set=0 常规 set=1 斜体
void print_String(char *s);
void kill_Font(void);             //退出时调用

#endif
