#ifndef _PHONE_H
#define _PHONE_H


//PO means Phone Order
#define		PO_ECHO_OFF		"ATE0\r"			//echo off
#define		PO_HANG_UP		"AT+CVHU=0\r"		//change ATH to hang up
#define		PO_HOLD_ON		"AT+CVHU=1\r"		//change ATH to hold on(default)
#define		PO_INTERRUPT	"ATH\r"				//hang up or hold on
#define		PO_ANSWER		"ATA\r"				//answer call
#define		PO_CALLER_ID	"AT+CLCC\r"			//echo caller ID

void init_Phone(void);
void select_Number(char num);
void calling(void);
void receive_Call(int state);
void phone_send(char *order);
void draw_Phone(void);
void exit_Phone(void);

#endif
