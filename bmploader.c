#include "bmploader.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <AR/ar.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif



static unsigned int getint(FILE *fp) {
	int c,c1,c2,c3;
	//get 4 bytes
	c=getc(fp);
	c1=getc(fp);
	c2=getc(fp);
	c3=getc(fp);

	return ((unsigned int) c)+(((unsigned int ) c1) <<8)+(((unsigned int )c2)<<16)+(((unsigned int)c3) << 24);
}

static unsigned int getshort(FILE *fp) {
	int c,c1;

	//get 2 bytes
	c=getc(fp);
	c1=getc(fp);
	return ((unsigned int )c)+(((unsigned int)c1) << 8);
}

//quick and dirty bitmap loader .. for 24 bit bitmaps with 1 plane only.
int imageLoad(char *filename,IMAGE *image) {
	FILE *file;
	unsigned long size;				//size of the image in bytes
	unsigned long i;				//standard counter
	unsigned short int planes;		//number of planes in image(must be 1)
	unsigned short int bpp;			//number of bits per pixel(must be 24)
	char temp;						//used to convert bgr to rgb color
	if ((file=fopen(filename,"rb"))==NULL) {
		printf("File Not Found: %s\n",filename);
		return 0;
	}

	//seek through the bmp header ,up to the width/height:
	fseek(file,18,SEEK_CUR);
	//no 100% errorchecking anymore!!!
	//read the width
	image->sizeX = getint(file);
	printf("Width of %s: %lu\n",filename,image->sizeX);
	//read the height
	image->sizeY = getint(file);
	printf("Height of %s: %lu\n",filename,image->sizeY);
	//calculate the size (assuming 24 bits or 3 bytes per pixel)
	size = image->sizeX * image->sizeY * 3;
	//read the planes
	planes = getshort(file);
	if (planes !=1) {
		printf("Planes from %s is not 1: %u\n",filename,planes);
		return 0;
	}
	
	bpp = getshort(file);
	if (bpp !=24) {
		printf("Bpp from %s is not 24 : %u\n",filename,bpp);
		return 0;
	}
	
	fseek(file,24,SEEK_CUR);
	//read  the data
	image->data=(char *)malloc(size);
	if (image->data==NULL) {
		printf("Error allocating memory for color-corrected image data\n");
		return 0;
	}
	if ((i=fread(image->data,size,1,file))!=1) {
		printf("Error reading image data from %s.\n",filename);
		return 0;
	}

	//bgr->rgb
	for (i=0;i<size;i+=3) {
		temp = image->data[i];
		image->data[i]=image->data[i+2];
		image->data[i+2]=temp;
	}

	return 1;
}

void loadGLTexture(int *texture,char *filename) {
	//load texture
	IMAGE *image;
	//allocate space for texture
	image=(IMAGE *)malloc(sizeof(IMAGE));
	if (image == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	if (!imageLoad(filename,image)) {
			exit(1);
	}

	//create texture,linear 
	//filtered texture
	glGenTextures(1,texture);
	glBindTexture(GL_TEXTURE_2D,*texture); //2d texture
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,0,3,image->sizeX,image->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,image->data);
}
