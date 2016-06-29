#ifndef _PAINT_H
#define _PAINT_H

typedef struct {
	double Trm[3];
}DOT;

typedef struct {
	int num;
	DOT dot[1000];
}PAINT;

void init_Paint(void);
void draw_Paint(void);

#endif 
