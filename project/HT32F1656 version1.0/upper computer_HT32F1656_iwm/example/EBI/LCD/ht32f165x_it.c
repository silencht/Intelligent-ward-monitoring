/*********************************************************************************************************//**
 * @file    EBI/LCD/ht32f165x_it.c
 * @version $Rev:: 929          $
 * @date    $Date:: 2015-09-16 #$
 * @brief   This file provides all interrupt service routine.
 *************************************************************************************************************/
#include "ht32.h"
#include "usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "MLX90614.h"
#include "light_voice.h"

/* Global functions ----------------------------------------------------------------------------------------*/
/**************************
*串口0中断服务函数
*param：无
*返回值：无
**************************/
void USART0_IRQHandler(void)
{
	u8 Res;//接收数据中转变量
	u8 len;//接收数据长度
	u8 t;
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
//	if(USART0_RX_STA&0x8000)
//		{			
//			len=USART0_RX_STA&0x3fff;//得到此次接收到的数据长度
////			LCD_ShowString(23,58,200,16,16,"UART0"); 
////			LCD_ShowString(23,38,200,16,16,USART0_RX_BUF); 	
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(HT_USART0, USART0_RX_BUF[t]);         //向串口0发送数据
//				while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//等待发送结束
//			}
////	    u0_printf("\r\n\r\n");//插入换行															
//			USART0_RX_STA=0;
//		}
 }
/**************************
*串口1中断服务函数
*param：无
*功能：WIFI模块收发（单下位机）
*返回值：无
**************************/
void USART1_IRQHandler(void)
{
	u8 Res;//接收数据中转变量
	u8 len;//接收数据长度
	u8 t,colon,mid,i;//colon表示冒号的位置
	uint8_t    lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag;
	//规定接收数据协议-----------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART1,USART_LSR_RFDR)!= RESET)//接收到数据
	{
		Res = USART_ReceiveData(HT_USART1);
		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
				if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
					else USART1_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0d
				{	
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
						USART1_RX_STA++;
						if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))
							{
								USART1_RX_STA=0;//数据量过大，重新开始接收	  
							}
					}		 
				}
			}
			WDT_Restart();                       // Reload Counter as WDTV Value（喂狗函数）
	 }
	//数据接收完成后进行操作：
			//单下位机通信格式规定如下：
			//---[f]为病房下位机、[r]为病人下位机、[z]为单下位机、'*'为分隔符、'$'为数据停止符，前6个数据为相应环境、人体参数，
			//后6个为数据范围判断位，每个数据判断的档位，上位机接受之后翻译：
			/*光强度：1很亮（提示拉窗帘）	2正常	3略暗（开一个led）	4很暗（开两个led）
			温度：1较高	2正常	3较低
			湿度：1潮湿	2正常	3干燥
			体温：1较高	2正常 3可能未穿戴 4低烧
			血氧：1病人有问题 2如果为95朝上那么数据有效	 3等于0则未穿戴设备
			if（血氧判断为==1），那么心率	1较低	2正常	3较高
			else 4未穿戴设备
			下位机if（心率血样数据持续为0|温度持续低于30度）提示病人穿戴设备；*/
			//+IPD,0,23:[z]lux*123*temp30*humi35*btemp*36*hrate75spo2*99$	（lux等不发送）
			//下位机send:   [z]*1234*26*34*37*75*99*1*1*1*1*1*1$        上位机receive:+IPD,0,15:[z]*1234*26*34*37*75*99*1*1*1*1*1*1$ 
			//上位机数据接收数组:u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 
	if(USART1_RX_STA&0x8000)
		{		
			len=USART1_RX_STA&0x3fff;//得到此次接收到的数据长度
			for(t=0;t<len;t++)
				{
					USART_SendData(HT_USART0, USART1_RX_BUF[t]);         //向串口0发送数据
					while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//等待发送结束
				}
			if(USART1_RX_BUF[0]=='+')
			{		
				//得到冒号指针坐标
				for(colon=1;USART1_RX_BUF[colon]!=':';colon++);
					//如果是病房下位机发送的数据串------------------------------------------------------------------------
					if(USART1_RX_BUF[colon+2]=='z')
					{							
							//正式数据显示
							mid=colon+5;
							//1
							for(i=0;USART1_RX_BUF[mid]!='*';mid++,i++) 
							{
								lux[i]=USART1_RX_BUF[mid];
							  if(i==3) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	//数据有乱码时用此行语句进行判断，如果数据位数越界，那么自动截断数据保留前有效位，然后移动mid指针到下一个数据						
							}	
							LCD_ShowString(15+16*4,105,230,16,16,"                 ");	//数据位数从多变少时，会产生屏幕遗留，每次用空格”清屏“
							LCD_ShowString(15+16*4,105,230,16,16,lux);
							//2
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								temp[i]=USART1_RX_BUF[mid];
								if(i==4) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,125,230,16,16,"                   ");
							LCD_ShowString(15+16*4,125,230,16,16,temp);
							//3
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								humi[i]=USART1_RX_BUF[mid];
								if(i==4) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}	
							LCD_ShowString(15+16*4,145,230,16,16,"                  ");
							LCD_ShowString(15+16*4,145,230,16,16,humi);
							
							//4
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++) 
							{
								btemp[i]=USART1_RX_BUF[mid];
								if(i==4) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}               
							LCD_ShowString(15+16*4,165,230,16,16,"                   ");
							LCD_ShowString(15+16*4,165,230,16,16,btemp);
							//5
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								hrate[i]=USART1_RX_BUF[mid];
								if(i==2) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,185,230,16,16,"                   ");
							LCD_ShowString(15+16*4,185,230,16,16,hrate);
							//6
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								spo2[i]=USART1_RX_BUF[mid];
								if(i==2) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,205,230,16,16,"                   ");	
							LCD_ShowString(15+16*4,205,230,16,16,spo2);	
	//---------------------------------------------------------------------------------------------------------------------------------------------------------------------						
							//1+
							lux_flag=USART1_RX_BUF[++mid];
							for(;USART1_RX_BUF[mid]!='*';mid++);	
							if(lux_flag=='1'){POINT_COLOR=RED;LCD_ShowString(15+16*7,105,230,16,16,"very light!");POINT_COLOR=BLUE;}
							else if(lux_flag=='2')LCD_ShowString(15+16*7,105,230,16,16,"good");
							else if(lux_flag=='3')LCD_ShowString(15+16*7,105,230,16,16,"little dark!");
							else if(lux_flag=='4'){POINT_COLOR=RED;LCD_ShowString(15+16*7,105,230,16,16,"very dark!");POINT_COLOR=BLUE;}
							//2+
							temperature_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='*';mid++);	
							if(temperature_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(15+16*7,125,230,16,16,"hot!");POINT_COLOR=BLUE;}
							else if(temperature_flag=='2')LCD_ShowString(15+16*7,125,230,16,16,"good!");
							else if(temperature_flag=='3'){POINT_COLOR=RED;LCD_ShowString(15+16*7,125,230,16,16,"cold!");POINT_COLOR=BLUE;}
							//3+
							humidity_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='*';mid++);	
							if(humidity_flag=='1')		{POINT_COLOR=RED; LCD_ShowString(15+16*7,145,230,16,16,"wet!");POINT_COLOR=BLUE;}
							else if(humidity_flag=='2')LCD_ShowString(15+16*7,145,230,16,16,"good!");
							else if(humidity_flag=='3'){POINT_COLOR=RED;LCD_ShowString(15+16*7,145,230,16,16,"dry!");POINT_COLOR=BLUE;}
							//4+
							body_temp_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='*';mid++);
							if(body_temp_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(15+16*7,165,230,16,16,"high!");POINT_COLOR=BLUE;}
							else if(body_temp_flag=='2')LCD_ShowString(15+16*7,165,230,16,16,"good!");
							else if(body_temp_flag=='3'){POINT_COLOR=RED;LCD_ShowString(15+16*7,165,230,16,16,"not wear!");POINT_COLOR=BLUE;}
							else if(body_temp_flag=='4'){POINT_COLOR=RED;LCD_ShowString(15+16*7,165,230,16,16,"low fever!");POINT_COLOR=BLUE;}
							//5+
							hrate_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='*';mid++);
							if(hrate_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(15+16*7,185,230,16,16,"low!");POINT_COLOR=BLUE;}
							else if(hrate_flag=='2')LCD_ShowString(15+16*7,185,230,16,16,"good!");
							else if(hrate_flag=='3'){POINT_COLOR=RED;LCD_ShowString(15+16*7,185,230,16,16,"high");;POINT_COLOR=BLUE;}
							else if(hrate_flag=='4'){POINT_COLOR=RED;LCD_ShowString(15+16*7,185,230,16,16,"not wear!");POINT_COLOR=BLUE;}
							//6+
							spo2_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='*';mid++);
							if(spo2_flag=='1')		 {POINT_COLOR=RED;LCD_ShowString(15+16*7,205,230,16,16,"low!");POINT_COLOR=BLUE;}
							else if(spo2_flag=='2') LCD_ShowString(15+16*7,205,230,16,16,"good!");
							else if(spo2_flag=='3'){POINT_COLOR=RED;LCD_ShowString(15+16*7,205,230,16,16,"not wear!");POINT_COLOR=BLUE;}
							call_flag=USART1_RX_BUF[++mid];
								for(;USART1_RX_BUF[mid]!='$';mid++);
							if(call_flag=='1')		   { POINT_COLOR=RED;BEEP_long_EN(); LCD_ShowString(15,225,230,16,16,"patient in danger!");POINT_COLOR=BLUE;}
							else if(call_flag=='0')  { LCD_ShowString(15,225,230,16,16,"patient is good  !");}
							//得到下位机标号							
							upper_monitor_flag=USART1_RX_BUF[5];  //现在为病房发送数据，下位机标号最高位置1	
						}	
					}		
			//如果标志位不为1的话，那么表示串口WIFI初始化未完成，那么不应该在这里将USART1_RX_STA置零			
			if(wifi_init_end_flag==1)
			{
			  USART1_RX_STA=0;
			}
			//下位机为单一类型----------------------------------------------------
//			  LCD_ShowString(15+16*6,225,230,16,16,"dan");
				upper_monitor_flag=upper_monitor_flag-'0';  //得到下位机esp8266数字编号
//			  LCD_ShowxNum(15+16*10,225,upper_monitor_flag,2,16,0);					
			//清空各类数据缓冲数组-------------------------------------------
			memset(USART1_RX_BUF,0,20*sizeof(unsigned char));
			memset(lux,0,4*sizeof(unsigned char));
			memset(temp,0,3*sizeof(unsigned char));
			memset(humi,0,3*sizeof(unsigned char));
			memset(btemp,0,2*sizeof(unsigned char));
			memset(hrate,0,3*sizeof(unsigned char));
			memset(spo2,0,3*sizeof(unsigned char));
		}

}  
/**************************
*串口1中断服务函数
*param：无
*功能：WIFI模块收发（双下位机）
*返回值：无
**************************
void USART1_IRQHandler(void)
{
	u8 Res;//接收数据中转变量
	u8 len;//接收数据长度
	u8 t,colon,mid,i;//colon表示冒号的位置
	//规定接收数据协议-----------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART1,USART_LSR_RFDR)!= RESET)//接收到数据
	{
		Res = USART_ReceiveData(HT_USART1);
		if((USART1_RX_STA&0x8000)==0)//接收未完成
			{
				if(USART1_RX_STA&0x4000)//接收到了0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//接收错误,重新开始
					else USART1_RX_STA|=0x8000;	//接收完成了 
				}
				else //还没收到0X0d
				{	
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
						USART1_RX_STA++;
						if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))
							{
								USART1_RX_STA=0;//数据量过大，重新开始接收	  
							}
					}		 
				}
			}
	 }
	//数据接收完成后进行操作：
	//---[f]为病房、[r]为病人、'*'为分隔符----------双下位机通信格式规定如下--------------------------------------
  //+IPD,0,23:[f]lux*123*temp30*humi35$
	//+IPD,0,23:[r]btemp*36*hrate75spo2*99$    （lux等不发送）
	//  send: [f]*12*34*56$  receive:+IPD,0,15:[f]*12*34*56$
	//上位机数据接收数组:
	//u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 		
	if(USART1_RX_STA&0x8000)
		{			
			len=USART1_RX_STA&0x3fff;//得到此次接收到的数据长度
			for(t=0;t<len;t++)
			{
				USART_SendData(HT_USART0, USART1_RX_BUF[t]);         //向串口0发送数据
				while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//等待发送结束
			}
			if(USART1_RX_BUF[0]=='+')
			{
				//得到冒号指针坐标
				for(colon=1;USART1_RX_BUF[colon]!=':';colon++);
					//如果是病房下位机发送的数据串------------------------------------------------------------------------
					if(USART1_RX_BUF[colon+2]=='f')
					{							
							//正式数据显示
							mid=colon+5;
							for(i=0;USART1_RX_BUF[mid]!='*';mid++,i++) 
							{
								lux[i]=USART1_RX_BUF[mid];
							  if(i==3) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}							
							}	
							LCD_ShowString(15+16*4,105,230,16,16,"          ");	
							LCD_ShowString(15+16*4,105,230,16,16,lux);
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								temp[i]=USART1_RX_BUF[mid];
								if(i==2) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,125,230,16,16,"            ");
							LCD_ShowString(15+16*4,125,230,16,16,temp);
							for(i=0,mid++;USART1_RX_BUF[mid]!='$';mid++,i++)
							{
								humi[i]=USART1_RX_BUF[mid];
								if(i==1) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}	
							LCD_ShowString(15+16*4,145,230,16,16,"          ");
							LCD_ShowString(15+16*4,145,230,16,16,humi);
							
						  //判断哪种下位机标号							
							upper_monitor_flag=USART1_RX_BUF[5]|0x80;  //现在为病房发送数据，下位机标号最高位置1
					
					}			
  				//如果是病人下位机发送的数据串----------------------------------------------------------------------------
					if(USART1_RX_BUF[colon+2]=='r')
					{							
							 mid=colon+5;
							 for(i=0;USART1_RX_BUF[mid]!='*';mid++,i++) 
							{
								btemp[i]=USART1_RX_BUF[mid];
								if(i==1) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,165,230,16,16,"         ");
							LCD_ShowString(15+16*4,165,230,16,16,btemp);
							for(i=0,mid++;USART1_RX_BUF[mid]!='*';mid++,i++)
							{
								hrate[i]=USART1_RX_BUF[mid];
								if(i==2) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,185,230,16,16,"         ");
							LCD_ShowString(15+16*4,185,230,16,16,hrate);
							for(i=0,mid++;USART1_RX_BUF[mid]!='$';mid++,i++)
							{
								spo2[i]=USART1_RX_BUF[mid];
								if(i==2) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	
							}
							LCD_ShowString(15+16*4,205,230,16,16,"      ");	
							LCD_ShowString(15+16*4,205,230,16,16,spo2);			

							//判断哪种下位机标号							
							upper_monitor_flag=USART1_RX_BUF[5]|0x40;//现在为病人发送数据，下位机标号最高位置0							
					}
 			}		
			//如果标志位不为1的话，那么表示串口WIFI初始化未完成，那么不应该在这里将USART1_RX_STA置零			
			if(wifi_init_end_flag==1)
			{
			  USART1_RX_STA=0;
			}
			//判断下位机----------------------------------------------------
			if(upper_monitor_flag&0x80)
			{
			  LCD_ShowString(15+16*6,225,230,16,16,"fang ");	
				upper_monitor_flag=upper_monitor_flag&0x7f-'0';
			  LCD_ShowxNum(15+16*10,225,upper_monitor_flag,2,16,0);
			}
			else if(upper_monitor_flag&0x40)
			{
				LCD_ShowString(15+16*6,225,230,16,16," ren ");	
				upper_monitor_flag=upper_monitor_flag&0x3f-'0';
			  LCD_ShowxNum(15+16*10,225,upper_monitor_flag,2,16,0);			
			}
			else {}
			//清空各类数据缓冲数组-------------------------------------------
			memset(USART1_RX_BUF,0,20*sizeof(unsigned char));
			memset(lux,0,4*sizeof(unsigned char));
			memset(temp,0,3*sizeof(unsigned char));
			memset(humi,0,3*sizeof(unsigned char));
			memset(btemp,0,2*sizeof(unsigned char));
			memset(hrate,0,3*sizeof(unsigned char));
			memset(spo2,0,3*sizeof(unsigned char));
		}

 }
*/

/*********************************************************************************************************//**
 * @brief   This function handles Hard Fault exception.
 * @retval  None
 ************************************************************************************************************/
void HardFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************//**
 * @brief   This function handles Memory Manage exception.
 * @retval  None
 ************************************************************************************************************/
void MemManage_Handler(void)
{
  while (1);
}

/*********************************************************************************************************//**
 * @brief   This function handles Bus Fault exception.
 * @retval  None
 ************************************************************************************************************/
void BusFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************//**
 * @brief   This function handles Usage Fault exception.
 * @retval  None
 ************************************************************************************************************/
void UsageFault_Handler(void)
{
  while (1);
}

/*********************************************************************************************************//**
 * @brief   This function handles SVCall exception.
 * @retval  None
 ************************************************************************************************************/
void SVC_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles Debug Monitor exception.
 * @retval  None
 ************************************************************************************************************/
void DebugMon_Handler(void)
{
}

/*********************************************************************************************************//**
 * @brief   This function handles PendSVC exception.
 * @retval  None
 ************************************************************************************************************/
void PendSV_Handler(void)
{
}

/*********************************************************************************************************//**
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
