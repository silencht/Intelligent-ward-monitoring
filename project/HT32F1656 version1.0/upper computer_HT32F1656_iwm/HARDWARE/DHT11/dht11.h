#ifndef __DHT11_H 
#define __DHT11_H  
#include "ht32_board.h" 
  
//IO�������� 
#define DHT11_IO_IN()  {GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_IN);}
#define DHT11_IO_OUT() {GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_OUT);}

#define DHT11_IO1_IN()  {GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_10, GPIO_DIR_IN);}
#define DHT11_IO1_OUT() {GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_10, GPIO_DIR_OUT);}
////IO��������1
#define	DHT11_DQ_OUT1 GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_7,  1) //���ݶ˿�	PA7  
#define	DHT11_DQ_OUT0 GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_7,  0) //���ݶ˿�	PA7  
#define	DHT11_DQ_IN  GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_7)  //���ݶ˿�	PA7

#define	DHT11_DQ1_OUT1 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_10,  1) //���ݶ˿�	PA7  
#define	DHT11_DQ1_OUT0 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_10,  0) //���ݶ˿�	PA7  
#define	DHT11_DQ1_IN  GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_10)  //���ݶ˿�	PA7

//////IO��������	    
//#define	DHT11_DQ_OUT PGout(11) //���ݶ˿�	PA0  
//#define	DHT11_DQ_IN  PGin(11)  //���ݶ˿�	PA0  


void DHT11_Init(void);//��ʼ��DHT11 
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//��ȡ��ʪ�� 
u8 DHT11_Read_Byte(void);//����һ���ֽ� 
u8 DHT11_Read_Bit(void);//����һ��λ 
u8 DHT11_Check(void);//����Ƿ����DHT11 
void DHT11_Rst(void);//��λDHT11 
u8 DHT11_ReadData1(u8 *temp,u8 *humi);
//�ڶ���

u8 DHT11_Read_Data_1(u8 *temp,u8 *humi);//��ȡ��ʪ�� 
u8 DHT11_Read_Byte_1(void);//����һ���ֽ� 
u8 DHT11_Read_Bit_1(void);//����һ��λ 
u8 DHT11_Check_1(void);//����Ƿ����DHT11 
void DHT11_Rst_1(void);//��λDHT11 
u8 DHT11_ReadData1_1(u8 *temp,u8 *humi);
void wenshidu(void);
#endif 

