#include "delay.h"
#include "usart.h"
#include "hc05.h"
#include "string.h"
#include "math.h"

DataSendTypeDef DataSend;

void Translation(void)
{
    u8 z=0;
    while(DataSend.CardNum[z]!='\0')
	{
		USART_SendData(HT_USART1 , DataSend.CardNum[z]);
		while(USART_GetLineStatus(HT_USART1,USART_LSR_TE )!=SET);
		z++;
	} 	       
}
//
void Receive(void)
{
    u8 i,reclen = 0;
	if(USART1_RX_STA&0x8000)        //���յ��Ž���������
	{
       DataSend.Region[0]=USART1_RX_BUF[0];
      // LCD_ShowString(117,180,200,16,16,DataSend.Region);
       USART1_RX_STA = 0;
	}
    if(USART0_RX_STA&0x8000)        //���յ�ԤԼϵͳ���͵�����
    {
        reclen=USART0_RX_STA&0X7FFF;	//�õ����ݳ���
        for(i=0;i<=reclen;i++)
        {
         USART_SendData(HT_USART1 ,USART0_RX_BUF[i]); //����ָ����Ž�
         while(USART_GetLineStatus(HT_USART1,USART_LSR_TE )!=SET); 
        }
        USART0_RX_STA = 0;
    }
}

