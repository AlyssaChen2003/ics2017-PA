#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);


// arith.c
make_EHelper(add);
make_EHelper(sub);
make_EHelper(cmp);
make_EHelper(inc);
make_EHelper(dec);
make_EHelper(neg);
make_EHelper(adc);
make_EHelper(sbb);
make_EHelper(mul);
make_EHelper(imul1);
make_EHelper(imul2);
make_EHelper(imul3);
make_EHelper(div);
make_EHelper(idiv);

// logic.c
make_EHelper(test);
make_EHelper(xor);
make_EHelper(or);
make_EHelper(and);
make_EHelper(sar);
make_EHelper(shl);
make_EHelper(shr);
make_EHelper(rol);
make_EHelper(setcc);
make_EHelper(not);

// control.c
make_EHelper(call);
make_EHelper(jmp);
make_EHelper(jcc);
make_EHelper(jmp_rm);
make_EHelper(ret);
make_EHelper(reti);
make_EHelper(call_rm);


// data-mov.c
make_EHelper(mov);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(pusha);
make_EHelper(popa);
make_EHelper(leave);
make_EHelper(cltd);
make_EHelper(cwtl);
make_EHelper(movsx);
//make_EHelper(movsxy)
make_EHelper(movzx);
make_EHelper(lea);

// special.c
make_EHelper(nop);

// system.c
make_EHelper(lidt);
make_EHelper(mov_r2cr);
make_EHelper(mov_cr2r);
make_EHelper(int);
make_EHelper(iret);
make_EHelper(in);
make_EHelper(out);