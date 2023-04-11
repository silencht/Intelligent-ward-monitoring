#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ht32f1655_56_gpio.h"
void LED_Init(void);			//PE6/7
void LED1_EN(void);
void LED2_EN(void);
void LED1_DISEN(void);
void LED2_DISEN(void);

void VOICE_EN(void);			//PE4
void VOICE_Init(void);
void VOICE_DISEN(void);

void BEEP_Init(void);       //PE5
void BEEP_EN(void);
void BEEP_DISEN(void);

void HUMI_init(void);   //PE3
void HUMI_EN(void);   
void HUMI_DISEN(void);
