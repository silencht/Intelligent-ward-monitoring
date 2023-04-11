#ifndef _IIC_H
#define _IIC_H
#include "ht32.h"
#define SCL_H         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_12,SET)
#define SCL_L         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_12,RESET)
   
#define SDA_H         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_13,SET)
#define SDA_L         GPIO_WriteOutBits(HT_GPIOB,GPIO_PIN_13,RESET)

#define SCL_read      GPIO_ReadInBit(HT_GPIOB,GPIO_PIN_12)
#define SDA_read      GPIO_ReadInBit(HT_GPIOB,GPIO_PIN_13)

//����IIC��GPIO�˿�
void I2C_GPIO_Config(void);

//IICֹͣ�ź�
void I2C_Stop(void);

//дһ���ֽ� ��Ŀ���ַ���ڲ��Ĵ�����ַ
u8 Single_WriteI2C_byte(u8 Slave_Address,u8 REG_Address,u8 data);

//��һ���ֽ� ��Ŀ���ַ���洢��Ԫ��ַ����Ż���ָ�룬��ȡ����
u8 Single_ReadI2C(u8 Slave_Address,u8 REG_Address,u8 *REG_data,u8 length);
#endif
