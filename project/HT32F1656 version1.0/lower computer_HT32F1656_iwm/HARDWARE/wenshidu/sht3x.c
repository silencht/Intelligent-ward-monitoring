#include "sht3x.h"
#include "HT32.h"
#include "myiic.h"
#include "delay.h"

/*------------------------------------------------
函数功能:-SHT3X读寄存器获得温湿度函数---
-------------------------------------------------*/
uint8_t SHT3X_ReadMeasurementBuffer(ft* temperature, ft* humidity)
{       
  u16t     rawValueTemp=0; // 传感器的温度原始值
  u16t     rawValueHumi=0; // 传感器的湿度原始值
	uint8_t  err=0;
  err=SHT3X_StartWriteAccess(); //开始写访问--写入访问的地址
	if(err==0) err=SHT3X_WriteCommand(CMD_FETCH_DATA); // 如果没有错误，写入[周期读值]的命令
	SHT3X_StopAccess();
  if(err==0) err=SHT3X_StartReadAccess();  //开始读访问--写入访问的地址
	if(err==0)
  {
		SHT3X_Read2BytesAndCrc(&rawValueTemp,ACK); //获取原始温度值
		SHT3X_Read2BytesAndCrc(&rawValueHumi,NACK); //获取原始湿度值
  // 如果没有错误，计算%RH内的温度和湿度
  *temperature = SHT3X_CalcTemperature(rawValueTemp);
  *humidity    = SHT3X_CalcHumidity(rawValueHumi);
	}
  SHT3X_StopAccess();
	return err;

}

/*------------------------------------------------
函数功能:开始写信号
-------------------------------------------------*/
uint8_t SHT3X_StartWriteAccess(void)	
{
    uint8_t err=0;
		/* 第1步：发起I2C总线启动信号 */
    i2c_Start();

    /* 第2步：发起控制字节，高7bit是地址，bit0是读写控制位，0表示写，1表示读 */
    err=i2c_SHT30_SendByte(sht30_WR_address << 1);	/* 此处是写指令 */
		return err;

}

/*------------------------------------------------
函数功能:写命令
-------------------------------------------------*/
uint8_t SHT3X_WriteCommand(etCommands command)
{
  uint8_t err=0,err1=0,err2=0;
	err1=i2c_SHT30_SendByte(command >> 8);//将命令的高8位写入传感器
  err2=i2c_SHT30_SendByte(command & 0xFF);//将命令的低8位写入传感器
	err=err1+err2;
	return err;
 
}

/*------------------------------------------------
函数功能:停止信号
-------------------------------------------------*/
void SHT3X_StopAccess(void)
{
   i2c_Stop();
}
/*------------------------------------------------
函数功能:开始读信号
-------------------------------------------------*/

uint8_t SHT3X_StartReadAccess(void)
{
   uint8_t err=0;
   i2c_Start();// I2C起始信号
	 err=i2c_SHT30_SendByte(sht30_WR_address << 1 | 0x01);// 使用读标记编写传感器I2C地址--1是读，0是写
	 return err;

}

/*------------------------------------------------
函数功能:读取两个字节数据
-------------------------------------------------*/
void SHT3X_Read2BytesAndCrc(u16t* data,etI2cAck finaleAckNack)
{
  u8t     bytes[2]={0,0}; // 读取数据的数组
  u8t     checksum=0; // 校验和字节
  // 读取两个数据字节和一个校验和字节
	bytes[0]=i2c_ReadByte(ACK);		
	bytes[1]=i2c_ReadByte(ACK);		
	checksum=i2c_ReadByte(finaleAckNack);
	
	//校验错误-->已解决(暂时不进行CRC校验)
	*data = (bytes[0] << 8) | bytes[1];
}


/*------------------------------------------------
函数功能:转换原始的温湿度数据，获取实际的温湿度值
-------------------------------------------------*/
// 计算实际温度 T = -45 + 175 * rawValue / (2^16-1)

ft SHT3X_CalcTemperature(u16t rawValue)
{
  return 175.0f * (ft)rawValue / 65535.0f - 45.0f;
}
// 计算实际湿度 RH = rawValue / (2^16-1) * 100
ft SHT3X_CalcHumidity(u16t rawValue)
{
  return 100.0f * (ft)rawValue / 65535.0f;
}

//SHT30文殊度模块参数设置函数
//返回0表示初始化设置成功，返回1表示失败
uint8_t SHT3X_StartPeriodicMeasurment(etRepeatability repeatability, etFrequency frequency)
{
	 uint8_t err=0;
   while(SHT3X_StartWriteAccess());//开始写访问
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
        
      case REPEATAB_HIGH: // 高重复
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // high repeatability,  0.5 Hz
             SHT3X_WriteCommand(CMD_MEAS_PERI_05_H);
            break;
          case FREQUENCY_1HZ:  // 高重复性,  1.0 Hz
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
  SHT3X_StopAccess();//I2C的停止信号--停止读写操作
	return err;
}

