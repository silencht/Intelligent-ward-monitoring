#ifndef __hj_H
#define __hj_H
#include "ht32.h"
#include "ht32_board.h"

void LED_Init(void);   //��
void LED_handle(void);
void MQ2_Init(void);//����1��2
u8  MQ2_handle(void);
void BEEP_Init(void);//������
u8 BEEP_handle(void);
void sensor_Init(void);//����
u8  sensor_handle(void);
void all_chuli(void);

#endif
