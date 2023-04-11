#include "BEEP.h" 
#include "delay.h"
/*-------------------------------------------------------------------------
	���ܣ�����GPIO��
	���ߣ������	
	ʱ�䣺2018/6/7(version1.0)
	�޸�˵����
--------------------------------------------------------------------------*/

/***************************************************************************
	*�������ƣ�GPIO��ʼ������
	*�������ܣ���ʼ��GPIO��
	*��ڲ�������
	*���ز�������
	*��ע��
		GPIO_InitStructure����˵����
		GPIO_Pin��  GPIO_Pin_x ��x can be 0~15
		GPIO_Mode�� GPIO_Mode_IN ���� GPIO_Mode_OUT ��� GPIO_Mode_AF �˿ڸ��� GPIO_Mode_AN ģ��
		GPIO_OType��GPIO_OType_PP ���������GPIO_OType_OD ��©���
		GPIO_Speed��GPIO_Speed_2MHz��GPIO_Speed_25MHz��GPIO_Speed_50MHz��GPIO_Speed_100MHz
		GPIO_PuPd�� GPIO_PuPd_NOPULL �������� GPIO_PuPd_UP ���� GPIO_PuPd_DOWN ����
                                  
****************************************************************************/
void BEEP_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;                //����GPIO���ýṹ��

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//ʹ��GPIOFʱ��

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;						 //����GPIOF8
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        //��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;       //��©���
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   //�ٶ�100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;         //��������
  GPIO_Init(GPIOF, &GPIO_InitStructure);               //��ʼ����������GPIOF�ڽ��г�ʼ��
	
  GPIO_SetBits(GPIOF,GPIO_Pin_8);        							 //GPIOF8���ø�
//GPIO_ResetBits(GPIOF,GPIO_Pin_9 | GPIO_Pin_10);      //GPIOF9,F10���õ�
//GPIO_WriteBit(GPIOF,GPIO_Pin_9 | GPIO_Pin_10,Bit_RESET);//GPIOF9,F10���õͣ�����Bit_SET����
}

//����������
void BEEP_long_EN(void )
{
 GPIO_ResetBits(GPIOF,GPIO_Pin_8);        							 //GPIOF8���õ�
 delay_ms(1500);
 BEEP_DISEN();
}
//���ߵ�ƽ��������ʹ��
void BEEP_DISEN(void)
{
 GPIO_SetBits(GPIOF,GPIO_Pin_8);                         //GPIOF8���ø�
}

