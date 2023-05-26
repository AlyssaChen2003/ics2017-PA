#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  rtl_push(eip);
  // printf("eip:%d, id_dest->val:%d\n",*eip,id_dest->val);
  rtl_addi(&decoding.jmp_eip,eip,id_dest->val);//decode.h #define id_dest (&decoding.dest)
  decoding.is_jmp = 1;

  print_asm("call %x", decoding.jmp_eip);
}


make_EHelper(ret) {
  rtl_pop(&decoding.jmp_eip);
  decoding.is_jmp = 1;
  print_asm("ret");
}
make_EHelper(reti) {
  rtl_pop(&decoding.jmp_eip);
  decoding.is_jmp = 1;
  cpu.esp+=id_dest->val;
  print_asm("ret");
}

make_EHelper(call_rm) {
  rtl_push(eip);
  decoding.jmp_eip=id_dest->val;
  decoding.is_jmp = 1;

  print_asm("call *%s", id_dest->str);
}
