#include "font.h"
extern "C" {
	#include "bmploader.h"
}
#include <stdio.h>
#include <stdlib.h>
#include <AR/ar.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

int texture_font,base,loop;
char file2[20]="Data/font.bmp";

void build_Font(){
    GLuint loop; /* Loop variable               */
    float cx;    /* Holds Our X Character Coord */
    float cy;    /* Holds Our Y Character Coord */
	int tmp_mo;
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&texture_font,file2);
    /* Creating 256 Display List */
    base  = glGenLists( 256 );
    /* Select Our Font Texture */
    glBindTexture( GL_TEXTURE_2D, texture_font );

    /* Loop Through All 256 Lists */
    for ( loop = 0; loop < 256; loop++ )
        {
	    /* NOTE:
	     *  BMPs are stored with the top-leftmost pixel being the
	     * last byte and the bottom-rightmost pixel being the first
	     * byte. So an image that is displayed as
	     *    1 0
	     *    0 0
	     * is represented data-wise like
	     *    0 0
	     *    0 1
	     * And because SDL_LoadBMP loads the raw data without
	     * translating to how it is thought of when viewed we need
	     * to start at the bottom-right corner of the data and work
	     * backwards to get everything properly. So the below code has
	     * been modified to reflect this. Examine how this is done and
	     * how the original tutorial is done to grasp the differences.
	     *
	     * As a side note BMPs are also stored as BGR instead of RGB
	     * and that is why we load the texture using GL_BGR. It's
	     * bass-ackwards I know but whattaya gonna do?
	     */
		tmp_mo=loop/16;
	    /* X Position Of Current Character */
	    cx = 1 - ( float )( loop % 16 ) / 16.0f;
	    /* Y Position Of Current Character */
	    cy = 1 - ( float )( loop / 16 ) / 16.0f;

            /* Start Building A List */
	    glNewList( base +  tmp_mo*16 +15-(loop%16) , GL_COMPILE );
	      /* Use A Quad For Each Character */
	    glBegin( GL_QUADS );
	        /* Texture Coord (Bottom Left) */
	        glTexCoord2f( cx - 0.0625, cy );
			/* Vertex Coord (Bottom Left) */
			glVertex3i( 0, 0, 8 );

			/* Texture Coord (Bottom Right) */
			glTexCoord2f( cx, cy );
			/* Vertex Coord (Bottom Right) */
			glVertex3i( 8, 0, 8 );

			/* Texture Coord (Top Right) */
			glTexCoord2f( cx, cy - 0.0625f );
			/* Vertex Coord (Top Right) */
			glVertex3i( 8, 0, 0 );

			/* Texture Coord (Top Left) */
			glTexCoord2f( cx - 0.0625f, cy - 0.0625f);
			/* Vertex Coord (Top Left) */
			glVertex3i( 0, 0, 0 );
	    glEnd( );

	      /* Move To The Left Of The Character */
	      glTranslated( 10, 0, 0 );
	    glEndList( );
        }
	glDisable(GL_TEXTURE_2D);
}

void print_Font(char ch,int set){
	if (set>1) set=1;
    /* Select our texture */
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);
	glColor3f(1.0,0.5,0.0); //注意这一句，后期修改，最好放在调用它的前面
	glTranslatef(0.0,0.0,0.0);

    glBindTexture( GL_TEXTURE_2D, texture_font );
	glDisable(GL_DEPTH_TEST);
    /* Choose The Font Set (0 or 1) */

    glListBase( base -32 + ( 128 * set ) );

    /* Write The Text To The Screen */
    glCallLists( 1, GL_BYTE, &ch );
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void print_String(char *s){
	int i;
	for (i=0;i<sizeof(s);i++){
		print_Font(s[i],0);
	}
}

void kill_Font(){
	glDeleteLists(base,256);
	return;
}
