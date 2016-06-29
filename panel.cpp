#include "panel.h"
#include "myvim.h"
extern "C"{
#include "bmploader.h"
}
#include "ait.h"
#include <AR/ar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <math.h>

const int	 ENDURE=20;

const int     GATE=8;
int			  is_num=0;
double		  rec_prepara;
int			  panel_texture[2];
bool          panel_tmp_bool=true;
//double		  interval=6.0;  //列表间距

extern void getbuff(char *buf,int n,FILE *fp);

PANEL *init_Panel(char *filename,char *type,char path[],double w,double h,double interval){
	FILE *fp;
	int i,j;
	int buf[256];
	char file_tmp[256];
	PANEL *panel;
	arMalloc(panel,PANEL,1);
	strcpy(panel->panel_name,type);
	panel->num=0;
	panel->visible=0;
	panel->file_on_id=-1;
	panel->file_work_id=-1;
	panel->pre_visible=0;
	panel->animation=0;
	panel->new_b=0;
	panel->w=w;
	panel->h=h;
	panel->interval=interval;
	panel->noFind=0;

	if ((fp=fopen(filename,"r"))==NULL) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}
	while ((fscanf(fp,"%s",buf))!=EOF) {panel->num++;}
	if ((fseek(fp,0,SEEK_SET))!=0) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}
	arMalloc(panel->music,MUSIC,panel->num);
	for (i=0;i<panel->num;i++){
		fscanf(fp,"%s",panel->music[i].name);
		strcpy(panel->music[i].path,path);
		strcat(panel->music[i].path,panel->music[i].name);
	}

	panel->DIST=(panel->num-1)*(panel->h+panel->interval);
	printf("donepanel\n");
	fclose(fp);
if (panel_tmp_bool){
	/* 面板的贴图预处理 */
	strcpy(file_tmp,"Data/bumps.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&panel_texture[0], file_tmp);

	/* 手板的贴图预处理 */
//	strcpy(filename,"Data/bumps.bmp");
//	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&panel_texture[1], file_tmp);

	glDisable(GL_TEXTURE_2D);
	panel_tmp_bool=false;
}
	return panel;
}

void change_new_b(PANEL *panel){
	if ((panel->visible==1) && (panel->pre_visible==0)){
		if (fabs(panel->DIST-panel->new_b)<=10) panel->new_b=panel->DIST;
		else panel->new_b+=10;
	}
	if ((panel->visible==0) && (panel->pre_visible==1)){
		if (fabs(panel->new_b)<=10) panel->new_b=0;
		else panel->new_b-=10;
	}
	return ;
}

void draw_Panel(PANEL *panel,double pos[]){
	/*w长，h宽*/
	double b;
	int i;
	int now_num;
	double spe_dis;
	double ratio;
	int point;
	double quan=(panel->num-1)*(panel->h+panel->interval);
	double add_b=panel->num*(panel->h+panel->interval)-panel->interval;

	//程序遵循的原则是执行特效时，不执行对选择，确定等动作的判断和列表条的画，这种状态一直持续到特效完成
	
	//========================只要执行drawobject就先画第一个列表条===================
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, add_b  );
	glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, add_b  );
	glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, add_b-panel->h);
	glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, add_b-panel->h);
	glEnd();
	glDisable(GL_TEXTURE_2D);
	//===============================================================================

	if(panel->animation==0)
	{
		if((pos[0]>0)&&(pos[0]<200)&&(pos[1]<20)&&(pos[1]>-20)&&(pos[2]>0)&&(pos[2]<((panel->h+panel->interval)*(panel->num+3))))
		{
			panel->visible=1;
			point=(int)(pos[2]/(panel->h+panel->interval));
	   		if(point>=(panel->num-1))
				point=panel->num-1;
			if(point<=0)
				point=0;
			if(point<=(panel->num-1))
			{
				if(point!=panel->file_on_id)
				{
						is_num=0;
				}
				panel->file_on_id=point;
				if(is_num!=0)
				{
			 		if((pos[0]-rec_prepara)<-1)
					{
						is_num+=1;
						rec_prepara=pos[0];
						if (is_num>=GATE)
							if(panel->file_work_id!=point)
							{
								panel->file_work_id=point;
								music_Play(&panel->music[point]);
								is_num=0;
							}
							else
							{
								panel->file_work_id=-1;
								music_Pause();
								is_num=0;
							}
					}
					else if((pos[0]-rec_prepara)>0)
					{
						rec_prepara=pos[0];
						is_num=0;
					}
					else
					{
						rec_prepara=pos[0];
					}
				}
				else
				{
					rec_prepara=pos[0];
					is_num+=1;
				}
			
			}
		}
	
		else       
		{
			if (panel->noFind>=ENDURE){
				panel->visible=0;
				is_num=0;
				panel->noFind=0;
			}
			else {
				is_num=0;
				panel->noFind++;
			}
		}
	}

	if(panel->visible==1)
	{
	  	if(panel->pre_visible==0)
		{
			//下拉特效程序开始，首先画第一个列表条
			//now_num作为下面要画几个完整的列表（包括了各表的间隔和表条）,b作为所有画的列表条的左下坐标
			//ratio是部分条的比例大小
			
			//画第一个列表条
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, add_b  );
			glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, add_b  );
			glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, add_b-panel->h);
			glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, add_b-panel->h);
			glEnd();
			glDisable(GL_TEXTURE_2D);		

			change_new_b(panel);
			panel->visible=1;
			panel->animation=1;
			now_num=(int)(panel->new_b/(panel->h+panel->interval));//此处的b待会儿改掉
			spe_dis=panel->new_b-now_num*(panel->h+panel->interval);
			b=quan-(panel->h+panel->interval);
				
			if(now_num!=0)
			{
  				for(i=1;i<=now_num;i++)
				{

					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, b );
					glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, b  );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					b-=panel->h+panel->interval;
				}
			}

			if(now_num!=0)
				b+=panel->h+panel->interval;
			else
				b=quan;

			if(spe_dis<=panel->h)
			{		
				ratio=spe_dis/panel->h;
				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, ratio); glVertex3f(-panel->w/2, 0, b		   );
				glTexCoord2f(1.0, ratio); glVertex3f( panel->w/2, 0, b		   );
				glTexCoord2f(1.0, 0.0  ); glVertex3f( panel->w/2, 0, b-spe_dis);
				glTexCoord2f(0.0, 0.0);   glVertex3f(-panel->w/2, 0, b-spe_dis);
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
			else
			{
				b-=spe_dis;
				glEnable(GL_TEXTURE_2D);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
				glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
				glBegin(GL_QUADS);
				glTexCoord2f(0.0, 0.0);	  glVertex3f(-panel->w/2, 0, b		  );
				glTexCoord2f(0.0, 1.0);   glVertex3f(-panel->w/2, 0, b+panel->h );
				glTexCoord2f(1.0, 1.0);   glVertex3f( panel->w/2, 0, b+panel->h );
				glTexCoord2f(1.0, 0.0);   glVertex3f( panel->w/2, 0, b         );
				glEnd();
				glDisable(GL_TEXTURE_2D);
			}
		}
		
		//下拉特效结束，复原各参数
		if(panel->new_b>=panel->DIST)//下拉特效结束标示
		{
			panel->pre_visible =1;
			panel->animation =0;
		}
		
		//之下程序是在没有执行任何特效时，即在执行选择，确定功能时，画出各个列表条，一次画完
		if(panel->animation==0)
		{
			b=-(panel->h+panel->interval);
			for(i=0;i<panel->num;i++)
			{
				b+=panel->h+panel->interval;
				if((panel->file_on_id==i) || (panel->file_work_id==i))
				{
					if(panel->file_work_id==i)
					{
						glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
						glEnable(GL_BLEND);
						glColor3f(0.0, 1.0, 0.0);
						glEnable(GL_TEXTURE_2D);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
						glBegin(GL_QUADS);
						glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2-2, 0, b    );
						glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2-2, 0, b+panel->h+2);
						glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2+2, 0, b+panel->h+2);
						glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2+2, 0, b    );
						glEnd();
						glPushMatrix();
							glTranslatef(-panel->w/2-2, 0, b);
							draw_String(panel->music[i].name,panel->w);
						glPopMatrix();

						glDisable(GL_TEXTURE_2D);
						glDisable(GL_BLEND);

						//drawfilename(-w/2-2, 0, b+h+2);
					}
					else
					{
						glEnable(GL_TEXTURE_2D);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
						//glColor3f(0.0, 1.0, 0.0);//位置绿
						glBegin(GL_QUADS);
						glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2-2, 0, b	   );
						glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2-2, 0, b+panel->h+2); 
						glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2+2, 0, b+panel->h+2);
						glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2+2, 0, b	   );
						glEnd();
						glPushMatrix();
							glTranslatef(-panel->w/2-2, 0, b);
							draw_String(panel->music[i].name,panel->w);
						glPopMatrix();

						glDisable(GL_TEXTURE_2D);
						//drawfilename(-w/2-2, 0, b+h+2);
					}
				}
				else
				{
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, b  );
					glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, b  );
					glEnd();
					glPushMatrix();
						glTranslatef(-panel->w/2, 0, b);
						draw_String(panel->music[i].name,panel->w);
					glPopMatrix();

					glDisable(GL_TEXTURE_2D);
					//drawfilename(-w/2, 0, b+h);
				}	 
			}
		}
	}
	 else
	{
		if(panel->pre_visible==1)
		{
			//满足上拉特效开始条件，上拉特效程序开始
			//首先画第一条列表，每次执行此程序都会先将第一条列表单独先画出来
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, add_b  );
			glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, add_b  );
			glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, add_b-panel->h);
			glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, add_b-panel->h);
			glEnd();
			glDisable(GL_TEXTURE_2D);

			//上拉特效的相关参数设定
			panel->visible=0;
			panel->animation=1;
			now_num=(int)(panel->new_b/(panel->h+panel->interval));
			spe_dis=panel->new_b-now_num*(panel->h+panel->interval);
			b=quan-(panel->h+panel->interval);
			
			//之下程序为在一次mainloop中画出特效中的一张画面
			if(now_num!=0)
			{
  				for(i=1;i<=now_num;i++)
				{
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, b  );
					glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, b+panel->h);
					glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, b  );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					b-=panel->h+panel->interval;
				}
			}
			if(now_num!=0)
				b+=panel->h+panel->interval;
			else
				b=quan;
			
			if(spe_dis!=0)
			{
				if(spe_dis<=panel->h)
				{		
					ratio=spe_dis/panel->h;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, ratio); glVertex3f(-panel->w/2, 0, b		   );
					glTexCoord2f(1.0, ratio); glVertex3f( panel->w/2, 0, b		   );
					glTexCoord2f(1.0, 0.0  ); glVertex3f( panel->w/2, 0, b-spe_dis);
					glTexCoord2f(0.0, 0.0);   glVertex3f(-panel->w/2, 0, b-spe_dis);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
				else
				{
					b-=spe_dis;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0);	  glVertex3f(-panel->w/2, 0, b  );
					glTexCoord2f(0.0, 1.0);   glVertex3f(-panel->w/2, 0, b+panel->h );
					glTexCoord2f(1.0, 1.0);   glVertex3f( panel->w/2, 0, b+panel->h );
					glTexCoord2f(1.0, 0.0);   glVertex3f( panel->w/2, 0, b   );
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
			}
			change_new_b(panel);
		}
		
		//上拉特效结束，各参数还原
		if(panel->new_b==0)//上拉特效结束的标示
		{
			panel->pre_visible=0;
			panel->animation=0;
		}
		
		//上拉特效结束后，只留下一个条作为提示
		if(panel->animation==0)
		{
			b=add_b;
			glEnable(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
			glBindTexture(GL_TEXTURE_2D, panel_texture[0]);
			glBegin(GL_QUADS);
			glTexCoord2f(0.0, 1.0); glVertex3f(-panel->w/2, 0, b  );
			glTexCoord2f(1.0, 1.0); glVertex3f( panel->w/2, 0, b  );
			glTexCoord2f(1.0, 0.0); glVertex3f( panel->w/2, 0, b-panel->h);
			glTexCoord2f(0.0, 0.0); glVertex3f(-panel->w/2, 0, b-panel->h);
			glEnd();
			glDisable(GL_TEXTURE_2D);
			return;
		}
	}
}

PANEL *free_Panel(PANEL *panel){
	int i;
	if (panel!=NULL && panel->music!=NULL) free(panel->music);
	if (panel!=NULL) free(panel);
	return NULL;
}
