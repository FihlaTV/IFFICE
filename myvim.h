#ifndef _MYVIM_H
#define _MYVIM_H

#include "bookshelf.h"

void creat_Paper(char *filename);
void write_Paper(char ch);
void write_Paper_string(char *s);
void read_Paper(PAPER *paper);
void print_Paper(char *s);
void erase_Paper(void);
void close_Paper(void);
int  isPaper_exist(void);
void draw_Paper(void);
void draw_String(char *buf,double len);

void init_Talkmybuffer(void);
void write_Talkmybuffer(char ch);
void erase_Talkmybuffer(void);

#endif
