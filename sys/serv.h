#ifndef __SERV_H__
#define __SERV_H__

#include "../include/global.h"


// 记录 sys 服务的数量
int SERV_NUM;

// sys 服务数量的最大值
#define SERV_MAX_NUM 99


// 在 SYS 中的进程的几种状态
#define SERV_STATUS_READY 0
#define SERV_STATUS_RUNNING 1
#define SERV_STATUS_UNREADY 2
#define SERV_STATUS_EMPTY 3  // 该索引还没有注册进服务


// 描述一个在 sys 服务表中的表项
typedef struct serv_table
{
  WORD CODE_SEG_BASE;  // 代码段基地址
  WORD CODE_SEG_LIMIT; // 代码段长度
  WORD PARA_SEG_BASE;  // 共享数据段基地址，不用写共享数据段的长度，因为长度指定为 3 个WORD
}SERV_TABLE, *ptr_serv_table;

// 指向当前要执行的 sys 服务表项
ptr_serv_table PTR_SERV_TABLE;

#define SERV_TABLE_LEN 12 // 一个该结构体占 12 个字节


// 描述一个在共享数据区中的 sys 服务的表项
typedef struct serv_para
{
  WORD OPT_CODE;  // 操作码
  WORD PARA_BASE; // 参数列表的基地址
  WORD PARA_NUM;  // 参数的数量
}SERV_PARA, *ptr_serv_para;

// 指向当前要执行的 sys 服务参数表项
ptr_serv_para PTR_SERV_PARA;

#define SERV_PARA_LEN 12 // 一个该结构体占 12 个字节


// 描述一个 sys 服务
typedef struct serv
{
  WORD idx;
  int status;
}SERV, *ptr_serv;

// 注册了所有的 sys 服务的全局表
SERV SERV_GLOBAL_TABLE[SERV_MAX_NUM];

// 指向当前需要执行的 sys 服务
WORD SERV_IDX;

// 表示未定义的 sys 服务索引，通常表示当前没有 sys 服务需要执行
#define SERV_UND_IDX 100


// 表示 sys 是否需要申请cpu
int SERV_IS_APPLY;

#define SERV_APPLY 1
#define SERV_UNAPPLY 0


// 记录最近发生错误的错误码
int SERV_ERR_CODE;

// 各种错误的定义
#define SERV_ERR_UND_IDX 1


extern void serv_setup();

extern WORD serv_run();

extern WORD serv_register_serv(WORD code_seg_base, WORD code_seg_limit, WORD para_seg_base);

extern void serv_send_para(WORD operation_code, WORD para_base, WORD para_num);

extern void serv_send_para_and_idx(WORD serv_idx, WORD operation_code, WORD para_base, WORD para_num);

extern void __SERV_S_jmp_to_serv(WORD serv_code_seg_base); // 定义于 sys.S


// sys 服务中操作码的定义，这里的操作码都是规定好的，所有的 sys 服务都必须包含这些操作码
#define SERV_RETURN_OPT 0x0


// Uart 服务注册在表中的相关信息
#define SERV_UART_CODE_SEG_BASE 0x31200000
#define SERV_UART_CODE_SEG_LIMIT 0x00004000
#define SERV_UART_PARA_SEG_BASE 0x31E00000


#endif
