#ifndef __KQ130F_H
#define __KQ130F_H
#include "ht32.h"
#include "ht32_board.h"


#define USART0_REC_LEN  			260  	//�����������ֽ��� 200

extern u8  USART0_RX_BUF[USART0_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART0_RX_STA;         		//����״̬���	

void usart0_init(u32 bound);
void sendData(u8 addr,u8 cmd,u8 *data,u8 bytenum);
u8 CheakReceiveData(void);




#endif
