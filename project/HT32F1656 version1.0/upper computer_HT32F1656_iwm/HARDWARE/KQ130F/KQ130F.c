#include "KQ130F.h"
#include "ht32.h"
#include "ht32_board.h"
#include "usart_int.h"

u8 USART0_RX_BUF[USART0_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART0_RX_STA=0;       //����״̬���	  
  
void usart0_init(u32 bound)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	
	USART_InitTypeDef USART_InitStructure;
	
	CKCUClock.Bit.USART0     = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Config AFIO mode as USART0_Rx and USART0_Tx function.                                                  */
  AFIO_GPAConfig(AFIO_PIN_2 | AFIO_PIN_3, AFIO_MODE_6);	
	
  /* Config AFIO mode as USARTx function                                                                    */
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
  USART_InitStructure.USART_Parity = USART_PARITY_NO;
  USART_InitStructure.USART_Mode = USART_MODE_NORMAL;

  /* Configure USART0 & UART0 interrupt                                                                     */
  USART_Interrupt_Init(HT_USART0, &USART_InitStructure, USART_RX_INT);
	USART_TxCmd(HT_USART0, ENABLE);
	USART_RxCmd(HT_USART0, ENABLE);
	USART_IntConfig(HT_USART0, USART_IER_RDAIE, ENABLE);	//�����ж�ʹ��
}


void USART0_IRQHandler(void)
{
	u8 Res;

	if(((HT_USART0->IIR) & USART_IID_RDA ) == USART_IID_RDA )  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		Res = USART_ReceiveData(HT_USART0);	//��ȡ���յ�������
		
		if((USART0_RX_STA&0x8000)==0)//����δ���
			{
			if(USART0_RX_STA&0x4000)//���յ���0x0d
				{
				if(Res!=0xfa)USART0_RX_STA=0;//���մ���,���¿�ʼ
				else USART0_RX_STA|=0x8000;	//��������� 
					
				}
			else //��û�յ�0X0D
				{	
				if(Res==0xfd)USART0_RX_STA|=0x4000;
				else
					{
					USART0_RX_BUF[USART0_RX_STA&0X3FFF]=Res ;
					USART0_RX_STA++;
					if(USART0_RX_STA>(USART0_REC_LEN-1))USART0_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 
     } 
}

//��������
//������ addr����ַ cmd������ data������������
//����ֵ ��
void sendData(u8 addr,u8 cmd,u8 *data,u8 bytenum)
{
	u32 crcTemp = 0;	//crc����У���
	u8 i;
	bytenum+=9;
	crcTemp += bytenum;					//����CRC
	crcTemp += addr;					//����CRC
	crcTemp += cmd;					//����CRC
	
	USART_SendData(HT_USART0,bytenum);					//�������ֽ���
	USART_SendData(HT_USART0,addr);							//���͵�ַ	 
	USART_SendData(HT_USART0,cmd);							//��������
	for(i = 0;i < bytenum-9;i++)
	{
		USART_SendData(HT_USART0,data[i]);						//��������
		crcTemp +=data[i];	//����CRC
	}
	USART_SendData(HT_USART0,(u8)((crcTemp&0xff000000)>>24));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x00ff0000)>>16));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x0000ff00)>>8));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x000000ff)>>0));	//����CRC
	USART_SendData(HT_USART0,0xfd);
	USART_SendData(HT_USART0,0xfa);
}


//����ֵ 0�����ճ���		1�����ճɹ�
u8 CheakReceiveData(void)
{
	u8 i;
	u32 crcTemp = 0;
	u32 crcrec=0;
	u8 bytenum=	USART0_RX_BUF[0];
 	

	for(i=0;i<bytenum-6 ;i++)
	{
		crcTemp += USART0_RX_BUF[i];					//����CRC
	}
	crcrec=(USART0_RX_BUF[i]<<24)+(USART0_RX_BUF[i+1]<<16)+(USART0_RX_BUF[i+2]<<8)+USART0_RX_BUF[i+3];
	if(crcrec==crcTemp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}




