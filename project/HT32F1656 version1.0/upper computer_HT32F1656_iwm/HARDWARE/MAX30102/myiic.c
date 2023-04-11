/*
*********************************************************************************************************
*
*	模块名称 : I2C总线驱动模块
*	文件名称 : bsp_i2c_gpio.c
*	版    本 : V1.0
*	说    明 : 用gpio模拟i2c总线, 适用于STM32F4系列CPU。该模块不包括应用层命令帧，仅包括I2C总线基本操作函数。
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2015-05-21 armfly  正式发布
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

/*
	应用说明：
	在访问I2C设备前，请先调用 i2c_CheckDevice() 检测I2C设备是否正常，该函数会配置GPIO
*/
#include "HT32.h"
#include "myiic.h"
#include "delay.h"
#include "sht3x.h"



u8 in_out=0;
/*
*********************************************************************************************************
*	函 数 名: bsp_InitI2C OUT
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现  D2:SCL    D3:SDA
*	形    参: 无
*	返 回 值: in_out=0
*********************************************************************************************************
*/
void bsp_InitI2C_OUT(void)
{	
		CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
		CKCUClock.Bit.PD                    = 1;
		CKCUClock.Bit.BKP                   = 1;
		CKCU_PeripClockConfig(CKCUClock, ENABLE);

//  /* Configure GPIO direction of output pins                            */
		GPIO_DirectionConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_DIR_OUT);//
  	GPIO_OpenDrainConfig(HT_GPIOD,PIN_I2C_SCL|PIN_I2C_SDA, ENABLE);
		in_out=0;
}
/*********************************************************************************************************
*	函 数 名: bsp_InitI2C IN
*	功能说明: 配置I2C总线的GPIO，采用模拟IO的方式实现  D2:SCL    D3:SDA
*	形    参: 无
*	返 回 值: in_out=1
*********************************************************************************************************/
void bsp_InitI2C_IN(void)
  {
		CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
		CKCUClock.Bit.PD                    = 1;
		CKCUClock.Bit.BKP                   = 1;
		CKCUClock.Bit.AFIO                  = 1;
		CKCU_PeripClockConfig(CKCUClock, ENABLE);

		AFIO_GPDConfig(AFIO_PIN_2|AFIO_PIN_3, AFIO_MODE_DEFAULT);
		/* Configure GPIO direction of input pins                                                                 */
		GPIO_DirectionConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_DIR_IN);
		/* Configure GPIO pull resistor of input pins                                                             */
//	  GPIO_PullResistorConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, GPIO_PR_UP);
		GPIO_InputConfig(HT_GPIOD, PIN_I2C_SCL|PIN_I2C_SDA, ENABLE);
		in_out=1;
	}
/*
*********************************************************************************************************
*	函 数 名: i2c_Delay
*	功能说明: I2C总线位延迟，最快400KHz
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void i2c_Delay(void)
{
	uint8_t i;
	/*　
		CPU主频168MHz时，在内部Flash运行, MDK工程不优化。用台式示波器观测波形。
		循环次数为5时，SCL频率  = 1.78MHz (读耗时: 92ms, 读写正常，但是用示波器探头碰上就读写失败。时序接近临界)
		循环次数为10时，SCL频率 = 1.1MHz (读耗时: 138ms, 读速度: 118724B/s)
		循环次数为30时，SCL频率 = 440KHz， SCL高电平时间1.0us，SCL低电平时间1.2us

		上拉电阻选择2.2K欧时，SCL上升沿时间约0.5us，如果选4.7K欧，则上升沿约1us

		实际应用选择400KHz左右的速率即可
	*/
	for (i = 0; i < 30; i++);
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Start
*	功能说明: CPU发起I2C总线启动信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Start(void)
{
if(in_out==0)
		{
		/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
			I2C_SDA_1();
			    delay_us(10);
			I2C_SCL_1();
//		  i2c_Delay();
      delay_us(10);
			I2C_SDA_0();
//			i2c_Delay();	
			delay_us(100);		
			I2C_SCL_0();
//			i2c_Delay();
			delay_us(100);
		}
else 
		{
    bsp_InitI2C_OUT();
		/* 当SCL高电平时，SDA出现一个下跳沿表示I2C总线启动信号 */
			I2C_SDA_1();
			    delay_us(10);
			I2C_SCL_1();
//		  i2c_Delay();
      delay_us(10);
			I2C_SDA_0();
//			i2c_Delay();	
			delay_us(100);		
			I2C_SCL_0();
//			i2c_Delay();
			delay_us(100);
		}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Stop
*	功能说明: CPU发起I2C总线停止信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Stop(void)
{	
	if(in_out==0)
		{
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
			I2C_SDA_0();
			I2C_SCL_1();
//			i2c_Delay();
      delay_us(100);
			I2C_SDA_1();
//			i2c_Delay();
			delay_us(100);
		}
else 
		{
    bsp_InitI2C_OUT();
	/* 当SCL高电平时，SDA出现一个上跳沿表示I2C总线停止信号 */
			I2C_SDA_0();
			I2C_SCL_1();
//			i2c_Delay();
      delay_us(100);
			I2C_SDA_1();
//			i2c_Delay();
			delay_us(100);
		}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_SendByte(uint8_t _ucByte)
{
		uint8_t i;
if(in_out==0)
		{	
			/* 先发送字节的高位bit7 */
			for (i = 0; i < 8; i++)
			{
				if (_ucByte & 0x80)
				{
					I2C_SDA_1();
				}
				else
				{
					I2C_SDA_0();
				}
//				i2c_Delay();
        delay_us(10);
				I2C_SCL_1();
//				i2c_Delay();	
        delay_us(50);//等待从机接收					
				I2C_SCL_0();
				if (i == 7)
				{
					 I2C_SDA_1(); // 释放总线
				}
				_ucByte <<= 1;	/* 左移一个bit */
//				i2c_Delay();
			}
			
//				I2C_SDA_1();	/* CPU释放SDA总线 */
			  i2c_Delay();
			  I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
				
		}
else
		{
		 bsp_InitI2C_OUT();

		/* 先发送字节的高位bit7 */
			for (i = 0; i < 8; i++)
			{
				if (_ucByte & 0x80)
				{
					I2C_SDA_1();
				}
				else
				{
					I2C_SDA_0();
				}
//				i2c_Delay();
        delay_us(10);
				I2C_SCL_1();
//				i2c_Delay();	
        delay_us(50);//等待从机接收					
				I2C_SCL_0();
				if (i == 7)
				{
					 I2C_SDA_1(); // 释放总线
				}
				_ucByte <<= 1;	/* 左移一个bit */
				i2c_Delay();
			}
			
//				I2C_SDA_1();	/* CPU释放SDA总线 */
//			  i2c_Delay();
			  I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
		}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_SHT30_SendByte
*	功能说明: CPU向I2C总线设备发送8bit数据
*	形    参:  _ucByte ： 等待发送的字节
*	返 回 值: 无
*********************************************************************************************************
*/
uint8_t i2c_SHT30_SendByte(uint8_t _ucByte)
{
		uint8_t     mask,err=3;
if(in_out==0)
		{		
				for(mask = 0x80; mask > 0; mask >>= 1)		// shift bit for masking (8 times)
				{
					if((mask & _ucByte) == 0) {I2C_SDA_0(); } 	//数据线拉低，发送0--printf("%ld\r\n",SDA_LOW());b:10001000
					else                     {I2C_SDA_1();}	//数据线拉高，发送1--printf("%ld\r\n",SDA_OPEN());
					delay_us(10);
					I2C_SCL_1();	  //时钟线拉高，告诉从机，主机数据发送完毕
					delay_us(50);   //电平保持时间，等待从机接收数据
					I2C_SCL_0();	  //时钟线拉低，告诉从机，主机需要发送数据
					delay_us(10);   //电平保持时间，等待时钟线稳定
				}
				I2C_SDA_1(); 		  //数据线上拉
				I2C_SCL_1();			//时钟线拉高，告诉从机，主机现在开始读取数据
				delay_us(100);                 					// data set-up time (t_SU;DAT)
	      err=i2c_SHT30_WaitAck();      //检测从机是否应答
				bsp_InitI2C_OUT();
		    I2C_SCL_0();
        delay_us(10);
				return err;
		}
else
		{
		    bsp_InitI2C_OUT();
				for(mask = 0x80; mask > 0; mask >>= 1)		// shift bit for masking (8 times)
				{
					if((mask & _ucByte) == 0) {I2C_SDA_0(); } 	//数据线拉低，发送0--printf("%ld\r\n",SDA_LOW());
					else                     {I2C_SDA_1();;}	//数据线拉高，发送1--printf("%ld\r\n",SDA_OPEN());
					delay_us(10);
					I2C_SCL_1();	  //时钟线拉高，告诉从机，主机数据发送完毕
					delay_us(50);   //电平保持时间，等待从机接收数据
					I2C_SCL_0();	  //时钟线拉低，告诉从机，主机需要发送数据
					delay_us(10);   //电平保持时间，等待时钟线稳定
				}
				I2C_SDA_1(); 		  //数据线上拉
				I2C_SCL_1();			//时钟线拉高，告诉从机，主机现在开始读取数据
				delay_us(100);                 					// data set-up time (t_SU;DAT)
				delay_us(200);
				err=i2c_SHT30_WaitAck();      //检测从机是否应答
				bsp_InitI2C_OUT();
		    I2C_SCL_0();
        delay_us(10);
				return err;
		}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_ReadByte
*	功能说明: CPU从I2C总线设备读取8bit数据
*	形    参:  
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
uint8_t i2c_ReadByte(etI2cAck ack)
{
		uint8_t i;
		uint8_t value;
		bsp_InitI2C_OUT();
		/* 读到第1个bit为数据的bit7 */
		value = 0;
		
		for (i = 0; i < 8; i++)
		{
			value <<= 1;
			I2C_SCL_0();
//		  i2c_Delay();//原本在for循环最后
      delay_us(20); //等待时钟线稳定
			
			I2C_SCL_1();
//			i2c_Delay();
			delay_us(30); 
			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())
			{
				value++;
			}
		  bsp_InitI2C_OUT();
      delay_us(20);

		}
		I2C_SCL_0();
		
		if(ack == ACK) 
		{
		I2C_SDA_0();	  
		delay_us(10);                  // data set-up time (t_SU;DAT)
		I2C_SCL_1();                    // clk #9 for ack
		delay_us(50);                  // SCL high time (t_HIGH)
		I2C_SCL_0();
		I2C_SDA_1();                    // release SDA-line
		delay_us(200);  
		}                              // send acknowledge if necessary printf("RxByte=0x%x  ACK\r\n",*rxByte);
		else if(ack == NACK)
		{
		I2C_SDA_1();
		delay_us(10);                  // data set-up time (t_SU;DAT)
		I2C_SCL_1();                    // clk #9 for ack
		delay_us(50);                  // SCL high time (t_HIGH)
		I2C_SCL_0();
		I2C_SDA_1();                    // release SDA-line
		delay_us(200);  
		}
		else{;}
		
//		else           {I2C_SDA_1();i2c_Delay();I2C_SCL_1();i2c_Delay();I2C_SCL_0();I2C_SDA_1();i2c_Delay();}						 //printf("RxByte=0x%x  NACK\r\n",*rxByte);
		
		return value;
	}

/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_WaitAck(void)
{
			uint8_t re;
//      bsp_InitI2C_OUT();			
//			I2C_SDA_1();	/* CPU释放SDA总线 */
//			i2c_Delay();
//			I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
			i2c_Delay();
			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
			{
				re = 1;
			}
			else
			{
				re = 0;
			}
		  bsp_InitI2C_OUT();
			I2C_SCL_0();
			i2c_Delay();
			return re;
}
/*
*********************************************************************************************************
*	函 数 名: i2c_WaitAck
*	功能说明: CPU产生一个时钟，并读取器件的ACK应答信号
*	形    参:  无
*	返 回 值: 返回0表示正确应答，1表示无器件响应
*********************************************************************************************************
*/
uint8_t i2c_SHT30_WaitAck(void)
{
			uint8_t re;
//      bsp_InitI2C_OUT();			
//			I2C_SDA_1();	/* CPU释放SDA总线 */
//			i2c_Delay();
//			I2C_SCL_1();	/* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
			i2c_Delay();			
			bsp_InitI2C_IN();
			if (I2C_SDA_READ())	/* CPU读取SDA口线状态 */
			{
				re = 1;
			}
			else
			{
				re = 0;
			}
			return re;
}

/*
*********************************************************************************************************
*	函 数 名: i2c_Ack
*	功能说明: CPU产生一个ACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_Ack(void)
{
if(in_out==0)
	{
		I2C_SDA_0();	/* CPU驱动SDA = 0 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU产生1个时钟 */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
		I2C_SDA_1();	/* CPU释放SDA总线 */
	}
else
	{  
	  bsp_InitI2C_OUT();
	 	I2C_SDA_0();	/* CPU驱动SDA = 0 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU产生1个时钟 */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
		I2C_SDA_1();	/* CPU释放SDA总线 */
	
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_NAck
*	功能说明: CPU产生1个NACK信号
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void i2c_NAck(void)
{	
	if(in_out==0)
	{
		I2C_SDA_1();	/* CPU驱动SDA = 1 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU产生1个时钟 */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();
	}
else
	{  
		bsp_InitI2C_OUT();
		I2C_SDA_1();	/* CPU驱动SDA = 1 */
		i2c_Delay();
		I2C_SCL_1();	/* CPU产生1个时钟 */
		i2c_Delay();
		I2C_SCL_0();
		i2c_Delay();	
	}
}

/*
*********************************************************************************************************
*	函 数 名: i2c_CheckDevice
*	功能说明: 检测I2C总线设备，CPU向发送设备地址，然后读取设备应答来判断该设备是否存在
*	形    参:  _Address：设备的I2C总线地址
*	返 回 值: 返回值 0 表示正确， 返回1表示未探测到
*********************************************************************************************************
*/
uint8_t i2c_CheckDevice(uint8_t _Address)
{
	uint8_t ucAck;
if(in_out==1)
	{
		if (I2C_SDA_READ() && I2C_SCL_READ())
		{
			i2c_Start();		/* 发送启动信号 */

			/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
			i2c_SendByte(_Address | I2C_WR);
			ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

			i2c_Stop();			/* 发送停止信号 */

			return ucAck;
		}
		return 1;	/* I2C总线异常 */
	}
else
	{  
		bsp_InitI2C_IN();
		if (I2C_SDA_READ() && I2C_SCL_READ())
		{
			i2c_Start();		/* 发送启动信号 */

			/* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
			i2c_SendByte(_Address | I2C_WR);
			ucAck = i2c_WaitAck();	/* 检测设备的ACK应答 */

			i2c_Stop();			/* 发送停止信号 */

			return ucAck;
		}
		return 1;	/* I2C总线异常 */
	}

}


/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
