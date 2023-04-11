#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "voice_light.h"
//BUZZ/LED引脚初始化，PE5/6/7
void LED_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE,GPIO_PIN_6|GPIO_PIN_7, GPIO_DIR_OUT); 
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6|GPIO_PIN_7, SET);//LED灯低电平亮，高电平不亮,BUZZ高电平触发
}
//拉低电平，使能LED1
void LED1_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, RESET);
}
//拉高电平，失能LED1
void LED1_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}
//拉低电平，使能LED2
void LED2_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_7, RESET);
}
//拉高电平，失能LED2
void LED2_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_7, SET);
}

//蜂鸣器引脚初始化，PE5--------------------------------------------------------------------------------------------------------------------------
void BEEP_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_5, GPIO_DIR_OUT); 
  GPIO_OpenDrainConfig(HT_GPIOE, GPIO_PIN_5, ENABLE); //开漏输出，蜂鸣器3.3V同样检测为低电平，所以要上拉电阻，用外部电源拉高至5V
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, SET);
}
//拉低电平，蜂鸣器使能
void BEEP_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, RESET);
 delay_ms(200);
 BEEP_DISEN();
}
//拉高电平，蜂鸣器使能
void BEEP_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, SET);
}
//语音播报引脚初始化，PE4------------------------------------------------------------------------------------------------------------------------
void VOICE_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_4, GPIO_DIR_OUT); 
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_4, RESET);
}
//语音播报使能
void VOICE_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_4, SET);//产生一个上升沿，语音播报一次
 delay_ms(10);
 VOICE_DISEN();
}
//语音播报失能
void VOICE_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_4, RESET);
}

//初始化加湿器
void HUMI_init()
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE,GPIO_PIN_3, GPIO_DIR_OUT); 
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, RESET);		//高电平失能
}
//开启加湿器
void HUMI_EN()
{
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, RESET);	
}
//关闭加湿器
void HUMI_DISEN()
{
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, SET);	
}
