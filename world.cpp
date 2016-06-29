#include "world.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <AR/ar.h>
#include <AR/matrix.h>

int pre_world_id=-1;

char *get_buff(char *buf,int n,FILE *fp){
	char *ret;
	for (;;){
		ret=fgets(buf,n,fp);
		if (ret==NULL) return NULL;
		if (buf[0]!='\n' && buf[0]!='#') return ret;
	}
}

void error_find(FILE *fp,BASE_POINT *base_info) {
	printf("error while loading the world\n");
	fclose(fp);
	free(base_info);
	return;
}
	
WORLD *build_the_world(const char *filename){
	FILE		*fp;
	WORLD		*world;
	BASE_POINT	*base_info;
	char		buf[256],buf1[256];
	int			num;
	int			i,j;
	
	if ((fp=fopen(filename,"r"))==NULL) return NULL;

	get_buff(buf,256,fp);
	if (sscanf(buf,"%d",&num)!=1){fclose(fp);return NULL;}

	arMalloc(base_info,BASE_POINT,num);

	for (i=0;i<num;i++){
		get_buff(buf,256,fp);
		if (sscanf(buf,"%s",buf1)!=1){
			error_find(fp,base_info);
			return NULL;
		}
		if ((base_info[i].patt_id=arLoadPatt(buf1))<0) {
			error_find(fp,base_info);
			return NULL;
		}

		get_buff(buf,256,fp);
		if (sscanf(buf,"%lf",&base_info[i].width)!=1) {
			error_find(fp,base_info);
			return NULL;
		}
		get_buff(buf,256,fp);
		if (sscanf(buf,"%lf %lf",&base_info[i].center[0],&base_info[i].center[1])!=2) {
			error_find(fp,base_info);
			return NULL;
		}

		for (j=0;j<3;j++){
			get_buff(buf,256,fp);
			if (sscanf(buf,"%lf %lf %lf %lf",&base_info[i].extra_trans[j][0],&base_info[i].extra_trans[j][1],&base_info[i].extra_trans[j][2],&base_info[i].extra_trans[j][3])!=4) {
				error_find(fp,base_info);
				return NULL;
			}
		}
	}
	fclose(fp);
	arMalloc(world,WORLD,1);
	if (world==NULL) {free(base_info);return NULL;}
	world->base_point = base_info;
	world->base_num   = num;
	return world;
}

int get_the_world(ARMarkerInfo *marker_info,int marker_num,WORLD *world){
	int		i,j,k,t,p;
	k=-1;
	for (i=0;i<world->base_num;i++){
		t=-1;
		for (j=0;j<marker_num;j++){
			if (world->base_point[i].patt_id==marker_info[j].id){
				if (t==-1) t=j;
				else if (marker_info[t].cf<marker_info[j].cf) t=j;
			}
		}
		if (t!=-1) if (k==-1) {k=t;p=i;}
					else if (marker_info[t].cf<marker_info[k].cf) {k=t;p=i;}
	}
	if (k==-1) return -1;

	//printf("%lf\n",marker_info[k].cf);
	
	arGetTransMatCont(&marker_info[k],world->base_point[p].trans,world->base_point[p].center,world->base_point[p].width,world->base_point[p].trans);
	//arGetTransMat(&marker_info[k],world->base_point[p].center,world->base_point[p].width,world->base_point[p].trans);

	arUtilMatMul(world->base_point[p].trans,world->base_point[p].extra_trans,world->trans);
	
	arUtilMatInv(world->trans,world->cam_to_world);
/*
	for (i=0;i<3;i++){
		for (j=0;j<4;j++)
			printf("%lf ",world->trans[i][j]);
	printf("\n");
	}
	printf("\n");
*/
	return 0;
}

//优先
int get_the_world2(ARMarkerInfo *marker_info,int marker_num,WORLD *world){
	int		i,j,k,t,p;
	bool    world_flag;
	k=-1;
	
	world_flag=true;
	if (pre_world_id!=-1){
		for (i=0;i<marker_num;i++)
			if (world->base_point[pre_world_id].patt_id==marker_info[i].id && marker_info[i].cf>0.75){
				k=i;
				world_flag=false;
				break;
			}
	}
	if (world_flag) {
		p=-1;
	for (i=0;i<world->base_num;i++){
		t=-1;
		for (j=0;j<marker_num;j++){
			if (world->base_point[i].patt_id==marker_info[j].id){
				if (t==-1) t=j;
				else if (marker_info[t].cf<marker_info[j].cf) t=j;
			}
		}
		if (t!=-1) if (k==-1) {k=t;p=i;}
					else if (marker_info[t].cf<marker_info[k].cf) {k=t;p=i;}
	}
	pre_world_id=p;
	}

	if (k==-1) return -1;
	

	printf("%d_%lf\n",world->base_point[pre_world_id].patt_id,marker_info[k].cf);
	
	arGetTransMat(&marker_info[k],world->base_point[pre_world_id].center,world->base_point[pre_world_id].width,world->base_point[pre_world_id].trans);

	arUtilMatMul(world->base_point[pre_world_id].trans,world->base_point[pre_world_id].extra_trans,world->trans);

	return 0;
}

//算数平均
int get_the_world3(ARMarkerInfo *marker_info,int marker_num,WORLD *world){
	int		i,j,k,t,p;
	double  world_tmp[3][4];
	int		world_tmp_num;
	k=-1;
	memset(world_tmp,0,sizeof(world_tmp));
	for (i=0;i<3;i++)
		for (j=0;j<4;j++)
			world->trans[i][j]=0;

	world_tmp_num=0;
	for (i=0;i<world->base_num;i++){
		t=-1;
		for (j=0;j<marker_num;j++){
			if (world->base_point[i].patt_id==marker_info[j].id && marker_info[j].cf>0.6){
				if (t==-1) t=j;
				else if (marker_info[t].cf<marker_info[j].cf) t=j;
			}
		}
		if (t!=-1) {
			printf("%d_%lf\n",world->base_point[i].patt_id,marker_info[t].cf);
			int ii,jj;
			world_tmp_num++;
			arGetTransMat(&marker_info[t],world->base_point[i].center,world->base_point[i].width,world->base_point[i].trans);
			arUtilMatMul(world->base_point[i].trans,world->base_point[i].extra_trans,world_tmp);
			for (ii=0;ii<3;ii++)
				for (jj=0;jj<4;jj++)
					world->trans[ii][jj]+=world_tmp[ii][jj];
		}
	}
	if (world_tmp_num==0) return -1;
	for (i=0;i<3;i++)
		for (j=0;j<4;j++)
			world->trans[i][j]=world->trans[i][j]/world_tmp_num;
	//printf("%d_%lf\n",world->base_point[p].patt_id,marker_info[k].cf);

	return 0;
}

int get_the_world4(ARMarkerInfo *marker_info,int marker_num,WORLD *world){
	int		i,j,k,t,p;
	k=-1;
	for (i=0;i<world->base_num;i++){
		t=-1;
		for (j=0;j<marker_num;j++){
			if (world->base_point[i].patt_id==marker_info[j].id){
				if (t==-1) t=j;
				else if (marker_info[t].cf<marker_info[j].cf) t=j;
			}
		}
		if (t!=-1) if (k==-1) {k=t;p=i;}
					else if (marker_info[t].cf<marker_info[k].cf) {k=t;p=i;}
	}
	if (k==-1) return -1;
	printf("%d_%lf\n",world->base_point[p].patt_id,marker_info[k].cf);
	arGetTransMat(&marker_info[k],world->base_point[p].center,world->base_point[p].width,world->base_point[p].trans);

	for (i=0;i<3;i++)
		for (j=0;j<4;j++)
			world->trans[i][j]=world->base_point[p].trans[i][j];
	for (i=0;i<3;i++)
		world->trans[i][3]+=world->base_point[p].extra_trans[i][3];
	//arUtilMatMul(world->base_point[p].trans,world->base_point[p].extra_trans,world->trans);

	return 0;
}

int get_the_world5(WORLD *world){
	world->trans[0][0]=0.997088;
	world->trans[0][1]=0.074920;
	world->trans[0][2]=-0.014225;
	world->trans[0][3]=-15.978866;
	world->trans[1][0]=0.019736;
	world->trans[1][1]=-0.433711;
	world->trans[1][2]=-0.900836;
	world->trans[1][3]=79.623588;
	world->trans[2][0]=-0.073660;
	world->trans[2][1]=0.897932;
	world->trans[2][2]=-0.433927;
	world->trans[2][3]=556.131600;
	return 0;
}



void destroy_the_world(WORLD *world){
	int i;
	for (i=0;i<world->base_num;i++){
		if (arFreePatt(world->base_point[i].patt_id)!=1)	return;
	}
	free(world->base_point);
	free(world);
	world=NULL;
	return ;
}
