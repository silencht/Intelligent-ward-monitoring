#include "ESP8266.h"
#include "usart.h"

void ESP8266_Init(void)
{
  while(atk_8266_send_cmd("AT","OK",20)){ LCD_ShowString(15,85,200,16,16,"wifi is no online");	}	
	delay_ms(20);
	u1_printf("ATE0\r\n");    //�رջ���
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPMUX=1","OK",50)){ LCD_ShowString(15,85,200,16,16,"cipmux wait");	}			            //������λ�� ESP8266-WIFI����������ģʽ	
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPSERVER=1,8086","OK",50)){ LCD_ShowString(15,85,200,16,16,"tcpserver wait");	}			//������λ�� ESP8266-WIFI����TCP SERVER
	delay_ms(20);
	while(atk_8266_send_cmd("AT+CIPSTO=6400","OK",50)){ LCD_ShowString(15,85,200,16,16,"overtime wait");}	//������λ�� ESP8266-WIFI��ʱ����ģʽ
	delay_ms(20);
	POINT_COLOR=DARKBLUE;
	LCD_ShowString(15,80,200,16,16,"  esp8266 init successful");
	POINT_COLOR=BLUE;
  wifi_init_end_flag=1;	
}
//��ATK-ESP8266��������
//cmd:���͵������ַ���
//ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
//waittime:�ȴ�ʱ��(��λ:10ms)
//����ֵ:0,���ͳɹ�(�õ����ڴ���Ӧ����)
//       1,����ʧ��
uint8_t atk_8266_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
	uint8_t res=0; 
	USART1_RX_STA=0;
	u1_printf("%s\r\n",cmd);	//����1��������
	if(ack&&waittime)		//��Ҫ�ȴ�Ӧ��
	{
		while(--waittime)	//�ȴ�����ʱ
		{
			delay_ms(50);
			
			if(USART1_RX_STA&0X8000)//���յ��ڴ���Ӧ����
			{
				if(atk_8266_check_cmd(ack))
				{
				  //u0_printf("ack:%s",(uint8_t*)ack);
					break;//�õ���Ч���� 
				}
				USART1_RX_STA=0;
			} 
		}
		if(waittime==0)res=1; 
	}
	return res;
}

//ATK-ESP8266���������,�����յ���Ӧ��
//str:�ڴ���Ӧ����
//����ֵ:0,û�еõ��ڴ���Ӧ����
//    ����,�ڴ�Ӧ������λ��(str��λ��)
uint8_t atk_8266_check_cmd(uint8_t *str)
{
	
	uint8_t strx=1;
	if(USART1_RX_STA&0X8000)		//���յ�һ��������
	{ 
		USART1_RX_BUF[USART1_RX_STA&0X7FFF]=0;//��ӽ�����
		strx=strcmp((const char*)USART1_RX_BUF,(const char*)str);
	} 
	return  strx;
}
