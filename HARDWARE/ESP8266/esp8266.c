#include "esp8266.h"

uint16_t PowerMode = 0;
extern u16 temp;
u8 Temp, Humidity;
//bool Switch = 0;//云端是Switch 0是关闭，1是打开
bool Door = 0;     //门都是1开
bool Bedroom = 0;  //门都是1开
bool Guest = 0;    //门都是1开
bool Curtain = 0;//1为开
bool Fan = 0;//1为开
bool Gas = 0;//1为检测到可燃气体

void ESP8266_Init(void)
{
	OLED_ShowString(0,0,"Waiting for esp8",16,1);
	OLED_ShowString(0,16,"266 Init",16,1);
    //重启一下esp8266
    printf("AT+RST\r\n");
    delay_ms(2000);

   //关闭回显
   	printf("ATE0\r\n");
   	delay_ms(100);

    //sta模式
    printf("AT+CWMODE=1\r\n");
    delay_ms(1000);

    //连接一下热点
    printf("AT+CWJAP=\"K50\",\"lhjzdchhh\"\r\n");
    delay_ms(3000);

    //设置时区和服务器
    printf("AT+CIPSNTPCFG=1,8,\"ntp1.aliyun.com\"\r\n");
    delay_ms(2000);

    //用户信息配置
    printf("AT+MQTTUSERCFG=0,1,\"a1eXs5dmrZe.HJJC|securemode=2\\,signmethod=hmacsha256\\,timestamp=1672745016493|\",\"HJJC&a1eXs5dmrZe\",\"0480a00d93a6f35cbe30086b252e5815ae199ac4ae65ca687408b44ba2414a12\",0,0,\"\"\r\n");
    delay_ms(2000);//注意逗号前要加2个转义字符。

    //连接服务器
    printf("AT+MQTTCONN=0,\"a1eXs5dmrZe.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883,1\r\n");
    delay_ms(2000);
	OLED_ShowString(0,0,"ESP8266 Finished!",16,1);
	BEEP = 1;
	delay_ms(100);
	BEEP = 0;
	delay_ms(100);
	BEEP = 1;
	delay_ms(300);
	BEEP = 0;
	delay_ms(500);
	OLED_Clear();
	OLED_ShowString(48,48,"Be running",16,1);
    //由于我已经订阅过了我就注释掉了
    //让设备上云
    //--------------------------------------------
    //订阅指令收云发来的消息
    ///sys/a1eXs5dmrZe/${deviceName}/thing/event/property/post_reply


    //printf("AT+MQTTSUB=0,\"/sys/a1eXs5dmrZe/HJJC/thing/service/property/set\",1\r\n");//订阅属性指令
    //delay_ms(1500);

    //printf("AT+MQTTSUB=0,\"/sys/a1eXs5dmrZe/HJJC/thing/event/AlarmEvent/post\",1\r\n");//订阅事件指令
    ////   /sys/a1eXs5dmrZe/${deviceName}/thing/event/${tsl.event.identifier}/post
    //delay_ms(1500);
}

//发的东西有点多，要分几次发
void SendMessage(int i)
{
    DHT11_Read_Data(&Temp, &Humidity);
    if(i % 15 == 0)
    {
        printf("AT+MQTTPUB=0,\"/sys/a1eXs5dmrZe/HJJC/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"params\\\":{\\\"ikun\\\":%d\\,\\\"Temp\\\":%d\\,\\\"Door\\\":%d\\,\\\"LightType\\\":%d\\,\\\"Curtain\\\":%d\\,\\\"KT\\\":%d\\,\\\"PowerMode\\\":%d}}\",0,0\r\n"
               , i, Temp, Door, temp, Curtain, (bool)KT, PowerMode);//重要变的
        delay_ms(500);
        printf("AT+MQTTPUB=0,\"/sys/a1eXs5dmrZe/HJJC/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"params\\\":{\\\"Bedroom\\\":%d\\,\\\"Guest\\\":%d\\,\\\"Humidity\\\":%d\\,\\\"FAN\\\":%d\\,\\\"BX\\\":%d}}\",0,0\r\n"
               , Bedroom, Guest, Humidity, Fan, (bool)BX);
    }
    else if(Gas_analyze())//燃气检测
    {
		OLED_ShowChinese(0,16,4,16,1);
		OLED_ShowChinese(16,16,5,16,1);
		OLED_ShowChinese(32,16,6,16,1);
		OLED_ShowChinese(48,16,7,16,1);
		OLED_ShowChinese(64,16,8,16,1);
		OLED_ShowChinese(80,16,9,16,1);
		OLED_ShowString(96,16,"!",16,1);
		Fan = 1,Curtain = 1,Door = 1,Bedroom = 1,Guest = 1;
		Fan_control(Fan);
		Curtain_control(Curtain);
		Meng(Door);
		Meng1(Bedroom);
		Meng2(Guest);
        printf("AT+MQTTPUB=0,\"/sys/a1eXs5dmrZe/HJJC/thing/event/property/post\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"params\\\":{\\\"Gas\\\":%d}}\",0,0\r\n"
               , Gas_analyze());
        u3_printf("krqt");
        WarmLight();
    }
}

void ProcessMessage(void)
{
    if(USART_RX_STA & 0x8000)
    {
        if(strncmp((const char *)USART_RX_BUF, "+MQTTSUBRECV", 12) == 0)
        {
            if(strstr((const char *)USART_RX_BUF, "PowerMode"))
            {
                char *j = strstr((const char *)USART_RX_BUF, "Mode");
                if(*(j + 6) == '0') //标准模式
                {
                    PowerMode = 0;
                    RGB_WHITE(108);
					u3_printf("bzms");//给小美说标准模式
                    temp = 0;
					KT = 1;//开
				    BX = 1;//开
					OLED_ShowString(0,16,"Standard Mode",16,1);
					OLED_ShowChinese(0,0,0,16,1);
					OLED_ShowChinese(16,0,1,16,1);
					OLED_ShowChinese(32,0,2,16,1);
					OLED_ShowChinese(48,0,3,16,1);
					OLED_Refresh();
                }
                else if(*(j + 6) == '1') //节能模式
                {
                    PowerMode = 1;
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
					u3_printf("jnms");
                    Fan_control(0);
                    Fan = 0;
                    KT = 0;//关
				    BX = 1;//关
					OLED_ShowString(0,16,"ECO Mode        ",16,1);
					OLED_ShowChinese(0,0,12,16,1);
					OLED_ShowChinese(16,0,13,16,1);
					OLED_ShowChinese(32,0,2,16,1);
					OLED_ShowChinese(48,0,3,16,1);
					OLED_Refresh();
                    
                }
                else if(*(j + 6) == '2') //阅读模式
                {
                    PowerMode = 2;
					RGB_YELLOW1(108);
					u3_printf("ydms");
                    temp = 4;
					OLED_ShowString(0,16,"Reading Mode        ",16,1);
					OLED_ShowChinese(0,0,14,16,1);
					OLED_ShowChinese(16,0,15,16,1);
					OLED_ShowChinese(32,0,2,16,1);
					OLED_ShowChinese(48,0,3,16,1);
					OLED_Refresh();
                }
                else if(*(j + 6) == '3') //氛围模式
                {
                    PowerMode = 3; 
					u3_printf("fwms"); 
					temp = 8;
					OLED_ShowString(0,16,"Atp Mode        ",16,1);
					OLED_ShowChinese(0,0,10,16,1);
					OLED_ShowChinese(16,0,11,16,1);
					OLED_ShowChinese(32,0,2,16,1);
					OLED_ShowChinese(48,0,3,16,1);
					OLED_Refresh();
                    rainbowCycle(108);
                    KT = 1;
					BX = 1;
                }
                else if(*(j + 6) == '4') //离家模式
                {
					PowerMode = 4;
					RGB_BLACK(108);//让他全关
					u3_printf("wzl");
					temp = 7;
					Meng(CLOSE);
					Meng1(CLOSE);
					Meng2(CLOSE);
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
            }
            else if(strstr((const char *)USART_RX_BUF, "KT"))
            {
                KT = !KT;
				if(KT == 0)
				{
					OLED_ShowString(0,16,"AC off      ",16,1);
				}
				else
				{
					OLED_ShowString(0,16,"AC on       ",16,1);
				}
            }
            else if(strstr((const char *)USART_RX_BUF, "BX"))
            {
                BX = !BX;
				if(BX == 0)
				OLED_ShowString(0,16,"frige off     ",16,1);
				else
				OLED_ShowString(0,16,"frige on    ",16,1);
            }
            else if(strstr((const char *)USART_RX_BUF, "FAN"))
            {
                Fan = !Fan;
                Fan_control(Fan);
				if(Fan == 1)
				OLED_ShowString(0,16,"fan on       ",16,1);
				else
				OLED_ShowString(0,16,"fan off      ",16,1);	
            }
            else if(strstr((const char *)USART_RX_BUF, "Door"))
            {
				Door = !Door;
                Meng(Door);
				if(Door == 1)
				OLED_ShowString(0,16,"Door on      ",16,1);
				else
				OLED_ShowString(0,16,"Door off     ",16,1);	
            }
            else if(strstr((const char *)USART_RX_BUF, "Bedroom"))
            {
				Bedroom = !Bedroom;
                Meng1(Bedroom);
				if(Bedroom == 1)
				OLED_ShowString(0,16,"Bedroom on",16,1);
				else
				OLED_ShowString(0,16,"Bedroom off",16,1);
            }
            else if(strstr((const char *)USART_RX_BUF, "Guest"))
            {
				Guest = !Guest;
                Meng2(Guest);
				if(Guest == 1)
				OLED_ShowString(0,16,"Guest on     ",16,1);
				else
				OLED_ShowString(0,16,"Guest off    ",16,1);
            }
            else if(strstr((const char *)USART_RX_BUF, "Curtain"))
            {
				Curtain = !Curtain;
                Curtain_control(Curtain);
				if(Curtain == 1)
				OLED_ShowString(0,16,"Curtain on   ",16,1);
				else
				OLED_ShowString(0,16,"Curtain off  ",16,1);
            }
            else if(strstr((const char *)USART_RX_BUF, "Type"))
            {
                char *i = strstr((const char *)USART_RX_BUF, "Type");
                //找到Type后面的数字
                if(*(i + 6) == '0')
                {
                    RGB_WHITE(108);
                    temp = 0;
					OLED_ShowString(70,0,"WHITE  ",16,1);
                }
                else if(*(i + 6) == '1' && *(i + 7) == '}')
                {
                    RGB_RED(108);
                    temp = 1;
					OLED_ShowString(70,0,"RED    ",16,1);
                }
                else if(*(i + 6) == '2')
                {
                    RGB_GREEN(108);
                    temp = 2;
					OLED_ShowString(70,0,"GREEN   ",16,1);
                }
                else if(*(i + 6) == '3')
                {
                    RGB_BLUE(108);
                    temp = 3;
					OLED_ShowString(70,0,"BULE   ",16,1);
                }
                else if(*(i + 6) == '4')
                {
                    RGB_YELLOW(108);
                    temp = 4;
					OLED_ShowString(70,0,"YELLOW ",16,1);
                }
                else if(*(i + 6) == '5')
                {
                    RGB_PURPLE(108);
                    temp = 5;
					OLED_ShowString(70,0,"PURPLE ",16,1);
                }
                else if(*(i + 6) == '6')
                {
                    RGB_SKY(108);
                    temp = 6;
					OLED_ShowString(70,0,"SKY    ",16,1);
                }
                else if(*(i + 6) == '7')
                {
                    RGB_BLACK(108);
                    temp = 7;
					OLED_ShowString(70,0,"BLACK  ",16,1);
                }
                else if(*(i + 6) == '8')
                {
					OLED_ShowString(70,0,"RAINBOW",16,1);
                    rainbowCycle(108);
                    temp = 8;
                }
                else if(*(i + 6) == '9')
                {
					OLED_ShowString(70,0,"HORSE  ",16,1);
                    zmd();
                    temp = 9;
                }
                else if(*(i + 7) == '0')
                {
					OLED_ShowString(70,0,"BREATHE",16,1);
                    qchxd();
                    temp = 10;
                }
            }
        }
        memset(USART_RX_BUF, 0, 200);
        USART_RX_STA = 0;
    }
}



