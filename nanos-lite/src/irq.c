#include "common.h"
extern _RegSet* do_syscall(_RegSet *r) ;
extern _RegSet* schedule(_RegSet *prev);
static _RegSet* do_event(_Event e, _RegSet* r) {
  switch (e.event) {
    case _EVENT_SYSCALL:
      //Log("syscall event!!!11111111111");
      do_syscall(r);
      //return schedule(r);//返回新的进程的tf
      break;
    case(_EVENT_TRAP):
      Log("self-trapped event!\n");
      return schedule(r);//返回新的进程的tf
    case(_EVENT_IRQ_TIME):
      Log("IRQ TIME event!\n");
      return schedule(r);//返回新的进程的tf
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
