#ifndef __USART_H
#define __USART_H
#include "stdio.h"
#include "ht32.h"
#include "ht32_board.h"
#include "usart_int.h"

#define USART0_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART0_MAX_SEND_LEN		800					//����ͻ����ֽ���
#define USART0_RX_EN 			1								//0,������;1,����.

extern u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 		//��λ�����ݽ�������
extern u8  upper_monitor_flag;            //�ж���������λ��

extern u8  USART0_RX_BUF[USART0_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART0_TX_BUF[USART0_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART0_RX_STA;   						//��������״̬
extern vu8  wifi_init_end_flag;   				//wifi��ʼ��������ɱ�־����ʼ��δ���Ϊ0�����Ϊ1�������ж������жϷ��������Ƿ����USART1_RX_STA��
extern float  body_temp;                    //�Ӵ���0�����ж�����ȡ�˵��������ݱ���
extern int     err_btemp;								//���´������������

#define USART1_MAX_RECV_LEN		800					//�����ջ����ֽ���
#define USART1_MAX_SEND_LEN		800					//����ͻ����ֽ���
#define USART1_RX_EN 			1					//0,������;1,����.

extern u8  USART1_RX_BUF[USART1_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8  USART1_TX_BUF[USART1_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern vu16 USART1_RX_STA;   						//��������״̬

void usart1_Init(u32 bound);
void usart0_Init(u32 bound);
void u0_printf(char* fmt,...);
void u1_printf(char* fmt,...);

#endif

