#include "beep.h"
#include "delay.h"
void GPTM_inint(u16 arr,u16 psc,u16 asc)
{
	uint32_t wCRR=0, wPSCR=0, wBeeIndex=0;
	GPTM_TimeBaseInitTypeDef TimeBaseInit;
	GPTM_OutputInitTypeDef OutInit; 
	
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.GPTM1      = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);	
	HT32F_DVB_GPxConfig(GPIO_PE, GPIO_PIN_15, AFIO_MODE_4);

	/* Enable PCLK of BUZZER GPTM */
  //CKCU_APBPerip1ClockConfig(BUZZER_BUZZER_CLK, ENABLE);

  /* Init BUZZER GPTM time-base */
  GPTM_TimeBaseStructInit(&TimeBaseInit);
  TimeBaseInit.CounterMode = GPTM_CNT_MODE_UP;
  TimeBaseInit.CounterReload = arr;
  TimeBaseInit.Prescaler = psc;
  TimeBaseInit.PSCReloadTime = GPTM_PSC_RLD_IMMEDIATE;
  GPTM_TimeBaseInit(HT_GPTM1, &TimeBaseInit);

  /* Clear Update Event Interrupt flag */
  GPTM_ClearFlag(HT_GPTM1, GPTM_FLAG_UEV);

  /* Init BUZZER GPTM Channel x to output PWM waveform with 33% duty */
  GPTM_OutputStructInit(&OutInit);
  OutInit.Channel = GPTM_CH_0;
  OutInit.OutputMode = GPTM_OM_PWM2;
  OutInit.Control = GPTM_CHCTL_ENABLE;
  OutInit.Polarity = GPTM_CHP_NONINVERTED;
  OutInit.Compare = asc ;
  GPTM_OutputInit(HT_GPTM1, &OutInit);

  /* Enable interrupt of BUZZER GPTM update event */
//  NVIC_EnableIRQ(BUZZER_IRQn);
  GPTM_IntConfig(HT_GPTM1, GPTM_INT_UEV, ENABLE);

  /* Enable BUZZER GPTM */
  GPTM_Cmd(HT_GPTM1, ENABLE);
	
}












