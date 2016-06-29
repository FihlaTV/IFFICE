#ifndef _MODE_H
#define _MODE_H

//#include "object.h"
typedef void (*mode_function)(void);

void mode_Init(void); //init 调用一次 载入floor
void mode_Translation(int mode); //mode 0:工作空间 1：娱乐空间 转换时将lock所有操作 kiss 调用参见myvim.cpp
void mode_Drawfloor(void);
void mode_SetFunction(mode_function mode_f_);

#endif
