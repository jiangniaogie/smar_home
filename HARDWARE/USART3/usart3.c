#include "usart3.h"

u16 temp = 7;//传上云端的
extern bool Curtain;//保持窗帘一致
extern bool Fan;
extern uint16_t PowerMode;
extern bool Door;
extern bool Bedroom;
extern bool Guest;

//u8 t;//用来进switch控制灯的亮度的
//串口发送缓存区
__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 	//发送缓冲,最大USART3_MAX_SEND_LEN字节
#ifdef USART3_RX_EN   								//如果使能了接收   	  
//串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 				//接收缓冲,最大USART3_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于100ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过100ms,则认为不是1次连续数据.也就是超过100ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART3_RX_STA = 0;
void USART3_IRQHandler(void)
{
    u8 res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
    {

        res = USART_ReceiveData(USART3);
        if((USART3_RX_STA & (1 << 15)) == 0) //接收完的一批数据,还没有被处理,则不再接收其他数据
        {
            if(USART3_RX_STA < USART3_MAX_RECV_LEN)		//还可以接收数据
            {
                TIM_SetCounter(TIM7, 0); //计数器清空
                if(USART3_RX_STA == 0)
                    TIM_Cmd(TIM7, ENABLE);  //使能定时器7
                USART3_RX_BUF[USART3_RX_STA++] = res;		//记录接收到的值
            }
            else
            {
                USART3_RX_STA |= 1 << 15;					//强制标记接收完成
            }
        }
    }
}
#endif
//初始化IO 串口3
//bound:波特率
void usart3_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); //使能USART3时钟

    USART_DeInit(USART3);  //复位串口3

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3); //GPIOB11复用为USART3
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3); //GPIOB10复用为USART3

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOB11和GPIOB10初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化GPIOB11，和GPIOB10

    USART_InitStructure.USART_BaudRate = bound;//波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART3, &USART_InitStructure); //初始化串口3

    USART_Cmd(USART3, ENABLE);               //使能串口

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2 ; //抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
    TIM7_Int_Init(1000 - 1, 8400 - 1);		//100ms中断
    USART3_RX_STA = 0;		//清零
    TIM_Cmd(TIM7, DISABLE); //关闭定时器7
}

//串口3,printf 函数
//确保一次发送数据不超过USART3_MAX_SEND_LEN字节
void u3_printf(char *fmt, ...)
{
    u16 i, j;
    va_list ap;
    va_start(ap, fmt);
    vsprintf((char *)USART3_TX_BUF, fmt, ap);
    va_end(ap);
    i = strlen((const char *)USART3_TX_BUF); //此次发送数据的长度
    for(j = 0; j < i; j++) //循环发送数据
    {
        while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET); //等待上次传输完成
        USART_SendData(USART3, (uint8_t)USART3_TX_BUF[j]); 	 //发送数据到串口3
    }
}


void U3_ProcessMessage()
{
    if(USART3_RX_STA & 0x8000)
    {
        if(strstr((const char *)USART3_RX_BUF, "bzms"))
        {
            PowerMode = 0;
            RGB_WHITE(108);
            temp = 0;
            KT = 1;//开
            BX = 1;//开
			OLED_ShowString(0,16,"Standard Mode  ",16,1);
            OLED_ShowChinese(0, 0, 0, 16, 1);
            OLED_ShowChinese(16, 0, 1, 16, 1);
            OLED_ShowChinese(32, 0, 2, 16, 1);
            OLED_ShowChinese(48, 0, 3, 16, 1);
            OLED_Refresh();
        }
        else if(strstr((const char *)USART3_RX_BUF, "ydms"))
        {
            PowerMode = 2;
            RGB_YELLOW1(108);
            temp = 4;
            KT = 1;
            BX = 1;
			OLED_ShowString(0,16,"Reading Mode   ",16,1);
            OLED_ShowChinese(0, 0, 14, 16, 1);
            OLED_ShowChinese(16, 0, 15, 16, 1);
            OLED_ShowChinese(32, 0, 2, 16, 1);
            OLED_ShowChinese(48, 0, 3, 16, 1);
            OLED_Refresh();
        } 
		else if(strstr((const char *)USART3_RX_BUF, "ljms"))
        {
			PowerMode = 4;
			RGB_BLACK(108);//让他全关
			u3_printf("wzl");
			temp = 7;
			Meng(2);
			Meng1(2);
			Meng2(2);
			Curtain_control(0);
			Curtain = 0;
			Fan_control(0);
			Fan = 0;
			KT = 0;
			BX = 1;
			OLED_ShowString(0,16,"Away Mode           ",16,1);
			OLED_ShowChinese(0,0,16,16,1);
			OLED_ShowChinese(16,0,17,16,1);
			OLED_ShowChinese(32,0,2,16,1);
			OLED_ShowChinese(48,0,3,16,1);
			OLED_Refresh();
		}
        else if(strstr((const char *)USART3_RX_BUF, "whitek"))
        {
            RGB_WHITE(108);
            temp = 0;
            OLED_ShowString(70, 0, "WHITE  ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "redk"))
        {
            RGB_RED(108);
            temp = 1;
            OLED_ShowString(70, 0, "RED    ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "greenk"))
        {
            RGB_GREEN(108);
            temp = 2;
            OLED_ShowString(70, 0, "GREEN   ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "bluek"))
        {
            RGB_BLUE(108);
            temp = 3;
            OLED_ShowString(70, 0, "BULE   ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "yellowk"))
        {
            RGB_YELLOW(108);
            temp = 4;
            OLED_ShowString(70, 0, "YELLOW ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "purplek"))
        {
            RGB_PURPLE(108);
            temp = 5;
            OLED_ShowString(70, 0, "PURPLE ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "skyk"))
        {
            RGB_SKY(108);
            temp = 6;
            OLED_ShowString(70, 0, "SKY    ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "ggg"))
        {
            RGB_BLACK(108);
            temp = 7;
            OLED_ShowString(70, 0, "BLACK  ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "ldjx"))
        {
                switch(temp)
                {
                case 0:
                    RGB_WHITE1(108);
                    break;
                case 1:
                    RGB_RED1(108);
                    break;
                case 2:
                    RGB_GREEN1(108);
                    break;
                case 3:
                    RGB_BLUE1(108);
                    break;
                case 4:
                    RGB_YELLOW1(108);
                    break;
                case 5:
                    RGB_PURPLE1(108);
                    break;
                case 6:
                    RGB_SKY1(108);
                    break;
                }
        }
        else if(strstr((const char *)USART3_RX_BUF, "ldzd"))
        {
                switch(temp)
                {
                case 0:
                    RGB_WHITE(108);
                    break;
                case 1:
                    RGB_RED(108);
                    break;
                case 2:
                    RGB_GREEN(108);
                    break;
                case 3:
                    RGB_BLUE(108);
                    break;
                case 4:
                    RGB_YELLOW(108);
                    break;
                case 5:
                    RGB_PURPLE(108);
                    break;
                case 6:
                    RGB_SKY(108);
                    break;
                }
        }
        else if(strstr((const char *)USART3_RX_BUF, "qshxdk"))
        {
            OLED_ShowString(70, 0, "BREATHE", 16, 1);
            temp = 10;
            RGB_RED_Breathe(108);
            delay_ms(10);

            RGB_YELLOW_Breathe(108);
            delay_ms(10);

            RGB_GREEN_Breathe(108);
            delay_ms(10);

            RGB_BLUE_Breathe(108);
            delay_ms(10);

            RGB_SKY_Breathe(108);
            delay_ms(10);

            RGB_PURPLE_Breathe(108);
            delay_ms(10);

            RGB_WHITE_Breathe(108);
            delay_ms(10);
            RGB_BLACK(108);
        }
        else if(strstr((const char *)USART3_RX_BUF, "zmd"))
        {
            OLED_ShowString(70, 0, "HORSE  ", 16, 1);
            zmd();
            temp = 9;
        }
        else if(strstr((const char *)USART3_RX_BUF, "chd"))
        {
            OLED_ShowString(70, 0, "RAINBOW", 16, 1);
            rainbowCycle(108);
            temp = 8;
        }
        else if(strstr((const char *)USART3_RX_BUF, "clk"))
        {
            Curtain = 1;
            Curtain_control(Curtain);
            OLED_ShowString(0, 16, "Curtain on   ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "clg"))
        {
            Curtain = 0;
            Curtain_control(Curtain);
            OLED_ShowString(0, 16, "Curtain off  ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "fsk"))
        {
            Fan = 1;
            Fan_control(1);
            OLED_ShowString(0, 16, "fan on       ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "fsg"))
        {
            Fan = 0;
            Fan_control(0);
            OLED_ShowString(0, 16, "fan off      ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "dmk"))
        {
            Meng(1);
            Door = 1;
            OLED_ShowString(0, 16, "Door on      ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "dmg"))
        {
            Meng(0);
            Door = 0;
            OLED_ShowString(0, 16, "Door off     ", 16, 1);
        }
		else if(strstr((const char *)USART3_RX_BUF, "zwmk"))
        {
			Bedroom = 1;
            Meng1(Bedroom);
            OLED_ShowString(0, 16, "Bedroom on ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "zwmg"))
        {
			Bedroom = 0;
            Meng1(Bedroom);
            OLED_ShowString(0, 16, "Bedroom off", 16, 1);
        }
		
		else if(strstr((const char *)USART3_RX_BUF, "guestk"))
        {
			Guest = 1;
            Meng2(Guest);
            OLED_ShowString(0, 16, "Guest on     ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "guestg"))
        {
			Guest = 0;
            Meng2(Guest);
            OLED_ShowString(0, 16, "Guest off    ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "ktk"))
        {
            KT = 1;
            OLED_ShowString(0, 16, "AC on        ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "ktg"))
        {
            KT = 0;
            OLED_ShowString(0, 16, "AC off       ", 16, 1);
        }
		else if(strstr((const char *)USART3_RX_BUF, "bxk"))
        {
            BX = 1;
            OLED_ShowString(0, 16, "frige on     ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "bxg"))
        {
            BX = 0;
            OLED_ShowString(0, 16, "frige off    ", 16, 1);
        }
        else if(strstr((const char *)USART3_RX_BUF, "whjl"))
        {
            PowerMode = 0;
            Door = 1;//开门
            Meng(Door);
            OLED_ShowString(0, 16, "Door on      ", 16, 1);
            RGB_WHITE(108);
            Curtain_control(1);
            temp = 0;
            Curtain = 1;
            KT = 1;//开
            BX = 1;//开
            OLED_ShowChinese(0, 0, 0, 16, 1);
            OLED_ShowChinese(16, 0, 1, 16, 1);
            OLED_ShowChinese(32, 0, 2, 16, 1);
            OLED_ShowChinese(48, 0, 3, 16, 1);
            OLED_Refresh();
        }
        else if(strstr((const char *)USART3_RX_BUF, "wzllll"))
        {
            PowerMode = 4;
            RGB_BLACK(108);//让他全关
            temp = 7;
            Meng(0);
            Meng1(0);
            Meng2(0);
            Door = 0;
            Bedroom = 0;
            Guest = 0;
            Curtain_control(0);
            Curtain = 0;
            Fan_control(0);
            Fan = 0;
            KT = 0;
            BX = 1;
            OLED_ShowChinese(0, 0, 16, 16, 1); //离
            OLED_ShowChinese(16, 0, 17, 16, 1); //家
            OLED_ShowChinese(32, 0, 2, 16, 1); //模
            OLED_ShowChinese(48, 0, 3, 16, 1); //式
        }
        memset(USART3_RX_BUF, 0, 50);
        USART3_RX_STA = 0;
    }
}










