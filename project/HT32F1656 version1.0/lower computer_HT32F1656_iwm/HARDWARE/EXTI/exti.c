#include "delay.h"
#include "exti.h"
#include "usart.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
#include "voice_light.h"

u8 call_flag0=0; //����ҽ����ť��־λ��1Ϊ���У�0Ϊ�Ǻ�
u8 call_flag1=0;
/**********************************************************************
	����ҽ����ť1
**********************************************************************/
void EXTI10_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_PullResistorConfig(HT_GPIOE, GPIO_PIN_10, GPIO_PR_DOWN); //��������
	AFIO_GPEConfig(AFIO_PIN_10, AFIO_MODE_1);	  //ģʽ1.GPIO����
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_10, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_10, ENABLE);
	
	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_10;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;  //����ȥ����ʹ��
	EXTI_InitStruct.EXTI_DebounceCnt = 720000;           
	EXTI_InitStruct.EXTI_IntType = EXTI_BOTH_EDGE;		 //�����ء��½��ؾ���Ч
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_10, AFIO_ESS_PE);   //ͨ��10��GPIOE��
	EXTI_IntConfig(EXTI_CHANNEL_10, ENABLE);  						 //ͨ��10���ж�����
	NVIC_EnableIRQ(EXTI10_IRQn);													 //�ж�ʹ��
}
/**********************************************************************
	����ҽ����ť1
**********************************************************************/
void EXTI11_init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.EXTI       = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	
	GPIO_PullResistorConfig(HT_GPIOE, GPIO_PIN_11, GPIO_PR_DOWN); //��������
	AFIO_GPEConfig(AFIO_PIN_11, AFIO_MODE_1);	  //ģʽ1.GPIO����
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_11, GPIO_DIR_IN);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_11, ENABLE);
	
	EXTI_InitStruct.EXTI_Channel = EXTI_CHANNEL_11;
	EXTI_InitStruct.EXTI_Debounce = EXTI_DEBOUNCE_ENABLE;  //����ȥ����ʹ��
	EXTI_InitStruct.EXTI_DebounceCnt = 720000;           
	EXTI_InitStruct.EXTI_IntType = EXTI_BOTH_EDGE;		 //�����ء��½��ؾ���Ч
	EXTI_Init(&EXTI_InitStruct);
	AFIO_EXTISourceConfig(AFIO_EXTI_CH_11, AFIO_ESS_PE);   //ͨ��11��GPIOE��
	EXTI_IntConfig(EXTI_CHANNEL_11, ENABLE);  						 //ͨ��11���ж�����
	NVIC_EnableIRQ(EXTI11_IRQn);													 //�ж�ʹ��
}

/**********************************************************************
�ⲿ�ж�10������
**********************************************************************/
void EXTI10_IRQHandler(void)
{
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_10,EXTI_EDGE_POSITIVE)!=RESET) //������
    {
			call_flag0=1;
    }
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_10,EXTI_EDGE_NEGATIVE)!=RESET)
	   {
		  call_flag0=0;
     }
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_10);//����жϱ�־λ
}
/**********************************************************************
�ⲿ�ж�11������
**********************************************************************/
void EXTI11_IRQHandler(void)
{
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_11,EXTI_EDGE_POSITIVE)!=RESET) //������
    {
			call_flag1=1;
    }
	if(EXTI_GetEdgeStatus(EXTI_CHANNEL_11,EXTI_EDGE_NEGATIVE)!=RESET)
	   {
		  call_flag1=0;
     }
	EXTI_ClearEdgeFlag(EXTI_CHANNEL_11);//����жϱ�־λ
}

