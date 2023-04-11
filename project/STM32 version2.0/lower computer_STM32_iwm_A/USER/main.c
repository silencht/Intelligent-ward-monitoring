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
	
//光强模块所用变量
	u8  raw_data[13]={0};
	uint16_t data_16[2]={0};
	uint32_t Lux,Lux_last;
//温湿度模块所用变量
etError   error;        // 错误标志
u32t      serialNumber; // 序列号
float     temperature;  // 温度
float     humidity;     // 湿度[%RH]
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
	I2C_GPIO_Config();	   //光强度模块引脚初始化
  max30102_init();       //心率血氧模块初始化
	EXTIX_Init();					 //外部中断初始化
//	Humi_Wind_Init();			 //加湿器/风扇初始化
//  Voice_Init();					 //语音播报初始化
//	Beep_Init();					 //蜂鸣器初始化
	Light4_Init();				 //室内照明*4Led初始化
  SHT3X_Init(0x44); 		 // Address: 0x44 = Sensor on EvalBoard connector         0x45 = Sensor on EvalBoard	
	error = SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_1HZ);	// 开始定期测量-->高重复性和每秒1次测量												
	IWDG_Init(7,1953);      //溢出时间约为25s		
	while(1) 
	{		 
			//计算光强度
			one:
			Lux_last=Lux;
			delay_ms(10);
			Single_ReadI2C(0xb6,0x00,raw_data,4);
			data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];//前高八位左移8，前低八位不动，或一起，幅值给data_16[0]
			data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];//后高八位左移8，后低八位不动，或一起，幅值给data_16[1]
			Lux=(((uint32_t)data_16[0])<<16)|data_16[1];			  //前十六位左移16，后十六位不动，或一起，幅值给Lux
			if(Lux==0 || Lux==Lux_last) goto one;		
//			u3_printf ("[A]*%d\r\n",Lux);
			//计算温/湿度
			two:
   		SHT3X_ReadMeasurementBuffer(&temperature, &humidity);// 读取测量缓冲区，获取温湿度的数据
			if(temperature==0) goto two;
//			u1_printf ("[A]*%.2f,%.2f$\r\n",temperature,humidity);
//			//计算心率和血氧饱和度
 	    calcu_heart_spo2();		
 //  		u1_printf ("[A]*%d,%d$\r\n",hrAvg,spo2Avg);
 //			//查询体温
			u4_printf("%c%c%c%c",102,102,2,86); //串口二查询输出温度值0x66 0x66 0x02 0x56 
			 Numerical_judgment();

//			printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
//    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
		 	u3_printf("[A]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
    				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag);
			IWDG_Feed();
				
		}
			 
 } 
 
//判断各类环境、人体数据是否在某个范围内


	//单下位机通信格式规定如下：
	//后6个为数据范围判断位，每个数据判断的档位，上位机接受之后翻译：
	
	/*光强度：  [1]很亮  [2]正常	[3] dark1	[4] dark2  [5]dark3  [6]dark4
			  温度：[1]较高 open wind[2]正常	[3]较低
			  湿度：[1]潮湿	[2]正常	[3]干燥 open humi
			  体温：[1]较高	[2]正常	[3]未穿戴 [4]低烧
			  血氧：[1]病人有问题 [2]如果为94朝上那么数据有效	 [3]等于0则未穿戴设备
					if（血氧判断为==1）
						那么心率:[1]较低	[2]正常	[3]较高
					else [4]未穿戴设备
					
						 光  温  湿 体 心 氧 光温湿体心氧 救                                     光 温  湿 体 心 氧 光温湿体心氧 救
	 //下位机send:   [A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$        上位机receive:+IPD,0,15:[A]*1234*26*34*37*75*99*1*1*1*1*1*1*1$ 
	 //上位机send:    1 开启加湿器 2关闭加湿器 3开启风扇 4关闭风扇 5提醒病人穿戴设备
	 */
void Numerical_judgment(void)
{
//----------------光照度----------------------------------------------
	if( (100<(Lux/100)) && ((Lux/100)<999) ){
			lux_flag =2;//正常
			En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);					
		}
	else if( (75<(Lux/100)) && ((Lux/100)<=100) ){
			lux_flag =3;//dark1
			if(close_led){En_Ledx(1,SET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
			else En_Ledx(1,RESET);
		}
	else if( (50<(Lux/100)) &&((Lux/100)<=75) ){
			lux_flag =4;//dark2
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);}
		}
	else if((25<(Lux/100)) &&((Lux/100)<=50)){
			lux_flag =5;//dark3
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,SET);En_Ledx(4,RESET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);}
		}
	else if(((Lux/100)<=25)){
			lux_flag =6;//dark4
		  if(close_led){En_Ledx(1,SET);En_Ledx(2,SET);En_Ledx(3,SET);En_Ledx(4,SET);}
			else {En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);}
	  }
	else{
			lux_flag =1;//very light
			En_Ledx(1,RESET);En_Ledx(2,RESET);En_Ledx(3,RESET);En_Ledx(4,RESET);
		}
		
//----------------温度------------------------------------------------	
	if((18<temperature)&&(temperature<28))temperature_flag=2;//舒适
	else if(temperature<18)temperature_flag=3;//较低
	else temperature_flag=1; //较高
	
//----------------湿度------------------------------------------------	
	if((42<humidity)&&(humidity<68))humidity_flag=2;//舒适
	else if(humidity<42)humidity_flag=3;//干燥
	else humidity_flag=1;//潮湿
	
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

