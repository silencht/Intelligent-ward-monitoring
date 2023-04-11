#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 

#define USART1_REC_LEN  		200  	//定义最大接收字节数 200
#define UART4_REC_LEN  		20  	//定义最大接收字节数 200
#define USART3_REC_LEN  		200  	//定义最大接收字节数 200
//#define UART4_REC_LEN  			200  	//定义最大接收字节数 200
extern u8  wifi_init_end_flag;   				//wifi初始化结束完成标志，初始化未完成为0，完成为1（用以判定串口中断服务函数中是否清除USART1_RX_STA）
extern u8  close_led;
extern float body_temp;


extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART1_REC_LEN个字节.末字节为换行符	
extern u8  UART4_RX_BUF[UART4_REC_LEN]; //接收缓冲,最大USART1_REC_LEN个字节.末字节为换行符
extern u8  USART3_RX_BUF[USART3_REC_LEN]; //接收缓冲,最大USART1_REC_LEN个字节.末字节为换行符	  	
//extern u8  UART4_RX_BUF[UART4_REC_LEN]; //接收缓冲,最大USART1_REC_LEN个字节.末字节为换行符 

//接收状态：bit15，	接收完成标志；bit14，接收到0x0d；bit13~0，	接收到的有效字节数目
extern u16 USART1_RX_STA;         //串口1接收状态标记	
extern u16 UART4_RX_STA;         //串口1接收状态标记
extern u16 USART3_RX_STA;         //串口1接收状态标记
//extern u16 UART4_RX_STA;      		//串口4接收状态标记	


void usart1_init(u32 bound);
void uart4_init(u32 bound);
void usart3_init(u32 bound);
//void uart4_init(u32 bound);

void u1_printf (char *fmt, ...) ;
void u4_printf (char *fmt, ...) ;
void u3_printf (char *fmt, ...) ;
//void u4_printf (char *fmt, ...) ;

#endif

