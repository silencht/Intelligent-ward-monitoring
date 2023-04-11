#include "rtc.h"

_calendar_obj calendar;		//时钟结构体

vu32 gwTimeDisplay = 0;

//月份数据表											 
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//平年的月份日期表
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};

void RTC_Init(void)
{
	/* Enable Backup Domain PCLK and waits until it can be accessed                                           */
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
  CKCUClock.Bit.BKP        = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);
  if (PWRCU_CheckReadyAccessed() != PWRCU_OK)
  {
    while (1);
  }

  /* Init LED1 and USART                                                                                    */
  HT32F_DVB_LEDInit(HT_LED1);

  RETARGET_Configuration();           /* Retarget Related configuration                                     */

  /* Enable NVIC RTC interrupt                                                                              */
  NVIC_EnableIRQ(RTC_IRQn);

  /* Check if the Power On Reset flag is set                                                                */
  if (PWRCU_GetFlagStatus() == PWRCU_FLAG_BAKPOR)
  {
    printf("\r\n\n Power On Reset occurred....");
  }

  if (PWRCU_ReadBackupRegister(PWRCU_BAKREG_0) != 0xA551)
  {
    u32 wInitTime = 0;
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    printf("\r\n\n RTC not yet configured....");

    /* RTC Configuration                                                                                    */
    RTC_Configuration();

    printf("\r\n RTC configured....");

    /* Adjust time by values entered by the user on the hyperterminal,
       Then store the init time to PWRCU_BAKREG_1. */
    wInitTime = Time_Regulate(2017,5,9,23,13,0);
    PWRCU_WriteBackupRegister(PWRCU_BAKREG_1, wInitTime);
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_2, calendar.w_year);
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_3, calendar.w_month);
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_4, calendar.w_date);

    /* Reset RTC Counter when Time is 23:59:59                                                              */
    RTC_SetCompare(86400 - wInitTime);

    PWRCU_WriteBackupRegister(PWRCU_BAKREG_0, 0xA551);
    /* Enable RTC                                                                                           */
    RTC_Cmd(ENABLE);
  }
  else
  {
    printf("\r\n No need to configure RTC....");
  }


	/* If 1s has passed                                                                                     */
	if (gwTimeDisplay == 1)
	{
		/* Display current time.
			 Current time is sum of the RTC counter value and the init time(stored in PWRCU_BAKREG_1 register).
			 The init time (PWRCU_BAKREG_1 register) will be clear if the RTC Match flag(CMFLAG) is set.
			 Refer to RTC_IRQHandler. */
//		Time_Display(RTC_GetCounter() + PWRCU_ReadBackupRegister(PWRCU_BAKREG_1));
		gwTimeDisplay = 0;
	}
}
/*********************************************************************************************************//**
 * @brief   This function handles RTC interrupt.
 * @retval  None
 * @details In RTC interrupt service routine:
 *  - Reset RTC init time when Time is 23:59:59.
 *    - Set RTC Compare register as 86400.
 *    - Reset PWRCU_BAKREG_1 to 0.
 *  - Toggle LED1 each 1s.
 *  - Set gwTimeDisplay to 1 to enable time update.
 *
 ************************************************************************************************************/
void RTC_IRQHandler(void)
{
  extern vu32 gwTimeDisplay;
  u8 bFlags;
	u16 year;
	u8 month, day;

  bFlags = RTC_GetFlagStatus();
  if((bFlags & 0x2) != 0x0) /* Match flag */
  {
    /* Reset RTC init time when Time is 23:59:59 */
    RTC_SetCompare(86400);
    PWRCU_WriteBackupRegister(PWRCU_BAKREG_1, 0);
		year = PWRCU_ReadBackupRegister(PWRCU_BAKREG_2);
		month = PWRCU_ReadBackupRegister(PWRCU_BAKREG_3);
		day = PWRCU_ReadBackupRegister(PWRCU_BAKREG_4);
		if(day<mon_table[month-1])
		{
			day++;
		}
		else if(day==mon_table[month-1]&&Is_Leap_Year(year)!=1&&month<12&&month!=2)
		{
			day = 1;month++;
		}
		else if(day==mon_table[month-1]&&month==12)
		{
			day = 1;month = 1;year++;
		}
		else if(month==2&&day==mon_table[1]&&Is_Leap_Year(year)==1)
		{
			day = 29;
		}
		else if(day==29&&month==2)
		{
			day = 1;month = 3;
		}
		
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_2,year);
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_3,month);
		PWRCU_WriteBackupRegister(PWRCU_BAKREG_4,day);
//		PWRCU_WriteBackupRegister(PWRCU_BAKREG_2,PWRCU_ReadBackupRegister(PWRCU_BAKREG_2)+1);
  }

  /* Toggle LED1 each 1s */
//  HT32F_DVB_LEDToggle(HT_LED1);

  /* Enable time update */
  gwTimeDisplay = 1;
}
/*********************************************************************************************************//**
 * @brief Configures the RTC.
 * @retval None
 * @details RTC configuration as following:
 *  - S/W reset backup domain.
 *  - Configure Low Speed External clock (LSE).
 *    - The LSE use fast startup mode.
 *      Shortly startup time but higher power consumption.
 *    - Enable the LSE.
 *    - Waits till LSE is ready.
 *  - Configure RTC.
 *    - Select LSE as RTC Clock Source.
 *    - Enable the RTC Second interrupt.
 *    - RTC prescaler = 32768 to generate 1 second interrupt.
 *    - Enable clear RTC counter by match function.
 *
 ************************************************************************************************************/
void RTC_Configuration(void)
{
  /* Reset Backup Domain                                                                                    */
  PWRCU_DeInit();

  /* Configure Low Speed External clock (LSE)                                                               */
  RTC_LSESMConfig(RTC_LSESM_FAST);
  RTC_LSECmd(ENABLE);
  while (CKCU_GetClockReadyStatus(CKCU_FLAG_LSERDY) == RESET);

  /* Configure the RTC                                                                                      */
  RTC_ClockSourceConfig(RTC_SRC_LSE);
  RTC_IntConfig(RTC_INT_CSEC, ENABLE);
  RTC_SetPrescaler(RTC_RPRE_32768);

  /* Restart counter when match event occurred                                                              */
  RTC_CMPCLRCmd(ENABLE);
}
/*********************************************************************************************************//**
  * @brief  Returns the time entered by user, using Hyperterminal.
  * @return Enter time, unit is second within a day.
  ***********************************************************************************************************/
u32 Time_Regulate(u16 syear,u8 smon,u8 sday,u32 hour,u32 min,u32 sec)
{
	calendar.w_year = syear;
	calendar.w_month = smon;
	calendar.w_date = sday;
  return((hour * 3600 + min * 60 + sec));
}

/*********************************************************************************************************//**
  * @brief  Displays the input time.
  * @param  wTimeVar: Displays time.
  * @retval None
  ***********************************************************************************************************/
void Time_Display(void)
{
	u32 wTimeVar = 0;
	wTimeVar = RTC_GetCounter() + PWRCU_ReadBackupRegister(PWRCU_BAKREG_1);
	
  /* Compute  hours   */
  calendar.hour = wTimeVar/3600;
  /* Compute minutes  */
  calendar.min = (wTimeVar % 3600)/60;
  /* Compute seconds  */
  calendar.sec = (wTimeVar % 3600)% 60;
	
	calendar.w_year = PWRCU_ReadBackupRegister(PWRCU_BAKREG_2);
	calendar.w_month = PWRCU_ReadBackupRegister(PWRCU_BAKREG_3);
	calendar.w_date = PWRCU_ReadBackupRegister(PWRCU_BAKREG_4);
	calendar.week = RTC_Get_Week(calendar.w_year,calendar.w_month,calendar.w_date);

//	printf("Date: %04u:%02u:%02u\r\n", (unsigned int)calendar.w_year, (unsigned int)calendar.w_month, (unsigned int)calendar.w_date);
//  printf("Time: %02u:%02u:%02u\r\n", (unsigned int)calendar.hour, (unsigned int)calendar.min, (unsigned int)calendar.sec);
}

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是.0,不是
u8 Is_Leap_Year(u16 year)
{
	if(year%4==0) //必须能被4整除
	{
		if(year%100==0)
		{
			if(year%400==0)return 1;//如果以00结尾,还要能被400整除 	   
			else return 0;
		}else return 1;
	}else return 0;
}



void RTC_Set(u16 syear,u8 smon,u8 sday)
{
	calendar.w_year = syear;
	calendar.w_month = smon;
	calendar.w_date = sday;
}

//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//输入参数：公历年月日
//返回值：星期号
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{
	u16 temp2;
	u8 yearH,yearL;
	
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7);
}
