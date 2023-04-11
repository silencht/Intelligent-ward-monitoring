/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h" 
#include "silencht_usart.h"	
#include <cstring>
#include "ESP8266.h"
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
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
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
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
/***************************************************************************
	*�������ƣ�USART2_IRQHandler
	*�������ܣ�����2�жϷ�����
	*��ڲ�������
	*���ز�������
	*��ע��    WIFI�жϽ��� 
	* ��λ��A ������0x0d��\r����0x0a��\n��  ��λ��B ������ 0x21��!����0x23��#��
****************************************************************************/
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 Res;

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a /0x21 0x23��β)
	{ 
		  Res =USART_ReceiveData(USART2);//(USART2->DR);	//��ȡ���յ�������
		  USART_SendData(USART1 ,Res);  
			if((USART2_RX_STA&0x8000)==0)//����δ���
			{
				if(USART2_A_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a){	USART2_RX_STA=0;	USART2_A_STA=0;	} //���մ���,���¿�ʼ
					else {	USART2_RX_STA|=0x8000;	USART2_A_STA=USART2_RX_STA;}//����1�������� }//A���������� 
				}
				else if(USART2_B_STA&0x4000) //���յ���!(0x21)
				{
					if(Res!=0x23){	USART2_RX_STA=0;	USART2_B_STA=0;	} //���մ���,���¿�ʼ
					else {	USART2_RX_STA|=0x8000;	USART2_B_STA=USART2_RX_STA;USART2_A_STA=0;}	
//����1��������}//B���������ˡ�������Ҫ�������������ˣ�Ѫ����Ľ�ѵ������WIFIÿ�ν��ջ��Զ�����һ������[0x0d 0x0a]��
//���Ϊ�˱���A�����ݶ�ȡB���־��ڣ�Ҫ��B�������֮�󣬱��룡��ǰɱ���������л��з���λ��A���־λ����USART2_A_STA=0;
				}			
				else //���A\B���������û�н��յ�
				{					 
					if(Res==0x0d){USART2_A_STA|=0x4000;}
					else if(Res==0x21){	USART2_B_STA|=0x4000;}
					else
					{
						USART2_RX_BUF[USART2_RX_STA&0X3FFF]=Res;
						USART2_RX_STA++;
						if(USART2_RX_STA>(USART2_REC_LEN-1)){	USART2_RX_STA=0;USART2_A_STA=0;USART2_B_STA=0;}//�������ݴ���,���¿�ʼ����	  
					}				
				 }
			}	 
   }  
	 
//�ѽ�����һ������----------------------------------------------------------	
		if(USART2_RX_STA&0x8000)  
		{
			if(USART2_A_STA&0x8000) //����������A�� 0X8000
			{
				memcpy(USART2_A_BUF,USART2_RX_BUF,sizeof(USART2_RX_BUF)); //�����յ�����ת�Ƶ������黺����
				memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF)); //����ܽ��ջ�����
				USART2_RX_STA=0; //����ܱ�־λ���Ա������һ������
			}
		  else if(USART2_B_STA&0x8000) //����������B�� 0X8020
			{
				memcpy(USART2_B_BUF,USART2_RX_BUF,sizeof(USART2_RX_BUF)); //�����յ�����ת�Ƶ������黺����
				memset(USART2_RX_BUF,0,sizeof(USART2_RX_BUF));
				USART2_RX_STA=0;
			}	
		 }	

} 
/***************************************************************************
	*�������ƣ�UART4_IRQHandler
	*�������ܣ�����4�жϷ�����
	*��ڲ�������
	*���ز�������
	*��ע��                   
****************************************************************************/
void UART4_IRQHandler(void)                	//����4�жϷ������
{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res =USART_ReceiveData(UART4);//(UART4->DR);	//��ȡ���յ�������
		
		if((UART4_RX_STA&0x8000)==0)//����δ���
		{
			if(UART4_RX_STA&0x4000)//���յ���0x0d
			{
				if(Res!=0x0a)UART4_RX_STA=0;//���մ���,���¿�ʼ
				else UART4_RX_STA|=0x8000;	//��������� 
			}
			else //��û�յ�0X0D
			{	
				if(Res==0x0d)UART4_RX_STA|=0x4000;
				else
				{
					UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;
					UART4_RX_STA++;
					if(UART4_RX_STA>(UART4_REC_LEN-1))UART4_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}   		 
  } 
#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
}





/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
 
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
