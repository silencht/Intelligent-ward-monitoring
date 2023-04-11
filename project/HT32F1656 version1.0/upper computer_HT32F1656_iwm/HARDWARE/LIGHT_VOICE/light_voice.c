#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "light_voice.h"
//蜂鸣器引脚初始化，PE6
void BEEP_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_6, GPIO_DIR_OUT); 
  GPIO_OpenDrainConfig(HT_GPIOE, GPIO_PIN_6, ENABLE); //开漏输出，蜂鸣器3.3V同样检测为低电平，所以要上拉电阻，用外部电源拉高至5V
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}

//蜂鸣器长鸣
void BEEP_long_EN(void )
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, RESET);
 delay_ms(1500);
 BEEP_DISEN();
}
//拉高电平，蜂鸣器使能
void BEEP_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}