#include "button.h"
extern "C" {
#include "bmploader.h"
}

#include <AR/ar.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

//int BUTTON_POOL::button_num=0;

BUTTON_POOL button_pool("ac",0,10);

BUTTON::BUTTON(char _name[],int _id,double _size_X,double _size_Z,char _texture[][256],double _x,double _y,double _z,void (*_callback)(void)) {
	int i;
	strcpy(name,_name);
	id=_id;
	size_X=_size_X;
	size_Z=_size_Z;
	glEnable(GL_TEXTURE_2D);
	for (i=0;i<3;i++){
		strcpy(texture[i],_texture[i]);
		loadGLTexture(&button_texture[i],texture[i]);
	}
	glDisable(GL_TEXTURE_2D);
	pos_x=_x; pos_y=_y; pos_z=_z;
	state=0;
	visible=0;
	worked=0;
	callback=_callback;
}

BUTTON::~BUTTON(){}

void BUTTON::set_Name(char *_name) {strcpy(name,_name);}
void BUTTON::set_Id(int _id) {id=_id;}
void BUTTON::set_Size(double _x,double _z) {size_X=_x;size_Z=_z;}
void BUTTON::set_Texture(char _texture[][256]) {	
	strcpy(texture[0],_texture[0]);
	strcpy(texture[1],_texture[1]);
	strcpy(texture[2],_texture[2]);
}
void BUTTON::set_Pos(double _x,double _y,double _z) {pos_x=_x;pos_y=_y;pos_z=_z;}
void BUTTON::set_State(int _state) {state=_state;}
void BUTTON::set_Visible(int _visible) {visible=_visible;}
void BUTTON::set_Callback(void (*_callback)(void)) {callback=_callback;}
	
void BUTTON::draw_Button(void) {
	//glPushMatrix();
	//	glTranslatef(pos_x,pos_y,pos_z);
		glEnable(GL_TEXTURE_2D);
		if (state==1) { 
			glEnable(GL_BLEND);
			glColor4f(1.0,1.0,1.0,0.5);
			glDisable(GL_DEPTH_TEST);
		}
		glBindTexture(GL_TEXTURE_2D,button_texture[state]);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0,0.0); glVertex3f(pos_x-size_X/2,pos_y,pos_z-size_Z/2);
			glTexCoord2f(1.0,0.0); glVertex3f(pos_x+size_X/2,pos_y,pos_z-size_Z/2);
			glTexCoord2f(1.0,1.0); glVertex3f(pos_x+size_X/2,pos_y,pos_z+size_Z/2);
			glTexCoord2f(0.0,1.0); glVertex3f(pos_x-size_X/2,pos_y,pos_z+size_Z/2);
		glEnd();
		if (state==1){
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
		}
		glDisable(GL_TEXTURE_2D);
	//glPopMatrix();
	if (state==2){
		if (callback!=NULL) callback();
		state=0;
	}
}

void BUTTON::check_State() {
	if (visible) draw_Button();
}

double BUTTON::caldis(double pos[]) {
	double a=pos[0]-pos_x,b=pos[1]-pos_y,c=pos[2]-pos_z;
	return (a*a+b*b+c*c*4);		// a^2+4b^2+4c^2<=5700
}


BUTTON_POOL::BUTTON_POOL(char _name[],int _id,int dl) {
	strcpy(name,_name);
	id=_id;
	deadline=dl;
	active_button_id=-1;
	preactive_button_id=-1;
	timecount=0;
	button_num=0;
}

BUTTON_POOL::~BUTTON_POOL() {}

BUTTON *BUTTON_POOL::register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void)) {
	//BUTTON *tmp_Button;
	button[button_num]=new BUTTON(name,button_num,sx,sy,t,x,y,z,_callback);
	button_num++;
	return button[button_num-1];
}

void BUTTON_POOL::draw_ButtonPool(void) {
	int i;
	for (i=0;i<button_num;i++)
		button[i]->check_State();
}

void BUTTON_POOL::check_ButtonPool(double pos[]) {
	int i,j;
	double tmp_min;
	if (timecount==0) active_button_id=-1;
	for (i=0;i<button_num;i++) 
		dis[i]=button[i]->caldis(pos);
	tmp_min=0xFFFFFF;j=-1;
	for (i=0;i<button_num;i++)
		if (dis[i]<tmp_min) {tmp_min=dis[i];j=i;}
	//printf("%lf ",tmp_min);
	if (j>=0 && tmp_min<=9000 && button[j]->visible) { //work place
		if (active_button_id==-1) {
		//	if (preactive_button_id!=j){
				active_button_id=j;
				preactive_button_id=j;
				button[j]->state=1;
				timecount=1;
		//	}
		}else {
			if (active_button_id==j) {
				timecount++;
				if (timecount>=deadline) {
					button[j]->state=2;
					timecount=0;
					active_button_id=-1;
				}
			}else {
				timecount=1;
				button[active_button_id]->state=0;
				active_button_id=preactive_button_id=j;
				button[j]->state=1;
			}
		}
	}else {
		if (active_button_id!=-1) {
			timecount--;
			if (timecount==0) {button[active_button_id]->state=0;active_button_id=-1;}
		}
	}
}

BUTTON *register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void)) {
	return button_pool.register_Button(name,sx,sy,t,x,y,z,_callback);
}

void draw_ButtonPool(){
	button_pool.draw_ButtonPool();
	return;
}

void check_ButtonPool(double pos[]){
	button_pool.check_ButtonPool(pos);
	return;
}
