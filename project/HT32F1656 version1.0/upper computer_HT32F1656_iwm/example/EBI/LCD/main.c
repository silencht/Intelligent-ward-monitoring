/* ---------------����ͷ�ļ�--------------------------------------------*/
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
/* -----------------��λ����������˵��----------------------------------	
����ϸ�����Doc��>readme.txt
-----------------------------------------------------------------------*/
/* ---------------�Զ��������------------------------------------------*/
#define NVIC_PRIORITY_GROUP_3          3 /* ��ռ���ȼ���Preemption: 4 bits / ��Ӧ���ȼ���Subpriority: 0 bits */

/*------------------------------------------------------------------------------
	��������main������
	�����������
	����ֵ����
*------------------------------------------------------------------------------*/
int main(void)
{   
/*----------------------����NVIC���ȼ�------------------------------------------*/

	NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_3);		//�ж����ȼ�����
	NVIC_SetPriority(USART0_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 1, 0));
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 0, 0));

/*----------------------��ʼ������----------------------------------------------*/

	usart0_Init(115200);			//����0��ʼ��
	usart1_Init(115200);			//����1(wifi)��ʼ��
	delay_init();							//��ʱ������ʼ��
	LCD_Init();								//LCD��ʼ��			
	screen_init_background(); //������ʼ��
	WDT_Init();               //���Ź���ʼ��
	BEEP_Init();							//��������ʼ��
	matrix_KEY_Init();				//������̳�ʼ��
	ESP8266_Init();           //ESP8266-WIFI��ʼ��
	
/*--------------��������ѭ����--------------------------------------------*/	
	while(1)
	  
/*--------------����������ѭ�������---------------------------------------*/	
		{
			key_get();
		}	
		
}


