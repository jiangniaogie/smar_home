#ifndef __ESP8266_H
#define __ESP8266_H
#include "string.h"
#include "stdbool.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "pwm.h"
#include "usart3.h"
#include "ws2812.h"
#include "dht11.h"
#include "beep.h"
#include "mq2.h"
#include "oled.h"
#include "motor.h"

void ESP8266_Init(void);
void SendMessage(int i);
void ProcessMessage(void);



#endif
