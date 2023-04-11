#ifndef __HC05_H
#define __HC05_H
#include "ht32.h"
#include "ht32_board.h"

typedef struct
{
	char CardNum[8];			//0--8 	ID����
	char StudentNum[11];	//9--21  ѧ��
	char Oneselftime[2];  //22--24	�Զ���ʱ��
	char Region[1];				//25--26	Ԥ������
}DataSendTypeDef;

extern DataSendTypeDef DataSend;

void Translation(void);	//�������ݿ�
void Receive(void);				//�������ݴ�����

#endif





