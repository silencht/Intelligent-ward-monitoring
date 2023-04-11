#ifndef _IIC_H
#define _IIC_H
#include "ht32.h"
#define SCL_H         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_12,SET)
#define SCL_L         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_12,RESET)
   
#define SDA_H         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_13,SET)
#define SDA_L         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_13,RESET)

#define SCL_read      GPIO_ReadInBit(HT_GPIOB,GPIO_PIN_12)
#define SDA_read      GPIO_ReadInBit(HT_GPIOB,GPIO_PIN_13)

//配置IIC，GPIO端口
void I2C_GPIO_Config(void);

//IIC停止信号
void I2C_Stop(void);

//写一个字节 ：目标地址，内部寄存器地址
u8 Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data);

//读一个字节 ：目标地址，存储单元地址，存放缓冲指针，读取长度
u8 Single_ReadI2C(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length);
#endif
