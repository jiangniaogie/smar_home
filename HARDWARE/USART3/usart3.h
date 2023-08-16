#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
#include "delay.h"
#include "stdio.h"	 	
#include "stdarg.h"	 	 
#include "string.h"	
#include "stdbool.h"  
#include "timer.h"
#include "WS2812.h"
#include "led.h"
#include "pwm.h"
#include "oled.h"

#define USART3_MAX_RECV_LEN		50					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		50					//����ͻ����ֽ���
#define USART3_RX_EN 			1					//0,������;1,����.

extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;   						//��������״̬

void TIM7_Int_Init(u16 arr,u16 psc);
void usart3_init(u32 bound);				//����3��ʼ�� 
void u3_printf(char* fmt, ...);
void U3_ProcessMessage(void);
void Curtain_control(bool curtain);
void Meng(bool Door);
void Meng1(bool Door);
void Meng2(bool Door);

void Fan_control(bool Fan);


#endif













