/** \file max30102.cpp ******************************************************
*
* Project: MAXREFDES117#
* Filename: max30102.cpp
* Description: This module is an embedded controller driver for the MAX30102
*
* Revision History:
*\n 1-18-2016 Rev 01.00 GL Initial release.
*\n
*/

#include "max30102.h"
#include "myiic.h"
#include "usart.h"
#include "algorithm.h"


#define max30102_WR_address 0xAE

#define MAX_BRIGHTNESS 255
uint32_t aun_ir_buffer[150]; //infrared LED sensor data
uint32_t aun_red_buffer[150];  //red LED sensor data
int32_t n_ir_buffer_length; //data length
int32_t n_spo2;  //SPO2 value
int8_t ch_spo2_valid;  //indicator to show if the SPO2 calculation is valid
int32_t n_heart_rate; //heart rate value
int8_t  ch_hr_valid;  //indicator to show if the heart rate calculation is valid
uint8_t uch_dummy;

int32_t hr_buf[16];
int32_t hrSum;
int32_t hrAvg;
int32_t spo2_buf[16];
int32_t spo2Sum;
int32_t spo2Avg;
int32_t spo2BuffFilled;
int32_t hrBuffFilled;
int32_t hrValidCnt = 0;
int32_t spo2ValidCnt = 0;
int32_t hrThrowOutSamp = 0;
int32_t spo2ThrowOutSamp = 0;
int32_t spo2Timeout = 0;
int32_t hrTimeout = 0;


//**************************************
//��IIC�豸д��һ���ֽ�����
//**************************************
bool maxim_max30102_write_reg(uint8_t uch_addr, uint8_t uch_data)
/**
* \brief        Write a value to a MAX30102 register
* \par          Details
*               This function writes a value to a MAX30102 register
*
* \param[in]    uch_addr    - register address
* \param[in]    uch_data    - register data
*
* \retval       true on success
*/
{
    /* ��1��������I2C���������ź� */
    i2c_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ */
    i2c_SendByte(uch_addr);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ��5������ʼд������ */
    i2c_SendByte(uch_data);

    /* ��6��������ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return true;	/* ִ�гɹ� */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return false;
}
//**************************************
//��IIC�豸��ȡһ���ֽ�����
//**************************************
bool maxim_max30102_read_reg(uint8_t uch_addr, uint8_t *puch_data)
/**
* \brief        Read a MAX30102 register
* \par          Details
*               This function reads a MAX30102 register
*
* \param[in]    uch_addr    - register address
* \param[out]   puch_data    - pointer that stores the register data
*
* \retval       true on success
*/
{
    /* ��1��������I2C���������ź� */
    i2c_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ�� */
    i2c_SendByte((uint8_t)uch_addr);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }


    /* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
    i2c_Start();

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

    /* ��8��������ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ��9������ȡ���� */
    {
        *puch_data = i2c_ReadByte();	/* ��1���ֽ� */

        i2c_NAck();	/* ���1���ֽڶ����CPU����NACK�ź�(����SDA = 1) */
    }
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return true;	/* ִ�гɹ� ����dataֵ */

cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return false;
}

bool maxim_max30102_init(void)
/**
* \brief        Initialize the MAX30102
* \par          Details
*               This function initializes the MAX30102
*
* \param        None
*
* \retval       true on success
*/
{
    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_1, 0xc0)) // INTR setting
        return false;
    if(!maxim_max30102_write_reg(REG_INTR_ENABLE_2, 0x00))
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_WR_PTR, 0x00)) //FIFO_WR_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_OVF_COUNTER, 0x00)) //OVF_COUNTER[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_RD_PTR, 0x00)) //FIFO_RD_PTR[4:0]
        return false;
    if(!maxim_max30102_write_reg(REG_FIFO_CONFIG, 0x6f)) //sample avg = 8, fifo rollover=false, fifo almost full = 17
        return false;
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x03))  //0x02 for Red only, 0x03 for SpO2 mode 0x07 multimode LED
        return false;
    if(!maxim_max30102_write_reg(REG_SPO2_CONFIG, 0x2F)) // SPO2_ADC range = 4096nA, SPO2 sample rate (400 Hz), LED pulseWidth (411uS)
        return false;

    if(!maxim_max30102_write_reg(REG_LED1_PA, 0x17))  //Choose value for ~ 4.5mA for LED1
        return false;
    if(!maxim_max30102_write_reg(REG_LED2_PA, 0x17))  // Choose value for ~ 4.5mA for LED2
        return false;
    if(!maxim_max30102_write_reg(REG_PILOT_PA, 0x7f))  // Choose value for ~ 25mA for Pilot LED
        return false;
    return true;
}

bool maxim_max30102_read_fifo(uint32_t *pun_red_led, uint32_t *pun_ir_led)

/**
* \brief        Read a set of samples from the MAX30102 FIFO register
* \par          Details
*               This function reads a set of samples from the MAX30102 FIFO register
*
* \param[out]   *pun_red_led   - pointer that stores the red LED reading data
* \param[out]   *pun_ir_led    - pointer that stores the IR LED reading data
*
* \retval       true on success
*/
{
    uint32_t un_temp;
    uint8_t uch_temp;
    *pun_ir_led = 0;
    *pun_red_led = 0;
    maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_temp);
    maxim_max30102_read_reg(REG_INTR_STATUS_2, &uch_temp);



    /* ��1��������I2C���������ź� */
    i2c_Start();

    /* ��2������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(max30102_WR_address | I2C_WR);	/* �˴���дָ�� */

    /* ��3��������ACK */
    if (i2c_WaitAck() != 0)
    {
        printf("read fifo failed");
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    /* ��4���������ֽڵ�ַ�� */
    i2c_SendByte((uint8_t)REG_FIFO_DATA);
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }


    /* ��6������������I2C���ߡ����濪ʼ��ȡ���� */
    i2c_Start();

    /* ��7������������ֽڣ���7bit�ǵ�ַ��bit0�Ƕ�д����λ��0��ʾд��1��ʾ�� */
    i2c_SendByte(max30102_WR_address | I2C_RD);	/* �˴��Ƕ�ָ�� */

    /* ��8��������ACK */
    if (i2c_WaitAck() != 0)
    {
        goto cmd_fail;	/* EEPROM������Ӧ�� */
    }

    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 16;
    *pun_red_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 8;
    *pun_red_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    *pun_red_led += un_temp;

    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 16;
    *pun_ir_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    un_temp <<= 8;
    *pun_ir_led += un_temp;
    un_temp = i2c_ReadByte();
    i2c_Ack();
    *pun_ir_led += un_temp;
    *pun_red_led &= 0x03FFFF; //Mask MSB [23:18]
    *pun_ir_led &= 0x03FFFF; //Mask MSB [23:18]

    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return true;
cmd_fail: /* ����ִ��ʧ�ܺ��мǷ���ֹͣ�źţ�����Ӱ��I2C�����������豸 */
    /* ����I2C����ֹͣ�ź� */
    i2c_Stop();
    return false;
}

bool maxim_max30102_reset()
/**
* \brief        Reset the MAX30102
* \par          Details
*               This function resets the MAX30102
*
* \param        None
*
* \retval       true on success
*/
{
    if(!maxim_max30102_write_reg(REG_MODE_CONFIG, 0x40))
        return false;
    else
        return true;
}



// the loop routine runs over and over again forever:
void calcu_heart_spo2(void)
{
	  uint32_t err_time=0;//�ж����ŵȴ���ʱ
    uint32_t un_min, un_max, un_prev_data, un_brightness;  //variables to calculate the on-board LED brightness that reflects the heartbeats
    int32_t i;
    float f_temp;

    un_brightness = 0;
    un_min = 0x3FFFF;
    un_max = 0;

    n_ir_buffer_length = 150; //buffer length of 150 stores 3 seconds of samples running at 50sps

    //read the first 150 samples, and determine the signal range
    for(i = 0; i < n_ir_buffer_length; i++)
    {
        while(KEY0 == 1)
				{			
					err_time++;
					if(err_time>50000)
						{
							err_time =0;
							break;
						}		
				}; //wait until the interrupt pin asserts
        maxim_max30102_read_fifo((aun_red_buffer + i), (aun_ir_buffer + i)); //read from MAX30102 FIFO

        if(un_min > aun_red_buffer[i])
            un_min = aun_red_buffer[i]; //update signal min
        if(un_max < aun_red_buffer[i])
            un_max = aun_red_buffer[i]; //update signal max
    }
    un_prev_data = aun_red_buffer[i];
    //calculate heart rate and SpO2 after first 150 samples (first 3 seconds of samples)
    maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

    //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
        i = 0;
        un_min = 0x3FFFF;
        un_max = 0;

        //dumping the first 50 sets of samples in the memory and shift the last 100 sets of samples to the top
        for(i = 50; i < 150; i++)
        {
            aun_red_buffer[i - 50] = aun_red_buffer[i];
            aun_ir_buffer[i - 50] = aun_ir_buffer[i];

            //update the signal min and max
            if(un_min > aun_red_buffer[i])
                un_min = aun_red_buffer[i];
            if(un_max < aun_red_buffer[i])
                un_max = aun_red_buffer[i];
        }

        //take 50 sets of samples before calculating the heart rate.
        for(i = 100; i < 150; i++)
        {
            un_prev_data = aun_red_buffer[i - 1];
            
						while(KEY0 == 1)
						{			
							err_time++;
							if(err_time>50000)
								{
									err_time =0;
									break;
								}		
						}; //wait until the interrupt pin asserts
            maxim_max30102_read_fifo((aun_red_buffer + i), (aun_ir_buffer + i));

            //calculate the brightness of the LED
            if(aun_red_buffer[i] > un_prev_data)
            {
                f_temp = aun_red_buffer[i] - un_prev_data;
                f_temp /= (un_max - un_min);
                f_temp *= MAX_BRIGHTNESS;
                f_temp = un_brightness - f_temp;
                if(f_temp < 0)
                    un_brightness = 0;
                else
                    un_brightness = (int)f_temp;
            }
            else
            {
                f_temp = un_prev_data - aun_red_buffer[i];
                f_temp /= (un_max - un_min);
                f_temp *= MAX_BRIGHTNESS;
                un_brightness += (int)f_temp;
                if(un_brightness > MAX_BRIGHTNESS)
                    un_brightness = MAX_BRIGHTNESS;
            }
        }
        maxim_heart_rate_and_oxygen_saturation(aun_ir_buffer, n_ir_buffer_length, aun_red_buffer, &n_spo2, &ch_spo2_valid, &n_heart_rate, &ch_hr_valid);

        if ((ch_hr_valid == 1) && (n_heart_rate < 190) && (n_heart_rate > 40))
        {
            hrTimeout = 0;

            // Throw out up to 1 out of every 5 valid samples if wacky
            if (hrValidCnt == 4)
            {
                hrThrowOutSamp = 1;
                hrValidCnt = 0;
                for (i = 12; i < 16; i++)
                {
                    if (n_heart_rate < hr_buf[i] + 10)
                    {
                        hrThrowOutSamp = 0;
                        hrValidCnt   = 4;
                    }
                }
            }
            else
            {
                hrValidCnt = hrValidCnt + 1;
            }

            if (hrThrowOutSamp == 0)
            {

                // Shift New Sample into buffer
                for(i = 0; i < 15; i++)
                {
                    hr_buf[i] = hr_buf[i + 1];
                }
                hr_buf[15] = n_heart_rate;

                // Update buffer fill value
                if (hrBuffFilled < 16)
                {
                    hrBuffFilled = hrBuffFilled + 1;
                }

                // Take moving average
                hrSum = 0;
                if (hrBuffFilled < 2)
                {
                    hrAvg = 0;
                }
                else if (hrBuffFilled < 4)
                {
                    for(i = 14; i < 16; i++)
                    {
                        hrSum = hrSum + hr_buf[i];
                    }
                    hrAvg = hrSum >> 1;
                }
                else if (hrBuffFilled < 8)
                {
                    for(i = 12; i < 16; i++)
                    {
                        hrSum = hrSum + hr_buf[i];
                    }
                    hrAvg = hrSum >> 2;
                }
                else if (hrBuffFilled < 16)
                {
                    for(i = 8; i < 16; i++)
                    {
                        hrSum = hrSum + hr_buf[i];
                    }
                    hrAvg = hrSum >> 3;
                }
                else
                {
                    for(i = 0; i < 16; i++)
                    {
                        hrSum = hrSum + hr_buf[i];
                    }
                    hrAvg = hrSum >> 4;
                }
            }
            hrThrowOutSamp = 0;
        }
        else
        {
            hrValidCnt = 0;
            if (hrTimeout == 4)
            {
                hrAvg = 0;
                hrBuffFilled = 0;
            }
            else
            {
                hrTimeout++;
            }
        }

        if ((ch_spo2_valid == 1) && (n_spo2 > 59))
        {
            spo2Timeout = 0;

            // Throw out up to 1 out of every 5 valid samples if wacky
            if (spo2ValidCnt == 4)
            {
                spo2ThrowOutSamp = 1;
                spo2ValidCnt = 0;
                for (i = 12; i < 16; i++)
                {
                    if (n_spo2 > spo2_buf[i] - 10)
                    {
                        spo2ThrowOutSamp = 0;
                        spo2ValidCnt   = 4;
                    }
                }
            }
            else
            {
                spo2ValidCnt = spo2ValidCnt + 1;
            }

            if (spo2ThrowOutSamp == 0)
            {

                // Shift New Sample into buffer
                for(i = 0; i < 15; i++)
                {
                    spo2_buf[i] = spo2_buf[i + 1];
                }
                spo2_buf[15] = n_spo2;

                // Update buffer fill value
                if (spo2BuffFilled < 16)
                {
                    spo2BuffFilled = spo2BuffFilled + 1;
                }

                // Take moving average
                spo2Sum = 0;
                if (spo2BuffFilled < 2)
                {
                    spo2Avg = 0;
                }
                else if (spo2BuffFilled < 4)
                {
                    for(i = 14; i < 16; i++)
                    {
                        spo2Sum = spo2Sum + spo2_buf[i];
                    }
                    spo2Avg = spo2Sum >> 1;
                }
                else if (spo2BuffFilled < 8)
                {
                    for(i = 12; i < 16; i++)
                    {
                        spo2Sum = spo2Sum + spo2_buf[i];
                    }
                    spo2Avg = spo2Sum >> 2;
                }
                else if (spo2BuffFilled < 16)
                {
                    for(i = 8; i < 16; i++)
                    {
                        spo2Sum = spo2Sum + spo2_buf[i];
                    }
                    spo2Avg = spo2Sum >> 3;
                }
                else
                {
                    for(i = 0; i < 16; i++)
                    {
                        spo2Sum = spo2Sum + spo2_buf[i];
                    }
                    spo2Avg = spo2Sum >> 4;
                }
            }
            spo2ThrowOutSamp = 0;
        }
        else
        {
            spo2ValidCnt = 0;
            if (spo2Timeout == 4)
            {
                spo2Avg = 0;
                spo2BuffFilled = 0;
            }
            else
            {
                spo2Timeout++;
            }
        }

}

void max30102_init(void)
{
	bsp_InitI2C();       //����Ѫ��ģ�����ų�ʼ��
	maxim_max30102_reset(); //resets the MAX30102 ��������
	maxim_max30102_read_reg(REG_INTR_STATUS_1, &uch_dummy); //Reads/clears the interrupt status register
	maxim_max30102_init();  //initialize the MAX30102
}
