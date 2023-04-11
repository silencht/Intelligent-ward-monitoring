#ifndef I2C_HAL_H
#define I2C_HAL_H

//-- Includes -----------------------------------------------------------------
#include "system.h"
#include "usart.h"
#include "sys.h"
#include "delay.h"
//-- Defines ------------------------------------------------------------------

//IO方向设置
#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

//IO操作函数	 
#define IIC_SDA    PBout(7) 		//SDA	 
#define READ_SDA   PBin(7) 	 		//输入SDA 
#define IIC_SCL    PBout(6) 		//SCL
#define READ_SCL   PBin(6) 	 		//输入SCL

// I2C 应答
typedef enum{
  ACK  = 0,
  NACK = 1,
}etI2cAck;

//=============================================================================
void I2c_Init(void);
void I2c_StartCondition(void);
void I2c_StopCondition(void);
etError I2c_WriteByte(u8t txByte);
etError I2c_ReadByte(u8t *rxByte, etI2cAck ack, u8t timeout);
etError I2c_GeneralCallReset(void);

#endif
