#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"


u8 sensor1_flag=0;
/**********************************************************************
外部中断4
红外热释电模块
**********************************************************************/
void EXTI10_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_PullResistorConfig(HT_GPIOE, GPIO_PIN_10, GPIO_PR_DOWN);
	AFIO_GPAConfig(AFIO_PIN_10, AFIO_MODE_1);	
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_10, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_10, ENABLE);
	
	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_10;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;
	EXTI_InitStruct.EXTI_DebounceCnt = 720000;
	EXTI_InitStruct.EXTI_IntType = EXTI_POSITIVE_EDGE;//
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_10, AFIO_ESS_PE);//
	EXTI_IntConfig(EXTI_CHANNEL_10, ENABLE);
	NVIC_EnableIRQ(EXTI10_IRQn);
}

/**********************************************************************
外部中断10服务函数
**********************************************************************/
void EXTI10_IRQHandler(void)
{
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_10,EXTI_EDGE_POSITIVE)!=RESET)
    {
        printf("get exti\r\n");    
   sensor1_flag=1;      
    }
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_10);
}

