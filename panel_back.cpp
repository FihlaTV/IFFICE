#include "panel.h"
#include "myvim.h"
#include "ait.h"
#include <AR/ar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

const int     GATE=8;
int			  is_num=0;
double		  rec_prepara;

extern void getbuff(char *buf,int n,FILE *fp);

PANEL *init_Panel(char *filename,char *type,char path[]){
	FILE *fp;
	int i,j;
	int buf[256];
	PANEL *panel;
	arMalloc(panel,PANEL,1);
	strcpy(panel->panel_name,type);
	panel->num=0;
	panel->visible=0;
	panel->file_on_id=-1;
	panel->file_work_id=-1;

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
	printf("donepanel\n");
	fclose(fp);
	return panel;
}

void draw_Panel(PANEL *panel,double w,double h,double pos[]){
	/*w长，h宽*/
	double b=-h;
	int i;
	int point;
	if((pos[0]>0)&&(pos[0]<200)&&(pos[2]>0)&&(pos[2]<(h*(panel->num+2)))&&(pos[1]>-30)&&(pos[1]<30))
	{
		point=(int)(pos[2]/h);
		if(point>panel->num)
			point=panel->num-1;
		if(point<0)
			point=0;
		if(point<panel->num)
		{
			if(point!=panel->file_on_id){
					is_num=0;
					//rec_prepara=1000;
			}	
			panel->file_on_id=point;
			if (is_num!=0){
			if((pos[0]-rec_prepara)<-1)
				{
					is_num+=1;
					rec_prepara=pos[0];
					if (is_num>=GATE)
						if(panel->file_work_id!=point)
						{
							printf("%d\n",point);
							panel->file_work_id=point;
							music_Play(&panel->music[point]);
							is_num=0;
					//		rec_prepara=1000;
						}
						else
						{
							panel->file_work_id=-1;
							music_Pause();
							is_num=0;
					//		rec_prepara=1000;
						}
				}
				else
				{
					if ((pos[0]-rec_prepara)>0)
					{
						rec_prepara=pos[0];
						is_num=0;
					}
					else {
						rec_prepara=pos[0];
					}
				}
			}
			else {
				rec_prepara=pos[0];
				is_num=1;
			}
		}
	}
		//glDisable(GL_LIGHTING);
		for(i=0;i<panel->num;i++)
		{
			b+=h;
			if((panel->file_on_id==i) || (panel->file_work_id==i))
			{
				if(panel->file_work_id==i)
				{
					//glEnable(GL_BLEND);
					glColor3f(1.0, 0.2, 0.2);
					glBegin(GL_QUADS);
						glVertex3f(-w/2-2, 0, b    );
						glVertex3f(-w/2-2, 0, b+h+2);
						glVertex3f( w/2+2, 0, b+h+2);
						glVertex3f( w/2+2, 0, b    );
					glEnd();
					//glDisable(GL_BLEND);
					//printf("%s\n",panel->music[i].path);
					glPushMatrix();
					glTranslatef(-w/2-2, 0, b);
						draw_String(panel->music[i].name,100);
					glPopMatrix();
					
				}
				else
				{
					glColor3f(0.2, 0.2, 0.2);
					glBegin(GL_QUADS);
						glVertex3f(-w/2-2, 0, b	   );
						glVertex3f(-w/2-2, 0, b+h+2); 
						glVertex3f( w/2+2, 0, b+h+2);
						glVertex3f( w/2+2, 0, b	   );
					glEnd();
					//drawfilename(-w/2-2, 0, b+h+2);
					glPushMatrix();
					glTranslatef(-w/2-2, 0, b);
						draw_String(panel->music[i].name,100);
					glPopMatrix();
				}
			}
			else
			{
				glColor3f(0.2,0.2,0.2);
				glBegin(GL_QUADS);
					glVertex3f(-w/2, 0, b  );
					glVertex3f(-w/2, 0, b+h);
					glVertex3f( w/2, 0, b+h);
					glVertex3f( w/2, 0, b  );
				glEnd();
				//drawfilename(-w/2, 0, b+h);
				glPushMatrix();
				glTranslatef(-w/2, 0, b);
					draw_String(panel->music[i].name,100);
				glPopMatrix();
			}	
		}
}

void free_Panel(PANEL *panel){
	int i;
	free(panel->music);
	free(panel);
	panel=NULL;
	return ;
}
