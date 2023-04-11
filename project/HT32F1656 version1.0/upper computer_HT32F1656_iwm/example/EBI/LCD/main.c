/* ---------------包含头文件--------------------------------------------*/
#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ebi_lcd.h"
#include "usart.h"
#include "timer.h"
#include "exti.h"
#include "max30102.h"
#include "algorithm.h"
#include "myiic.h"
#include "sht3x.h"
#include "ESP8266.h"
#include "wdt.h"
#include "light_voice.h"
#include "matrix.h"
/* -----------------上位机引脚配置说明----------------------------------	
工程细节详见Doc—>readme.txt
-----------------------------------------------------------------------*/
/* ---------------自定义参数宏------------------------------------------*/
#define NVIC_PRIORITY_GROUP_3          3 /* 抢占优先级：Preemption: 4 bits / 响应优先级：Subpriority: 0 bits */

/*------------------------------------------------------------------------------
	函数名：main主函数
	输入输出：无
	返回值：无
*------------------------------------------------------------------------------*/
int main(void)
{   
/*----------------------配置NVIC优先级------------------------------------------*/

	NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_3);		//中断优先级分组
	NVIC_SetPriority(USART0_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 1, 0));
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 0, 0));

/*----------------------初始化函数----------------------------------------------*/

	usart0_Init(115200);			//串口0初始化
	usart1_Init(115200);			//串口1(wifi)初始化
	delay_init();							//延时函数初始化
	LCD_Init();								//LCD初始化			
	screen_init_background(); //背景初始化
	WDT_Init();               //看门狗初始化
	BEEP_Init();							//蜂鸣器初始化
	matrix_KEY_Init();				//矩阵键盘初始化
	ESP8266_Init();           //ESP8266-WIFI初始化
	
/*--------------进入无限循环体--------------------------------------------*/	
	while(1)
	  
/*--------------病房主程序循环体代码---------------------------------------*/	
		{
			key_get();
		}	
		
}


