#include "WS2812.h"
#include "stdlib.h"

int count = 0;

// Some Static Colors
RGBColor_TypeDef HHH0   = {0, 0, 0};
const RGBColor_TypeDef WHITE1    = {25, 25, 25};
const RGBColor_TypeDef RED1      = {25, 0, 0};
const RGBColor_TypeDef GREEN1    = {0, 25, 0};
const RGBColor_TypeDef BLUE1     = {0, 0, 25};
const RGBColor_TypeDef YELLOW1   = {25, 25, 0};
const RGBColor_TypeDef PURPLE1   = {25, 0, 25};
const RGBColor_TypeDef SKY1      = {0, 25, 25};


const RGBColor_TypeDef WHITE    = {255, 255, 255};
const RGBColor_TypeDef RED      = {255, 0, 0};
const RGBColor_TypeDef GREEN    = {0, 255, 0};
const RGBColor_TypeDef BLUE     = {0, 0, 255};
const RGBColor_TypeDef YELLOW   = {255, 255, 0};
const RGBColor_TypeDef PURPLE   = {255, 0, 255};
const RGBColor_TypeDef SKY      = {0, 255, 255};
const RGBColor_TypeDef BLACK    = {0, 0, 0};

const RGBColor_TypeDef ORANGE   = {127, 106, 0};
const RGBColor_TypeDef MAGENTA  = {255, 0, 255};

static u8 pixelBuffer[Pixel_S1_NUM][24];//灯珠


/***********************************************************************************************
**     name: WS2812b_Configuration
** function:  WS2812B SPI DMA总线初始化
**parameter: void
************************************************************************************************/
/*****************************************
 说明：
 SPI2：
 引脚：使用的是PB15引脚，在TFTLCD下的LCD BL
 时钟：根据总线图，SPI2由APB1(42MHz)分频而来
 ****************************************/
void WS2812b_Configuration(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //使能GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //使能SPI2时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //DMA1时钟使能

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                    //PB15复用功能输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        //初始化

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);      //PB15复用为 SPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //设置SPI工作模式:设置为主SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //串行同步时钟的空闲状态为低电平
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//42M/8=5.25M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRC值计算的多项式
    SPI_Init(SPI2, &SPI_InitStructure);                                   //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(SPI2, ENABLE);                                                //使能SPI外设

    DMA_DeInit(DMA1_Stream4);

    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}                      //等待DMA可配置

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                            //通道选择
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;                //DMA外设地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)pixelBuffer;                 //DMA 存储器0地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                   //存储器到外设模式
    DMA_InitStructure.DMA_BufferSize = Pixel_S1_NUM * 24;                     //数据传输量
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //外设非增量模式
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //存储器增量模式
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //外设数据长度:8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //存储器数据长度:8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // 使用普通模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                     //中等优先级
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;               //存储器突发单次传输
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;       //外设突发单次传输
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);                               //初始化DMA Stream

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // 使能SPI2的DMA发送
    DMA_Cmd(DMA1_Stream4, DISABLE);                            //关闭DMA传输
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}	      //确保DMA可以被设置
    DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_S1_NUM * 24);   //数据传输量
    DMA_Cmd(DMA1_Stream4, ENABLE);

    delay_ms(1);
    RGB_BLACK(Pixel_S1_NUM);                                   //RGB RESET
    delay_ms(1);
}

/***********************************************************************************************
**     name: rgb_SetColor
** function: 设定某个RGB LED的颜色
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SetColor(u16 LedId, RGBColor_TypeDef Color)
{

    u16 i;

    if( LedId > ( Pixel_S1_NUM ) )
    {
        printf("Error:Out of Range!\r\n");
        return;                               //to avoid overflow
    }

    for(i = 0; i <= 7; i++)
    {
        pixelBuffer[LedId][i] = ( (Color.G & (1 << (7 - i)) ) ? (CODE1) : CODE0 );
    }
    for(i = 8; i <= 15; i++)
    {
        pixelBuffer[LedId][i] = ( (Color.R & (1 << (15 - i)) ) ? (CODE1) : CODE0 );
    }
    for(i = 16; i <= 23; i++)
    {
        pixelBuffer[LedId][i] = ( (Color.B & (1 << (23 - i)) ) ? (CODE1) : CODE0 );
    }
}

/***********************************************************************************************
**     name: rgb_SendArray
** function: Configure colors to RGB pixel series.
             RGBColor_TypeDef: pointer to a RGBColor_TypeDef structure that contains the color configuration information for the RGB pixel.
**parameter: void
**   return: void
************************************************************************************************/
void rgb_SendArray(void)
{

    if(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) != RESET) //等待DMA1_Steam5传输完成
    {
        DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);               //清除DMA1_Steam5传输完成标志，先预想DMA_FLAG_TCIF0的零，代表的是Channel
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // 使能SPI2的DMA发送
        DMA_Cmd(DMA1_Stream4, DISABLE);                            //关闭DMA传输
        while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}	      //确保DMA可以被设置
        DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_S1_NUM * 24);   //数据传输量
        DMA_Cmd(DMA1_Stream4, ENABLE);                             //开启DMA传输
    }
}

/***********************************************************************************************
**     name: RGB_RED
** function: 设定颜色为RED
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_RED(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, RED);
    }
    rgb_SendArray();
}

void RGB_RED1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, RED1);
    }
    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_PURPLE
** function: 设定颜色为PURPLE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_PURPLE(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, PURPLE);
    }
    rgb_SendArray();
}

void RGB_PURPLE1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, PURPLE1);
    }
    rgb_SendArray();
}



/***********************************************************************************************
**     name: RGB_SKY
** function: 设定颜色为SKY
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_SKY(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, SKY);
    }
    rgb_SendArray();
}
void RGB_SKY1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, SKY1);
    }
    rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_GREEN
** function: 设定颜色为GREEN
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_GREEN(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, GREEN);
    }
    rgb_SendArray();
}
void RGB_GREEN1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, GREEN1);
    }
    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_BLUE
** function: 设定颜色为BLUE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLUE(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, BLUE);
    }
    rgb_SendArray();
}

void RGB_BLUE1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, BLUE1);
    }
    rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_YELLOW
** function: 设定颜色为YELLOW
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_YELLOW(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, YELLOW);
    }
    rgb_SendArray();
}

void RGB_YELLOW1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, YELLOW1);
    }
    rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_BLACK
** function: 设定颜色为all-off
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_BLACK(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {

        rgb_SetColor(i, BLACK);
    }
    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_WHITE
** function: 设定颜色为WHITE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_WHITE(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, WHITE);
    }
    rgb_SendArray();
}

void RGB_WHITE1(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, WHITE1);
    }
    rgb_SendArray();
}
/***********************************************************************************************
**     name: RGB_MAGENTA
** function: 设定颜色为MAGENTA
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_MAGENTA(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, MAGENTA);
    }
    rgb_SendArray();
}

/***********************************************************************************************
**     name: RGB_ORANGE
** function: 设定颜色为ORANGE
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void RGB_ORANGE(u16 Pixel_LEN)
{
    u16 i;
    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, ORANGE);
    }
    rgb_SendArray();
}
/***********************************************************************************************
**     name: Colourful_Wheel
** function: 将颜色转换为GRB
**parameter: WheelPos 颜色数值
**   return: RGBColor_TypeDef 颜色GRB
************************************************************************************************/
RGBColor_TypeDef Colourful_Wheel(u8 WheelPos)
{
    RGBColor_TypeDef color;
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85)
    {
        color.R = 255 - WheelPos * 3;
        color.G = 0;
        color.B = WheelPos * 3;
        return color;
    }
    if(WheelPos < 170)
    {
        WheelPos -= 85;
        color.R = 0;
        color.G = WheelPos * 3;
        color.B = 255 - WheelPos * 3;
        return color;
    }
    WheelPos -= 170;
    color.R = WheelPos * 3;
    color.G = 255 - WheelPos * 3;
    color.B = 0;
    return color;
}

/***********************************************************************************************
**     name: rainbowCycle
** function: 呼吸灯功能
**parameter: Pixel_LEN 灯珠数
**   return: void
************************************************************************************************/
void rainbowCycle(u16 Pixel_LEN)
{
    u16 i, j = 0;
    for(j = 0; j < 1023; j++)                  // 1 cycles of all colors on wheel
    {
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, Colourful_Wheel(((i * 256 / Pixel_LEN) + j) & 255));
        }
        rgb_SendArray();
        delay_ms(10);
    }
    RGB_MAGENTA(108);
}

//////////////////////走马灯
void sky(u16 Pixel_LEN)
{
    while(count++ <= 108)
    {
        RGB_SKY(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void white(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_WHITE(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void red(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_RED(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void purple(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_PURPLE(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void magenta(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_MAGENTA(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void orange(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_ORANGE(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void green(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_GREEN(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void blue(u16 Pixel_LEN)
{
    while(count++ <= Pixel_LEN)
    {
        RGB_BLUE(count);
        delay_ms(10);
    }
    RGB_BLACK(108);
    count = 0;
}

void zmd(void)
{
    RGB_BLACK(108);
    red(108);
    orange(108);
    magenta(108);
    green(108);
    blue(108);
    sky(108);
    purple(108);
    white(108);
	Random_zm(108);
}

void colorful(u16 Pixel_LEN)
{
    u16 i = 0;
    while(i++ < Pixel_LEN)
    {
        if(i < 18)
        {
            rgb_SetColor(i, BLUE);
        }
        else if(i >= 18 && i < 36)
        {
            rgb_SetColor(i, SKY);
        }
        else if(i >= 36 && i < 54)
        {
            rgb_SetColor(i, ORANGE);
        }
        else if(i >= 54 && i < 72)
        {
            rgb_SetColor(i, RED);
        }
        else
        {
            rgb_SetColor(i, MAGENTA);
        }
        rgb_SendArray();
        delay_ms(50);
    }
}

void RGB_SKY_Breathe(u16 Pixel_LEN)//SKY
{
    u16 i;
    u16 temp = 0;
    for(temp = 0; temp <= 255; temp++)
    {
        HHH0.G = temp;
        HHH0.B = temp;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(temp = 255; temp > 0; temp--)
    {
        HHH0.G = temp;
        HHH0.B = temp;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_RED_Breathe(u16 Pixel_LEN)//RED
{
    u16 i;
    u16 r = 0;
    for(r = 0; r <= 255; r++)
    {
        HHH0.R = r;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(r = 255; r > 0; r--)
    {
        HHH0.R = r;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_GREEN_Breathe(u16 Pixel_LEN)//GREEN
{
    u16 i;
    u16 g = 0;
    for(g = 0; g <= 255; g++)
    {
        HHH0.G = g;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(g = 255; g > 0; g--)
    {
        HHH0.G = g;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_BLUE_Breathe(u16 Pixel_LEN)//BLUE
{
    u16 i;
    u16 j = 0;
    for(j = 0; j <= 255; j++)
    {
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(j = 255; j > 0; j--)
    {
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_YELLOW_Breathe(u16 Pixel_LEN)//YELLOW
{
    u16 i;
    u16 j = 0;
    for(j = 0; j <= 255; j++)
    {
        HHH0.R = j;
        HHH0.G = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(j = 255; j > 0; j--)
    {
        HHH0.R = j;
        HHH0.G = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_PURPLE_Breathe(u16 Pixel_LEN)//PURPLE
{
    u16 i;
    u16 j = 0;
    for(j = 0; j <= 255; j++)
    {
        HHH0.R = j;
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(j = 255; j > 0; j--)
    {
        HHH0.R = j;
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void RGB_WHITE_Breathe(u16 Pixel_LEN)//WHITE
{
    u16 i;
    u16 j = 0;
    for(j = 0; j <= 255; j++)
    {
        HHH0.R = j;
        HHH0.G = j;
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    for(j = 255; j > 0; j--)
    {
        HHH0.R = j;
        HHH0.G = j;
        HHH0.B = j;
        for(i = 0; i < Pixel_LEN; i++)
        {
            rgb_SetColor(i, HHH0);
        }
        rgb_SendArray();
        delay_ms(3);
    }
    RGB_BLACK(108);
}

void qchxd(void)
{
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

void WarmLight(void)
{
    u16 time = 0;
    for(time = 0; time < 3; time++)
    {
        RGB_RED(108);
        BEEP = 1;
        delay_ms(300);
        RGB_BLACK(108);
        BEEP = 0;
        delay_ms(300);
    }
}

void Random_Light(u16 Pixel_LEN)
{
    u16 i;

    for(i = 0; i < Pixel_LEN; i++)
    {
        rgb_SetColor(i, Colourful_Wheel(rand() % 255));
    }
    rgb_SendArray();
}

void Random_zm(u16 Pixel_LEN)//随机灯的走马版
{
	while(count++ <= 108)
	{
		Random_Light(count);
		delay_ms(10);
	}
	RGB_BLACK(108);
	count = 0;
}

