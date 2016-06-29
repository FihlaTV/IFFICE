#ifndef _MYMOVE_H
#define _MYMOVE_H

#include "object.h"

typedef void (*function)(void);

void SetFly(AR_OBJ *object,int start,int end,int step,double end_mat[][4]);
void MatSet(double s[3][4], double num0,double num1,double num2,double num3,double num4,double num5,double num6,double num7,double num8,double num9,double num10,double num11);
void ObjectFly(AR_OBJ *object, int objectnum);
void SetFunction(function ff);
void CalculatePath(void);

#endif
