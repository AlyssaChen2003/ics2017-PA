#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_push(&id_dest->val);

  print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest, &t0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  t0 = cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);

  print_asm("popa");
}


make_EHelper(leave) {
  rtl_sr(4, 4, &cpu.ebp);
  rtl_pop(&cpu.ebp);

  print_asm("leave");
}

void rtl_msb_helper(rtlreg_t* dest, const rtlreg_t* src, int width) {
  rtl_msb(dest, src, width);
  *dest = (*dest == 1 ? -1 : 0);
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_msb_helper(&t0, &cpu.eax, 2);
    cpu.edx = (t0 == -1 ? -1 : 0);
  } else {
    rtl_msb_helper(&t0, &cpu.eax, 4);
    cpu.edx = (t0 == -1 ? -1 : 0);
  }
  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}


make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {

    rtl_sext(&t0, &cpu.eax, 1);
    cpu.eax = (cpu.eax & 0xffff0000) | (t0 & 0xffff);
  }
  else {
    rtl_sext(&cpu.eax, &cpu.eax, 2);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
