#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include "delay.h"
#include "stdbool.h"

#define MOTORA1 PDout(0)	// 
#define MOTORA2 PDout(1)	// 	 
#define MOTORB1 PDout(14)	// 
#define MOTORB2 PDout(15)	// 	 
#define FAN1 PDout(4)	
#define FAN2 PDout(5)

void Curtain_Init(void);
void FAN_Init(void);
void Fan_control(bool Fan);
void Curtain_control(bool curtain);

#endif
