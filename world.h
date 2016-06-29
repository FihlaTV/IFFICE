#ifndef _WORLD_H
#define _WORLD_H

#include <AR/ar.h>

typedef struct {
	int		patt_id;
	double	width;
	double	center[2];
	double	extra_trans[3][4];
	double	trans[3][4];
	int		visible;
} BASE_POINT;

typedef struct {
	BASE_POINT *base_point;
	int			base_num;
	double		trans[3][4];
	double		cam_to_world[3][4];
} WORLD;

WORLD *build_the_world(const char *filename);
int   get_the_world(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int   get_the_world2(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int   get_the_world3(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int   get_the_world4(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int	  get_the_world5(WORLD *world);
void  destroy_the_world(WORLD *world);


#endif 
