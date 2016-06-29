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

#include "head.h"
#include "config.h"

//Camera configuration
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


char		   *cparam_name = "Data/camera_para.dat";
ARParam			cparam;


char			*world_name="Data/multi/marker.dat";

char			*mqo_girl="Data/MM.mqo";
char			*chavi="Data/movie/test.avi";
CONSOLE			*console=NULL;
SHELF			*shelf=NULL;
CDBOX			*cdbox=NULL;
WORLD			*world;
PANEL			*music_panel=NULL;
char			*object_conf="Data/object_conf";
char			*bookshelf_conf="Data/bookshelf_info";
char			*cdbox_conf="Data/cdbox_info";
char			*action_conf="Data/action_conf";
int				kiss=0;
int				allstate=0;
int				is_patt_console_exist=0;
int				delay=60,printer_tmp=0;
char			*tmp_cmd="ls ";

AR_OBJ			*AR_obj;	
int				obj_num;

int				wander_mode=0;

int				picturetest;

extern bool		show_phone_panel; //电话面板是否绘制
extern HAND		hand;
bool			Peo_flag=false;

//test call backfunction------------
int				kawayi=99;
//---------------------------------
float eye_angle=0.0,deltaAngle = 0.0,ratio;
float eye_x=0.0f,eye_y=100.75f,eye_z=65.0f;
float eye_lx=0.0f,eye_ly=0.0f,eye_lz=-1.0f;
//GLint snowman_display_list;
int deltaMove = 0;
//---------------------------------

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
//
void orientMe(float ang) {
	eye_lx+=1;
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
        printf("*** %f (frame/sec)\n", (double)count/arUtilTimer());
        cleanup();
        exit(0);
	}

	if (kiss!=0) return;
	if (allstate==0){
	switch (key)
	{
		/* quit if the ESC key is pressed */
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
			printf("M\n");
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
				//printf("hui che\n");
				allstate=console->activeid+1;
				console->activeid=-1;
				console->state=INACTIVE;
			}
			if (allstate==1){
				shelf->activebookid=0;
				AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
				argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
			}
			if (allstate==7){
				creat_Paper("Data/test");
				kiss=2;
				glPushMatrix();
			}
			if (allstate==3){
				open_avi(chavi);	
			}
			if (allstate==2){
				cdbox->activecdid=0;
				AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
				argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
			}
			if (allstate==4){
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
					shelf->book[shelf->activebookid].activepaperid=(shelf->book[shelf->activebookid].firstpaperid+1)%shelf->book[shelf->activebookid].numofpapers; //暂时把激活的paper设置如此，后面再改
					read_Paper(&shelf->book[shelf->activebookid].paper[shelf->book[shelf->activebookid].activepaperid]);
					allstate=7;
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

	if (allstate==2){
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

	if (allstate==7){
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
	if (allstate==4){
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
					shelf->book[shelf->activebookid].firstpaperid=(shelf->book[shelf->activebookid].firstpaperid+1)%shelf->book[shelf->activebookid].numofpapers;
					break;
				case GLUT_KEY_RIGHT:
					kiss=1;
					action_Setplay(0,0,-1,10,-1);
					shelf->book[shelf->activebookid].movestate=-20;
					if (shelf->book[shelf->activebookid].firstpaperid==0) shelf->book[shelf->activebookid].firstpaperid=shelf->book[shelf->activebookid].numofpapers;
					shelf->book[shelf->activebookid].firstpaperid-=1;
					//shelf->book[shelf->activebookid].movestate=-50;
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
	if (allstate==7){
		if (key==GLUT_KEY_F2) {
			close_Paper();
			if (shelf->activebookid!=-1 && shelf->book[shelf->activebookid].state==ACTIVE)
			allstate=1;
			else allstate=0;
			glPopMatrix();
		}
		if (key==GLUT_KEY_F3) { //打印
			print_Paper(shelf->book[shelf->activebookid].paper[shelf->book[shelf->activebookid].activepaperid].path);
			action_Setplay(1,0,-1,1,1);
		}
	}
	if (allstate==2){
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
//test callback function-----------
void test_f(){
//	printf("done!%d\n",kawayi);
	return;
}
//---------------------------------

/* main loop */
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
    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }
    if( count == 0 ) arUtilTimerReset();
    count++;

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

	/* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }
    arVideoCapNext();

	if (!wander_mode){
	
		if (get_the_world(marker_info,marker_num,world)<0) {argSwapBuffers();return ;}
	}
	else {
		get_the_world5(world);
	}

	check_Hand(marker_info,marker_num,world->trans);
	
	ObjectFly(AR_obj,obj_num);

	ninja_AI(count);	

    draw();

    argSwapBuffers();
}

void init( void )
{
    ARParam  wparam;
	int		 tmp; //无用
	double   tmp_mat[3][4];
	
    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }
    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );


	if ((world=build_the_world(world_name))==NULL ){
		printf("error while loading the world\n");
		exit(0);
	}
	printf("creat the world successful!\n");

    /* open the graphics window */
  //  argInit( &cparam, 1.0, 0, 0, 0, 0 );
	argInit( &cparam, 2.0, 1, 0, 0, 0 );
	//*****************************************************
	//console init
	console=init_Console(console,3,4,50.0,30.0);
	register_Console(console); //
	//*****************************************************
	//mqo init
	mqoInit();
	init_Hand();
	AR_obj=reg_object(AR_obj,object_conf,obj_num);
	printf("%d\n",AR_obj[1].id);
	
	//phone panel init
	init_Phone();

	//shelf init
	shelf=init_Shelf(shelf,bookshelf_conf);
	//cdbox init
	cdbox=init_Cdbox(cdbox,cdbox_conf);
	//*****************************************************
	//font init
	build_Font();
	//*****************************************************
	//music init
	music_Init();
	
	ring=load_sound("Data/music_/msg.wav");

	//addresslist init
	Peo_init_Panel("address",100,20.0,5.0);
	//*****************************************************
	//bate_object
	//object_init_test(&object);
	//picturetest
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	loadGLTexture(&picturetest,"Data/calling.bmp");
	printf("%d\n",picturetest);
	glDisable(GL_TEXTURE_2D);

	strcpy(music_tmp.path,"Data/music/music0/first.mp3");
	//music_Init();
	//music_Play(&music_tmp);
	//*****************************************************
	//计算路径
	CalculatePath();
	//MatSet(tmp_mat,1,0,0,100,0,1,0,100,0,0,1,100);
	//SetFly(&AR_obj[3],1000,1500,10,tmp_mat);
	//mode_Init()
	mode_Init();
	//*****************************************************
	//init_person
	//init_Person();
	
	//*****************************************************
	//init_action
	action_Loading(action_conf);

	//*****************************************************
	init_talk_buffer(&talk_buffer);
	if ((tmp=pthread_create(&talk_id,NULL,talk_main,NULL))!=0){
		printf("create pthread error!\n");
		exit(1);
	}
	printf("create a pthread\n");

	init_Startmenu();

	return ;
}

void initGL()
{
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
//	glEnable(GL_BLEND);
	glutSpecialFunc(specialKeyEvent);
//	glutIgnoreKeyRepeat(1);
//	glutSpecialUpFunc(releaseKey);
//	glutReshapeFunc(reShape);
	glDisable(GL_TEXTURE_2D);
	return;
}

/* cleanup function called when program exits */
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
//	Mix_HaltChannel(-1);
//	Mix_CloseAudio();
//	SDL_Quit();
	//exit(0);
}

//没用，又被改回去了。。。
void reShape(int w,int h) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (h==0) h=1;
	//argInit2(1);
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


void drawQUAD(float x0,float z0,float x1,float z1,float y0)
{
	//glDisable(GL_DEPTH_TEST);
	glColor3f(0.2,0.2,0.2);
	glBegin(GL_QUADS);
		glVertex3f(x0,y0,z0);
		glVertex3f(x1,y0,z0);
		glVertex3f(x1,y0,z1);
		glVertex3f(x0,y0,z1);	
	glEnd();
	//glEnable(GL_DEPTH_TEST);
}
void draw( void )
{
    double    gl_para[16];
	double	  gl_para_test[16];


	int i,j;
    char ch;	
	double tmp_pos[3];
	tmp_pos[0]=hand.pos[0];
	tmp_pos[1]=hand.pos[1];


    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	glMatrixMode(GL_MODELVIEW);
//-------------------------------------------------------------
    /* load the camera transformation matrix */
//    argConvGlpara(patt_trans, gl_para);
	argConvGlpara(world->trans,gl_para);
	glLoadMatrixd( gl_para );
if (wander_mode){
//	gluLookAt(eye_x, eye_y, eye_z, 0.0,0.0,0.0, 0.0f,1.0f,0.0f);

	if (deltaMove)
		moveMeFlat(deltaMove);
	if (deltaAngle) {
		eye_angle += deltaAngle;
		orientMe(eye_angle);
	}
}
//--------------------------------------------------------------
//----------------------------------------
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

//----------------------------------------
	//*****************************************************
	//draw the console
	if (console->state){
		//SetLight();
		glPushMatrix();
		glTranslatef( -75.0, -50.0, 0.0 );
		draw_Console(console);
		glPopMatrix();
	}
	//*****************************************************
	//draw the shelf
	if (shelf->visible){
		glPushMatrix();
		//glTranslatef(80.0,100.0,0.0);
		draw_Shelf(shelf);
		glPopMatrix();
	}

	if (music_panel!=NULL){
		glTranslatef(0.0,0.0,50);
		tmp_pos[2]=hand.pos[2]-50;
		draw_Panel(music_panel,tmp_pos);
		glTranslatef(0.0,0.0,-50.0);
	}
	//*****************************************************
	//draw mqo model
	SetLight();
	//draw object
	glEnable(GL_LIGHTING);
	if (AR_obj[3].pos==0)
	draw_Hand();
	if (allstate!=5)
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
						glLoadMatrixd(hand.hand_to_cam4);
						//glMultMatrixd(AR_obj[i].mat_to_hand4);
						glRotatef(90.0,1.0,0.0,0.0);
						mqoCallModel(AR_obj[i].model);
					glPopMatrix();
				}
			}
//	glDisable(GL_LIGHTING);
	//****************************************************
	//action
	glPushMatrix();
//	glEnable(GL_LIGHTING); //需要加光照的要放在一块渲染！！
		action_Play();
	glDisable(GL_LIGHTING);
	glPopMatrix();

//----------------------
	if (allstate==5)
	draw_Paint();
//----------------------

	
	//*****************************************************
	//draw paper
	if (isPaper_exist()){
		glPushMatrix();
			draw_Paper();
		glPopMatrix();
	}
	//*****************************************************
	//movie
	if (watching()){
		glPushMatrix();
	//	if (hand.exist){
	//		glLoadMatrixd(hand.hand_to_cam4);
	//		glMultMatrixd(AR_obj[3].mat_to_hand4);
	//	}
	//	else {
			//glMultMatrixd(AR_obj[3].mat_to_world4);
	//	}
		if (AR_obj[3].pos==0)
			glMultMatrixd(AR_obj[3].mat_to_world4);
		else glLoadMatrixd(hand.hand_to_cam4);
		avi_play();
		glPopMatrix();
	}
	//*****************************************************
	//talk
	if (talk_flag){
		glPushMatrix();
			draw_talking();
		glPopMatrix();
	}
	//****************************************************
	//floor
	glPushMatrix();
		mode_Drawfloor();
	glPopMatrix();
	//****************************************************
	//phone_panel
	glPushMatrix();
		glTranslatef(0.0,0.0,50.0);
		glRotatef(-20.0,1.0,0.0,0.0);
		draw_Phone();
	glPopMatrix();

	//*****************************************************
	//button test
//	tmp_pos[2]+=30;

	check_ButtonPool(hand.pos);
	draw_ButtonPool();	
	
	glTranslatef(0.0,0.0,100.0);
		draw_Startmenu();
	glTranslatef(0.0,0.0,-100.0);

	draw_Message();


	if (Peo_flag) {
		tmp_pos[2]=hand.pos[2]/0.6;
		tmp_pos[2]-=150;	
		glPushMatrix();
		//glRotatef(-30,0.0,0.0,1.0);
		glScalef(0.6,1.0,0.6);
		glTranslatef(0,0.0,150.0);
		draw_Addresslist(tmp_pos);
		glPopMatrix();
	}
		//glPushMatrix();
		//glPointSize(10.0);
	if (AR_obj[3].pos==0) {
		glColor3f(1.0,0.0,0.0);
		//glRotatef(90.0,1.0,0.0,0.0);
		glBegin(GL_LINES);
			glVertex3f(hand.pos[0],hand.pos[1],0.0);
			glVertex3f(hand.pos[0],hand.pos[1],hand.pos[2]);

			glVertex3f(hand.pos[0]+10,hand.pos[1]+10,0.0);
			glVertex3f(hand.pos[0]-10,hand.pos[1]-10,0.0);

			glVertex3f(hand.pos[0]+10,hand.pos[1]-10,0.0);
			glVertex3f(hand.pos[0]-10,hand.pos[1]+10,0.0);
		glEnd();
	}
	//glPointSize(1.0);
	//glPopMatrix();

    glDisable( GL_LIGHTING );
    glDisable( GL_DEPTH_TEST );
}

