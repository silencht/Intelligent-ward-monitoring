#include "exti.h"
#include "delay.h" 
#include "stm32f4xx_exti.h"
#include "silencht_usart.h"
#include "ESP8266.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

u8 upper_monitorA_flag;
u8 upper_monitorB_flag;

static unsigned char humi_mask=1,wind_mask=3;
//�ⲿ�ж�0������� PF0
void EXTI0_IRQHandler(void)
{
	delay_ms(10);	//����
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
	 EXTI_ClearITPendingBit(EXTI_Line0); //���LINE10�ϵ��жϱ�־λ 
}	
//�ⲿ�ж�1������� PF1
void EXTI1_IRQHandler(void)
{
	delay_ms(10);	//����
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
	 EXTI_ClearITPendingBit(EXTI_Line1);//���LINE1�ϵ��жϱ�־λ 
}
//�ⲿ�ж�1������� PF2
void EXTI2_IRQHandler(void)
{
	delay_ms(10);	//����
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_2)==0) //faliing
		{
		//	u1_printf ("2 faliing��%d",voice_mask);
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
	 EXTI_ClearITPendingBit(EXTI_Line2);//���LINE1�ϵ��жϱ�־λ 
}
//�ⲿ�ж�1������� PF3
void EXTI3_IRQHandler(void)
{
	delay_ms(10);	//����
	if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_3)==0) //faliing
		{
		//	u1_printf ("2 faliing��%d",voice_mask);
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
	 EXTI_ClearITPendingBit(EXTI_Line3);//���LINE1�ϵ��жϱ�־λ 
}
	   
//�ⲿ�жϳ�ʼ������
//�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	KEY_Init(); //������Ӧ��IO�ڳ�ʼ��
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//ʹ��SYSCFGʱ��
	
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource0);//PF0 ���ӵ��ж���0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource1);//PF1 ���ӵ��ж���1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource2);//PF2 ���ӵ��ж���2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOF, EXTI_PinSource3);//PF2 ���ӵ��ж���2
	
  /* ����EXTI_Line0 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;//LINE0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //            
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//ʹ��LINE9
  EXTI_Init(&EXTI_InitStructure);//����
	
	/* ����EXTI_Line1 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	/* ����EXTI_Line2 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
	/* ����EXTI_Line3 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line3;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//�ж��¼�
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;//�ж���ʹ��
  EXTI_Init(&EXTI_InitStructure);//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;//�ⲿ0
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;//�ⲿ�ж�1
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;//�ⲿ�ж�2
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;//�����ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);//����
	

	   
}

//������ʼ������ GPIOF0/F1/F2
void KEY_Init(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //��ʪ���ͷ��ȶ�Ӧ����,voice
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);//��ʼ��GPIOF0123
} 










