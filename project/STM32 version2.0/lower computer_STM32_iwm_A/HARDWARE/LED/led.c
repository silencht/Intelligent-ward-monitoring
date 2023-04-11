#include "led.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//LED 系统LED初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PB,PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;				
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.6
 GPIO_ResetBits(GPIOE,GPIO_Pin_6);						 //PE.6 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	    		 //LED1-->PE.5 端口配置, 推挽输出
 GPIO_Init(GPIOE, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
 GPIO_ResetBits(GPIOE,GPIO_Pin_5); 						 //PE.5 输出高 
}

//室内照明
void Light4_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //使能PF端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;  //PF2-3-4-5			
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //Opendrain输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOF
 
 GPIO_SetBits(GPIOF,GPIO_Pin_2);  //High 2-3-4-5
 GPIO_SetBits(GPIOF,GPIO_Pin_3);
 GPIO_SetBits(GPIOF,GPIO_Pin_4); 
 GPIO_SetBits(GPIOF,GPIO_Pin_5);  
}
//this fuction is to open and close Led ,its first param is Led num(must be 1~4),and the last param is your open or close's willing
void En_Ledx(u8 x,FlagStatus BitVal)
{
	 switch(x)
	 { 
		case 1:	 
				if(BitVal!=Bit_SET){	GPIOF->BSRR = GPIO_Pin_2;	}
				else GPIOF->BRR = GPIO_Pin_2; 
				break; 
				
	  case 2:	
				if(BitVal!=Bit_SET){	GPIOF->BSRR = GPIO_Pin_3;	}
				else GPIOF->BRR = GPIO_Pin_3; 
				break;
		case 3: 
				if(BitVal!=Bit_SET){	GPIOF->BSRR = GPIO_Pin_4;	}
				else GPIOF->BRR = GPIO_Pin_4; 
				break;
		case 4:
				if(BitVal!=Bit_SET){	GPIOF->BSRR = GPIO_Pin_5;	}
				else GPIOF->BRR = GPIO_Pin_5; 
				break;	
		default: break; 
} 
  
}

void Beep_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //使能PF端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;  //PF7			
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; 		 //Opendrain输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOF
 
 GPIO_SetBits(GPIOF,GPIO_Pin_7);  //High 7
 
}

void Beep_1s(void)
{
	GPIO_ResetBits(GPIOF,GPIO_Pin_7);  //Low 7
	delay_ms(800);
  GPIO_SetBits(GPIOF,GPIO_Pin_7);    //High 7
}

void Voice_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //使能PF端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;  //PF6			
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOF
 
 GPIO_ResetBits(GPIOF,GPIO_Pin_6);  //High 6
 
}
//occur a rising edge to enable voice 
void Voice_En(void)
{
	GPIO_SetBits(GPIOF,GPIO_Pin_6);    //High 6
	delay_ms(10);
	GPIO_ResetBits(GPIOF,GPIO_Pin_6);  //High 6
}

void Humi_Wind_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE); //使能PF端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;  //PF8-9	
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOF, &GPIO_InitStructure);					 //根据设定参数初始化GPIOF
 
 GPIO_ResetBits(GPIOF,GPIO_Pin_8|GPIO_Pin_9);  //Low 8-9
}

void Humi_En(void)
{
	GPIO_SetBits(GPIOF,GPIO_Pin_8);  //High8
}
void Humi_Dis(void)
{
	GPIO_ResetBits(GPIOF,GPIO_Pin_8); //Low8
}
void Wind_En(void)
{
	GPIO_SetBits(GPIOF,GPIO_Pin_9);  //High 9
}
void Wind_Dis(void)
{
	GPIO_ResetBits(GPIOF,GPIO_Pin_9);  //Low9
}

