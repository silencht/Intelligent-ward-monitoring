#ifndef  __EXTI_H
#define  __EXTI_H
#include "ht32.h"
#include "ht32_board.h" 


extern u8 call_flag0;                    //呼叫医生按钮标志位
extern u8 call_flag1;
void EXTI10_init(void);
void EXTI11_init(void);
#endif
