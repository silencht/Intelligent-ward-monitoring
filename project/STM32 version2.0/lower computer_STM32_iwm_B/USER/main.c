#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "usart.h"
#include "IIC.h"
#include "max30102.h" 	
#include "sht3x.h"
#include "wdg.h"
#include "exti.h"
	
//各类数据范围判断位
	u8   			lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag;
//函数声明
	void Numerical_judgment(void)	;	
	
//主程序
 int main(void)
 {	
	delay_init();	    	   //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	usart1_init(115200);	 //测试用串口1
	uart4_init(9600);     //体温模块用串口4
	u4_printf("%c%c%c%c",102,102,2,86); //查询输出温度值0x66 0x66 0x02 0x56
	usart3_init(115200);	 //wifi模块用串口3
 	LED_Init ();			     //LED端口初始化
  max30102_init();       //心率血氧模块初始化
	Humi_Wind_Init();			 //加湿器/风扇初始化
  Voice_Init();					 //语音播报初始化
	Beep_Init();					 //蜂鸣器初始化	
	EXTIX_Init();					 //外部中断初始化
	IWDG_Init(7,1953);      //溢出时间约为25s		
	while(1) 
	{		 
			//计算心率和血氧饱和度
 	    calcu_heart_spo2();		
			//查询体温
			u4_printf("%c%c%c%c",102,102,2,86);
			delay_ms(100);			
			Numerical_judgment();
		u1_printf("[B]*%.2f*%d*%d*%d*%d*%d*%d$\r\n",(body_temp/100),hrAvg,spo2Avg,body_temp_flag,hrate_flag,spo2_flag,call_flag);
//			printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
//    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
		 	u3_printf("[B]*%.2f*%d*%d*%d*%d*%d*%d$!#",(body_temp/100),hrAvg,spo2Avg,body_temp_flag,hrate_flag,spo2_flag,call_flag);
			IWDG_Feed();
				
		}
			 
 } 
 

//											  光  温  湿 体 心 氧 光温湿体心氧 救                                     光 温  湿 体 心 氧 光温湿体心氧 救
	 //下位机send:   [A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$        上位机receive:+IPD,0,15:[A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$ 
	 //上位机send:    1 开启加湿器 2关闭加湿器 3开启风扇 4关闭风扇 5提醒病人穿戴设备
//判断各类环境、人体数据是否在某个范围内
void Numerical_judgment(void)
{
//----------------体温------------------------------------------------	
	if((35<(body_temp/100))&&((body_temp/100)<=37.5))
		{
			body_temp_flag=2;//（正常体温）
		}
	else if((body_temp/100)<=35)
		{
			body_temp_flag=3;//可能未穿戴（不然就是人凉了）
		}
	else if(38<(body_temp/100))
		{
			body_temp_flag=1;//体温较高
		}
	else if( (37.5<(body_temp/100))	&& ((body_temp/100)<38) )
		{
			body_temp_flag=4;//（低烧）
		}	
//----------------血氧/心率-------------------------------------------
	if(93<spo2Avg)spo2_flag=2;//正常
	else if(spo2Avg==0)spo2_flag=3;//没穿戴设备
	else spo2_flag=1;//可能有病
		
	if(spo2Avg!=0){
			if((58<hrAvg)&&(hrAvg<100))hrate_flag=2;//正常
			else if(hrAvg<58)hrate_flag=1;//心率较低
			else	hrate_flag=3;//心率较高
		}
	else hrate_flag=4;//未穿戴设备
}

