#include "wdt.h"
#include "ht32f1655_56_wdt.h"
void WDT_Init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  /* Initialize LED                                                                                        */
  CKCUClock.Bit.PC                    = 1;
	CKCUClock.Bit.AFIO                  = 1;
  CKCUClock.Bit.BKP                   = 1;
	/* Enable WDT APB clock                                                                                   */
  CKCUClock.Bit.WDT 									= 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  /* Configure AFIO mode of output pins                                                                     */
  AFIO_GPxConfig(GPIO_PC, AFIO_PIN_9, AFIO_MODE_DEFAULT);
  /* Configure GPIO direction of output pins                                                                */
  GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_9, GPIO_DIR_OUT);
  /* Infinite loop to read data from input pin and then output to LED                                       */
	
	/*----------------------------- WatchDog configuration ---------------------------------------------------*/
  WDT_DeInit();                        //���Ź������ָ�Ĭ��
  WDT_SetPrescaler(WDT_PRESCALER_128); // Set Prescaler Value 128           ���Ź�ʱ������17s����
  WDT_SetReloadValue(0xEFF);           // Set Reload Value 1110 1111 1111 =3839    32khz/32=1000hz ��3839/1000=3.839s��32khz/64=500hz��3839/500=7.6s
  WDT_ResetCmd(ENABLE);								 //���ÿ��Ź�ģʽΪ��λ�������жϣ�
	WDT_Cmd(ENABLE);										 //���Ź�ʹ��
  WDT_Restart();                       // Reload Counter as WDTV Value��ι��������
	GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, SET);		//���Ź�����ָʾ�ƣ���ɫ������λ������ԣ�  	
}
void close_wdt_led(void)
{
  GPIO_WriteOutBits(HT_GPIOC, GPIO_PIN_9, RESET);	//�رտ��Ź���
}

