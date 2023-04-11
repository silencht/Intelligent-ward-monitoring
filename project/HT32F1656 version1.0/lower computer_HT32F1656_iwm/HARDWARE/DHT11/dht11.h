#ifndef __DHT11_H 
#define __DHT11_H  
#include "ht32_board.h" 
  
//IO方向设置 
#define DHT11_IO_IN()  {GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_IN);}
#define DHT11_IO_OUT() {GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_OUT);}

#define DHT11_IO1_IN()  {GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_10, GPIO_DIR_IN);}
#define DHT11_IO1_OUT() {GPIO_DirectionConfig(HT_GPIOB, GPIO_PIN_10, GPIO_DIR_OUT);}
////IO操作函数1
#define	DHT11_DQ_OUT1 GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_7,  1) //数据端口	PA7  
#define	DHT11_DQ_OUT0 GPIO_WriteOutBits(HT_GPIOA, GPIO_PIN_7,  0) //数据端口	PA7  
#define	DHT11_DQ_IN  GPIO_ReadInBit(HT_GPIOA, GPIO_PIN_7)  //数据端口	PA7

#define	DHT11_DQ1_OUT1 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_10,  1) //数据端口	PA7  
#define	DHT11_DQ1_OUT0 GPIO_WriteOutBits(HT_GPIOB, GPIO_PIN_10,  0) //数据端口	PA7  
#define	DHT11_DQ1_IN  GPIO_ReadInBit(HT_GPIOB, GPIO_PIN_10)  //数据端口	PA7

//////IO操作函数	    
//#define	DHT11_DQ_OUT PGout(11) //数据端口	PA0  
//#define	DHT11_DQ_IN  PGin(11)  //数据端口	PA0  


void DHT11_Init(void);//初始化DHT11 
u8 DHT11_Read_Data(u8 *temp,u8 *humi);//读取温湿度 
u8 DHT11_Read_Byte(void);//读出一个字节 
u8 DHT11_Read_Bit(void);//读出一个位 
u8 DHT11_Check(void);//检测是否存在DHT11 
void DHT11_Rst(void);//复位DHT11 
u8 DHT11_ReadData1(u8 *temp,u8 *humi);
//第二个

u8 DHT11_Read_Data_1(u8 *temp,u8 *humi);//读取温湿度 
u8 DHT11_Read_Byte_1(void);//读出一个字节 
u8 DHT11_Read_Bit_1(void);//读出一个位 
u8 DHT11_Check_1(void);//检测是否存在DHT11 
void DHT11_Rst_1(void);//复位DHT11 
u8 DHT11_ReadData1_1(u8 *temp,u8 *humi);
void wenshidu(void);
#endif 

