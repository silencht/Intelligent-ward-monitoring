#include "sys.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 	  
 
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

u8  wifi_init_end_flag = 0;//wifi��ʼ��������ɱ�־����ʼ��δ���Ϊ0�����Ϊ1�������ж������жϷ��������Ƿ����USART1_RX_STA��
float body_temp=0;	
/***************************************************************************
	*���ܣ��������
	*��ע��                
****************************************************************************/  
   	
u8 USART1_RX_BUF[USART1_REC_LEN];  //���ջ���,���USART1_REC_LEN���ֽ�.
u8 UART4_RX_BUF[UART4_REC_LEN];  //���ջ���,���UART4_REC_LEN���ֽ�. 
u8 USART3_RX_BUF[USART3_REC_LEN];  //���ջ���,���UART4_REC_LEN���ֽ�.
//u8 UART4_RX_BUF[UART4_REC_LEN];    //���ջ���,���UART4_REC_LEN���ֽ�.


u16 USART1_RX_STA=0; 			//����1����״̬���	
u16 UART4_RX_STA=0; 			//����2����״̬���	
u16 USART3_RX_STA=0; 			//����2����״̬���
//u16 UART4_RX_STA=0;       //����4����״̬���	

/***************************************************************************
	*�������ƣ�usart1_init
	*�������ܣ���ʼ��IO ����1
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOA9����TX   GPIOA10����RX                     
****************************************************************************/
void usart1_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
	
}
/***************************************************************************
	*�������ƣ�uart4_init
	*�������ܣ���ʼ��IO ����4
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOA2����TX   GPIOA3����RX                     
****************************************************************************/
void uart4_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //ʹ��USART2ʱ��
  
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART2_RX	  GPIOA.3��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOA.3 

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //��������2���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(UART4, &USART_InitStructure); //��ʼ������4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(UART4, ENABLE);                    //ʹ�ܴ���4
	
}
/***************************************************************************
	*�������ƣ�usart3_init
	*�������ܣ���ʼ��IO ����3
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOB10����TX   GPIOB11����RX                     
****************************************************************************/
void usart3_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //ʹ��USART3ʱ��
  
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
  //USART3_RX	  GPIOB.11��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.3 

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //��������2���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������2
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���2
	
}


/***************************************************************************
	*�������ƣ�u1_printf
	*�������ܣ�����1��ӡ����
	*��ڲ������ο���׼C printf����
	*���ز�������
	*��ע��                   
****************************************************************************/
void u1_printf (char *fmt, ...) 
{ 
	char buffer[200+1];  // ���ͻ������鳤���Լ����� 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 200+1, fmt, arg_ptr); 
	while ((i < 200) && buffer[i]) 
		{ 
		  USART1->SR; //��ֹ���ַ���ʧ
			USART_SendData(USART1, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*�������ƣ�u4_printf
	*�������ܣ�����2��ӡ����
	*��ڲ������ο���׼C printf����
	*���ز�������
	*��ע��                   
****************************************************************************/
void u4_printf (char *fmt, ...) 
{ 
	char buffer[50+1];  // ���ͻ������鳤���Լ����� 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 50+1, fmt, arg_ptr); 
	while ((i < 50) && buffer[i]) 
		{ 
		  UART4->SR; //��ֹ���ַ���ʧ
			USART_SendData(UART4, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*�������ƣ�u4_printf
	*�������ܣ�����2��ӡ����
	*��ڲ������ο���׼C printf����
	*���ز�������
	*��ע��                   
****************************************************************************/
void u3_printf (char *fmt, ...) 
{ 
	char buffer[200+1];  // ���ͻ������鳤���Լ����� 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 200+1, fmt, arg_ptr); 
	while ((i < 200) && buffer[i]) 
		{ 
		  USART3->SR; //��ֹ���ַ���ʧ
			USART_SendData(USART3, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}



 



