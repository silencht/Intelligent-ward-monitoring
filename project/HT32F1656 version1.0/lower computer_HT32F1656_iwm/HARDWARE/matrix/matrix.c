#include "matrix.h"
#include "delay.h"
#include "usart.h"
#include "ebi_lcd.h"
#include "string.h"

#define look 100 //登录
#define back 101// 退出
#define reset 102 //重置
#define loop 103 //上一层
#define YES  104 //ok
#define CANCEL 105 //取消
#define card   106//打卡
//重量模式
#define LIGHT 1		//轻
#define HEAVY 2		//重
//全局变量
u8 anjian=0;
u16 Key_Num;
u8 KeyVal=0;

/*******************************************************************************************
函数名：matrix_KEY_Init
入口参数：无
出口参数：无
函数功能：初始化按键IO口
*******************************************************************************************/
void matrix_KEY_Init(void) 			//按键IO初始化函数
{
	CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};
	CKCUClock.Bit.PE         = 1;
	CKCUClock.Bit.AFIO       = 1;
	CKCUClock.Bit.BKP        = 1;
	CKCU_PeripClockConfig(CKCUClock, ENABLE);
	AFIO_GPxConfig(GPIO_PE, AFIO_PIN_8|AFIO_PIN_9|AFIO_PIN_10|AFIO_PIN_11, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_DIR_OUT);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, ENABLE);
	GPIO_SetOutBits(HT_GPIOE,GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11);
	AFIO_GPxConfig(GPIO_PE, AFIO_PIN_12|AFIO_PIN_13|AFIO_PIN_14|AFIO_PIN_15, AFIO_MODE_DEFAULT);
	GPIO_DirectionConfig(HT_GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_DIR_IN);
	GPIO_PullResistorConfig(HT_GPIOE,GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PR_UP);
	GPIO_InputConfig(HT_GPIOE, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, ENABLE);
	
}
/*******************************************************************************************
函数名：matrix_KEY_Scan
入口参数：无
出口参数：KeyVal
函数功能：得到键值（行列扫描）
*******************************************************************************************/
u8 matrix_KEY_Scan(void)
{
	u16 temp;
	anjian=0;
	HT_GPIOE->DOUTR&=0xf0ff;							//行输出0 0 0 0
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;						//读取列值
	if(temp==0xf000)									//无按下
		return 0;
	else
	{
		delay_ms(5);
		temp=HT_GPIOE->DINR;						
		temp&=0xf000;
		if(temp==0xf000)
		return 0;										//去抖
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfeff;							//行输出1 1 1 0
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&=0xf000;						//读列输入
	switch(temp)
	{
		case 0xe000:KeyVal=1;break;
		case 0xd000:KeyVal=5;break;
		case 0xb000:KeyVal=9;break;
		case 0x7000:KeyVal=13;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfdff;							//行输出1 1 0 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=2;break;
		case 0xd000:KeyVal=6;break;
		case 0xb000:KeyVal=10;break;
		case 0x7000:KeyVal=14;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xfbff;							//行输出1 0 1 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=3;break;
		case 0xd000:KeyVal=7;break;
		case 0xb000:KeyVal=11;break;
		case 0x7000:KeyVal=15;break;
	}
	HT_GPIOE->DOUTR|=0x0f00;
	HT_GPIOE->DOUTR&=0xf7ff;							//行输出0 1 1 1
	delay_ms(1);
	temp=(HT_GPIOE->DINR)&0xf000;
	switch(temp)
	{
		case 0xe000:KeyVal=4;break;
		case 0xd000:KeyVal=8;break;
		case 0xb000:KeyVal=12;break;
		case 0x7000:KeyVal=16;break;
	}
	return KeyVal;										//返回键值
}

void chushihua1(void)									//初始化显示
{
		 GPIO_SetOutBits(HT_GPIOA,GPIO_PIN_6);
		 ili9320_chuxianshi();                   		//LCD界面初始化显示
}

u8 key_vaul[12];
u8 key_mask[12];
static u8 i = 0;
static u8 flag = 0;
void key_get(void)
{
	u8 key;
	key = matrix_KEY_Scan();
	if(key&&flag==0)
	{
		flag = 1;
		switch(key)
		{
			case 1:
				key_vaul[i] = '1';
                key_mask[i] ='1';			
            break;
			case 2:
				key_vaul[i] = '2';
                key_mask[i] ='2';
				break;
			case 3:
				key_vaul[i] = '3';
                 key_mask[i] ='3';
				break;
//			case 4:
//				key_vaul[i] = '\0';
//                key_mask[i] ='0';
//				break;
			case 5:
				key_vaul[i] = '4';
                 key_mask[i] ='4';
				break;
			case 6:
				key_vaul[i] = '5';
                key_mask[i] ='5';
				break;
			case 7:
				key_vaul[i] = '6';
                 key_mask[i] ='6';
				break;
			case 8:
				key_vaul[i] = '3';
                key_mask[i] ='3';
				break;
			case 9:
				key_vaul[i] = '7';
                key_mask[i] ='7';
				break;
			case 10:
				key_vaul[i] = '8';
                key_mask[i] ='8';
				break;
			case 11:
				key_vaul[i] = '9';
             key_mask[i] ='9';
				break;
			case 12:
				key_vaul[i] = '3';
             key_mask[i] ='3';
				break;
//			case 13:
//				key_vaul[i] = '\0';
//               key_mask[i] ='\0';
//				break;
			case 14:
				key_vaul[i] = '0';
               key_mask[i] ='0';
				break;
			case 15:
				key_vaul[i] = '#';
                key_mask[i] ='#';
				break;
			case 16:
				key_vaul[i] = '3';
               key_mask[i] ='3';
				break;
			default:
				break;
		}
		i++;
		key_vaul[i] = '\0';
          key_mask[i] ='\0';
	}
	else if(key==0)
	{
		flag = 0;
	}
//	key_vaul[++i] = '\0';
}
//（一一对应关系）
//	
//1	5	9	13	|		1	 2	  3	     退格//左移
//2	6	10	14	|		4	 5	  6		 打卡//++
//3	7	11	15	|		7	 8	  9		 重置//--
//4	8	12	16	|	  确认 	 0   取消    登录
//*******************************************************/
u16 Num_Select(void)
{
	switch(KeyVal)
	{
		case 1:		Key_Num = 1;				break;
		case 2:		Key_Num = 4;				break;
		case 3:		Key_Num = 7;				break;
		case 4: 	Key_Num = YES;			    break;
		
		case 5:		Key_Num = 2;				break;
		case 6:		Key_Num = 5;				break;
		case 7:		Key_Num = 8;				break;
		case 8:		Key_Num = 0;				break;
		
		case 9:		Key_Num = 3;				break;	
	     case 10:	Key_Num = 6;				break;
		case 11:	Key_Num = 9;				break;
		case 12:	Key_Num = CANCEL;			break;
		
		case 13:	Key_Num = loop;		break;
		case 14:	Key_Num =card ;		break;
		case 15:	Key_Num =reset ;	break;
		case 16:	Key_Num = look;		break;		
	default:	break;
	}
	return Key_Num;
}
void write_sfm(char add,char date)//数字改变
{
	char shi,ge;
	shi=date/10;
	ge=date%10;
	LCD_ShowNum(124+add,110,shi,2,16);
    LCD_ShowNum(124+add,110,ge,2,16);
}
char table1[]="00:00";
void order_time(void)
{   
  u8 s1num,flag=0;
  char shi,fen;  
            
    if( Num_Select()==look)
    {               
//           BACK_COLOR=WHITE;  //背景色
//	        LCD_Fill(5,63,235,240,WHITE);
          LCD_ShowString(150,70,200,16,16,"Login");
          LCD_ShowString(120,90,200,16,16,"ID:");
          LCD_ShowString(120,130,200,16,16,"password:"); 
          LCD_ShowString(10,180,200,16,16,"reserve time:");
          LCD_ShowString(124,180,200,16,16,table1);
          LCD_ShowString(10,200,200,16,16,"where:");
         delay_ms(500); 
    }                   
          key_get();        
          LCD_ShowString(120,110,100,16,16,key_vaul);  //目前只能输一行 换行？
    
    
//     if( Num_Select()==YES)  外部中断？
//         flag=1;
//        delay_ms(); 
//     if(flag==1)
//     {
//          key_get();
//          LCD_ShowString(88,110,100,16,16,key_mask); 
//     }
        
     
//时间  位置 写？    
//     key_get();        
//     LCD_ShowString(65,130,200,16,16,key_vaul);

    
//    if(Num_Select()==loop)
//    {
//        s1num++;
//        while(Num_Select()==loop)   
//        {
//            if(s1num==1)
//			{
//				LCD_DrawPoint(124,110);        //光标时   
//			}
//                 if(s1num==2)
//			{
//				 LCD_DrawPoint(134,110);           //光标-分
//			}
//                 if(s1num==3)
//			{
//				 LCD_DrawPoint(0,0);       
//			}                 
//        }
//    }  //选取光标位置
//        if(s1num!=0)
//        {       			
//				if(Num_Select()==card)//++
//				{
//					while(Num_Select()==card);
//					if(s1num==1)
//					{
//						shi++;
//						if(shi==24)
//							shi=0;
//						write_sfm(4,shi);
//																		
//					}
//					if(s1num==2)
//					{
//						fen++;
//						if(fen==60)
//							fen=0;
//						write_sfm(7,fen);
//						
//					}
//            
//                }
//                 			
//				if(Num_Select()==reset)//++
//				{
//					while(Num_Select()==reset);
//					if(s1num==1)
//					{
//						shi--;
//						if(shi==-1)
//							shi=23;
//						write_sfm(4,shi);
//																		
//					}
//					if(s1num==2)
//					{
//						fen--;
//						if(fen==-1)
//							fen=59;
//						write_sfm(7,fen);
//						
//					}
//            
//                }
//                
//      }
    
  
}

