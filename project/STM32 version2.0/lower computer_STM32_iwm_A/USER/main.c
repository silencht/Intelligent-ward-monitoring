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
	
//��ǿģ�����ñ���
	u8  raw_data[13]={0};
	uint16_t data_16[2]={0};
	uint32_t Lux,Lux_last;
//��ʪ��ģ�����ñ���
etError   error;        // �����־
u32t      serialNumber; // ���к�
float     temperature;  // �¶�
float     humidity;     // ʪ��[%RH]
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
	I2C_GPIO_Config();	   //��ǿ��ģ�����ų�ʼ��
  max30102_init();       //����Ѫ��ģ���ʼ��
	EXTIX_Init();					 //�ⲿ�жϳ�ʼ��
//	Humi_Wind_Init();			 //��ʪ��/���ȳ�ʼ��
//  Voice_Init();					 //����������ʼ��
//	Beep_Init();					 //��������ʼ��
	Light4_Init();				 //��������*4Led��ʼ��
  SHT3X_Init(0x44); 		 // Address: 0x44 = Sensor on EvalBoard connector         0x45 = Sensor on EvalBoard	
	error = SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_1HZ);	// ��ʼ���ڲ���-->���ظ��Ժ�ÿ��1�β���												
	IWDG_Init(7,1953);      //���ʱ��ԼΪ25s		
	while(1) 
	{		 
			//�����ǿ��
			one:
			Lux_last=Lux;
			delay_ms(10);
			Single_ReadI2C(0xb6,0x00,raw_data,4);
			data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];//ǰ�߰�λ����8��ǰ�Ͱ�λ��������һ�𣬷�ֵ��data_16[0]
			data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];//��߰�λ����8����Ͱ�λ��������һ�𣬷�ֵ��data_16[1]
			Lux=(((uint32_t)data_16[0])<<16)|data_16[1];			  //ǰʮ��λ����16����ʮ��λ��������һ�𣬷�ֵ��Lux
			if(Lux==0 || Lux==Lux_last) goto one;		
//			u3_printf ("[A]*%d\r\n",Lux);
			//������/ʪ��
			two:
   		SHT3X_ReadMeasurementBuffer(&temperature, &humidity);// ��ȡ��������������ȡ��ʪ�ȵ�����
			if(temperature==0) goto two;
//			u1_printf ("[A]*%.2f,%.2f$\r\n",temperature,humidity);
//			//�������ʺ�Ѫ�����Ͷ�
 	    calcu_heart_spo2();		
 //  		u1_printf ("[A]*%d,%d$\r\n",hrAvg,spo2Avg);
 //			//��ѯ����
			u4_printf("%c%c%c%c",102,102,2,86); //���ڶ���ѯ����¶�ֵ0x66 0x66 0x02 0x56 
			 Numerical_judgment();

//			printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
//    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
		 	u3_printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
			IWDG_Feed();
				
		}
			 
 } 
 
//�жϸ��໷�������������Ƿ���ĳ����Χ��


	//����λ��ͨ�Ÿ�ʽ�涨���£�
	//��6��Ϊ���ݷ�Χ�ж�λ��ÿ�������жϵĵ�λ����λ������֮���룺
	
	/*��ǿ�ȣ�  [1]����  [2]����	[3] dark1	[4] dark2  [5]dark3  [6]dark4
			  �¶ȣ�[1]�ϸ� open wind[2]����	[3]�ϵ�
			  ʪ�ȣ�[1]��ʪ	[2]����	[3]���� open humi
			  ���£�[1]�ϸ�	[2]����	[3]δ���� [4]����
			  Ѫ����[1]���������� [2]���Ϊ94������ô������Ч	 [3]����0��δ�����豸
					if��Ѫ���ж�Ϊ==1��
						��ô����:[1]�ϵ�	[2]����	[3]�ϸ�
					else [4]δ�����豸
					
						 ��  ��  ʪ �� �� �� ����ʪ������ ��                                     �� ��  ʪ �� �� �� ����ʪ������ ��
	 //��λ��send:   [A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$        ��λ��receive:+IPD,0,15:[A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$ 
	 //��λ��send:    1 ������ʪ�� 2�رռ�ʪ�� 3�������� 4�رշ��� 5���Ѳ��˴����豸
	 */
void Numerical_judgment(void)
{
//----------------���ն�----------------------------------------------
	if( (100<(Lux/100)) && ((Lux/100)<999) ){
			lux_flag =2;//����
			En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);					
		}
	else if( (75<(Lux/100)) && ((Lux/100)<=100) ){
			lux_flag =3;//dark1
			if(close_led){En_Ledx(1,SET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
			else En_Ledx(1,RESET);
		}
	else if( (50<(Lux/100)) &&((Lux/100)<=75) ){
			lux_flag =4;//dark2
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);}
		}
	else if((25<(Lux/100)) &&((Lux/100)<=50)){
			lux_flag =5;//dark3
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,SET);En_Ledx(4,RESET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);}
		}
	else if(((Lux/100)<=25)){
			lux_flag =6;//dark4
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,SET);En_Ledx(4,SET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
	  }
	else{
			lux_flag =1;//very light
			En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);
		}
		
//----------------�¶�------------------------------------------------	
	if((18<temperature)&&(temperature<28))temperature_flag=2;//����
	else if(temperature<18)temperature_flag=3;//�ϵ�
	else temperature_flag=1; //�ϸ�
	
//----------------ʪ��------------------------------------------------	
	if((42<humidity)&&(humidity<68))humidity_flag=2;//����
	else if(humidity<42)humidity_flag=3;//����
	else humidity_flag=1;//��ʪ
	
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

