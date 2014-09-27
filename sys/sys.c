#include "sys.h"


static WORD sys_get_empty_idx()
{
  WORD serv_idx;
  
  for(serv_idx=0; serv_idx<SYS_SERV_MAX_NUM; serv_idx++)
    if( SYS_SERV_GLOBAL_TABLE[serv_idx].status == SYS_SERV_STATUS_EMPTY )
      break;

  return serv_idx;
};


static void sys_add_serv(WORD serv_idx, WORD code_seg_base, WORD code_seg_limit, WORD para_seg_base)
{
  // 得到索引指向的 sys 表项
  PTR_SYS_SERV_TABLE = (ptr_sys_serv_table)(SYS_REGISTER_TABLE_BASE + serv_idx * SYS_SERV_TABLE_LEN);

  PTR_SYS_SERV_TABLE -> CODE_SEG_BASE = code_seg_base;
  PTR_SYS_SERV_TABLE -> CODE_SEG_LIMIT = code_seg_limit;
  PTR_SYS_SERV_TABLE -> PARA_SEG_BASE = para_seg_base;

  SYS_SERV_GLOBAL_TABLE[serv_idx].status = SYS_SERV_STATUS_READY;
};


static WORD sys_recv_result();


/* 初始化 sys 这项服务 */

void sys_setup()
{
  int i;
  WORD serv_idx;


  Uart_SendString("sys setup.\n",11);


  // 初始化 sys 服务的全局表
  for(i=0; i<SYS_SERV_MAX_NUM; i++)
    {
      SYS_SERV_GLOBAL_TABLE[i].idx = i;
      SYS_SERV_GLOBAL_TABLE[i].status = SYS_SERV_STATUS_EMPTY;
    }

  // 开始注册所有的 sys 服务，相关的宏参数都定义在头文件中
  serv_idx = sys_register_serv(SERV_UART_CODE_SEG_BASE, SERV_UART_CODE_SEG_LIMIT, SERV_UART_PARA_SEG_BASE); // 注册串口服务

  // 初始化 sys 不需要申请 CPU
  SYS_IS_APPLY = SYS_UNAPPLY;
  SYS_SERV_IDX = SYS_SERV_UND_IDX;
}


/* 运行 sys 服务 */

WORD sys_run()
{
  WORD sys_serv_code_seg_base;
  WORD result;

  if( SYS_SERV_IDX == SYS_SERV_UND_IDX )
    return;

  // 利用 SYS_SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SYS_SERV_TABLE = (ptr_sys_serv_table)(SYS_REGISTER_TABLE_BASE + SYS_SERV_IDX * SYS_SERV_TABLE_LEN);
  sys_serv_code_seg_base = PTR_SYS_SERV_TABLE -> CODE_SEG_BASE;
  
  // 下面这个函数定义在 sys.S 中
  sys_jmp_to_serv_asm(sys_serv_code_seg_base);

  result = sys_recv_result();

  // 执行完毕，为下面两个参数复值
  SYS_IS_APPLY = SYS_UNAPPLY;
  SYS_SERV_IDX = SYS_SERV_UND_IDX;

  return result;
}


WORD sys_register_serv(WORD code_seg_base, WORD code_seg_limit, WORD para_seg_base)
{
  WORD serv_idx;

  serv_idx = sys_get_empty_idx();

  sys_add_serv(serv_idx, code_seg_base, code_seg_limit, para_seg_base);

  return serv_idx;
}


void sys_send_para(WORD opt_code, WORD para_base, WORD para_num)
{
  // 利用 SYS_SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SYS_SERV_TABLE = (ptr_sys_serv_table)(SYS_REGISTER_TABLE_BASE + SYS_SERV_IDX * SYS_SERV_TABLE_LEN);
  PTR_SYS_SERV_PARA = (ptr_sys_serv_para)(PTR_SYS_SERV_TABLE -> PARA_SEG_BASE);

  PTR_SYS_SERV_PARA -> OPT_CODE = opt_code;
  PTR_SYS_SERV_PARA -> PARA_BASE = para_base;
  PTR_SYS_SERV_PARA -> PARA_NUM =para_num;
}


WORD sys_recv_result()
{
  WORD opt_code;
  WORD result;

  // 利用 SYS_SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SYS_SERV_TABLE = (ptr_sys_serv_table)(SYS_REGISTER_TABLE_BASE + SYS_SERV_IDX * SYS_SERV_TABLE_LEN);
  PTR_SYS_SERV_PARA = (ptr_sys_serv_para)(PTR_SYS_SERV_TABLE -> PARA_SEG_BASE);

  opt_code = PTR_SYS_SERV_PARA -> OPT_CODE;

  if(opt_code == SYS_SERV_RETURN_OPT)
    {
      result = PTR_SYS_SERV_PARA -> PARA_BASE;
      return result;
    }

  return NULL;
}
