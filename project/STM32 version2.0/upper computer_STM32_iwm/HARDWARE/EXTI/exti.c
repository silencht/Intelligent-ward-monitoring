#include "exti.h"
#include "delay.h" 
#include "stm32f4xx_exti.h"
#include "silencht_usart.h"
#include "ESP8266.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/4
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

u8 upper_monitorA_flag;
u8 upper_monitorB_flag;

static unsigned char humi_mask=1,wind_mask=3;
//外部中断0服务程序 PF0
void EXTI0_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_0)==0) //falling
	{
	  u1_printf("%d\r\n",humi_mask);
		switch(upper_monitorB_flag)
			{
				case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("%d\r\n",humi_mask);LCD_Fill(6,57,18,72,RED);break ;
				case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("%d\r\n",humi_mask);LCD_Fill(6,57,18,72,RED);break ;
				case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u2_printf("%d\r\n",humi_mask);LCD_Fill(6,57,18,72,RED);break ;
				case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("%d\r\n",humi_mask);LCD_Fill(6,57,18,72,RED);break ;
				case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("%d\r\n",humi_mask);LCD_Fill(6,57,18,72,RED);break ;
				default: break ;													
			}
		if(humi_mask==1)humi_mask=2;
		else {humi_mask=1;LCD_Fill(6,57,18,72,WHITE);}
		
	}
	 EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE10上的中断标志位 
}	
//外部中断1服务程序 PF1
void EXTI1_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_1)==0) //faliing
		{
			u1_printf("%d\r\n",wind_mask);
			switch(upper_monitorB_flag)
				{
						
					case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("%d\r\n",wind_mask);LCD_Fill(222,57,234,72,RED);break ;
					case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("%d\r\n",wind_mask);LCD_Fill(222,57,234,72,RED);break ;
					case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("%d\r\n",wind_mask);LCD_Fill(222,57,234,72,RED);break ;
					case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("%d\r\n",wind_mask);LCD_Fill(222,57,234,72,RED);break ;
					case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("%d\r\n",wind_mask);LCD_Fill(222,57,234,72,RED);break ;
					default: break ;											
				}
		if(wind_mask==3)wind_mask=4;
		else {wind_mask=3;LCD_Fill(222,57,234,72,WHITE);}
		}
	 EXTI_ClearITPendingBit(EXTI_Line1);//清除LINE1上的中断标志位 
}
//外部中断1服务程序 PF2
void EXTI2_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)==0) //faliing
		{
		//	u1_printf ("2 faliing，%d",voice_mask);
			switch(upper_monitorB_flag)
				{
						
					case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("5\r\n");break ;
					case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("5\r\n");break ;
					case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("5\r\n");break ;
					case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("5\r\n");break ;
					case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("5\r\n");break ;
					default: break ;											
				}
		}
	 EXTI_ClearITPendingBit(EXTI_Line2);//清除LINE1上的中断标志位 
}
//外部中断1服务程序 PF3
void EXTI3_IRQHandler(void)
{
	delay_ms(10);	//消抖
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)==0) //faliing
		{
		//	u1_printf ("2 faliing，%d",voice_mask);
			switch(upper_monitorA_flag)
				{
						
					case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("6\r\n");break ;
					case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("6\r\n");break ;
					case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("6\r\n");break ;
					case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("6\r\n");break ;
					case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("6\r\n");break ;
					default: break ;											
				}
		}
	 EXTI_ClearITPendingBit(EXTI_Line3);//清除LINE1上的中断标志位 
}
	   
//外部中断初始化程序
//中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //按键对应的IO口初始化
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//使能SYSCFG时钟
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource0);//PF0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource1);//PF1 连接到中断线1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource2);//PF2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource3);//PF2 连接到中断线2
	
  /* 配置EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //            
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能LINE9
  EXTI_Init(&EXTI_InitStructure);//配置
	
	/* 配置EXTI_Line1 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
	/* 配置EXTI_Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
	/* 配置EXTI_Line3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断事件
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//中断线使能
  EXTI_Init(&EXTI_InitStructure);//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//外部0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//外部中断1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//外部中断2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//子优先级2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//使能外部中断通道
  NVIC_Init(&NVIC_InitStructure);//配置
	

	   
}

//按键初始化函数 GPIOF0/F1/F2
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //加湿器和风扇对应引脚,voice
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);//初始化GPIOF0123
} 










