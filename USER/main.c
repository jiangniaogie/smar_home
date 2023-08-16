#include "sys.h"
#include "delay.h"
#include "esp8266.h"
#include "mq2.h"
#include "usart3.h"
#include "oled.h"
#include "motor.h"

int i = 1;
extern u16 temp;//���ǵƴ�ģʽ��״̬
extern uint16_t PowerMode;//ģʽ
extern bool Door1, Door2;
extern bool Curtain;//���ִ���һ��
extern bool KongT;
extern bool BingX;
extern bool Fan;
extern bool Door;
extern bool Bedroom;
extern bool Guest;

int main(void)
{
	
	delay_init(168);
	OLED_Init();//OELD��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    uart_init(115200);//wifiģ�����ָ��
    usart3_init(115200);//����ģ��
    LED_Init();//����յ�
    Curtain_Init();//���� 2�����
    FAN_Init();//���� һ�����
    BEEP_Init();//����������
    DHT11_Init();//��ʪ��
    TIM3_PWM_Init(200 - 1, 8400 - 1);//3����
    WS2812b_Configuration();//�ƴ�
    MQ2_Init();//��ȼ������
    ESP8266_Init();//wifiģ��
    
    TIM_SetCompare1(TIM3, CLOSE);//90�����״̬
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

