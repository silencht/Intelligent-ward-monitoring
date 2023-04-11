#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "light_voice.h"
//���������ų�ʼ����PE6
void BEEP_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_6, GPIO_DIR_OUT); 
  GPIO_OpenDrainConfig(HT_GPIOE, GPIO_PIN_6, ENABLE); //��©�����������3.3Vͬ�����Ϊ�͵�ƽ������Ҫ�������裬���ⲿ��Դ������5V
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}

//����������
void BEEP_long_EN(void )
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, RESET);
 delay_ms(1500);
 BEEP_DISEN();
}
//���ߵ�ƽ��������ʹ��
void BEEP_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}