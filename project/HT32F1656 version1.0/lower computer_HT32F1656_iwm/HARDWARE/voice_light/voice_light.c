#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "voice_light.h"
//BUZZ/LED���ų�ʼ����PE5/6/7
void LED_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE,GPIO_PIN_6|GPIO_PIN_7, GPIO_DIR_OUT); 
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6|GPIO_PIN_7, SET);//LED�Ƶ͵�ƽ�����ߵ�ƽ����,BUZZ�ߵ�ƽ����
}
//���͵�ƽ��ʹ��LED1
void LED1_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, RESET);
}
//���ߵ�ƽ��ʧ��LED1
void LED1_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_6, SET);
}
//���͵�ƽ��ʹ��LED2
void LED2_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_7, RESET);
}
//���ߵ�ƽ��ʧ��LED2
void LED2_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_7, SET);
}

//���������ų�ʼ����PE5--------------------------------------------------------------------------------------------------------------------------
void BEEP_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_5, GPIO_DIR_OUT); 
  GPIO_OpenDrainConfig(HT_GPIOE, GPIO_PIN_5, ENABLE); //��©�����������3.3Vͬ�����Ϊ�͵�ƽ������Ҫ�������裬���ⲿ��Դ������5V
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, SET);
}
//���͵�ƽ��������ʹ��
void BEEP_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, RESET);
 delay_ms(200);
 BEEP_DISEN();
}
//���ߵ�ƽ��������ʹ��
void BEEP_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_5, SET);
}
//�����������ų�ʼ����PE4------------------------------------------------------------------------------------------------------------------------
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
//��������ʹ��
void VOICE_EN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_4, SET);//����һ�������أ���������һ��
 delay_ms(10);
 VOICE_DISEN();
}
//��������ʧ��
void VOICE_DISEN(void)
{
 GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_4, RESET);
}

//��ʼ����ʪ��
void HUMI_init()
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PE                    = 1;
  CKCUClock.Bit.BKP                   = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOE,GPIO_PIN_3, GPIO_DIR_OUT); 
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, RESET);		//�ߵ�ƽʧ��
}
//������ʪ��
void HUMI_EN()
{
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, RESET);	
}
//�رռ�ʪ��
void HUMI_DISEN()
{
	GPIO_WriteOutBits(HT_GPIOE, GPIO_PIN_3, SET);	
}
