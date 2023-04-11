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
	if(USART1_RX_STA&0x8000)        //接收到门禁返回数据
	{
       DataSend.Region[0]=USART1_RX_BUF[0];
      // LCD_ShowString(117,180,200,16,16,DataSend.Region);
       USART1_RX_STA = 0;
	}
    if(USART0_RX_STA&0x8000)        //接收到预约系统发送的数据
    {
        reclen=USART0_RX_STA&0X7FFF;	//得到数据长度
        for(i=0;i<=reclen;i++)
        {
         USART_SendData(HT_USART1 ,USART0_RX_BUF[i]); //发送指令给门禁
         while(USART_GetLineStatus(HT_USART1,USART_LSR_TE )!=SET); 
        }
        USART0_RX_STA = 0;
    }
}

