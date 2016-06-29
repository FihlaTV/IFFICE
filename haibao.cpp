#include "haibao.h"
#include "mqoloader.h"

#include <AR/ar.h>
#ifndef __APPLE__
#	include <GL/glut.h>
#else
#	include <OpenGL/glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>

static HAIBAO_ACT *haibao_act;
static int		  q_num;

char *getbuff1(char *buf,int n,FILE *fp){
	char *ret;
	for(;;){
		ret=fgets(buf,n,fp);
		if (ret==NULL) return NULL;
		if (buf[0]!='\n' && buf[0]!='#') return ret;
	}
}

void error_find(FILE *fp){
	printf("error while loading haibao");
	fclose(fp);
	return ;
}

HAIBAO_ACT *load_sequence(char *haibao_conf,int *seq_num){
	FILE *fp;
	char buf[256];
	int  n_seq,i,k;
	char filename[256];
	int	 frame;
	double  scale;
	HAIBAO_ACT *haibao_tmp;
	
	fp=fopen(haibao_conf,"r");
	if (fp==NULL) {printf("error open the file %s\n",haibao_conf);return NULL;}

	getbuff1(buf,256,fp);
	if (sscanf(buf,"%d",&n_seq)!=1) {error_find(fp);return NULL;}
	
	arMalloc(haibao_tmp,HAIBAO_ACT,n_seq);
	if (haibao_tmp==NULL) return NULL;
	q_num=n_seq;

	for (i=0;i<n_seq;i++){
		printf("Model loading... %d/%d\n",i+1,n_seq);
		getbuff1(buf,256,fp);
		if (sscanf(buf,"%s",filename)!=1){error_find(fp);return NULL;}
		getbuff1(buf,256,fp);
		if (sscanf(buf,"%d",&frame)!=1){error_find(fp);return NULL;}
		getbuff1(buf,256,fp);
		if (sscanf(buf,"%lf",&scale)!=1){error_find(fp);return NULL;}
		getbuff1(buf,256,fp);
		if (sscanf(buf,"%lf %lf %lf %lf",&haibao_tmp[i].rotate[0],&haibao_tmp[i].rotate[1],&haibao_tmp[i].rotate[2],&haibao_tmp[i].rotate[3])!=4){
			error_find(fp);
			return NULL;
		}
				printf("test\n");
		getbuff1(buf,256,fp);
		if (sscanf(buf,"%lf %lf %lf",&haibao_tmp[i].translate[0],&haibao_tmp[i].translate[1],&haibao_tmp[i].translate[2])!=3){
			error_find(fp);
			return NULL;
		}
		haibao_tmp[i].action=mqoCreateSequence(filename,frame,scale);
		haibao_tmp[i].now_frame=0;
		haibao_tmp[i].is_stop=0;
	}
	*seq_num=n_seq;
	return haibao_tmp;
}	

int action_loading(char *haibao_conf){
	int seq_num;
	printf("%s\n",haibao_conf);
	haibao_act=load_sequence(haibao_conf,&seq_num);
	printf("haibao is here!\n");
	return seq_num;
}

void action_play(int action_id){
	HAIBAO_ACT *haibao_tmp=&haibao_act[action_id];
	glPushMatrix();
		glRotatef(haibao_tmp->rotate[0],haibao_tmp->rotate[1],haibao_tmp->rotate[2],haibao_tmp->rotate[3]);
		glTranslatef(haibao_tmp->translate[0],haibao_tmp->translate[1],haibao_tmp->translate[2]);
		mqoCallSequence(haibao_tmp->action,haibao_tmp->now_frame);
		haibao_tmp->now_frame++;
		if (haibao_tmp->now_frame==haibao_tmp->action.n_frame) haibao_tmp->now_frame=0;
	glPopMatrix();
	return;
}

void action_free(void){
	int i;
	if (q_num==0) return ;
	for (i=0;i<q_num;i++){
		mqoDeleteSequence(haibao_act[i].action);
	}
	free(haibao_act);
	haibao_act=NULL;
	q_num=0;
	return;
}
