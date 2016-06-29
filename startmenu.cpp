#include "startmenu.h"
extern "C" {
#include "bmploader.h"
}
#include "head.h"

#include <AR/ar.h>
#include <AR/gsub.h>
#include <AR/matrix.h>
#include <GL/glut.h>

#include <stdio.h>
#include <string.h>
#include <math.h>


const double PI=3.1415926;
STARTMENU startmenu;

extern	int allstate;
extern  SHELF *shelf;
extern  AR_OBJ *AR_obj;
extern  int obj_num;
extern  int kiss;
extern  int now_mode;
extern CDBOX *cdbox;
extern PANEL *music_panel;
extern char *tmp_cmd;
extern char *chavi;
extern bool show_phone_panel;
extern bool Peo_flag;

extern mode_function mode_f;

void mode_callback0(void) {
	cdbox->activecdid=0;
	AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
	argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
	return ;
}

void mode_callback1(void) {
	shelf->activebookid=0;
	AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
	argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
	return ;
}

void mode_callback2(void) {
	open_avi(chavi);
	return ;
}

void mode_callback3(void) {
	show_phone_panel=true;
	return ;
}

void button0_callback(void) {
	int i;
	if (startmenu.rank==0) {
		startmenu.active=1-startmenu.active;
		for (i=1;i<=3;i++)	startmenu.button[i]->set_Visible(startmenu.active);
	}
	if (startmenu.rank==1) {
		if (allstate==1) {
			AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
			shelf->activebookid=-1;
			allstate=0;
		}
		if (allstate==2) {
			allstate=0;
			AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
			cdbox->activecdid=-1;
		}
		if (allstate==3) {
			allstate=0;
			avi_stop();
			for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(1);
		}
		if (allstate==4) {
			show_phone_panel=false;
			allstate=0;
		}
		if (allstate==5) {
			startmenu.button[5]->set_Visible(0);
			for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(1);
			allstate=0;
		}
		if (allstate==6) {
			Peo_flag=false;
			allstate=0;
		}
		if (allstate==7) {
			close_Paper();
			if (shelf->activebookid!=-1 && shelf->book[shelf->activebookid].state==ACTIVE)
			allstate=1;
			else allstate=0;
		}
		if (allstate==0) {
			startmenu.active=1;
			startmenu.rank=0;
		}
	}
	if (startmenu.rank==2) {
		if (allstate==1) {
			shelf->book[shelf->activebookid].state=INACTIVE;
			startmenu.rank=1;
		}
		if (allstate==2) {
			music_Quit();
			cdbox->cd[cdbox->activecdid].state=INACTIVE;
			music_panel=free_Panel(music_panel);
			startmenu.rank=1;
			for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(1);
		}
		if (allstate==7) {
			close_Paper();
		//	if (shelf->activebookid!=-1 && shelf->book[shelf->activebookid].state==ACTIVE)
			allstate=1;
		//	else allstate=0;
			startmenu.button[4]->set_Visible(0);
		}
		
	}


	return ;
}

void button1_callback(void) {
	if (startmenu.rank==0) {
		if (startmenu.state==0){
			startmenu.state=1;
			startmenu.complete=6;
		}
	}
	if (startmenu.rank==1) {
		if (allstate==1) {
			AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
			shelf->activebookid=(shelf->activebookid-1)%shelf->numofbooks;
			if (shelf->activebookid<0) shelf->activebookid+=shelf->numofbooks;
			AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
			argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
		}
		if (allstate==2) {
			AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
			cdbox->activecdid=(cdbox->activecdid-1)%cdbox->numofcds;
			if (cdbox->activecdid<0) cdbox->activecdid+=cdbox->numofcds;
			AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
			argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
		}
	}
	if (startmenu.rank==2) {
		if (allstate==1) {
			kiss=1;
			action_Setplay(0,0,-1,1,1);
			shelf->book[shelf->activebookid].movestate=20;
			shelf->book[shelf->activebookid].firstpaperid=(shelf->book[shelf->activebookid].firstpaperid+1)%shelf->book[shelf->activebookid].numofpapers;
		}
	}

	return ;
}

void button2_callback(void) {
	if (startmenu.rank==0) {
		if (startmenu.state==0) {
			startmenu.state=-1;
			startmenu.complete=-6;
		}
	}
	if (startmenu.rank==1) {
		if (allstate==1) {
			AR_obj[shelf->activebookid+4].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
			shelf->activebookid=(shelf->activebookid+1)%shelf->numofbooks;
			AR_obj[shelf->activebookid+4].mat_to_world3[2][3]+=10;
			argConvGlpara(AR_obj[shelf->activebookid+4].mat_to_world3,AR_obj[shelf->activebookid+4].mat_to_world4);
		}
		if (allstate==2) {
			AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]-=10;
			argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
			cdbox->activecdid=(cdbox->activecdid+1)%cdbox->numofcds;
			AR_obj[cdbox->activecdid+7].mat_to_world3[2][3]+=10;
			argConvGlpara(AR_obj[cdbox->activecdid+7].mat_to_world3,AR_obj[cdbox->activecdid+7].mat_to_world4);
		}
	}
	if (startmenu.rank==2) {
		if (allstate==1) {
			kiss=1;
			action_Setplay(0,0,-1,10,-1);
			shelf->book[shelf->activebookid].movestate=-20;
			if (shelf->book[shelf->activebookid].firstpaperid==0) shelf->book[shelf->activebookid].firstpaperid=shelf->book[shelf->activebookid].numofpapers;
			shelf->book[shelf->activebookid].firstpaperid-=1;
		}
	}
	return ;
}

void button3_callback(void) {
	if (startmenu.rank==0) {
		allstate=startmenu.application+1;
		switch (allstate) {
			case 1:
				if (now_mode==1) {
					mode_Translation(0);
					mode_SetFunction(mode_callback1);
				}
				else mode_callback1();
				break;
			case 2:
				if (now_mode==0) {
					mode_Translation(0);
					mode_SetFunction(mode_callback0);
				}
				else mode_callback0();
				break;
			case 3:
				if (now_mode==0) {
					mode_Translation(0);
					mode_SetFunction(mode_callback2);
				}
				else mode_callback2();
				for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(0);
				break;
			case 4:
				if (now_mode==1) {
					mode_Translation(0);
					mode_SetFunction(mode_callback3);
				}
				else mode_callback3();
				break;
			case 5:
				startmenu.button[5]->set_Visible(1);
				for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(0);
				break;
			case 6:
				Peo_flag=true;
				break;
		}
		startmenu.rank=1;
		startmenu.active=0;
	}
	else 
	if (startmenu.rank==1) {
		if (allstate==1) {
			shelf->book[shelf->activebookid].state=ACTIVE;
			startmenu.rank=2;
		}
		if (allstate==2) {
			cdbox->cd[cdbox->activecdid].state=ACTIVE;
			char tmp_cmd_[256];
			strcpy(tmp_cmd_,tmp_cmd);
			strcat(tmp_cmd_,cdbox->cd[cdbox->activecdid].path);
			shell_exec(tmp_cmd_);
			if (music_panel!=NULL) music_panel=free_Panel(music_panel);
			music_panel=init_Panel("myshell.txt","music",cdbox->cd[cdbox->activecdid].path,100,10,2);
			startmenu.rank=2;
			for (int i=1;i<=3;i++)	startmenu.button[i]->set_Visible(0);
		}
	}
	else 
	if (startmenu.rank==2) {
		if (allstate==1) {
			shelf->book[shelf->activebookid].activepaperid=(shelf->book[shelf->activebookid].firstpaperid+1)%shelf->book[shelf->activebookid].numofpapers; //ææ¶ææ¿æ´»çpaperè®¾ç½®å¦æ­¤ï¼åé¢åæ¹
			read_Paper(&shelf->book[shelf->activebookid].paper[shelf->book[shelf->activebookid].activepaperid]);
			startmenu.button[4]->set_Visible(1);
			allstate=7;
		}
	}
	printf("%d\n",startmenu.application);
	return ;
}

void button4_callback() {
	action_Setplay(1,0,-1,1,1);
	print_Paper(shelf->book[shelf->activebookid].paper[shelf->book[shelf->activebookid].activepaperid].path);
	return ;
}

void button5_callback() {
	init_Paint();
	return ;
}

void init_Startmenu(){
	char	path0[3][256]={"Data/p/home.bmp","Data/p/home.bmp","Data/p/home.bmp"};
	char	path1[3][256]={"Data/p/up.bmp"  ,"Data/p/up.bmp"  ,"Data/p/up.bmp"};
	char	path2[3][256]={"Data/p/down.bmp","Data/p/down.bmp","Data/p/down.bmp"};
	char	path3[3][256]={"Data/p/key.bmp" ,"Data/p/key.bmp" ,"Data/p/key.bmp"};
	char	path4[3][256]={"Data/p/printer.bmp","Data/p/printer.bmp","Data/p/printer.bmp"};
	char	path5[3][256]={"Data/p/trash.bmp","Data/p/trash.bmp","Data/p/trash.bmp"};

	startmenu.menu[1]=mqoCreateModel("Data/model/menu1.mqo",0.2);
	startmenu.menu[2]=mqoCreateModel("Data/model/menu2.mqo",0.2);
	startmenu.menu[3]=mqoCreateModel("Data/model/menu3.mqo",0.2);
	startmenu.menu[4]=mqoCreateModel("Data/model/menu4.mqo",0.2);
	startmenu.menu[5]=mqoCreateModel("Data/model/menu5.mqo",0.2);
	startmenu.menu[6]=mqoCreateModel("Data/model/menu6.mqo",0.2);

	strcpy(startmenu.name,"startmenu");
	startmenu.active=0;
	startmenu.application=0;
	startmenu.state=0;
	startmenu.rank=0;
	startmenu.complete=0;
	startmenu.radius=60.0;

	startmenu.button[0]=register_Button("menu_start",32,32,path0,200,-50,30,button0_callback);
	startmenu.button[1]=register_Button("menu_up",20,20,path1,140,0,130,button1_callback);
	startmenu.button[2]=register_Button("menu_down",20,20,path2,140,0,90,button2_callback);
	startmenu.button[3]=register_Button("menu_ac",20,20,path3,140,0,50,button3_callback);
	startmenu.button[4]=register_Button("menu_printer",32,32,path4,-200,-50,30,button4_callback);
	startmenu.button[5]=register_Button("menu_trash",32,32,path5,150,-50,30,button5_callback);

	startmenu.button[0]->set_Visible(1);
	printf("done startmenu\n");

	return ;
}

void draw_Startmenu(){
	double	x,z;
	int		i;
	double	angle=PI*startmenu.complete/180;
	double	angle_inc=PI/3;
	glColor3f(0.0,0.0,0.0);
	if (startmenu.active){
		//glEnable(GL_LIGHTING);	
		for (i=0;i<6;i++){
			int j=(i+startmenu.application)%6;
			x=cos(angle)*startmenu.radius;
			z=sin(angle)*startmenu.radius;

			glPushMatrix();
			glTranslatef(x,0.0,z);
			glRotatef(90.0,1.0,0.0,0.0);
			mqoCallModel(startmenu.menu[j+1]);
			glPopMatrix();

			angle+=angle_inc;
		}
	//	glDisable(GL_LIGHTING);
		if (startmenu.state!=0){
			if (startmenu.complete%60==0) {
				if (startmenu.state==-1) startmenu.application=(startmenu.application+1)%6;
				else{
					if (startmenu.application==0) startmenu.application=6;
					startmenu.application=(startmenu.application-1)%6;
				}
				startmenu.state=0; 
				startmenu.complete=0;
			}
			else {
				startmenu.complete+=startmenu.state*6;
			}
		}
		else {
			glLineWidth(2.0);
			glColor3f(1.0,0.0,0.0);
			glBegin(GL_LINE_STRIP);
				glVertex3f(44,0.0,24);
				glVertex3f(36,0.0,24);
				glVertex3f(36,0.0,-24);
				glVertex3f(44,0.0,-24);
			glEnd();
			glBegin(GL_LINE_STRIP);
				glVertex3f(76,0.0,24);
				glVertex3f(84,0.0,24);
				glVertex3f(84,0.0,-24);
				glVertex3f(76,0.0,-24);
			glEnd();
			glLineWidth(1.0);
		}
	}
	return ;
}
