#include "sht3x.h"
#include "i2c_hal.h"

//-- Defines ------------------------------------------------------------------
// ���������ʽ��CRC
#define POLYNOMIAL  0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

//-----------------------------------------------------------------------------
// ����GPIOB-12
#define RESET_LOW()  (GPIOB->BSRR = 0x10000000) // ����PB12
#define RESET_HIGH() (GPIOB->BSRR = 0x00001000) // ����PB12

// ����λ GPIOB-10
#define ALERT_READ   (GPIOB->IDR  & 0x0400)     // ������
//=============================================================================

//-- ȫ�ֱ��� ---------------------------------------------------------
static u8t _i2cAddress; // I2C Address

//-- ��̬����ԭ�� -----------------------------------------------
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
void SHT3X_Init(u8t i2cAddress)          /* -- ��ʪ�ȳ�ʼ�� -- */
{
  RCC->APB2ENR |= 0x00000008;  // ��GPIOB��ʱ�� 
  I2c_Init(); 	// ��ʼ��I2C
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
  
  error = SHT3X_StartWriteAccess();//����I2C
  error |= SHT3X_WriteCommand(CMD_READ_SERIALNBR);// д�������кš�����
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();// ���û�д��󣬿�ʼ������
  // ���û�д������Ķ���һ�����к���
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&serialNumWords[0], ACK, 100);
  // ���û�д������Ķ��ڶ������к���
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&serialNumWords[1], NACK, 0);
  SHT3X_StopAccess();
  // ���û�д���calc���к�Ϊ32λ����
  if(error == NO_ERROR)
  {
    *serialNumber = (serialNumWords[0] << 16) | serialNumWords[1];
  }
  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ReadStatus(u16t* status) //STH3X��״̬
{
  etError error;
	
  error = SHT3X_StartWriteAccess();
  // ���û�д���д�롰��ȡ״̬������
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_READ_STATUS);
  // ���û�д��󣬿�ʼ������
  if(error == NO_ERROR) error = SHT3X_StartReadAccess(); 
  // ���û�д��󣬶�ȡ״̬
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(status, NACK, 0);
  SHT3X_StopAccess();//ֹͣ����
  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ClearAllAlertFlags(void)
{
  etError error;
  error = SHT3X_StartWriteAccess(); 
  // ���û�д���д�����״̬�Ĵ�������
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_CLEAR_STATUS); 
  SHT3X_StopAccess();//ֹͣ���� 
  return error;
}

/*----------------------------------------------------
��������:��ȡ��ʪ������
��������:
				ft* temperature ���ڴ洢�¶ȵı���
				ft* humidity		���ڴ洢ʪ�ȵı���
				etRepeatability repeatability  �����ķ�ʽ
				etMode mode		������ģʽ
----------------------------------------------------*/
etError SHT3X_GetTempAndHumi(ft* temperature, ft* humidity,
                             etRepeatability repeatability, etMode mode,
                             u8t timeout)
{
  etError error;
                               
  switch(mode)
  {    
    case MODE_CLKSTRETCH: // ͨ��ʱ����չģʽ����¶�
      error = SHT3X_GetTempAndHumiClkStretch(temperature, humidity,
                                             repeatability, timeout);
      break;
    case MODE_POLLING:    // ʹ����ѯģʽ��ȡ�¶�
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
��������:ʱ����չģʽ��ȡ����
��������:ͨ��ʱ����չģʽ��ȡ��ʪ������
��������:
				ft* temperature ���ڴ洢�¶ȵı���
				ft* humidity		���ڴ洢ʪ�ȵı���
				etRepeatability repeatability  �����ķ�ʽ
				u8t timeout			��ʱʱ��
----------------------------------------------------*/
etError SHT3X_GetTempAndHumiClkStretch(ft* temperature, ft* humidity,
                                       etRepeatability repeatability,
                                       u8t timeout)
{
  etError error;       
  u16t    rawValueTemp; // ���������¶�ԭʼֵ
  u16t    rawValueHumi; // ��������ʪ��ԭʼֵ
  
  error = SHT3X_StartWriteAccess();//��ʼд����ʵ�ַ0x45--��λ0����д����
  // ���Ӧ���޴�
	error = NO_ERROR;
  if(error == NO_ERROR)
  {
    // ��ʱ������ģʽ�¿�ʼ��������������Ŀ��ظ��ԣ���Ӧ������
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
  // ���޴��󣬿�ʼ������
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();
  // ���޴��󣬶��¶ȵĳ�ʼֵ
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, timeout);
  // ���޴��󣬶�ʪ�ȵĳ�ʼֵ
	error = NO_ERROR;
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);
  
  SHT3X_StopAccess();//ֹͣ����
  
  // ���޴��� ����%RH�ڵ��¶Ⱥ�ʪ��
	error = NO_ERROR;
  if(error == NO_ERROR)
  {
    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(rawValueHumi);
  }
  return error;
}

//��������:��ѯģʽ����¶�--------------------------------------------
etError SHT3X_GetTempAndHumiPolling(ft* temperature, ft* humidity,
                                    etRepeatability repeatability,
                                    u8t timeout)
{
  etError error;           
  u16t    rawValueTemp;    // ���������¶�ԭʼֵ
  u16t    rawValueHumi;    // ��������ʪ��ԭʼֵ
  
  error  = SHT3X_StartWriteAccess();//��ʼд����ʵ�ַ
   // ���Ӧ���޴�
  if(error == NO_ERROR)
  {
    // ����ѯģʽ�¿�ʼ��������������Ŀ��ظ��ԣ���Ӧ������
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
	
  // ���޴��󣬵ȵ�����׼����ʼ
  if(error == NO_ERROR)
  {	
      // ���޴��󣬿�ʼ������
      error = SHT3X_StartReadAccess();
      // ��ʱ1ms
      delay_us(1000);
  }
  // ���޴��󣬶��¶Ⱥ�ʪ�ȵĳ�ʼֵ
  if(error == NO_ERROR)				//ͨ����⣬֮ǰ�޴���
  {
    error |= SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, 0);
    error |= SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);
  }
  SHT3X_StopAccess();//ֹͣ����
  // ���޴��� ����%RH�ڵ��¶Ⱥ�ʪ��
  if(error == NO_ERROR)
  {
    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(rawValueHumi);
  }
  return error;
}

//��������:���ڲ���-------------------------------------------------
etError SHT3X_StartPeriodicMeasurment(etRepeatability repeatability,
                                      etFrequency frequency)
{
  etError error;        // error code
  
  error = SHT3X_StartWriteAccess();//��ʼд����
  
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
        
      case REPEATAB_HIGH: // ���ظ�
        switch(frequency)
        {
          case FREQUENCY_HZ5:  // high repeatability,  0.5 Hz
            error |= SHT3X_WriteCommand(CMD_MEAS_PERI_05_H);
            break;
          case FREQUENCY_1HZ:  // ���ظ���,  1.0 Hz
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

  SHT3X_StopAccess();//I2C��ֹͣ�ź�--ֹͣ��д����

  return error;
}

//-----------------------------------------------------------------------------
etError SHT3X_ReadMeasurementBuffer(ft* temperature, ft* humidity)
{
  etError  error;        
  u16t     rawValueTemp; // ���������¶�ԭʼֵ
  u16t     rawValueHumi; // ��������ʪ��ԭʼֵ

  error = SHT3X_StartWriteAccess();//��ʼд����--д����ʵĵ�ַ
  if(error == NO_ERROR) error = SHT3X_WriteCommand(CMD_FETCH_DATA); // ���û�д���д��[���ڶ�ֵ]������
	SHT3X_StopAccess();
  if(error == NO_ERROR) error = SHT3X_StartReadAccess();  //��ʼ������--д����ʵĵ�ַ
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueTemp, ACK, 0); //��Ӧ��ģʽ��ȡԭʼ�¶�ֵ
  if(error == NO_ERROR) error = SHT3X_Read2BytesAndCrc(&rawValueHumi, NACK, 0);//�Է�Ӧ��ģʽ��ȡԭʼʪ��ֵ
  // ���û�д��󣬼���%RH�ڵ��¶Ⱥ�ʪ��
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
static etError SHT3X_StartWriteAccess(void)//��ʼд����
{
  etError error; 	
  // I2C��ʼ�ź�
  I2c_StartCondition();
  // ��д���д������I2C��ַ
  error = I2c_WriteByte(_i2cAddress << 1);//0λд
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_StartReadAccess(void)
{
  etError error;
  // I2C��ʼ�ź�
  I2c_StartCondition();
  // ʹ�ö���Ǳ�д������I2C��ַ--1�Ƕ���0��д
  error = I2c_WriteByte(_i2cAddress << 1 | 0x01);//0x8B
	
  return error;
}

//-----------------------------------------------------------------------------
static void SHT3X_StopAccess(void)
{
  // I2Cֹͣ�ź�
  I2c_StopCondition();
}

//-----------------------------------------------------------------------------
static etError SHT3X_WriteCommand(etCommands command)
{
  etError error; 
  error  = I2c_WriteByte(command >> 8);//������ĸ�8λд�봫����
  error |= I2c_WriteByte(command & 0xFF);//������ĵ�8λд�봫����
  return error;//����д��������Ӧ���źţ�����Ƿ�д��ɹ�
}

//-----------------------------------------------------------------------------
static etError SHT3X_Read2BytesAndCrc(u16t* data, etI2cAck finaleAckNack,
                                      u8t timeout)
{
  etError error;    
  u8t     bytes[2]; // ��ȡ���ݵ�����
  u8t     checksum; // У����ֽ�
  // ��ȡ���������ֽں�һ��У����ֽ�
                        error = I2c_ReadByte(&bytes[0], ACK, (u8t)255);
  if(error == NO_ERROR) error = I2c_ReadByte(&bytes[1], ACK, (u8t)255);
  if(error == NO_ERROR) error = I2c_ReadByte(&checksum, finaleAckNack, (u8t)255);
	//У�����-->�ѽ��
  if(error == NO_ERROR) error = SHT3X_CheckCrc(bytes, 2, checksum);// ��֤У���
	if(error == NO_ERROR)
	{
		*data = (bytes[0] << 8) | bytes[1];// �������ֽںϲ�Ϊһ��16λֵ		
	}
  return error;
}

//-----------------------------------------------------------------------------
static etError SHT3X_Write2BytesAndCrc(u16t data)
{
  etError error;    
  u8t     bytes[2]; // ��ȡ���ݵ�����
  u8t     checksum; // У����ֽ�
  
  bytes[0] = data >> 8;
  bytes[1] = data & 0xFF;
  checksum = SHT3X_CalcCrc(bytes, 2);//��ȡУ���
 
  // д�����������ֽں�һ��У����ֽ�
                        error = I2c_WriteByte(bytes[0]); // д��λ
  if(error == NO_ERROR) error = I2c_WriteByte(bytes[1]); // д��λ
  if(error == NO_ERROR) error = I2c_WriteByte(checksum); // дУ���
  
  return error;
}

static u8t SHT3X_CalcCrc(u8t data[], u8t nbrOfBytes)
{
  u8t bit;        // λ��
  u8t crc = 0xFF; // ����У���
  u8t byteCtr;    // �ֽڼ�����
  
  // �ø����Ķ���ʽ����8λУ���
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
  u8t crc;     // ����У���
  // ����8λ��У���
  crc = SHT3X_CalcCrc(data, nbrOfBytes);
  // ��֤У���
  if(crc != checksum) return CHECKSUM_ERROR;
  else                return NO_ERROR;
}
/*------------------------------------------------
��������:ת��ԭʼ����ʪ�����ݣ���ȡʵ�ʵ���ʪ��ֵ
-------------------------------------------------*/

// ����ʵ���¶� T = -45 + 175 * rawValue / (2^16-1)
static ft SHT3X_CalcTemperature(u16t rawValue)
{
  return 175.0f * (ft)rawValue / 65535.0f - 45.0f;
}

// ����ʵ��ʪ�� RH = rawValue / (2^16-1) * 100
static ft SHT3X_CalcHumidity(u16t rawValue)
{
  return 100.0f * (ft)rawValue / 65535.0f;
}

/*----------------------------------------------
��������:�¶�ת��(ԭʼ-->ʵ��)
��������:����ȡ��ԭʼ�¶�����ת��Ϊʵ�ʵ��¶�����
��������:
				ft temperature �����ԭʼ�¶�ֵ
��		ʽ:rawT = (temperature + 45) / 175 * (2^16-1)
------------------------------------------------*/
static u16t SHT3X_CalcRawTemperature(ft temperature)
{
  return (temperature + 45.0f) / 175.0f * 65535.0f;
}

/*----------------------------------------------
��������:ʪ��ת��(ԭʼ-->ʵ��)
��������:����ȡ��ԭʼʪ������ת��Ϊʵ�ʵ��¶�����
��������:
				ft humidity �����ԭʼʪ��ֵ
��		ʽ:rawRH = humidity / 100 * (2^16-1)
------------------------------------------------*/
static u16t SHT3X_CalcRawHumidity(ft humidity)
{
  return humidity / 100.0f * 65535.0f;
}
