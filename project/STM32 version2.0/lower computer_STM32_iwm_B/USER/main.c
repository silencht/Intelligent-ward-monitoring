#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "IIC.h"
#include "max30102.h" 	
#include "sht3x.h"
#include "wdg.h"
#include "exti.h"
	
//�������ݷ�Χ�ж�λ
	u8   			lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag;
//��������
	void Numerical_judgment(void)	;	
	
//������
 int main(void)
 {	
	delay_init();	    	   //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	usart1_init(115200);	 //�����ô���1
	uart4_init(9600);     //����ģ���ô���4
	u4_printf("%c%c%c%c",102,102,2,86); //��ѯ����¶�ֵ0x66 0x66 0x02 0x56
	usart3_init(115200);	 //wifiģ���ô���3
 	LED_Init ();			     //LED�˿ڳ�ʼ��
  max30102_init();       //����Ѫ��ģ���ʼ��
	Humi_Wind_Init();			 //��ʪ��/���ȳ�ʼ��
  Voice_Init();					 //����������ʼ��
	Beep_Init();					 //��������ʼ��	
	EXTIX_Init();					 //�ⲿ�жϳ�ʼ��
	IWDG_Init(7,1953);      //���ʱ��ԼΪ25s		
	while(1) 
	{		 
			//�������ʺ�Ѫ�����Ͷ�
 	    calcu_heart_spo2();		
			//��ѯ����
			u4_printf("%c%c%c%c",102,102,2,86);
			delay_ms(100);			
			Numerical_judgment();
		u1_printf("[B]*%.2f*%d*%d*%d*%d*%d*%d$\r\n",(body_temp/100),hrAvg,spo2Avg,body_temp_flag,hrate_flag,spo2_flag,call_flag);
//			printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
//    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
		 	u3_printf("[B]*%.2f*%d*%d*%d*%d*%d*%d$!#",(body_temp/100),hrAvg,spo2Avg,body_temp_flag,hrate_flag,spo2_flag,call_flag);
			IWDG_Feed();
				
		}
			 
 } 
 

//											  ��  ��  ʪ �� �� �� ����ʪ������ ��                                     �� ��  ʪ �� �� �� ����ʪ������ ��
	 //��λ��send:   [A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$        ��λ��receive:+IPD,0,15:[A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$ 
	 //��λ��send:    1 ������ʪ�� 2�رռ�ʪ�� 3�������� 4�رշ��� 5���Ѳ��˴����豸
//�жϸ��໷�������������Ƿ���ĳ����Χ��
void Numerical_judgment(void)
{
//----------------����------------------------------------------------	
	if((35<(body_temp/100))&&((body_temp/100)<=37.5))
		{
			body_temp_flag=2;//���������£�
		}
	else if((body_temp/100)<=35)
		{
			body_temp_flag=3;//����δ��������Ȼ���������ˣ�
		}
	else if(38<(body_temp/100))
		{
			body_temp_flag=1;//���½ϸ�
		}
	else if( (37.5<(body_temp/100))	&& ((body_temp/100)<38) )
		{
			body_temp_flag=4;//�����գ�
		}	
//----------------Ѫ��/����-------------------------------------------
	if(93<spo2Avg)spo2_flag=2;//����
	else if(spo2Avg==0)spo2_flag=3;//û�����豸
	else spo2_flag=1;//�����в�
		
	if(spo2Avg!=0){
			if((58<hrAvg)&&(hrAvg<100))hrate_flag=2;//����
			else if(hrAvg<58)hrate_flag=1;//���ʽϵ�
			else	hrate_flag=3;//���ʽϸ�
		}
	else hrate_flag=4;//δ�����豸
}

