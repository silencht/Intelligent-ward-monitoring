#ifndef __KQ130F_H
#define __KQ130F_H
#include "ht32.h"
#include "ht32_board.h"


#define USART0_REC_LEN  			260  	//定义最大接收字节数 200

extern u8  USART0_RX_BUF[USART0_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART0_RX_STA;         		//接收状态标记	

void usart0_init(u32 bound);
void sendData(u8 addr,u8 cmd,u8 *data,u8 bytenum);
u8 CheakReceiveData(void);




#endif
