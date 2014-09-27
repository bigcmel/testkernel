#include "../serv_global.h"
#include "serv_uart.h"


static WORD serv_get_idx(WORD code_seg_base);
static void serv_choose_opt(WORD opt_code, WORD* para_list, WORD para_num);

void main()
{
  // 获取该服务在 sys 服务表中的索引
  WORD serv_idx;

  // 从共享内存中获取的参数
  WORD* para_seg_list;
  WORD opt_code;
  WORD para_base;
  WORD para_num;
  WORD* para_list;


  // 不仅获得索引，还得到了全局变量 CODE_SEG_LIMIT 和 PARA_SEG_BASE 的值
  serv_idx = serv_get_idx(CODE_SEG_BASE);


  // 获得所有需要的参数值
  para_seg_list = (WORD*)PARA_SEG_BASE;

  opt_code = *para_seg_list;
  para_base = *(para_seg_list + 1);
  para_num = *(para_seg_list + 2);

  para_list = (WORD*)para_base;

  
  // 下面就是根据 opt_code 选择 sys 服务中的一个函数，并把 para_list 和 para_num 作为统一参数传递给该函数
  serv_choose_opt(opt_code, para_list, para_num);

  return;
}


WORD serv_get_idx(WORD code_seg_base)
{
  //  这里还要对全局变量 CODE_SEG_LIMIT 和 PARA_SEG_BASE 赋值
  


  // 在获得了全局变量 PARA_SEG_BASE 后，就可以得到要存储操作码的地址 OPT_CODE_BASE 和要存储返回值的地址 RETURN_CODE_BASE
  OPT_CODE_BASE = PARA_SEG_BASE;
  RETURN_CODE_BASE = PARA_SEG_BASE + 4;
}


void serv_choose_opt(WORD opt_code, WORD* para_list, WORD para_num)
{
  switch( opt_code )
    {
    case SERV_UART_INIT:
      serv_uart_init();
      break;
    default:
      SERV_ERR_CODE = SERV_ERR_UND_OPT;
      break;
    }
}
