#include "ESP8266.h"
#include "string.h"
 
void ESP8266_Init(void)
{ 
  POINT_COLOR=RED;
  while(atk_8266_send_cmd("AT","OK",20)){ LCD_ShowString(19,57,200,16,16,"wifi is no online");	}	
	delay_ms(20);
	u2_printf("ATE0\r\n");    //关闭回显
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPMUX=1","OK",50)){ LCD_ShowString(19,57,200,16,166,"cipmux wait");	}			            //设置上位机 ESP8266-WIFI启动多连接模式	
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPSERVER=1,8086","OK",50)){ LCD_ShowString(19,57,200,16,16,"tcpserver wait");	}			//设置上位机 ESP8266-WIFI开启TCP SERVER
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPSTO=6400","OK",50)){ LCD_ShowString(19,57,200,16,16,"overtime wait");}	//设置上位机 ESP8266-WIFI超时秒数模式
	delay_ms(20);
	POINT_COLOR=LIGHTGREEN;
	LCD_ShowString(15,57,200,16,16,"  esp8266 init successful");
	POINT_COLOR=BLUE;
	memset(USART2_A_BUF,0,sizeof(USART2_A_BUF));
	USART2_A_STA=0;USART2_RX_STA=0; //清空所有标志位
}
//向ATK-ESP8266发送命令
//cmd:发送的命令字符串
//ack:期待的应答结果,如果为空,则表示不需要等待应答
//waittime:等待时间(单位:10ms)
//返回值:0,发送成功(得到了期待的应答结果)
//       1,发送失败
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART2_A_STA=0;
	u2_printf("%s\r\n",cmd);	//串口1发送命令
	if(ack&&waittime)		//需要等待应答
	{
		while(--waittime)	//等待倒计时
		{
			delay_ms(50);
			
			if(USART2_A_STA&0X8000)//接收到期待的应答结果
			{
				if(atk_8266_check_cmd(ack))
				{
				  //u0_printf("ack:%s",(uint8_t*)ack);
					break;//得到有效数据 
				}
				USART2_A_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//ATK-ESP8266发送命令后,检测接收到的应答
//str:期待的应答结果
//返回值:0,没有得到期待的应答结果
//    其他,期待应答结果的位置(str的位置)
uint8_t atk_8266_check_cmd(uint8_t *str)
{
	
	uint8_t strx=0;
	if(USART2_A_STA&0X8000)		//接收到一次数据了
	{ 
		USART2_A_BUF[USART2_RX_STA&0X3FFF]=0;//添加结束符   B: 0011 
		strx=strcmp((const char*)USART2_A_BUF,(const char*)str);
	} 
	return  strx;
}
