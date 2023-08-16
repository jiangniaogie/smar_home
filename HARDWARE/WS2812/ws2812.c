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

static u8 pixelBuffer[Pixel_S1_NUM][24];//����


/***********************************************************************************************
**     name: WS2812b_Configuration
** function:  WS2812B SPI DMA���߳�ʼ��
**parameter: void
************************************************************************************************/
/*****************************************
 ˵����
 SPI2��
 ���ţ�ʹ�õ���PB15���ţ���TFTLCD�µ�LCD BL
 ʱ�ӣ���������ͼ��SPI2��APB1(42MHz)��Ƶ����
 ****************************************/
void WS2812b_Configuration(void)
{

    GPIO_InitTypeDef  GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);  //ʹ��SPI2ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);  //DMA1ʱ��ʹ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;                    //PB15���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                  //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;            //100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                  //����
    GPIO_Init(GPIOB, &GPIO_InitStructure);                        //��ʼ��

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);      //PB15����Ϊ SPI2

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;    //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		                      //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		                  //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		                        //����ͬ��ʱ�ӵĿ���״̬Ϊ�͵�ƽ
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	                        //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		                          //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//42M/8=5.25M
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	                  //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;	                            //CRCֵ����Ķ���ʽ
    SPI_Init(SPI2, &SPI_InitStructure);                                   //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI2, ENABLE);                                                //ʹ��SPI����

    DMA_DeInit(DMA1_Stream4);

    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}                      //�ȴ�DMA������

    DMA_InitStructure.DMA_Channel = DMA_Channel_0;                            //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&SPI2->DR;                //DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)pixelBuffer;                 //DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                   //�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize = Pixel_S1_NUM * 24;                     //���ݴ�����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          //���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   //�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   //�������ݳ���:8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           //�洢�����ݳ���:8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // ʹ����ͨģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                     //�е����ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;               //�洢��ͻ�����δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;       //����ͻ�����δ���
    DMA_Init(DMA1_Stream4, &DMA_InitStructure);                               //��ʼ��DMA Stream

    SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // ʹ��SPI2��DMA����
    DMA_Cmd(DMA1_Stream4, DISABLE);                            //�ر�DMA����
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}	      //ȷ��DMA���Ա�����
    DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_S1_NUM * 24);   //���ݴ�����
    DMA_Cmd(DMA1_Stream4, ENABLE);

    delay_ms(1);
    RGB_BLACK(Pixel_S1_NUM);                                   //RGB RESET
    delay_ms(1);
}

/***********************************************************************************************
**     name: rgb_SetColor
** function: �趨ĳ��RGB LED����ɫ
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

    if(DMA_GetFlagStatus(DMA1_Stream4, DMA_FLAG_TCIF4) != RESET) //�ȴ�DMA1_Steam5�������
    {
        DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);               //���DMA1_Steam5������ɱ�־����Ԥ��DMA_FLAG_TCIF0���㣬�������Channel
        SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);	         // ʹ��SPI2��DMA����
        DMA_Cmd(DMA1_Stream4, DISABLE);                            //�ر�DMA����
        while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE) {}	      //ȷ��DMA���Ա�����
        DMA_SetCurrDataCounter(DMA1_Stream4, Pixel_S1_NUM * 24);   //���ݴ�����
        DMA_Cmd(DMA1_Stream4, ENABLE);                             //����DMA����
    }
}

/***********************************************************************************************
**     name: RGB_RED
** function: �趨��ɫΪRED
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪPURPLE
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪSKY
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪGREEN
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪBLUE
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪYELLOW
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪall-off
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪWHITE
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪMAGENTA
**parameter: Pixel_LEN ������
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
** function: �趨��ɫΪORANGE
**parameter: Pixel_LEN ������
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
** function: ����ɫת��ΪGRB
**parameter: WheelPos ��ɫ��ֵ
**   return: RGBColor_TypeDef ��ɫGRB
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
** function: �����ƹ���
**parameter: Pixel_LEN ������
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

//////////////////////�����
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

void Random_zm(u16 Pixel_LEN)//����Ƶ������
{
	while(count++ <= 108)
	{
		Random_Light(count);
		delay_ms(10);
	}
	RGB_BLACK(108);
	count = 0;
}

