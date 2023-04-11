#include "sys.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 	  
 
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

u8  wifi_init_end_flag = 0;//wifi初始化结束完成标志，初始化未完成为0，完成为1（用以判定串口中断服务函数中是否清除USART1_RX_STA）
float body_temp=0;	
/***************************************************************************
	*功能：定义变量
	*备注：                
****************************************************************************/  
   	
u8 USART1_RX_BUF[USART1_REC_LEN];  //接收缓冲,最大USART1_REC_LEN个字节.
u8 UART4_RX_BUF[UART4_REC_LEN];  //接收缓冲,最大UART4_REC_LEN个字节. 
u8 USART3_RX_BUF[USART3_REC_LEN];  //接收缓冲,最大UART4_REC_LEN个字节.
//u8 UART4_RX_BUF[UART4_REC_LEN];    //接收缓冲,最大UART4_REC_LEN个字节.


u16 USART1_RX_STA=0; 			//串口1接收状态标记	
u16 UART4_RX_STA=0; 			//串口2接收状态标记	
u16 USART3_RX_STA=0; 			//串口2接收状态标记
//u16 UART4_RX_STA=0;       //串口4接收状态标记	

/***************************************************************************
	*函数名称：usart1_init
	*函数功能：初始化IO 串口1
	*入口参数：bound:波特率
	*返回参数：无
	*备注： GPIOA9——TX   GPIOA10——RX                     
****************************************************************************/
void usart1_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
  //USART1_RX	  GPIOA.10初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART1, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART1, ENABLE);                    //使能串口1 
	
}
/***************************************************************************
	*函数名称：uart4_init
	*函数功能：初始化IO 串口4
	*入口参数：bound:波特率
	*返回参数：无
	*备注： GPIOA2——TX   GPIOA3——RX                     
****************************************************************************/
void uart4_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE); //使能USART2时钟
  
	//USART2_TX   GPIOA.2
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PA.2
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART2_RX	  GPIOA.3初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOA.3 

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn; //开启串口2的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(UART4, &USART_InitStructure); //初始化串口4
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(UART4, ENABLE);                    //使能串口4
	
}
/***************************************************************************
	*函数名称：usart3_init
	*函数功能：初始化IO 串口3
	*入口参数：bound:波特率
	*返回参数：无
	*备注： GPIOB10——TX   GPIOB11——RX                     
****************************************************************************/
void usart3_init(u32 bound){
  //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE); //使能USART3时钟
  
	//USART3_TX   GPIOB.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.2
   
  //USART3_RX	  GPIOB.11初始化
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.3 

  //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //开启串口2的中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口2
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口2
	
}


/***************************************************************************
	*函数名称：u1_printf
	*函数功能：串口1打印函数
	*入口参数：参考标准C printf函数
	*返回参数：无
	*备注：                   
****************************************************************************/
void u1_printf (char *fmt, ...) 
{ 
	char buffer[200+1];  // 发送缓冲数组长度自己定义 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 200+1, fmt, arg_ptr); 
	while ((i < 200) && buffer[i]) 
		{ 
		  USART1->SR; //防止首字符丢失
			USART_SendData(USART1, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*函数名称：u4_printf
	*函数功能：串口2打印函数
	*入口参数：参考标准C printf函数
	*返回参数：无
	*备注：                   
****************************************************************************/
void u4_printf (char *fmt, ...) 
{ 
	char buffer[50+1];  // 发送缓冲数组长度自己定义 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 50+1, fmt, arg_ptr); 
	while ((i < 50) && buffer[i]) 
		{ 
		  UART4->SR; //防止首字符丢失
			USART_SendData(UART4, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}
/***************************************************************************
	*函数名称：u4_printf
	*函数功能：串口2打印函数
	*入口参数：参考标准C printf函数
	*返回参数：无
	*备注：                   
****************************************************************************/
void u3_printf (char *fmt, ...) 
{ 
	char buffer[200+1];  // 发送缓冲数组长度自己定义 
	u8 i = 0; 
	va_list arg_ptr; 
	va_start(arg_ptr, fmt);   
	vsnprintf(buffer, 200+1, fmt, arg_ptr); 
	while ((i < 200) && buffer[i]) 
		{ 
		  USART3->SR; //防止首字符丢失
			USART_SendData(USART3, (u8) buffer[i++]); 
			while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);  
		} 
	va_end(arg_ptr); 
}



 



