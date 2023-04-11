#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "voice_light.h"

u8 call_flag0=0; //呼叫医生按钮标志位，1为呼叫，0为非呼
u8 call_flag1=0;
/**********************************************************************
	呼叫医生按钮1
**********************************************************************/
void EXTI10_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_PullResistorConfig(HT_GPIOE, GPIO_PIN_10, GPIO_PR_DOWN); //下拉电阻
	AFIO_GPEConfig(AFIO_PIN_10, AFIO_MODE_1);	  //模式1.GPIO功能
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_10, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_10, ENABLE);
	
	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_10;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;  //按键去抖动使能
	EXTI_InitStruct.EXTI_DebounceCnt = 720000;           
	EXTI_InitStruct.EXTI_IntType = EXTI_BOTH_EDGE;		 //上升沿、下降沿均有效
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_10, AFIO_ESS_PE);   //通道10，GPIOE口
	EXTI_IntConfig(EXTI_CHANNEL_10, ENABLE);  						 //通道10，中断配置
	NVIC_EnableIRQ(EXTI10_IRQn);													 //中断使能
}
/**********************************************************************
	呼叫医生按钮1
**********************************************************************/
void EXTI11_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_PullResistorConfig(HT_GPIOE, GPIO_PIN_11, GPIO_PR_DOWN); //下拉电阻
	AFIO_GPEConfig(AFIO_PIN_11, AFIO_MODE_1);	  //模式1.GPIO功能
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_11, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_11, ENABLE);
	
	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_11;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;  //按键去抖动使能
	EXTI_InitStruct.EXTI_DebounceCnt = 720000;           
	EXTI_InitStruct.EXTI_IntType = EXTI_BOTH_EDGE;		 //上升沿、下降沿均有效
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_11, AFIO_ESS_PE);   //通道11，GPIOE口
	EXTI_IntConfig(EXTI_CHANNEL_11, ENABLE);  						 //通道11，中断配置
	NVIC_EnableIRQ(EXTI11_IRQn);													 //中断使能
}

/**********************************************************************
外部中断10服务函数
**********************************************************************/
void EXTI10_IRQHandler(void)
{
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_10,EXTI_EDGE_POSITIVE)!=RESET) //上升沿
    {
			call_flag0=1;
    }
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_10,EXTI_EDGE_NEGATIVE)!=RESET)
	   {
		  call_flag0=0;
     }
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_10);//清除中断标志位
}
/**********************************************************************
外部中断11服务函数
**********************************************************************/
void EXTI11_IRQHandler(void)
{
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_11,EXTI_EDGE_POSITIVE)!=RESET) //上升沿
    {
			call_flag1=1;
    }
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_11,EXTI_EDGE_NEGATIVE)!=RESET)
	   {
		  call_flag1=0;
     }
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_11);//清除中断标志位
}

