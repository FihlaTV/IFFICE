#include "phone.h"
#include "mqoloader.h"
#include "myvim.h"
#include "action.h"
#include "message.h"
#include "ait.h"
extern "C" {
#include "bmploader.h"
}

#ifndef __APPLE__
	#include <GL/gl.h>
	#include <GL/glut.h>
#else
	#include <OpenGL/gl.h>
	#include <GLUT/glut.h>
#endif

#include <AR/ar.h>

#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include	 <math.h>		
#include	 <string.h>
#include	 <pthread.h>
#include     <unistd.h>     /*Unix标准函数定义*/
#include     <sys/types.h>  /**/
#include     <sys/stat.h>   /**/
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX终端控制定义*/
#include     <errno.h>      /*错误号定义*/

#define		PI	3.14159

bool		show_phone_panel=false;		//电话面板是否绘制
MQO_MODEL	phone_panel;
char		*phone_panel_file="Data/number.mqo";
int			phone_count=0;
double		angle[]={0.000000,1.000000,0.173648,0.984808,0.342020,0.939693,0.500000,0.866026,0.642787,0.766045,0.766044,0.642788,0.866025,0.500001,0.939692,0.342021,0.984808,0.173649,1.000000,0.000001,0.984808,-0.173647,0.939693,-0.342019,0.866026,-0.499998,0.766046,-0.642786,0.642789,-0.766043,0.500002,-0.866024,0.342022,-0.939692,0.173651,-0.984807,0.000003,-1.000000,-0.173645,-0.984808,-0.342017,-0.939694,-0.499997,-0.866027,-0.642785,-0.766047,-0.766042,-0.642790,-0.866024,-0.500003,-0.939691,-0.342024,-0.984807,-0.173652,-1.000000,-0.000004,-0.984808,0.173644,-0.939694,0.342016,-0.866028,0.499996,-0.766047,0.642784,-0.642791,0.766041,-0.500004,0.866023,-0.342025,0.939691,-0.173653,0.984807};
double		phone_x[10]={0.0,-20.0,0.0,20.0,-20.0,0.0,20.0,-20.0,0.0,20.0};
double		phone_z[10]={8.0,68.0,68.0,68.0,48.0,48.0,48.0,28.0,28.0,28.0};
int			now_number=-1;

int			phone_state=0;			//1:有来电 
int			clcc_state=0;			//来电显示
char		number_buffer[256];
char		number_len=0;
char		dev[]="/dev/ttyS1";
int			phone_port;

int			phone_new_call;
Mix_Chunk	*music_new_message;
char		music_new_message_file[]="Data/music_/new_message.wav";
int			phone_tmp_i=0;

//***********************************
/***@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void*/

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
	    B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300,
	    38400,  19200,  9600, 4800, 2400, 1200,  300, };
void set_speed(int fd, int speed)
{
  int   i;
  int   status;
  struct termios   Opt;
  tcgetattr(fd, &Opt);
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
   {
   	if  (speed == name_arr[i])
   	{
   	    tcflush(fd, TCIOFLUSH);
    	cfsetispeed(&Opt, speed_arr[i]);
    	cfsetospeed(&Opt, speed_arr[i]);
    	status = tcsetattr(fd, TCSANOW, &Opt);
    	if  (status != 0)
            perror("tcsetattr fd1");
     	return;
     	}
   tcflush(fd,TCIOFLUSH);
   }
}
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄*
*@param  databits 类型  int 数据位   取值 为 7 或者8*
*@param  stopbits 类型  int 停止位   取值为 1 或者2*
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if  ( tcgetattr(fd,&options)  !=  0){
		perror("SetupSerial 1");
		return(false);
	}
  options.c_cflag &= ~CSIZE;
  switch (databits) /*设置数据位数*/
  {
  	case 7:
  		options.c_cflag |= CS7;
  		break;
  	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return (false);
	}
	switch (parity)
  	{
  	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);  /* 设置为奇效验*/
		options.c_iflag |= INPCK;             /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;     /* Enable parity */
		options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return (false);
		}
  /* 设置停止位*/
	 switch (stopbits){
		case 1:
			options.c_cflag &= ~CSTOPB;
			break;
		case 2:
			options.c_cflag |= CSTOPB;
			break;
		default:
			fprintf(stderr,"Unsupported stop bits\n");
			return (false);
	}
	/* Set input parity option */
	if (parity != 'n')
  		options.c_iflag |= INPCK;
	options.c_cc[VTIME] = 150; // 15 seconds
	options.c_cc[VMIN] = 0;

	tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
	if (tcsetattr(fd,TCSANOW,&options) != 0){
	perror("SetupSerial 3");
		return (false);
	}
	return (true);
 }
/**
*@breif 打开串口
*/
int OpenDev(char *Dev)
{
	int	fd = open( Dev, O_RDWR );         //| O_NOCTTY | O_NDELAY
	if (-1 == fd)
		{ /*设置数据位数*/
			perror("Can't Open Serial Port");
			return -1;
		}
	else
	return fd;

}
//***********************************

void *phone_receive(void *){
	int nread,i,j;
	char buf[256];
	while (1){
		while ((nread=read(phone_port,buf,256))>0){
			buf[nread]='\0';
		//	printf("read:%d\n",nread);
			//receive a call 未加对方挂断检测！
			if (strstr(buf,"RING")!=NULL && phone_state==0) {
				printf("calling you!\n");
				phone_state=1;
			//	phone_send(PO_CALLER_ID);
			//	receive_Call(1);
			}

			if ((phone_state==3 && strstr(buf,"OK")!=NULL && strchr(buf,'"')==NULL) || strstr(buf,"NO CARRIER")!=NULL) {
				printf("talking ok\n");
				phone_tmp_i=0;
				number_len=0;
				number_buffer[0]='\0';
				phone_state=0;
				receive_Call(0);
			}

			if ((strstr(buf,"CLCC")!=NULL && phone_state==1) || clcc_state>0) {
				phone_state=2;
				if (clcc_state==0) clcc_state=1;
				char *pp=&buf[0];
			//	printf("len:%d\n",strlen(buf));
				
				if (clcc_state==1) {
					while (*pp!='"' && *pp!='\0') pp++;
					if (*pp=='"') {clcc_state=2;pp++;phone_tmp_i=0;}
				}
				if (pp[0]!='\0' && clcc_state==2) {
					//pp++;
					j=0;
					while (pp[j]!='"' && pp[j]!='\0' && phone_tmp_i<11) {
						if (pp[j]>='0' && pp[j]<='9'){
							number_buffer[phone_tmp_i]=pp[j];
							phone_tmp_i++;
						}
						j++;
					}
					if (phone_tmp_i==11 || pp[j]=='"') {
						clcc_state=0;phone_state=3;
						number_len=11;
						number_buffer[11]='\0';
						receive_Call(1);
						//printf("number:%s\n",number_buffer);
					}
				}
			}
			if (phone_state==1) {phone_send(PO_CALLER_ID);}
			
			printf("%s\n",buf);
		}
	}
	printf("over\n");
}

//打开串口，创建子线程
void open_Port() {
	int len;
	pthread_t tmp_id;
	phone_port=OpenDev(dev);
	if (phone_port>0) set_speed(phone_port,115200); //设置波特率
	else {
		printf("cannot open serial port!\n");
		return ;
	}
	if ((set_Parity(phone_port,8,1,'N'))==false){
		printf("set parity error!\n");
		exit(1);
	}
	len=pthread_create(&tmp_id,NULL,phone_receive,NULL);
	if (len!=0) {
		printf("create phone pthread error!\n");
		exit(1);
	}
	else printf("create phone pthread succeed!\n");

	//echo off
	phone_send(PO_ECHO_OFF);
	//phone_send(PO_HANG_UP);
	char buff[]="atd13484585026;\r";
//	printf("%d\n",sizeof(buff));
//	write(phone_port,buff,sizeof(buff));
	return ;
}

//打开串口，载入电话面板模型
void init_Phone() {
	double tmp_pos[3]={100,50,120};
	music_Init();
	//未调用mqoInit() 故在主函数里的调用位置要注意一下
	if ((phone_panel=mqoCreateModel(phone_panel_file,1.0))==NULL) {
		printf("cannot find the model phone_panel");
		exit(1);
	}
	phone_new_call=register_Message(tmp_pos,60,30,"Data/calling.bmp");
	music_new_message=load_sound(music_new_message_file);
	memset(number_buffer,'\0',sizeof(number_buffer));
	open_Port(); //打开串口，创建子线程

	return ;
}

void select_Number(char num) {
	if (num==0x8 && number_len!=0) {number_len--;number_buffer[number_len]='\0';}
	else {
		if (num!=0x8){
			number_buffer[number_len++]=num;
			now_number=num-'0';
			phone_count=0;
		}
	}
	return;
}

void phone_send(char *order) {
	write(phone_port,order,strlen(order));
	return ;
}

void calling() {
	char tmp_buffer[256];
	memset(tmp_buffer,'\0',sizeof(tmp_buffer));
	strcat(tmp_buffer,"ATD");
	strcat(tmp_buffer,number_buffer);
	tmp_buffer[14]=';';
	tmp_buffer[15]='\r';
	tmp_buffer[16]='\0';
	printf("%d\n",strlen(tmp_buffer));
	printf("%s\n",tmp_buffer);
	write(phone_port,tmp_buffer,strlen(tmp_buffer));
	memset(number_buffer,'\0',sizeof(number_buffer));
	number_len=0;
	return ;
}

void action_Call(void){
	if (phone_state==3) {
		action_Setplay(3,0,-1,1,1);
	}
	return ;
}

void receive_Call(int state) {
	if (state) {
		action_Setplay(2,2,6,1,1);
		action_Setplay(3,0,-1,1,1);
		action_Setcallback(3,action_Call);
		call_Message(phone_new_call);
		Mix_PlayChannel(-1,music_new_message,0);
	}
	else {
		action_Setplay(2,2,-1,6,-1);
		over_Message(phone_new_call);
	}
	return ;
}

void draw_Phone() {
	int aa,bb;
	if (show_phone_panel){
		glEnable(GL_LIGHTING);
		glRotatef(90.0,1.0,0.0,0.0);
		mqoCallModel(phone_panel);

		glDisable(GL_LIGHTING);  //调试时注意这里把灯光关掉了
		glDisable(GL_DEPTH_TEST);
		glRotatef(-90.0,1.0,0.0,0.0);

		if (now_number!=-1){
			phone_count++;
			glPointSize(3.0);
			glColor3f(1.0,0.0,0.0);
			glBegin(GL_POINTS);
				aa=(phone_count*2+1)%72,bb=(phone_count*2)%72;
				glVertex3f(angle[aa]*7+phone_x[now_number],0.0,angle[bb]*7+phone_z[now_number]);
				aa=((phone_count+12)*2+1)%72;bb=((phone_count+12)*2)%72;
				glVertex3f(angle[aa]*7+phone_x[now_number],0.0,angle[bb]*7+phone_z[now_number]);
				aa=((phone_count+24)*2+1)%72,bb=((phone_count+24)*2)%72;
				glVertex3f(angle[aa]*7+phone_x[now_number],0.0,angle[bb]*7+phone_z[now_number]);
			glEnd();
			if (phone_count==72) {now_number=-1;phone_count=0;}
			glPointSize(1.0); //恢复一下
		}
		glEnable(GL_DEPTH_TEST);

		glTranslatef(-25,0.0,90);
		glColor3f(0.2,0.2,0.2);
		glBegin(GL_QUADS);
			glVertex3f(0.0,0.0,0.0);
			glVertex3f(60,0.0,0.0);
			glVertex3f(60,0.0,10);
			glVertex3f(0.0,0.0,10);
		glEnd();
		if (number_len!=0){
			draw_String(number_buffer,60);
		}
	}
		return ;
}

void exit_Phone(){
	if (phone_port>0)
		close(phone_port);
	return ;
}

