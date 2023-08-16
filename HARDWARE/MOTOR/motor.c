#include "motor.h"
uint8_t Curtain_Flag = 0;//0Ϊ�� 1Ϊ��

void FAN_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;//
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIO
	FAN1 = 0;
	FAN2 = 0;
}

//�����ĳ�ʼ��
void Curtain_Init(void)
{
    //�˲������ֶ��޸�IO������

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��PORTDʱ��

    //GPIOD14,15pwm�ڳ�ʼ��
    //GPIOD0,D1��ʼ������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
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
