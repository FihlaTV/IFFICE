#include "object.h"
#include "mqoloader.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AR/ar.h>
#include <AR/gsub.h>
#ifdef __APPLE__
#	include <GLUT/glut.h>
#else
#	include <GL/glut.h>
#endif

extern void get_buff(char *buf,int n,FILE *fp);

AR_OBJ *reg_object(AR_OBJ *AR_obj,char *filename,int &num) {
	FILE *fp;
	int	 i,j;
	char buf[256];
	int tmp_hide;

	printf("Loading objects from file:%s\n",filename);

	if ((fp=fopen(filename,"r"))==NULL){
		printf("cannot open the file:%s\n",filename);
		exit(1);
	}
	get_buff(buf,256,fp);
	if (sscanf(buf,"%d",&num)!=1){
		printf("error while loading object\n");
		fclose(fp);
		exit(1);
	}
	arMalloc(AR_obj,AR_OBJ,num);
	
	for (i=0;i<num;i++){
		get_buff(buf,256,fp);
		sscanf(buf,"%s",AR_obj[i].name);
		get_buff(buf,256,fp);
		if (sscanf(buf,"%d",&AR_obj[i].id)!=1){
			printf("error while loading object\n");
			fclose(fp);
			exit(1);
		}
		for (j=0;j<3;j++){
			get_buff(buf,256,fp);
			sscanf(buf,"%lf%lf%lf%lf",&AR_obj[i].mat_to_world3[j][0],&AR_obj[i].mat_to_world3[j][1],&AR_obj[i].mat_to_world3[j][2],&AR_obj[i].mat_to_world3[j][3]);
		}
		for (j=0;j<3;j++){
			AR_obj[i].mat_to_hand3[j][0]=AR_obj[i].mat_to_world3[j][0];
			AR_obj[i].mat_to_hand3[j][1]=AR_obj[i].mat_to_world3[j][1];
			AR_obj[i].mat_to_hand3[j][2]=AR_obj[i].mat_to_world3[j][2];
			AR_obj[i].mat_to_hand3[j][3]=AR_obj[i].mat_to_world3[j][3];

			AR_obj[i].mat_to_hide[j][0]=AR_obj[i].mat_to_world3[j][0];
			AR_obj[i].mat_to_hide[j][1]=AR_obj[i].mat_to_world3[j][1];
			AR_obj[i].mat_to_hide[j][2]=AR_obj[i].mat_to_world3[j][2];
			AR_obj[i].mat_to_hide[j][3]=AR_obj[i].mat_to_world3[j][3];
		}

		get_buff(buf,256,fp);
		sscanf(buf,"%d",&tmp_hide);
		AR_obj[i].mat_to_hide[2][3]-=tmp_hide;


		get_buff(buf,256,fp);
		sscanf(buf,"%d",&AR_obj[i].is_can_move);
		get_buff(buf,256,fp);
		sscanf(buf,"%d",&AR_obj[i].visible);
		get_buff(buf,256,fp);
		if (!AR_obj[i].visible){
			AR_obj[i].mat_to_world3[2][3]-=tmp_hide;
		}
		sscanf(buf,"%d",&AR_obj[i].active);
		get_buff(buf,256,fp);
		sscanf(buf,"%lf",&AR_obj[i].scale);
		get_buff(buf,256,fp);
		sscanf(buf,"%s",AR_obj[i].mqo_file);
		AR_obj[i].pos=0;
		AR_obj[i].Locked=0;
		AR_obj[i].state=0;

		if ((AR_obj[i].model=mqoCreateModel(AR_obj[i].mqo_file,AR_obj[i].scale))==NULL) {
			printf("cannot find the model %s\n",AR_obj[i].mqo_file);
			fclose(fp);
			exit(1);
		}
	}
	for (i=0;i<num;i++) {
		argConvGlpara(AR_obj[i].mat_to_world3,AR_obj[i].mat_to_world4);
		argConvGlpara(AR_obj[i].mat_to_hand3,AR_obj[i].mat_to_hand4);
	}
	fclose(fp);
	return AR_obj;
}

