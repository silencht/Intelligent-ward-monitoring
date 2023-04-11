
#include "ht32.h"
#include "usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "MLX90614.h"
#include "voice_light.h"
/* Global functions ----------------------------------------------------------------------------------------*/
/**************************
*串口0：WIFI客户端，中断服务函数
*param：无
*返回值：无
**************************/
void USART0_IRQHandler(void)
{
	u8 Res;//接收数据中转变量
	//规定接收数据协议--------------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART0,USART_LSR_RFDR)!= RESET)//接收到数据
	{
		Res = USART_ReceiveData(HT_USART0);//接受到的数据幅给res
		if((USART0_RX_STA&0x8000)==0)//接受标志最高位等于0，接收未完成
			{
				if(USART0_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a)USART0_RX_STA=0;//0x0d后面不是0x0a，所以接收错误,重新开始接收
					else USART0_RX_STA|=0x8000;	//接收完成 
				}
				else //还没收到0X0d
				{	
					if(Res==0x0d)USART0_RX_STA|=0x4000;//是否接收到0x0d
					else
					{
						USART0_RX_BUF[USART0_RX_STA&0X3FFF]=Res ;//依次幅值给接收缓存数组
						USART0_RX_STA++;
						if(USART0_RX_STA>(USART0_MAX_RECV_LEN-1))
							{
							USART0_RX_STA=0;//数据量过大，重新开始接收	  
							}
					}		 
				}
			}
	
	}
	//数据接收完成后进行操作：---------------------------------------------------------------
	if(USART0_RX_STA&0x8000)
		{			
		  if(USART0_RX_BUF[0]=='1'|USART0_RX_BUF[0]=='2')		VOICE_EN();	
			else if(USART0_RX_BUF[0]=='5')  HUMI_DISEN();
			else if(USART0_RX_BUF[0]=='6')	HUMI_EN();
			USART0_RX_STA=0;
		}
 }
/**************************
*串口1中断服务函数
*功能：体温模块测试中断函数
*param：无
*返回值：无
**************************/
void USART1_IRQHandler(void)
{
	u8 Res;//接收数据中转变量
	u8 flg;//flg：两个0x66帧头检测标志位  t：发送循环变量
	//规定接收数据协议--------------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART1,USART_LSR_RFDR)!= RESET)//接收到数据
	{
		Res = USART_ReceiveData(HT_USART1);//接受到的数据幅给res
		if((USART1_RX_STA&0x8000)==0)//接受标志最高位等于0，接收未完成
			{
				if(USART1_RX_STA&0x4000)//接收到了0x66
				{
					if(flg==1)
					{
						if(Res!=0x66){USART1_RX_STA=0;flg=0;}//0x66后面不是0x66，所以接收错误,重新开始接收
					}
					else 
					{ 
						flg=0;
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;//依次幅值给接收缓存数组
						USART1_RX_STA++; 
						if(USART1_RX_STA==0x4008)      //体温模块包括0x66帧头在内共9个数据，第二个0x66放在数组的第一位 
						{
							USART1_RX_STA|=0x8000;//接收完成 
						}
					}
				}
				else //还没收到0X66
				{	
					if(Res==0x66){USART1_RX_STA|=0x4000; flg=1; }//是否接收到0x66
					else
					{
						
						if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))
							{
							USART1_RX_STA=0;//数据量过大，重新开始接收	  
							}
					}		 
				}
			} 
	
	}
	//数据接收完成后进行操作：---------------------------------------------------------------
	if(USART1_RX_STA&0x8000)
		{		
      body_temp = (float) count_body_tempe(USART1_RX_BUF)	;//更新体温变量值

 //如果体温模块持续大于100°，那么err_btemp计数器++，一旦计数器达到10次，那么体温模块进入死循环，等待看门狗复位
			if(100<body_temp)
				{
					err_btemp++;
					if(err_btemp==65534) while(1);
				}
			USART1_RX_STA=0;
		}
 } 
 


/*********************************************************************************************************
 * @brief   This function handles Hard Fault exception.
 * @retval  None
 ************************************************************************************************************/
void HardFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************
 * @brief   This function handles Memory Manage exception.
 * @retval  None
 ************************************************************************************************************/
void MemManage_Handler(void)
{
  while (1);
}

/*********************************************************************************************************
 * @brief   This function handles Bus Fault exception.
 * @retval  None
 ************************************************************************************************************/
void BusFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************
 * @brief   This function handles Usage Fault exception.
 * @retval  None
 ************************************************************************************************************/
void UsageFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************
 * @brief   This function handles SVCall exception.
 * @retval  None
 ************************************************************************************************************/
void SVC_Handler(void)
{
}

/*********************************************************************************************************
 * @brief   This function handles Debug Monitor exception.
 * @retval  None
 ************************************************************************************************************/
void DebugMon_Handler(void)
{
}

/*********************************************************************************************************
 * @brief   This function handles PendSVC exception.
 * @retval  None
 ************************************************************************************************************/
void PendSV_Handler(void)
{
}

/*********************************************************************************************************
 * @brief   This function handles SysTick Handler.
 * @retval  None
 ************************************************************************************************************/
void SysTick_Handler(void)
{
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
