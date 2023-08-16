#include "sys.h"
#include "delay.h"
#include "esp8266.h"
#include "mq2.h"
#include "usart3.h"
#include "oled.h"
#include "motor.h"

int i = 1;
extern u16 temp;//这是灯带模式的状态
extern uint16_t PowerMode;//模式
extern bool Door1, Door2;
extern bool Curtain;//保持窗帘一致
extern bool KongT;
extern bool BingX;
extern bool Fan;
extern bool Door;
extern bool Bedroom;
extern bool Guest;

int main(void)
{
	
	delay_init(168);
	OLED_Init();//OELD屏
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);//wifi模块接收指令
    usart3_init(115200);//语音模块
    LED_Init();//冰箱空调
    Curtain_Init();//窗帘 2个电机
    FAN_Init();//风扇 一个电机
    BEEP_Init();//蜂鸣器报警
    DHT11_Init();//温湿度
    TIM3_PWM_Init(200 - 1, 8400 - 1);//3扇门
    WS2812b_Configuration();//灯带
    MQ2_Init();//可燃气体检测
    ESP8266_Init();//wifi模块
    
    TIM_SetCompare1(TIM3, CLOSE);//90°关门状态
    TIM_SetCompare2(TIM3, CLOSE);
    TIM_SetCompare3(TIM3, CLOSE);
	Curtain_control(0);

    while(1)
    {	
		ProcessMessage();
        U3_ProcessMessage();
        SendMessage(i++);
        if(i >= 1000)
        {
            i = 1;
        }
        delay_ms(10);
    }
}

