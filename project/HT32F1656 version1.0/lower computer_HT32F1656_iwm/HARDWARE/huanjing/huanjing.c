#include "huanjing.h"
#include "delay.h"
#include "ebi_lcd.h"
#include "hc05.h"
#include "exti.h"
//extern u8 sensor1_flag;
//u8 LED_flag=0;//1234
//u8 beep_flag=0;
u8 MQ2_flag=0;
u8 ren=0;
 u8 sensor_flag;
//sensor D15   ����Ϊ�� ���  Ϊ��
//led  D10-13
//MQ2  A6  A1    ����Ϊ��
//BEEP  D14
void sensor_Init(void)		//�ⲿ�ж�
{
    CKCU_PeripClockConfig_TypeDef  CKCUClock = {{0}};
    CKCUClock.Bit.PD 					=1;
    CKCUClock.Bit.AFIO				=1;
    CKCUClock.Bit.BKP 				=1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PD, GPIO_PIN_15,  AFIO_MODE_DEFAULT);
    GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_15, GPIO_DIR_IN);
    GPIO_PullResistorConfig(HT_GPIOD, GPIO_PIN_15,GPIO_PR_DOWN);//����
    GPIO_InputConfig(HT_GPIOD, GPIO_PIN_15, ENABLE);
    GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_15);
}
u8  sensor_handle(void)
{
    if(GPIO_ReadInBit(HT_GPIOD,GPIO_PIN_15)==SET)
				sensor_flag=1;
    else
        sensor_flag= 0;
    return sensor_flag;
}
void LED_Init(void)
{
    CKCU_PeripClockConfig_TypeDef  CKCUClock = {{0}};
    CKCUClock.Bit.PD =1;
    CKCUClock.Bit.AFIO=1;
    CKCUClock.Bit.BKP        = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PD, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13,  AFIO_MODE_DEFAULT);
    //AFIO_GPDConfig( GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, AFIO_MODE_1);
    GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13, GPIO_DIR_OUT);
    GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);//����
}
//D14 -����
void BEEP_Init(void)
{
    CKCU_PeripClockConfig_TypeDef  CKCUClock = {{0}};
    CKCUClock.Bit.PD =1;
    CKCUClock.Bit.AFIO=1;
    CKCUClock.Bit.BKP        = 1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PD, GPIO_PIN_14,  AFIO_MODE_DEFAULT);
    GPIO_DirectionConfig(HT_GPIOD, GPIO_PIN_14, GPIO_DIR_OUT);
    //GPIO_InputConfig(HT_GPIOD, GPIO_PIN_14, ENABLE);
    GPIO_PullResistorConfig(HT_GPIOD, GPIO_PIN_14,GPIO_PR_DOWN);//������down//
    GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_14);//����
}
//A6--����
void MQ2_Init(void)
{
    CKCU_PeripClockConfig_TypeDef  CKCUClock = {{0}};
    CKCUClock.Bit.PA 					=1;
    CKCUClock.Bit.AFIO				=1;
    CKCUClock.Bit.BKP        	=1;
    CKCU_PeripClockConfig(CKCUClock, ENABLE);
    HT32F_DVB_GPxConfig(GPIO_PA, GPIO_PIN_6| GPIO_PIN_1,  AFIO_MODE_DEFAULT);
    AFIO_GPAConfig(AFIO_PIN_6| GPIO_PIN_1, AFIO_MODE_1);//AFIO_GPxConfig(GPIO_PA, AFIO_PIN_6, AFIO_MODE_DEFAULT);
    GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_6| GPIO_PIN_1, GPIO_DIR_IN);
    GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_6| GPIO_PIN_1,GPIO_PR_DOWN);
    GPIO_InputConfig(HT_GPIOA, GPIO_PIN_6| GPIO_PIN_1, ENABLE);
    GPIO_ClearOutBits(HT_GPIOA,GPIO_PIN_6| GPIO_PIN_1);
//         GPIO_PullResistorConfig(HT_GPIOA, GPIO_PIN_6,GPIO_PR_UP);
//       GPIO_InputConfig(HT_GPIOA, GPIO_PIN_6, ENABLE);
//       GPIO_ClearOutBits(HT_GPIOA,GPIO_PIN_6);

}

u8  MQ2_handle(void)
{
    if(GPIO_ReadInBit(HT_GPIOA,GPIO_PIN_6| GPIO_PIN_1)==SET)//
        MQ2_flag=1;//������ʱ
    else
        MQ2_flag= 0;
    return MQ2_flag;
}

void all_chuli(void)
{
    if(MQ2_handle()==1)  //�����쳣
    {
        GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_14);//����
        POINT_COLOR=RED;
        LCD_ShowString(10,180,200,16,16,"Leave The Class");
        LCD_ShowString(115,150,200,16,16,"fog is unnormal");
        delay_ms(1000);
        LCD_Fill(115,150,235,180,LGRAY);   //����Ч��

        POINT_COLOR=BLUE;
    }
    else
    {
        LCD_ShowString(115,150,200,16,16,"fog is normal ");
        GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_14);//�رձ���--
    }
//�Ʒ�����   ��ԤԼλ�����뵽�����У����ȶԣ�        ��Ҫ��RC522ˢ������
    if(DataSend.Region[0]==31&&(sensor_flag=1))//ASCLL
    {
        DataSend.Region[0] = 0;
         ren++;
         GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_10);//����1
         LCD_Fill(7,70,52,115,RED);
    }
    if(DataSend.Region[0]==32&&(sensor_flag=1))
    {  
        DataSend.Region[0] = 0;
        ren++;
        GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_11);//����2
        LCD_Fill(62,70,107,115,RED);
    }
    if(DataSend.Region[0]==33&&(sensor_flag=1))
    {   
        DataSend.Region[0] = 0;
        ren++;
        GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_12);//����3
        LCD_Fill(7,125,52,170,RED);

    }
    if(DataSend.Region[0]==34&&(sensor_flag=1))
    {   
        DataSend.Region[0] = 0;
        ren++;
        GPIO_SetOutBits(HT_GPIOD,GPIO_PIN_13);//����4
        LCD_Fill(62,125,107,170,RED);

    }
     if(DataSend.Region[0]==35&&(sensor_flag=1))
    {   
        DataSend.Region[0] = 0;
        ren--;
        GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_10);//�ر�����1
      LCD_Fill(7,70,52,115,BLUE);
    }
       if(DataSend.Region[0]==36&&(sensor_flag=1))
    {   
        DataSend.Region[0] = 0;
        ren--;
        GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_11);//�ر�����2
        LCD_Fill(62,70,107,115,BLUE);
    }

       if(DataSend.Region[0]==37&&(sensor_flag=1))
    {  
        DataSend.Region[0] = 0;
        ren--;
        GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_12);//�ر�����3
        LCD_Fill(7,125,52,170,BLUE);
    }

       if(DataSend.Region[0]==38&&(sensor_flag=1))
    {
        DataSend.Region[0] = 0;
        ren--;
        GPIO_ClearOutBits(HT_GPIOD,GPIO_PIN_13);//�ر�����4
        LCD_Fill(62,125,107,170,BLUE);    
    }


}
