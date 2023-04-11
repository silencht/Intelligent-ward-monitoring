/* ---------------包含头文件--------------------------------------------*/
#include "delay.h"
#include "ht32.h"
#include "ht32_board.h"
#include "ebi_lcd.h"
#include "usart.h"
#include "max30102.h"
#include "algorithm.h"
#include "myiic.h"
#include "sht3x.h"
#include "ESP8266.h"
#include "wdt.h"
#include "timer.h"
#include "voice_light.h"
#include "exti.h"
/* ---------------下位机引脚配置说明----------------------------------------
   	 上下位机通信协议、工程细节详见DOC—>readme.txt
--------------------------------------------------------------------------*/

/* ---------------自定义参数宏---------------------------------------------*/
#define NVIC_PRIORITY_GROUP_3          3 /* 抢占优先级：Preemption: 4 bits / 响应优先级：Subpriority: 0 bits */

/*----------------------声明函数，自定义变量-----------------------------------------*/

			void Numerical_judgment(void)	;	
			
  /*--------MAX30102(心率血氧模块)	-------*/
			u8       raw_data[13]={0};
			uint16_t data_16[2]={0};
			uint32_t Lux;
			extern uint8_t uch_dummy;			
	/*--------sht30(温湿度模块)-------------*/
			ft     temperature=0;  // 温度
			ft     humidity=0;     // 湿度[%RH]
	/*--------各类数据范围判断位-------------*/
	    uint8_t    lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag;
			uint8_t    voice_flag=0;//语音播报模块变量
			uint8_t		 waring_flag=0;
			
/*------------------------------------------------------------------------------
	函数名：main主函数
	输入输出：无
	返回值：无
*------------------------------------------------------------------------------*/
int main(void)
{   			
  /*--------esp8266-wifi模块)-------------*/
			USART0_RX_STA = 0;	//串口0、1接收数据状态标志位（相关看定义处）
			USART1_RX_STA = 0;  
			
/*----------------------配置NVIC优先级------------------------------------------*/

	NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_3);		//中断优先级分组
	NVIC_SetPriority(USART0_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 1, 0));
	NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_PRIORITY_GROUP_3, 0, 0));

/*----------------------初始化函数----------------------------------------------*/

	usart0_Init(115200);			//串口0（WIFI客户端）初始化
	usart1_Init(9600);			  //串口1(体温模块)初始化
	delay_init();							//延时函数初始化
  I2C0_Init();						  //硬件IIC0初始化
	LED_Init();								//LED灯初始化
	BEEP_Init();							//蜂鸣器初始化
	VOICE_Init();							//语音播报初始化
	bsp_InitI2C_OUT();        //GPIO模拟IIC，输出功能函数初始化，如要读取管脚电平，需进行输入功能函数初始化
	LCD_Init();								//LCD初始化		
  EXTI10_init ();           //外部中断10（呼叫医生按钮1中断）初始化
  EXTI11_init ();           //外部中断11（呼叫医生按钮2中断）初始化
	HUMI_init();							//加湿器初始化
	SHT3X_StartPeriodicMeasurment(REPEATAB_HIGH, FREQUENCY_10HZ);//SHT30温湿度模块模式配置完成
	
	maxim_max30102_reset();   //重启 MAX30102	
  maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
  maxim_max30102_init();    //initialize the MAX30102
	max30102_int_line();      //中断线PD4读输入函数初始化
	
	WDT_Init();               //看门狗初始化
	delay_ms(1000);						//WDT亮灯1s
	BEEP_EN();								//开机启动声
	WDT_Restart();					  // Reload Counter as WDTV Value（喂狗函数）
/*--------------进入无限循环体--------------------------------------------*/	
	while(1)
	  
/*--------------病房主程序循环体代码---------------------------------------*/	
		{
		  /*-------光强度模块代码-----------------*/	
				close_wdt_led();			
				I2C_BufferRead(raw_data,0x00,4); 
				data_16[0]=(((uint16_t)raw_data[0])<<8)|raw_data[1];
				data_16[1]=(((uint16_t)raw_data[2])<<8)|raw_data[3];
				Lux=(((uint32_t)data_16[0])<<16)|data_16[1];
		  /*--------sht30(温湿度模块)-------------*/
			  restart:bsp_InitI2C_OUT();
			  while(SHT3X_ReadMeasurementBuffer(&temperature, &humidity));
        if(humidity!=100);
			  else goto restart;	//利用goto排模块数据组中的错误组，若数据错误，重新检测				
			/*--------MAX30102(心率血氧模块)	-------*/
					max30102_loop();
  				Numerical_judgment();
  				u0_printf ("[z]*%d*%.2f*%.2f*%.2f*%d*%d*%d*%d*%d*%d*%d*%d*%d$\r\n",(Lux/100),temperature,humidity,(body_temp/100),hrAvg,spo2Avg,
  				lux_flag,temperature_flag,humidity_flag,body_temp_flag,hrate_flag,spo2_flag,call_flag0|call_flag1);

			    WDT_Restart();				// 喂狗
		}
}





//判断各类环境、人体数据是否在某个范围内
void Numerical_judgment(void)
{
//----------------光照度----------------------------------------------
	if( (80<(Lux/100)) && ((Lux/100)<999) ){
			lux_flag =2;//正常
			LED1_DISEN();
			LED2_DISEN();
		}
	else if( (35<=(Lux/100)) && ((Lux/100)<=80) ){
			lux_flag =3;//较暗
			LED1_EN ();
			LED2_DISEN();
		}
	else if( ((Lux/100)<=35) ){
			lux_flag =4;//较暗
			LED1_EN ();
			LED2_EN ();
		}
	else {
		lux_flag =1;//很亮
		LED1_DISEN();
		LED2_DISEN();
		}
		
//----------------温度------------------------------------------------	
	if((18<temperature)&&(temperature<28))temperature_flag=2;//舒适
	else if(temperature<18)temperature_flag=3;//较低
	else temperature_flag=1;//较高
	
//----------------湿度------------------------------------------------	
	if((42<humidity)&&(humidity<68))humidity_flag=2;//舒适
	else if(humidity<42)humidity_flag=3;//干燥
	else humidity_flag=1;//潮湿
	
//----------------体温------------------------------------------------	
	if((35<(body_temp/100))&&((body_temp/100)<37.5))
		{
			body_temp_flag=2;//（正常体温）
		}
	else if((body_temp/100)<=35)
		{
			body_temp_flag=3;//可能未穿戴（不然就是人凉了）
			voice_flag++;
		}
	else if(38<(body_temp/100))
		{
			body_temp_flag=1;//体温较高
			waring_flag++;
			if(waring_flag==6)
				{
					BEEP_EN();
					waring_flag=0;
				}
		}
	else if( (37.5<(body_temp/100))	&& ((body_temp/100)<38) )
		{
			body_temp_flag=4;//（低烧）
		}
		
//----------------血氧/心率-------------------------------------------
	if(95<spo2Avg)spo2_flag=2;//正常
	else if(spo2Avg==0)spo2_flag=3;//没穿戴设备
	else 
		{
			spo2_flag=1;//可能有病
				waring_flag++;
				if(waring_flag==6)
					{
						BEEP_EN();
						waring_flag=0;
					}
		}
		
	if(spo2Avg!=0){
			if((58<hrAvg)&&(hrAvg<100)){
					hrate_flag=2;//正常
				}
			else if(hrAvg<58){
				hrate_flag=1;//心率较低
					waring_flag++;
					if(waring_flag==10)
						{
							BEEP_EN();
							waring_flag=0;
						}
				}
			else{
				hrate_flag=3;//心率较高
					waring_flag++;
					if(waring_flag==10)
						{
							BEEP_EN();
							waring_flag=0;
						}
				}
	}
	else{
		hrate_flag=4;//未穿戴设备
		voice_flag++;
	}
}
