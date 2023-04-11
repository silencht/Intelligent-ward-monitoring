#ifndef __LIGHT_H
#define __LIGHT_H
#include "stdio.h"	
#include "ht32.h"
#include "ht32_board.h"

#define USART1_REC_LEN  			260  	//�����������ֽ��� 200
#define Head									0xfa	//��ͷ
#define Tail									0xf5	//��β
#define THISNODEADDRESS				0x81	//������ַ
					
#define CMD_NOTHING						0x00	//������
#define CMD_HELP							0x01	//���

extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void usart1_init(u32 bound);
void u1sendData(u8 target);
u8 u1CheakReceiveData(void);

#endif








