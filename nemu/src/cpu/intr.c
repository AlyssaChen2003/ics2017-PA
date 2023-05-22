#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
    // 获取门描述符
    vaddr_t gate_addr = cpu.idtr.base + 8 * NO;

    // P 位校验
    if (cpu.idtr.limit < 0) {
        assert(0);
    }

    // 将 EFLAGS、CS、返回地址压栈
    uint32_t t0 = cpu.cs;  // cpu.cs 只有 16 位，需要转换成 32 位
    rtl_push(&cpu.eflags);
    rtl_push(&t0);
    rtl_push(&ret_addr);

    // 组合中断处理程序入口点
    uint32_t high, low;
    low = vaddr_read(gate_addr, 4) & 0xffff;
    high = vaddr_read(gate_addr + 4, 4) & 0xffff0000;

    // 设置 eip 跳转
    decoding.jmp_eip = high | low;
    decoding.is_jmp = true;
    // 注意：这里直接跳转到 eip，需要在调用 raise_intr 函数之后再执行 decode 和 execute
}

void dev_raise_intr() {
  cpu.INTR = true;
}
