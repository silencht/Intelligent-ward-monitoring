#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
	
#define USART1_REC_LEN  		200  	//�����������ֽ��� 200
#define USART2_REC_LEN  		200  	//�����������ֽ��� 200
#define UART4_REC_LEN  			200  	//�����������ֽ��� 200

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�	
extern u8  USART2_RX_BUF[USART2_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з�	  	
extern u8  UART4_RX_BUF[UART4_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 

extern u8 USART2_A_BUF[USART2_REC_LEN];   //��λ��A����ջ���,���USART2_REC_LEN���ֽ�. 
extern u8 USART2_B_BUF[USART2_REC_LEN]; 	//��λ��B����ջ���,���USART2_REC_LEN���ֽ�. 

//����״̬��bit15��	������ɱ�־��bit14�����յ�0x0d��bit13~0��	���յ�����Ч�ֽ���Ŀ
extern u16 USART1_RX_STA;         //����1����״̬���	
extern u16 USART2_RX_STA;         //����1����״̬���
extern u16 UART4_RX_STA;      		//����4����״̬���	

extern u16 USART2_A_STA; //���յ���λ��A��־λ
extern u16 USART2_B_STA; //���յ���λ��B��־λ

void usart1_init(u32 bound);
void usart2_init(u32 bound);
void uart4_init(u32 bound);

void u1_printf (char *fmt, ...) ;
void u2_printf (char *fmt, ...) ;
void u4_printf (char *fmt, ...) ;

#endif


