#include "sensor.h"
#include "delay.h"
u8 sensor_flag=1;
void sensor_Init(void)
{

    CKCU_PeripClockConfig_TypeDef  CKCUClock ={{0}};
    CKCUClock.Bit.PA =1;
    CKCUClock.Bit.AFIO=1;
    CKCUClock.Bit.BKP = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PA, GPIO_PIN_5,  AFIO_MODE_DEFAULT);
    AFIO_GPAConfig(AFIO_PIN_5, AFIO_MODE_1);
//		AFIO_GPxConfig(GPIO_PA, AFIO_PIN_6, AFIO_MODE_DEFAULT);
   GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_5, GPIO_DIR_IN);    
    GPIO_SetOutBits(HT_GPIOA,GPIO_PIN_5);
    GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_5,GPIO_PR_DOWN );
}
//0 有效  在有人时输出为低电平
u8  sensor_handle(void)
{
  if(GPIO_ReadInBit(HT_GPIOA,GPIO_PIN_5)==RESET)
     sensor_flag=0;
    else
    sensor_flag= 1;
    return sensor_flag;
}

