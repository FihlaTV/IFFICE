#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef __APPLE__
#include <GL/glut.h>
#include <GL/gl.h>
#else
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#endif

#include <AR/ar.h>
extern "C"{
#include "bmploader.h"
}
#include "myvim.h"
#include "font.h"
#include "bookshelf.h"
#include "myshell.h"
#include "mus_panel.h"

//personnal definition
char		  Mus_entry[256];
char	      Mus_add_tmp[256];
char		  Mus_add_title[256];
int			  Mus_person_from=10;//增加的列表人的贴图从texture[10]开始
int			  Mus_add_mark=0;
int			  Mus_input=0;
int			  Mus_del=0;
int			  Mus_NUM=5;
int			  Mus_tmp_mark=0;//增加联系人所用的参数
int			  Mus_is_num=0;
int			  Mus_ENDURE=20;//为了不频繁出现上提下拉特效而做的容错
int			  Mus_R_num;
int			  Mus_NONE=15;
int			  Mus_rec_space=0;
double		  Mus_rec_prepara;
double		  Mus_quan;
double		  Mus_add_b;
double		  Mus_ano_b;
int			  music_panel_texture[20];
#define       Mus_GATE	6	 //阈值：触发“确定”的次数

bool		  texture_flag=false;


extern mus_PANEL *music_panel;

void Mus_init_recover() {
	int num;
	if(music_panel->num >Mus_NUM) num=Mus_NUM;
	else num=music_panel->num;

	if((music_panel->top>0)&&((music_panel->top+Mus_R_num)==music_panel->num))//利用了music_panel->num在执行删除时已经先于此函数执行了--操作，故Mus_R_num也无需再加一了
		music_panel->top--;
	music_panel->rec_work_id=Mus_NONE;//为了防止画sublist在每一次删除后，都对此参数予以复原至Mus_NONE
	music_panel->file_work_id=-2;
	music_panel->DIST=num*(music_panel->h+music_panel->interval)-music_panel->h;
	music_panel->list_long=num*(music_panel->h+music_panel->interval)-music_panel->interval;
	Mus_R_num=music_panel->num >Mus_NUM? Mus_NUM-1: music_panel->num-1;
}

void Mus_init_Panel(char *filename,char *type,char path[],double w,double h,double interval) {
	FILE *fp;
	int i;
	char char_tmp[256];
	char file_tmp[256];
	arMalloc(music_panel,mus_PANEL,1);
	strcpy(music_panel->panel_name,type);
	music_panel->num=0;
	music_panel->visible=0;
	music_panel->file_on_id=-2;
	music_panel->file_work_id=-2;
	music_panel->rec_work_id=Mus_NONE;
	music_panel->pre_visible=0;
	music_panel->animation=0;
	music_panel->animation1=0;
	music_panel->top=0;
	music_panel->new_b=0;
	music_panel->new_b1=0;
	music_panel->noFind=0;
	music_panel->w=w;
	music_panel->h=h;
	music_panel->interval=interval;
    Mus_quan=(Mus_NUM-1)*(music_panel->h+music_panel->interval);
    Mus_add_b=Mus_NUM*(music_panel->h+music_panel->interval)-music_panel->interval;
    Mus_ano_b=Mus_add_b+music_panel->h+4.0;//画联系人表头的位置
	Mus_R_num=music_panel->num >Mus_NUM? Mus_NUM-1: music_panel->num-1;

	if ((fp=fopen(filename,"r"))==NULL) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}

	while ((fgets(char_tmp,255,fp))!=NULL) {printf("%s",char_tmp);music_panel->num++;}

	if ((fseek(fp,0,SEEK_SET))!=0) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}

	arMalloc(music_panel->music,MUSIC,music_panel->num);
	for (i=0;i<music_panel->num;i++){
		//fscanf(fp,"%s",music_panel->music[i].path);
		fscanf(fp,"%s",music_panel->music[i].name);
		strcpy(music_panel->music[i].path,path);
		strcat(music_panel->music[i].path,music_panel->music[i].name);
	}
	printf("1\n");
	Mus_init_recover();
	printf("2\n");
	fclose(fp);
	if (!texture_flag){
		/* 面板的贴图预处理 */
		strcpy(file_tmp,"Data/p/panel/c.bmp");
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&music_panel_texture[0], file_tmp);

		/* 手板的贴图预处理 */
		strcpy(file_tmp,"Data/p/panel/te.bmp");
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&music_panel_texture[1], file_tmp);

		strcpy(file_tmp,"Data/p/panel/cc.bmp");
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&music_panel_texture[2], file_tmp);

		strcpy(file_tmp,"Data/p/panel/sj.bmp");
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&music_panel_texture[4], file_tmp);

		strcpy(file_tmp,"Data/p/panel/xj.bmp");
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&music_panel_texture[5], file_tmp);

		texture_flag=true;
	}
	printf("init_panel OK\n");
}



void add_music(char key) {
	FILE * fp;
	if(key!='#'){
		Mus_entry[Mus_tmp_mark++]=key;
		if(key!=' ') {
			switch(Mus_rec_space) {
				case 0:
					strcpy(Mus_add_title,"Please enter the music path:");
					Mus_add_tmp[Mus_add_mark++]=key;
					Mus_add_tmp[Mus_add_mark]='\0';
					strcat(Mus_add_title,Mus_add_tmp);
					break;
				case 1:
					strcpy(Mus_add_title,"Please enter the music name:");
					Mus_add_tmp[Mus_add_mark++]=key;
					Mus_add_tmp[Mus_add_mark]='\0';
					strcat(Mus_add_title,Mus_add_tmp);
					break;
			}
		}
		else {
			Mus_add_mark=0;
			Mus_rec_space++;
			switch(Mus_rec_space) {
				case 1:
					strcpy(Mus_add_title,"Please enter the music name:");
					break;
			}
		}

		return;
	}
	else{
		printf("%c",key);
		Mus_entry[Mus_tmp_mark++]='\n';
		Mus_entry[Mus_tmp_mark]='\0';
		fp=fopen("shell.txt","a+");
		fprintf(fp,"%s",Mus_entry);
		fclose(fp);
		fp=fopen("file_tmp.txt","w+");
		fprintf(fp,"%s",Mus_entry);
		rewind(fp);
		fscanf(fp,"%s",music_panel->music[music_panel->num].path);
		fscanf(fp,"%s",music_panel->music[music_panel->num].name);
		Mus_rec_space=0;//复原添加联系人所用到的参数
		Mus_input=20;
		strcpy(Mus_add_title,"adding music success!");
		Mus_tmp_mark=0;//恢复增加操作所用到的最重要的参数
		music_panel->num++;
		Mus_init_recover();
		printf("\n添加新music成功！\n");
		printf("music_panel->rec_work_id的大小是：%d\n",music_panel->rec_work_id);
		return;
	}
}

void del_music() {
	char char_tmp[256];					//和删除操作有关
	char data;
	char char_gather[1024]="\0";		//和删除操作有关 ,这里赋的初始值并无关系
	char file_tmp[256]="file_tmp.txt";
	int	i=0;							//i,k,z,loc1,loc2和删除操作有关
	int k=0;
	int z=0;
	int loc1=0;
	int loc2;
	FILE * fp;
	if(music_panel->rec_work_id==Mus_NONE) {
		printf("您没有选中任何要删除的music\n");
			return;
	}
	fp=fopen("shell.txt","r");
	while ((fgets(char_tmp,255,fp))!=NULL) {strcat(char_gather,char_tmp);printf("%s\n",char_tmp);}
	printf("%s\n",char_gather);
	i=Mus_R_num+music_panel->top-music_panel->rec_work_id;
	printf("您正在删除的是第%d个music\n",i);
	while(char_gather[k]!='\0'){
		if(char_gather[k]=='\n'){
			if(z==i){
				loc2=k;
				break;
			}
			else{
				loc1=k;
				z+=1;
			}
		}
		k+=1;
	}
	k+=1;
	if(i!=(music_panel->num-1)){
		if(loc1==0){
			while(char_gather[k]!='\0'){
				char_gather[k-(loc2-loc1+1)]=char_gather[k];
				k+=1;
			}
			char_gather[k-(loc2-loc1+1)]='\0';
		}
		else{
			while(char_gather[k]!='\0'){
				char_gather[k-(loc2-loc1)]=char_gather[k];
				k+=1;
			}
			char_gather[k-(loc2-loc1)]='\0';
		}
	}
	else{
		char_gather[loc1+1]='\0';
	}
	k=0;
	printf("要输入文件的数据是：%s\n",char_gather);
	fp=fopen("shell.txt","w");
	while((data=char_gather[k])!='\0'){
		fputc(data,fp);
		k+=1;
	}
	fclose(fp);
	music_panel->file_work_id=-2;
	music_panel->rec_work_id=Mus_NONE;
	music_panel->num--;
	printf("文件写入完毕，删除成功！\n");
	Mus_init_recover();
	free(music_panel->music);
	fopen("shell.txt","r");
	arMalloc(music_panel->music,MUSIC,music_panel->num);
	for (i=0;i<music_panel->num;i++){
		fscanf(fp,"%s",music_panel->music[i].path);
		fscanf(fp,"%s",music_panel->music[i].name);
	}
	music_panel->new_b=music_panel->DIST;
	fclose(fp);
	Mus_del=20;
	return;
}


/*
static void   keyEvent( unsigned char key, int x, int y)
{
	if(Mus_input > 20){
		add_music(key);
		return;
	}
    // quit if the ESC key is pressed
	switch(key)
	{

		case 'a' :
			if((music_panel->music=(MUSIC*)realloc(music_panel->music,(music_panel->num+1)*sizeof(MUSIC)))==NULL)
			{
				printf("realloc memory error!\n");
				break;
			}
			Mus_input=21;
			strcpy(Mus_add_title,"Please enter the music path:");
			printf("请输入新的music信息\n");
			break;

		case 'd' :
			del_music();
	}
}
*/


/*清除music_panel,释放内存*/
void Mus_free_Panel(){
	if (music_panel!=NULL) {
		free(music_panel->music);
		free(music_panel);
		music_panel=NULL;
	}
	return ;
}



void Mus_change_new_b(mus_PANEL *music_panel){
	if ((music_panel->visible==1) && (music_panel->pre_visible==0)){
		if (abs(music_panel->DIST-music_panel->new_b)<=8) music_panel->new_b=music_panel->DIST;
		else music_panel->new_b+=8;
	}
	if ((music_panel->visible==0) && (music_panel->pre_visible==1)){
		if (abs(music_panel->new_b)<=8) music_panel->new_b=0;
		else music_panel->new_b-=8;
	}
	return ;
}

void Mus_change_new_b1(double path){
	double LONG=music_panel->h+music_panel->interval;
	if(abs(LONG-music_panel->new_b1)<=path) music_panel->new_b1=LONG;
	else music_panel->new_b1+=path;
	return;
}

void mus_draw_Texture(int texture_id, double left_bottom_x, double left_bottom_y, double right_top_x, double right_top_y) {
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(left_bottom_x, 0, left_bottom_y);
	glTexCoord2f(1.0, 0.0); glVertex3f(right_top_x  , 0, left_bottom_y);
	glTexCoord2f(1.0, 1.0); glVertex3f(right_top_x  , 0, right_top_y  );
	glTexCoord2f(0.0, 1.0); glVertex3f(left_bottom_x, 0, right_top_y  );
	glEnd();
	glDisable(GL_TEXTURE_2D);
	return;
}

void mus_draw_Script(double x, double y, double z, int use_xyz, char *script, double page_width) {
	int str_len;
	if(use_xyz==1) {
		glPushMatrix();
		glTranslatef(x, y, z);
		draw_String(script, page_width);
		glPopMatrix();
	}
	else {
		glPushMatrix();
		str_len=strlen(script);
		glTranslatef(x-5*str_len/2, y, z+5);
		draw_String(script, page_width);
		glPopMatrix();
	}
	return;
}

void Mus_draw_Panel(double pos[]) {
	/*w长，h宽*/
	double b;
	int i;
	int now_num;
	double spe_dis;
	double ratio;
	int point;

	//程序遵循的原则是执行特效时，不执行对选择，确定等动作的判断和列表条的画，这种状态一直持续到特效完成
	i=0;
	glPushMatrix();
	glTranslatef(0.0, 0.0, 20.0);
	mus_draw_Texture(music_panel_texture[0],-music_panel->w/2,Mus_add_b,music_panel->w/2,Mus_add_b+music_panel->h);
	//如果正在增加联系人，则输出提示信息
	if(Mus_input > 0) {
		mus_draw_Script(-music_panel->w/2-20, 0.0, Mus_ano_b, 1, Mus_add_title, 150);
		if(Mus_input<=20)
			Mus_input--;
	}
	else {
		strcpy(Mus_add_title," ");
	}

	if(Mus_del > 0) {
		mus_draw_Script(-music_panel->w/2-20, 0.0, Mus_ano_b, 1, "Remove music success!", 150);
		Mus_del--;
	}
	pos[2]-=20.0;
	if((music_panel->animation==0)&&(music_panel->animation1==0))
	{
		if((pos[0]>0)&&(pos[0]<300)&&(pos[2]>-20)&&(pos[2]<((Mus_NUM+1)*(music_panel->h+music_panel->interval))))
		{
			pos[2]+=music_panel->h/2;
			music_panel->visible=1;
			point=(int)((pos[2]-Mus_add_b+music_panel->list_long)/(music_panel->h+music_panel->interval));
	   		if(point>=(Mus_R_num+1))
				point=Mus_NUM;
			if(pos[2]<15)
				point=-1;

				if(point!=music_panel->file_on_id)
				{
						Mus_is_num=0;
				}
				music_panel->file_on_id=point;
				if(Mus_is_num!=0)
				{
			 		if((pos[0]-Mus_rec_prepara)<-1)
					{
						Mus_is_num+=1;
						Mus_rec_prepara=pos[0];
						if (Mus_is_num>=Mus_GATE)
							if(music_panel->file_work_id!=point)
							{
								music_panel->file_work_id=point;
								if((music_panel->file_work_id!=Mus_NUM)&&(music_panel->file_work_id!=-1)) {
								   	music_panel->rec_work_id=point;
								}
								Mus_is_num=0;
							}
							else
							{
								if((music_panel->file_work_id!=Mus_NUM)&&(music_panel->file_work_id!=-1))
								{
									music_panel->rec_work_id=Mus_NONE;
								}
								music_panel->file_work_id=-2;
								//music_Pause();
								Mus_is_num=0;
							}
					}
					else if((pos[0]-Mus_rec_prepara)>0)
					{
						Mus_rec_prepara=pos[0];
						Mus_is_num=0;
					}
					else
					{
						Mus_rec_prepara=pos[0];
					}
				}
				else
				{
					Mus_rec_prepara=pos[0];
					Mus_is_num+=1;
				}
		}


		else if(music_panel->noFind>=Mus_ENDURE)
		{
			music_panel->visible=0;
			Mus_is_num=0;
			music_panel->noFind=0;
		}
		else
		{
			Mus_is_num=0;
			music_panel->noFind++;
		}
	}

	if(music_panel->animation1==0) {
		if(music_panel->visible==1)
		{
	  		if(music_panel->pre_visible==0)
			{
				//画第一个列表条
				Mus_change_new_b(music_panel);
				if(music_panel->new_b < music_panel->h) {//先处理第一个条
					ratio=music_panel->new_b/ music_panel->h;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0  ); glVertex3f(-music_panel->w/2, 0, Mus_add_b-music_panel->new_b);
					glTexCoord2f(1.0, 0.0  ); glVertex3f( music_panel->w/2, 0, Mus_add_b-music_panel->new_b);
					glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, Mus_add_b );
					glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, Mus_add_b );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					mus_draw_Script(0, 0, Mus_add_b-music_panel->new_b, 0, music_panel->music[0].name, music_panel->w);
				}
				else {
					mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, Mus_add_b-music_panel->h, music_panel->w/2, Mus_add_b);
					mus_draw_Script(0, 0, Mus_add_b-music_panel->h, 0, music_panel->music[0].name, music_panel->w);
				}
				music_panel->visible=1;
				music_panel->animation=1;
				now_num=(int)((music_panel->new_b-music_panel->h)/(music_panel->h+music_panel->interval));//此处的b待会儿改掉
				spe_dis=music_panel->new_b-music_panel->h-now_num*(music_panel->h+music_panel->interval);
				b=Mus_add_b-music_panel->h-(music_panel->h+music_panel->interval);

				if(now_num!=0)
				{
  					for(i=1;i<=now_num;i++)
					{
						mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b, music_panel->w/2, b+music_panel->h);
						mus_draw_Script(0, 0, b, 0, music_panel->music[i].name, music_panel->w);
						b-=music_panel->h+music_panel->interval;
					}
				}
				if(now_num!=0)
					b+=music_panel->h+music_panel->interval;
				else
					b=Mus_quan;
				if(i==0)
					i=1;
				else
					i=now_num+1;
				if(spe_dis > 0) {//目的在于既能画出非第一条之外的部分条，又不重复画第一条
					if(spe_dis<=music_panel->h)
					{
						ratio=spe_dis/music_panel->h;
						glEnable(GL_TEXTURE_2D);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
						glBegin(GL_QUADS);
						glTexCoord2f(0.0, 0.0);   glVertex3f(-music_panel->w/2, 0, b-spe_dis);
						glTexCoord2f(1.0, 0.0  ); glVertex3f( music_panel->w/2, 0, b-spe_dis);
						glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, b		  );
						glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, b		  );
						glEnd();
						glDisable(GL_TEXTURE_2D);
						mus_draw_Script(0, 0, b-spe_dis, 0, music_panel->music[i].name, music_panel->w);
					}
					else
					{
						b-=spe_dis;
						mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b, music_panel->w/2, b+music_panel->h);
						mus_draw_Script(0, 0, b, 0, music_panel->music[i].name, music_panel->w);
					}
				}
			}
			//下拉特效结束，复原各参数
			if(music_panel->new_b==music_panel->DIST)//下拉特效结束标示
			{
				music_panel->pre_visible =1;
				music_panel->animation =0;
			}
		}
		 else
		{
			if(music_panel->pre_visible==1)
			{
				if(music_panel->new_b < music_panel->h) {
					ratio=music_panel->new_b/music_panel->h;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0  ); glVertex3f(-music_panel->w/2, 0, Mus_add_b-music_panel->new_b);
					glTexCoord2f(1.0, 0.0  ); glVertex3f( music_panel->w/2, 0, Mus_add_b-music_panel->new_b);
					glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, Mus_add_b		   );
					glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, Mus_add_b		   );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					mus_draw_Script(0, 0, Mus_add_b-music_panel->new_b, 0, music_panel->music[music_panel->top].name, music_panel->w);
				}
				else {
					mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, Mus_add_b-music_panel->h, music_panel->w/2, Mus_add_b);
					mus_draw_Script(0, 0, Mus_add_b-music_panel->h, 0, music_panel->music[music_panel->top].name, music_panel->w);
				}

				music_panel->visible=0;
				music_panel->animation=1;
				now_num=(int)((music_panel->new_b-music_panel->h)/(music_panel->h+music_panel->interval));
				spe_dis=music_panel->new_b-music_panel->h-now_num*(music_panel->h+music_panel->interval);
				b=Mus_add_b-music_panel->h-(music_panel->h+music_panel->interval);

				if(now_num!=0)
				{
  					for(i=1;i<=now_num;i++)
					{
						mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b, music_panel->w/2, b+music_panel->h);
						mus_draw_Script(0, 0, b, 0, music_panel->music[music_panel->top+i].name, music_panel->w);
						b-=music_panel->h+music_panel->interval;
					}
					//system("pause");
				}
				if(now_num!=0)
					b+=music_panel->h+music_panel->interval;
				else
					b=Mus_quan;
				if(i=0)
					i=music_panel->top+1;
				else
					i=music_panel->top+i;
				if(spe_dis > 0) {//目的在于既能画出非第一条之外的部分条，又不重复画第一条
					if(spe_dis<=music_panel->h)
						{
							ratio=spe_dis/music_panel->h;
							glEnable(GL_TEXTURE_2D);
							glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
							glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
							glBegin(GL_QUADS);
							glTexCoord2f(0.0, 0.0  ); glVertex3f(-music_panel->w/2, 0, b-spe_dis);
							glTexCoord2f(1.0, 0.0  ); glVertex3f( music_panel->w/2, 0, b-spe_dis);
							glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, b		  );
							glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, b		  );
							glEnd();
							glDisable(GL_TEXTURE_2D);
							mus_draw_Script(0, 0, b-spe_dis, 0, music_panel->music[music_panel->top+i].name, music_panel->w);
						}
					else
					{
						b-=spe_dis;
						mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b, music_panel->w/2, b+music_panel->h);
						mus_draw_Script(0, 0, b, 0, music_panel->music[music_panel->top+i].name, music_panel->w);
					}
				}
				Mus_change_new_b(music_panel);
			}

			//上拉特效结束，各参数还原
			if(music_panel->new_b==0)//上拉特效结束的标示
			{
				music_panel->pre_visible=0;
				music_panel->animation=0;
			}
			glPopMatrix();
			return;
		}
	}

	if(music_panel->animation==0)
	{
		if((music_panel->file_on_id==Mus_NUM)||(music_panel->file_on_id==-1)) {
			if(music_panel->file_on_id==Mus_NUM) {
				b=Mus_add_b+music_panel->h;//贴上箭头的左上位置
				if(music_panel->file_work_id==Mus_NUM) {
					//此处贴图要变为工作时的上箭头
					glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
					glEnable(GL_BLEND);
					glColor3f(0.0, 1.0, 0.0);
					mus_draw_Texture(music_panel_texture[4], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
					glDisable(GL_BLEND);
					//要进行下移特效了,下移后要复原new_b1,更改music_panel->top
					if((music_panel->top!=0)&&(music_panel->num>Mus_NUM)) {
						printf("下移效果中music_panel->rec_work_id的大小是%d\n",music_panel->rec_work_id);
						music_panel->animation1=1;
						b=Mus_add_b;
						if(music_panel->new_b1!=(music_panel->h+music_panel->interval)) {//总的new_b1距离开始
							if(music_panel->new_b1 < music_panel->interval) {//这时上面的一个条还未下来
								b-=music_panel->new_b1;
							}//上面的条未下来的情形处理完毕
							else {//处理上面的条部分下来的情形
								ratio=(music_panel->new_b1-music_panel->interval)/music_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, 0.0); glVertex3f(-music_panel->w/2, 0, b-(music_panel->new_b1-music_panel->interval));
								glTexCoord2f(1.0, 0.0); glVertex3f( music_panel->w/2, 0, b-(music_panel->new_b1-music_panel->interval));
								glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, b	);
								glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, b  );
								glEnd();
								glDisable(GL_TEXTURE_2D);
								mus_draw_Script(0.0, 0.0, b-(music_panel->new_b1-music_panel->interval), 0, music_panel->music[music_panel->top-1].name,music_panel->w+4);
								b-=music_panel->new_b1;
							}
							for(i=Mus_NUM-1;i>=1;i--){
								if(music_panel->rec_work_id==i){
									glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
									glEnable(GL_BLEND);
									glColor3f(0.0, 1.0, 0.0);
									mus_draw_Texture(music_panel_texture[2], -music_panel->w/2-2, b-music_panel->h-2, music_panel->w/2+2, b+2);
									glDisable(GL_BLEND);
									mus_draw_Script(0.0, 0.0, b-music_panel->h-2, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w+4);
									}

								else
								{
									mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
									mus_draw_Script(0.0, 0.0, b-music_panel->h, 0, music_panel->music[Mus_R_num+music_panel->top-i].name, music_panel->w);
								}

								b-=music_panel->h+music_panel->interval;
							}
							if(music_panel->new_b1<=music_panel->h){//处理上面的条部分下来，但下面的条还未全移去的情形
								ratio=1-(music_panel->h-music_panel->new_b1)/music_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, ratio);glVertex3f(-music_panel->w/2, 0, b-(music_panel->h-music_panel->new_b1));//同上
								glTexCoord2f(1.0, ratio);glVertex3f( music_panel->w/2, 0, b-(music_panel->h-music_panel->new_b1));//注意此处的贴图坐标
								glTexCoord2f(1.0, 1.0 ); glVertex3f( music_panel->w/2, 0, b );
								glTexCoord2f(0.0, 1.0 ); glVertex3f(-music_panel->w/2, 0, b );
								glEnd();
								glDisable(GL_TEXTURE_2D);
								mus_draw_Script(0.0, 0.0, b-(music_panel->h-music_panel->new_b1), 0, music_panel->music[Mus_R_num+music_panel->top-i].name, music_panel->w+4);
							}
							Mus_change_new_b1(2.0);
						}
					}//下移特效到此结束

					if(music_panel->new_b1==(music_panel->h+music_panel->interval)) {
						music_panel->new_b1=0;
						music_panel->top--;//复原完毕
						music_panel->animation1=0;
						if(music_panel->rec_work_id!=Mus_NONE)
							music_panel->rec_work_id--;
						music_panel->file_work_id=Mus_NUM+1;
					}
				}
				else{
					mus_draw_Texture(music_panel_texture[4], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
				}//不论如何，移到上箭头处的所有情况处理完毕
			}

			else {
				b=0;
				if(music_panel->file_work_id==-1) {

					glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
					glEnable(GL_BLEND);
					glColor3f(0.0, 1.0, 0.0);
					mus_draw_Texture(music_panel_texture[5], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
					glDisable(GL_BLEND);

					if((music_panel->top < (music_panel->num-Mus_NUM))&&(music_panel->num>Mus_NUM)) {
						printf("上移中music_panel->rec_work_id的大小是%d\n",music_panel->rec_work_id);
						music_panel->animation1=1;
						b=Mus_add_b;
						if(music_panel->new_b1!=(music_panel->h+music_panel->interval)) {//总的new_b1距离开始
							if(music_panel->new_b1 > music_panel->h) { //这时上面的一个条已完全上去
								b=b-(music_panel->h+music_panel->interval)+music_panel->new_b1;
							}
							else {//处理上面的条部分上去的情形
								ratio=(music_panel->h-music_panel->new_b1)/music_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, 0.0); glVertex3f(-music_panel->w/2, 0, b-(music_panel->h-music_panel->new_b1));
								glTexCoord2f(1.0, 0.0); glVertex3f( music_panel->w/2, 0, b-(music_panel->h-music_panel->new_b1));
								glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, b);
								glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, b  );
								glEnd();
								glDisable(GL_TEXTURE_2D);
								b=b-(music_panel->h+music_panel->interval)+music_panel->new_b1;
								mus_draw_Script(0.0, 0.0, b-(music_panel->h-music_panel->new_b1), 0, music_panel->music[music_panel->top].name, music_panel->w);
							}
							for(i=Mus_NUM-2;i>=0;i--) {
								if(music_panel->rec_work_id==i){
									glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
									glEnable(GL_BLEND);
									glColor3f(0.0, 1.0, 0.0);
									mus_draw_Texture(music_panel_texture[2], -music_panel->w/2-2, b-music_panel->h-2, music_panel->w/2+2, b+2);
									glDisable(GL_BLEND);
									mus_draw_Script(0.0, 0.0, b-music_panel->h-2, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w+4);
								}
								else
								{
									mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
									mus_draw_Script(0.0, 0.0, b-music_panel->h, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w);
									//drawfilename(-w/2, 0, b+h);
								}

								b-=music_panel->h+music_panel->interval;
							}
							if(music_panel->new_b1 > music_panel->interval){//处理下面的条已有部分上来的状况
								ratio=1-(music_panel->new_b1-music_panel->interval)/music_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, music_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, ratio); glVertex3f(-music_panel->w/2, 0, b-(music_panel->new_b1-music_panel->interval));//同上
								glTexCoord2f(1.0, ratio); glVertex3f( music_panel->w/2, 0, b-(music_panel->new_b1-music_panel->interval));//注意此处的贴图坐标
								glTexCoord2f(1.0, 1.0); glVertex3f( music_panel->w/2, 0, b);
								glTexCoord2f(0.0, 1.0); glVertex3f(-music_panel->w/2, 0, b);
								glEnd();
								glDisable(GL_TEXTURE_2D);
								mus_draw_Script(0.0, 0.0, b-(music_panel->new_b1-music_panel->interval), 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w);
							}
							Mus_change_new_b1(2.0);
						}
					}

					if(music_panel->new_b1==(music_panel->h+music_panel->interval)) {
						music_panel->new_b1=0;
						music_panel->top++;//复原完毕
						music_panel->animation1=0;
						if(music_panel->rec_work_id!=Mus_NONE)
							music_panel->rec_work_id++;
						music_panel->file_work_id=Mus_NUM+1;
					}
				}
				else{
					mus_draw_Texture(music_panel_texture[5], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
				}
			}
		}
	}
	

	b=Mus_add_b+music_panel->h+music_panel->interval;
	if((music_panel->animation1==0)&&(music_panel->animation==0)) {
		//画0~Mus_R_num个表条
		for(i=Mus_R_num;i>=0;i--)
		{
			b-=music_panel->h+music_panel->interval;
			if((music_panel->file_on_id==i) || (music_panel->file_work_id==i) || (music_panel->rec_work_id==i))
			{
				if((music_panel->file_work_id==i) || (music_panel->rec_work_id==i))
				{
					glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
					glEnable(GL_BLEND);
					glColor4f(1.0, 0.0, 0.0, 0.5);
					mus_draw_Texture(music_panel_texture[2], -music_panel->w/2-2, b-music_panel->h-2, music_panel->w/2+2, b+2);
					glDisable(GL_BLEND);
					mus_draw_Script(0.0, 0.0, b-music_panel->h, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w+4);
				}
				else
				{
					mus_draw_Texture(music_panel_texture[2], -music_panel->w/2-2, b-music_panel->h-2, music_panel->w/2+2, b+2);
					mus_draw_Script(0.0, 0.0, b-music_panel->h, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w+2);
				}
			}
			else
			{
				mus_draw_Texture(music_panel_texture[0], -music_panel->w/2, b-music_panel->h, music_panel->w/2, b);
				mus_draw_Script(0.0, 0.0, b-music_panel->h, 0, music_panel->music[Mus_R_num+music_panel->top-i].name,music_panel->w);
			}
		}
	}
	glPopMatrix();
}

