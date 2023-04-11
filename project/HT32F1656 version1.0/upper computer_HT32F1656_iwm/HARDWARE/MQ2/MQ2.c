#include "MQ2.h"
#include "delay.h"
u8 MQ2_flag=0;
void MQ2_Init(void)
{
    CKCU_PeripClockConfig_TypeDef  CKCUClock ={{0}};
    CKCUClock.Bit.PA =1;
    CKCUClock.Bit.AFIO=1;
    CKCUClock.Bit.BKP        = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PA, GPIO_PIN_6,  AFIO_MODE_DEFAULT);
    AFIO_GPAConfig(AFIO_PIN_6, AFIO_MODE_1);//AFIO_GPxConfig(GPIO_PA, AFIO_PIN_6, AFIO_MODE_DEFAULT);
   GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_6, GPIO_DIR_IN);    
   GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_6,GPIO_PR_UP);
   GPIO_InputConfig(HT_GPIOA, GPIO_PIN_6, ENABLE);
    GPIO_ClearOutBits(HT_GPIOA,GPIO_PIN_6);
 // GPTM_IntConfig(HT_GPTM1, GPTM_INT_UEV, ENABLE);
  /* Enable  GPTM */
 // GPTM_Cmd(HT_GPTM1, ENABLE);
}    
	//烟雾处理

 u8  MQ2_handle(void)
 {
    if(GPIO_ReadInBit(HT_GPIOA,GPIO_PIN_6)==SET)//
     MQ2_flag=1;//有烟雾时
    else
    MQ2_flag= 0;
    
    return MQ2_flag;
    
 }
    
