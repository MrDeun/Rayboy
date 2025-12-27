#include "../include/all.hpp"
#include "fmt/core.h"
extern cpu_context ctx;

uint16_t reverse(uint16_t n) {
  return ((n & 0xFF00) >> 8) | ((n & 0x00FF) << 8);
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

  case RT_AF:
    return reverse(*((uint16_t *)&ctx.regs.A));
  case RT_BC:
    return reverse(*((uint16_t *)&ctx.regs.B));
  case RT_DE:
    return reverse(*((uint16_t *)&ctx.regs.D));
  case RT_HL:
    return reverse(*((uint16_t *)&ctx.regs.H));

  case RT_PC:
    return ctx.regs.PC;
  case RT_SP:
    return ctx.regs.SP;
  default:
    return 0;
  }
}

void cpu_set_reg(reg_type rt, uint16_t val) {
  switch (rt) {
  case RT_A:
    ctx.regs.A = val & 0xFF;
    break;
  case RT_F:
    ctx.regs.F = val & 0xFF;
    break;
  case RT_B:
    ctx.regs.B = val & 0xFF;
    break;
  case RT_C: {
    ctx.regs.C = val & 0xFF;
  } break;
  case RT_D:
    ctx.regs.D = val & 0xFF;
    break;
  case RT_E:
    ctx.regs.E = val & 0xFF;
    break;
  case RT_H:
    ctx.regs.H = val & 0xFF;
    break;
  case RT_L:
    ctx.regs.L = val & 0xFF;
    break;

  case RT_AF:
    *((uint16_t *)&ctx.regs.A) = reverse(val);
    break;
  case RT_BC:
    *((uint16_t *)&ctx.regs.B) = reverse(val);
    break;
  case RT_DE:
    *((uint16_t *)&ctx.regs.D) = reverse(val);
    break;
  case RT_HL: {
    *((uint16_t *)&ctx.regs.H) = reverse(val);
    break;
  }

  case RT_PC:
    ctx.regs.PC = val;
    break;
  case RT_SP:
    ctx.regs.SP = val;
    break;
  case RT_NONE:
    break;
  }
}

uint8_t cpu_read_reg8(reg_type rt) {
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
  case RT_HL:
    return bus_read(cpu_read_reg(RT_HL));

  default: {
    auto err = fmt::format("Illegal registry has been found: {}", (int)rt);
    ERROR(err.c_str());
  }
  }
}

void cpu_set_reg8(reg_type rt, uint8_t val) {
  switch (rt) {
  case RT_A:
    ctx.regs.A = val & 0xFF;
    break;
  case RT_F:
    ctx.regs.F = val & 0xFF;
    break;
  case RT_B:
    ctx.regs.B = val & 0xFF;
    break;
  case RT_C:
    ctx.regs.C = val & 0xFF;
    break;
  case RT_D:
    ctx.regs.D = val & 0xFF;
    break;
  case RT_E:
    ctx.regs.E = val & 0xFF;
    break;
  case RT_H:
    ctx.regs.H = val & 0xFF;
    break;
  case RT_L:
    ctx.regs.L = val & 0xFF;
    break;
  case RT_HL:
    bus_write(cpu_read_reg(RT_HL), val);
    break;
  default: {
    auto err = fmt::format("Illegal registry has been found: {}", (int)rt);
    ERROR(err.c_str());
  }
  }
}
