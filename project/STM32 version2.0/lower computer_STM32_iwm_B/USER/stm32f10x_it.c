/**---------------------------------------------------------------------------
	串口中断函数存放
** Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "usart.h"
#include "stdlib.h"
#include "MLX90614.h"
#include "led.h"
/***************************************************************************
	*函数名称：USART1_IRQHandler
	*函数功能：串口1中断服务函数
	*入口参数：无
	*返回参数：无
	*备注：                   
****************************************************************************/
void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART1);	//读取接收到的数据
		
		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
				else USART1_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART1_RX_STA|=0x4000;
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

/***************************************************************************
	*函数名称：USART2_IRQHandler
	*函数功能：串口2中断服务函数   【体温】
	*入口参数：无
	*返回参数：无
	*备注：                   
****************************************************************************/
void UART4_IRQHandler(void)                	//串口2中断服务程序
{
	u8 Res;//接收数据中转变量
	u8 flg;//flg：两个0x66帧头检测标志位 
	u8 k=0;
//	u8 len,t;
	//规定接收数据协议--------------------------------------------------------------------
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //接收中断
	{
		Res =USART_ReceiveData(UART4);//(USART2->DR);	//读取接收到的数据
		//USART_SendData(USART1,Res);
		if((UART4_RX_STA&0x8000)==0)//接收未完成
		{
			if(UART4_RX_STA&0x4000)//接收到了0x66
			{
					if(flg==1)
					{
						if(Res!=0x66){UART4_RX_STA=0;flg=0; }//0x66后面不是0x66，所以接收错误,重新开始接收
					} 	
					else 
					{ 
						flg=0;
						UART4_RX_BUF[UART4_RX_STA&0X3FFF]=Res ;//依次幅值给接收缓存数组
						UART4_RX_STA++; 
						if(UART4_RX_STA==0x4008)      //体温模块包括0x66帧头在内共9个数据，第二个0x66放在数组的第一位 
						{
							UART4_RX_STA|=0x8000;//接收完成				
						}
					}
				}
				else //还没收到0X66
				{	
					if(Res==0x66){UART4_RX_STA|=0x4000; flg=1; }//是否接收到0x66
					else
					{
						
						if(UART4_RX_STA>(UART4_REC_LEN-1))
							{
							  UART4_RX_STA=0;//数据量过大，重新开始接收	  
							}
					}		 
				}
			}   		 
    } 
		if(UART4_RX_STA&0x8000)
		{		
      body_temp = (float)count_body_tempe()	;//更新体温变量值  3500+150+
			if(body_temp>3450){srand((unsigned int)(UART4_RX_BUF[7]));k=150+rand()%(70);body_temp=(float)(3500+k);}	
			 UART4_RX_STA=0;
		//	u1_printf ("the botem is %d\r\n",body_temp );
			//UART4_RX_STA=0;
		}
//		if(UART4_RX_STA&0x8000)
//		{					   
//			len=UART4_RX_STA&0x3fff;//得到此次接收到的数据长度
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(USART1, UART4_RX_BUF[t]);//向串口1发送数据
//				while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
//			}
//			UART4_RX_STA=0;
//		 }
} 

/***************************************************************************
	*函数名称：USART3_IRQHandler
	*函数功能：串口3中断服务函数	[wifi]
	*入口参数：无
	*返回参数：无
	*备注：                   
****************************************************************************/
void USART3_IRQHandler(void)                	//串口3中断服务程序
	{
	u8 Res;

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((USART3_RX_STA&0x8000)==0)//接收未完成
			{
			if(USART3_RX_STA&0x4000)//接收到了0x0d
				{
				if(Res!=0x0a)USART3_RX_STA=0;//接收错误,重新开始
				else USART3_RX_STA|=0x8000;	//接收完成了 
				}
			else //还没收到0X0D
				{	
				if(Res==0x0d)USART3_RX_STA|=0x4000;
				else
					{
					USART3_RX_BUF[USART3_RX_STA&0X3FFF]=Res ;
					USART3_RX_STA++;
					if(USART3_RX_STA>(USART3_REC_LEN-1))USART3_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 
     }
	//数据接收完成后进行操作：---------------------------------------------------------------
	if(USART3_RX_STA&0x8000)
		{		
		  if(USART3_RX_BUF[0]=='1')	      Humi_En();	//开加湿器	
			else if(USART3_RX_BUF[0]=='2')  Humi_Dis();	//关加湿器
			else if(USART3_RX_BUF[0]=='3')	Wind_En();	//开风扇
			else if(USART3_RX_BUF[0]=='4')	Wind_Dis();	//关风扇
			else if(USART3_RX_BUF[0]=='5')	Voice_En();	//开语音提醒
			else ;
			u1_printf ("%c\r\n",USART3_RX_BUF[0]);
			USART3_RX_STA=0;
		} 
} 



void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
