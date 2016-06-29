#include "cdbox.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <AR/ar.h>

char path_cd[20]="Data/music/music0/"; //17
char path_of_cd[256];

extern void get_buff(char *buf,int n,FILE *fp);

CDBOX *init_Cdbox(CDBOX *cdbox,char *name) {
	FILE *fp;
	char buf[256];
	int  i;
	arMalloc(cdbox,CDBOX,1);
	printf("Opening cdbox file %s\n",name);

	if ((fp=fopen(name,"r"))==NULL) {
		printf("cannot open the file %s\n",name);
		exit(1);
	}
	get_buff(buf,256,fp);
	strcpy(cdbox->name,buf);
	get_buff(buf,256,fp);

	if (sscanf(buf,"%d",&cdbox->id)!=1) {
		fclose(fp);
		printf("error while reading file 1%s\n",name);
		exit(1);
	}

	get_buff(buf,256,fp);
	if (sscanf(buf,"%d%d",&cdbox->sizeX,&cdbox->sizeY)!=2) {
		fclose(fp);
		printf("error while reading file 2%s\n",name);
		exit(1);
	}
	
	cdbox->numofcds=cdbox->sizeX*cdbox->sizeY;
	cdbox->visible=VISIBLE;
	cdbox->activecdid=-1;

	arMalloc(cdbox->cd,CD,cdbox->numofcds);

	for (i=0;i<cdbox->numofcds;i++){
		get_buff(buf,256,fp);
		strcpy(cdbox->cd[i].name,buf);
		get_buff(buf,256,fp);
		if(sscanf(buf,"%d",&cdbox->cd[i].id)!=1) {
			fclose(fp);
			printf("error while reading file 3%s\n",name);
			exit(1);
		}
		path_cd[16]=(char)('0'+i);
		strcpy(cdbox->cd[i].path,path_cd);
		cdbox->cd[i].state=INACTIVE;
		cdbox->cd[i].visible=VISIBLE;
	}
	printf("donecdbox\n");
	return cdbox;
}

