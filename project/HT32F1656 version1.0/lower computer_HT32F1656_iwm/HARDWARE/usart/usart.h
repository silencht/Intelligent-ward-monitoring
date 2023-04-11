#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "ht32.h"
#include "ht32_board.h"
#include "usart_int.h"

#define USART0_MAX_RECV_LEN		800					//最大接收缓存字节数
#define USART0_MAX_SEND_LEN		800					//最大发送缓存字节数
#define USART0_RX_EN 			1								//0,不接收;1,接收.

extern u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 		//上位机数据接收数组
extern u8  upper_monitor_flag;            //判断是哪种下位机

extern u8  USART0_RX_BUF[USART0_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART0_TX_BUF[USART0_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART0_RX_STA;   						//接收数据状态
extern vu8  wifi_init_end_flag;   				//wifi初始化结束完成标志，初始化未完成为0，完成为1（用以判定串口中断服务函数中是否清除USART1_RX_STA）
extern float  body_temp;                    //从串口0接收中断中提取人的体温数据变量
extern int     err_btemp;								//体温错误次数计数器

#define USART1_MAX_RECV_LEN		800					//最大接收缓存字节数
#define USART1_MAX_SEND_LEN		800					//最大发送缓存字节数
#define USART1_RX_EN 			1					//0,不接收;1,接收.

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern vu16 USART1_RX_STA;   						//接收数据状态

void usart1_Init(u32 bound);
void usart0_Init(u32 bound);
void u0_printf(char* fmt,...);
void u1_printf(char* fmt,...);

#endif

