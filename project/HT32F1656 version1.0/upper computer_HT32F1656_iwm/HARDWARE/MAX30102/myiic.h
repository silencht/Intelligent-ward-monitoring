#ifndef __MYIIC_H
#define __MYIIC_H
/*
*********************************************************************************************************
*
*	ģ������ : I2C��������ģ��
*	�ļ����� : bsp_i2c_gpio.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ���
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "HT32.h"
#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */


/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */

//  #define RCC_I2C_PORT 	RCC_APB2Periph_GPIOC		/* GPIO�˿�ʱ�� */

#define PORT_I2C_SCL	HT_GPIOD			/* SCL:GPIO�˿� */
#define PIN_I2C_SCL		GPIO_PIN_2		/* GPIO���� */

#define PORT_I2C_SDA	HT_GPIOD			/* SDA:GPIO�˿� */
#define PIN_I2C_SDA		GPIO_PIN_3		/* GPIO���� */

#define I2C_SCL_PIN		GPIO_PIN_2			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_PIN_3			/* ���ӵ�SDA�����ߵ�GPIO */

/* �����дSCL��SDA�ĺ� */
#define I2C_SCL_1()  GPIO_WriteOutBits(PORT_I2C_SCL, PIN_I2C_SCL, SET)				/* SCL = 1 */
#define I2C_SCL_0()  GPIO_WriteOutBits(PORT_I2C_SCL, PIN_I2C_SCL, RESET)			/* SCL = 0 */

#define I2C_SDA_1()  GPIO_WriteOutBits(PORT_I2C_SDA, PIN_I2C_SDA, SET)				/* SDA = 1 */
#define I2C_SDA_0()  GPIO_WriteOutBits(PORT_I2C_SDA, PIN_I2C_SDA, RESET)			/* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInBit(PORT_I2C_SDA,I2C_SDA_PIN)	/* ��SDA����״̬ */
#define I2C_SCL_READ()  GPIO_ReadInBit(PORT_I2C_SCL,I2C_SCL_PIN)	/* ��SCL����״̬ */

// I2C Ӧ��
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

















