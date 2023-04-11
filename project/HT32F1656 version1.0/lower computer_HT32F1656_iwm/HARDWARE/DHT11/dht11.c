#include "dht11.h" 
#include "delay.h" 
#include "ebi_lcd.h"
//pA7 PB10
//��λDHT11
void DHT11_Rst(void)	    
{
	DHT11_IO_OUT(); 	//SET OUTPUT 
	DHT11_DQ_OUT0; 	//����DQ 
	delay_ms(20);    	//��������18ms 
	DHT11_DQ_OUT1; 	//DQ=1  
	delay_us(30);     	//��������20~40us 
}
void DHT11_Rst_1(void)	    
{
	 DHT11_IO1_OUT(); 	//SET OUTPUT 
	DHT11_DQ1_OUT0; 	//����DQ 
	delay_ms(20);    	//��������18ms 
	DHT11_DQ1_OUT1; 	//DQ=1  
	delay_us(30);     	//��������20~40us 
} 
//�ȴ�DHT11�Ļ�Ӧ 
//����1:δ��⵽DHT11�Ĵ��� 
//����0:���� 
u8 DHT11_Check(void) 	    
{    
	u8 retry=0; 
	DHT11_IO_IN();//SET INPUT
	while (DHT11_DQ_IN&&retry<100)//DHT11������40~80us 
	{
		retry++; 
		delay_us(1);
	}
	if(retry>=100)return 1; 
	else retry=0;
	while (!DHT11_DQ_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us 
	{ 
		retry++; 
		delay_us(1); 
	}
	if(retry>=100)return 1;	     
	return 0;
} 
u8 DHT11_Check_1(void) 	    
{    
	u8 retry=0; 
	DHT11_IO1_IN();//SET INPUT
	while (DHT11_DQ1_IN&&retry<100)//DHT11������40~80us 
	{
		retry++; 
		delay_us(1);
	}
	if(retry>=100)return 1; 
	else retry=0;
	while (!DHT11_DQ1_IN&&retry<100)//DHT11���ͺ���ٴ�����40~80us 
	{ 
		retry++; 
		delay_us(1); 
	}
	if(retry>=100)return 1;	     
	return 0;
} 
//��DHT11��ȡһ��λ 
//����ֵ��1/0 
u8 DHT11_Read_Bit(void) 	  
{ 
	u8 retry=0; 
	while(DHT11_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ 
	{ 
		retry++; 
		delay_us(1); 
	} 
	retry=0; 
	while(!DHT11_DQ_IN&&retry<100)//�ȴ���ߵ�ƽ 
	{ 
		retry++; 
		delay_us(1); 
	} 
	delay_us(40);//�ȴ�40us 
	if(DHT11_DQ_IN)return 1; 
	else return 0;	    
} 
u8 DHT11_Read_Bit_1(void) 	  
{ 
	u8 retry=0; 
	while(DHT11_DQ1_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ 
	{ 
		retry++; 
		delay_us(1); 
	} 
	retry=0; 
	while(!DHT11_DQ1_IN&&retry<100)//�ȴ���ߵ�ƽ 
	{ 
		retry++; 
		delay_us(1); 
	} 
	delay_us(40);//�ȴ�40us 
	if(DHT11_DQ1_IN)return 1; 
	else return 0;	    
} 
//��DHT11��ȡһ���ֽ� 
//����ֵ������������ 
u8 DHT11_Read_Byte(void)     
{         
	u8 i,dat; 
	dat=0; 
	for (i=0;i<8;i++)  
	{ 
		dat<<=1;  
		dat|=DHT11_Read_Bit(); 
	}	     
	return dat; 
} 
u8 DHT11_Read_Byte_1(void)     
{         
	u8 i,dat; 
	dat=0; 
	for (i=0;i<8;i++)  
	{ 
		dat<<=1;  
		dat|=DHT11_Read_Bit_1(); 
	}	     
	return dat; 
} 
//��DHT11��ȡһ������ 
//temp:�¶�ֵ(��Χ:0~50��) 
//humi:ʪ��ֵ(��Χ:20%~90%) 
//����ֵ��0,����;1,��ȡʧ�� 
u8 DHT11_Read_Data(u8 *temp,u8 *humi)
{         
	u8 buf[5]; 
	u8 i; 
	DHT11_Rst(); 
	if(DHT11_Check()==0) 
	{ 
		for(i=0;i<5;i++)//��ȡ40λ���� 
		{ 
			buf[i]=DHT11_Read_Byte(); 
		} 
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]) 
		{ 
			*humi=buf[0]; 
			*temp=buf[2]; 
		} 
	}else return 1; 
	return 0;	     
} 
u8 DHT11_Read_Data_1(u8 *temp,u8 *humi)
{
	u8 buf[5]; 
	u8 i; 
	DHT11_Rst_1(); 
	if(DHT11_Check_1()==0) 
	{ 
		for(i=0;i<5;i++)//��ȡ40λ���� 
		{ 
			buf[i]=DHT11_Read_Byte_1(); 
		} 
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4]) 
		{ 
			*humi=buf[0]; 
			*temp=buf[2]; 
		} 
	}else return 1; 
	return 0;	     
}
u8 DHT11_ReadData1(u8 *temp,u8 *humi)    
{
 	u8 buf[5];
	u8 i;
	DHT11_Rst();
	if(DHT11_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[1];
			*temp=buf[3];
		}
	}else return 1;
	return 0;
}
u8 DHT11_ReadData1_1(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_Rst_1();
	if(DHT11_Check_1()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
		{
			buf[i]=DHT11_Read_Byte_1();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[1];
			*temp=buf[3];
		}
	}else return 1;
	return 0;
}
//��ʼ��DHT11��IO�� DQ ͬʱ���DHT11�Ĵ��� 
//����1:������ 
//����0:����    	
void DHT11_Init(void) //PA7
{ 
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PA        = 1;
  CKCUClock.Bit.AFIO      = 1;
  CKCUClock.Bit.BKP       = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_7, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_OUT);
	GPIO_InputConfig(HT_GPIOA,  GPIO_PIN_7, ENABLE);
	GPIO_SetOutBits(HT_GPIOA, GPIO_PIN_7);
	
  CKCUClock.Bit.PB         = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.BKP        = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PB, AFIO_PIN_10, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_10, GPIO_DIR_OUT);
	GPIO_InputConfig(HT_GPIOB,  GPIO_PIN_10, ENABLE);
	GPIO_SetOutBits(HT_GPIOB, GPIO_PIN_10);
}
/************************��ʪ����ʾ********************************/
void wenshidu(void)
{
		float NA;
		u32 luona,xinxin;
		u8 temperature,temperature1;  	    
		u8 humidity,humidity1; 		
		float NA1;
		u32 luona1,xinxin1;
		u8 temperature_1,temperature1_1;  	    
		u8 humidity_1,humidity1_1; 	    
		DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ
	  DHT11_ReadData1(&temperature1,&humidity1);	
    LCD_ShowString(10,250,200,16,16,"temp1:");
		LCD_ShowxNum(40+18,250,temperature,2,16,0);		//��ʾ�¶�
		LCD_ShowxNum(40+33+10,250,temperature1,2,16,0);
		LCD_ShowString(40+35,250,200,16,16,".");
		LCD_ShowString(10,270,200,16,16,"humi1:");
		LCD_ShowxNum(35+8+14,270,humidity,2,16,0);			//��ʾʪ��	 	   
		LCD_ShowString(35+25+14,270,200,16,16,".");
		LCD_ShowxNum(35+33+14,270,humidity1,2,16,0);
		NA=(temperature/10.0)*0.4-(humidity/100.0)*8.34+15;
    
//    LCD_ShowString(10,290,200,16,16,"compa1:");    
//    LCD_ShowString(88,290,200,16,16,".");
	    xinxin=NA*10;luona=xinxin%10;
//		 LCD_ShowxNum(70,290,NA/1.0,2,16,0);
//          LCD_ShowxNum(95,290,luona,1,16,0);
//	 	  delay_ms(500);  
          //�ڶ���
		DHT11_Read_Data_1(&temperature_1,&humidity_1);		//��ȡ��ʪ��ֵ
		DHT11_ReadData1_1(&temperature1_1,&humidity1_1);	
		LCD_ShowString(110,250,200,16,16,"temp2:");
		LCD_ShowxNum(140+18,250,temperature_1,2,16,0);		//��ʾ�¶�
		LCD_ShowxNum(140+33+10,250,temperature1_1,2,16,0);
		LCD_ShowString(140+35,250,200,16,16,".");
		LCD_ShowString(110,270,200,16,16,"humi2:");
		LCD_ShowxNum(135+8+14,270,humidity_1,2,16,0);			//��ʾʪ��	 	   
		LCD_ShowString(135+25+14,270,200,16,16,".");
		LCD_ShowxNum(135+33+14,270,humidity1_1,2,16,0);
		NA1=(temperature_1/10.0)*0.4-(humidity_1/100.0)*8.34+15;
//    LCD_ShowString(110,290,200,16,16,"compa2:");
//    LCD_ShowString(188,290,200,16,16,".");
	  xinxin1=NA1*10;luona1=xinxin1%10;
//		 LCD_ShowxNum(170,290,NA1/1.0,2,16,0);
//          LCD_ShowxNum(195,290,luona1,1,16,0);
	 	delay_ms(500);
//        if(temperature>50||humidity>70)//��������
//        {
//             LCD_ShowString(10,90,200,16,16,"please leave the class");
//              GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_14);//����            
//        }
// float NA;
//	    u32 luona,xinxin;
//	    u8 temperature,temperature1;  	    
//	    u8 humidity,humidity1; 				  
//			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ
//	    DHT11_ReadData1(&temperature1,&humidity1);	
//         LCD_ShowString(5,250,200,16,16,"temp1��");
//			LCD_ShowxNum(105+18,250,temperature,2,16,0);		//��ʾ�¶�
//			LCD_ShowxNum(105+33+10,250,temperature1,2,16,0);
//	    LCD_ShowString(105+35,250,200,16,16,".");
//     	LCD_ShowString(5,270,200,16,16,"humi1��");
//	    LCD_ShowxNum(95+8+14,270,humidity,2,16,0);			//��ʾʪ��	 	   
//			LCD_ShowString(95+25+14,270,200,16,16,".");
//	    LCD_ShowxNum(95+33+14,270,humidity1,2,16,0);
//	    NA=(temperature/10.0)*0.4-(humidity/100.0)*8.34+15;
//	    LCD_ShowString(122+20,290,200,16,16,".");
//	    xinxin=NA*10;luona=xinxin%10;
//		 LCD_ShowxNum(105+20,290,NA/1.0,2,16,0);
//          LCD_ShowxNum(130+20,290,luona,1,16,0);
//	 	  delay_ms(500);  
}
