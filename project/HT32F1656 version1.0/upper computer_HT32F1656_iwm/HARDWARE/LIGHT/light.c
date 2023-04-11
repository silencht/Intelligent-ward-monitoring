#include "light.h"
#include "dht11.h" 
#include "adc.h"
#include "ht32.h"
#include "ht32_board.h"
#include "usart_int.h"
#include "string.h"
#include "stdio.h"
u8 USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 USART1_RX_STA=0;       //接收状态标记	  
  
void usart1_init(u32 bound)
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	USART_InitTypeDef USART_InitStructure;
	CKCUClock.Bit.USART1     = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  AFIO_GPAConfig(AFIO_PIN_4 | AFIO_PIN_5, AFIO_MODE_6);		
  USART_InitStructure.USART_BaudRate = bound;
  USART_InitStructure.USART_WordLength = USART_WORDLENGTH_8B;
  USART_InitStructure.USART_StopBits = USART_STOPBITS_1;
  USART_InitStructure.USART_Parity = USART_PARITY_NO;
  USART_InitStructure.USART_Mode = USART_MODE_NORMAL;
  USART_Interrupt_Init(HT_USART1, &USART_InitStructure, USART_RX_INT);
	USART_TxCmd(HT_USART1, ENABLE);
	USART_RxCmd(HT_USART1, ENABLE);
	USART_IntConfig(HT_USART1, USART_IER_RDAIE, ENABLE);	//接收中断使能
}
void USART1_IRQHandler(void)
{
	u8 Res;

	if(((HT_USART1->IIR) & USART_IID_RDA ) == USART_IID_RDA )  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res = USART_ReceiveData(HT_USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0xfa)USART1_RX_STA=0;//接收错误,重新开始
				else USART1_RX_STA|=0x8000;	//接收完成了 
					
				}
			else //还没收到0X0D
				{	
				if(Res==0xfd)USART1_RX_STA|=0x4000;
				else
					{
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
					USART1_RX_STA++;
					if(USART1_RX_STA>(USART1_REC_LEN-1))USART1_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     } 
}
//发送数据
//参数： addr：地址 cmd：命令 data：待发送数据
//返回值 无
void u1sendData(u8 target)
{
	u8 temp=0;
	u8 humi;
	vu16 adcx;
	vu16 ppm;	//浓度
	DHT11_Read_Data(&temp,&humi);//读取温湿度 
	adcx=Get_Adc_Average(ADC_CH_0,10);

	ppm =(vu16)( adcx * 33 * 2 * 200 / 40960 );	//ppm为0时输出电压为1v
	
	USART_SendData(HT_USART1,0x0d);							//发送总字节数
	USART_SendData(HT_USART1,Head);							//发送包头
	USART_SendData(HT_USART1,target);			//发送目标地址	
	USART_SendData(HT_USART1,THISNODEADDRESS);			//发送本机地址	
	
	USART_SendData(HT_USART1,temp);
	USART_SendData(HT_USART1,humi);
	USART_SendData(HT_USART1,(u16)ppm &0xff);
	USART_SendData(HT_USART1,((u16)ppm>>8)&0xff);
	
	
	USART_SendData(HT_USART1,0x00);							//发送空员工号
	USART_SendData(HT_USART1,CMD_NOTHING);			//发送无请求命令
	temp=(u8)(Head+target+THISNODEADDRESS+temp+humi+((u16)ppm &0xff)+(((u16)ppm>>8)&0xff)+	0x00+CMD_NOTHING);
	USART_SendData(HT_USART1,temp);							//发送校验和
	USART_SendData(HT_USART1,Tail);
	USART_SendData(HT_USART1,0xfd);
	USART_SendData(HT_USART1,0xfa);
}


//返回值 0：接收出错		1：接收成功
u8 u1CheakReceiveData(void)
{
	u8 i;
	u32 crcTemp = 0;
	u32 crcrec=0;
	u8 bytenum=	USART1_RX_BUF[0];
 	

	for(i=0;i<bytenum-6 ;i++)
	{
		crcTemp += USART1_RX_BUF[i];					//计算CRC
	}
	crcrec=(USART1_RX_BUF[i]<<24)+(USART1_RX_BUF[i+1]<<16)+(USART1_RX_BUF[i+2]<<8)+USART1_RX_BUF[i+3];
	if(crcrec==crcTemp)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}




