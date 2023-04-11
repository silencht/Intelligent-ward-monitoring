#ifndef SYSTEM_H
#define SYSTEM_H

//-- Includes -----------------------------------------------------------------
#include "typedefs.h"              // type definitions
//-- Enumerations -------------------------------------------------------------
// Error codes
typedef enum{
  NO_ERROR       = 0x00, // û����
  ACK_ERROR      = 0x01, // Ӧ�����
  CHECKSUM_ERROR = 0x02, // У��Ͳ�ƥ�����
  TIMEOUT_ERROR  = 0x04, // ��ʱ����
  PARM_ERROR     = 0x80, // ����������Χ���
}etError;

#endif

