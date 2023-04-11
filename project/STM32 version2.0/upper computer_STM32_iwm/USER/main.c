#include "sys.h"
#include "delay.h"
#include "silencht_usart.h"	
#include "led.h"
#include "lcd.h"
#include "ESP8266.h"
#include "iwdg.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "rtc.h"
#include "BEEP.h" 
#include "exti.h"

int main(void)
{ 
  u8 colon,mid,i;				//colon��ʾð�ŵ�λ��
	u8 lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag;
	u8 lux[4]={0},temp[5]={0},humi[5]={0},btemp[5]={0},hrate[4]={0},spo2[3]={0}; 		//��λ�����ݽ�������
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);      //��ʼ����ʱ����
	usart1_init(115200);  //���Դ���
	usart2_init(115200);  //WIFI����2��ʼ�� GPIOA2����TX   GPIOA3����RX  
	LED_Init();					  //��ʼ��LED
 	LCD_Init();           //��ʼ��LCD
	EXTIX_Init();					//��ʼ���ⲿ�ж�
  BEEP_Init();				  //BEEP��ʼ��	
	screen_init_background();	//�����ʼ��
	IWDG_Init(6,2000);    //4*2^6=256*2000= 32000 /32=1000ms= 16s
  ESP8266_Init();       //WIFI��ʼ��
	My_RTC_Init();        //RTC��ʼ��
	RTC_Set_WakeUp(RTC_WakeUpClock_CK_SPRE_16bits,0);		//����WAKE UP�ж�,1�����ж�һ��
	POINT_COLOR=RED;      //������ɫ����ɫ
  while(1)
	{		
		IWDG_Feed();//ι��
		LED0=!LED0;	
/*****************************************************************************************************************************


																											����A����λ��

******************************************************************************************************************************/		

		  if(USART2_A_STA&0x8000) //����������A��
			 {	
//����ժȡ---------------------------------------------------------------------------------------------------
					if(USART2_A_BUF[0]=='+') //�жϵ�һ��+��
					{	
					
						for(colon=1;USART2_A_BUF[colon]!=':';colon++);//�õ�ð��ָ������													
						mid=colon+5;//�ƶ�����һ��������λ	
						upper_monitorA_flag=USART2_A_BUF[5];//�õ���λ�����	
						upper_monitorA_flag=upper_monitorA_flag-'0';  //�õ���λ��esp8266���ֱ��
								 //1
									for(i=0;USART2_A_BUF[mid]!='*';i++,mid++) 
									{
										lux[i]=USART2_A_BUF[mid];
										if(i==3) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	//����������ʱ�ô����������жϣ��������λ��Խ�磬��ô�Զ��ض����ݱ���ǰ��Чλ��Ȼ���ƶ�midָ�뵽��һ������						
									}	
									LCD_ShowString(82,100,40,16,16,"     ");	//����λ���Ӷ����ʱ���������Ļ������ÿ���ÿո�������
									LCD_ShowString(82,100,32,16,16,lux);   //��λ����ʾʱ���ڶ���Ī�����������λ����ԭ����ʱ����
									//2
									for(i=0,mid++;USART2_A_BUF[mid]!='*';mid++,i++)
									{
										temp[i]=USART2_A_BUF[mid];
										if(i==4) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}
									LCD_ShowString(82,118,43,16,16,"     ");
									LCD_ShowString(82,118,43,16,16,temp);
									//3
									for(i=0,mid++;USART2_A_BUF[mid]!='*';mid++,i++)
									{
										humi[i]=USART2_A_BUF[mid];
										if(i==4) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}	
									LCD_ShowString(82,136,43,16,16,"     ");
									LCD_ShowString(82,136,43,16,16,humi);
									
									//4
									for(i=0,mid++;USART2_A_BUF[mid]!='*';mid++,i++) 
									{
										btemp[i]=USART2_A_BUF[mid];
										if(i==4) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}               
									LCD_ShowString(82,154,43,16,16,"     ");
									LCD_ShowString(82,154,43,16,16,btemp);
									//5
									for(i=0,mid++;USART2_A_BUF[mid]!='*';mid++,i++)
									{
										hrate[i]=USART2_A_BUF[mid];
										if(i==2) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}
									LCD_ShowString(82,174,43,16,16,"     ");
									LCD_ShowString(82,174,43,16,16,hrate);
									//6
									for(i=0,mid++;USART2_A_BUF[mid]!='*';mid++,i++)
									{
										spo2[i]=USART2_A_BUF[mid];
										if(i==2) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}
									LCD_ShowString(82,194,43,16,16,"     ");	
									LCD_ShowString(82,194,43,16,16,spo2);	
//���ݷ�Χժȡ---------------------------------------------------------------------------------------------------------					
									//1+
									lux_flag=USART2_A_BUF[++mid];
									for(;USART2_A_BUF[mid]!='*';mid++);	
									if(lux_flag=='1'){POINT_COLOR=RED;LCD_ShowString(131,100,104,16,16,"very light!");POINT_COLOR=BLUE;}
									else if(lux_flag=='2')LCD_ShowString(131,100,104,16,16,"   good!   ");
									else if(lux_flag=='3')LCD_ShowString(131,100,104,16,16,"   dark1!  ");
									else if(lux_flag=='4'){POINT_COLOR=RED;LCD_ShowString(131,100,104,16,16,"   dark2!  ");POINT_COLOR=BLUE;}
									else if(lux_flag=='5'){POINT_COLOR=RED;LCD_ShowString(131,100,104,16,16,"   dark3!  ");POINT_COLOR=BLUE;}
									else if(lux_flag=='6'){POINT_COLOR=RED;LCD_ShowString(131,100,104,16,16,"   dark4!  ");POINT_COLOR=BLUE;}
									//2+
									temperature_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='*';mid++);	
									if(temperature_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(131,118,104,16,16,"   hot!");POINT_COLOR=BLUE;}
									else if(temperature_flag=='2')LCD_ShowString(131,118,104,16,16,"   good!");
									else if(temperature_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,118,104,16,16,"   cold!");POINT_COLOR=BLUE;}
									//3+
									humidity_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='*';mid++);	
									if(humidity_flag=='1')		{POINT_COLOR=RED; LCD_ShowString(131,136,104,16,16,"   wet! ");POINT_COLOR=BLUE;}
									else if(humidity_flag=='2')LCD_ShowString(131,136,104,16,16,"   good! ");
									else if(humidity_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,136,104,16,16,"   dry! ");POINT_COLOR=BLUE;}
									//4+
									body_temp_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='*';mid++);
									if(body_temp_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(131,154,104,16,16,"   high!    ");POINT_COLOR=BLUE;}
									else if(body_temp_flag=='2')LCD_ShowString(131,154,104,16,16,"   good!    ");
									else if(body_temp_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,154,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									else if(body_temp_flag=='4'){POINT_COLOR=RED;LCD_ShowString(131,154,104,16,16,"  low fever!");POINT_COLOR=BLUE;}
									//5+
									hrate_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='*';mid++);
									if(hrate_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(131,174,104,16,16,"   low!    ");POINT_COLOR=BLUE;}
									else if(hrate_flag=='2')LCD_ShowString(131,174,104,16,16,"   good!    ");
									else if(hrate_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,174,104,16,16,"   high!    ");;POINT_COLOR=BLUE;}
									else if(hrate_flag=='4'){POINT_COLOR=RED;LCD_ShowString(131,174,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									//6+
									spo2_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='*';mid++);
									if(spo2_flag=='1')		 {POINT_COLOR=RED;LCD_ShowString(131,194,104,16,16,"   low!    ");POINT_COLOR=BLUE;}
									else if(spo2_flag=='2') LCD_ShowString(131,194,104,16,16,"   good!    ");
									else if(spo2_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,194,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									call_flag=USART2_A_BUF[++mid];
										for(;USART2_A_BUF[mid]!='$';mid++);
									if(call_flag=='1')		   { POINT_COLOR=RED;LCD_Fill(7,274,119,313,GREEN); LCD_ShowString(48,280,12,16,24,"1");POINT_COLOR=BLUE;}
									else if(call_flag=='0')  { POINT_COLOR=BLACK;LCD_Fill(7,274,119,313,RED); LCD_ShowString(48,280,12,16,24,"1");BEEP_long_EN();POINT_COLOR=BLUE;}
								//��ո������ݻ�������-------------------------------------------
								memset(USART2_A_BUF,0,sizeof(USART2_A_BUF));
								memset(lux,0,sizeof(lux));
								memset(temp,0,sizeof(temp));
								memset(humi,0,sizeof(humi));
								memset(btemp,0,sizeof(btemp));
								memset(hrate,0,sizeof(hrate));
								memset(spo2,0,sizeof(spo2));
							}	
						  USART2_A_STA=0;
							
				} 

/*****************************************************************************************************************************


																											����B����λ��

******************************************************************************************************************************/			
			else if(USART2_B_STA&0x8000) //����������B��
				{
//����ժȡ-----------------------------------------------------------------------------------------------
					if(USART2_B_BUF[0]=='+') //�жϵ�һ��+��
					{						
						upper_monitorB_flag=USART2_B_BUF[5];//�õ���λ�����	
						upper_monitorB_flag=upper_monitorB_flag-'0';  //�õ���λ��esp8266���ֱ��
					//	u1_printf ("num is %d\r\n",upper_monitorB_flag);
						for(colon=1;USART2_B_BUF[colon]!=':';colon++);//�õ�ð��ָ������													
						mid=colon+5;//�ƶ�����һ��������λ	
								  //1
									for(i=0;USART2_B_BUF[mid]!='*';mid++,i++) 
									{
										btemp[i]=USART2_B_BUF[mid];
										if(i==5) { for(;USART2_A_BUF[mid]!='*';mid++);break;	}	
									}               
									LCD_ShowString(82,214,40,16,16,"     ");
									LCD_ShowString(82,214,40,16,16,btemp);
									//2
									for(i=0,mid++;USART2_B_BUF[mid]!='*';mid++,i++)
									{
										hrate[i]=USART2_B_BUF[mid];
										if(i==2) { for(;USART2_B_BUF[mid]!='*';mid++);break;	}	
									}
									LCD_ShowString(82,234,43,16,16,"     ");
									LCD_ShowString(82,234,43,16,16,hrate);
									//3
									for(i=0,mid++;USART2_B_BUF[mid]!='*';mid++,i++)
									{
										spo2[i]=USART2_B_BUF[mid];
										if(i==2) { for(;USART2_B_BUF[mid]!='*';mid++);break;	}	
									}
									LCD_ShowString(82,254,43,16,16,"     ");	
									LCD_ShowString(82,254,43,16,16,spo2);	
//���ݷ�Χժȡ---------------------------------------------------------------------------------------------------------										
									//1+
									body_temp_flag=USART2_B_BUF[++mid];
										for(;USART2_B_BUF[mid]!='*';mid++);
									if(body_temp_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(131,214,104,16,16,"   high!    ");POINT_COLOR=BLUE;}
									else if(body_temp_flag=='2')LCD_ShowString(131,214,104,16,16,"   good!    ");
									else if(body_temp_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,214,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									else if(body_temp_flag=='4'){POINT_COLOR=RED;LCD_ShowString(131,214,104,16,16,"  low fever!");POINT_COLOR=BLUE;}
									//2+
									hrate_flag=USART2_B_BUF[++mid];
										for(;USART2_B_BUF[mid]!='*';mid++);
									if(hrate_flag=='1')		  {POINT_COLOR=RED;LCD_ShowString(131,234,104,16,16,"   low!    ");POINT_COLOR=BLUE;}
									else if(hrate_flag=='2')LCD_ShowString(131,234,104,16,16,"   good!    ");
									else if(hrate_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,234,104,16,16,"   high!    ");;POINT_COLOR=BLUE;}
									else if(hrate_flag=='4'){POINT_COLOR=RED;LCD_ShowString(131,234,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									//3+
									spo2_flag=USART2_B_BUF[++mid];
										for(;USART2_B_BUF[mid]!='*';mid++);
									if(spo2_flag=='1')		 {POINT_COLOR=RED;LCD_ShowString(131,254,104,16,16,"   low!    ");POINT_COLOR=BLUE;}
									else if(spo2_flag=='2') LCD_ShowString(131,254,104,16,16,"   good!    ");
									else if(spo2_flag=='3'){POINT_COLOR=RED;LCD_ShowString(131,254,104,16,16,"  not wear!");POINT_COLOR=BLUE;}
									call_flag=USART2_B_BUF[++mid];
										for(;USART2_B_BUF[mid]!='$';mid++);
									if(call_flag=='1')		   { POINT_COLOR=RED;LCD_Fill(121,274,233,313,GREEN); LCD_ShowString(168,280,12,16,24,"2");POINT_COLOR=BLUE;}
									else if(call_flag=='0')  { POINT_COLOR=RED;LCD_Fill(121,274,233,313,RED); LCD_ShowString(168,280,12,16,24,"2");BEEP_long_EN();POINT_COLOR=BLUE;}
							}		
						  USART2_B_STA=0;
		//			  LCD_ShowxNum(15+16*10,225,upper_monitorB_flag,2,16,0);					
					//��ո������ݻ�������-------------------------------------------
					memset(USART2_B_BUF,0,sizeof(USART2_B_BUF));
					memset(btemp,0,sizeof(btemp));
					memset(hrate,0,sizeof(hrate));
					memset(spo2,0,sizeof(spo2));				
				 }			 
  		}		 
         
}
