#ifndef _HAIBAO_H
#define _HAIBAO_H

#include "mqoloader.h"

#define  ACTION_STAND  0
#define  ACTION_TEST   1

typedef struct {
	int		action;
	double  rot;
	double  pos[3];
}HAIBAO_STATE;

typedef struct {
	MQO_SEQUENCE	action;
	double			rotate[4];
	double			translate[3];
	int				now_frame;
	int				is_stop;
}HAIBAO_ACT;

int action_loading(char *haibao_conf);
void action_play(int action_id);
void action_free(void);

#endif
