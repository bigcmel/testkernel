#ifndef __SERV_UART_H__
#define __SERV_UART_H__

// 该服务的代码段起始地址
#define CODE_SEG_BASE 0x31200000



// 下面即该服务提供的所有函数所对应的操作码
#define SERV_UART_INIT 0x1



// 下面即该服务提供的所有函数
extern void serv_uart_init();




#endif
