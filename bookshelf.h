#ifndef _BOOKSHELF_H
#define _BOOKSHELF_H

typedef struct {
	char	name[256];
	int		id;
	char	buffer0[256];
	char	buffer1[1024];
	char	path[256];
	double  pos[2];
	int		state;
	int		visible;
} PAPER;

typedef struct {
	char	name[256];
	int		id;
	char	*buffer;
	PAPER	*paper;
	int		numofpapers;
	int		firstpaperid;
	int		activepaperid;
	int		state;
	int		movestate;
	int		visible;
	double	pos[2][3];
} BOOK;

typedef struct {
	char	name[256];
	int		id;
	int		numofbooks;
	int		state;
	int		visible;
	BOOK	*book;
	int		activebookid;
	int		sizeX,sizeY;
} SHELF;

//void	bookInit(BOOK *book,FILE *fp);
SHELF	*init_Shelf(SHELF *shelf,char *name);
//void	paperList(BOOK *book);
//void	paperActive(PAPER *paper);
void	draw_Shelf(SHELF *shelf);
void	move_Paper(BOOK *book);

#endif
