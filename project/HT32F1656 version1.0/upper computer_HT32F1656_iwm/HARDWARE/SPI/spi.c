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

unsigned char CT[2];			//������
unsigned char SN[4]; 			//����
unsigned char ST[4];

/***********************
*����˵����
*1--SS  <----->PA7
*2--SCK <----->PA4
*3--MOSI<----->PA5
*4--MISO<----->PA6
*5--����
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
*��ʱnns
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
*SPIx дһ���ֽ�
*Byte:Ҫд����ֽ�
*����ֵ:��ȡ�����ֽ�
*******************************************************************/
u8 SPIWriteByte(u8 Byte)
{
    while((HT_SPI0->SR&0X02)==0);		//�ȴ���������
    HT_SPI0->DR=Byte;	 	            //����һ��byte
    while((HT_SPI0->SR&0X01)==0);    //�ȴ�������һ��byte
    return HT_SPI0->DR;          	  //�����յ�������
}

/*******************************************************************
*SPIx ��дһ���ֽ�
*TxData:Ҫд����ֽ�
*����ֵ:��ȡ�����ֽ�
*******************************************************************/
u8 SPI1_ReadWriteByte(u8 TxData)
{
    u8 retry=0;
    while (SPI_GetFlagStatus(HT_SPI0, SPI_FLAG_TXBE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
    {
        retry++;
        if(retry>200)return 0;
    }
    SPI_SendData(HT_SPI0, TxData); //ͨ������SPIx����һ������
    retry=0;

    while (SPI_GetFlagStatus(HT_SPI0, SPI_FLAG_RXBNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
    {
        retry++;
        if(retry>200)return 0;
    }
    return SPI_ReceiveData(HT_SPI0); //����ͨ��SPIx������յ�����
}
/*******************************************************************
*��ʼ��RC522
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
*��    �ܣ�Ѱ��
*����˵��: req_code[IN]:Ѱ����ʽ
*                0x52 = Ѱ��Ӧ�������з���14443A��׼�Ŀ�
*                0x26 = Ѱδ��������״̬�Ŀ�
*          	  pTagType[OUT]����Ƭ���ʹ���
*                0x4400 = Mifare_UltraLight
*                0x0400 = Mifare_One(S50)
*                0x0200 = Mifare_One(S70)
*                0x0800 = Mifare_Pro(X)
*                0x4403 = Mifare_DESFire
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ�����ײ
*����˵��: pSnr[OUT]:��Ƭ���кţ�4�ֽ�
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ�ѡ����Ƭ
*����˵��: pSnr[IN]:��Ƭ���кţ�4�ֽ�
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ���֤��Ƭ����
*����˵��: auth_mode[IN]: ������֤ģʽ
*                 0x60 = ��֤A��Կ
*                 0x61 = ��֤B��Կ
*          addr[IN]�����ַ
*          pKey[IN]������
*          pSnr[IN]����Ƭ���кţ�4�ֽ�
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ���ȡM1��һ������
*����˵��: addr[IN]�����ַ
*          p [OUT]�����������ݣ�16�ֽ�
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ�д���ݵ�M1��һ��
*����˵��: addr[IN]�����ַ
*          p [IN]��д������ݣ�16�ֽ�
*��    ��: �ɹ�����MI_OK
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
*��    �ܣ����Ƭ��������״̬
*��    ��: �ɹ�����MI_OK
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
*��MF522����CRC16����
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
*��    �ܣ���λRC522
*��    ��: �ɹ�����MI_OK
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

    WriteRawRC(ModeReg,0x3D);            //��Mifare��ͨѶ��CRC��ʼֵ0x6363
    WriteRawRC(TReloadRegL,30);
    WriteRawRC(TReloadRegH,0);
    WriteRawRC(TModeReg,0x8D);
    WriteRawRC(TPrescalerReg,0x3E);

    WriteRawRC(TxAutoReg,0x40);//����Ҫ

    return MI_OK;
}
/*******************************************************************
*����RC632�Ĺ�����ʽ
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
*��    �ܣ���RC632�Ĵ���
*����˵����Address[IN]:�Ĵ�����ַ
*��    �أ�������ֵ
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
*��    �ܣ�дRC632�Ĵ���
*����˵����	Address[IN]:�Ĵ�����ַ
*         	value[IN]:д���ֵ
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
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void SetBitMask(u8   reg,u8   mask)
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg,tmp | mask);  // set bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ���RC522�Ĵ���λ
//����˵����reg[IN]:�Ĵ�����ַ
//          mask[IN]:��λֵ
/////////////////////////////////////////////////////////////////////
void ClearBitMask(u8   reg,u8   mask)
{
    char   tmp = 0x0;
    tmp = ReadRawRC(reg);
    WriteRawRC(reg, tmp & ~mask);  // clear bit mask
}

/////////////////////////////////////////////////////////////////////
//��    �ܣ�ͨ��RC522��ISO14443��ͨѶ
//����˵����Command[IN]:RC522������
//          pIn [IN]:ͨ��RC522���͵���Ƭ������
//          InLenByte[IN]:�������ݵ��ֽڳ���
//          pOut [OUT]:���յ��Ŀ�Ƭ��������
//          *pOutLenBit[OUT]:�������ݵ�λ����
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
    ClearBitMask(ComIrqReg,0x80);	//�������ж�λ
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);	 	//��FIFO����

    for (i=0; i<InLenByte; i++)
    {
        WriteRawRC(FIFODataReg, pIn [i]);
    }
    WriteRawRC(CommandReg, Command);
//   	 n = ReadRawRC(CommandReg);

    if (Command == PCD_TRANSCEIVE)
    {
        SetBitMask(BitFramingReg,0x80);     //��ʼ����
    }

    //i = 600;//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms
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
//��������
//ÿ��������ر����շ���֮��Ӧ������1ms�ļ��
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
//�ر�����
/////////////////////////////////////////////////////////////////////
void PcdAntennaOff(void)
{
    ClearBitMask(TxControlReg, 0x03);
}
/*************************************
*�������ܣ���ȡ��ƬID
*��������
***************************************/
u8 KEY[6]= {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
u8 cardflag;
void ReadID(void)								//��SPI1��Ӧ�Ŀ�
{
    u8 i;
    unsigned char status;
    status = PcdRequest(PICC_REQALL,CT);/*����*/
    if(status==MI_OK)//�����ɹ�
    {
        status = PcdAnticoll(SN);/*����ײ*/
    }
    if(status==MI_OK)//���nײ�ɹ�
    {
        status=MI_ERR;
//        for(i=0; i<4; i++)
//        {
//            u0_printf(SN[i]);
//        }
        u0_printf("\r\n");
        TurnID(SN);	 //ת�����Ŀ��ţ���������num[]��
    }
}
char num[9]= {0};
/*************************************
*�������ܣ���ʾ���Ŀ��ţ���ʮ��������ʾ
*������x��y ����
*		p ���ŵĵ�ַ
*		charcolor �ַ�����ɫ
*		bkcolor   ��������ɫ
***************************************/
void TurnID(u8 *p)	 //ת�����Ŀ��ţ���ʮ��������ʾ
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


