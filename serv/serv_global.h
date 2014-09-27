#ifndef __SERV_GLOBAL_H__
#define __SERV_GLOBAL_H__


typedef unsigned char BYTE;
typedef unsigned short HWORD;
typedef unsigned long WORD;


// sys 服务中操作码的定义，这里的操作码都是规定好的，所有的 sys 服务都必须包含这些操作码
#define SYS_SERV_RETURN_OPT 0x0


WORD CODE_SEG_LIMIT;
WORD PARA_SEG_BASE;

WORD OPT_CODE_BASE;
WORD RETURN_CODE_BASE;


// 记录最近发生的错误的错误码
WORD SERV_ERR_CODE;

#define SERV_ERR_UND_OPT 0x1


#endif
