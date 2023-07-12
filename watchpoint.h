#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
    int NO;         // 监视点编号
    int hitNum;     // 触发次数
    char expr[32];     // 监视表达式
    int result;   // 原始值
    struct watchpoint *next;  // 指向下一个监视点的指针
    uint32_t expr_record_val;
  

  /* TODO: Add more members if necessary */


} WP;
void info_watchpoint();

#endif
