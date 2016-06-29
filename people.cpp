#include "addresslist.h"
#include "myvim.h"
#include "bookshelf.h"
#include "font.h"
#include "myshell.h"
#include "panel.h"

extern "C"{
#include "bmploader.h"
}

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

#include <AR/ar.h>

int persons=1;
PERSON person[]={"look",123456,"13484585026","lookup@love.office","Data/people/lookup.bmp"};



//personnal definition
char		  Peo_entry[256];
char	      Peo_add_tmp[256];
char		  Peo_add_title[256];
int			  Peo_person_from=10;//增加的列表人的贴图从texture[10]开始
int			  Peo_add_mark=0;
int			  Peo_input=0;
int			  Peo_del=0;
int			  Peo_NUM=5;
int			  Peo_tmp_mark=0;//增加联系人所用的参数
int			  Peo_is_num=0;
int			  Peo_ENDURE=20;//为了不频繁出现上提下拉特效而做的容错
int			  Peo_sub_is_num=0;
int			  Peo_R_num;
int			  Peo_NONE=15;
int			  Peo_rec_space=0;
int			  Peo_on_anoid=0;
double		  Peo_sub_rec_prepara;
double		  Peo_rec_prepara;
double		  Peo_quan;
double		  Peo_add_b;
double		  Peo_ano_b;
int			  Peo_panel_texture[25];//the maximum of linkman in this program is 25
int			  Peo_sublist_texture[4];//include the icon of phoning
#define       Peo_GATE	6	 //阈值：触发“确定”的次数
char		  *PATH="Data/music/music0/";
double		  PERSON_W=80.0;
double		  PERSON_H=20.0;
double        PERSON_INTERVAL=6.0;




PANEL *Peo_panel;

//作为两个不断变化的变量Peo_panel->DIST和Peo_panel->list的赋值函数,此函数在包括init_panel等函数中调用
void Peo_init_recover() {
	int num;
	if(Peo_panel->num >Peo_NUM) num=Peo_NUM;
	else num=Peo_panel->num;
	
	if((Peo_panel->top>0)&&((Peo_panel->top+Peo_R_num)==Peo_panel->num))//利用了Peo_panel->num在执行删除时已经先于此函数执行了--操作，故Peo_R_num也无需再加一了
	Peo_panel->top--;
	Peo_panel->rec_work_id=Peo_NONE;//为了防止画sublist在每一次删除后，都对此参数予以复原至Peo_NONE
	Peo_panel->file_work_id=-2;
	Peo_panel->DIST=(num-1)*(Peo_panel->h+Peo_panel->interval);
	Peo_panel->list_long=num*(Peo_panel->h+Peo_panel->interval)-Peo_panel->interval;
	Peo_R_num=Peo_panel->num >Peo_NUM? Peo_NUM-1: Peo_panel->num-1;
}

/*
void init_Person(){
	glEnable(GL_TEXTURE_2D);
		loadGLTexture(&person[0].head_texture,person[0].picture);
	glDisable(GL_TEXTURE_2D);
	return;
}
*/


//读入文件，初始化结构体,完成贴图预处理
void Peo_init_Panel(char *type,double w,double h,double interval) {
	FILE *fp;
	int i;
	char * filename="Data/config/addresslist";
	char char_tmp[256];
	char file_tmp[256];
	arMalloc(Peo_panel,PANEL,1);
	strcpy(Peo_panel->panel_name,type);
	Peo_panel->num=0;
	Peo_panel->visible=0;
	Peo_panel->file_on_id=-2;
	Peo_panel->file_work_id=-2;
	Peo_panel->rec_work_id=Peo_NONE;
	Peo_panel->pre_visible=0;
	Peo_panel->animation=0;
	Peo_panel->animation1=0;
	Peo_panel->animation2=1;
	Peo_panel->top=0;
	Peo_panel->new_b=0;
	Peo_panel->new_b1=0;
	Peo_panel->noFind=0;
	Peo_panel->w=w;
	Peo_panel->h=h;
	Peo_panel->interval=interval;
	Peo_panel->person_w=PERSON_W;
	Peo_panel->person_h=PERSON_H;
	Peo_panel->person_interval=PERSON_INTERVAL;
	Peo_panel->sub_file_on_id=-1;
	Peo_panel->sub_file_work_id=-1;
    Peo_quan=(Peo_NUM-1)*(Peo_panel->h+Peo_panel->interval);
    Peo_add_b=Peo_NUM*(Peo_panel->h+Peo_panel->interval)-Peo_panel->interval;
    Peo_ano_b=Peo_add_b+Peo_panel->h+4.0;//画联系人表头的位置
	Peo_R_num=Peo_panel->num >Peo_NUM? Peo_NUM-1: Peo_panel->num-1;
	
glEnable(GL_TEXTURE_2D);
		loadGLTexture(&person[0].head_texture,person[0].picture);
	glDisable(GL_TEXTURE_2D);
	
	if ((fp=fopen(filename,"r"))==NULL) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}
	
	while ((fgets(char_tmp,255,fp))!=NULL) {printf("%s",char_tmp);Peo_panel->num++;}
	printf("%d=============================\n",Peo_panel->num);
	if ((fseek(fp,0,SEEK_SET))!=0) {
		printf("error while opening file %s\n",filename);
		exit(1);
	}

	arMalloc(Peo_panel->person,PERSON,Peo_panel->num);
	for (i=0;i<Peo_panel->num;i++){
		fscanf(fp,"%s",Peo_panel->person[i].name);
		fscanf(fp,"%s",Peo_panel->person[i].phone);
		fscanf(fp,"%s",Peo_panel->person[i].email);
		fscanf(fp,"%s",Peo_panel->person[i].picture);
		Peo_panel->person[i].id=i;
		//strcpy(Peo_panel->person[i].path,path);
		//strcat(Peo_panel->person[i].path,Peo_panel->person[i].name);
	}
	
	for (i=0;i<Peo_panel->num;i++){
		printf("%s %s %s %s\n",Peo_panel->person[i].name,Peo_panel->person[i].phone,Peo_panel->person[i].email,Peo_panel->person[i].picture);
	}
	Peo_init_recover();
	fclose(fp);
	/* 面板的贴图预处理 */
	strcpy(file_tmp,"Data/p/addresslist/c.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_panel_texture[0], file_tmp);

	/* 手板的贴图预处理 */
	strcpy(file_tmp,"Data/p/addresslist/te.bmp");//联系人列表标示图标
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_panel_texture[1], file_tmp);

	strcpy(file_tmp,"Data/p/addresslist/cc.bmp");//联系人列表标示图标
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_panel_texture[2], file_tmp);

	strcpy(file_tmp,"Data/p/addresslist/sj.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_panel_texture[4], file_tmp);

	strcpy(file_tmp,"Data/p/addresslist/xj.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_panel_texture[5], file_tmp);

//联系人头像读入
	for (i=0;i<Peo_panel->num;i++){
		strcpy(file_tmp,Peo_panel->person[i].picture);
		glEnable(GL_TEXTURE_2D);
		loadGLTexture(&Peo_panel_texture[7+i], file_tmp);
	}

//联系人头像读入结束
	strcpy(file_tmp,"Data/p/addresslist/mail.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_sublist_texture[0], file_tmp);

	strcpy(file_tmp,"Data/p/addresslist/phone.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_sublist_texture[1], file_tmp);

	strcpy(file_tmp,"Data/p/addresslist/arrow.bmp");
	glEnable(GL_TEXTURE_2D);
	loadGLTexture(&Peo_sublist_texture[2], file_tmp);
	
	printf("init_panel OK\n");
	return;
}

void add_person(char key) {
	FILE * fp;
	int i; //用于缺省输入时的相关信息注入
	char file_tmp[256];
	if(!((key==' ')&&(Peo_rec_space==4))) {
		Peo_entry[Peo_tmp_mark++]=key;
		if(key!=' ') {
			switch(Peo_rec_space) {
				case 0:		
					strcpy(Peo_add_title,"Please enter the name:");
					Peo_add_tmp[Peo_add_mark++]=key;
					Peo_add_tmp[Peo_add_mark]='\0';
					strcat(Peo_add_title,Peo_add_tmp);
					printf("Peo_add_title的大小是：%s\n",Peo_add_title);
					break;
				case 1:
					strcpy(Peo_add_title,"Please enter the phone num:");
					Peo_add_tmp[Peo_add_mark++]=key;
					Peo_add_tmp[Peo_add_mark]='\0';
					strcat(Peo_add_title,Peo_add_tmp);
					printf("Peo_add_title的大小是：%s\n",Peo_add_title);
					break;
				case 2:
					strcpy(Peo_add_title,"Please enter the Email address:");
					Peo_add_tmp[Peo_add_mark++]=key;
					Peo_add_tmp[Peo_add_mark]='\0';
					strcat(Peo_add_title,Peo_add_tmp);
					printf("Peo_add_title的大小是：%s\n",Peo_add_title);
					break;
				case 3:
					strcpy(Peo_add_title,"Please enter the picture path:");
					Peo_add_tmp[Peo_add_mark++]=key;
					Peo_add_tmp[Peo_add_mark]='\0';
					strcat(Peo_add_title,Peo_add_tmp);
					printf("Peo_add_title的大小是：%s\n",Peo_add_title);
					break;
			}
		}
		else {
			if(strlen(Peo_add_tmp)==0) {
		 		printf("出现了正确的空格\n");
				Peo_tmp_mark--;
				if(Peo_rec_space<=2) {
					strcpy(Peo_add_tmp, "NONE");
					for(i=0; i<strlen(Peo_add_tmp); i++) {
						Peo_entry[Peo_tmp_mark++]=Peo_add_tmp[i];
					}
					Peo_entry[Peo_tmp_mark++]=' ';//给最后加上空格作为分隔符
				}
				else {
					strcpy(Peo_add_tmp, "Data/p/addresslist/te.bmp");
					for(i=0; i<strlen(Peo_add_tmp); i++) {
						Peo_entry[Peo_tmp_mark++]=Peo_add_tmp[i];
					}
				}
			}
			Peo_add_mark=0;
			Peo_add_tmp[0]='\0';
			Peo_rec_space++;
			switch(Peo_rec_space) {
				case 1:
					strcpy(Peo_add_title,"Please enter the phone num:");
					break;
				case 2:
					strcpy(Peo_add_title,"Please enter the Email address:");
					break;
				case 3:
					strcpy(Peo_add_title,"Please enter the picture path:");
					break;
			}
		}
		
		return;
	}
	else{
		printf("%c",key);
		Peo_entry[Peo_tmp_mark++]='\n';
		Peo_entry[Peo_tmp_mark]='\0';
		fp=fopen("Data/config/addresslist","a+");
		fprintf(fp,"%s",Peo_entry);
		fclose(fp);
		fp=fopen("file_tmp.txt","w+");
		fprintf(fp,"%s",Peo_entry);
		rewind(fp);
		fscanf(fp,"%s",Peo_panel->person[Peo_panel->num].name);
		fscanf(fp,"%s",Peo_panel->person[Peo_panel->num].phone); 
		fscanf(fp,"%s",Peo_panel->person[Peo_panel->num].email);
		fscanf(fp,"%s",Peo_panel->person[Peo_panel->num].picture);
		Peo_rec_space=0;//复原添加联系人所用到的参数
		Peo_input=20;
		strcpy(Peo_add_title,"Adding Success!");
		Peo_tmp_mark=0;//恢复增加操作所用到的最重要的参数

		//对新增的联系人的头像进行读入
		strcpy(file_tmp,Peo_panel->person[Peo_panel->num].picture);
		glEnable(GL_TEXTURE_2D);
		printf("读入成功！\n");
		loadGLTexture(&Peo_panel_texture[7+Peo_panel->num], file_tmp);
		//新头像读入完成

		Peo_panel->num++;
		Peo_init_recover(); 
		return;
	}
}

void del_person() {
	char char_tmp[256];					//和删除操作有关
	char data;
	char char_gather[1024]="\0";		//和删除操作有关
	char file_tmp[256]="file_tmp.txt";
	int	i=0;							//i,k,z,loc1,loc2和删除操作有关
	int k=0;
	int z=0;
	int loc1=0;
	int loc2;
	FILE * fp;
	if(Peo_panel->rec_work_id==Peo_NONE) {
		printf("您没有选中任何要删除的联系人\n");
			return;
	}
	fp=fopen("Data/config/addresslist","r");
	//将全部person的数据读入到char_gather数组中
	while ((fgets(char_tmp,255,fp))!=NULL) {strcat(char_gather,char_tmp);printf("%s\n",char_tmp);}
	printf("%s\n",char_gather);
	i=Peo_R_num+Peo_panel->top-Peo_panel->rec_work_id;
	printf("您正在删除的是第%d联系人\n",i);
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
	if(i!=(Peo_panel->num-1)){
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
	fp=fopen("Data/config/addresslist","w");
	while((data=char_gather[k])!='\0'){
		fputc(data,fp);
		k+=1;
	}
	fclose(fp);
	Peo_panel->file_work_id=-2;
	Peo_panel->rec_work_id=Peo_NONE;
	Peo_panel->num--;
	printf("文件写入完毕，删除成功！\n");
	Peo_init_recover();
	free(Peo_panel->person);
	fopen("Data/config/addresslist","r");
	arMalloc(Peo_panel->person,PERSON,Peo_panel->num);
	for (i=0;i<Peo_panel->num;i++){
		fscanf(fp,"%s",Peo_panel->person[i].name); 
		fscanf(fp,"%s",Peo_panel->person[i].phone);
		fscanf(fp,"%s",Peo_panel->person[i].email);
		fscanf(fp,"%s",Peo_panel->person[i].picture);
		Peo_panel->person[i].id=i;
	}
	Peo_panel->new_b=Peo_panel->DIST;
	fclose(fp);
	Peo_del=20;
	return;     
}

/*
void free_Panel(PANEL *Peo_panel){
	free(Peo_panel->person);
	free(Peo_panel);
	Peo_panel=NULL;

	return ;
}
*/
void Peo_change_new_b(PANEL *Peo_panel){
	if ((Peo_panel->visible==1) && (Peo_panel->pre_visible==0)){
		if (abs(Peo_panel->DIST-Peo_panel->new_b)<=10) Peo_panel->new_b=Peo_panel->DIST;
		else Peo_panel->new_b+=10;
	}
	if ((Peo_panel->visible==0) && (Peo_panel->pre_visible==1)){
		if (abs(Peo_panel->new_b)<=10) Peo_panel->new_b=0;
		else Peo_panel->new_b-=10;
	}
	return ;
}

void Peo_change_new_b1(double path){
	double LONG=Peo_panel->h+Peo_panel->interval;
	if(abs(LONG-Peo_panel->new_b1)<=path) Peo_panel->new_b1=LONG;
	else Peo_panel->new_b1+=path;
	return;
}

void draw_Texture(int texture_id, double left_bottom_x, double left_bottom_y, double right_top_x, double right_top_y) {
	glEnable(GL_TEXTURE_2D);
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

void draw_Quards(double left_bottom_x, double left_bottom_y, double right_top_x, double right_top_y) {
	glBegin(GL_QUADS);
    glVertex3f(left_bottom_x, 0, left_bottom_y);
    glVertex3f(right_top_x  , 0, left_bottom_y);
    glVertex3f(right_top_x  , 0, right_top_y  );
    glVertex3f(left_bottom_x, 0, right_top_y  );
    glEnd();
}

//if use_xyz=1, xyz is used to wholy locate the script, 
//else xyz is used only to relocate the script to the middle of the page with the page_width
void draw_Script(double x, double y, double z, int use_xyz, char *script, double page_width) {
	int str_len;	
	if(use_xyz==1) {
		glPushMatrix();
		glTranslatef(x, y, z);
		glScalef(1.2, 1.2, 1.2);
		draw_String(script, page_width/1.2);
		glPopMatrix();
	}
	else {
		glPushMatrix();
		str_len=strlen(script);
		glTranslatef(x-5*str_len/2, y, z+5);
		glScalef(1.3, 1.3, 1.3);
		draw_String(script, page_width);
		glPopMatrix();
	}
	return;
}

//对于子列表来说，所有的on_id都不重要，重要的是work_id，包括了：Peo_panel->file_work_id及Peo_panel->rec_work_id;
//原来的rec_work_id似乎和file_work_id挂钩，
void Peo_draw_sublist(double pos[] ,int Peo_R_num) {
	double  bottom=(Peo_NUM+1)*(Peo_panel->h+Peo_panel->interval);
	int		point;
	int		i;
	double  top;
	double  b=0;
	char	char_tmp[256];
//开始画头像和任务标示，只要Peo_panel->rec_work_id！=Peo_NONE就画，只要Peo_panel->sub_file_work_id！=-1就画
	i=Peo_R_num+Peo_panel->top-Peo_panel->rec_work_id;
	glPushMatrix();
	glTranslatef(-180.0, 0.0, bottom);//Peo_ano_b+Peo_panel->h+Peo_panel->interval
	draw_Texture(Peo_panel_texture[7+i], 30.0, -40.0, Peo_panel->w+30.0, 70);
	glPopMatrix();

	if(Peo_panel->sub_file_on_id!=-1) {
		glPushMatrix();
		glTranslatef(-180.0, 0.0, 20.0);
		if(Peo_panel->sub_file_on_id>1)
			i=1;
		else 
			i=Peo_panel->sub_file_on_id;
		draw_Texture(Peo_sublist_texture[i], 40.0, 0.0, Peo_panel->w+20.0, 80);
		glPopMatrix();
	}
//头像和任务标示画完
	if((Peo_panel->file_on_id==Peo_NUM)&&(pos[2]>bottom))
		Peo_panel->file_on_id=-2;//防止上箭头老是出现
	glPushMatrix();
	glTranslatef(0,0,bottom);
	if((pos[0]>0)&&(pos[0]<300)&&(pos[2]>(bottom-Peo_panel->person_interval))&&(pos[2]<(bottom+3*(Peo_panel->person_h+Peo_panel->person_interval))))
	{
		top=pos[2]-bottom+Peo_panel->person_interval/2;
		point=(int)(top/(Peo_panel->person_h+Peo_panel->person_interval));
		if(point>=2)
			point=2;
		if(point<=0)
			point=0;
		if(point<=2)
		{
			if(point!=Peo_panel->sub_file_on_id)
			{
					Peo_panel->sub_file_on_id=0;
			} 
			Peo_panel->sub_file_on_id=point;
			if(Peo_sub_is_num!=0)
			{
	 			if((pos[0]-Peo_sub_rec_prepara)<-1)
				{
					Peo_sub_is_num+=1;
					Peo_sub_rec_prepara=pos[0];
					if (Peo_sub_is_num>=Peo_GATE)
						if(Peo_panel->sub_file_work_id!=point)
						{
							Peo_panel->sub_file_work_id=point;
							 //music_Play(&Peo_panel->person[point]);
							Peo_sub_is_num=0;
						}
						else
						{
							Peo_panel->sub_file_work_id=-1;
							//music_Pause();
							Peo_sub_is_num=0;
						}
				}
				else if((pos[0]-Peo_sub_rec_prepara)>0)
				{
					Peo_sub_rec_prepara=pos[0];
					Peo_sub_is_num=0;
				}
				else
				{
					Peo_sub_rec_prepara=pos[0];
				}
			}
			else
			{
				Peo_sub_rec_prepara=pos[0];
				Peo_sub_is_num+=1;
			}
		
		}
	}
	else{
		Peo_sub_is_num=0;
	}
	b=2*(Peo_panel->person_h+Peo_panel->person_interval);
	draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->person_h);
	draw_Script(0.0, 0.0, b, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-Peo_panel->rec_work_id].name,Peo_panel->person_w);	
	b=-(Peo_panel->person_h+Peo_panel->person_interval);
	for(i=0;i<=1;i++)
	{
		switch(i){
			case 0:strcpy(char_tmp,Peo_panel->person[Peo_R_num+Peo_panel->top-Peo_panel->rec_work_id].email);
				break;
			case 1:strcpy(char_tmp,Peo_panel->person[Peo_R_num+Peo_panel->top-Peo_panel->rec_work_id].phone);
				break;
			default: printf("不存在这项选择!\n");
		}
		b+=Peo_panel->person_h+Peo_panel->person_interval;
		if((Peo_panel->sub_file_on_id==i) || (Peo_panel->sub_file_work_id==i))
		{
			if(Peo_panel->sub_file_work_id==i)
			{	
				glDisable(GL_LIGHTING);
				glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
				glEnable(GL_BLEND);	
				glDisable(GL_DEPTH_TEST);
				glColor4f(0.8, 0.6, 0.4, 0.3);
				draw_Texture(Peo_panel_texture[2], -Peo_panel->w/2, b, Peo_panel->w/2+3, b+Peo_panel->person_h+2);
				draw_Script(0.0, 0.0, b, 0, char_tmp,Peo_panel->person_w);
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
			else
			{
				draw_Texture(Peo_panel_texture[2], -Peo_panel->w/2, b, Peo_panel->w/2+3, b+Peo_panel->person_h+2);
				draw_Script(0.0, 0.0, b, 0, char_tmp,Peo_panel->person_w);
			}
		}
		else
		{
			draw_Texture(Peo_panel_texture[2], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->person_h);
			draw_Script(0.0, 0.0, b, 0, char_tmp,Peo_panel->person_w);
		}	 
	}
	glPopMatrix();
}

void Peo_draw_Panel(double pos[]) {
	/*w长，h宽*/
	double b;
	int i;
	int now_num;
	double spe_dis;
	double ratio;
	int point;
//	pos[2]-=150;
	//如果正在增加联系人，则输出提示信息
	if(Peo_input > 0) {
		glColor3f(0.3, 0.3, 1.0); //写字处的背景条的颜色设为蓝色
		glPushMatrix();
		glTranslatef(-150.0, 0.0, 10.0);
		draw_Quards(-10.0, -10.0, 150.0, 12.0);
		draw_Script(0.0, 0.0, 0.0, 1, Peo_add_title,150); 
		glPopMatrix();
		if(Peo_input<=20)
			Peo_input--;
	}
	else {
		strcpy(Peo_add_title," ");
	}

	//如果删除成功，则输出提示信息
	if(Peo_del > 0) {
		glColor3f(0.3, 0.3, 1.0);
		glPushMatrix();
		glTranslatef(-150.0, 0.0, 10.0);
		draw_Quards(-10.0, -10.0, 150.0, 12.0);
		draw_Script(0.0, 0.0, 0.0, 1, "Remove Success!",150); 
		glPopMatrix();
		Peo_del--;
	}
	//程序遵循的原则是执行特效时，不执行对选择，确定等动作的判断和列表条的画，这种状态一直持续到特效完成
	i=0;
	glPushMatrix(); 
	glTranslatef(50.0, 0.0, 20.0); 
	//========================只要执行drawobject就先画第一个列表条,即表头，上标明“联系人表单“===================
	//可在此加上画"联系人列表单表条"  
	if(Peo_panel->animation2==1) {
		draw_Texture(Peo_panel_texture[1], -Peo_panel->w/3, Peo_add_b, Peo_panel->w/3, Peo_add_b+50);
	}
	//============================================================================================================
	pos[2]-=20.0;//必须要改变的，使下箭头画在patt上面，这样便于测试

	if((Peo_panel->animation==0)&&(Peo_panel->animation1==0))
	{
		if((pos[0]>0)&&(pos[0]<150)&&(pos[1]>-100)&&(pos[1]<100)&&(pos[2]>-20)&&(pos[2]<((Peo_NUM+1)*(Peo_panel->h+Peo_panel->interval))))
		{
			pos[2]+=Peo_panel->h/2;
			Peo_panel->visible=1;
			point=(int)((pos[2]-Peo_add_b+Peo_panel->list_long)/(Peo_panel->h+Peo_panel->interval));
	   		if(point>=(Peo_R_num+1))
				point=Peo_NUM;
			if(pos[2]<=Peo_panel->h/2)//////////////////////////
				point=-1;

				if(point!=Peo_panel->file_on_id)
				{
					if(Peo_on_anoid>=10) {//To avoid the unwanted moving on other bars, use the Peo_on_aon_id to reduce the wrong rate,just as the noFind.
						Peo_is_num=0;
						Peo_on_anoid=0;
					}
				} 
				Peo_panel->file_on_id=point;
				if(Peo_is_num!=0)            
				{
			 		if((pos[0]-Peo_rec_prepara)<-1)
					{
						Peo_is_num+=1;
						Peo_rec_prepara=pos[0];
						if (Peo_is_num>=Peo_GATE)
							if(Peo_panel->file_work_id!=point)
							{ 
								Peo_panel->file_work_id=point;
								if((Peo_panel->file_work_id!=Peo_NUM)&&(Peo_panel->file_work_id!=-1)) {
								   	Peo_panel->rec_work_id=point;
									Peo_panel->sub_file_on_id=-1;
									Peo_panel->sub_file_work_id=-1;
								}
								//music_Play(&Peo_panel->music[point]);
								Peo_is_num=0;
							}
							else
							{
								if((Peo_panel->file_work_id!=Peo_NUM)&&(Peo_panel->file_work_id!=-1))
								{
									Peo_panel->rec_work_id=Peo_NONE;
									Peo_panel->sub_file_on_id=-1;
									Peo_panel->sub_file_work_id=-1;
								}
								Peo_panel->file_work_id=-2;
								//music_Pause();
								Peo_is_num=0;
							}
					}
					else if((pos[0]-Peo_rec_prepara)>0)
					{
						Peo_rec_prepara=pos[0];
						Peo_is_num=0;
					}
					else
					{
						Peo_rec_prepara=pos[0];
					}
				}
				else
				{
					Peo_rec_prepara=pos[0];
					Peo_is_num+=1;
				}	
		}

		//在联系人列表中，选中某人后不能执行上提下拉操作，但在音乐列表中，此操作可以，故此部分有变化
		else if(Peo_panel->rec_work_id==Peo_NONE)     
		{
			if(Peo_panel->noFind>=Peo_ENDURE)
			{
				Peo_panel->visible=0;
				Peo_is_num=0;
				Peo_panel->noFind=0;
			}
			else
			{
				Peo_is_num=0;
				Peo_panel->noFind++;
			}
		}
		else{
			Peo_is_num=0;
		};
	}

//之下的处理在animation1=0时处理
	if((Peo_panel->animation1==0)&&(Peo_panel->rec_work_id==Peo_NONE)) {
		if(Peo_panel->visible==1)
		{
	  		if(Peo_panel->pre_visible==0)
			{
				//printf("开始下拉特效\n");
				//printf("开始下拉特效\n");
				//下拉特效程序开始，首先画第一个列表条
				//now_num作为下面要画几个完整的列表（包括了各表的间隔和表条）,b作为所有画的列表条的左下坐标
				//ratio是部分条的比例大小	
				//画第一个列表条
				Peo_change_new_b(Peo_panel);
				if(Peo_panel->new_b < Peo_panel->h) {//先处理第一个条
					ratio=Peo_panel->new_b/ Peo_panel->h;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
					glBegin(GL_QUADS);	
					glTexCoord2f(0.0, 0.0  ); glVertex3f(-Peo_panel->w/2, 0, Peo_add_b-Peo_panel->new_b);
					glTexCoord2f(1.0, 0.0  ); glVertex3f( Peo_panel->w/2, 0, Peo_add_b-Peo_panel->new_b);
					glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, Peo_add_b );
					glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, Peo_add_b );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					draw_Script(0, 0, Peo_add_b-Peo_panel->new_b, 0, Peo_panel->person[Peo_panel->top].name, Peo_panel->w);
				}
				else {
					draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, Peo_add_b-Peo_panel->h, Peo_panel->w/2, Peo_add_b);
					draw_Script(0, 0, Peo_add_b-Peo_panel->h, 0, Peo_panel->person[Peo_panel->top].name, Peo_panel->w);
				}

				Peo_panel->visible=1;
				Peo_panel->animation=1;
				now_num=(int)((Peo_panel->new_b-Peo_panel->h)/(Peo_panel->h+Peo_panel->interval));//此处的b待会儿改掉
				spe_dis=Peo_panel->new_b-Peo_panel->h-now_num*(Peo_panel->h+Peo_panel->interval);
				b=Peo_add_b-Peo_panel->h-(Peo_panel->h+Peo_panel->interval);
					
				if(now_num!=0)
				{
  					for(i=1;i<=now_num;i++)
					{
						draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->h);
						draw_Script(0, 0, b, 0, Peo_panel->person[i+Peo_panel->top].name, Peo_panel->w);
						b-=Peo_panel->h+Peo_panel->interval;
					}
				}
				if(now_num!=0)
					b+=Peo_panel->h+Peo_panel->interval;
				else 
					b=Peo_quan;
				if(i==0)
					i=1;
				else
					i=now_num+1;

				if(spe_dis > 0) {//目的在于既能画出非第一条之外的部分条，又不重复画第一条
					if(spe_dis<=Peo_panel->h)
					{		
						ratio=spe_dis/Peo_panel->h;
						glEnable(GL_TEXTURE_2D);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
						glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
						glBegin(GL_QUADS);	
						glTexCoord2f(0.0, 0.0);   glVertex3f(-Peo_panel->w/2, 0, b-spe_dis);
						glTexCoord2f(1.0, 0.0  ); glVertex3f( Peo_panel->w/2, 0, b-spe_dis);
						glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, b		  );
						glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, b		  );
						glEnd();
						glDisable(GL_TEXTURE_2D);
						draw_Script(0, 0, b-spe_dis, 0, Peo_panel->person[i+Peo_panel->top].name, Peo_panel->w);
					}
					else
					{
						b-=spe_dis;
						draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->h);
						draw_Script(0, 0, b, 0, Peo_panel->person[i+Peo_panel->top].name, Peo_panel->w);
					}
				}
			}
			
			//下拉特效结束，复原各参数
			if(Peo_panel->new_b==Peo_panel->DIST)//下拉特效结束标示
			{
				Peo_panel->pre_visible =1;
				Peo_panel->animation =0;
				Peo_panel->animation2=0;
			}
		}
		 else
		{
			if(Peo_panel->pre_visible==1)
			{
				//printf("上提特效开始\n");
				//printf("Peo_panel->file_work_id的大小是%d\n",Peo_panel->file_work_id);
				//满足上拉特效开始条件，上拉特效程序开始
				//首先画第一条列表，每次执行此程序都会先将第一条列表单独先画出来
				if(Peo_panel->new_b < Peo_panel->h) {
					ratio=Peo_panel->new_b/Peo_panel->h;
					glEnable(GL_TEXTURE_2D);
					glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
					glBegin(GL_QUADS);
					glTexCoord2f(0.0, 0.0  ); glVertex3f(-Peo_panel->w/2, 0, Peo_add_b-Peo_panel->new_b);
					glTexCoord2f(1.0, 0.0  ); glVertex3f( Peo_panel->w/2, 0, Peo_add_b-Peo_panel->new_b);
					glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, Peo_add_b				   );
					glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, Peo_add_b				   );
					glEnd();
					glDisable(GL_TEXTURE_2D);
					draw_Script(0, 0, Peo_add_b-Peo_panel->new_b, 0, Peo_panel->person[Peo_panel->top].name, Peo_panel->w);
				}
				else {
					draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, Peo_add_b-Peo_panel->h, Peo_panel->w/2, Peo_add_b);
					draw_Script(0, 0, Peo_add_b-Peo_panel->h, 0, Peo_panel->person[Peo_panel->top].name, Peo_panel->w);
				}

				//上拉特效的相关参数设定
				Peo_panel->visible=0;
				Peo_panel->animation=1;
				now_num=(int)((Peo_panel->new_b-Peo_panel->h)/(Peo_panel->h+Peo_panel->interval));
				spe_dis=Peo_panel->new_b-Peo_panel->h-now_num*(Peo_panel->h+Peo_panel->interval);
				b=Peo_add_b-Peo_panel->h-(Peo_panel->h+Peo_panel->interval);
				
				//之下程序为在一次mainloop中画出特效中的一张画面
				if(now_num!=0)
				{
  					for(i=1;i<=now_num;i++)
					{		
						draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->h);
						draw_Script(0, 0, b, 0, Peo_panel->person[Peo_panel->top+i].name, Peo_panel->w);
						b-=Peo_panel->h+Peo_panel->interval;
					}
					//system("pause");
				}
				if(now_num!=0)
					b+=Peo_panel->h+Peo_panel->interval;
				else 
					b=Peo_quan;
				if(i=0)
					i=Peo_panel->top+1;
				else 
					i=Peo_panel->top+i;
				
				if(spe_dis > 0) {//目的在于既能画出非第一条之外的部分条，又不重复画第一条
					if(spe_dis<=Peo_panel->h)
						{		
							ratio=spe_dis/Peo_panel->h;
							glEnable(GL_TEXTURE_2D);
							glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
							glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
							glBegin(GL_QUADS);
							glTexCoord2f(0.0, 0.0  ); glVertex3f(-Peo_panel->w/2, 0, b-spe_dis);
							glTexCoord2f(1.0, 0.0  ); glVertex3f( Peo_panel->w/2, 0, b-spe_dis);
							glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, b		  );
							glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, b		  );
							glEnd();
							glDisable(GL_TEXTURE_2D);		
							draw_Script(0, 0, b-spe_dis, 0, Peo_panel->person[Peo_panel->top+i].name, Peo_panel->w);
						}
					else
					{
						b-=spe_dis;
						draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b, Peo_panel->w/2, b+Peo_panel->h);
						draw_Script(0, 0, b, 0, Peo_panel->person[Peo_panel->top+i].name, Peo_panel->w);
					}
				}
				Peo_change_new_b(Peo_panel);
			}
			
			//上拉特效结束，各参数还原
			if(Peo_panel->new_b==0)//上拉特效结束的标示
			{
				Peo_panel->pre_visible=0;
				Peo_panel->animation=0;
				Peo_panel->animation2=1;
			}
			glPopMatrix();
			return;
		}
	}
//之下程序是在没有执行上提，下拉特效时的上移下移特效
	if(Peo_panel->animation==0)
	{
		if((Peo_panel->file_on_id==Peo_NUM)||(Peo_panel->file_on_id==-1)) {
			if(Peo_panel->file_on_id==Peo_NUM) {
				b=Peo_add_b+Peo_panel->h;//贴上箭头的左上位置
				if(Peo_panel->file_work_id==Peo_NUM) {
					//此处贴图要变为工作时的上箭头
					glDisable(GL_LIGHTING);
					glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
					glEnable(GL_BLEND);	
					glDisable(GL_DEPTH_TEST);
					glColor4f(0.8, 0.6, 0.4, 0.3);
					draw_Texture(Peo_panel_texture[4], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
					glDisable(GL_BLEND);
					glEnable(GL_DEPTH_TEST);
					//要进行下移特效了,下移后要复原new_b1,更改Peo_panel->top 
					if((Peo_panel->top!=0)&&(Peo_panel->num>Peo_NUM)) {
						//printf("下移效果中Peo_panel->rec_work_id的大小是%d\n",Peo_panel->rec_work_id);
						Peo_panel->animation1=1;
						b=Peo_add_b;
						//new_b1=0;
						if(Peo_panel->new_b1!=(Peo_panel->h+Peo_panel->interval)) {//总的new_b1距离开始
							if(Peo_panel->new_b1 < Peo_panel->interval) {//这时上面的一个条还未下来
								b-=Peo_panel->new_b1;
							}//上面的条未下来的情形处理完毕
							else {//处理上面的条部分下来的情形
								ratio=(Peo_panel->new_b1-Peo_panel->interval)/Peo_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, b  );
								glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, b	);
								glTexCoord2f(1.0, 0.0); glVertex3f( Peo_panel->w/2, 0, b-(Peo_panel->new_b1-Peo_panel->interval));
								glTexCoord2f(0.0, 0.0); glVertex3f(-Peo_panel->w/2, 0, b-(Peo_panel->new_b1-Peo_panel->interval));
								glEnd();
								glDisable(GL_TEXTURE_2D);
								if(Peo_panel->new_b1 > Peo_panel->h/2)
									draw_Script(0.0, 0.0, b-(Peo_panel->new_b1-Peo_panel->interval), 0, Peo_panel->person[Peo_panel->top-1].name,Peo_panel->w+4);
								b-=Peo_panel->new_b1;
							}
							//printf("file_on_id是%d======================\n",Peo_panel->file_on_id);
							for(i=Peo_NUM-1;i>=1;i--){
								if(Peo_panel->rec_work_id==i){
									glDisable(GL_LIGHTING);
									glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
									glEnable(GL_BLEND);	
									glDisable(GL_DEPTH_TEST);
									glColor4f(0.0, 0.6, 0.0, 0.3);
									draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2-2, b-Peo_panel->h-2, Peo_panel->w/2+2, b+2);
									glDisable(GL_BLEND);
									glEnable(GL_DEPTH_TEST);
									draw_Script(0.0, 0.0, b-Peo_panel->h-2, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w+4);
								}
									
								else
								{	
									draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
									draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w);
								}	 
							
								b-=Peo_panel->h+Peo_panel->interval;
							}
							if(Peo_panel->new_b1<=Peo_panel->h){//处理上面的条部分下来，但下面的条还未全移去的情形
								ratio=1-(Peo_panel->h-Peo_panel->new_b1)/Peo_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, 1.0 ); glVertex3f(-Peo_panel->w/2, 0, b );
								glTexCoord2f(1.0, 1.0 ); glVertex3f( Peo_panel->w/2, 0, b );
								glTexCoord2f(1.0, ratio);glVertex3f( Peo_panel->w/2, 0, b-(Peo_panel->h-Peo_panel->new_b1));//注意此处的贴图坐标
								glTexCoord2f(0.0, ratio);glVertex3f(-Peo_panel->w/2, 0, b-(Peo_panel->h-Peo_panel->new_b1));//同上
								glEnd();
								glDisable(GL_TEXTURE_2D);
								if(Peo_panel->new_b1 < Peo_panel->h/2)
									draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w+4);
							}

							//上面条部分下来的情形处理结束
							if(Peo_panel->rec_work_id!=Peo_NONE) {
								Peo_draw_sublist(pos,Peo_R_num);
							}
							Peo_change_new_b1(2.0);
						}
					}//下移特效到此结束
					//开始进行new_b和Peo_panel->top的复原
					if(Peo_panel->new_b1==(Peo_panel->h+Peo_panel->interval)) {
						Peo_panel->new_b1=0;
						Peo_panel->top--;//复原完毕
						Peo_panel->animation1=0;
						if(Peo_panel->rec_work_id!=Peo_NONE)//这样就避免了没选中联系人的情况下，上移下移造成的Peo_panel->rec_work_id误改变而错误地画sub_list
							Peo_panel->rec_work_id--;
						Peo_panel->file_work_id=Peo_NUM+1;
					}
				}
				else{//待会儿这个贴图要改，是移动到上箭头处要贴的上箭头
					draw_Texture(Peo_panel_texture[4], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
				}//不论如何，移到上箭头处的所有情况处理完毕
			}
			  
			else {  
				b=0;
				if(Peo_panel->file_work_id==-1) {
					//此处贴图要变为工作时的下箭头
					glDisable(GL_LIGHTING);
					glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
					glEnable(GL_BLEND);	
					glDisable(GL_DEPTH_TEST);
					glColor4f(0.8, 0.6, 0.4, 0.3);
					draw_Texture(Peo_panel_texture[5], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
					glDisable(GL_BLEND);
					glEnable(GL_DEPTH_TEST);
					//要进行上移特效了,上移后要复原new_b1,更改Peo_panel->top和animation1
					if((Peo_panel->top < (Peo_panel->num-Peo_NUM))&&(Peo_panel->num>Peo_NUM)) {
						//printf("上移中Peo_panel->rec_work_id的大小是%d\n",Peo_panel->rec_work_id);
						Peo_panel->animation1=1;
						b=Peo_add_b;
						if(Peo_panel->new_b1!=(Peo_panel->h+Peo_panel->interval)) {//总的new_b1距离开始
							if(Peo_panel->new_b1 > Peo_panel->h) { //这时上面的一个条已完全上去
								b=b-(Peo_panel->h+Peo_panel->interval)+Peo_panel->new_b1;
							}
							else {//处理上面的条部分上去的情形							
								ratio=(Peo_panel->h-Peo_panel->new_b1)/Peo_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, b  );
								glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, b);
								glTexCoord2f(1.0, 0.0); glVertex3f( Peo_panel->w/2, 0, b-(Peo_panel->h-Peo_panel->new_b1));
								glTexCoord2f(0.0, 0.0); glVertex3f(-Peo_panel->w/2, 0, b-(Peo_panel->h-Peo_panel->new_b1));
								glEnd();
								glDisable(GL_TEXTURE_2D);
								b=b-(Peo_panel->h+Peo_panel->interval)+Peo_panel->new_b1;
								if(Peo_panel->new_b1 < Peo_panel->h/2)
									draw_Script(0.0, 0.0, b-(Peo_panel->h-Peo_panel->new_b1), 0, Peo_panel->person[Peo_panel->top].name, Peo_panel->w);
							}
							for(i=Peo_NUM-2;i>=0;i--) {
								if(Peo_panel->rec_work_id==i){							
									glDisable(GL_LIGHTING);
									glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
									glEnable(GL_BLEND);	
									glDisable(GL_DEPTH_TEST);
									glColor4f(0.0, 0.6, 0.0, 0.3);
									draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2-2, b-Peo_panel->h-2, Peo_panel->w/2+2, b+2);
									glDisable(GL_BLEND);
									glEnable(GL_DEPTH_TEST);
									draw_Script(0.0, 0.0, b-Peo_panel->h-2, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w+4);
								}
								else
								{
									draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
									draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w);
								}	 
							
								b-=Peo_panel->h+Peo_panel->interval;
							}
							if(Peo_panel->new_b1 > Peo_panel->interval){//处理下面的条已有部分上来的状况
								ratio=1-(Peo_panel->new_b1-Peo_panel->interval)/Peo_panel->h;
								glEnable(GL_TEXTURE_2D);
								glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
								glBindTexture(GL_TEXTURE_2D, Peo_panel_texture[0]);
								glBegin(GL_QUADS);
								glTexCoord2f(0.0, 1.0); glVertex3f(-Peo_panel->w/2, 0, b);
								glTexCoord2f(1.0, 1.0); glVertex3f( Peo_panel->w/2, 0, b);
								glTexCoord2f(1.0, ratio); glVertex3f( Peo_panel->w/2, 0, b-(Peo_panel->new_b1-Peo_panel->interval));//注意此处的贴图坐标
								glTexCoord2f(0.0, ratio); glVertex3f(-Peo_panel->w/2, 0, b-(Peo_panel->new_b1-Peo_panel->interval));//同上
								glEnd();
								glDisable(GL_TEXTURE_2D);
								draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w);
							}
							if(Peo_panel->rec_work_id!=Peo_NONE) {
								Peo_draw_sublist(pos,Peo_R_num);
							}
							Peo_change_new_b1(2.0);
						}
					}//上移特效到此结束
					//开始进行new_b和Peo_panel->top的复原
					if(Peo_panel->new_b1==(Peo_panel->h+Peo_panel->interval)) {
						Peo_panel->new_b1=0;
						Peo_panel->top++;//复原完毕
						Peo_panel->animation1=0;
						if(Peo_panel->rec_work_id!=Peo_NONE)
							Peo_panel->rec_work_id++;
						Peo_panel->file_work_id=Peo_NUM+1;//只是权衡之计，总之为了避免连续两次下降和上升，始终使Peo_panel->file_work_id在每次移动后，都不等于Peo_NUM或-1即可
					}
				}
				else{//待会儿这个贴图要改，是移动到下箭头处要贴的下箭头
					draw_Texture(Peo_panel_texture[5], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
				}//不论如何，移到上箭头处的所有情况处理完毕				
			}
		}
	}

	//复原b参数，方便之下不存在滚轮效果时的画图,Peo_R_num就是Peo_panel->num大于Peo_NUM或小于两种情况下时，在全画状态下要画的列表条数
	b=Peo_add_b+Peo_panel->h+Peo_panel->interval;
	if((Peo_panel->animation1==0)&&(Peo_panel->animation==0)) {	
		//画0~Peo_R_num个表条
		for(i=Peo_R_num;i>=0;i--)
		{
			//printf("=================================================\n");
			//printf("Peo_panel->file_work_id的大小是%d\n",Peo_panel->file_work_id);
			//printf("Peo_panel->rec_work_id的大小是%d\n",Peo_panel->rec_work_id);
			b-=Peo_panel->h+Peo_panel->interval;
			if((Peo_panel->file_on_id==i) || (Peo_panel->file_work_id==i) || (Peo_panel->rec_work_id==i))
			{
				if((Peo_panel->file_work_id==i) || (Peo_panel->rec_work_id==i))
				{
					glDisable(GL_LIGHTING);
					glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);
					glEnable(GL_BLEND);	
					glDisable(GL_DEPTH_TEST);
					glColor4f(0.8, 0.6, 0.4, 0.3);
					draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2-2, b-Peo_panel->h-2, Peo_panel->w/2+2, b+2);
					glDisable(GL_BLEND);
					glEnable(GL_DEPTH_TEST);
					draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w+4);
				}
				else
				{
					draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2-2, b-Peo_panel->h-2, Peo_panel->w/2+2, b+2);
					draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w+2);
				}
			}
			else
			{
				draw_Texture(Peo_panel_texture[0], -Peo_panel->w/2, b-Peo_panel->h, Peo_panel->w/2, b);
				draw_Script(0.0, 0.0, b-Peo_panel->h, 0, Peo_panel->person[Peo_R_num+Peo_panel->top-i].name,Peo_panel->w);
			} 
			if(Peo_panel->rec_work_id!=Peo_NONE) {
				Peo_draw_sublist(pos,Peo_R_num);
			}
		}
	}
	glPopMatrix();
	//pos[2]-=150;
}

void draw_Addresslist(double pos[]) {
	Peo_draw_Panel(pos);
	return ;
}
