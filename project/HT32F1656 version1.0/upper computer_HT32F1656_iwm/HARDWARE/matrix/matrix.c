#include "matrix.h"
#include "delay.h"
#include "usart.h"
#include "ebi_lcd.h"
#include "string.h"
#include "ESP8266.h"
#include "light_voice.h"

//全局变量
u8 anjian=0;
u16 Key_Num;
u8 KeyVal=0;

/*******************************************************************************************
函数名：matrix_KEY_Init
入口参数：无
出口参数：无
函数功能：初始化按键IO口
*******************************************************************************************/
void matrix_KEY_Init(void) 			//按键IO初始化函数
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.BKP        = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PE, AFIO_PIN_8|AFIO_PIN_9|AFIO_PIN_10|AFIO_PIN_11, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_DIR_OUT);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, ENABLE);
	GPIO_SetOutBits(HT_GPIOE,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	AFIO_GPxConfig(GPIO_PE, AFIO_PIN_12|AFIO_PIN_13|AFIO_PIN_14|AFIO_PIN_15, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_DIR_IN);
	GPIO_PullResistorConfig(HT_GPIOE,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PR_UP);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, ENABLE);
	
}
/*******************************************************************************************
函数名：matrix_KEY_Scan
入口参数：无
出口参数：KeyVal
函数功能：得到键值（行列扫描）
*******************************************************************************************/
u8 matrix_KEY_Scan(void)
{
	u16 temp;
	anjian=0;
	HT_GPIOE->DOUTR&=0xf0ff;							//GPIOE PIN8-11,行输出0 0 0 0
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;					//读取列值
	if(temp==0xf000)									    //GPIO PIN12-15 无按下
		return 0;
	else
	{
		delay_ms(5);
		temp=HT_GPIOE->DINR;						
		temp&=0xf000;
		if(temp==0xf000)
		return 0;										        //去抖
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfeff;							    //行输出1 1 1 0 GPIO pin8输出0
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&=0xf000;						//读列输入
	switch(temp)
	{
		case 0xe000:KeyVal=1;break;
		case 0xd000:KeyVal=5;break;
		case 0xb000:KeyVal=9;break;
		case 0x7000:KeyVal=13;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfdff;							//行输出1 1 0 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=2;break;
		case 0xd000:KeyVal=6;break;
		case 0xb000:KeyVal=10;break;
		case 0x7000:KeyVal=14;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfbff;							//行输出1 0 1 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=3;break;
		case 0xd000:KeyVal=7;break;
		case 0xb000:KeyVal=11;break;
		case 0x7000:KeyVal=15;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xf7ff;							//行输出0 1 1 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=4;break;
		case 0xd000:KeyVal=8;break;
		case 0xb000:KeyVal=12;break;
		case 0x7000:KeyVal=16;break;
	}
	return KeyVal;												 //返回键值
}

void key_get(void)
{
	u8 key;
	key = matrix_KEY_Scan();
		
	switch(key)
		{
			case 1:	
					switch(upper_monitor_flag)
						{
						  case 0:LCD_ShowString(210,35,200,16,16," 1");atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u1_printf("1\r\n");break ;
							case 1:LCD_ShowString(210,35,200,16,16," 1");atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u1_printf("1\r\n");break ;
							case 2:LCD_ShowString(210,35,200,16,16," 1");atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u1_printf("1\r\n");break ;
							case 3:LCD_ShowString(210,35,200,16,16," 1");atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u1_printf("1\r\n");break ;
							case 4:LCD_ShowString(210,35,200,16,16," 1");atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u1_printf("1\r\n");break ;
							default: 	LCD_ShowString(210,35,200,16,16," 1");break ;													
						}						
        break;
			case 2:
						switch(upper_monitor_flag)
						{
						  case 0:LCD_ShowString(210,35,200,16,16," 2");atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u1_printf("2\r\n");break ;
							case 1:LCD_ShowString(210,35,200,16,16," 2");atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u1_printf("2\r\n");break ;
							case 2:LCD_ShowString(210,35,200,16,16," 2");atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u1_printf("2\r\n");break ;
							case 3:LCD_ShowString(210,35,200,16,16," 2");atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u1_printf("2\r\n");break ;
							case 4:LCD_ShowString(210,35,200,16,16," 2");atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u1_printf("2\r\n");break ;
							default: 	LCD_ShowString(210,35,200,16,16," 2");break ;													
						}
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
						switch(upper_monitor_flag)
						{
						 		
							case 0:LCD_ShowString(15+16*11,145,230,16,16,"off!");atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u1_printf("6\r\n");break ;
							case 1:LCD_ShowString(15+16*11,145,230,16,16,"off!");atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u1_printf("6\r\n");break ;
							case 2:LCD_ShowString(15+16*11,145,230,16,16,"off!");atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u1_printf("6\r\n");break ;
							case 3:LCD_ShowString(15+16*11,145,230,16,16,"off!");atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u1_printf("6\r\n");break ;
							case 4:LCD_ShowString(15+16*11,145,230,16,16,"off!");atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u1_printf("6\r\n");break ;
							default:LCD_ShowString(15+16*11,145,230,16,16,"err!"); break ;											
						}
				break;
			case 6:
						switch(upper_monitor_flag)
						{
						 	case 0:LCD_ShowString(15+16*11,145,230,16,16," on!");atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u1_printf("5\r\n");break ;
							case 1:LCD_ShowString(15+16*11,145,230,16,16," on!");atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u1_printf("5\r\n");break ;
							case 2:LCD_ShowString(15+16*11,145,230,16,16," on!");atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u1_printf("5\r\n");break ;
							case 3:LCD_ShowString(15+16*11,145,230,16,16," on!");atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u1_printf("5\r\n");break ;
							case 4:LCD_ShowString(15+16*11,145,230,16,16," on!");atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u1_printf("5\r\n");break ;
							default: LCD_ShowString(15+16*11,145,230,16,16,"err!");	break ;								
						}
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			case 13:
				break;
			case 14:
				break;
			case 15:
				break;
			case 16:
				break;
			default:
				break;
		}
	}

