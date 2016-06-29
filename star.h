//星云激活特效
#ifndef _STAR_H
#define _STAR_H

typedef struct {
	double x,y,z;
}STAR;

typedef struct {
	int		num_of_stars;
	STAR	*star;
}NEBULA;	//星云

void init_Nebula(void);
void draw_Nebula(int scale);
void destroy_Nebula(void);

#endif
