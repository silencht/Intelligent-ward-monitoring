#ifndef SYSTEM_H
#define SYSTEM_H

//-- Includes -----------------------------------------------------------------
#include "typedefs.h"              // type definitions
//-- Enumerations -------------------------------------------------------------
// Error codes
typedef enum{
  NO_ERROR       = 0x00, // 没错误
  ACK_ERROR      = 0x01, // 应答错误
  CHECKSUM_ERROR = 0x02, // 校验和不匹配错误
  TIMEOUT_ERROR  = 0x04, // 超时错误
  PARM_ERROR     = 0x80, // 参数超出范围误差
}etError;

#endif

