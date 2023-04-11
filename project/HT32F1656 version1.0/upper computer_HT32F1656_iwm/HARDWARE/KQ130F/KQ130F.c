#include "KQ130F.h"
#include "ht32.h"
#include "ht32_board.h"
#include "usart_int.h"

u8 USART0_RX_BUF[USART0_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART0_RX_STA=0;       //接收状态标记	  
  
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
	USART_IntConfig(HT_USART0, USART_IER_RDAIE, ENABLE);	//接收中断使能
}


void USART0_IRQHandler(void)
{
	u8 Res;

	if(((HT_USART0->IIR) & USART_IID_RDA ) == USART_IID_RDA )  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(HT_USART0);	//读取接收到的数据
		
		if((USART0_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART0_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0xfa)USART0_RX_STA=0;//接收错误,重新开始
				else USART0_RX_STA|=0x8000;	//接收完成了 
					
				}
			else //还没收到0X0D
				{	
				if(Res==0xfd)USART0_RX_STA|=0x4000;
				else
					{
					USART0_RX_BUF[USART0_RX_STA&0X3FFF]=Res ;
					USART0_RX_STA++;
					if(USART0_RX_STA>(USART0_REC_LEN-1))USART0_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
}

//发送数据
//参数： addr：地址 cmd：命令 data：待发送数据
//返回值 无
void sendData(u8 addr,u8 cmd,u8 *data,u8 bytenum)
{
	u32 crcTemp = 0;	//crc换成校验和
	u8 i;
	bytenum+=9;
	crcTemp += bytenum;					//计算CRC
	crcTemp += addr;					//计算CRC
	crcTemp += cmd;					//计算CRC
	
	USART_SendData(HT_USART0,bytenum);					//发送总字节数
	USART_SendData(HT_USART0,addr);							//发送地址	 
	USART_SendData(HT_USART0,cmd);							//发送命令
	for(i = 0;i < bytenum-9;i++)
	{
		USART_SendData(HT_USART0,data[i]);						//发送数据
		crcTemp +=data[i];	//计算CRC
	}
	USART_SendData(HT_USART0,(u8)((crcTemp&0xff000000)>>24));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x00ff0000)>>16));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x0000ff00)>>8));
	USART_SendData(HT_USART0,(u8)((crcTemp&0x000000ff)>>0));	//发送CRC
	USART_SendData(HT_USART0,0xfd);
	USART_SendData(HT_USART0,0xfa);
}


//返回值 0：接收出错		1：接收成功
u8 CheakReceiveData(void)
{
	u8 i;
	u32 crcTemp = 0;
	u32 crcrec=0;
	u8 bytenum=	USART0_RX_BUF[0];
 	

	for(i=0;i<bytenum-6 ;i++)
	{
		crcTemp += USART0_RX_BUF[i];					//计算CRC
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




