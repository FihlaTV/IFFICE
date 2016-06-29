#ifndef _CDBOX_H
#define _CDBOX_H

typedef struct {
	char	name[256];
	int		id;
	char	path[256];
	int		state;
	int		visible;
}CD;

typedef struct {
	char	name[256];
	int		id;
	int		numofcds;
	int		state;
	int		visible;
	CD		*cd;
	int		activecdid;
	int		sizeX,sizeY;
}CDBOX;

CDBOX *init_Cdbox(CDBOX *cdbox,char *name);

#endif
