/*********************************************************************************************************//**
 * @file    SPI/FIFO_SEL_Hardware/main.c
 * @version $Rev:: 929          $
 * @date    $Date:: 2015-09-16 #$
 * @brief   Main program.
 *************************************************************************************************************
 * @attention
 *
 * Firmware Disclaimer Information
 *
 * 1. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, which is supplied by Holtek Semiconductor Inc., (hereinafter referred to as "HOLTEK") is the
 *    proprietary and confidential intellectual property of HOLTEK, and is protected by copyright law and
 *    other intellectual property laws.
 *
 * 2. The customer hereby acknowledges and agrees that the program technical documentation, including the
 *    code, is confidential information belonging to HOLTEK, and must not be disclosed to any third parties
 *    other than HOLTEK and the customer.
 *
 * 3. The program technical documentation, including the code, is provided "as is" and for customer reference
 *    only. After delivery by HOLTEK, the customer shall use the program technical documentation, including
 *    the code, at their own risk. HOLTEK disclaims any expressed, implied or statutory warranties, including
 *    the warranties of merchantability, satisfactory quality and fitness for a particular purpose.
 *
 * <h2><center>Copyright (C) 2014 Holtek Semiconductor Inc. All rights reserved</center></h2>
 ************************************************************************************************************/

/* Includes ------------------------------------------------------------------------------------------------*/
#include "spi.h"
#include "ebi_lcd.h"
#include "string.h"
#include "delay.h"
#include "usart.h"

unsigned char CT[2];			//卡类型
unsigned char SN[4]; 			//卡号
unsigned char ST[4];

/***********************
*连线说明：
*1--SS  <----->PA7
*2--SCK <----->PA4
*3--MOSI<----->PA5
*4--MISO<----->PA6
*5--悬空
*6--GND <----->GND
*7--RST <----->PC6
*8--VCC <----->VCC
***********************/

void SPI0_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
  CKCU_PeripClockConfig_TypeDef CKCUClock = {{0}};

  /* Enable NVIC SPI0 & SPI1 interrupt*/
//  NVIC_EnableIRQ(SPI0_IRQn);
//  NVIC_EnableIRQ(SPI1_IRQn);

  /* Enable PA, SPI0, SPI1 & AFIO clock*/
	CKCUClock.Bit.PA	 = 1;
	CKCUClock.Bit.PC	 = 1;
  CKCUClock.Bit.SPI0 = 1;
  CKCUClock.Bit.AFIO = 1;
  CKCU_PeripClockConfig(CKCUClock, ENABLE);

  /* SPI0_SEL idle state is HIGH*/
  GPIO_SetOutBits(HT_GPIOA, GPIO_PIN_7);
  GPIO_DirectionConfig(HT_GPIOA, GPIO_PIN_7, GPIO_DIR_OUT);

  /* Configure related IO to SPI0 mode*/
  AFIO_GPAConfig(AFIO_PIN_4 | AFIO_PIN_5 | AFIO_PIN_6|AFIO_PIN_7, AFIO_MODE_5);
	
	/* Configure rc522 RST IO Mode*/
	GPIO_PullResistorConfig(HT_GPIOC, GPIO_PIN_4, GPIO_PR_DOWN);	
	AFIO_GPCConfig(AFIO_PIN_4, AFIO_MODE_1);
	GPIO_DirectionConfig(HT_GPIOC, GPIO_PIN_4, GPIO_DIR_OUT);	
	
  /* Configure related IO to SPI1 mode*/
//  AFIO_GPDConfig(AFIO_PIN_13 | AFIO_PIN_14 | AFIO_PIN_15, AFIO_MODE_5);
//  AFIO_GPEConfig(AFIO_PIN_0, AFIO_MODE_5);

  /* SPI0 configuration: Master mode                                                                        */
  SPI_InitStructure.SPI_Mode = SPI_MASTER;
  SPI_InitStructure.SPI_FIFO = SPI_FIFO_DISABLE;
  SPI_InitStructure.SPI_DataLength = SPI_DATALENGTH_8;
  SPI_InitStructure.SPI_SELMode = SPI_SEL_SOFTWARE;
  SPI_InitStructure.SPI_SELPolarity = SPI_SELPOLARITY_LOW;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_LOW;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_FIRST;
  SPI_InitStructure.SPI_FirstBit = SPI_FIRSTBIT_MSB;
  SPI_InitStructure.SPI_RxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_TxFIFOTriggerLevel = 0;
  SPI_InitStructure.SPI_ClockPrescaler = 2;
  SPI_Init(HT_SPI0, &SPI_InitStructure);

  /* SPI1 configuration: Slave mode*/
//  SPI_InitStructure.SPI_Mode = SPI_SLAVE;
//  SPI_Init(HT_SPI1, &SPI_InitStructure);

  /* Enable SPI0 & SPI1*/
  
//  SPI_Cmd(HT_SPI1, ENABLE);

  /* Set SEL as output mode for slave select*/
  SPI_SELOutputCmd(HT_SPI0, ENABLE);

  /* Enable SPI0 & SPI1 RXBNE interrupt*/
//  SPI_IntConfig(HT_SPI0, SPI_INT_RXBNE, ENABLE);
//  SPI_IntConfig(HT_SPI1, SPI_INT_RXBNE, ENABLE);

	SPI_Cmd(HT_SPI0, ENABLE);

}
/*******************************************************************
*延时nns
*******************************************************************/
void delay_ns(u32 ns)
{
    u32 i;
    for(i=0; i<ns; i++)
    {
        __nop();
        __nop();
        __nop();
    }
}

/*******************************************************************
*SPIx 写一个字节
*Byte:要写入的字节
*返回值:读取到的字节
*******************************************************************/
u8 SPIWriteByte(u8 Byte)
{
    while((HT_SPI0->SR&0X02)==0);		//等待发送区空
    HT_SPI0->DR=Byte;	 	            //发送一个byte
    while((HT_SPI0->SR&0X01)==0);    //等待接收完一个byte
    return HT_SPI0->DR;          	  //返回收到的数据
}

/*******************************************************************
*SPIx 读写一个字节
*TxData:要写入的字节
*返回值:读取到的字节
*******************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 retry=0;
    while (SPI_GetFlagStatus(HT_SPI0, SPI_FLAG_TXBE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        retry++;
        if(retry>200)return 0;
    }
    SPI_SendData(HT_SPI0, TxData); //通过外设SPIx发送一个数据
    retry=0;

    while (SPI_GetFlagStatus(HT_SPI0, SPI_FLAG_RXBNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        retry++;
        if(retry>200)return 0;
    }
    return SPI_ReceiveData(HT_SPI0); //返回通过SPIx最近接收的数据
}
/*******************************************************************
*初始化RC522
*******************************************************************/
void InitRc522(void)
{
		SPI0_Init();
    PcdReset();
    PcdAntennaOff();
    delay_ms(2);
    PcdAntennaOn();
    M500PcdConfigISOType( 'A' );
}

void Reset_RC522(void)
{
    PcdReset();
    PcdAntennaOff();
    delay_ms(2);
    PcdAntennaOn();
}

/*******************************************************************
*功    能：寻卡
*参数说明: req_code[IN]:寻卡方式
*                0x52 = 寻感应区内所有符合14443A标准的卡
*                0x26 = 寻未进入休眠状态的卡
*          	  pTagType[OUT]：卡片类型代码
*                0x4400 = Mifare_UltraLight
*                0x0400 = Mifare_One(S50)
*                0x0200 = Mifare_One(S70)
*                0x0800 = Mifare_Pro(X)
*                0x4403 = Mifare_DESFire
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdRequest(u8   req_code,u8 *pTagType)
{
    char   status;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN];

    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x07);
    SetBitMask(TxControlReg,0x03);

    ucComMF522Buf[0] = req_code;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,1,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x10))
    {
        *pTagType     = ucComMF522Buf[0];
        *(pTagType+1) = ucComMF522Buf[1];
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/*******************************************************************
*功    能：防冲撞
*参数说明: pSnr[OUT]:卡片序列号，4字节
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdAnticoll(u8 *pSnr)
{
    char   status;
    u8   i,snr_check=0;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN];


    ClearBitMask(Status2Reg,0x08);
    WriteRawRC(BitFramingReg,0x00);
    ClearBitMask(CollReg,0x80);

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x20;

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,2,ucComMF522Buf,&unLen);

    if (status == MI_OK)
    {
        for (i=0; i<4; i++)
        {
            *(pSnr+i)  = ucComMF522Buf[i];
            snr_check ^= ucComMF522Buf[i];
        }
        if (snr_check != ucComMF522Buf[i])
        {
            status = MI_ERR;
        }
    }

    SetBitMask(CollReg,0x80);
    return status;
}

/*******************************************************************
*功    能：选定卡片
*参数说明: pSnr[IN]:卡片序列号，4字节
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdSelect(u8 *pSnr)
{
    char   status;
    u8   i;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_ANTICOLL1;
    ucComMF522Buf[1] = 0x70;
    ucComMF522Buf[6] = 0;
    for (i=0; i<4; i++)
    {
        ucComMF522Buf[i+2] = *(pSnr+i);
        ucComMF522Buf[6]  ^= *(pSnr+i);
    }
    CalulateCRC(ucComMF522Buf,7,&ucComMF522Buf[7]);

    ClearBitMask(Status2Reg,0x08);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,9,ucComMF522Buf,&unLen);

    if ((status == MI_OK) && (unLen == 0x18))
    {
        status = MI_OK;
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/*******************************************************************
*功    能：验证卡片密码
*参数说明: auth_mode[IN]: 密码验证模式
*                 0x60 = 验证A密钥
*                 0x61 = 验证B密钥
*          addr[IN]：块地址
*          pKey[IN]：密码
*          pSnr[IN]：卡片序列号，4字节
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdAuthState(u8   auth_mode,u8   addr,u8 *pKey,u8 *pSnr)
{
    char   status;
    u8   unLen;
    u8   ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = auth_mode;
    ucComMF522Buf[1] = addr;
    memcpy(&ucComMF522Buf[2], pKey, 6);
    memcpy(&ucComMF522Buf[8], pSnr, 4);
    status = PcdComMF522(PCD_AUTHENT,ucComMF522Buf,12,ucComMF522Buf,&unLen);
    if ((status != MI_OK) || (!(ReadRawRC(Status2Reg) & 0x08)))
    {
        status = MI_ERR;
    }

    return status;
}

/*******************************************************************
*功    能：读取M1卡一块数据
*参数说明: addr[IN]：块地址
*          p [OUT]：读出的数据，16字节
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdRead(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_READ;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);
    if ((status == MI_OK) && (unLen == 0x90))
    {
        for (i=0; i<16; i++)
        {
            *(p +i) = ucComMF522Buf[i];
        }
    }
    else
    {
        status = MI_ERR;
    }

    return status;
}

/*******************************************************************
*功    能：写数据到M1卡一块
*参数说明: addr[IN]：块地址
*          p [IN]：写入的数据，16字节
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdWrite(u8   addr,u8 *p )
{
    char   status;
    u8   unLen;
    u8   i,ucComMF522Buf[MAXRLEN];

    ucComMF522Buf[0] = PICC_WRITE;
    ucComMF522Buf[1] = addr;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
    {
        status = MI_ERR;
    }

    if (status == MI_OK)
    {
        for (i=0; i<16; i++)
        {
            ucComMF522Buf[i] = *(p +i);
        }
        CalulateCRC(ucComMF522Buf,16,&ucComMF522Buf[16]);

        status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,18,ucComMF522Buf,&unLen);
        if ((status != MI_OK) || (unLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
        {
            status = MI_ERR;
        }
    }

    return status;
}

/*******************************************************************
*功    能：命令卡片进入休眠状态
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdHalt(void)
{
    char status;
    u8 unLen;
    u8 ucComMF522Buf[MAXRLEN];
	
    ucComMF522Buf[0] = PICC_HALT;
    ucComMF522Buf[1] = 0;
    CalulateCRC(ucComMF522Buf,2,&ucComMF522Buf[2]);

    status = PcdComMF522(PCD_TRANSCEIVE,ucComMF522Buf,4,ucComMF522Buf,&unLen);

    return status;
}

/*******************************************************************
*用MF522计算CRC16函数
*******************************************************************/
void CalulateCRC(u8 *pIn ,u8   len,u8 *pOut )
{
    u8 i,n;
    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    for (i=0; i<len; i++)
    {
        WriteRawRC(FIFODataReg, *(pIn +i));
    }
    WriteRawRC(CommandReg, PCD_CALCCRC);
    i = 0xFF;
    do
    {
        n = ReadRawRC(DivIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x04));
    pOut [0] = ReadRawRC(CRCResultRegL);
    pOut [1] = ReadRawRC(CRCResultRegM);
}

/*******************************************************************
*功    能：复位RC522
*返    回: 成功返回MI_OK
*******************************************************************/
char PcdReset(void)
{
    //PORTD|=(1<<RC522RST);
    SET_RC522RST;
    delay_ns(10);
    //PORTD&=~(1<<RC522RST);
    CLR_RC522RST;
    delay_ns(10);
    //PORTD|=(1<<RC522RST);
    SET_RC522RST;
    delay_ns(10);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    WriteRawRC(CommandReg,PCD_RESETPHASE);
    delay_ns(10);

    WriteRawRC(ModeReg,0x3D);            //和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);

    WriteRawRC(TxAutoReg,0x40);//必须要

    return MI_OK;
}
/*******************************************************************
*设置RC632的工作方式
*******************************************************************/
char M500PcdConfigISOType(u8   type)
{
    if (type == 'A')                     //ISO14443_A
    {
        ClearBitMask(Status2Reg,0x08);
        WriteRawRC(ModeReg,0x3D);//3F
        WriteRawRC(RxSelReg,0x86);//84
        WriteRawRC(RFCfgReg,0x7F);   //4F
        WriteRawRC(TReloadRegL,30);//tmoLength);// TReloadVal = 'h6a =tmoLength(dec)
        WriteRawRC(TReloadRegH,0);
        WriteRawRC(TModeReg,0x8D);
        WriteRawRC(TPrescalerReg,0x3E);
        delay_ns(1000);
        PcdAntennaOn();
    }
    else {
        return 1;
    }

    return MI_OK;
}
/*******************************************************************
*功    能：读RC632寄存器
*参数说明：Address[IN]:寄存器地址
*返    回：读出的值
*******************************************************************/
u8 ReadRawRC(u8   Address)
{
    u8   ucAddr;
    u8   ucResult=0;
    CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E)|0x80;

    SPIWriteByte(ucAddr);
    ucResult=SPIReadByte();
    SET_SPI_CS;
    return ucResult;
}

/*******************************************************************
*功    能：写RC632寄存器
*参数说明：	Address[IN]:寄存器地址
*         	value[IN]:写入的值
*******************************************************************/
void WriteRawRC(u8   Address, u8   value)
{
    u8   ucAddr;
//	u8 tmp;

    CLR_SPI_CS;
    ucAddr = ((Address<<1)&0x7E);

    SPIWriteByte(ucAddr);
    SPIWriteByte(value);
    SET_SPI_CS;

//	tmp=ReadRawRC(Address);
//
//	if(value!=tmp)
//		printf("wrong\n");
}
/////////////////////////////////////////////////////////////////////
//功    能：置RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:置位值
/////////////////////////////////////////////////////////////////////
void SetBitMask(u8   reg,u8   mask)
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：清RC522寄存器位
//参数说明：reg[IN]:寄存器地址
//          mask[IN]:清位值
/////////////////////////////////////////////////////////////////////
void ClearBitMask(u8   reg,u8   mask)
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//功    能：通过RC522和ISO14443卡通讯
//参数说明：Command[IN]:RC522命令字
//          pIn [IN]:通过RC522发送到卡片的数据
//          InLenByte[IN]:发送数据的字节长度
//          pOut [OUT]:接收到的卡片返回数据
//          *pOutLenBit[OUT]:返回数据的位长度
/////////////////////////////////////////////////////////////////////
char PcdComMF522(u8   Command,
                 u8 *pIn ,
                 u8   InLenByte,
                 u8 *pOut ,
                 u8 *pOutLenBit)
{
    char   status = MI_ERR;
    u8   irqEn   = 0x00;
    u8   waitFor = 0x00;
    u8   lastBits;
    u8   n;
    u16   i;
    switch (Command)
    {
    case PCD_AUTHENT:
        irqEn   = 0x12;
        waitFor = 0x10;
        break;
    case PCD_TRANSCEIVE:
        irqEn   = 0x77;
        waitFor = 0x30;
        break;
    default:
        break;
    }

    WriteRawRC(ComIEnReg,irqEn|0x80);
    ClearBitMask(ComIrqReg,0x80);	//清所有中断位
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	//清FIFO缓存

    for (i=0; i<InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pIn [i]);
    }
    WriteRawRC(CommandReg, Command);
//   	 n = ReadRawRC(CommandReg);

    if (Command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg,0x80);     //开始传送
    }

    //i = 600;//根据时钟频率调整，操作M1卡最大等待时间25ms
    i = 2000;
    do
    {
        n = ReadRawRC(ComIrqReg);
        i--;
    }
    while ((i!=0) && !(n&0x01) && !(n&waitFor));
    ClearBitMask(BitFramingReg,0x80);

    if (i!=0)
    {
        if(!(ReadRawRC(ErrorReg)&0x1B))
        {
            status = MI_OK;
            if (n & irqEn & 0x01)
            {
                status = MI_NOTAGERR;
            }
            if (Command == PCD_TRANSCEIVE)
            {
                n = ReadRawRC(FIFOLevelReg);
                lastBits = ReadRawRC(ControlReg) & 0x07;
                if (lastBits)
                {
                    *pOutLenBit = (n-1)*8 + lastBits;
                }
                else
                {
                    *pOutLenBit = n*8;
                }
                if (n == 0)
                {
                    n = 1;
                }
                if (n > MAXRLEN)
                {
                    n = MAXRLEN;
                }
                for (i=0; i<n; i++)
                {
                    pOut [i] = ReadRawRC(FIFODataReg);
                }
            }
        }
        else
        {
            status = MI_ERR;
        }

    }


    SetBitMask(ControlReg,0x80);           // stop timer now
    WriteRawRC(CommandReg,PCD_IDLE);
    return status;
}

/////////////////////////////////////////////////////////////////////
//开启天线
//每次启动或关闭天险发射之间应至少有1ms的间隔
/////////////////////////////////////////////////////////////////////
void PcdAntennaOn(void)
{
    u8 i;
    i = ReadRawRC(TxControlReg);
    if (!(i & 0x03))
    {
        SetBitMask(TxControlReg, 0x03);
    }
}

/////////////////////////////////////////////////////////////////////
//关闭天线
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
/*************************************
*函数功能：读取卡片ID
*参数：无
***************************************/
u8 KEY[6]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
u8 cardflag;
void ReadID(void)								//读SPI1对应的卡
{
    u8 i;
    unsigned char status;
    status = PcdRequest(PICC_REQALL,CT);/*尋卡*/
    if(status==MI_OK)//尋卡成功
    {
        status = PcdAnticoll(SN);/*防冲撞*/
    }
    if(status==MI_OK)//防衝撞成功
    {
        status=MI_ERR;
//        for(i=0; i<4; i++)
//        {
//            u0_printf(SN[i]);
//        }
        u0_printf("\r\n");
        TurnID(SN);	 //转换卡的卡号，存在数组num[]内
    }
}
char num[9]= {0};
/*************************************
*函数功能：显示卡的卡号，以十六进制显示
*参数：x，y 坐标
*		p 卡号的地址
*		charcolor 字符的颜色
*		bkcolor   背景的颜色
***************************************/
void TurnID(u8 *p)	 //转换卡的卡号，以十六进制显示
{
    unsigned char i;
    for(i=0; i<4; i++)
    {
        num[i*2]=p[i]>>4;
        num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
        num[i*2+1]=p[i]&0x0f;
        num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
//        u0_printf((unsigned char)num[i*2]);
//				u0_printf((unsigned char)num[i*2+1]);
    }
    num[8]=0;
		u0_printf(num);
		u0_printf("\r\n");
		LCD_ShowString(30,30,200,16,16,num);
}



