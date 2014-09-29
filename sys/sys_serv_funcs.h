#ifndef __SYS_SERV_FUNCS_H__
#define __SYS_SERV_FUNCS_H__

#include "../include/global.h"
#include "sys.h"

#define SERV_MAX_PARA_NUM 5


// 下面即该服务提供的所有函数所对应的操作码
#define SERV_UART_INIT 0x1
#define SERV_UART_SendByte 0x2
#define SERV_UART_SendString 0x3


WORD SERV_UART_IDX;


#define UART_SendByte_PARA_NUM 1
extern WORD sys_serv_uart_SendByte(char ch);

#define UART_SendString_PARA_NUM 2
extern WORD sys_serv_uart_SendString(const char* str, unsigned int len);



#endif
