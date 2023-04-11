#include "sys.h"
#include "silencht_matrix_key.h"
#include "delay.h"
#include "lcd.h"
#include "ESP8266.h"


u8 upper_monitor_flag; //B��λ�����


/***************************************************************************
	*�������ƣ�matrix_keys_Init
	*�������ܣ���ʼ���������
	*��ڲ�������
	*���ز�������
	*��ע��                         
****************************************************************************/
void matrix_keys_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructuress;
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);           							 //��GPIOFʱ��               
 GPIO_InitStructuress.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  //��ʼ��PF0-PF3
 GPIO_InitStructuress.GPIO_Mode = GPIO_Mode_OUT ;      												 //���ģʽ
 GPIO_InitStructuress.GPIO_OType = GPIO_OType_PP;         										 //����Ϊ�������
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;													 //100MHZ����
 GPIO_InitStructuress.GPIO_PuPd =GPIO_PuPd_UP;     														 //��������
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 																		 //GPIOF��ʼ������
 
 GPIO_InitStructuress.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;   //��ʼ��PF4-PF7
 GPIO_InitStructuress.GPIO_Mode =GPIO_Mode_OUT ;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd = GPIO_PuPd_DOWN ;														 //��������
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_SetBits( GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);						 //high 0-3
 GPIO_ResetBits( GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);					 //low  4-7
}
/***************************************************************************
	*�������ƣ�matrix_keys_ANTI_Init
	*�������ܣ�����ʼ��������̣������������Ź���
	*��ڲ�������
	*���ز�������
	*��ע��                         
****************************************************************************/
void matrix_keys_ANTI_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStructuress;   
 GPIO_InitStructuress.GPIO_Pin =GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;    
 GPIO_InitStructuress.GPIO_Mode = GPIO_Mode_OUT ;      
 GPIO_InitStructuress.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd =GPIO_PuPd_UP;    
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_InitStructuress.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;  
 GPIO_InitStructuress.GPIO_Mode =GPIO_Mode_OUT ;
 GPIO_InitStructuress.GPIO_Speed = GPIO_Speed_100MHz;
 GPIO_InitStructuress.GPIO_PuPd = GPIO_PuPd_DOWN ;
 GPIO_Init(GPIOF, &GPIO_InitStructuress); 
 
 GPIO_ResetBits( GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3); 							//low  0-3
 GPIO_SetBits( GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);								//high 4-7
}
/***************************************************************************
	*�������ƣ�keys_scan
	*�������ܣ����̰���ɨ��
	*��ڲ�������
	*���ز�����u8���ͼ�ֵ
	*��ע��                         
****************************************************************************/
u8 keys_scan(void)
{
	u8 a=0,b=0,t=0;	//�����С����ݴ����
	matrix_keys_Init(); //���̳�ʼ��
	if((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0) //�������ֵ��Ϊ0000 1111�����а������£�
		{
			delay_ms (20);	//��ʱ20���룬������
			if((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0) //�ٴμ�ⰴ���������
			{
				a = GPIO_ReadInputData(GPIOF)&0xff;			 //��ֵ�����ݴ����a
			}
			matrix_keys_ANTI_Init();//GPIO������ʽ��ת
			if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f) //�������ֵ��Ϊ1111 0000
				{
					if((GPIO_ReadInputData(GPIOF)&0xff)!=0x0f)
					{
						b = GPIO_ReadInputData(GPIOF)&0xff;			 //��ֵ�����ݴ����b
					}
				}
				a = (~a)|(~b);//�������������	
			switch(a)	    //�Ա�����ֵ
			{
				case 0xee: b = 16; break;		//�Աȵõ��ļ�ֵ��b��ֵ���
				case 0xed: b = 15; break;
				case 0xeb: b = 14; break;
				case 0xe7: b = 13; break;
				case 0xde: b = 12; break;
				case 0xdd: b = 11; break;
				case 0xdb: b = 10; break;
				case 0xd7: b = 9 ; break;
				case 0xbe: b = 8 ; break;
				case 0xbd: b = 7 ; break;
				case 0xbb: b = 6 ; break;
				case 0xb7: b = 5 ; break;
				case 0x7e: b = 4 ; break;   //4 �رշ���
				case 0x7d: b = 3 ; break;		//3 ��������																																																																																																																																																														
				case 0x7b: b = 2 ; break;		//2 �رռ�ʪ��
				case 0x77: b = 1 ; break;		//1 ������ʪ��
				default:   b = 0 ; break;   //��ֵ��������ֵ���0
			}
			while((GPIO_ReadInputData(GPIOF)&0xff)!=0xf0)//�ȴ������ɿ�,��ʱ�Զ��ɿ�
			{
				t++;
				if(t>10){ t=0;break;}			
			}
		}
  return (b);//���ؼ�ֵ���
} 
/***************************************************************************
	*�������ƣ�keys_send
	*�������ܣ�����ֵͨ��usart����
	*��ڲ�����key 
	*���ز�����null
	*��ע��                         
****************************************************************************/
void keys_send(u8 key)
{	

	switch(key)
		{
			case 1:	
					switch(upper_monitor_flag)
						{
						  case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("1\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("1\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u2_printf("1\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("1\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("1\r\n");break ;
							default: break ;													
						}						
        break;
			case 2:
						switch(upper_monitor_flag)
						{
						  case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("2\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("2\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50);	u2_printf("2\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("2\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("2\r\n");break ;
							default: break ;													
						}
				break;
			case 3:
						switch(upper_monitor_flag)
						{
						 		
							case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("3\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("3\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("3\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("3\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("3\r\n");break ;
							default: break ;											
						}
				break;
			case 4:
						switch(upper_monitor_flag)
						{
						 		
							case 0:atk_8266_send_cmd("AT+CIPSEND=0,3","OK",50); u2_printf("4\r\n");break ;
							case 1:atk_8266_send_cmd("AT+CIPSEND=1,3","OK",50); u2_printf("4\r\n");break ;
							case 2:atk_8266_send_cmd("AT+CIPSEND=2,3","OK",50); u2_printf("4\r\n");break ;
							case 3:atk_8266_send_cmd("AT+CIPSEND=3,3","OK",50); u2_printf("4\r\n");break ;
							case 4:atk_8266_send_cmd("AT+CIPSEND=4,3","OK",50); u2_printf("4\r\n");break ;
							default: break ;											
						}
				break;
			case 5:
				break;
			case 6:
				break;
			case 7:
				break;
			case 8:
				break;
			case 9:
				break;
			case 10:
				break;
			case 11:
				break;
			case 12:
				break;
			case 13:
				break;
			case 14:
				break;
			case 15:
				break;
			case 16:
				break;
			default:
				break;
		}
	}

