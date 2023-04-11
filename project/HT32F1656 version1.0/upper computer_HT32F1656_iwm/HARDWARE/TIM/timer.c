#include "timer.h"

extern vu16 USART0_RX_STA;
extern vu16 USART1_RX_STA;

void BFTM0_init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.BFTM0      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  NVIC_EnableIRQ(BFTM0_IRQn);
  NVIC_SetPriorityGrouping(BFTM0_IRQn);
  NVIC_SetPriority(BFTM0_IRQn, NVIC_EncodePriority(BFTM0_IRQn, 3, 0));                                                                                
  NVIC_SetPendingIRQ(BFTM0_IRQn);	
  BFTM_SetCompare(HT_BFTM0, SystemCoreClock/100);		//定时10ms
  BFTM_SetCounter(HT_BFTM0, 0);
  BFTM_IntConfig(HT_BFTM0, ENABLE);
  BFTM_EnaCmd(HT_BFTM0, ENABLE);
}

void BFTM0_IRQHandler(void)
{
	if(BFTM_GetFlagStatus(HT_BFTM0) != RESET)
	{
		USART0_RX_STA |= 1<<15;		//标志接收完成
		BFTM_ClearFlag(HT_BFTM0); //清除中断标志位
		BFTM_EnaCmd(HT_BFTM0,DISABLE);
	}
}

void BFTM1_init(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.BFTM1      = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  NVIC_EnableIRQ(BFTM1_IRQn);
  NVIC_SetPriorityGrouping(BFTM1_IRQn);
  NVIC_SetPriority(BFTM1_IRQn, NVIC_EncodePriority(BFTM1_IRQn, 3, 1));                                                                                
  NVIC_SetPendingIRQ(BFTM1_IRQn);	
  BFTM_SetCompare(HT_BFTM1, SystemCoreClock/100);		//定时10ms
  BFTM_SetCounter(HT_BFTM1, 0);
  BFTM_IntConfig(HT_BFTM1, ENABLE);
  BFTM_EnaCmd(HT_BFTM1, ENABLE);
}

void BFTM1_IRQHandler(void)
{
	if(BFTM_GetFlagStatus(HT_BFTM1) != RESET)
	{
		USART1_RX_STA |= 1<<15;		//标志接收完成
		BFTM_ClearFlag(HT_BFTM1); //清除中断标志位
		BFTM_EnaCmd(HT_BFTM1,DISABLE);
	}
}

