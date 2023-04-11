#ifndef __HC05_H
#define __HC05_H
#include "ht32.h"
#include "ht32_board.h"

typedef struct
{
	char CardNum[8];			//0--8 	ID卡号
	char StudentNum[11];	//9--21  学号
	char Oneselftime[2];  //22--24	自定义时长
	char Region[1];				//25--26	预定区域
}DataSendTypeDef;

extern DataSendTypeDef DataSend;

void Translation(void);	//发送数据块
void Receive(void);				//接收数据处理函数

#endif





