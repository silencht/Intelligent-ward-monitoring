#include "BEEP.h" 
#include "delay.h"
/*-------------------------------------------------------------------------
	功能：操作GPIO口
	作者：孙昌贺	
	时间：2018/6/7(version1.0)
	修改说明：
--------------------------------------------------------------------------*/

/***************************************************************************
	*函数名称：GPIO初始化函数
	*函数功能：初始化GPIO口
	*入口参数：无
	*返回参数：无
	*备注：
		GPIO_InitStructure参数说明：
		GPIO_Pin：  GPIO_Pin_x ，x can be 0~15
		GPIO_Mode： GPIO_Mode_IN 输入 GPIO_Mode_OUT 输出 GPIO_Mode_AF 端口复用 GPIO_Mode_AN 模拟
		GPIO_OType：GPIO_OType_PP 推挽输出、GPIO_OType_OD 开漏输出
		GPIO_Speed：GPIO_Speed_2MHz、GPIO_Speed_25MHz、GPIO_Speed_50MHz、GPIO_Speed_100MHz
		GPIO_PuPd： GPIO_PuPd_NOPULL 无拉电阻 GPIO_PuPd_UP 上拉 GPIO_PuPd_DOWN 下拉
                                  
****************************************************************************/
void BEEP_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;                //定义GPIO配置结构体

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;						 //配置GPIOF8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;       //开漏输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //速度100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;         //上拉电阻
  GPIO_Init(GPIOF, &GPIO_InitStructure);               //初始化函数，对GPIOF口进行初始化
	
  GPIO_SetBits(GPIOF,GPIO_Pin_8);        							 //GPIOF8设置高
//GPIO_ResetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);      //GPIOF9,F10设置低
//GPIO_WriteBit(GPIOF,GPIO_Pin_9 | GPIO_Pin_10,Bit_RESET);//GPIOF9,F10设置低，可用Bit_SET拉高
}

//蜂鸣器长鸣
void BEEP_long_EN(void )
{
 GPIO_ResetBits(GPIOF,GPIO_Pin_8);        							 //GPIOF8设置低
 delay_ms(1500);
 BEEP_DISEN();
}
//拉高电平，蜂鸣器使能
void BEEP_DISEN(void)
{
 GPIO_SetBits(GPIOF,GPIO_Pin_8);                         //GPIOF8设置高
}

