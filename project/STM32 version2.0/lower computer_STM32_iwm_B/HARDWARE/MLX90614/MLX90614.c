#include "MLX90614.h"
#include "usart.h"
#include <stdlib.h>
//66֡ͷ��66֡ͷ[0]��01��ʽ[1]��04�ֽ���[2]��0A[3]��83[4]��0A[5]��83[6]��1AУ��[7] 
uint16_t  count_body_tempe(void)
{
	uint16_t body_tempe=0;
	body_tempe=((uint16_t)UART4_RX_BUF[3]<<8|UART4_RX_BUF[4]);
	
	return body_tempe;
}
