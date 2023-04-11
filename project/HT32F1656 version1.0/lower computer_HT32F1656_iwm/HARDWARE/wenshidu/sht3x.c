#include "sht3x.h"
#include "HT32.h"
#include "myiic.h"
#include "delay.h"

/*------------------------------------------------
��������:-SHT3X���Ĵ��������ʪ�Ⱥ���---
-------------------------------------------------*/
uint8_t SHT3X_ReadMeasurementBuffer(ft* temperature, ft* humidity)
{       
  u16t     rawValueTemp=0; // ���������¶�ԭʼֵ
  u16t     rawValueHumi=0; // ��������ʪ��ԭʼֵ
	uint8_t  err=0;
  err=SHT3X_StartWriteAccess(); //��ʼд����--д����ʵĵ�ַ
	if(err==0) err=SHT3X_WriteCommand(CMD_FETCH_DATA); // ���û�д���д��[���ڶ�ֵ]������
	SHT3X_StopAccess();
  if(err==0) err=SHT3X_StartReadAccess();  //��ʼ������--д����ʵĵ�ַ
	if(err==0)
  {
		SHT3X_Read2BytesAndCrc(&rawValueTemp,ACK); //��ȡԭʼ�¶�ֵ
		SHT3X_Read2BytesAndCrc(&rawValueHumi,NACK); //��ȡԭʼʪ��ֵ
  // ���û�д��󣬼���%RH�ڵ��¶Ⱥ�ʪ��
  *temperature = SHT3X_CalcTemperature(rawValueTemp);
  *humidity    = SHT3X_CalcHumidity(rawValueHumi);
	}
  SHT3X_StopAccess();
	return err;

}

/*------------------------------------------------
��������:��ʼд�ź�
-------------------------------------------------*/
uint8_t SHT3X_StartWriteAccess(void)	
{
    uint8_t err=0;
		/* ��1��������I2C���������ź� */
    i2c_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    err=i2c_SHT30_SendByte(sht30_WR_address << 1);	/* �˴���дָ�� */
		return err;

}

/*------------------------------------------------
��������:д����
-------------------------------------------------*/
uint8_t SHT3X_WriteCommand(etCommands command)
{
  uint8_t err=0,err1=0,err2=0;
	err1=i2c_SHT30_SendByte(command >> 8);//������ĸ�8λд�봫����
  err2=i2c_SHT30_SendByte(command & 0xFF);//������ĵ�8λд�봫����
	err=err1+err2;
	return err;
 
}

/*------------------------------------------------
��������:ֹͣ�ź�
-------------------------------------------------*/
void SHT3X_StopAccess(void)
{
   i2c_Stop();
}
/*------------------------------------------------
��������:��ʼ���ź�
-------------------------------------------------*/

uint8_t SHT3X_StartReadAccess(void)
{
   uint8_t err=0;
   i2c_Start();// I2C��ʼ�ź�
	 err=i2c_SHT30_SendByte(sht30_WR_address << 1 | 0x01);// ʹ�ö���Ǳ�д������I2C��ַ--1�Ƕ���0��д
	 return err;

}

/*------------------------------------------------
��������:��ȡ�����ֽ�����
-------------------------------------------------*/
void SHT3X_Read2BytesAndCrc(u16t* data,etI2cAck finaleAckNack)
{
  u8t     bytes[2]={0,0}; // ��ȡ���ݵ�����
  u8t     checksum=0; // У����ֽ�
  // ��ȡ���������ֽں�һ��У����ֽ�
	bytes[0]=i2c_ReadByte(ACK);		
	bytes[1]=i2c_ReadByte(ACK);		
	checksum=i2c_ReadByte(finaleAckNack);
	
	//У�����-->�ѽ��(��ʱ������CRCУ��)
	*data = (bytes[0] << 8) | bytes[1];
}


/*------------------------------------------------
��������:ת��ԭʼ����ʪ�����ݣ���ȡʵ�ʵ���ʪ��ֵ
-------------------------------------------------*/
// ����ʵ���¶� T = -45 + 175 * rawValue / (2^16-1)

ft SHT3X_CalcTemperature(u16t rawValue)
{
  return 175.0f * (ft)rawValue / 65535.0f - 45.0f;
}
// ����ʵ��ʪ�� RH = rawValue / (2^16-1) * 100
ft SHT3X_CalcHumidity(u16t rawValue)
{
  return 100.0f * (ft)rawValue / 65535.0f;
}

//SHT30�����ģ��������ú���
//����0��ʾ��ʼ�����óɹ�������1��ʾʧ��
uint8_t SHT3X_StartPeriodicMeasurment(etRepeatability repeatability, etFrequency frequency)
{
	 uint8_t err=0;
   while(SHT3X_StartWriteAccess());//��ʼд����
  // if no error, start periodic measurement 
    // use depending on the required repeatability and frequency,the corresponding command
    switch(repeatability)
    {
      case REPEATAB_LOW: // low repeatability
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // low repeatability,  0.5 Hz
									SHT3X_WriteCommand(CMD_MEAS_PERI_05_L);
            break;          
          case FREQUENCY_1HZ:  // low repeatability,  1.0 Hz
                  SHT3X_WriteCommand(CMD_MEAS_PERI_1_L);
            break;          
          case FREQUENCY_2HZ:  // low repeatability,  2.0 Hz
                  SHT3X_WriteCommand(CMD_MEAS_PERI_2_L);
            break;          
          case FREQUENCY_4HZ:  // low repeatability,  4.0 Hz
                  SHT3X_WriteCommand(CMD_MEAS_PERI_4_L);
            break;          
          case FREQUENCY_10HZ: // low repeatability, 10.0 Hz
                  SHT3X_WriteCommand(CMD_MEAS_PERI_10_L);
            break;          
          default:
            break;
        }
        break;
        
      case REPEATAB_MEDIUM: // medium repeatability
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // medium repeatability,  0.5 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_05_M);
						break;
          case FREQUENCY_1HZ:  // medium repeatability,  1.0 Hz
             err=SHT3X_WriteCommand(CMD_MEAS_PERI_1_M);
						break;        
          case FREQUENCY_2HZ:  // medium repeatability,  2.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_2_M);
						break;        
          case FREQUENCY_4HZ:  // medium repeatability,  4.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_4_M);
						break;      
          case FREQUENCY_10HZ: // medium repeatability, 10.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_10_M);
						break;
          default:
						break;
        }
        break;
        
      case REPEATAB_HIGH: // ���ظ�
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // high repeatability,  0.5 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_05_H);
            break;
          case FREQUENCY_1HZ:  // ���ظ���,  1.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_1_H);
            break;
          case FREQUENCY_2HZ:  // high repeatability,  2.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_2_H);
            break;
          case FREQUENCY_4HZ:  // high repeatability,  4.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_4_H);
            break;
          case FREQUENCY_10HZ: // high repeatability, 10.0 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_10_H);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  SHT3X_StopAccess();//I2C��ֹͣ�ź�--ֹͣ��д����
	return err;
}

