// Video specific
#include <avifile/avifile.h>
#include <avifile/version.h>

#if AVIFILE_MINOR_VERSION > 6
  #include <avifile/avm_default.h>
#else
  #include <avifile/default.h>
#endif

#include <avifile/image.h>
#include <avifile/StreamInfo.h>

//gl
#include <GL/glut.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include "videoavi.h"
#include "ait.h"

using namespace std;


GLuint texture_avi;
int msec;
int width;                      // width and height of video stream
int height;
double stream_fps = 0;          // fps (for playback, since we don't use avi player)

GLUquadricObj *quadratic;       // Storage For Our Quadratic Objects
avm::IReadFile *avifile = 0;      // represents the avi file
avm::IReadStream *avistream = 0;  // represents the avi stream
StreamInfo *streaminfo = 0;       // represents the avi streaminfo
CImage *image = 0;                // an image (provided by avifile)
uint8_t *ui_image_copy = 0;       // image data (for use with gluScaleImage and glTexImage2D)

bool flag_avi=false;

MUSIC avi_music;


void open_avi(char *filename){
	avifile=avm::CreateReadFile(filename);
	if (avifile->IsOpened())
		avistream=avifile->GetStream(0,avm::IReadStream::Video);
	else {printf("ERROR opening avifile\n");exit(1);}

	avistream->StartStreaming();
	streaminfo=avistream->GetStreamInfo();
	width=streaminfo->GetVideoWidth();
	height=streaminfo->GetVideoHeight();
	stream_fps=streaminfo->GetFps();

	ui_image_copy=new uint8_t[width*height*3*sizeof(uint8_t)];
	
	glGenTextures(1,&texture_avi);
	glEnable(GL_TEXTURE_2D);
    glBindTexture( GL_TEXTURE_2D, texture_avi );  // use previously created texture object and set options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	strcpy(avi_music.path,"Data/movie/test.wav");
	music_Play(&avi_music);

	flag_avi=true;
}

void avi_play(){
	int i;
	char temp;
	image=avistream->GetFrame(true);
	if (!avistream->Eof() && image){
		for (int j=0; j<height; ++j)
		  for (int p=0;p<width;p++)
			  for (int k=0;k<3;k++)
			  ui_image_copy[j*width*3+p*3+k]=*(image->At(k,height-j-1)+p);
	for (i=0;i<width*height;i+=3) {
		temp = ui_image_copy[i];
		ui_image_copy[i]=ui_image_copy[i+2];
		ui_image_copy[i+2]=temp;
	}	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture_avi);
	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,ui_image_copy);
	image->Release();
	}else {
		avistream->Seek(0);
	}
/*	glBegin(GL_QUADS);  
		glTexCoord2f(1.0 , 1.0 ); glVertex3f( 100.0f, -20.0f, 150.0f);
		glTexCoord2f(0.0 , 1.0 ); glVertex3f(-100.0f, -20.0f, 150.0f);
		glTexCoord2f(0.0 , 0.0 ); glVertex3f(-100.0f, -20.0f, 20.0f );
		glTexCoord2f(1.0 , 0.0 ); glVertex3f( 100.0f, -20.0f, 20.0f );
	glEnd(); 
*/

	/*
	 	glRotatef(-30,1.0,0.0,0.0);	
		glTranslatef(-30.0,-7.0,8.0);

		glBegin(GL_QUADS);					
			//glColor3f(1.0,0.0,0.0);
			glTexCoord2f(0.0,0.0); glVertex3f(2.0,0.0,2.0);
			glTexCoord2f(1.0,0.0); glVertex3f(58.0,0.0,2.0);
			glTexCoord2f(1.0,1.0); glVertex3f(58.0,0.0,33.0);
			glTexCoord2f(0.0,1.0); glVertex3f(2.0,0.0,33.0);
		glEnd();	

	 */

		glRotatef(-30,1.0,0.0,0.0);	
		glTranslatef(-63.0,-16.0,16.0);

		glBegin(GL_QUADS);					
			//glColor3f(1.0,0.0,0.0);
			glTexCoord2f(0.0,0.0); glVertex3f(4.0,0.0,4.0);
			glTexCoord2f(1.0,0.0); glVertex3f(116.0,0.0,4.0);
			glTexCoord2f(1.0,1.0); glVertex3f(116.0,0.0,66.0);
			glTexCoord2f(0.0,1.0); glVertex3f(4.0,0.0,66.0);
		glEnd();	
   glDisable(GL_TEXTURE_2D);	
}

void avi_stop(void){
	avistream->StopStreaming();
	flag_avi=false;
	music_Quit();
	return;
}

bool watching(void) {
	return flag_avi;
}
