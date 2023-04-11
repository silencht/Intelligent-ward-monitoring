#include "sht3x.h"
#include "i2c_hal.h"

//-- Defines ------------------------------------------------------------------
// 发电机多项式对CRC
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//-----------------------------------------------------------------------------
// 重置GPIOB-12
#define RESET_LOW()  (GPIOB->BSRR = 0x10000000) // 拉低PB12
#define RESET_HIGH() (GPIOB->BSRR = 0x00001000) // 拉高PB12

// 警报位 GPIOB-10
#define ALERT_READ   (GPIOB->IDR  & 0x0400)     // 读警报
//=============================================================================

//-- 全局变量 ---------------------------------------------------------
static u8t _i2cAddress; // I2C Address

//-- 静态函数原型 -----------------------------------------------
static etError SHT3X_WriteAlertLimitData(ft humidity, ft temperature);
static etError SHT3X_ReadAlertLimitData(ft* humidity, ft* temperature);
static etError SHT3X_StartWriteAccess(void);
static etError SHT3X_StartReadAccess(void);
static void SHT3X_StopAccess(void);
static etError SHT3X_WriteCommand(etCommands command);
static etError SHT3X_Read2BytesAndCrc(u16t* data, etI2cAck finaleAckNack,
                                      u8t timeout);
static etError SHT3X_Write2BytesAndCrc(u16t data);
static u8t SHT3X_CalcCrc(u8t data[], u8t nbrOfBytes);
static etError SHT3X_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum);
static ft SHT3X_CalcTemperature(u16t rawValue);
static ft SHT3X_CalcHumidity(u16t rawValue);
static u16t SHT3X_CalcRawTemperature(ft temperature);
static u16t SHT3X_CalcRawHumidity(ft humidity);

//-----------------------------------------------------------------------------
void SHT3X_Init(u8t i2cAddress)          /* -- 温湿度初始化 -- */
{
  RCC->APB2ENR |= 0x00000008;  // 开GPIOB口时钟 
  I2c_Init(); 	// 初始化I2C
  SHT3X_SetI2cAdr(i2cAddress);
}

//-----------------------------------------------------------------------------
void SHT3X_SetI2cAdr(u8t i2cAddress)
{
  _i2cAddress = i2cAddress;		//_i2cAddress=0x45--(0x45<<1)=0x8A
}

etError SHT3x_ReadSerialNumber(u32t* serialNumber)
{
  etError error; 
  u16t serialNumWords[2];
  
  error = SHT3X_StartWriteAccess();//开启I2C
  error |= SHT3X_WriteCommand(CMD_READ_SERIALNBR);// 写“读序列号”命令
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();// 如果没有错误，开始读访问
  // 如果没有错误，请阅读第一个序列号字
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&serialNumWords[0], ACK, 100);
  // 如果没有错误，请阅读第二个序列号字
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&serialNumWords[1], NACK, 0);
  SHT3X_StopAccess();
  // 如果没有错误，calc序列号为32位整数
  if(error == NO_ERROR)
  {
    *serialNumber = (serialNumWords[0] << 16) | serialNumWords[1];
  }
  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ReadStatus(u16t* status) //STH3X读状态
{
  etError error;
	
  error = SHT3X_StartWriteAccess();
  // 如果没有错误，写入“读取状态”命令
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_READ_STATUS);
  // 如果没有错误，开始读访问
  if(error == NO_ERROR) error = SHT3X_StartReadAccess(); 
  // 如果没有错误，读取状态
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(status, NACK, 0);
  SHT3X_StopAccess();//停止访问
  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ClearAllAlertFlags(void)
{
  etError error;
  error = SHT3X_StartWriteAccess(); 
  // 如果没有错误，写入清除状态寄存器命令
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_CLEAR_STATUS); 
  SHT3X_StopAccess();//停止访问 
  return error;
}

/*----------------------------------------------------
函数功能:获取温湿度数据
函数参数:
				ft* temperature 用于存储温度的变量
				ft* humidity		用于存储湿度的变量
				etRepeatability repeatability  测量的方式
				etMode mode		测量的模式
----------------------------------------------------*/
etError SHT3X_GetTempAndHumi(ft* temperature, ft* humidity,
                             etRepeatability repeatability, etMode mode,
                             u8t timeout)
{
  etError error;
                               
  switch(mode)
  {    
    case MODE_CLKSTRETCH: // 通过时钟伸展模式获得温度
      error = SHT3X_GetTempAndHumiClkStretch(temperature, humidity,
                                             repeatability, timeout);
      break;
    case MODE_POLLING:    // 使用轮询模式获取温度
      error = SHT3X_GetTempAndHumiPolling(temperature, humidity,
                                          repeatability, timeout);
      break;
    default:              
      error = PARM_ERROR;
      break;
  }

  return error;
}
/*----------------------------------------------------
函数名称:时钟伸展模式获取数据
函数功能:通过时钟伸展模式获取温湿度数据
函数参数:
				ft* temperature 用于存储温度的变量
				ft* humidity		用于存储湿度的变量
				etRepeatability repeatability  测量的方式
				u8t timeout			超时时间
----------------------------------------------------*/
etError SHT3X_GetTempAndHumiClkStretch(ft* temperature, ft* humidity,
                                       etRepeatability repeatability,
                                       u8t timeout)
{
  etError error;       
  u16t    rawValueTemp; // 传感器的温度原始值
  u16t    rawValueHumi; // 传感器的湿度原始值
  
  error = SHT3X_StartWriteAccess();//开始写入访问地址0x45--低位0代表写操作
  // 如果应答无错
	error = NO_ERROR;
  if(error == NO_ERROR)
  {
    // 在时钟伸缩模式下开始测量，根据所需的可重复性，相应的命令
    switch(repeatability)
    {
      case REPEATAB_LOW:
        error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_L);
        break;
      case REPEATAB_MEDIUM:
        error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_M);
        break;
      case REPEATAB_HIGH:
        error = SHT3X_WriteCommand(CMD_MEAS_CLOCKSTR_H);//0x2C06
        break;
      default:
        error = PARM_ERROR;
        break;
    }
  }
  // 若无错误，开始读访问
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();
  // 若无错误，读温度的初始值
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, timeout);
  // 若无错误，读湿度的初始值
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);
  
  SHT3X_StopAccess();//停止访问
  
  // 若无错误， 计算%RH内的温度和湿度
	error = NO_ERROR;
  if(error == NO_ERROR)
  {
    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(rawValueHumi);
  }
  return error;
}

//函数名称:轮询模式获得温度--------------------------------------------
etError SHT3X_GetTempAndHumiPolling(ft* temperature, ft* humidity,
                                    etRepeatability repeatability,
                                    u8t timeout)
{
  etError error;           
  u16t    rawValueTemp;    // 传感器的温度原始值
  u16t    rawValueHumi;    // 传感器的湿度原始值
  
  error  = SHT3X_StartWriteAccess();//开始写入访问地址
   // 如果应答无错
  if(error == NO_ERROR)
  {
    // 在轮询模式下开始测量，根据所需的可重复性，相应的命令
    switch(repeatability)
    {
      case REPEATAB_LOW:
        error = SHT3X_WriteCommand(CMD_MEAS_POLLING_L);
        break;
      case REPEATAB_MEDIUM:
        error = SHT3X_WriteCommand(CMD_MEAS_POLLING_M);
        break;
      case REPEATAB_HIGH:
        error = SHT3X_WriteCommand(CMD_MEAS_POLLING_H);
        break;
      default:
        error = PARM_ERROR;
        break;
    }
  }
	SHT3X_StopAccess();
  delay_us(50000);
	
  // 如无错误，等到测量准备开始
  if(error == NO_ERROR)
  {	
      // 若无错误，开始读访问
      error = SHT3X_StartReadAccess();
      // 延时1ms
      delay_us(1000);
  }
  // 若无错误，读温度和湿度的初始值
  if(error == NO_ERROR)				//通过检测，之前无错误
  {
    error |= SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, 0);
    error |= SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);
  }
  SHT3X_StopAccess();//停止访问
  // 若无错误， 计算%RH内的温度和湿度
  if(error == NO_ERROR)
  {
    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(rawValueHumi);
  }
  return error;
}

//函数名称:定期测量-------------------------------------------------
etError SHT3X_StartPeriodicMeasurment(etRepeatability repeatability,
                                      etFrequency frequency)
{
  etError error;        // error code
  
  error = SHT3X_StartWriteAccess();//开始写访问
  
  // if no error, start periodic measurement 
  if(error == NO_ERROR)
  {
    // use depending on the required repeatability and frequency,the corresponding command
    switch(repeatability)
    {
      case REPEATAB_LOW: // low repeatability
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // low repeatability,  0.5 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_05_L);
            break;          
          case FREQUENCY_1HZ:  // low repeatability,  1.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_1_L);
            break;          
          case FREQUENCY_2HZ:  // low repeatability,  2.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_2_L);
            break;          
          case FREQUENCY_4HZ:  // low repeatability,  4.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_4_L);
            break;          
          case FREQUENCY_10HZ: // low repeatability, 10.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_10_L);
            break;          
          default:
            error |= PARM_ERROR;
            break;
        }
        break;
        
      case REPEATAB_MEDIUM: // medium repeatability
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // medium repeatability,  0.5 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_05_M);
						break;
          case FREQUENCY_1HZ:  // medium repeatability,  1.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_1_M);
						break;        
          case FREQUENCY_2HZ:  // medium repeatability,  2.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_2_M);
						break;        
          case FREQUENCY_4HZ:  // medium repeatability,  4.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_4_M);
						break;      
          case FREQUENCY_10HZ: // medium repeatability, 10.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_10_M);
						break;
          default:
            error |= PARM_ERROR;
						break;
        }
        break;
        
      case REPEATAB_HIGH: // 高重复
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // high repeatability,  0.5 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_05_H);
            break;
          case FREQUENCY_1HZ:  // 高重复性,  1.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_1_H);
            break;
          case FREQUENCY_2HZ:  // high repeatability,  2.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_2_H);
            break;
          case FREQUENCY_4HZ:  // high repeatability,  4.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_4_H);
            break;
          case FREQUENCY_10HZ: // high repeatability, 10.0 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_10_H);
            break;
          default:
            error |= PARM_ERROR;
            break;
        }
        break;
      default:
        error |= PARM_ERROR;
        break;
    }
  }

  SHT3X_StopAccess();//I2C的停止信号--停止读写操作

  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ReadMeasurementBuffer(ft* temperature, ft* humidity)
{
  etError  error;        
  u16t     rawValueTemp; // 传感器的温度原始值
  u16t     rawValueHumi; // 传感器的湿度原始值

  error = SHT3X_StartWriteAccess();//开始写访问--写入访问的地址
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_FETCH_DATA); // 如果没有错误，写入[周期读值]的命令
	SHT3X_StopAccess();
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();  //开始读访问--写入访问的地址
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, 0); //以应答模式获取原始温度值
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);//以非应答模式获取原始湿度值
  // 如果没有错误，计算%RH内的温度和湿度
  if(error == NO_ERROR)
  {
    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(rawValueHumi);
  }

  SHT3X_StopAccess();

  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_EnableHeater(void)
{
  etError error; // error code

  error = SHT3X_StartWriteAccess();

  // if no error, write heater enable command
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_HEATER_ENABLE);

  SHT3X_StopAccess();

  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_DisableHeater(void)
{
  etError error; // error code

  error = SHT3X_StartWriteAccess();

  // if no error, write heater disable command
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_HEATER_DISABLE);

  SHT3X_StopAccess();

  return error;
}


//-----------------------------------------------------------------------------
etError SHT3X_SetAlertLimits(ft humidityHighSet,   ft temperatureHighSet,
                             ft humidityHighClear, ft temperatureHighClear,
                             ft humidityLowClear,  ft temperatureLowClear,
                             ft humidityLowSet,    ft temperatureLowSet)
{
  etError  error;  // error code
  
  // write humidity & temperature alter limits, high set
  error = SHT3X_StartWriteAccess();
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_W_AL_LIM_HS);
  if(error == NO_ERROR) error = SHT3X_WriteAlertLimitData(humidityHighSet,
                                                          temperatureHighSet);
  SHT3X_StopAccess();
  if(error == NO_ERROR)
  {
    // write humidity & temperature alter limits, high clear
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_W_AL_LIM_HC);
    if(error == NO_ERROR) error = SHT3X_WriteAlertLimitData(humidityHighClear,
                                                            temperatureHighClear);
    SHT3X_StopAccess();
  }
  if(error == NO_ERROR)
  {
    // write humidity & temperature alter limits, low clear
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_W_AL_LIM_LC);
    if(error == NO_ERROR) error = SHT3X_WriteAlertLimitData(humidityLowClear,
                                                            temperatureLowClear);
    SHT3X_StopAccess();
  }
  if(error == NO_ERROR)
  {
    // write humidity & temperature alter limits, low set
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_W_AL_LIM_LS);
    if(error == NO_ERROR) error = SHT3X_WriteAlertLimitData(humidityLowSet,
                                                            temperatureLowSet);
    SHT3X_StopAccess();
  }
  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_GetAlertLimits(ft* humidityHighSet,   ft* temperatureHighSet,
                             ft* humidityHighClear, ft* temperatureHighClear,
                             ft* humidityLowClear,  ft* temperatureLowClear,
                             ft* humidityLowSet,    ft* temperatureLowSet)
{
  etError  error;  // error code
  // read humidity & temperature alter limits, high set
  error = SHT3X_StartWriteAccess();
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_R_AL_LIM_HS);
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();
  if(error == NO_ERROR) error = SHT3X_ReadAlertLimitData(humidityHighSet,
                                                         temperatureHighSet);
  SHT3X_StopAccess();

  if(error == NO_ERROR)
  {
    // read humidity & temperature alter limits, high clear
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_R_AL_LIM_HC);
    if(error == NO_ERROR) error = SHT3X_StartReadAccess();
    if(error == NO_ERROR) error = SHT3X_ReadAlertLimitData(humidityHighClear,
                                                           temperatureHighClear);
    SHT3X_StopAccess();
  }

  if(error == NO_ERROR)
  {
    // read humidity & temperature alter limits, low clear
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_R_AL_LIM_LC);
    if(error == NO_ERROR) error = SHT3X_StartReadAccess();
    if(error == NO_ERROR) error = SHT3X_ReadAlertLimitData(humidityLowClear,
                                                           temperatureLowClear);
    SHT3X_StopAccess();
  }

  if(error == NO_ERROR)
  {
    // read humidity & temperature alter limits, low set
    error = SHT3X_StartWriteAccess();
    if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_R_AL_LIM_LS);
    if(error == NO_ERROR) error = SHT3X_StartReadAccess();
    if(error == NO_ERROR) error = SHT3X_ReadAlertLimitData(humidityLowSet,
                                                           temperatureLowSet);
    SHT3X_StopAccess();
  }
  return error;
}
                             
//-----------------------------------------------------------------------------
bt SHT3X_ReadAlert(void)
{
  // read alert pin
  return (ALERT_READ != 0) ? TRUE : FALSE;
}

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

                             
//-----------------------------------------------------------------------------
static etError SHT3X_WriteAlertLimitData(ft humidity, ft temperature)
{
  etError  error;           // error code
  
  i16t rawHumidity;
  i16t rawTemperature;
  
  if((humidity < 0.0f) || (humidity > 100.0f) 
  || (temperature < -45.0f) || (temperature > 130.0f))
  {
    error = PARM_ERROR;
  }
  else
  {
    rawHumidity    = SHT3X_CalcRawHumidity(humidity);
    rawTemperature = SHT3X_CalcRawTemperature(temperature);

    error = SHT3X_Write2BytesAndCrc((rawHumidity & 0xFE00) | ((rawTemperature >> 7) & 0x001FF));
  }
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_ReadAlertLimitData(ft* humidity, ft* temperature)
{
  etError  error; 
  u16t     data;

  error = SHT3X_Read2BytesAndCrc(&data, NACK, 0);
  if(error == NO_ERROR)
  {
    *humidity = SHT3X_CalcHumidity(data & 0xFE00);
    *temperature = SHT3X_CalcTemperature(data << 7);
  }
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_StartWriteAccess(void)//开始写访问
{
  etError error; 	
  // I2C起始信号
  I2c_StartCondition();
  // 用写标记写传感器I2C地址
  error = I2c_WriteByte(_i2cAddress << 1);//0位写
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_StartReadAccess(void)
{
  etError error;
  // I2C起始信号
  I2c_StartCondition();
  // 使用读标记编写传感器I2C地址--1是读，0是写
  error = I2c_WriteByte(_i2cAddress << 1 | 0x01);//0x8B
	
  return error;
}

//-----------------------------------------------------------------------------
static void SHT3X_StopAccess(void)
{
  // I2C停止信号
  I2c_StopCondition();
}

//-----------------------------------------------------------------------------
static etError SHT3X_WriteCommand(etCommands command)
{
  etError error; 
  error  = I2c_WriteByte(command >> 8);//将命令的高8位写入传感器
  error |= I2c_WriteByte(command & 0xFF);//将命令的低8位写入传感器
  return error;//返回写入命令后的应答信号，检测是否写入成功
}

//-----------------------------------------------------------------------------
static etError SHT3X_Read2BytesAndCrc(u16t* data, etI2cAck finaleAckNack,
                                      u8t timeout)
{
  etError error;    
  u8t     bytes[2]; // 读取数据的数组
  u8t     checksum; // 校验和字节
  // 读取两个数据字节和一个校验和字节
                        error = I2c_ReadByte(&bytes[0], ACK, (u8t)255);
  if(error == NO_ERROR) error = I2c_ReadByte(&bytes[1], ACK, (u8t)255);
  if(error == NO_ERROR) error = I2c_ReadByte(&checksum, finaleAckNack, (u8t)255);
	//校验错误-->已解决
  if(error == NO_ERROR) error = SHT3X_CheckCrc(bytes, 2, checksum);// 验证校验和
	if(error == NO_ERROR)
	{
		*data = (bytes[0] << 8) | bytes[1];// 将两个字节合并为一个16位值		
	}
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_Write2BytesAndCrc(u16t data)
{
  etError error;    
  u8t     bytes[2]; // 读取数据的数组
  u8t     checksum; // 校验和字节
  
  bytes[0] = data >> 8;
  bytes[1] = data & 0xFF;
  checksum = SHT3X_CalcCrc(bytes, 2);//获取校验和
 
  // 写入两个数据字节和一个校验和字节
                        error = I2c_WriteByte(bytes[0]); // 写高位
  if(error == NO_ERROR) error = I2c_WriteByte(bytes[1]); // 写低位
  if(error == NO_ERROR) error = I2c_WriteByte(checksum); // 写校验和
  
  return error;
}

static u8t SHT3X_CalcCrc(u8t data[], u8t nbrOfBytes)
{
  u8t bit;        // 位码
  u8t crc = 0xFF; // 计算校验和
  u8t byteCtr;    // 字节计数器
  
  // 用给定的多项式计算8位校验和
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)// byteCtr=0->1
  {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit)
    {
      if(crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else           crc = (crc << 1);
    }
  }
  return crc;
}

//-----------------------------------------------------------------------------
static etError SHT3X_CheckCrc(u8t data[], u8t nbrOfBytes, u8t checksum)
{
  u8t crc;     // 计算校验和
  // 计算8位的校验和
  crc = SHT3X_CalcCrc(data, nbrOfBytes);
  // 验证校验和
  if(crc != checksum) return CHECKSUM_ERROR;
  else                return NO_ERROR;
}
/*------------------------------------------------
函数功能:转换原始的温湿度数据，获取实际的温湿度值
-------------------------------------------------*/

// 计算实际温度 T = -45 + 175 * rawValue / (2^16-1)
static ft SHT3X_CalcTemperature(u16t rawValue)
{
  return 175.0f * (ft)rawValue / 65535.0f - 45.0f;
}

// 计算实际湿度 RH = rawValue / (2^16-1) * 100
static ft SHT3X_CalcHumidity(u16t rawValue)
{
  return 100.0f * (ft)rawValue / 65535.0f;
}

/*----------------------------------------------
函数名称:温度转换(原始-->实际)
函数功能:将获取的原始温度数据转换为实际的温度数据
函数参数:
				ft temperature 传入的原始温度值
公		式:rawT = (temperature + 45) / 175 * (2^16-1)
------------------------------------------------*/
static u16t SHT3X_CalcRawTemperature(ft temperature)
{
  return (temperature + 45.0f) / 175.0f * 65535.0f;
}

/*----------------------------------------------
函数名称:湿度转换(原始-->实际)
函数功能:将获取的原始湿度数据转换为实际的温度数据
函数参数:
				ft humidity 传入的原始湿度值
公		式:rawRH = humidity / 100 * (2^16-1)
------------------------------------------------*/
static u16t SHT3X_CalcRawHumidity(ft humidity)
{
  return humidity / 100.0f * 65535.0f;
}
