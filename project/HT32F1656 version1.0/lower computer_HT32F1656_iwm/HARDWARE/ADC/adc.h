#ifndef __ADC_H
#define __ADC_H
#include "ht32.h"
#include "ht32_board.h"

void AdcInit(void);
u16 Get_Adc_Average(u8 ch,u8 times);
void adcxianshi(void);


#endif
