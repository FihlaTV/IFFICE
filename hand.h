#ifndef _HAND_H
#define _HAND_H


#include "mqoloader.h"
#include <AR/ar.h>

typedef struct HAND {
	double		 hand_to_cam3[3][4];	//matrix_0
	double		 hand_to_cam4[16];	
//	double		 cam_to_hand3[3][4];	//matrix_2	
	double		 hand_to_world3[3][4];	//matrix_1
	double		 hand_to_world4[16];

	char		 mqo_file[256];
	MQO_MODEL	 model_hand;
	int			 state;					//z
	int			 exist;					//0:not detected 1:detected
	char		 pat_file[256];
	int			 pat_id;
	double		 pat_width;
	double		 pat_center[2];
	double		 pos[3];				//the x y z in world
};

void init_Hand(void);
void check_Hand(ARMarkerInfo *marker_info,int marker_num,double world_trans[][4]);
int	 is_Hand_exist(void);
void draw_Hand(void);
//double **call_Handmatrix(int matrix_num);	//matrix_num 0,1

#endif
