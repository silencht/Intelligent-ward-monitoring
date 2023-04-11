#ifndef __MYIIC_H
#define __MYIIC_H
/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : bsp_i2c_gpio.h
*	版    本 : V1.0
*	说    明 : 头文件。
*
*	Copyright (C), 2012-2013, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "HT32.h"
#define I2C_WR	0		/* 写控制bit */
#define I2C_RD	1		/* 读控制bit */


/* 定义I2C总线连接的GPIO端口, 用户只需要修改下面4行代码即可任意改变SCL和SDA的引脚 */

//  #define RCC_I2C_PORT 	RCC_APB2Periph_GPIOC		/* GPIO端口时钟 */

#define PORT_I2C_SCL	HT_GPIOD			/* SCL:GPIO端口 */
#define PIN_I2C_SCL		GPIO_PIN_2		/* GPIO引脚 */

#define PORT_I2C_SDA	HT_GPIOD			/* SDA:GPIO端口 */
#define PIN_I2C_SDA		GPIO_PIN_3		/* GPIO引脚 */

#define I2C_SCL_PIN		GPIO_PIN_2			/* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN		GPIO_PIN_3			/* 连接到SDA数据线的GPIO */

/* 定义读写SCL和SDA的宏 */
#define I2C_SCL_1()  GPIO_WriteOutBits(PORT_I2C_SCL, PIN_I2C_SCL, SET)				/* SCL = 1 */
#define I2C_SCL_0()  GPIO_WriteOutBits(PORT_I2C_SCL, PIN_I2C_SCL, RESET)			/* SCL = 0 */

#define I2C_SDA_1()  GPIO_WriteOutBits(PORT_I2C_SDA, PIN_I2C_SDA, SET)				/* SDA = 1 */
#define I2C_SDA_0()  GPIO_WriteOutBits(PORT_I2C_SDA, PIN_I2C_SDA, RESET)			/* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInBit(PORT_I2C_SDA,I2C_SDA_PIN)	/* 读SDA口线状态 */
#define I2C_SCL_READ()  GPIO_ReadInBit(PORT_I2C_SCL,I2C_SCL_PIN)	/* 读SCL口线状态 */

// I2C 应答
typedef enum{
  ACK  = 0,
  NACK = 1,
	NO   =3 ,
}etI2cAck;

void bsp_InitI2C_OUT(void);
void bsp_InitI2C_IN(void);
void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(etI2cAck ack);
uint8_t i2c_SHT30_SendByte(uint8_t _ucByte);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);

#endif

















