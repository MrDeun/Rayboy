#include "../include/all.hpp"

extern cpu_context ctx;

void cpu_set_reg(reg_type rt, uint16_t value) {
  switch (rt) {
  case RT_NONE:
    break;
  case RT_A:
    ctx.regs.A = value & 0x00FF;
    break;
  case RT_F:
    ctx.regs.F = value & 0x00FF;
    break;
  case RT_B:
    ctx.regs.B = value & 0x00FF;
    break;
  case RT_C:
    ctx.regs.C = value & 0x00FF;
    break;
  case RT_D:
    ctx.regs.D = value & 0x00FF;
    break;
  case RT_E:
    ctx.regs.E = value & 0x00FF;
    break;
  case RT_H:
    ctx.regs.H = value & 0x00FF;
    break;
  case RT_L:
    ctx.regs.L = value & 0x00FF;
    break;
  case RT_SP:
    ctx.regs.SP = value;
  case RT_PC:
    ctx.regs.PC = value;
  case RT_AF:
    ctx.regs.AF = value;
  case RT_BC:
    ctx.regs.BC = value;
  case RT_DE:
    ctx.regs.DE = value;
  case RT_HL:
    ctx.regs.HL = value;
  }
}

uint16_t cpu_read_reg(reg_type rt) {
  switch (rt) {
  case RT_A:
    return ctx.regs.A;
  case RT_F:
    return ctx.regs.F;
  case RT_B:
    return ctx.regs.B;
  case RT_C:
    return ctx.regs.C;
  case RT_D:
    return ctx.regs.D;
  case RT_E:
    return ctx.regs.E;
  case RT_H:
    return ctx.regs.H;
  case RT_L:
    return ctx.regs.L;
  case RT_SP:
    return ctx.regs.SP;
  case RT_PC:
    return ctx.regs.PC;
  case RT_AF:
    return ctx.regs.AF;
  case RT_BC:
    return ctx.regs.BC;
  case RT_DE:
    return ctx.regs.DE;
  case RT_HL:
    return ctx.regs.HL;
  default:
    return 0;
  }
}