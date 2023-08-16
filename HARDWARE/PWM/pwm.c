#include "pwm.h"
#include "led.h"
#include "usart.h"
#include "stdbool.h"
extern bool Door,Door1,Door2;

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//����PWM�ĺ���ȫд��������

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//TIM3 PWM���ֳ�ʼ�� ���ţ�
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr, u32 psc)
{
    //�˲������ֶ��޸�IO������

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  	//TIM3ʱ��ʹ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 	//ʹ��PORTCʱ��

    GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3); //GPIOC6����Ϊ��ʱ��3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3); //GPIOC6����Ϊ��ʱ��3
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3); //GPIOC6����Ϊ��ʱ��3
//    GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM3); //GPIOC6����Ϊ��ʱ��3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 |GPIO_Pin_8;           //GPIOB9
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
    GPIO_Init(GPIOC, &GPIO_InitStructure);             //��ʼ��PB14

    TIM_TimeBaseStructure.TIM_Prescaler = psc; //��ʱ����Ƶ
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //���ϼ���ģʽ
    TIM_TimeBaseStructure.TIM_Period = arr; //�Զ���װ��ֵ
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //��ʼ����ʱ��3

    //��ʼ��TIM12 Channel1 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM1 4OC1
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);//ͨ��2
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);//ͨ��3
	
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM3��CCR1�ϵ�Ԥװ�ؼĴ���

    TIM_ARRPreloadConfig(TIM3, ENABLE); //ARPEʹ��

    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3
}

//�����ŵ�
void Meng(bool Door)
{
    if(Door == 0)
    {
        TIM_SetCompare1(TIM3, CLOSE);//
    }
    else
    {
        TIM_SetCompare1(TIM3, 5);//
    }
}

void Meng1(bool Door)
{
    if(Door == 0)
    {
        TIM_SetCompare2(TIM3, CLOSE);//
    }
    else
    {
        TIM_SetCompare2(TIM3, 25);//
    }
}

void Meng2(bool Door)
{
    if(Door == 0)
    {
        TIM_SetCompare3(TIM3, CLOSE);//
    }
    else
    {
        TIM_SetCompare3(TIM3, 25);//
    }
}


