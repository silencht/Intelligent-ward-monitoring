#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
#include "delay.h"
#include "stm32f10x_gpio.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define LED0 PBout(5)// PB5
#define LED1 PEout(5)// PE5	

void LED_Init(void);//��ʼ��
void Light4_Init(void);
void En_Ledx(u8 x,FlagStatus BitVal);
void Beep_Init(void);
void Beep_1s(void);
void Voice_Init(void);
void Voice_En(void);
void Humi_Wind_Init(void);
void Humi_En(void);
void Humi_Dis(void);
void Wind_En(void);
void Wind_Dis(void);


		 				    
#endif
