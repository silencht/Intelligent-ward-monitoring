#include "key.h"
#include "delay.h"								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.PA         = 1;
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.BKP        = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	AFIO_GPxConfig(GPIO_PA, AFIO_PIN_6|AFIO_PIN_1, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_6|GPIO_PIN_1, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOA, GPIO_PIN_6|GPIO_PIN_1, ENABLE);
	GPIO_SetOutBits(HT_GPIOA,GPIO_PIN_6|GPIO_PIN_1);
	GPIO_PullResistorConfig(HT_GPIOA,GPIO_PIN_6|GPIO_PIN_1, GPIO_PR_UP);
}
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(KEY0==0||KEY1==0))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(KEY0==0)return KEY0_PRES;
		else if(KEY1==0)return KEY1_PRES;
	}else if(KEY0==1&&KEY1==1)key_up=1; 	    
 	return 0;// 无按键按下
}
