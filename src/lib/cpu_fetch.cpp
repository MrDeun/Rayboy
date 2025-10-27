#include "../include/all.hpp"

extern cpu_context ctx;

void fetch_data() {
  ctx.mem_destination = 0;
  ctx.dest_is_mem = false;

  switch (ctx.cur_instruction->mode) {
  case AM_IMP:
    return;
  case AM_R:
    ctx.fetch_data = cpu_read_reg(ctx.cur_instruction->reg1);
    return;
  case AM_R_D8:
    ctx.fetch_data = bus_read(ctx.regs.PC);
    emu_cycles(1);
    ctx.regs.PC++;
    return;
  case AM_D16: {
    uint16_t low = bus_read(ctx.regs.PC);
    emu_cycles(1);
    uint16_t high = bus_read(ctx.regs.PC + 1);
    emu_cycles(1);
    ctx.fetch_data = low | (high << 8);
    ctx.regs.PC += 2;
    return;
  }

  default:
    fmt::println("Unknown Addressing mode!");
    exit(-7);
  }
}
