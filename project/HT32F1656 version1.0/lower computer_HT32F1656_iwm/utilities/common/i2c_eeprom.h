/*********************************************************************************************************//**
 * @file    i2c_eeprom.h
 * @version $Rev:: 5            $
 * @date    $Date:: 2017-05-11 #$
 * @brief   The header file of i2c_eeprom.
 *************************************************************************************************************/
/* Define to prevent recursive inclusion -------------------------------------------------------------------*/
#ifndef __I2C_EEPROM_H
#define __I2C_EEPROM_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------------------------------------*/
#include "ht32_board.h"

/* Exported constants --------------------------------------------------------------------------------------*/
/** @defgroup I2C_EEPROM_Exported_Constants I2C EEPROM exported constants
  * @{
  */
#define I2C_SPEED                (30000)                                        /*!< I2C speed              */
#define I2C_EEPROM_CAPACITY      (256)                                          /*!< capacity in bytes      */
#define I2C_EEPROM_PAGE_SIZE     (8)                                            /*!< page size in bytes     */
#define I2C_EEPROM_ADDR_MASK     (I2C_EEPROM_CAPACITY - 1)                      /*!< address mask           */
#define I2C_EEPROM_PAGE_MASK     (I2C_EEPROM_PAGE_SIZE - 1)                     /*!< page size mask         */
#define I2C_DEV_ADDR             (0x5B)                                         /*!< I2C device address     */
/**
  * @}
  */

/* Exported functions --------------------------------------------------------------------------------------*/
/** @defgroup I2C_EEPROM_Exported_Functions I2C EEPROM exported functions
  * @{
  */
void I2C0_Init(void);

/*  @brief 页面编写。
	* @param pbuf:数据源指向缓冲区的指针。
	* @param WriteAddr:目的地 地址。
	* @param NumByteToWrite:数量的字节。
	* @retval没有
*/
void I2C_EEPROM_PageWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);

/*  @brief 写数字的序列数据。
	* @param pbuf:数据源指向缓冲区的指针。
	* @param WriteAddr:目的地 地址。
	* @param NumByteToWrite:数量的字节。
	* @retval没有
*/
void I2C_BufferWrite(u8* pBuffer, u8 WriteAddr, u16 NumByteToWrite);
void I2C_BufferRead(u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);
/**
  * @}
  */


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_EEPROM_H -----------------------------------------------------------------------------------*/
