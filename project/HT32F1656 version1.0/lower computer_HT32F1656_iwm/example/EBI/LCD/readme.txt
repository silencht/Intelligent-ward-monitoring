	
	下位机模块硬件连接：
【声光报警部分】：
	  [两只LED灯]： 
		长脚5V供电
		短脚分别 连接HT32	PE6,PE7
	  [语音播报模块]： 
		5V供电，P-E脚上升沿有效
		P-E	引脚 连接HT32	PE4
	  [蜂鸣器警报模块]：
		5V供电，I/O脚低电平有效
		I/O引脚	连接带上拉电阻的5V供电引脚板，引脚板连接HT32	PE5

【ESP8266-WIFI模块】：
		5V供电，RXD：UART接收端, TXD：UART发送端
		RXD 引脚 连接HT32	TX  PA2 (串口0)
		TXD 引脚 连接HT32	RX  PA3 (串口0)

【GY39-44009光强度模块】：
		5V供电，CT：SCL时钟线, DR：SDA数据线
	    CT引脚 连接HT32	PB12 (SCL) (硬件IIC)
		DR引脚 连接HT32	PB13(SDA) (硬件IIC)
		
【SHT3X温湿度模块】：
		5V供电，SDA/SCL引脚与心率模块、HT32开发板PD3/2引脚一起分别对应连接到[IIC上拉电阻排针板] SDA/SCL端；此IIC不同于光强度模块，为GPIO模拟IIC。
[IIC上拉电阻排针板]：5V电源连接两个470kΩ上拉电阻的一端，两电阻另一端引出排针组，分别标示为SDA与SCL。	
		SDA引脚 连接HT32 	PD3 (模拟IIC)
		SCL 引脚 连接HT32	PD2 (模拟IIC)

【MAX30102心率模块】：
		5V供电，INT：中断输入端	
SDA引脚 连接HT32 	PD3 (模拟IIC)
		SCL 引脚 连接HT32	PD2 (模拟IIC)
		INT 引脚 连接HT32	PD4

【MLX90614体温模块】：
		5V供电，RX：UART接收端 ，TX：UART发送端
	    RX引脚 连接HT32  TX	PA4 (串口1)
		TX引脚 连接HT32  RX	PA5 (串口1)



【呼叫医生外部中断开关】：
		上升、下降沿有效
开关一端连接5V电源，另一端连接HT32 	PE10
		
【JLINK下载线】
JLINK与HT32板共地
GND (4/6/8)引脚 连接 HT32 	GND
SWCLK	(9)引脚 连接 HT32	PA12
SWDIO	(7)引脚 连接 HT32	PA13
     ______
	 |1	  2|
	 |3	  4|		
	 |5	  6|		
   __|7	  8|		
  |	9	 10|
  |_ 11	 12|
	 |13 14|
	 |15 16|
	 |17 18|
	 |19_20|



	 

	 
	 
	 	//单下位机通信格式规定如下：
	    //---[f]为病房下位机、[r]为病人下位机、[z]为单下位机、'*'为分隔符、'$'为数据停止符，前6个数据为相应环境、人体参数，
		//后6个为数据范围判断位，每个数据判断的档位，上位机接受之后翻译：
		
		/*光强度：1很亮（提示拉窗帘）	2正常	3略暗（开一个led）	4很暗（开两个led）
				  温度：1较高	2正常	3较低
				  湿度：1潮湿	2正常	3干燥
				  体温：1较高	2正常 3可能未穿戴 4低烧
				  血氧：1病人有问题 2如果为95朝上那么数据有效	 3等于0则未穿戴设备
						if（血氧判断为==1），那么心率	1较低	2正常	3较高
						else 4未穿戴设备
					    下位机if（心率血样数据持续为0|温度持续低于30度）提示病人穿戴设备；*/
				//+IPD,0,23:[z]lux*123*temp30*humi35*btemp*36*hrate75spo2*99$	（lux等不发送）
				
		 //下位机send:   [z]*1234*26*34*37*75*99*1*1*1*1*1*1$        上位机receive:+IPD,0,15:[z]*1234*26*34*37*75*99*1*1*1*1*1*1$ 
		 //上位机数据接收数组:u8  lux[4],temp[3],humi[3],btemp[2],hrate[4],spo2[3]; 
				
				
				
				