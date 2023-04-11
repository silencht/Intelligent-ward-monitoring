/* ---------------����ͷ�ļ�--------------------------------------------*/
#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ebi_lcd.h"
#include "usart.h"
#include "max30102.h"
#include "algorithm.h"
#include "myiic.h"
#include "sht3x.h"
#include "ESP8266.h"
#include "wdt.h"
#include "timer.h"
#include "voice_light.h"
#include "exti.h"
/* ---------------��λ����������˵��----------------------------------------
   	 ����λ��ͨ��Э�顢����ϸ�����DOC��>readme.txt
--------------------------------------------------------------------------*/

/* ---------------�Զ��������---------------------------------------------*/
#define NVIC_PRIORITY_GROUP_3          3 /* ��ռ���ȼ���Preemption: 4 bits / ��Ӧ���ȼ���Subpriority: 0 bits */

/*----------------------�����������Զ������-----------------------------------------*/

			void Numerical_judgment(void)	;	
			
  /*--------MAX30102(����Ѫ��ģ��)	-------*/
			u8       raw_data[13]={0};
			uint16_t data_16[2]={0};
			uint32_t Lux;
			extern uint8_t uch_dummy;			
	/*--------sht30(��ʪ��ģ��)-------------*/
			ft     temperature=0;  // �¶�
			ft     humidity=0;     // ʪ��[%RH]
	/*--------�������ݷ�Χ�ж�λ-------------*/
	    uint8_t    lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag;
			uint8_t    voice_flag=0;//��������ģ�����
			uint8_t		 waring_flag=0;
			
/*------------------------------------------------------------------------------
	��������main������
	�����������
	����ֵ����
*------------------------------------------------------------------------------*/
int main(void)
{   			
  /*--------esp8266-wifiģ��)-------------*/
			USART0_RX_STA = 0;	//����0��1��������״̬��־λ����ؿ����崦��
			USART1_RX_STA = 0;  
			
/*----------------------����NVIC���ȼ�------------------------------------------*/

	NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_3);		//�ж����ȼ�����
	NVIC_SetPriority(USART0_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 1, 0));
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 0, 0));

/*----------------------��ʼ������----------------------------------------------*/

	usart0_Init(115200);			//����0��WIFI�ͻ��ˣ���ʼ��
	usart1_Init(9600);			  //����1(����ģ��)��ʼ��
	delay_init();							//��ʱ������ʼ��
  I2C0_Init();						  //Ӳ��IIC0��ʼ��
	LED_Init();								//LED�Ƴ�ʼ��
	BEEP_Init();							//��������ʼ��
	VOICE_Init();							//����������ʼ��
	bsp_InitI2C_OUT();        //GPIOģ��IIC��������ܺ�����ʼ������Ҫ��ȡ�ܽŵ�ƽ����������빦�ܺ�����ʼ��
	LCD_Init();								//LCD��ʼ��		
  EXTI10_init ();           //�ⲿ�ж�10������ҽ����ť1�жϣ���ʼ��
  EXTI11_init ();           //�ⲿ�ж�11������ҽ����ť2�жϣ���ʼ��
	HUMI_init();							//��ʪ����ʼ��
	SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_10HZ);//SHT30��ʪ��ģ��ģʽ�������
	
	maxim_max30102_reset();   //���� MAX30102	
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
  maxim_max30102_init();    //initialize the MAX30102
	max30102_int_line();      //�ж���PD4�����뺯����ʼ��
	
	WDT_Init();               //���Ź���ʼ��
	delay_ms(1000);						//WDT����1s
	BEEP_EN();								//����������
	WDT_Restart();					  // Reload Counter as WDTV Value��ι��������
/*--------------��������ѭ����--------------------------------------------*/	
	while(1)
	  
/*--------------����������ѭ�������---------------------------------------*/	
		{
		  /*-------��ǿ��ģ�����-----------------*/	
				close_wdt_led();			
				I2C_BufferRead(raw_data,0x00,4); 
				data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];
				data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
				Lux=(((uint32_t)data_16[0])<<16)|data_16[1];
		  /*--------sht30(��ʪ��ģ��)-------------*/
			  restart:bsp_InitI2C_OUT();
			  while(SHT3X_ReadMeasurementBuffer(&temperature, &humidity));
        if(humidity!=100);
			  else goto restart;	//����goto��ģ���������еĴ����飬�����ݴ������¼��				
			/*--------MAX30102(����Ѫ��ģ��)	-------*/
					max30102_loop();
  				Numerical_judgment();
  				u0_printf ("[z]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
  				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag0|call_flag1);

			    WDT_Restart();				// ι��
		}
}





//�жϸ��໷�������������Ƿ���ĳ����Χ��
void Numerical_judgment(void)
{
//----------------���ն�----------------------------------------------
	if( (80<(Lux/100)) && ((Lux/100)<999) ){
			lux_flag =2;//����
			LED1_DISEN();
			LED2_DISEN();
		}
	else if( (35<=(Lux/100)) && ((Lux/100)<=80) ){
			lux_flag =3;//�ϰ�
			LED1_EN ();
			LED2_DISEN();
		}
	else if( ((Lux/100)<=35) ){
			lux_flag =4;//�ϰ�
			LED1_EN ();
			LED2_EN ();
		}
	else {
		lux_flag =1;//����
		LED1_DISEN();
		LED2_DISEN();
		}
		
//----------------�¶�------------------------------------------------	
	if((18<temperature)&&(temperature<28))temperature_flag=2;//����
	else if(temperature<18)temperature_flag=3;//�ϵ�
	else temperature_flag=1;//�ϸ�
	
//----------------ʪ��------------------------------------------------	
	if((42<humidity)&&(humidity<68))humidity_flag=2;//����
	else if(humidity<42)humidity_flag=3;//����
	else humidity_flag=1;//��ʪ
	
//----------------����------------------------------------------------	
	if((35<(body_temp/100))&&((body_temp/100)<37.5))
		{
			body_temp_flag=2;//���������£�
		}
	else if((body_temp/100)<=35)
		{
			body_temp_flag=3;//����δ��������Ȼ���������ˣ�
			voice_flag++;
		}
	else if(38<(body_temp/100))
		{
			body_temp_flag=1;//���½ϸ�
			waring_flag++;
			if(waring_flag==6)
				{
					BEEP_EN();
					waring_flag=0;
				}
		}
	else if( (37.5<(body_temp/100))	&& ((body_temp/100)<38) )
		{
			body_temp_flag=4;//�����գ�
		}
		
//----------------Ѫ��/����-------------------------------------------
	if(95<spo2Avg)spo2_flag=2;//����
	else if(spo2Avg==0)spo2_flag=3;//û�����豸
	else 
		{
			spo2_flag=1;//�����в�
				waring_flag++;
				if(waring_flag==6)
					{
						BEEP_EN();
						waring_flag=0;
					}
		}
		
	if(spo2Avg!=0){
			if((58<hrAvg)&&(hrAvg<100)){
					hrate_flag=2;//����
				}
			else if(hrAvg<58){
				hrate_flag=1;//���ʽϵ�
					waring_flag++;
					if(waring_flag==10)
						{
							BEEP_EN();
							waring_flag=0;
						}
				}
			else{
				hrate_flag=3;//���ʽϸ�
					waring_flag++;
					if(waring_flag==10)
						{
							BEEP_EN();
							waring_flag=0;
						}
				}
	}
	else{
		hrate_flag=4;//δ�����豸
		voice_flag++;
	}
}
