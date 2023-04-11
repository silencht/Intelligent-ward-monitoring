#include "i2c_hal.h"

// SDA on PB7
#define SDA_LOW()  IIC_SDA=0 // set SDA to low
#define SDA_OPEN() IIC_SDA=1 // set SDA to open-drain
#define SDA_READ   READ_SDA  // read SDA

// SCL on PB6           						/* -- adapt the defines for your uC -- */
#define SCL_LOW()  IIC_SCL=0 // set SCL to low
#define SCL_OPEN() IIC_SCL=1 // set SCL to open-drain
#define SCL_READ   READ_SCL  // read SCL


//-----------------------------------------------------------------------------
void I2c_Init(void)                      /* -- adapt the init for your uC -- */
{
	RCC->APB2ENR|=1<<3;			//��ʹ������IO PORTBʱ�� 	
  SDA_OPEN();
	SCL_OPEN();
	GPIOB->CRL&=0X00FFFFFF;	//PB6/7 �������
	GPIOB->CRL|=0X55000000;	   
}

//-----------------------------------------------------------------------------
void I2c_StartCondition(void)//��ʼ�ź�
{
  SDA_OPEN();     //sda�����
	delay_us(10);
	SCL_OPEN();
	delay_us(10);
 	SDA_LOW();//START:when CLK is high,DATA change form high to low 
	delay_us(100);
	SCL_LOW();//ǯסI2C���ߣ�׼�����ͻ�������� 
	delay_us(100);
}

//-----------------------------------------------------------------------------
void I2c_StopCondition(void)
{
  SCL_LOW();
	delay_us(10);
	SDA_LOW();
	delay_us(10);
	SCL_OPEN();
	delay_us(100);//STOP:when CLK is high DATA change form low to high
	SDA_OPEN();
	delay_us(100);
}

//-----------------------------------------------------------------------------
etError I2c_WriteByte(u8t txByte)
{
  etError error = NO_ERROR;
  u8t     mask;
  for(mask = 0x80; mask > 0; mask >>= 1)		// shift bit for masking (8 times)
  {
    if((mask & txByte) == 0) {SDA_LOW(); } 	//���������ͣ�����0--printf("%ld\r\n",SDA_LOW());
    else                     {SDA_OPEN();}	//���������ߣ�����1--printf("%ld\r\n",SDA_OPEN());
		delay_us(10);
		SCL_OPEN();	  //ʱ�������ߣ����ߴӻ����������ݷ������
    delay_us(50);  //��ƽ����ʱ�䣬�ȴ��ӻ���������
    SCL_LOW();	  //ʱ�������ͣ����ߴӻ���������Ҫ��������
    delay_us(10);  //��ƽ����ʱ�䣬�ȴ�ʱ�����ȶ�
  }
  SDA_OPEN(); 		//����������
  SCL_OPEN();			//ʱ�������ߣ����ߴӻ����������ڿ�ʼ��ȡ����
  delay_us(100);                 					// data set-up time (t_SU;DAT)
  if(GPIOB->IDR&(1<<7)) error = ACK_ERROR;       // ���ӻ��Ƿ��ӦӦ��SDA_READ(GPIOB_7)Ϊ1��Ӧ��ʧ��
	SCL_LOW();
  delay_us(200);                					// wait to see byte package on scope
  return error;                         // return error code
}

//-----------------------------------------------------------------------------
etError I2c_ReadByte(u8t *rxByte, etI2cAck ack, u8t timeout)
{
		etError error = NO_ERROR;
		u8t mask;
		*rxByte = 0x00;
		for(mask = 0x80; mask > 0; mask >>= 1) // shift bit for masking (8 times)
		{ 
			SCL_LOW();			 //ʱ�������ͣ����ߴӻ���������Ҫ����
			delay_us(20);    //��ƽ����ʱ�䣬�ȴ�ʱ�����ȶ� 
			SCL_OPEN();      //ʱ�������ߣ����ߴӻ��������������ڶ�ȡ����  
			delay_us(30);                		// SCL high time (t_HIGH)
			if(GPIOB->IDR&(1<<7)) *rxByte |= mask;   // read bit
			delay_us(20);                		// clock set-up time (t_SU;CLK)       
		}
		SCL_LOW();
		if(ack == ACK) {SDA_LOW();}              // send acknowledge if necessary printf("RxByte=0x%x  ACK\r\n",*rxByte);
		else           {SDA_OPEN();}						 //printf("RxByte=0x%x  NACK\r\n",*rxByte);
		delay_us(10);                  // data set-up time (t_SU;DAT)
		SCL_OPEN();                    // clk #9 for ack
		delay_us(50);                  // SCL high time (t_HIGH)
		SCL_LOW();
		SDA_OPEN();                    // release SDA-line
		delay_us(200);                 // wait to see byte package on scope
		return error;                  // return with no error
}

//-----------------------------------------------------------------------------
etError I2c_GeneralCallReset(void)
{
  etError error;
  
  I2c_StartCondition();
                        error = I2c_WriteByte(0x00);
  if(error == NO_ERROR) error = I2c_WriteByte(0x06);
  
  return error;
}
