#include "usart.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"
#include "mlx90614.h"

//USARTx_RX_STA������״̬��bit15��	 ������ɱ�־
//												bit14��	 ���յ�0x0d
//		    							  bit13~0�����յ�����Ч�ֽ���Ŀ
vu16 USART0_RX_STA = 0;
vu16 USART1_RX_STA = 0;
vu8  wifi_init_end_flag = 0;//wifi��ʼ��������ɱ�־����ʼ��δ���Ϊ0�����Ϊ1�������ж������жϷ��������Ƿ����USART1_RX_STA��

u8 lux[4]={0},temp[5]={0},humi[5]={0},btemp[5]={0},hrate[4]={0},spo2[3]={0}; 		//��λ�����ݽ�������
u8 upper_monitor_flag='0';                      //��λ�����,���������λΪ1�����ˣ����λΪ0
u8 USART0_RX_BUF[USART0_MAX_RECV_LEN]; 			//����0���ջ���,���USART0_MAX_RECV_LEN���ֽ�.
u8 USART0_TX_BUF[USART0_MAX_SEND_LEN]; 			//����0���ͻ���,���USART0_MAX_SEND_LEN�ֽ�

u8 USART1_RX_BUF[USART1_MAX_RECV_LEN]; 			//����1���ջ���,���USART1_MAX_RECV_LEN���ֽ�.
u8 USART1_TX_BUF[USART1_MAX_SEND_LEN]; 			//����1���ͻ���,���USART1_MAX_SEND_LEN�ֽ�

/**************************
*����0��ʼ�� + �ж�ʹ��
*param��u32 bound ������
*����ֵ����
**************************/
void usart0_Init(u32 bound)
{
    USART_InitTypeDef Usart0_InitStructurer;
    CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
    CKCUClock.Bit.USART0     = 1;
    CKCUClock.Bit.AFIO       = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    AFIO_GPAConfig(AFIO_PIN_2 | AFIO_PIN_3, AFIO_MODE_6);//ʱ�ӳ�ʼ�����˿ڸ���USART

    Usart0_InitStructurer.USART_BaudRate=bound;
    Usart0_InitStructurer.USART_Mode=USART_MODE_NORMAL;
    Usart0_InitStructurer.USART_Parity=USART_PARITY_NO ;
    Usart0_InitStructurer.USART_StopBits=USART_STOPBITS_1 ;
    Usart0_InitStructurer.USART_WordLength=USART_WORDLENGTH_8B;
    USART_Init (HT_USART0,&Usart0_InitStructurer);//USART��ʼ�����������������ã�����żУ�飬8λ����λ��ֹͣλ1

    USART_TxCmd(HT_USART0, ENABLE);//ʹ��YX
    USART_RxCmd(HT_USART0, ENABLE);//ʹ��RX

    USART_IntConfig(HT_USART0, USART_IER_RDAIE, ENABLE);    //�ж�ʹ��
		
    NVIC_EnableIRQ(USART0_IRQn);  //�����ⲿ�ж�
}

/**************************
*����1��ʼ�� + �ж�ʹ��
*param��u32 bound ������
*����ֵ����
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
*����1���ͺ���
*param:u8 data���͵��ֽ�
*����ֵ����
******************************************************/
//����1����1�ֽ�
void USART1_SendChar(u8 data)
{
    while((HT_USART1->LSR &(1 << 6)) == 0);//�ȴ���������
    HT_USART1->RBR = data;//����һ�ֽ�����
}

/******************************************************
*����0   printf ����
*ȷ��һ�η������ݲ�����USART0_MAX_SEND_LEN�ֽ�
******************************************************/
void u0_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART0_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART0_TX_BUF);		//�˴η������ݵĳ���
    for(j=0; j<i; j++)							//ѭ����������
    {
        while(USART_GetLineStatus(HT_USART0,USART_LSR_TE)==RESET); //ѭ������,ֱ���������
        USART_SendData(HT_USART0,USART0_TX_BUF[j]);
    }
}

/******************************************************
*����1   printf ����
*ȷ��һ�η������ݲ�����USART1_MAX_SEND_LEN�ֽ�
******************************************************/
void u1_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART1_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART1_TX_BUF);		//�˴η������ݵĳ���
    for(j=0; j<i; j++)							//ѭ����������
    {
        while(USART_GetLineStatus(HT_USART1,USART_LSR_TE)==RESET); //ѭ������,ֱ���������
        USART_SendData(HT_USART1,USART1_TX_BUF[j]);
    }
}
