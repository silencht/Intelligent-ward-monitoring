#include "adc.h"
#include "ht32.h"
#include "ht32_board.h"
#include "delay.h"
#include "ebi_lcd.h"

//MQ2�ɼ���ѹֵ
void AdcInit(void)
{
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{ 0 }};    //ʹ������ΪPA0
  CKCUClock.Bit.PA         = 1; 
  CKCUClock.Bit.AFIO       = 1;
  CKCUClock.Bit.ADC        = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  CKCU_SetADCPrescaler(CKCU_ADCPRE_DIV64);
  AFIO_GPAConfig(AFIO_PIN_0 , AFIO_MODE_2);
  ADC_RegularGroupConfig(HT_ADC, DISCONTINUOUS_MODE, 1, 1);
  ADC_RegularChannelConfig(HT_ADC, ADC_CH_0, 0, 0);
  ADC_RegularTrigConfig(HT_ADC, ADC_TRIG_SOFTWARE);
}

u16 Get_Adc(u8 ch)
{ 
	ADC_SoftwareStartConvCmd(HT_ADC, ENABLE);
	
		return ADC_GetConversionData(HT_ADC , ch);
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	vu16 adcvel=0;
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	adcvel=(vu16) ( temp_val / times );
	return adcvel;
}
//�õ�MQ2��ѹ��С			�ɼ�A0		ģ���ź�
//ʹ��D0		�����ź�
void adcxianshi(void)
{
	u16 adcvel=0;
	u8 m;
	float j,ADC_ConvertedValueLocal;
	adcvel=Get_Adc_Average(ADC_CH_0,10);
	ADC_ConvertedValueLocal =(float)adcvel/4096*3.3; // ��ȡת����ADֵ
	j=ADC_ConvertedValueLocal/3.3*100.0;
	m=j*10;
	m=m%10;
	LCD_ShowxNum(193,155,j/1,2,16,0);
	LCD_ShowString(209,155,200,16,16,".");
	LCD_ShowxNum(215,155,m,1,16,0);
}


