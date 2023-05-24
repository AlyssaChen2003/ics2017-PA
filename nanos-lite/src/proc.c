#include "proc.h"

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC];
static int nr_proc = 0;
PCB *current = NULL;

uintptr_t loader(_Protect *as, const char *filename);

void load_prog(const char *filename) {
  int i = nr_proc ++;
  _protect(&pcb[i].as);

  uintptr_t entry = loader(&pcb[i].as, filename);

  // TODO: remove the following three lines after you have implemented _umake()
  // _switch(&pcb[i].as);
  // current = &pcb[i];
  // ((void (*)(void))entry)();

  _Area stack;
  stack.start = pcb[i].stack;
  stack.end = stack.start + sizeof(pcb[i].stack);

  pcb[i].tf = _umake(&pcb[i].as, stack, stack, (void *)entry, NULL, NULL);
}
// static uint32_t count = 1;
// extern int current_game ;
//进程调度
_RegSet* schedule(_RegSet *prev) {
  // return NULL;
  if(current!=NULL){//current:当前进程的PCB指针
    current->tf=prev;//保存tf
  }
  // current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
  // Log("ptr=0x%x\n",(uintptr_t)current->as.ptr);
  else{
    current=&pcb[0];//初始进程为0号进程
  }
  static int num=0;
  static const int freq=1000;
  if(current==&pcb[0]){
    num++;
  }
  else{
    current=&pcb[0];
  }
  if(num==freq){//如果到达1000次，则切换成进程1
    current=&pcb[1];
    num=0;
  }
  _switch(&current->as);//切换虚拟地址空间
  return current->tf;
}