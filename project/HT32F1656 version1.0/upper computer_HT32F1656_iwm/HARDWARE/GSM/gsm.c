#include "gsm.h"
#include "usart.h"
#include "delay.h"

#define swap16(x) (x&0XFF)<<8|(x&0XFF00)>>8		//�ߵ��ֽڽ����궨��

u8 reciveFlag1;
u8 smsRcvFlag1;
u8 GsmRcv[200] = {0};
u8 GsmRcvCnt = 0;
u8 GsmAtFlag = 0;
u8 GsmRcvAt[100];
/*************************************
�������ܣ����Ͷ���
��ڲ�������
���ڲ�����
*************************************/
void Uart2Sendshaohao(u8 *data_athao)
{
	unsigned char cnt1=0;
	
	USART_IntConfig(HT_USART1, USART_IER_RDAIE, DISABLE);//�رմ���2�ж�
	while(*(data_athao+cnt1))	//�ж�һ�������Ƿ����
	{
		USART_SendData(HT_USART1,*(data_athao+cnt1));
		cnt1++;
	}
	USART_IntConfig(HT_USART1, USART_IER_RDAIE, ENABLE);//��������2�ж�
}

void USART1_IRQHandler(void)
{
	if ( ( (HT_USART1->IIR) & USART_IID_RDA ) == USART_IID_RDA )//??????,??494?
	{
		GsmRcv[reciveFlag1]=HT_USART1->RBR;
//		if(GsmRcv[reciveFlag]=='+')
//		  {
//			GsmRcv[0]=GsmRcv[reciveFlag];
//			reciveFlag=0;
//		}
		reciveFlag1++;
		//????  \r\n?????????
    if(smsRcvFlag1==0)
		{
			if(GsmRcv[reciveFlag1-2]=='\r'&&GsmRcv[reciveFlag1-1]=='\n'&&reciveFlag1>=2) 
			{
				GsmAtFlag=1;
				GsmRcv[reciveFlag1-2]='\0';//��ӽ�����־
				reciveFlag1=0;
			}
		}
//		}else{//GPS���ݴ���
//			if(GsmRcv[reciveFlag-2]=='\r'&&GsmRcv[reciveFlag-1]=='\n'&&reciveFlag>=2) {
//				GPSOK=1;
//				GsmRcv[reciveFlag-2]='\0';//��ӽ�����־
//				reciveFlag=0;
//			}
//		}
//		reciveFlag++;
//		if(reciveFlag==96)
//			reciveFlag=0;
//		aa=USART1->RBR;
//		USART_SendData (USART0,aa);
	}
//    USART_IntConfig(USART1, USART_IER_RDAIE, ENABLE);	
}



