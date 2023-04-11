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
*����0�жϷ�����
*param����
*����ֵ����
**************************/
void USART0_IRQHandler(void)
{
	u8 Res;//����������ת����
	u8 len;//�������ݳ���
	u8 t;
	//�涨��������Э��--------------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART0,USART_LSR_RFDR)!= RESET)//���յ�����
	{
		Res = USART_ReceiveData(HT_USART0);//���ܵ������ݷ���res
		if((USART0_RX_STA&0x8000)==0)//���ܱ�־���λ����0������δ���
			{
				if(USART0_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a)USART0_RX_STA=0;//0x0d���治��0x0a�����Խ��մ���,���¿�ʼ����
					else USART0_RX_STA|=0x8000;	//������� 
				}
				else //��û�յ�0X0d
				{	
					if(Res==0x0d)USART0_RX_STA|=0x4000;//�Ƿ���յ�0x0d
					else
					{
						USART0_RX_BUF[USART0_RX_STA&0X3FFF]=Res ;//���η�ֵ�����ջ�������
						USART0_RX_STA++;
						if(USART0_RX_STA>(USART0_MAX_RECV_LEN-1))
							{
							USART0_RX_STA=0;//�������������¿�ʼ����	  
							}
					}		 
				}
			}
	
	}
	//���ݽ�����ɺ���в�����---------------------------------------------------------------
//	if(USART0_RX_STA&0x8000)
//		{			
//			len=USART0_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
////			LCD_ShowString(23,58,200,16,16,"UART0"); 
////			LCD_ShowString(23,38,200,16,16,USART0_RX_BUF); 	
//			for(t=0;t<len;t++)
//			{
//				USART_SendData(HT_USART0, USART0_RX_BUF[t]);         //�򴮿�0��������
//				while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//�ȴ����ͽ���
//			}
////	    u0_printf("\r\n\r\n");//���뻻��															
//			USART0_RX_STA=0;
//		}
 }
/**************************
*����1�жϷ�����
*param����
*���ܣ�WIFIģ���շ�������λ����
*����ֵ����
**************************/
void USART1_IRQHandler(void)
{
	u8 Res;//����������ת����
	u8 len;//�������ݳ���
	u8 t,colon,mid,i;//colon��ʾð�ŵ�λ��
	uint8_t    lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag;
	//�涨��������Э��-----------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART1,USART_LSR_RFDR)!= RESET)//���յ�����
	{
		Res = USART_ReceiveData(HT_USART1);
		if((USART1_RX_STA&0x8000)==0)//����δ���
			{
				if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
					else USART1_RX_STA|=0x8000;	//��������� 
				}
				else //��û�յ�0X0d
				{	
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
						USART1_RX_STA++;
						if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))
							{
								USART1_RX_STA=0;//�������������¿�ʼ����	  
							}
					}		 
				}
			}
			WDT_Restart();                       // Reload Counter as WDTV Value��ι��������
	 }
	//���ݽ�����ɺ���в�����
			//����λ��ͨ�Ÿ�ʽ�涨���£�
			//---[f]Ϊ������λ����[r]Ϊ������λ����[z]Ϊ����λ����'*'Ϊ�ָ�����'$'Ϊ����ֹͣ����ǰ6������Ϊ��Ӧ���������������
			//��6��Ϊ���ݷ�Χ�ж�λ��ÿ�������жϵĵ�λ����λ������֮���룺
			/*��ǿ�ȣ�1��������ʾ��������	2����	3�԰�����һ��led��	4�ܰ���������led��
			�¶ȣ�1�ϸ�	2����	3�ϵ�
			ʪ�ȣ�1��ʪ	2����	3����
			���£�1�ϸ�	2���� 3����δ���� 4����
			Ѫ����1���������� 2���Ϊ95������ô������Ч	 3����0��δ�����豸
			if��Ѫ���ж�Ϊ==1������ô����	1�ϵ�	2����	3�ϸ�
			else 4δ�����豸
			��λ��if������Ѫ�����ݳ���Ϊ0|�¶ȳ�������30�ȣ���ʾ���˴����豸��*/
			//+IPD,0,23:[z]lux*123*temp30*humi35*btemp*36*hrate75spo2*99$	��lux�Ȳ����ͣ�
			//��λ��send:   [z]*1234*26*34*37*75*99*1*1*1*1*1*1$        ��λ��receive:+IPD,0,15:[z]*1234*26*34*37*75*99*1*1*1*1*1*1$ 
			//��λ�����ݽ�������:u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 
	if(USART1_RX_STA&0x8000)
		{		
			len=USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			for(t=0;t<len;t++)
				{
					USART_SendData(HT_USART0, USART1_RX_BUF[t]);         //�򴮿�0��������
					while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//�ȴ����ͽ���
				}
			if(USART1_RX_BUF[0]=='+')
			{		
				//�õ�ð��ָ������
				for(colon=1;USART1_RX_BUF[colon]!=':';colon++);
					//����ǲ�����λ�����͵����ݴ�------------------------------------------------------------------------
					if(USART1_RX_BUF[colon+2]=='z')
					{							
							//��ʽ������ʾ
							mid=colon+5;
							//1
							for(i=0;USART1_RX_BUF[mid]!='*';mid++,i++) 
							{
								lux[i]=USART1_RX_BUF[mid];
							  if(i==3) { for(;USART1_RX_BUF[mid]!='*';mid++);break;	}	//����������ʱ�ô����������жϣ��������λ��Խ�磬��ô�Զ��ض����ݱ���ǰ��Чλ��Ȼ���ƶ�midָ�뵽��һ������						
							}	
							LCD_ShowString(15+16*4,105,230,16,16,"                 ");	//����λ���Ӷ����ʱ���������Ļ������ÿ���ÿո�������
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
							//�õ���λ�����							
							upper_monitor_flag=USART1_RX_BUF[5];  //����Ϊ�����������ݣ���λ��������λ��1	
						}	
					}		
			//�����־λ��Ϊ1�Ļ�����ô��ʾ����WIFI��ʼ��δ��ɣ���ô��Ӧ�������ｫUSART1_RX_STA����			
			if(wifi_init_end_flag==1)
			{
			  USART1_RX_STA=0;
			}
			//��λ��Ϊ��һ����----------------------------------------------------
//			  LCD_ShowString(15+16*6,225,230,16,16,"dan");
				upper_monitor_flag=upper_monitor_flag-'0';  //�õ���λ��esp8266���ֱ��
//			  LCD_ShowxNum(15+16*10,225,upper_monitor_flag,2,16,0);					
			//��ո������ݻ�������-------------------------------------------
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
*����1�жϷ�����
*param����
*���ܣ�WIFIģ���շ���˫��λ����
*����ֵ����
**************************
void USART1_IRQHandler(void)
{
	u8 Res;//����������ת����
	u8 len;//�������ݳ���
	u8 t,colon,mid,i;//colon��ʾð�ŵ�λ��
	//�涨��������Э��-----------------------------------------------------------------
	if(USART_GetLineStatus(HT_USART1,USART_LSR_RFDR)!= RESET)//���յ�����
	{
		Res = USART_ReceiveData(HT_USART1);
		if((USART1_RX_STA&0x8000)==0)//����δ���
			{
				if(USART1_RX_STA&0x4000)//���յ���0x0d
				{
					if(Res!=0x0a)USART1_RX_STA=0;//���մ���,���¿�ʼ
					else USART1_RX_STA|=0x8000;	//��������� 
				}
				else //��û�յ�0X0d
				{	
					if(Res==0x0d)USART1_RX_STA|=0x4000;
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res ;
						USART1_RX_STA++;
						if(USART1_RX_STA>(USART1_MAX_RECV_LEN-1))
							{
								USART1_RX_STA=0;//�������������¿�ʼ����	  
							}
					}		 
				}
			}
	 }
	//���ݽ�����ɺ���в�����
	//---[f]Ϊ������[r]Ϊ���ˡ�'*'Ϊ�ָ���----------˫��λ��ͨ�Ÿ�ʽ�涨����--------------------------------------
  //+IPD,0,23:[f]lux*123*temp30*humi35$
	//+IPD,0,23:[r]btemp*36*hrate75spo2*99$    ��lux�Ȳ����ͣ�
	//  send: [f]*12*34*56$  receive:+IPD,0,15:[f]*12*34*56$
	//��λ�����ݽ�������:
	//u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 		
	if(USART1_RX_STA&0x8000)
		{			
			len=USART1_RX_STA&0x3fff;//�õ��˴ν��յ������ݳ���
			for(t=0;t<len;t++)
			{
				USART_SendData(HT_USART0, USART1_RX_BUF[t]);         //�򴮿�0��������
				while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)!=SET);//�ȴ����ͽ���
			}
			if(USART1_RX_BUF[0]=='+')
			{
				//�õ�ð��ָ������
				for(colon=1;USART1_RX_BUF[colon]!=':';colon++);
					//����ǲ�����λ�����͵����ݴ�------------------------------------------------------------------------
					if(USART1_RX_BUF[colon+2]=='f')
					{							
							//��ʽ������ʾ
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
							
						  //�ж�������λ�����							
							upper_monitor_flag=USART1_RX_BUF[5]|0x80;  //����Ϊ�����������ݣ���λ��������λ��1
					
					}			
  				//����ǲ�����λ�����͵����ݴ�----------------------------------------------------------------------------
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

							//�ж�������λ�����							
							upper_monitor_flag=USART1_RX_BUF[5]|0x40;//����Ϊ���˷������ݣ���λ��������λ��0							
					}
 			}		
			//�����־λ��Ϊ1�Ļ�����ô��ʾ����WIFI��ʼ��δ��ɣ���ô��Ӧ�������ｫUSART1_RX_STA����			
			if(wifi_init_end_flag==1)
			{
			  USART1_RX_STA=0;
			}
			//�ж���λ��----------------------------------------------------
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
			//��ո������ݻ�������-------------------------------------------
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
