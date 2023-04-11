#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 

#define USART1_REC_LEN  		200  	//�����������ֽ��� 200
#define UART4_REC_LEN  		20  	//�����������ֽ��� 200
#define USART3_REC_LEN  		200  	//�����������ֽ��� 200
//#define UART4_REC_LEN  			200  	//�����������ֽ��� 200
extern u8  wifi_init_end_flag;   				//wifi��ʼ��������ɱ�־����ʼ��δ���Ϊ0�����Ϊ1�������ж������жϷ��������Ƿ����USART1_RX_STA��
extern u8  close_led;
extern float body_temp;


extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART1_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�	
extern u8  UART4_RX_BUF[UART4_REC_LEN]; //���ջ���,���USART1_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //���ջ���,���USART1_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�	  	
//extern u8  UART4_RX_BUF[UART4_REC_LEN]; //���ջ���,���USART1_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

//����״̬��bit15��	������ɱ�־��bit14�����յ�0x0d��bit13~0��	���յ�����Ч�ֽ���Ŀ
extern u16 USART1_RX_STA;         //����1����״̬���	
extern u16 UART4_RX_STA;         //����1����״̬���
extern u16 USART3_RX_STA;         //����1����״̬���
//extern u16 UART4_RX_STA;      		//����4����״̬���	


void usart1_init(u32 bound);
void uart4_init(u32 bound);
void usart3_init(u32 bound);
//void uart4_init(u32 bound);

void u1_printf (char *fmt, ...) ;
void u4_printf (char *fmt, ...) ;
void u3_printf (char *fmt, ...) ;
//void u4_printf (char *fmt, ...) ;

#endif

