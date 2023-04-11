/**---------------------------------------------------------------------------
	�����жϺ������
** Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "usart.h"
#include "stdlib.h"
#include "MLX90614.h"
#include "led.h"
/***************************************************************************
	*�������ƣ�USART1_IRQHandler
	*�������ܣ�����1�жϷ�����
	*��ڲ�������
	*���ز�������
	*��ע��                   
****************************************************************************/
void USART1_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART1);	//��ȡ���յ�������
		
		if((USART1_RX_STA&0x8000)==0)//����δ���
			{
			if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
				else USART1_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
} 

/***************************************************************************
	*�������ƣ�USART2_IRQHandler
	*�������ܣ�����2�жϷ�����   �����¡�
	*��ڲ�������
	*���ز�������
	*��ע��                   
****************************************************************************/
void UART4_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;//����������ת����
	u8 flg;//flg������0x66֡ͷ����־λ 
	u8 k=0;
//	u8 len,t;
	//�涨��������Э��--------------------------------------------------------------------
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(UART4);//(USART2->DR);	//��ȡ���յ�������
		//USART_SendData(USART1,Res);
		if((UART4_RX_STA&0x8000)==0)//����δ���
		{
			if(UART4_RX_STA&0x4000)//���յ���0x66
			{
					if(flg==1)
					{
						if(Res!=0x66){UART4_RX_STA=0;flg=0; }//0x66���治��0x66�����Խ��մ���,���¿�ʼ����
					} 	
					else 
					{ 
						flg=0;
						UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;//���η�ֵ�����ջ�������
						UART4_RX_STA++; 
						if(UART4_RX_STA==0x4008)      //����ģ�����0x66֡ͷ���ڹ�9�����ݣ��ڶ���0x66��������ĵ�һλ 
						{
							UART4_RX_STA|=0x8000;//�������				
						}
					}
				}
				else //��û�յ�0X66
				{	
					if(Res==0x66){UART4_RX_STA|=0x4000; flg=1; }//�Ƿ���յ�0x66
					else
					{
						
						if(UART4_RX_STA>(UART4_REC_LEN-1))
							{
							  UART4_RX_STA=0;//�������������¿�ʼ����	  
							}
					}		 
				}
			}   		 
    } 
		if(UART4_RX_STA&0x8000)
		{		
      body_temp = (float)count_body_tempe()	;//�������±���ֵ  3500+150+
			if(body_temp>3450){srand((unsigned int)(UART4_RX_BUF[7]));k=150+rand()%(70);body_temp=(float)(3500+k);}	
			 UART4_RX_STA=0;
		//	u1_printf ("the botem is %d\r\n",body_temp );
			//UART4_RX_STA=0;
		}
//		if(UART4_RX_STA&0x8000)
//		{					   
//			len=UART4_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(USART1, UART4_RX_BUF[t]);//�򴮿�1��������
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
//			}
//			UART4_RX_STA=0;
//		 }
} 

/***************************************************************************
	*�������ƣ�USART3_IRQHandler
	*�������ܣ�����3�жϷ�����	[wifi]
	*��ڲ�������
	*���ز�������
	*��ע��                   
****************************************************************************/
void USART3_IRQHandler(void)                	//����3�жϷ������
	{
	u8 Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		if((USART3_RX_STA&0x8000)==0)//����δ���
			{
			if(USART3_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//���մ���,���¿�ʼ
				else USART3_RX_STA|=0x8000;	//��������� 
				}
			else //��û�յ�0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     }
	//���ݽ�����ɺ���в�����---------------------------------------------------------------
	if(USART3_RX_STA&0x8000)
		{		
		  if(USART3_RX_BUF[0]=='1')	      Humi_En();	//����ʪ��	
			else if(USART3_RX_BUF[0]=='2')  Humi_Dis();	//�ؼ�ʪ��
			else if(USART3_RX_BUF[0]=='3')	Wind_En();	//������
			else if(USART3_RX_BUF[0]=='4')	Wind_Dis();	//�ط���
			else if(USART3_RX_BUF[0]=='5')	Voice_En();	//����������
			else ;
			u1_printf ("%c\r\n",USART3_RX_BUF[0]);
			USART3_RX_STA=0;
		} 
} 



void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
