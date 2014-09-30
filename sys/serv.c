#include "serv.h"
#include "serv_funcs.h"


static WORD serv_get_empty_idx()
{
  WORD serv_idx;
  
  for(serv_idx=0; serv_idx<SERV_MAX_NUM; serv_idx++)
    if( SERV_GLOBAL_TABLE[serv_idx].status == SERV_STATUS_EMPTY )
      break;

  return serv_idx;
};


static void add_serv(WORD serv_idx, WORD code_seg_base, WORD code_seg_limit, WORD para_seg_base)
{
  // 得到索引指向的 sys 表项
  PTR_SERV_TABLE = (ptr_serv_table)(SERV_REGISTER_TABLE_BASE + serv_idx * SERV_TABLE_LEN);

  PTR_SERV_TABLE -> CODE_SEG_BASE = code_seg_base;
  PTR_SERV_TABLE -> CODE_SEG_LIMIT = code_seg_limit;
  PTR_SERV_TABLE -> PARA_SEG_BASE = para_seg_base;

  SERV_GLOBAL_TABLE[serv_idx].status = SERV_STATUS_READY;
};


static WORD serv_recv_result();


/* 初始化 sys 这项服务 */

void serv_setup()
{
  int i;


  Uart_SendString("serv setup.\n",12);


  // 初始化 sys 服务的全局表
  for(i=0; i<SERV_MAX_NUM; i++)
    {
      SERV_GLOBAL_TABLE[i].idx = i;
      SERV_GLOBAL_TABLE[i].status = SERV_STATUS_EMPTY;
    }

  // 开始注册所有的 sys 服务，相关的宏参数都定义在头文件中
  SERV_UART_IDX = serv_register_serv(SERV_UART_CODE_SEG_BASE, SERV_UART_CODE_SEG_LIMIT, SERV_UART_PARA_SEG_BASE); // 注册串口服务,SERV_UART_IDX 定义在 serv_funcs.h 中  


  // 初始化 sys 不需要申请 CPU
  SERV_IS_APPLY = SERV_UNAPPLY;
  SERV_IDX = SERV_UND_IDX;
}


/* 运行 sys 服务 */

WORD serv_run()
{
  WORD serv_code_seg_base;
  WORD result;

  if( SERV_IDX == SERV_UND_IDX )
    return;

  // 利用 SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SERV_TABLE = (ptr_serv_table)(SERV_REGISTER_TABLE_BASE + SERV_IDX * SERV_TABLE_LEN);
  serv_code_seg_base = PTR_SERV_TABLE -> CODE_SEG_BASE;
  
  // 下面这个函数定义在 serv.S 中
  __SERV_S_jmp_to_serv(serv_code_seg_base);

  result = serv_recv_result();

  // 执行完毕，为下面两个参数复值
  SERV_IS_APPLY = SERV_UNAPPLY;
  SERV_IDX = SERV_UND_IDX;

  return result;
}


WORD serv_register_serv(WORD code_seg_base, WORD code_seg_limit, WORD para_seg_base)
{
  WORD serv_idx;

  serv_idx = serv_get_empty_idx();

  add_serv(serv_idx, code_seg_base, code_seg_limit, para_seg_base);

  return serv_idx;
}


void serv_send_para(WORD opt_code, WORD para_base, WORD para_num)
{
  // 利用 SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SERV_TABLE = (ptr_serv_table)(SERV_REGISTER_TABLE_BASE + SERV_IDX * SERV_TABLE_LEN);
  PTR_SERV_PARA = (ptr_serv_para)(PTR_SERV_TABLE -> PARA_SEG_BASE);

  PTR_SERV_PARA -> OPT_CODE = opt_code;
  PTR_SERV_PARA -> PARA_BASE = para_base;
  PTR_SERV_PARA -> PARA_NUM =para_num;
}


WORD serv_recv_result()
{
  WORD opt_code;
  WORD result;

  // 利用 SERV_IDX 参数来确定要执行哪个 sys 服务
  PTR_SERV_TABLE = (ptr_serv_table)(SERV_REGISTER_TABLE_BASE + SERV_IDX * SERV_TABLE_LEN);
  PTR_SERV_PARA = (ptr_serv_para)(PTR_SERV_TABLE -> PARA_SEG_BASE);

  opt_code = PTR_SERV_PARA -> OPT_CODE;

  if(opt_code == SERV_RETURN_OPT)
    {
      result = PTR_SERV_PARA -> PARA_BASE;
      return result;
    }

  return NULL;
}


void serv_send_para_and_idx(WORD serv_idx, WORD opt_code, WORD para_base, WORD para_num)
{
  SERV_IDX = serv_idx;

  serv_send_para(opt_code, para_base, para_num);
}
