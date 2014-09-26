#include "pm.h"

// pm 的错误处理函数
static void pm_err_handler();

// 开始运行 pm 中拿到令牌的进程
static void pm_run();

// 得到 sys 进程是否申请 CPU
static int pm_get_sys_is_apply( WORD sys_idx );

// 添加一个进程项到全局进程表中
static void pm_add_to_pm_table(WORD proc_idx, PTRFUNC proc_setup);

// 查看 initd 中是否有应用程序进程
static int pm_initd_is_empty();


// 为操作系统安装 pm，并将其初始化
void pm_setup()
{
  WORD proc_idx;

  static PTRFUNC ptr_initd_setup = initd_setup;
  static PTRFUNC ptr_sys_setup = sys_setup;
  static PTRFUNC ptr_mm_setup = mm_setup;
  static PTRFUNC ptr_cpum_setup = cpum_setup;


  // 生成全局进程表
  pm_add_to_pm_table(PM_INITD_IDX, ptr_initd_setup);
  pm_ptr_initd = &PM_TABLE[PM_INITD_IDX]; // 为全局指针赋值，方便其他会用到该指针的地方

  pm_add_to_pm_table(PM_SYS_IDX, ptr_sys_setup);
  pm_ptr_sys = &PM_TABLE[PM_SYS_IDX];

  pm_add_to_pm_table(PM_MM_IDX, ptr_mm_setup);
  pm_ptr_mm = &PM_TABLE[PM_MM_IDX];

  pm_add_to_pm_table(PM_CPUM_IDX, ptr_cpum_setup);
  pm_ptr_cpum = &PM_TABLE[PM_CPUM_IDX];

  
  // 为每一个进程执行他们自己的初始化函数
  for(proc_idx=0 ; proc_idx<PM_PROC_NUM ; proc_idx++)
    {
      (*(PM_TABLE[proc_idx].proc_setup_func))();
      PM_TABLE[proc_idx].status = PM_PROC_STATUS_READY;
    }

}

/* pm 的调度算法
   如 initd 有任务，则将令牌给 initd
   如 sys 进程需要整理，则给该 sys 进程令牌
   如果都不满足，则让系统休眠
 */
void pm_scheduling()
{

  PM_TOKEN = PM_SLEEP;

  if((( pm_ptr_initd->status == PM_PROC_STATUS_READY ) && ( pm_initd_is_empty()!=INITD_EMPTY )) || ( pm_ptr_initd->status == PM_PROC_STATUS_RUNNING ))
    {
      PM_TOKEN = PM_INITD_IDX;
      Uart_SendString("INITD\n",6);
    }
  else if(( pm_get_sys_is_apply(PM_SYS_IDX)==SYS_APPLY ) && ( PM_TABLE[PM_SYS_IDX].status==PM_PROC_STATUS_READY ))
    {
      PM_TOKEN = PM_SYS_IDX;
      Uart_SendString("SYS\n",4);
    }
  else if(( pm_get_sys_is_apply(PM_MM_IDX)==MM_APPLY ) && ( PM_TABLE[PM_MM_IDX].status==PM_PROC_STATUS_READY ))
    {
      PM_TOKEN = PM_MM_IDX;
      Uart_SendString("MM\n",3);
    }
 
 
  PM_TABLE[PM_TOKEN].status = PM_PROC_STATUS_RUNNING;
  pm_run();
  PM_TABLE[PM_TOKEN].status = PM_PROC_STATUS_READY;
}


static void pm_run()
{
  switch( PM_TOKEN )
    {
    case PM_INITD_IDX:
      initd_run();
      break;
    case PM_SYS_IDX:
      sys_run();
      break;
    case PM_MM_IDX:
      mm_run();
      break;
    case PM_CPUM_IDX:
      cpum_run();
      break;
    case PM_SLEEP:
      break;
    default:
      PM_ERR_CODE = PM_ERR_UND_TOKEN;
      pm_err_handler();
      break;
    }
  
}

static void pm_add_to_pm_table(WORD proc_idx, PTRFUNC proc_setup)
{
  ptr_pm_node pm_node_tmp = &PM_TABLE[proc_idx];

  pm_node_tmp->idx = proc_idx;

  pm_node_tmp->proc_setup_func = proc_setup;

  pm_node_tmp->status = PM_PROC_STATUS_UNREADY;
}

static int pm_initd_is_empty()
{
  return INITD_FILL_APP_NUM;
}

static int pm_get_sys_is_apply( WORD sys_idx )
{
  switch( sys_idx )
    {
    case PM_SYS_IDX:
      return SYS_IS_APPLY;
    case PM_MM_IDX:
      return MM_IS_APPLY;
    default:
      PM_ERR_CODE = PM_ERR_UND_TOKEN;
      pm_err_handler();
      break;
    }
}

static void pm_err_handler()
{}


