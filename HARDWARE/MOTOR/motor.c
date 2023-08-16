#include "motor.h"
uint8_t Curtain_Flag = 0;//0为关 1为开

void FAN_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIO时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIO
	FAN1 = 0;
	FAN2 = 0;
}

//窗帘的初始化
void Curtain_Init(void)
{
    //此部分需手动修改IO口设置

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能PORTD时钟

    //GPIOD14,15pwm口初始化
    //GPIOD0,D1初始化设置
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
    GPIO_ResetBits(GPIOD, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15);
}

void Curtain_control(bool curtain)
{
    if(curtain == 0 && Curtain_Flag != 0)
    {
        MOTORA1 = 1;
        MOTORA2 = 0;
        MOTORB1 = 0;
        MOTORB2 = 1;
        delay_ms(400);
        MOTORA1 = 0;
        MOTORB2 = 0;
		Curtain_Flag = 0;
    }
    else if(curtain == 1 && Curtain_Flag != 1)
    {
        MOTORA1 = 0;
        MOTORA2 = 1;
        MOTORB1 = 1;
        MOTORB2 = 0;
        delay_ms(400);
        MOTORA2 = 0;
        MOTORB1 = 0;
		Curtain_Flag = 1;
    }
}

void Fan_control(bool Fan)
{
	if(Fan == 0)
	{
		FAN1 = 0;
		FAN2 = 0;
	}
	else if(Fan == 1)
	{
		FAN1 = 1;
		FAN2 = 0;
	}
}
