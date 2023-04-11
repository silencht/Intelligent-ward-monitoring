#ifndef __RTC_H
#define __RTC_H
#include "ht32.h"
#include "ht32_board.h"

//ʱ��ṹ��
typedef struct 
{
	vu8 hour;
	vu8 min;
	vu8 sec;			
	//������������
	vu16 w_year;
	vu8  w_month;
	vu8  w_date;
	vu8  week;		 
}_calendar_obj;
extern _calendar_obj calendar;	//�����ṹ��

void RTC_Init(void);				//RTCʱ�ӳ�ʼ��
void RTC_Configuration(void);
u32 Time_Regulate(u16 syear,u8 smon,u8 sday,u32 Tmp_HH,u32 Tmp_MM, u32 Tmp_SS);
void Time_Display(void);
u8 Is_Leap_Year(u16 year);
void RTC_Set(u16 syear,u8 smon,u8 sday);
u8 RTC_Get(void);
u8 RTC_Get_Week(u16 year,u8 month,u8 day);

#endif

