#include "sys_mm.h"


// 对当前的内存分配做出整理，优化，比如合并碎片空间，腾出整块儿空间啥的
static void sys_mm_optimize();

// 初始化描述 sdram 中每M空间的使用情况的表，该表也反映了物理地址到虚拟地址的映射
static void sys_mm_init_sdram_table();

// 判断 ttb 中的一个表项所对应的虚拟空间是否有效
static inline WORD sys_mm_the_section_is_avaliable(WORD ttb_tmp);

// 标记 sdram_table 中的一个表项所对应的物理地址为已用
static inline void sys_mm_mark_the_one_M(WORD sdram_table_idx);

// 根据 ttb 表中的信息得到该表项对应的物理地址
static inline WORD sys_mm_get_sdram_table_idx( WORD ttb_tmp );


/* 初始化 sys_mm 这项服务 */

void sys_mm_setup()
{
  /* 初始化记录物理内存地址使用状况的全局表 */
  sys_mm_init_sdram_table();


  // 初始化 sys_mm 不需要申请 CPU
  SYS_MM_IS_APPLY = SYS_MM_UNAPPLY;

}


/* 运行一次 sys_mm 服务 */

void sys_mm_run()
{
  // 姑且就先只做一次内存的管理，优化，有新功能以后再加
  sys_mm_optimize();

}


/* 建立页表，建立虚拟地址到物理地址的映射，
   pagetable_base_addr 参数是指向页表基址的指针，
   该地址将被放置在 CP15 的 C2 寄存器中 */

void sys_mm_setup_pagetable( BYTE* pagetable_base_addr )
{
  WORD ttb_base = (WORD)pagetable_base_addr;

  __asm__
    (
     "MOV R0, #0\n"
     "MCR P15, 0, R0, C7, C7, 0\n" //p205 使无效指令cache和数据cache      
     "MCR P15, 0, R0, C7, C10, 4\n" //p205 清空写缓冲区
     "MCR P15, 0, R0, C8, C7, 0\n" //p189 使无效快表内容

     "MOV R4, %0\n" // 读取页表的基地址到 R4
     "MCR P15, 0, R4, C2, C0, 0\n" // C2 中存储着页表的基地址

     "MCR P15, 0, R0, C1, C0, 0\n": /* 汇编语句部分结束 */ : /* 无输出部分 */
     "r"( ttb_base ) // 输入部分，传入一个参数，以占位符'%0'表示
     );
}


/* 其他应用程序通过调用此函数来向 sys_mm 服务申请内存空间，
   app_idx 为该应用程序在表中的索引值，
   vir_addr 为要申请的地址空间的基地址
   length 为要申请的地址空间的长度 */

WORD sys_mm_apply_sdram( WORD app_idx, WORD vir_addr, WORD length )
{
  WORD ttb_idx;
  WORD ttb_num;
  WORD ttb_tmp;
  WORD sdram_table_idx;

  WORD i;


  if( ! (vir_addr >> 25) ) // 如果虚拟地址小于 32M，就会牵扯切换上下文的因素
    {
      vir_addr += app_idx * 0x02000000;
    }

  ttb_idx = vir_addr >> 20;
  ttb_num = length >> 20;
  
  for(i=0 ; i<ttb_num ; i++)
    {

      ttb_tmp = SYS_MM_PAGETABLE[ttb_idx];
  
      if( sys_mm_the_section_is_avaliable(ttb_tmp) )
	{
	  sdram_table_idx = sys_mm_get_sdram_table_idx(ttb_tmp);

	  sys_mm_mark_the_one_M(sdram_table_idx);
	}

      ttb_idx++;
    }

  return RETURN_SUCCESS;
}


static void sys_mm_optimize()
{}

static void sys_mm_init_sdram_table()
{
  WORD* ttb_ptr;
  WORD ttb_tmp;
  HWORD sdram_table_tmp;

  WORD ttb_idx;
  WORD sdram_table_idx;


  /* 建立该表与页表之间的映射 */

  SYS_MM_PAGETABLE = (WORD*)SYS_MM_VIR_TTB_BASE; // 初始化指向系统页表的全局变量

  ttb_ptr = SYS_MM_PAGETABLE;

  for(ttb_idx=0 ; ttb_idx<SYS_MM_VIR_SIZE ; ttb_idx++)
    {
      ttb_tmp = *ttb_ptr;

      if( sys_mm_the_section_is_avaliable( ttb_tmp ) )
	{
	  sdram_table_idx = sys_mm_get_sdram_table_idx(ttb_tmp);
	  
	  sdram_table_tmp = ( ttb_idx << 4 );

	  if( sdram_table_idx < SYS_MM_SDRAM_SIZE )
	    SYS_MM_SDRAM_TABLE[sdram_table_idx] = sdram_table_tmp;
	}

      ttb_ptr++;
    }


  /* 初始化物理地址中哪些地址已被使用，
     到此为止只有内核占用了内存，所以指定这些地址为已用，
     内核占用的内存，是 sdram 开头的2M */

  for(sdram_table_idx=0; sdram_table_idx<32; sdram_table_idx++)
    sys_mm_mark_the_one_M(sdram_table_idx);

}

static inline WORD sys_mm_the_section_is_avaliable(WORD ttb_tmp)
{
  if(( ttb_tmp & 0x3 ) == 0x2) // 判断是否是段描述符
    return RETURN_SUCCESS;
  else
    return RETURN_FAILED;
}

static inline void sys_mm_mark_the_one_M(WORD sdram_table_idx)
{
  SYS_MM_SDRAM_TABLE[sdram_table_idx] |= 0x1;
}

static inline WORD sys_mm_get_sdram_table_idx( WORD ttb_tmp )
{
  return ((ttb_tmp >> 20) - (SYS_MM_PHY_SDRAM_BASE >> 20));
}
