/*----------------------------------------------------------------------
 * IFFICE 程序清单
 *
 * Copyright (c) 2010- XDU Embeded Lab
 * Auther Hao Li 	  roselone@acm.org
 * 		  Fang Yi     yifang@ieee.org
 *		  XiaoJie Jin peakjxj@yahoo.cn	
 *----------------------------------------------------------------------*/
 
 
/*----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
 
/*----------------------------------------------------------------------
 * file IFFICE.cpp
 * IFFICE 主进程
 *----------------------------------------------------------------------*/

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#ifndef __APPLE__
#include <GL/gl.h>
#include <GL/glut.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glut.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#include <AR/config.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/gsub_lite.h>

#include "header.h"
#include "config.h"

//摄像头配置
#ifdef _WIN32
char		   *vconf="Data\\WDM_camera_flipV.xml";
#else
char		   *vconf="";
#endif

int			    xsize,ysize;
int				thresh=100;
int				count=0;
int				talk_flag=0;
pthread_t		talk_id;
extern TALK_BUFFER talk_buffer;
Mix_Chunk		*ring;

//载入配置文件
char		    *cparam_name = "Data/camera_para.dat";
char			*object_conf="Data/object_conf";
char			*bookshelf_conf="Data/bookshelf_info";
char			*cdbox_conf="Data/cdbox_info";
char			*action_conf="Data/action_conf";

//载入marker
char		    *patt_name ="Data/patt.ar";
char			*world_name="Data/multi/marker.dat";
char			*patt_console="Data/hand.pat";
int			     patt_id[2];
double          patt_width[2]     = {70.0,90.0};
double          patt_center[2] = {0.0, 0.0};
double          patt_trans[3][4];
double			 patt_trans_console[3][4];

int				kiss=0;
int				allstate=0;
int				is_patt_console_exist=0;
int				delay=60,printer_tmp=0;
char			*tmp_cmd="ls ";

ARParam			 cparam;
CONSOLE			*console=NULL;
SHELF			*shelf=NULL;
CDBOX			*cdbox=NULL;
MQO_MODEL		model_girl,model_book;
WORLD			*world;
PANEL			*music_panel=NULL;
AR_OBJ			*AR_obj;
	
int				obj_num;
int				wander_mode=0;
int				picturetest;

extern bool		show_phone_panel; 	

float eye_angle=0.0,deltaAngle = 0.0,ratio;
float eye_x=0.0f,eye_y=100.75f,eye_z=65.0f;
float eye_lx=0.0f,eye_ly=0.0f,eye_lz=-1.0f;
int deltaMove = 0;
//------------------------------------------------------------

MUSIC           music_tmp;
BUTTON			*button_tmp;
char			tmp_path[3][256]={"Data/bumps.bmp","Data/bumps_.bmp","Data/bumps.bmp"};

void   init(void);
void   initGL(void);
void   cleanup(void);
void   keyEvent( unsigned char key, int x, int y);
void   mouseEvent(int button,int state,int x,int y);
void   motionEvent(int x,int y);
void   specialKeyEvent(int key,int x,int y);
void   mainLoop(void);
void   SetLight(void);  
void   draw( void );
void   visibility(int );
void   reShape(int w,int h);
void   drawQUAD(float,float,float,float,float);

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	init();
	initGL();
    arVideoCapStart();
	glutReshapeFunc(reShape);
    argMainLoop( mouseEvent, keyEvent, mainLoop );
	return (0);
}

//----------------------------------------------------
void orientMe(float ang) {
	eye_lx = sin(ang);
	eye_lz = -cos(ang);
}


void moveMeFlat(int i) {
	eye_x = eye_x + i*(eye_lx)*0.1;
	eye_z = eye_z + i*(eye_lz)*0.1;
}
//-------------------------------------------------------
//
//
void mouseEvent(int button,int state,int x,int y){
	if (state==GLUT_UP){
	}
	else if (state==GLUT_DOWN){
	}
	if (button==GLUT_WHEEL_UP)
		printf("%d,%d\n",x,y);
	return ;
}

void motionEvent(int x,int y){
	return ;
}

void   keyEvent( unsigned char key, int x, int y)
{
	int i;
	if (key==0x1b){ 
		//ESC退出 mainloop
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
	}

	if (kiss!=0) return;
	if (allstate==0){
	switch (key)
	{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '0':
		case 'Y':
		case 'B':
			if (console->state!=0)
			active_Console_ID(console,(int)(key-'1'));
			break;
		case 'c':
		case 'C':
			console->state=1-console->state;
			break;
		case 'm':
		case 'M':
			mode_Translation(0);
			break;
		case 'W':
		case 'w':
			wander_mode=1-wander_mode;
			break;
		case ' ':
			if (talk_flag){
				init_Talkmybuffer();
				allstate=8;
			}
			break;
		case 0xD:
			if (console->activeid!=-1){
				allstate=console->activeid+1;
				console->activeid=-1;
				console->state=INACTIVE;
			}
			if (allstate==1){
				shelf->activebookid=0;
				AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
				argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
			}
			if (allstate==2){
				creat_Paper("Data/test");
				kiss=2;
				glPushMatrix();
			}
			if (allstate==3){
				open_avi(chavi);	
			}
			if (allstate==4){
				cdbox->activecdid=0;
				AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
				argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
			}
			if (allstate==5){
				show_phone_panel=true;
			}
			break;
		case 0x8:
			console->state=INACTIVE;
			break;
		default:
			printf("%d\n",(int)key);
			break;
    }
	return ;
	}
	if (allstate==1){
		switch (key){
			case 0xD:
				if (shelf->book[shelf->activebookid].state!=ACTIVE)
					shelf->book[shelf->activebookid].state=ACTIVE;
				else {
					shelf->book[shelf->activebookid].activepaperid=(shelf->book[shelf->activebookid].firstpaperid+1)%shelf->book[shelf->activebookid].numofpapers;
					read_Paper(&shelf->book[shelf->activebookid].paper[shelf->book[shelf->activebookid].activepaperid]);
					allstate=2;
				}
				break;
			case 0x8:
				if (shelf->book[shelf->activebookid].state==ACTIVE){
				   if (shelf->book[shelf->activebookid].activepaperid!=-1)
					   shelf->book[shelf->activebookid].activepaperid=-1;
				   else
					shelf->book[shelf->activebookid].state=INACTIVE;
				}
				else {
					allstate=0;
					console->state=ACTIVE;
					AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
					shelf->activebookid=-1;
				}
				break;
			default:
				break;
		}
	}

	if (allstate==4){
		switch (key) {
			case 0xD:
				if (cdbox->cd[cdbox->activecdid].state!=ACTIVE){
					cdbox->cd[cdbox->activecdid].state=ACTIVE;
					char tmp_cmd_[256];
					strcpy(tmp_cmd_,tmp_cmd);
					strcat(tmp_cmd_,cdbox->cd[cdbox->activecdid].path);
					shell_exec(tmp_cmd_);
					if (music_panel!=NULL) music_panel=free_Panel(music_panel);
					music_panel=init_Panel("myshell.txt","music",cdbox->cd[cdbox->activecdid].path,100,10,2);
				}
				else {
				}
				break;
			case 0x8:
				if (cdbox->cd[cdbox->activecdid].state!=ACTIVE){
					allstate=0;
					console->state=ACTIVE;
					AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
					cdbox->activecdid=-1;
				}
				else {
					music_Quit();
					cdbox->cd[cdbox->activecdid].state=INACTIVE;
					music_panel=free_Panel(music_panel);
				}
				break;
			default:
				break;
		}
	}

	if (allstate==2){
		switch (key){
			case 0xD:
				if (isPaper_exist()) write_Paper('\n');
				else {
				}
				break;
			case 0x8:
				if (isPaper_exist()) {
					erase_Paper();
				}
				else allstate=0;
				break;
			default:
				write_Paper(key);
				break;
		}
	}
	if (allstate==3){
		switch (key){
			case 'q':
				allstate=0;
				avi_stop();
				break;
		}
	}
	if (allstate==8){
		if (!talk_flag) allstate=0;
		switch (key){
			case 0xD:
				write_Talkmybuffer('\n');
				allstate=0;
				break;
			case 0x8:
				erase_Talkmybuffer();
				break;
			default:
				write_Talkmybuffer(key);
				break;
		}
	}
	if (allstate==5){
		if (key=='q'){
			show_phone_panel=false;
			allstate=0;
		}
		else {
			if (key>='0' && key<='9' || key==0x8) select_Number(key);
			if (key=='p') calling();
			if (key=='b') phone_send(PO_INTERRUPT);
			if (key=='t') phone_send(PO_ANSWER);
		}
	}
}

void releaseKey(int key, int x, int y) {

	switch (key) {
		case GLUT_KEY_LEFT : 
		case GLUT_KEY_RIGHT : 
			deltaAngle = 0.0f;break;
		case GLUT_KEY_UP : 
		case GLUT_KEY_DOWN : 
			deltaMove = 0;break;
	}
}

void   specialKeyEvent(int key,int x,int y) {
	if (kiss!=0) return;
	if (wander_mode) {
		switch (key) {
			case GLUT_KEY_LEFT : 
				deltaAngle = -0.01f;break;
			case GLUT_KEY_RIGHT : 
				deltaAngle = 0.01f;break;
			case GLUT_KEY_UP : 
				deltaMove = 1;break;
			case GLUT_KEY_DOWN : 
				deltaMove = -1;break;
		}
		return ;
	}

	if (allstate==1){
		if (shelf->book[shelf->activebookid].state==ACTIVE){
			switch (key)
			{
				case GLUT_KEY_F1:
					break;
				case GLUT_KEY_LEFT:
					kiss=1;
					action_Setplay(0,0,-1,1,1);
					shelf->book[shelf->activebookid].movestate=20;
					break;
				case GLUT_KEY_RIGHT:
					break;
				case GLUT_KEY_UP:
				case GLUT_KEY_DOWN:
				default:
					break;
			}
		}
		else {
			switch (key){
				case GLUT_KEY_LEFT:
					AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
					shelf->activebookid=(shelf->activebookid-1)%shelf->numofbooks;
					if (shelf->activebookid<0) shelf->activebookid+=shelf->numofbooks;
					AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
					argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
					break;
				case GLUT_KEY_RIGHT:
					AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
					shelf->activebookid=(shelf->activebookid+1)%shelf->numofbooks;
					AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
					argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
					break;
				default:
					break;
			}
		}
	return;
	}
	if (allstate==2){
		if (key==GLUT_KEY_F2) {
			close_Paper();
			if (shelf->activebookid!=-1 && shelf->book[shelf->activebookid].state==ACTIVE)
			allstate=1;
			else allstate=0;
			glPopMatrix();
		}
		if (key==GLUT_KEY_F3) { 
			action_Setplay(1,0,-1,1,1);
		}
	}
	if (allstate==4){
		if (cdbox->cd[cdbox->activecdid].state==ACTIVE){
		}
		else {
			switch (key){
				case GLUT_KEY_LEFT:
					AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
					cdbox->activecdid=(cdbox->activecdid-1)%cdbox->numofcds;
					if (cdbox->activecdid<0) cdbox->activecdid+=cdbox->numofcds;
					AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
					argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
					break;
				case GLUT_KEY_RIGHT:
					AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
					argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
					cdbox->activecdid=(cdbox->activecdid+1)%cdbox->numofcds;
					AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
					argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
					break;
				default:
					break;
			}
		}
		return ;
	}
}

/* main loop 主循环 */
void mainLoop(void)
{
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             i,j, k,ms;
	float			s_elapsed;
	static int		ms_prev;
	double			iworldtrans[3][4];
	double			tmptrans[3][4];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /* 从摄像头获取一帧图像 */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count == 0 ) arUtilTimerReset();
    count++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	/* 检测marker */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }
    arVideoCapNext();

    /* 检测物体可见性 */
    k = -1;i=-1;
    for( j = 0; j < marker_num; j++ ) {

		if (patt_id[1] == marker_info[j].id) {
			if (i==-1) i=j;
			else if (marker_info[i].cf < marker_info[j].cf ) i=j;
		}
    }

    /* 获取坐标转换矩阵 */

	if (i==-1){
		is_patt_console_exist=0;
	}else {
		is_patt_console_exist=1;
		arGetTransMat(&marker_info[i],patt_center,patt_width[1],patt_trans_console);
	}

	if (!wander_mode){
	
		if (get_the_world(marker_info,marker_num,world)<0) {argSwapBuffers();return ;}
	}
	else {
		get_the_world5(world);
	}

	ObjectFly(AR_obj,obj_num);
	ninja_AI(count);	
    draw();
    
    //双缓冲模式
    argSwapBuffers();
    
    return ;
}

/* 初始化 */
void init( void )
{
    ARParam  wparam;
	double   tmp_mat[3][4];
	
    /* 读取视频流 */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* 初始化窗口 */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* 载入摄像头参数 */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    if( (patt_id[0]=arLoadPatt(patt_name)) < 0 ) {
        printf("pattern load error !!\n");
        exit(0);
    }
	
	if ((patt_id[1]=arLoadPatt(patt_console))<0) {
		printf("pattern load error !!\n");
		exit(0);
	}

	if ((world=build_the_world(world_name))==NULL ){
		printf("error while loading the world\n");
		exit(0);
	}
	printf("creat the world successful!\n");

    argInit( &cparam, 1.0, 0, 0, 0, 0 );
    
	//------------------------------------------------------
	//console init
	console=init_Console(console,3,4,50.0,30.0);
	register_Console(console); 
	//-------------------------------------------------------
	//mqo init
	mqoInit();
	if ((model_girl=mqoCreateModel(mqo_girl,0.05))==NULL) {
		printf("cannot find the model girl");
		exit(1);
	}
	AR_obj=reg_object(AR_obj,object_conf,obj_num);
	printf("%d\n",AR_obj[1].id);
	
	//phone panel init
	init_Phone();

	//shelf init
	shelf=init_Shelf(shelf,bookshelf_conf);
	//cdbox init
	cdbox=init_Cdbox(cdbox,cdbox_conf);
	//-----------------------------------------------------
	//font init
	build_Font();
	//-----------------------------------------------------
	//music init
	music_Init();
	ring=load_sound("Data/music_/msg.wav");
	
	//------------------------------------------------------
	//计算路径
	CalculatePath();
	mode_Init();
	//-------------------------------------------------------
	//init_person
	init_Person();
	//-------------------------------------------------------
	//init_action
	action_Loading(action_conf);

	//-------------------------------------------------------
	init_talk_buffer(&talk_buffer);
	if ((tmp=pthread_create(&talk_id,NULL,talk_main,NULL))!=0){
		printf("create pthread error!\n");
		exit(1);
	}
	printf("create a pthread\n");

	return ;
}

void initGL()
{
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glutSpecialFunc(specialKeyEvent);
	glutIgnoreKeyRepeat(1);
	glutSpecialUpFunc(releaseKey);
	glDisable(GL_TEXTURE_2D);
	return;
}

void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
	kill_Font();
	action_Free();
	destroy_the_world(world);
	music_Quit();
	exit_Phone();
	return ;
}

void reShape(int w,int h) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (h==0) h=1;
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return;
}

void visibility(int visible) {
	if (visible==GLUT_VISIBLE) {
		glutIdleFunc(mainLoop);
	}else {
		glutIdleFunc(NULL);
	}
	return ;
}

void print_bitmap_string(void* font,const char* s)
{
   if (s && strlen(s)) {
      while (*s) {
         glutStrokeCharacter(font, *s);
         s++;
      }
   }
}


void SetLight(void)
{
	GLfloat light_diffuse[]  = { 0.9, 0.9, 0.9, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[]  = { 0.3, 0.3, 0.3, 0.1 };	
	GLfloat light_position[] = { 100.0, -200.0, 200.0, 0.0 };	


	glLightfv( GL_LIGHT0, GL_DIFFUSE,  light_diffuse );	 
	glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
	glLightfv( GL_LIGHT0, GL_AMBIENT,  light_ambient );	
	glLightfv( GL_LIGHT0, GL_POSITION, light_position ); 

	glShadeModel( GL_SMOOTH );	
	glEnable( GL_LIGHT0 );		


}

void draw( void )
{
    double    gl_para[16];
	double	  gl_para_test[16];
	double	  ipatt_trans[3][4];
	double	  patt_test[3][4];
	double	  tmp_pos[]={0.0,0.0,0.0};

	int i,j;
    char ch;	
    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
	glMatrixMode(GL_MODELVIEW);
	//-------------------------------------------------------------
	argConvGlpara(world->trans,gl_para);
	glLoadMatrixd( gl_para );
	if (wander_mode){
		gluLookAt(eye_x, eye_y, eye_z, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);
	
		if (deltaMove)
			moveMeFlat(deltaMove);
		if (deltaAngle) {
			eye_angle += deltaAngle;
			orientMe(eye_angle);
		}
	}
	//--------------------------------------------------------------
	//console
	arUtilMatInv(world->trans,ipatt_trans);
	if (is_patt_console_exist){
		argConvGlpara(patt_trans_console,gl_para_test);
		arUtilMatMul(ipatt_trans,patt_trans_console,patt_test);
		tmp_pos[0]=patt_test[0][3];
		tmp_pos[1]=patt_test[1][3];
		tmp_pos[2]=patt_test[2][3]-30;
		AR_obj[3].pos=1;
	}
	else AR_obj[3].pos=0;

	//--------------------------------------------------------------
	if (wander_mode){
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glColor4f(0.8,0.9,0.9,0.9);
		glBegin(GL_QUADS);
			glVertex3f(-300,-200,0);
			glVertex3f(300,-200,0);
			glVertex3f(300,200,0);
			glVertex3f(-300,200,0);	
		glEnd();
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}

	//-----------------------------------------------------
	//draw the console
	if (console->state){
		//SetLight();
		glPushMatrix();
		glTranslatef( -75.0, -50.0, 0.0 );
		draw_Console(console);
		glPopMatrix();
	}
	//--------------------------------------------------
	//draw the shelf
	if (shelf->visible){
		glPushMatrix();
		draw_Shelf(shelf);
		glPopMatrix();
	}

	if (music_panel!=NULL){
		glTranslatef(0.0,0.0,30);
		draw_Panel(music_panel,tmp_pos);
		glTranslatef(0.0,0.0,-30.0);
	}
	//------------------------------------------------
	//draw mqo model
	glPushMatrix();
		SetLight();
		glEnable(GL_LIGHTING);
		glRotatef(90.0,1.0,0.0,0.0);
		glTranslatef(100.0,60.0,70.0);
		glDisable(GL_LIGHTING);
	glPopMatrix();

	//draw object
	glEnable(GL_LIGHTING);
		for (i=0;i<obj_num;i++)
			if (AR_obj[i].visible){
				if (AR_obj[i].pos==0){
				glPushMatrix();
					glMultMatrixd(AR_obj[i].mat_to_world4);
					glRotatef(90.0,1.0,0.0,0.0);
					mqoCallModel(AR_obj[i].model);
				glPopMatrix();
				}
				else {
					glPushMatrix();
					glLoadMatrixd(gl_para_test);
					glMultMatrixd(AR_obj[i].mat_to_hand4);
					glRotatef(90.0,1.0,0.0,0.0);
					mqoCallModel(AR_obj[i].model);
					glPopMatrix();
				}
			}
			
	glPushMatrix();
		action_Play();
	glDisable(GL_LIGHTING);
	glPopMatrix();
	
	//-----------------------------------------------------
	//draw paper
	if (isPaper_exist()){
		glPushMatrix();
			draw_Paper();
		glPopMatrix();
	}
	//-----------------------------------------------------
	//movie
	if (allstate==3){
		glPushMatrix();
		if (is_patt_console_exist){
			glLoadMatrixd(gl_para_test);
			glMultMatrixd(AR_obj[3].mat_to_hand4);
		}
		else {
			glMultMatrixd(AR_obj[3].mat_to_world4);
		}
		avi_play();
		glPopMatrix();
	}
	//-----------------------------------------------------
	//talk
	if (talk_flag){
		glPushMatrix();
			draw_talking();
		glPopMatrix();
	}
	//-----------------------------------------------------
	//floor
	glPushMatrix();
		mode_Drawfloor();
	glPopMatrix();
	//-----------------------------------------------------
	//phone_panel
	glPushMatrix();
		glTranslatef(0.0,0.0,50.0);
		glRotatef(-20.0,1.0,0.0,0.0);
		draw_Phone();
	glPopMatrix();

	//-----------------------------------------------------
	//button
	tmp_pos[2]+=30;
	check_ButtonPool(tmp_pos);
	draw_ButtonPool();
	
	draw_Message();

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
}

// --end of IFFICE.cpp

/*----------------------------------------------------------------------
 * file config.h
 *----------------------------------------------------------------------*/
 
#ifndef _CONFING_H
#define _CONFING_H

#define ACTIVE    1
#define INACTIVE  0
#define VISIBLE   1
#define INVISIBLE 0

#ifndef TRUE
#define TRUE	  1
#define FALSE	  0
#endif

#define GLUT_WHEEL_UP   3
#define GLUT_WHEEL_DOWN 4

#endif

// --end of config.h

/*----------------------------------------------------------------------
 * file header.h
 * 头文件列表
 *----------------------------------------------------------------------*/

#ifndef _O_STRUCT_H
#define _O_STRUCT_H

#include "config.h"
#include "console.h"
extern "C"{
#include "bmploader.h"
}
#include "mqoloader.h"
#include "world.h"
#include "mode.h"
#include "object.h"
#include "action.h"
#include "mymove.h"
#include "button.h"
#include "panel.h"
#include "message.h"
#include "font.h"
#include "myvim.h"
#include "ait.h"
#include "videoavi.h"
#include "myshell.h"
#include "talk_thread.h"
#include "bookshelf.h"
#include "cdbox.h"
#include "star.h"
#include "addresslist.h"
#include "phone.h"

#endif

// --end of header.h

/*----------------------------------------------------------------------
 * file bmploader.h
 * 加载bmp位图(.bmp)
 *----------------------------------------------------------------------*/
 
#ifndef _BMPLODER_H
#define _BMPLODER_H

typedef struct {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
}IMAGE;

//加载一张bmp，句柄放在image里，返回1成功，0失败
int imageLoad(char *filename,IMAGE *image);
//制作纹理贴图
void loadGLTexture(int *texture,char *filename);

#endif

// --end of bmploader.h

/*----------------------------------------------------------------------
 * file mqoloader.h
 * 加载水杉模型(.mqo) 
 *----------------------------------------------------------------------*/

/*----------------------------------------------------------------------
	1.关于纹理
		可加载bmp,jpeg,png格式纹理
		加载jpeg格式需要库：libjpeg.lib jpeglib.h
			宏定义 DEF_USE_LIBJPEG
		加载png格式需要库 ：libpng.lib zlib.lib png.h zlib.h
			宏定义 DEF_USE_LIBPNG
	2. mqo读取相关函数
		(1) 初始化，argInit()使用后
			mqoInit();
		(2) 加载模型
			MQO_MODEL model;
			model = mqoCreateModel( "mario.mqo", 1.0 );
		(3) 调用模型
			mqoCallModel( model );
		(4) 清除模型
			mqoDeleteModel( model );
		(5) 终止(退出前调用)
			mqoCleanup();
	3. mqo动画相关函数
		(1)初始化，同上
			mqoInit();
		(2)加载模型 例如mario0.mqo~mario9.mqo
			MQO_SEQUENCE seq;
			seq = mqoCreateSequence( "mario%d.mqo", 10, 1.0 );
		(3)调用指定的帧序号序列
			mqoCallSequence( seq, i );
		(4)清除释放
			mqoDeleteSequence( seq );
		(5) 同上
			mqoCleanup();
	4. 显示功能
		-支持水杉ver1.0/2.0~2.4
	   信息支持 
	    -材料 颜色（光） 纹理映射（非凹凸贴图/UV贴图）
	   支持对象
		-显示/隐藏切换
		-顶点信息
		-表面信息（非顶点颜色）			
------------------------------------------------------------------------*/



/*----------------------------------------------------------------------
 * 用户设定
 *----------------------------------------------------------------------*/

#ifndef _MQOLOADER_H
#define _MQOLOADER_H

#define MAX_TEXTURE				100			// 最高纹理数
#define MAX_OBJECT				50			// 最高对象数目
#define SIZE_STR				256			// 字符串大小缓冲区
#define DEF_IS_LITTLE_ENDIAN	1			// 指定字节序（基于intel=1）
#define DEF_USE_LIBJPEG			0			// libjpeg 0/1 不使用/使用 
#define DEF_USE_LIBPNG			0			// libpng  0/1 不使用/使用



/*----------------------------------------------------------------------
 * 编译选项
 *----------------------------------------------------------------------*/

// JPEG
#ifdef D_JPEG
	#undef	DEF_USE_LIBJPEG
	#define	DEF_USE_LIBJPEG 1
#endif

// JPEG
#ifdef D_NO_JPEG
	#undef	DEF_USE_LIBJPEG
	#define	DEF_USE_LIBJPEG 0
#endif

// PNG
#ifdef D_PNG
	#undef	DEF_USE_LIBPNG
	#define	DEF_USE_LIBPNG 1
#endif

// PNG
#ifdef D_NO_PNG
	#undef	DEF_USE_LIBPNG
	#define	DEF_USE_LIBPNG 0
#endif

#ifdef WIN32
	#include <windows.h>
#else
	#ifndef MAX_PATH
		#define MAX_PATH    256
	#endif
	#ifndef TRUE
		#define TRUE    (1==1)
	#endif
	#ifndef FALSE
	    #define FALSE   (1!=1)
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/glut.h>
	#include <OpenGL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
//	#include <gl/glext.h>
#endif


/*----------------------------------------------------------------------
 * 使用jpeg 
 *----------------------------------------------------------------------*/

#if DEF_USE_LIBJPEG

	#define XMD_H //int16 和 int32 避免错误
	#ifdef FAR
		#undef FAR
	#endif

	#include "jpeglib.h"
	#pragma comment(lib,"libjpeg.lib")

#endif


/*----------------------------------------------------------------------
 * 使用png
 * ---------------------------------------------------------------------*/

#if DEF_USE_LIBPNG

	#include "png.h"
	#include "zlib.h"
	#pragma comment(lib,"libpng.lib")
	#pragma comment(lib,"zlib.lib")

#endif

#ifndef MAX
	#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

//TGA格式定义
#define DEF_TGA_COLOR_MAP_FLAG_VALID	1
#define DEF_TGA_TYPE_NON				0
#define DEF_TGA_TYPE_INDEX				1
#define DEF_TGA_TYPE_FULL				2
#define DEF_TGA_TYPE_MONO				3
#define DEF_TGA_TYPE_RLEINDEX			9
#define DEF_TGA_TYPE_RLEFULL			10
#define DEF_TGA_TYPE_RLEMONO			11
#define DEF_TGA_BIT_INFO_RIGHT_TO_LEFT	0x00
#define DEF_TGA_BIT_INFO_LEFT_TO_RIGHT	0x10
#define DEF_TGA_BIT_INFO_DOWN_TO_TOP	0x00
#define DEF_TGA_BIT_INFO_TOP_TO_DOWN	0x20

typedef struct {
	unsigned char	id;
	unsigned char	color_map_flag;
	unsigned char	type;
	unsigned short	color_map_entry;
	unsigned char	color_map_entry_size;
	unsigned short	x;
	unsigned short	y;
	unsigned short	width;
	unsigned short	height;
	unsigned char	depth;
	unsigned char	bit_info;
} STR_TGA_HEAD;

typedef struct {
	GLfloat r;
	GLfloat g;
	GLfloat b;
	GLfloat a;
} glCOLOR4f;

typedef struct {
	GLfloat x;
	GLfloat y;
} glPOINT2f;

typedef struct tag_glPOINT3f {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} glPOINT3f;


/*------------------------------------------------------------------------
 * 结构面
 *------------------------------------------------------------------------*/
typedef struct {
	int			n;		// 
	int			m;		// 材料表面数目
	int			v[4];	// 顶点数组
	glPOINT2f	uv[4];	// UV坐标
} MQO_FACE;


/*------------------------------------------------------------------------
 * 材料的信息结构
 *------------------------------------------------------------------------*/
typedef struct {
	glCOLOR4f	col;				// 色彩
	GLfloat		dif[4];				// 漫反射
	GLfloat		amb[4];				// 环境光
	GLfloat		emi[4];				// 自然超明
	GLfloat		spc[4];				// 反射
	GLfloat		power;				// 反射光强度
	int			useTex;				// 使用纹理
	char		texFile[SIZE_STR];	// 纹理文件
	char		alpFile[SIZE_STR];	// alpha通道值
	GLuint		texName;			// 纹理名称
} MQO_MATDATA;


/*------------------------------------------------------------------------
 * 对象结构定义
 *------------------------------------------------------------------------*/
typedef struct {
	char		objname[SIZE_STR];	// 对象名称
	int			visible;			// 可视
	int			shading;			// 底纹 0:遮光1:发光
	float		facet;				// 平滑角
	int			n_face;				// 面数
	int			n_vertex;			// 顶点数
	MQO_FACE	*F;					// 面
	glPOINT3f	*V;					// 顶点
} MQO_OBJDATA;


/*------------------------------------------------------------------------
 * 纹理库
 *------------------------------------------------------------------------*/
typedef struct {
	GLuint			texture_id;			// 纹理编号
	int				texsize;			// 纹理尺寸
	char			texfile[MAX_PATH];	// 纹理文件地址
	char			alpfile[MAX_PATH];	// 
	unsigned char	alpha;				// 
} TEXTURE_POOL;


/*------------------------------------------------------------------------
 * 顶点结构
 *------------------------------------------------------------------------*/
typedef struct {		
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
	GLfloat uv[2];		// UV (u, v)
} VERTEX_TEXUSE;


/*------------------------------------------------------------------------
 * 顶点结构（不使用纹理）
 *------------------------------------------------------------------------*/
typedef struct {
	GLfloat point[3];	//  (x, y, z)
	GLfloat normal[3];	//  (x, y, z)
} VERTEX_NOTEX;


/*------------------------------------------------------------------------
 * 材料
 *------------------------------------------------------------------------*/
typedef struct {
	int				isValidMaterialInfo;// 材料信息 启用/禁用
	int				isUseTexture;		// 有无纹理 USE_TEXTURE / NOUSE_TEXTURE
	GLuint			texture_id;			// 纹理id (OpenGL)
	GLuint			VBO_id;				// 顶点缓冲区编号
	int				datanum;			// 
	GLfloat			color[4];			// 颜色数组 (r, g, b, a)
	GLfloat			dif[4];				// 漫反射
	GLfloat			amb[4];				// 环境光
	GLfloat			emi[4];				// 自动光照
	GLfloat			spc[4];				// 反射
	GLfloat			power;				// 反射光强度
	VERTEX_NOTEX	*vertex_p;			// 顶点（无贴图）
	VERTEX_TEXUSE	*vertex_t;			// 顶点（有贴图）
} MQO_MATERIAL;


/*------------------------------------------------------------------------
 * 内部对象
 *------------------------------------------------------------------------*/
typedef struct {
	char			objname[SIZE_STR];		// 对象名称
	int				isVisible;				// 是否可视
	int				isShadingFlat;			// 光滑阴影
	int				matnum;					// 材质数目
	MQO_MATERIAL	*mat;					// 材质阵列
} MQO_INNER_OBJECT;


/*------------------------------------------------------------------------
 * model对象实体
 *------------------------------------------------------------------------*/
typedef struct {
	unsigned char		alpha;				// 
	int					objnum;				// 内部对象数目
	MQO_INNER_OBJECT	obj[MAX_OBJECT];	// 内部对象阵列
} MQO_OBJECT;

typedef MQO_OBJECT * MQO_MODEL;		// MQO_MODEL


/*------------------------------------------------------------------------
 * MQO序列 
 *------------------------------------------------------------------------*/
typedef struct {
	MQO_MODEL	model;		// 模型
	int			n_frame;	// 帧的数目
} MQO_SEQUENCE;


/*------------------------------------------------------------------------
 * VBO 扩展设计
 *------------------------------------------------------------------------*/
#ifdef WIN32
	#define GL_ARRAY_BUFFER_ARB	0x8892
	#define GL_STATIC_DRAW_ARB	0x88E4
	typedef void (APIENTRY * PFNGLBINDBUFFERARBPROC)    (GLenum target, GLuint buffer);
	typedef void (APIENTRY * PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
	typedef void (APIENTRY * PFNGLGENBUFFERSARBPROC)    (GLsizei n, GLuint *buffers);
	typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC)    (GLenum target, int size, const GLvoid *data, GLenum usage);
#endif


/*------------------------------------------------------------------------
 * 定义 Vertex_Buffer_Object 宏
 *------------------------------------------------------------------------*/

#ifdef _MQOLOADER_C
	#define _MQOLOADER_C_EXTERN
#else
	#define _MQOLOADER_C_EXTERN extern
#endif

_MQOLOADER_C_EXTERN int g_isVBOSupported;	// 支持openGL的顶点缓冲区

#ifdef WIN32	
	// VBO Extension 
	__GLMETASEQ_C__EXTERN PFNGLGENBUFFERSARBPROC glGenBuffersARB;		// VBO 设名代
	__GLMETASEQ_C__EXTERN PFNGLBINDBUFFERARBPROC glBindBufferARB;		// VBO 设绑定
	__GLMETASEQ_C__EXTERN PFNGLBUFFERDATAARBPROC glBufferDataARB;		// VBO 数据加载
	__GLMETASEQ_C__EXTERN PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;	// VBO 删除
#endif

#undef __GLMETASEQ_C__EXTERN


/*------------------------------------------------------------------------
 * 函数声明
 *------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif


// 初始化
void mqoInit(void);

// 清空
void mqoCleanup(void);

// 创建模型 （模型路径，比例）
MQO_MODEL	 mqoCreateModel(char *filename, double scale); 

// 创建动画 （格式，）
MQO_SEQUENCE mqoCreateSequence(const char *format, int n_file, double scale);

// 创建动画-扩展版本
MQO_SEQUENCE mqoCreateSequenceEx(const char *format, int n_file, double scale,
								 int fade_inout, unsigned char alpha);

// 调用模型
void mqoCallModel(MQO_MODEL model);

// 调用动画
void mqoCallSequence(MQO_SEQUENCE seq, int i);

// 删除模型
void mqoDeleteModel(MQO_MODEL model);

// 删除动画
void mqoDeleteSequence(MQO_SEQUENCE seq);


#ifdef __cplusplus
}
#endif

#endif	

// -- end of mqoloader.h

/*----------------------------------------------------------------------
 * file world.h
 * 建立世界坐标系 
 *----------------------------------------------------------------------*/
 
#ifndef _WORLD_H
#define _WORLD_H

#include <AR/ar.h>

typedef struct {
	int		patt_id;
	double	width;
	double	center[2];
	double	extra_trans[3][4];
	double	trans[3][4];
	int		visible;
} BASE_POINT;

typedef struct {
	BASE_POINT *base_point;
	int			base_num;
	double		trans[3][4];
} WORLD;

WORLD *build_the_world(const char *filename);

int    get_the_world(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int    get_the_world2(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int    get_the_world3(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int    get_the_world4(ARMarkerInfo *marker_info,int marker_num,WORLD *world);
int    get_the_world5(WORLD *world);
void   destroy_the_world(WORLD *world);

#endif 

// --end of world.h

/*----------------------------------------------------------------------
 * file mode.h
 * 模式切换 工作/娱乐
 *----------------------------------------------------------------------*/
 
#ifndef _MODE_H
#define _MODE_H

void mode_Init(void); //init 调用一次 载入floor
void mode_Translation(int mode); //mode 0:工作空间 1：娱乐空间 转换时将lock所有操作
void mode_Drawfloor(void);

#endif

// --end of mode.h

/*----------------------------------------------------------------------
 * file object.h
 * 3D模型相关
 *----------------------------------------------------------------------*/
 
#ifndef _OBJECT_H
#define _OBJECT_H

#include "mqoloader.h"

typedef struct AR_OBJ {
	char	name[256];
	int		id;
	double	mat_to_cam4[16];
	double	mat_to_cam3[3][4];
	double	mat_to_world4[16];
	double  mat_to_world3[3][4];
	double  mat_to_hand4[16];
	double  mat_to_hand3[3][4];
	
	double  mat_to_hide[3][4];

	double  mat_from[3][4];					//变换
	double  mat_to[3][4];
	int		step;							//当前动作完成到第一步
	
	double  end_mat_to_world3[3][4];
	double	FlyDeltaMat[3][4];				//每次移动的矩阵值

	int		Locked;							//飞行路径是否已被锁定

	int		StartStep;						//起始
	int		EndStep;						//终止
	int		CurrentStep;					//当前
	int		FlyStep;						//每次移动的步数
	double	scale_FlyWay;


	double	dis[3];
	int		is_can_move;					//是否可以被手板吸附
	int		pos;							//位置 0 : 世界 1：手板
	int		state;							//0:静止状态，可以接受命令 1：动画状态，不能接受指令；
	int		style;							//当state=1时，确定调动的特效函数
	int		visible;						//是否绘制
	int		active;							//0:功能未被激活  1：功能被激活，进入功能状态，此时底下显示激活贴图
	double	scale;							//mqo载入比例
	char	mqo_file[256];
	MQO_MODEL model;						//mqo句柄	
};

//载入3D模型配置文件
AR_OBJ *reg_object(AR_OBJ *AR_obj,char *filename,int &num);

#endif 

// --end of object.h

/*----------------------------------------------------------------------
 * file action.h
 * 模型动画控制
 *----------------------------------------------------------------------*/
 
#ifndef _ACTION_H
#define _ACTION_H

#define MAX_ACTION_NUM 5   					 	//每个动作物体最大动作数量	

#include "mqoloader.h"

typedef void (*ACTION_CALLBACK)(void); 		//动作完成时的回调函数

typedef struct {
	int		action_id;   						//动作编号 -1 时不动 
	int		obj_id;      						//对应object编号,调用obj的坐标位置
	int		now_frame;   						//当前帧
	int		unit;		 						//
	int		act_num;	 						//动作数量
	int		hold;								//停在某帧
	MQO_SEQUENCE *act_sequence; 				//容器
	int		act_frame[MAX_ACTION_NUM];			//动作i的帧数 
	double  act_scale[MAX_ACTION_NUM];			//动作i的scale
	char    act_file[MAX_ACTION_NUM][256]; 		//动作文件路径
	ACTION_CALLBACK callback;   				//动作完成的回调函数，初始化为NULL
}ACTION_OBJ;

typedef struct {
	ACTION_OBJ *action_obj; 					//动画物体
	int			act_obj_num;					//动画物体数量
}ACTIONS;										//动作

//载入动画配置
void action_Loading(char *filename); 

//状态检测机制
void action_Play(void);    

//启动动作
void action_Setplay(int action_obj_id,int action_id,int hold,int start_frame,int unit);

//注册动画物体action_obj_id的回调函数
void action_Setcallback(int action_obj_id,ACTION_CALLBACK action_callback);

//桌面助理AI
void ninja_AI(int count);

//释放内存
void action_Free(void);		

#endif

// --end of action.h
 
/*----------------------------------------------------------------------
 * file mymove.h
 * 坐标移动控制
 *----------------------------------------------------------------------*/
 
#ifndef _MYMOVE_H
#define _MYMOVE_H

#include "object.h"

typedef void (*function)(void);

void SetFly(AR_OBJ *object,int start,int end,int step,double end_mat[][4]);

void MatSet(double s[3][4], double num0,double num1,double num2,double num3,double num4,double num5,double num6,double num7,double num8,double num9,double num10,double num11);

void ObjectFly(AR_OBJ *object, int objectnum);

void SetFunction(function ff);

void CalculatePath(void);

#endif

// --end of mymove.h
 
/*----------------------------------------------------------------------
 * file button.h
 * 按钮控件
 *----------------------------------------------------------------------*/
 
#ifndef _BUTTON_H
#define _BUTTON_H

#define		BUTTON_POOL_MAXSIZE		30  					//button pool 最大容量

#include <stdlib.h>

class BUTTON {
	char	name[256];										//按钮名
	int		id;												//id
	double	size_X,size_Z;									//按钮size
	char	texture[3][256];								//贴图路径
	int		button_texture[3];								//贴图句柄
	double	pos_x,pos_y,pos_z;								//在世界坐标系中的位置
	int		state;											//状态0:unchecked 1:checked but inactive 2:active
	int		visible;										//状态0:invisible 1:visible
	int		worked;											//状态0:wait for work 1:tired and donot want to work
	void	(*callback)(void);								//事件处理函数callback function

public:
	BUTTON(char _name[]="NONE",int _id=-1,double _size_X=100,double _size_Z=10,char _texture[][256]=NULL,double _x=0,double _y=0,double _z=0,void (*_callback)(void)=NULL);
	~BUTTON();

	void	set_Name(char *_name);
	void	set_Id(int _id);
	void	set_Size(double _x,double _z);
	void	set_Texture(char _texture[][256]);
	void	set_Pos(double _x,double _y,double _z);
	void	set_State(int _state);
	void	set_Visible(int _visible);
	void	set_Callback(void (*_callback)(void));
	
	void	draw_Button(void);
	void	check_State(void);
	double  caldis(double pos[]);

	friend	class BUTTON_POOL;
};

class BUTTON_POOL {
	char			name[256];
	int				id;
	int				active_button_id;
	int				preactive_button_id;
	int				timecount;
	int				deadline;
	double			dis[BUTTON_POOL_MAXSIZE];
	BUTTON			*button[BUTTON_POOL_MAXSIZE];
	int				button_num;
	
public:
	BUTTON_POOL(char _name[]="NONE",int _id=-1,int dl=30);
	~BUTTON_POOL();
	
	BUTTON	*register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void));
	void	draw_ButtonPool(void);
	void	check_ButtonPool(double pos[]);
};

//按钮注册
BUTTON *register_Button(char name[],double sx,double sy,char t[][256],double x,double y,double z,void (*_callback)(void));

//按钮绘制
void	draw_ButtonPool(void);

//按钮状态检测
void	check_ButtonPool(double pos[]);

#endif

// --end of button.h
 
/*----------------------------------------------------------------------
 * file panel.h
 * 列表控件
 *----------------------------------------------------------------------*/
 
#ifndef _PANEL_H
#define _PANEL_H

#include "ait.h"

typedef struct {
	char	panel_name[10];		//控制面板的类型（音乐，视频）
	int		num;				//文件的数量
	int		file_work_id;		//当前工作文件的位置
	int		file_on_id;			//当前移动到的文件位置
	MUSIC	*music;			    //各文件名
	int		visible;			//面板可见或不可见
	int		pre_visible;		//之前的可见性
	double w,h;
	int		animation;			//上下拉
	double	new_b;
	double DIST;
	double interval;			//列表间距
	int		noFind;
}PANEL;							

//初始化列表
PANEL *init_Panel(char *filename,char *type,char path[],double w,double h,double interval);

//绘制列表
void  draw_Panel(PANEL *panel,double pos[]);

//释放空间
PANEL *free_Panel(PANEL *panel);

#endif

// --end of panel.h
 
/*----------------------------------------------------------------------
 * file message.h
 * 信息控件
 *----------------------------------------------------------------------*/
 
#ifndef _MESSAGE_H
#define _MESSAGE_H

#define		MESSAGE_POOL_MAXSIZE	30

//message为bmp贴图
typedef struct {
	int		id;
	int		texture_id;
	int		state;		//0:未激活 1：激活
	double  pos[3];		//先不支持旋转 以后改成[3][4]的
	double  w,h;
	int		current_frame;
	int		twinkle_frame;
	double	alpha;
	double  adjust;
}MESSAGE;

//注册message
int register_Message(double pos[],double w,double h,char *file);  //return message id

//弹出message
void call_Message(int id);

//结束message
void over_Message(int id);

//绘制message
void draw_Message();

#endif 

// --end of message.h
 
/*----------------------------------------------------------------------
 * file font.h
 * 创建字体
 *----------------------------------------------------------------------*/
 
#ifndef _FONT_H
#define _FONT_H

void build_Font(void);			  			//导入字体，在init里调用
void print_Font(char ch,int set); 			//set=0 常规 set=1 斜体
void print_String(char *s);
void kill_Font(void);             			//退出时调用

#endif

// --end of font.h
 
/*----------------------------------------------------------------------
 * file myvim.h
 * 文本编辑功能
 *----------------------------------------------------------------------*/
 
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

// --end of myvim.h
 
/*----------------------------------------------------------------------
 * file ait.h
 * 音乐播放器
 *----------------------------------------------------------------------*/
 
#ifndef _AIT_H
#define _AIT_H

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

typedef struct MUSIC {
	char name[256];
	char path[256];
};

Mix_Chunk *load_sound(char *);

void music_Init(void);							//初始化
void music_Play(MUSIC *music); 					//播放音乐 play music ,with looping
void music_Pause(void); 						//暂停音乐 pause music 
void music_Resume(void); 						//恢复播放 resume paused music
void music_Rewind(void); 						//重新播放 rewind music to beginning
void music_Quit(void);

#endif 

// --end of ait.h
 
/*----------------------------------------------------------------------
 * file videoavi.h
 * 视频播放器
 *----------------------------------------------------------------------*/
 
#ifndef _VIDEOAVI_H
#define _VIDEOAVI_H

extern void open_avi(char *filename);
extern void avi_play(void);
extern void avi_stop(void);

#endif

// --end of videoavi.h
 
/*----------------------------------------------------------------------
 * file myshell.h
 * 调用系统shell
 *----------------------------------------------------------------------*/
 
#ifndef _MYSHELL_H
#define _MYSHELL_H

//调用系统shell 运行cmd
void shell_exec(char *cmd); 

#endif

// --end of myshell.h

/*----------------------------------------------------------------------
 * file console.h
 * 主控面板
 *----------------------------------------------------------------------*/
 
#ifndef _CONSOLE_H
#define _CONSOLE_H

typedef struct {
	char	*name;
	int		id;
	int		visible;
	double	width,height;
	double  pos[2];
	double	color_R,color_G,color_B;
	double	alpha;
	int		state;
}SIGLE_CON;

typedef struct {
	char	*name;
	int		id;
	int		drawlist;
	int		state;
	int		visible;
	int		num_w,num_h;
	SIGLE_CON *con;
	double	alphe;
	int		activeid;
}CONSOLE;

//初始化控制面板
CONSOLE *init_Console(CONSOLE *tmp_CON,int num_w,int num_h,double width,double height);
// 注册一个面板绘制
void	register_Console(CONSOLE *console);
//渲染控制面板,在xz轴平面
void	draw_Console(CONSOLE *console);
//激活某个控件
void	active_Console_ID(CONSOLE *console,int id);

#endif

// --end of console.h

/*----------------------------------------------------------------------
 * file talk_thread.h
 * 基于Socket的网络通信
 *----------------------------------------------------------------------*/
 
#ifndef _TALK_THREAD_H
#define _TALK_THREAD_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

//通信缓冲区结构
struct TALK_BUFFER {
	char buf[10][256];	 						 //实际缓存数组
	char name[10][256];    						 //谁说的话
	pthread_mutex_t lock; 						 //互斥体 lock 用于对缓冲区的互斥操作
	int	readpos,writepos,drawpos;				 //读写指针
};

void init_talk_buffer(TALK_BUFFER *talk_buffer);
void *talk_main(void *);
void draw_talking(void);

#endif

// --end of talk_thread.h
 
/*----------------------------------------------------------------------
 * file bookshelf.h
 * 文档阅览
 *----------------------------------------------------------------------*/
 
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

void	bookInit(BOOK *book,FILE *fp);
SHELF	*init_Shelf(SHELF *shelf,char *name);
void	paperList(BOOK *book);
void	paperActive(PAPER *paper);
void	draw_Shelf(SHELF *shelf);
void	move_Paper(BOOK *book);

#endif

// --end of bookshell.h
 
/*----------------------------------------------------------------------
 * file cdbox.h
 * 音乐相关
 *----------------------------------------------------------------------*/
 
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

// --end of cdbox.h
 
/*----------------------------------------------------------------------
 * file star.h
 * 星云激活特效
 *----------------------------------------------------------------------*/
 
#ifndef _STAR_H
#define _STAR_H

typedef struct {
	double x,y,z;
}STAR;

typedef struct {
	int		num_of_stars;
	STAR	*star;
}NEBULA;	//星云

void init_Nebula(void);
void draw_Nebula(int scale);
void destroy_Nebula(void);

#endif

// --end of star.h
 
/*----------------------------------------------------------------------
 * file addresslist.h
 * 通讯录
 *----------------------------------------------------------------------*/
 
#ifndef _ADDRESSLIST_H
#define _ADDRESSLIST_H

typedef struct {
	char name[256];     
	int  id;			
	char phone[256];	
	char email[256];
	char picture[256];
	int	 head_texture;
}PERSON;

void init_Person(void);

#endif

// --end of addresslist.h
 
/*----------------------------------------------------------------------
 * file phone.h
 * 手机通信
 *----------------------------------------------------------------------*/
 
#ifndef _PHONE_H
#define _PHONE_H


//PO means Phone Order
#define		PO_ECHO_OFF		"ATE0\r"			//echo off
#define		PO_HANG_UP		"AT+CVHU=0\r"		//change ATH to hang up
#define		PO_HOLD_ON		"AT+CVHU=1\r"		//change ATH to hold on(default)
#define		PO_INTERRUPT	"ATH\r"				//hang up or hold on
#define		PO_ANSWER		"ATA\r"				//answer call
#define		PO_CALLER_ID	"AT+CLCC\r"			//echo caller ID

void init_Phone(void);
void select_Number(char num);
void calling(void);
void receive_Call(int state);
void phone_send(char *order);
void draw_Phone(void);
void exit_Phone(void);

#endif

// --end of phone.h

