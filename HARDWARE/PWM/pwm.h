#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "stdbool.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define CLOSE 15

void TIM3_PWM_Init(u32 arr,u32 psc);
void Meng(bool Door);
void Meng1(bool Door);
void Meng2(bool Door);



#endif
