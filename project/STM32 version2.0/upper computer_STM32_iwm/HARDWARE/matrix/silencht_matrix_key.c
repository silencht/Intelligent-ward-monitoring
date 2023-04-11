#include "sys.h"
#include "silencht_matrix_key.h"
#include "delay.h"
#include "lcd.h"
#include "ESP8266.h"


u8 upper_monitor_flag; //B下位机标号


/***************************************************************************
	*函数名称：matrix_keys_Init
	*函数功能：初始化矩阵键盘
	*入口参数：无
	*返回参数：无
	*备注：                         
****************************************************************************/
void matrix_keys_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructuress;
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);           							 //打开GPIOF时钟               
 GPIO_InitStructuress.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  //初始化PF0-PF3
 GPIO_InitStructuress.GPIO_Mode = GPIO_Mode_OUT ;      												 //输出模式
 GPIO_InitStructuress.GPIO_OType = GPIO_OType_PP;         										 //设置为推挽输出
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;													 //100MHZ速率
 GPIO_InitStructuress.GPIO_PuPd =GPIO_PuPd_UP;     														 //上拉电阻
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 																		 //GPIOF初始化函数
 
 GPIO_InitStructuress.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;   //初始化PF4-PF7
 GPIO_InitStructuress.GPIO_Mode =GPIO_Mode_OUT ;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd = GPIO_PuPd_DOWN ;														 //下拉电阻
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_SetBits( GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);						 //high 0-3
 GPIO_ResetBits( GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);					 //low  4-7
}
/***************************************************************************
	*函数名称：matrix_keys_ANTI_Init
	*函数功能：反初始化矩阵键盘，交换行列引脚功能
	*入口参数：无
	*返回参数：无
	*备注：                         
****************************************************************************/
void matrix_keys_ANTI_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructuress;   
 GPIO_InitStructuress.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;    
 GPIO_InitStructuress.GPIO_Mode = GPIO_Mode_OUT ;      
 GPIO_InitStructuress.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd =GPIO_PuPd_UP;    
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_InitStructuress.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  
 GPIO_InitStructuress.GPIO_Mode =GPIO_Mode_OUT ;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd = GPIO_PuPd_DOWN ;
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_ResetBits( GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3); 							//low  0-3
 GPIO_SetBits( GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);								//high 4-7
}
/***************************************************************************
	*函数名称：keys_scan
	*函数功能：键盘按键扫描
	*入口参数：无
	*返回参数：u8类型键值
	*备注：                         
****************************************************************************/
u8 keys_scan(void)
{
	u8 a=0,b=0,t=0;	//定义行、列暂存变量
	matrix_keys_Init(); //键盘初始化
	if((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0) //如果行列值不为0000 1111（即有按键按下）
		{
			delay_ms (20);	//延时20毫秒，防抖动
			if((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0) //再次检测按键按下情况
			{
				a = GPIO_ReadInputData(GPIOF)&0xff;			 //键值放入暂存变量a
			}
			matrix_keys_ANTI_Init();//GPIO工作方式反转
			if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f) //如果行列值不为1111 0000
				{
					if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f)
					{
						b = GPIO_ReadInputData(GPIOF)&0xff;			 //键值放入暂存变量b
					}
				}
				a = (~a)|(~b);//将两个数据相或	
			switch(a)	    //对比数据值
			{
				case 0xee: b = 16; break;		//对比得到的键值给b键值编号
				case 0xed: b = 15; break;
				case 0xeb: b = 14; break;
				case 0xe7: b = 13; break;
				case 0xde: b = 12; break;
				case 0xdd: b = 11; break;
				case 0xdb: b = 10; break;
				case 0xd7: b = 9 ; break;
				case 0xbe: b = 8 ; break;
				case 0xbd: b = 7 ; break;
				case 0xbb: b = 6 ; break;
				case 0xb7: b = 5 ; break;
				case 0x7e: b = 4 ; break;   //4 关闭风扇
				case 0x7d: b = 3 ; break;		//3 开启风扇																																																																																																																																																														
				case 0x7b: b = 2 ; break;		//2 关闭加湿器
				case 0x77: b = 1 ; break;		//1 开启加湿器
				default:   b = 0 ; break;   //键值错误处理，键值编号0
			}
			while((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0)//等待按键松开,超时自动松开
			{
				t++;
				if(t>10){ t=0;break;}			
			}
		}
  return (b);//返回键值编号
} 
/***************************************************************************
	*函数名称：keys_send
	*函数功能：功能值通过usart发送
	*入口参数：key 
	*返回参数：null
	*备注：                         
****************************************************************************/
void keys_send(u8 key)
{	

	switch(key)
		{
			case 1:	
					switch(upper_monitor_flag)
						{
						  case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("1\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("1\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u2_printf("1\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("1\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("1\r\n");break ;
							default: break ;													
						}						
        break;
			case 2:
						switch(upper_monitor_flag)
						{
						  case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("2\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("2\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u2_printf("2\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("2\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("2\r\n");break ;
							default: break ;													
						}
				break;
			case 3:
						switch(upper_monitor_flag)
						{
						 		
							case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("3\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("3\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("3\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("3\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("3\r\n");break ;
							default: break ;											
						}
				break;
			case 4:
						switch(upper_monitor_flag)
						{
						 		
							case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("4\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("4\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("4\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("4\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("4\r\n");break ;
							default: break ;											
						}
				break;
			case 5:
				break;
			case 6:
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

