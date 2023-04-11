#include "exti.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "usart.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//外部中断 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   
//外部中断0服务程序

u8 call_flag=1;

void EXTIX_Init(void)
{
 
 	  EXTI_InitTypeDef EXTI_InitStructure;
 	  NVIC_InitTypeDef NVIC_InitStructure;

    EXTI_GPIO_Init();	 //	PC0按键端口初始化

  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟

    //GPIOC.0 中断线以及中断初始化配置   下降沿触发
  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource0);

  	EXTI_InitStructure.EXTI_Line=EXTI_Line0;	//KEY2
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


  	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;	//抢占优先级0， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;					//子优先级0
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure); 
 
}

//外部中断0服务程序 
void EXTI0_IRQHandler(void)
{
	delay_ms(5);
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1)	 	 //not press call key
	{		
    delay_ms(30);//消抖	
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==1)	 
		{
			call_flag=1;//printf ("1");
		}	
	}
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==0)	 	 //press call key
	{		
	  delay_ms(30);//消抖	
		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0)==0)	
		{
			call_flag=0;//printf ("0");
		}	
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0); //清除LINE0上的中断标志位  
}
