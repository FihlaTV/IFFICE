#ifndef _BMPLODER_H
#define _BMPLODER_H

typedef struct {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
}IMAGE;


//getint and getshort are help functions to load the bitmap byte by byte
//static unsigned int getint(FILE *fp);
//static unsigned int getshort(FILE *fp);
//加载一张bmp，句柄放在image里，返回1成功，0失败
int imageLoad(char *filename,IMAGE *image);
//制作纹理贴图
void loadGLTexture(int *texture,char *filename);

#endif
