#ifndef __SILENCHT_MATRIX_KEY_H
#define __SILENCHT_MATRIX_KEY_H
#include "sys.h"


extern u8 upper_monitor_flag; //B下位机标号

//函数声明
void matrix_keys_Init(void);
void matrix_keys_ANTI_Init(void);
u8   keys_scan(void);
void keys_send(u8 key);

#endif

