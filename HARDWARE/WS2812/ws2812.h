#ifndef __WS2812_H
#define __WS2812_H

/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "beep.h"


typedef struct
{
  u8 R;
  u8 G;
  u8 B;
} RGBColor_TypeDef;

//extern u8 pixelBuffer[][24];
extern const RGBColor_TypeDef RED ;
extern const RGBColor_TypeDef GREEN;
extern const RGBColor_TypeDef BLUE;
extern const RGBColor_TypeDef SKY;
extern const RGBColor_TypeDef MAGENTA ;
extern const RGBColor_TypeDef YELLOW ;
extern const RGBColor_TypeDef ORANGE;
extern const RGBColor_TypeDef BLACK;
extern const RGBColor_TypeDef WHITE;
extern const RGBColor_TypeDef PURPLE;
/* Exported constants --------------------------------------------------------*/
#define Pixel_S1_NUM 108		//���� RGB����

/**************************************
 ˵����

 WS2812B����Э��(��λ��ns)��
       min     typ     max
bit 0
 T0H:  220      -      420
 T0L:  750      -      1600
 
bit 1 
 T1H:  750      -      1600
 T1L:  220      -      420
 
 
 RESET: time > 300us


8��ƵAPB1��42MHz/8 = 5.25MHz
ʱ������Ϊ��1/5.25/1e6 = 1.90e-7=190ns
**************************************/

#define CODE0 0xC0 // 0��, ���͵�ʱ�� 1100 0000  ���ݲ�ͬ��SCK�ʵ�����
#define CODE1 0xFC // 1��, ���͵�ʱ�� 1111 1100

void WS2812b_Configuration(void);

/* Basic Color Effects -------------------------------------------------------*/
void RGB_RED(u16 Pixel_LEN);
void RGB_GREEN(u16 Pixel_LEN);
void RGB_BLUE(u16 Pixel_LEN);
void RGB_YELLOW(u16 Pixel_LEN);
void RGB_MAGENTA(u16 Pixel_LEN);
void RGB_BLACK(u16 Pixel_LEN);
void RGB_WHITE(u16 Pixel_LEN);
void RGB_SKY(u16 Pixel_LEN);
void RGB_ORANGE(u16 Pixel_LEN);
void RGB_PURPLE(u16 Pixel_LEN);
/* Complicated Color Effects -------------------------------------------------*/
void rainbowCycle(u16 Pixel_LEN);
void sky(u16 Pixel_LEN);
void white(u16 Pixel_LEN);
void red(u16 Pixel_LEN);
void purple(u16 Pixel_LEN);
void magenta(u16 Pixel_LEN);
void orange(u16 Pixel_LEN);
void green(u16 Pixel_LEN);
void blue(u16 Pixel_LEN);
void zmd(void);
void colorful(u16 Pixel_LEN);
void RGB_SKY_Breathe(u16 Pixel_LEN);//SKY
void RGB_RED_Breathe(u16 Pixel_LEN);//RED
void RGB_GREEN_Breathe(u16 Pixel_LEN);//
void RGB_BLUE_Breathe(u16 Pixel_LEN);//
void RGB_YELLOW_Breathe(u16 Pixel_LEN);//
void RGB_PURPLE_Breathe(u16 Pixel_LEN);//
void RGB_WHITE_Breathe(u16 Pixel_LEN);//
void qchxd(void);
//��С����
void RGB_WHITE1(u16 Pixel_LEN);
void RGB_RED1(u16 Pixel_LEN);
void RGB_GREEN1(u16 Pixel_LEN);
void RGB_BLUE1(u16 Pixel_LEN);
void RGB_YELLOW1(u16 Pixel_LEN);
void RGB_PURPLE1(u16 Pixel_LEN);
void RGB_SKY1(u16 Pixel_LEN);
void WarmLight(void);
void Random_Light(u16 Pixel_LEN);
void Random_zm(u16 Pixel_LEN);//����Ƶ������



#endif /* __WS2812_H */

