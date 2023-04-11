#ifndef SHT3X_H
#define SHT3X_H

//-- Includes -----------------------------------------------------------------
#include "system.h"
#include "myiic.h"
#include "usart.h"
#include "delay.h"
//-- 枚举 -------------------------------------------------------------
// 传感器命令

#define sht30_WR_address    0x44
typedef enum{
  CMD_READ_SERIALNBR  = 0x3780, // read serial number
  CMD_READ_STATUS     = 0xF32D, // read status register
  CMD_CLEAR_STATUS    = 0x3041, // clear status register
  CMD_HEATER_ENABLE   = 0x306D, // enabled heater
  CMD_HEATER_DISABLE  = 0x3066, // disable heater
  CMD_SOFT_RESET      = 0x30A2, // soft reset
  CMD_MEAS_CLOCKSTR_H = 0x2C06, // 测量方式		:	时钟拉伸，高重复性
  CMD_MEAS_CLOCKSTR_M = 0x2C0D, // measurement: clock stretching, medium repeatability
  CMD_MEAS_CLOCKSTR_L = 0x2C10, // measurement: clock stretching, low repeatability
  CMD_MEAS_POLLING_H  = 0x2400, // measurement: polling, high repeatability
  CMD_MEAS_POLLING_M  = 0x240B, // measurement: polling, medium repeatability
  CMD_MEAS_POLLING_L  = 0x2416, // measurement: polling, low repeatability
  CMD_MEAS_PERI_05_H  = 0x2032, // measurement: periodic 0.5 mps, high repeatability
  CMD_MEAS_PERI_05_M  = 0x2024, // measurement: periodic 0.5 mps, medium repeatability
  CMD_MEAS_PERI_05_L  = 0x202F, // measurement: periodic 0.5 mps, low repeatability
  CMD_MEAS_PERI_1_H   = 0x2130, // 测量方式		: 周期性		 1 mps，高重复性
  CMD_MEAS_PERI_1_M   = 0x2126, // measurement: periodic 1 mps, medium repeatability
  CMD_MEAS_PERI_1_L   = 0x212D, // measurement: periodic 1 mps, low repeatability
  CMD_MEAS_PERI_2_H   = 0x2236, // measurement: periodic 2 mps, high repeatability
  CMD_MEAS_PERI_2_M   = 0x2220, // measurement: periodic 2 mps, medium repeatability
  CMD_MEAS_PERI_2_L   = 0x222B, // measurement: periodic 2 mps, low repeatability
  CMD_MEAS_PERI_4_H   = 0x2334, // 测量方式		: 周期性		 4 mps，高重复性
  CMD_MEAS_PERI_4_M   = 0x2322, // measurement: periodic 4 mps, medium repeatability
  CMD_MEAS_PERI_4_L   = 0x2329, // measurement: periodic 4 mps, low repeatability
  CMD_MEAS_PERI_10_H  = 0x2737, // measurement: periodic 10 mps, high repeatability
  CMD_MEAS_PERI_10_M  = 0x2721, // measurement: periodic 10 mps, medium repeatability
  CMD_MEAS_PERI_10_L  = 0x272A, // measurement: periodic 10 mps, low repeatability
  CMD_FETCH_DATA      = 0xE000, // 周期模式--读出测量值
  CMD_R_AL_LIM_LS     = 0xE102, // read alert limits, low set
  CMD_R_AL_LIM_LC     = 0xE109, // read alert limits, low clear
  CMD_R_AL_LIM_HS     = 0xE11F, // read alert limits, high set
  CMD_R_AL_LIM_HC     = 0xE114, // read alert limits, high clear
  CMD_W_AL_LIM_HS     = 0x611D, // write alert limits, high set
  CMD_W_AL_LIM_HC     = 0x6116, // write alert limits, high clear
  CMD_W_AL_LIM_LC     = 0x610B, // write alert limits, low clear
  CMD_W_AL_LIM_LS     = 0x6100, // write alert limits, low set
  CMD_NO_SLEEP        = 0x303E,
}etCommands;

// 测量的重复性
typedef enum{
  REPEATAB_HIGH,   // 高度重复
  REPEATAB_MEDIUM, // medium repeatability
  REPEATAB_LOW,    // low repeatability
}etRepeatability;

// 测量模式
typedef enum{
  MODE_CLKSTRETCH, // 时钟拉伸
  MODE_POLLING,    // 轮询
}etMode;

typedef enum{
  FREQUENCY_HZ5,  //  每秒钟0.5测量
  FREQUENCY_1HZ,  //  1.0 measurements per seconds
  FREQUENCY_2HZ,  //  2.0 measurements per seconds
  FREQUENCY_4HZ,  //  4.0 measurements per seconds
  FREQUENCY_10HZ, // 10.0 measurements per seconds
}etFrequency;

//-- Typedefs -----------------------------------------------------------------
// 状态寄存器
typedef union {
  u16t u16;
  struct{
    #ifdef LITTLE_ENDIAN  // bit-order is little endian
    u16t CrcStatus     : 1; // write data checksum status
    u16t CmdStatus     : 1; // command status
    u16t Reserve0      : 2; // reserved
    u16t ResetDetected : 1; // system reset detected
    u16t Reserve1      : 5; // reserved
    u16t T_Alert       : 1; // temperature tracking alert
    u16t RH_Alert      : 1; // humidity tracking alert
    u16t Reserve2      : 1; // reserved
    u16t HeaterStatus  : 1; // heater status
    u16t Reserve3      : 1; // reserved
    u16t AlertPending  : 1; // alert pending status 
    #else                 // bit-order is big endian
    u16t AlertPending  : 1;
    u16t Reserve3      : 1;
    u16t HeaterStatus  : 1;
    u16t Reserve2      : 1;
    u16t RH_Alert      : 1;
    u16t T_Alert       : 1;
    u16t Reserve1      : 5;
    u16t ResetDetected : 1;
    u16t Reserve0      : 2;
    u16t CmdStatus     : 1;
    u16t CrcStatus     : 1;
    #endif
  }bit;
} regStatus;


uint8_t SHT3X_ReadMeasurementBuffer(ft* temperature, ft* humidity);
uint8_t SHT3X_StartWriteAccess(void)	;
uint8_t SHT3X_WriteCommand(etCommands command);
void SHT3X_StopAccess(void);
uint8_t SHT3X_StartReadAccess(void);
void SHT3X_Read2BytesAndCrc(u16t* data,etI2cAck finaleAckNack);
ft SHT3X_CalcTemperature(u16t rawValue);
ft SHT3X_CalcHumidity(u16t rawValue);
uint8_t i2c_SHT30_WaitAck(void);
uint8_t SHT3X_StartPeriodicMeasurment(etRepeatability repeatability, etFrequency frequency);
#endif
