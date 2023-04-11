#include "sys.h"
#include "silencht_usart.h"	
#include <stdio.h>
#include <stdarg.h>
#include <cstring>
	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

/***************************************************************************
	*���ܣ�֧��printf����,������Ҫѡ��use MicroLIB
	*��ע��                   
****************************************************************************/  
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
/***************************************************************************
	*���ܣ��������
	*��ע��                
****************************************************************************/  
   	
u8 USART1_RX_BUF[USART1_REC_LEN];  //���ջ���,���USART1_REC_LEN���ֽ�.
u8 USART2_RX_BUF[USART2_REC_LEN];  //���ջ���,���USART2_REC_LEN���ֽ�. 
u8 UART4_RX_BUF[UART4_REC_LEN];    //���ջ���,���UART4_REC_LEN���ֽ�.

u8 USART2_A_BUF[USART2_REC_LEN];   //��λ��A����ջ���,���USART2_REC_LEN���ֽ�. 
u8 USART2_B_BUF[USART2_REC_LEN]; 	 //��λ��B����ջ���,���USART2_REC_LEN���ֽ�. 

u16 USART1_RX_STA=0; 			//����1����״̬���	
u16 USART2_RX_STA=0; 			//����2����״̬���	
u16 UART4_RX_STA=0;       //����4����״̬���	

u16 USART2_A_STA=0; //���յ���λ��A/B��־λ
u16 USART2_B_STA=0; //���յ���λ��A/B��־λ
/***************************************************************************
	*�������ƣ�usart1_init
	*�������ܣ���ʼ��IO ����1
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOA9����TX   GPIOA10����RX                     
****************************************************************************/
void usart1_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);//ʹ��USART1ʱ��
 
	//����1��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1); //GPIOA9����ΪUSART1
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1); //GPIOA10����ΪUSART1
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //GPIOA9��GPIOA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA9��PA10

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1 
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//��������ж�
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
}
/***************************************************************************
	*�������ƣ�usart2_init
	*�������ܣ���ʼ��IO ����2
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOA2����TX   GPIOA3����RX                     
****************************************************************************/
void usart2_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
 
	//����2��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART1); //GPIOA2����ΪUSART2--TX
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART1); //GPIOA3����ΪUSART2--RX
	
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��PA2��PA3

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	
  USART_Cmd(USART2, ENABLE);  //ʹ�ܴ���2 
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//��������ж�
	//Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�0   ��Ϊ������ȼ�����֤�������Ƚ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	
}

/***************************************************************************
	*�������ƣ�uart4_init
	*�������ܣ���ʼ��IO ����4 
	*��ڲ�����bound:������
	*���ز�������
	*��ע�� GPIOC10����TX   GPIOC11����RX                     
****************************************************************************/
void uart4_init(u32 bound){
  GPIO_InitTypeDef GPIO_InitStructure;   //GPIO�˿����ýṹ��
	USART_InitTypeDef USART_InitStructure; //USART���ýṹ��
	NVIC_InitTypeDef NVIC_InitStructure;	 //�ж����ȼ����ýṹ��
	USART_DeInit(UART4);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); 		 //ʹ��GPIOCʱ��   	C10 TX 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);		 //ʹ��UART4ʱ��    C11 RX
 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);  //GPIOC10����ΪUART4_TX
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4);  //GPIOC11����ΪUART4_RX
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;  //GPIOC10��GPIOC11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;					   //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				 //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 					 //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 						 //����
	GPIO_Init(GPIOC,&GPIO_InitStructure); 									 //��ʼ��PC10/PC11

	USART_InitStructure.USART_BaudRate = bound;							 //����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;     //һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;				 //����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART4, &USART_InitStructure);                //��ʼ������4
	
  USART_Cmd(UART4, ENABLE);  //ʹ�ܴ���4	
	
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);					 //��������4�����ж�

  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;				 //����4�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;  //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		     //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			     //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	                         //����ָ���Ĳ�����ʼ��NVIC�Ĵ���
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
	char buffer[50+1];  // ���ͻ������鳤���Լ����� 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 50+1, fmt, arg_ptr); 
	while ((i < 50) && buffer[i]) 
		{ 
		  USART1->SR; //��ֹ���ַ���ʧ
			USART_SendData(USART1, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*�������ƣ�u2_printf
	*�������ܣ�����2��ӡ����
	*��ڲ������ο���׼C printf����
	*���ز�������
	*��ע��                   
****************************************************************************/
void u2_printf (char *fmt, ...) 
{ 
	char buffer[50+1];  // ���ͻ������鳤���Լ����� 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 50+1, fmt, arg_ptr); 
	while ((i < 50) && buffer[i]) 
		{ 
		  USART2->SR; //��ֹ���ַ���ʧ
			USART_SendData(USART2, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*�������ƣ�u4_printf
	*�������ܣ�����4��ӡ����
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




