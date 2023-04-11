#ifndef __LIGHT_H
#define __LIGHT_H
#include "stdio.h"	
#include "ht32.h"
#include "ht32_board.h"

#define USART1_REC_LEN  			260  	//定义最大接收字节数 200
#define Head									0xfa	//包头
#define Tail									0xf5	//包尾
#define THISNODEADDRESS				0x81	//本机地址
					
#define CMD_NOTHING						0x00	//无请求
#define CMD_HELP							0x01	//求救

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART1_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void usart1_init(u32 bound);
void u1sendData(u8 target);
u8 u1CheakReceiveData(void);

#endif








