/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.c
*	��    �� : V1.0
*	˵    �� : ��gpioģ��i2c����, ������STM32F4ϵ��CPU����ģ�鲻����Ӧ�ò�����֡��������I2C���߻�������������
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2015-05-21 armfly  ��ʽ����
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

/*
	Ӧ��˵����
	�ڷ���I2C�豸ǰ�����ȵ��� i2c_CheckDevice() ���I2C�豸�Ƿ��������ú���������GPIO
*/
#include "HT32.h"
#include "myiic.h"
#include "delay.h"
#include "sht3x.h"



u8 in_out=0;
/*
*********************************************************************************************************
*	�� �� ��: bsp_InitI2C OUT
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��  D2:SCL    D3:SDA
*	��    ��: ��
*	�� �� ֵ: in_out=0
*********************************************************************************************************
*/
void bsp_InitI2C_OUT(void)
{	
		CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
		CKCUClock.Bit.PD                    = 1;
		CKCUClock.Bit.BKP                   = 1;
		CKCU_PeripClockConfig(CKCUClock, ENABLE);

//  /* Configure GPIO direction of output pins                            */
		GPIO_DirectionConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_DIR_OUT);//
  	GPIO_OpenDrainConfig(HT_GPIOD,PIN_I2C_SCL|PIN_I2C_SDA, ENABLE);
		in_out=0;
}
/*********************************************************************************************************
*	�� �� ��: bsp_InitI2C IN
*	����˵��: ����I2C���ߵ�GPIO������ģ��IO�ķ�ʽʵ��  D2:SCL    D3:SDA
*	��    ��: ��
*	�� �� ֵ: in_out=1
*********************************************************************************************************/
void bsp_InitI2C_IN(void)
  {
		CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
		CKCUClock.Bit.PD                    = 1;
		CKCUClock.Bit.BKP                   = 1;
		CKCUClock.Bit.AFIO                  = 1;
		CKCU_PeripClockConfig(CKCUClock, ENABLE);

		AFIO_GPDConfig(AFIO_PIN_2|AFIO_PIN_3, AFIO_MODE_DEFAULT);
		/* Configure GPIO direction of input pins                                                                 */
		GPIO_DirectionConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_DIR_IN);
		/* Configure GPIO pull resistor of input pins                                                             */
//	  GPIO_PullResistorConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_PR_UP);
		GPIO_InputConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, ENABLE);
		in_out=1;
	}
/*
*********************************************************************************************************
*	�� �� ��: i2c_Delay
*	����˵��: I2C����λ�ӳ٣����400KHz
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;
	/*��
		CPU��Ƶ168MHzʱ�����ڲ�Flash����, MDK���̲��Ż�����̨ʽʾ�����۲Ⲩ�Ρ�
		ѭ������Ϊ5ʱ��SCLƵ��  = 1.78MHz (����ʱ: 92ms, ��д������������ʾ����̽ͷ���ϾͶ�дʧ�ܡ�ʱ��ӽ��ٽ�)
		ѭ������Ϊ10ʱ��SCLƵ�� = 1.1MHz (����ʱ: 138ms, ���ٶ�: 118724B/s)
		ѭ������Ϊ30ʱ��SCLƵ�� = 440KHz�� SCL�ߵ�ƽʱ��1.0us��SCL�͵�ƽʱ��1.2us

		��������ѡ��2.2Kŷʱ��SCL������ʱ��Լ0.5us�����ѡ4.7Kŷ����������Լ1us

		ʵ��Ӧ��ѡ��400KHz���ҵ����ʼ���
	*/
	for (i = 0; i < 30; i++);
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Start
*	����˵��: CPU����I2C���������ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Start(void)
{
if(in_out==0)
		{
		/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
			I2C_SDA_1();
			    delay_us(10);
			I2C_SCL_1();
//		  i2c_Delay();
      delay_us(10);
			I2C_SDA_0();
//			i2c_Delay();	
			delay_us(100);		
			I2C_SCL_0();
//			i2c_Delay();
			delay_us(100);
		}
else 
		{
    bsp_InitI2C_OUT();
		/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C���������ź� */
			I2C_SDA_1();
			    delay_us(10);
			I2C_SCL_1();
//		  i2c_Delay();
      delay_us(10);
			I2C_SDA_0();
//			i2c_Delay();	
			delay_us(100);		
			I2C_SCL_0();
//			i2c_Delay();
			delay_us(100);
		}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Stop
*	����˵��: CPU����I2C����ֹͣ�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Stop(void)
{	
	if(in_out==0)
		{
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
			I2C_SDA_0();
			I2C_SCL_1();
//			i2c_Delay();
      delay_us(100);
			I2C_SDA_1();
//			i2c_Delay();
			delay_us(100);
		}
else 
		{
    bsp_InitI2C_OUT();
	/* ��SCL�ߵ�ƽʱ��SDA����һ�������ر�ʾI2C����ֹͣ�ź� */
			I2C_SDA_0();
			I2C_SCL_1();
//			i2c_Delay();
      delay_us(100);
			I2C_SDA_1();
//			i2c_Delay();
			delay_us(100);
		}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    ��:  _ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
		uint8_t i;
if(in_out==0)
		{	
			/* �ȷ����ֽڵĸ�λbit7 */
			for (i = 0; i < 8; i++)
			{
				if (_ucByte & 0x80)
				{
					I2C_SDA_1();
				}
				else
				{
					I2C_SDA_0();
				}
//				i2c_Delay();
        delay_us(10);
				I2C_SCL_1();
//				i2c_Delay();	
        delay_us(50);//�ȴ��ӻ�����					
				I2C_SCL_0();
				if (i == 7)
				{
					 I2C_SDA_1(); // �ͷ�����
				}
				_ucByte <<= 1;	/* ����һ��bit */
//				i2c_Delay();
			}
			
//				I2C_SDA_1();	/* CPU�ͷ�SDA���� */
			  i2c_Delay();
			  I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
				
		}
else
		{
		 bsp_InitI2C_OUT();

		/* �ȷ����ֽڵĸ�λbit7 */
			for (i = 0; i < 8; i++)
			{
				if (_ucByte & 0x80)
				{
					I2C_SDA_1();
				}
				else
				{
					I2C_SDA_0();
				}
//				i2c_Delay();
        delay_us(10);
				I2C_SCL_1();
//				i2c_Delay();	
        delay_us(50);//�ȴ��ӻ�����					
				I2C_SCL_0();
				if (i == 7)
				{
					 I2C_SDA_1(); // �ͷ�����
				}
				_ucByte <<= 1;	/* ����һ��bit */
				i2c_Delay();
			}
			
//				I2C_SDA_1();	/* CPU�ͷ�SDA���� */
//			  i2c_Delay();
			  I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
		}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_SHT30_SendByte
*	����˵��: CPU��I2C�����豸����8bit����
*	��    ��:  _ucByte �� �ȴ����͵��ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t i2c_SHT30_SendByte(uint8_t _ucByte)
{
		uint8_t     mask,err=3;
if(in_out==0)
		{		
				for(mask = 0x80; mask > 0; mask >>= 1)		// shift bit for masking (8 times)
				{
					if((mask & _ucByte) == 0) {I2C_SDA_0(); } 	//���������ͣ�����0--printf("%ld\r\n",SDA_LOW());b:10001000
					else                     {I2C_SDA_1();}	//���������ߣ�����1--printf("%ld\r\n",SDA_OPEN());
					delay_us(10);
					I2C_SCL_1();	  //ʱ�������ߣ����ߴӻ����������ݷ������
					delay_us(50);   //��ƽ����ʱ�䣬�ȴ��ӻ���������
					I2C_SCL_0();	  //ʱ�������ͣ����ߴӻ���������Ҫ��������
					delay_us(10);   //��ƽ����ʱ�䣬�ȴ�ʱ�����ȶ�
				}
				I2C_SDA_1(); 		  //����������
				I2C_SCL_1();			//ʱ�������ߣ����ߴӻ����������ڿ�ʼ��ȡ����
				delay_us(100);                 					// data set-up time (t_SU;DAT)
	      err=i2c_SHT30_WaitAck();      //���ӻ��Ƿ�Ӧ��
				bsp_InitI2C_OUT();
		    I2C_SCL_0();
        delay_us(10);
				return err;
		}
else
		{
		    bsp_InitI2C_OUT();
				for(mask = 0x80; mask > 0; mask >>= 1)		// shift bit for masking (8 times)
				{
					if((mask & _ucByte) == 0) {I2C_SDA_0(); } 	//���������ͣ�����0--printf("%ld\r\n",SDA_LOW());
					else                     {I2C_SDA_1();;}	//���������ߣ�����1--printf("%ld\r\n",SDA_OPEN());
					delay_us(10);
					I2C_SCL_1();	  //ʱ�������ߣ����ߴӻ����������ݷ������
					delay_us(50);   //��ƽ����ʱ�䣬�ȴ��ӻ���������
					I2C_SCL_0();	  //ʱ�������ͣ����ߴӻ���������Ҫ��������
					delay_us(10);   //��ƽ����ʱ�䣬�ȴ�ʱ�����ȶ�
				}
				I2C_SDA_1(); 		  //����������
				I2C_SCL_1();			//ʱ�������ߣ����ߴӻ����������ڿ�ʼ��ȡ����
				delay_us(100);                 					// data set-up time (t_SU;DAT)
				delay_us(200);
				err=i2c_SHT30_WaitAck();      //���ӻ��Ƿ�Ӧ��
				bsp_InitI2C_OUT();
		    I2C_SCL_0();
        delay_us(10);
				return err;
		}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_ReadByte
*	����˵��: CPU��I2C�����豸��ȡ8bit����
*	��    ��:  
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(etI2cAck ack)
{
		uint8_t i;
		uint8_t value;
		bsp_InitI2C_OUT();
		/* ������1��bitΪ���ݵ�bit7 */
		value = 0;
		
		for (i = 0; i < 8; i++)
		{
			value <<= 1;
			I2C_SCL_0();
//		  i2c_Delay();//ԭ����forѭ�����
      delay_us(20); //�ȴ�ʱ�����ȶ�
			
			I2C_SCL_1();
//			i2c_Delay();
			delay_us(30); 
			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())
			{
				value++;
			}
		  bsp_InitI2C_OUT();
      delay_us(20);

		}
		I2C_SCL_0();
		
		if(ack == ACK) 
		{
		I2C_SDA_0();	  
		delay_us(10);                  // data set-up time (t_SU;DAT)
		I2C_SCL_1();                    // clk #9 for ack
		delay_us(50);                  // SCL high time (t_HIGH)
		I2C_SCL_0();
		I2C_SDA_1();                    // release SDA-line
		delay_us(200);  
		}                              // send acknowledge if necessary printf("RxByte=0x%x  ACK\r\n",*rxByte);
		else if(ack == NACK)
		{
		I2C_SDA_1();
		delay_us(10);                  // data set-up time (t_SU;DAT)
		I2C_SCL_1();                    // clk #9 for ack
		delay_us(50);                  // SCL high time (t_HIGH)
		I2C_SCL_0();
		I2C_SDA_1();                    // release SDA-line
		delay_us(200);  
		}
		else{;}
		
//		else           {I2C_SDA_1();i2c_Delay();I2C_SCL_1();i2c_Delay();I2C_SCL_0();I2C_SDA_1();i2c_Delay();}						 //printf("RxByte=0x%x  NACK\r\n",*rxByte);
		
		return value;
	}

/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    ��:  ��
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
			uint8_t re;
//      bsp_InitI2C_OUT();			
//			I2C_SDA_1();	/* CPU�ͷ�SDA���� */
//			i2c_Delay();
//			I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
			i2c_Delay();
			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
			{
				re = 1;
			}
			else
			{
				re = 0;
			}
		  bsp_InitI2C_OUT();
			I2C_SCL_0();
			i2c_Delay();
			return re;
}
/*
*********************************************************************************************************
*	�� �� ��: i2c_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    ��:  ��
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
uint8_t i2c_SHT30_WaitAck(void)
{
			uint8_t re;
//      bsp_InitI2C_OUT();			
//			I2C_SDA_1();	/* CPU�ͷ�SDA���� */
//			i2c_Delay();
//			I2C_SCL_1();	/* CPU����SCL = 1, ��ʱ�����᷵��ACKӦ�� */
			i2c_Delay();			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())	/* CPU��ȡSDA����״̬ */
			{
				re = 1;
			}
			else
			{
				re = 0;
			}
			return re;
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_Ack
*	����˵��: CPU����һ��ACK�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_Ack(void)
{
if(in_out==0)
	{
		I2C_SDA_0();	/* CPU����SDA = 0 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU����1��ʱ�� */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
		I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	}
else
	{  
	  bsp_InitI2C_OUT();
	 	I2C_SDA_0();	/* CPU����SDA = 0 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU����1��ʱ�� */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
		I2C_SDA_1();	/* CPU�ͷ�SDA���� */
	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_NAck
*	����˵��: CPU����1��NACK�ź�
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void i2c_NAck(void)
{	
	if(in_out==0)
	{
		I2C_SDA_1();	/* CPU����SDA = 1 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU����1��ʱ�� */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
	}
else
	{  
		bsp_InitI2C_OUT();
		I2C_SDA_1();	/* CPU����SDA = 1 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU����1��ʱ�� */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();	
	}
}

/*
*********************************************************************************************************
*	�� �� ��: i2c_CheckDevice
*	����˵��: ���I2C�����豸��CPU�����豸��ַ��Ȼ���ȡ�豸Ӧ�����жϸ��豸�Ƿ����
*	��    ��:  _Address���豸��I2C���ߵ�ַ
*	�� �� ֵ: ����ֵ 0 ��ʾ��ȷ�� ����1��ʾδ̽�⵽
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
if(in_out==1)
	{
		if (I2C_SDA_READ() && I2C_SCL_READ())
		{
			i2c_Start();		/* ���������ź� */

			/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
			i2c_SendByte(_Address | I2C_WR);
			ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

			i2c_Stop();			/* ����ֹͣ�ź� */

			return ucAck;
		}
		return 1;	/* I2C�����쳣 */
	}
else
	{  
		bsp_InitI2C_IN();
		if (I2C_SDA_READ() && I2C_SCL_READ())
		{
			i2c_Start();		/* ���������ź� */

			/* �����豸��ַ+��д����bit��0 = w�� 1 = r) bit7 �ȴ� */
			i2c_SendByte(_Address | I2C_WR);
			ucAck = i2c_WaitAck();	/* ����豸��ACKӦ�� */

			i2c_Stop();			/* ����ֹͣ�ź� */

			return ucAck;
		}
		return 1;	/* I2C�����쳣 */
	}

}


/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
