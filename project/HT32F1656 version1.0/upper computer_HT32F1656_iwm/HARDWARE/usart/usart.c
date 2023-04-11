#include "usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"
#include "mlx90614.h"

//USARTx_RX_STA，接收状态：bit15，	 接收完成标志
//												bit14，	 接收到0x0d
//		    							  bit13~0，接收到的有效字节数目
vu16 USART0_RX_STA = 0;
vu16 USART1_RX_STA = 0;
vu8  wifi_init_end_flag = 0;//wifi初始化结束完成标志，初始化未完成为0，完成为1（用以判定串口中断服务函数中是否清除USART1_RX_STA）

u8 lux[4]={0},temp[5]={0},humi[5]={0},btemp[5]={0},hrate[4]={0},spo2[3]={0}; 		//上位机数据接收数组
u8 upper_monitor_flag='0';                      //下位机标号,病房：最高位为1，病人：最高位为0
u8 USART0_RX_BUF[USART0_MAX_RECV_LEN]; 			//串口0接收缓冲,最大USART0_MAX_RECV_LEN个字节.
u8 USART0_TX_BUF[USART0_MAX_SEND_LEN]; 			//串口0发送缓冲,最大USART0_MAX_SEND_LEN字节

u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 			//串口1接收缓冲,最大USART1_MAX_RECV_LEN个字节.
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			//串口1发送缓冲,最大USART1_MAX_SEND_LEN字节

/**************************
*串口0初始化 + 中断使能
*param：u32 bound 波特率
*返回值：无
**************************/
void usart0_Init(u32 bound)
{
    USART_InitTypeDef Usart0_InitStructurer;
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.USART0     = 1;
    CKCUClock.Bit.AFIO       = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    AFIO_GPAConfig(AFIO_PIN_2 | AFIO_PIN_3, AFIO_MODE_6);//时钟初始化，端口复用USART

    Usart0_InitStructurer.USART_BaudRate=bound;
    Usart0_InitStructurer.USART_Mode=USART_MODE_NORMAL;
    Usart0_InitStructurer.USART_Parity=USART_PARITY_NO ;
    Usart0_InitStructurer.USART_StopBits=USART_STOPBITS_1 ;
    Usart0_InitStructurer.USART_WordLength=USART_WORDLENGTH_8B;
    USART_Init (HT_USART0,&Usart0_InitStructurer);//USART初始化，波特率自行设置，无奇偶校验，8位数据位，停止位1

    USART_TxCmd(HT_USART0, ENABLE);//使能YX
    USART_RxCmd(HT_USART0, ENABLE);//使能RX

    USART_IntConfig(HT_USART0, USART_IER_RDAIE, ENABLE);    //中断使能
		
    NVIC_EnableIRQ(USART0_IRQn);  //开启外部中断
}

/**************************
*串口1初始化 + 中断使能
*param：u32 bound 波特率
*返回值：无
**************************/
void usart1_Init(u32 bound)
{
    USART_InitTypeDef Usart1_InitStructurer;
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.USART1     = 1;
    CKCUClock.Bit.AFIO       = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    AFIO_GPAConfig(AFIO_PIN_4 | AFIO_PIN_5, AFIO_MODE_6);

    Usart1_InitStructurer.USART_BaudRate=bound;
    Usart1_InitStructurer.USART_Mode=USART_MODE_NORMAL;
    Usart1_InitStructurer.USART_Parity=USART_PARITY_NO ;
    Usart1_InitStructurer.USART_StopBits=USART_STOPBITS_1 ;
    Usart1_InitStructurer.USART_WordLength=USART_WORDLENGTH_8B;
    USART_Init (HT_USART1,&Usart1_InitStructurer);

    USART_TxCmd(HT_USART1, ENABLE);
    USART_RxCmd(HT_USART1, ENABLE);

    USART_IntConfig(HT_USART1, USART_IER_RDAIE, ENABLE);

    NVIC_EnableIRQ(USART1_IRQn);  
}

/******************************************************
*串口1发送函数
*param:u8 data发送的字节
*返回值：无
******************************************************/
//串口1发送1字节
void USART1_SendChar(u8 data)
{
    while((HT_USART1->LSR &(1 << 6)) == 0);//等待发送器空
    HT_USART1->RBR = data;//发送一字节数据
}

/******************************************************
*串口0   printf 函数
*确保一次发送数据不超过USART0_MAX_SEND_LEN字节
******************************************************/
void u0_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART0_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART0_TX_BUF);		//此次发送数据的长度
    for(j=0; j<i; j++)							//循环发送数据
    {
        while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)==RESET); //循环发送,直到发送完毕
        USART_SendData(HT_USART0,USART0_TX_BUF[j]);
    }
}

/******************************************************
*串口1   printf 函数
*确保一次发送数据不超过USART1_MAX_SEND_LEN字节
******************************************************/
void u1_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART1_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART1_TX_BUF);		//此次发送数据的长度
    for(j=0; j<i; j++)							//循环发送数据
    {
        while(USART_GetLineStatus(HT_USART1,USART_LSR_TE)==RESET); //循环发送,直到发送完毕
        USART_SendData(HT_USART1,USART1_TX_BUF[j]);
    }
}
