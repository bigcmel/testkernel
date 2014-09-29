#ifndef __SERV_GLOBAL_H__
#define __SERV_GLOBAL_H__

#define Mode_USR 0x10
#define Mode_FIQ 0x11
#define Mode_IRQ 0x12
#define Mode_SVC 0x13
#define Mode_ABT 0x17
#define Mode_UND 0x1B
#define Mode_SYS 0x1F


#define NULL 0x0
#define RETURN_SUCCESS 1
#define RETURN_FAILED (-1)


typedef unsigned char BYTE;
typedef unsigned short HWORD; // 半字，两个字节
typedef unsigned long WORD; // 字，四个字节


typedef void(*PTRFUNC)(void); // PTRFUNC 是一个指向函数的指针


/* sys 服务的相关参数 */

#define SYS_REGISTER_TABLE_BASE 0x31100000
#define SYS_REGISRER_TABLE_LIMIT 0x00100000

#define SYS_CODE_BASE 0x31200000
#define SYS_CODE_LIMIT 0x00C00000

#define SYS_SHARE_RAM_BASE 0x31E00000
#define SYS_SHARE_RAM_LIMIT 0x00100000


// sys 服务中操作码的定义，这里的操作码都是规定好的，所有的 sys 服务都必须包含这些操作码
#define SYS_SERV_RETURN_OPT 0x0


WORD CODE_SEG_LIMIT;
WORD PARA_SEG_BASE;

WORD OPT_CODE_BASE;
WORD RETURN_CODE_BASE;


// 记录最近发生的错误的错误码
WORD SERV_ERR_CODE;

#define SERV_ERR_UND_OPT 0x1

extern void serv_handle_error();



#endif
