#ifndef _MQ2_H
#define _MQ2_H
 
//C�ļ�����Ҫ��������ͷ�ļ�
#include "sys.h"
#include "delay.h"
#include "math.h"
#include "stdbool.h" 
 
#define SMOG_READ_TIMES	6				//��������������ȡ����,����ô���,Ȼ��ȡƽ��ֵ
 
//C�ļ��ж���ĺ���������
void MQ2_Init(void);
float Smog_Get_Vol(void);	//��ȡMQ7�������ĵ�ѹֵ
//float MQ7_PPM_Calibration(void);
float Smog_GetPPM(void);
bool Gas_analyze(void);
	
#endif

