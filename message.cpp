#include "message.h"

extern "C" {
#include "bmploader.h"
}
#include <AR/ar.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include <stdio.h>
#include <string.h>

MESSAGE msg_pool[MESSAGE_POOL_MAXSIZE];
int		msg_num=0;

int register_Message(double pos[],double w,double h,char *file){
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&msg_pool[msg_num].texture_id,file);
	glDisable(GL_TEXTURE_2D);
	msg_pool[msg_num].id=msg_num;
	msg_pool[msg_num].state=0;
	msg_pool[msg_num].pos[0]=pos[0];
	msg_pool[msg_num].pos[1]=pos[1];
	msg_pool[msg_num].pos[2]=pos[2];
	msg_pool[msg_num].w=w;
	msg_pool[msg_num].h=h;
	msg_pool[msg_num].current_frame=0;
	msg_pool[msg_num].twinkle_frame=100;
	msg_pool[msg_num].alpha=1.0;
	msg_pool[msg_num].adjust=-0.02;
	return (msg_num++);
}

void call_Message(int id){
	msg_pool[id].state=1;
	return ;
}

void over_Message(int id){
	msg_pool[id].state=0;
	msg_pool[id].current_frame=0;
	msg_pool[id].alpha=1.0;
	msg_pool[id].adjust=-0.02;
	return ;
}

void draw_Texture(int id){
//	glPushMatrix();
//		glTranslatef(msg_pool[id].pos[0],msg_pool[id].pos[1],msg_pool[id].pos[2]);
		if (msg_pool[id].current_frame<msg_pool[id].twinkle_frame) {
			glEnable(GL_BLEND);
			glDisable(GL_DEPTH_TEST);
			glColor4f(1.0,1.0,1.0,msg_pool[id].alpha);
		}
		glBindTexture(GL_TEXTURE_2D,msg_pool[id].texture_id);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0, 0.0); glVertex3f(msg_pool[id].pos[0]-msg_pool[id].w/2, msg_pool[id].pos[1], msg_pool[id].pos[2]  );
			glTexCoord2f(1.0, 0.0); glVertex3f(msg_pool[id].pos[0]+msg_pool[id].w/2, msg_pool[id].pos[1], msg_pool[id].pos[2]  );
			glTexCoord2f(1.0, 1.0); glVertex3f(msg_pool[id].pos[0]+msg_pool[id].w/2, msg_pool[id].pos[1], msg_pool[id].pos[2]+msg_pool[id].h  );
			glTexCoord2f(0.0, 1.0); glVertex3f(msg_pool[id].pos[0]-msg_pool[id].w/2, msg_pool[id].pos[1], msg_pool[id].pos[2]+msg_pool[id].h  );
		glEnd();
		if (msg_pool[id].current_frame<msg_pool[id].twinkle_frame) {
			if (msg_pool[id].alpha>=1)   msg_pool[id].adjust=-0.02;
			if (msg_pool[id].alpha<=0.5) msg_pool[id].adjust=0.02;
			msg_pool[id].alpha+=msg_pool[id].adjust;
			msg_pool[id].current_frame++;
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}
//	glPopMatrix();
	return ;
}

void draw_Message(){
	int i;
	glEnable(GL_TEXTURE_2D);
	for (i=0;i<msg_num;i++) {
		if (msg_pool[i].state) draw_Texture(i);
	}
	glDisable(GL_TEXTURE_2D);
	return ;
}
