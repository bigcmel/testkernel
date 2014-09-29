#include "sys_serv_funcs.h"


WORD sys_serv_uart_SendByte(char ch)
{
  WORD opt_code;
  WORD para_base[SERV_MAX_PARA_NUM];
  WORD para_num;

  WORD result;


  SYS_SERV_IDX = SERV_UART_IDX;

  opt_code = SERV_UART_SendByte;
  para_num = UART_SendByte_PARA_NUM;

  para_base[0] = (WORD)(&ch);

  sys_send_para(opt_code, (WORD)para_base, para_num);
  result = sys_run();

  return result;
}

WORD sys_serv_uart_SendString(const char* str, unsigned int len)
{
  WORD opt_code;
  WORD para_base[SERV_MAX_PARA_NUM];
  WORD para_num;

  WORD result;


  SYS_SERV_IDX = SERV_UART_IDX;

  opt_code = SERV_UART_SendString;
  para_num = UART_SendString_PARA_NUM;

  para_base[0] = (WORD)str;
  para_base[1] = (WORD)len;

  sys_send_para(opt_code, (WORD)para_base, para_num);
  result = sys_run();

  return result;
}

