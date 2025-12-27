#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <cstdio>

void cpu_set_flags(cpu_context *ctx, int8_t z, int8_t n, int8_t h, int8_t c) {
  if (z != -1) {
    BIT_SET(ctx->regs.F, 7, z);
  }
  if (n != -1) {
    BIT_SET(ctx->regs.F, 6, n);
  }
  if (h != -1) {
    BIT_SET(ctx->regs.F, 5, h);
  }
  if (c != -1) {
    BIT_SET(ctx->regs.F, 4, c);
  }
}

static bool is_16bit(reg_type rt) {
  switch (rt) {
  case RT_A:
  case RT_F:
  case RT_B:
  case RT_C:
  case RT_D:
  case RT_E:
  case RT_H:
  case RT_L:
    return false;
  case RT_HL:
  case RT_AF:
  case RT_BC:
  case RT_DE:
  case RT_SP:
  case RT_PC:
    return true;
  default: {
    auto err = fmt::format("Illegal State in is_16bit({})", (int)rt);
    ERROR(err.c_str());
  }
  }
}

static bool check_cond(cpu_context *ctx) {
  bool z = CPU_FLAG_Z;
  bool c = CPU_FLAG_C;

  switch (ctx->cur_instruction->cond) {
  case CT_NONE:
    return true;
  case CT_NZ:
    return !z;
  case CT_Z:
    return z;
  case CT_NC:
    return !c;
  case CT_C:
    return c;
  }

  return false;
}
static void goto_addr(cpu_context *ctx, uint16_t address, bool pushPC) {
  if (check_cond(ctx)) {
    if (pushPC) {
      stack_push16(ctx->regs.PC);
      emu_cycles(2);
    }

    ctx->regs.PC = ctx->fetch_data;
    emu_cycles(1);
  }
}

static void proc_and(cpu_context *ctx) {
  ctx->regs.A &= ctx->fetch_data;
  cpu_set_flags(ctx, ctx->regs.A == 0, 0, 1, 0);
}
static void proc_xor(cpu_context *ctx) {
  ctx->regs.A ^= ctx->fetch_data & 0xff;
  cpu_set_flags(ctx, ctx->regs.A == 0, 0, 0, 0);
}
static void proc_or(cpu_context *ctx) {
  ctx->regs.A |= ctx->fetch_data & 0xff;
  cpu_set_flags(ctx, ctx->regs.A == 0, 0, 0, 0);
}
static void proc_cp(cpu_context *ctx) {
  int n = (int)ctx->regs.A - (int)ctx->fetch_data;
  cpu_set_flags(ctx, n == 0, 1,
                ((int)ctx->regs.A & 0x0f) - ((int)ctx->fetch_data & 0x0f) < 0,
                n < 0);
}

static void proc_add(cpu_context *ctx) {
  uint32_t val = cpu_read_reg(ctx->cur_instruction->reg1) + ctx->fetch_data;
  bool is16bit = is_16bit(ctx->cur_instruction->reg1);
  if (is16bit) {
    emu_cycles(1);
  }

  if (ctx->cur_instruction->reg1 == RT_SP) {
    val = cpu_read_reg(ctx->cur_instruction->reg1) + (char)ctx->fetch_data;
  }

  int z = (val & 0xff) == 0;
  int h = (cpu_read_reg(ctx->cur_instruction->reg1) & 0xf) +
              (ctx->fetch_data & 0xf) >=
          0x10;
  int c = (int)(cpu_read_reg(ctx->cur_instruction->reg1) & 0xff) +
              (int)(ctx->fetch_data & 0xff) >=
          0x100;

  if (is16bit) {
    z = -1;
    h = (cpu_read_reg(ctx->cur_instruction->reg1) & 0xfff) +
            (ctx->fetch_data & 0xfff) >
        0x1000;
    uint32_t n = ((uint32_t)cpu_read_reg(ctx->cur_instruction->reg1)) +
                 ((uint32_t)ctx->fetch_data);
    c = n >= 0x10000;
  }

  if (ctx->cur_instruction->reg1 == RT_SP) {
    z = 0;
    h = (cpu_read_reg(ctx->cur_instruction->reg1) & 0xf) +
            (ctx->fetch_data & 0xf) >=
        0x10;
    c = (int)(cpu_read_reg(ctx->cur_instruction->reg1) & 0xff) +
            (int)(ctx->fetch_data & 0xff) >=
        0x100;
  }

  cpu_set_reg(ctx->cur_instruction->reg1, val);
  cpu_set_flags(ctx, z, 0, h, c);
}
static void proc_sub(cpu_context *ctx) {
  uint16_t val = cpu_read_reg(ctx->cur_instruction->reg1) - ctx->fetch_data;
  int z = val == 0;
  int h = ((int)cpu_read_reg(ctx->cur_instruction->reg1) & 0xf) -
              ((int)ctx->fetch_data & 0xf) <
          0;
  int c =
      ((int)cpu_read_reg(ctx->cur_instruction->reg1)) - ((int)ctx->fetch_data) <
      0;

  cpu_set_reg(ctx->cur_instruction->reg1, val);
  cpu_set_flags(ctx, z, 1, h, c);
}
static void proc_sbc(cpu_context *ctx) {
  uint16_t val = CPU_FLAG_C + ctx->fetch_data;
  int z = cpu_read_reg(ctx->cur_instruction->reg1) == 0;
  int h = ((int)cpu_read_reg(ctx->cur_instruction->reg1) & 0xf) -
              ((int)ctx->fetch_data & 0xf) - (int)CPU_FLAG_C <
          0;
  int c = ((int)cpu_read_reg(ctx->cur_instruction->reg1)) -
              ((int)ctx->fetch_data) - (int)CPU_FLAG_C <
          0;

  cpu_set_reg(ctx->cur_instruction->reg1,
              cpu_read_reg(ctx->cur_instruction->reg1) - val);
  cpu_set_flags(ctx, z, 1, h, c);
}
static void proc_adc(cpu_context *ctx) {
  uint16_t u = ctx->fetch_data;
  uint16_t a = ctx->regs.A;
  uint16_t c = CPU_FLAG_C;

  ctx->regs.A = (a + u + c) & 0xFF;

  cpu_set_flags(ctx, ctx->regs.A == 0, 0, (a & 0xF) + (u & 0xF) + c > 0xF,
                a + u + c > 0xFF);
}

static void proc_none(cpu_context *ctx) {
  ERROR("Tried to fetch illegal processor for IN_NONE!");
}
static void proc_dec(cpu_context *ctx) {
  uint16_t val = cpu_read_reg(ctx->cur_instruction->reg1) - 1;

  if (is_16bit(ctx->cur_instruction->reg1)) {
      emu_cycles(1);
  }

  if (ctx->cur_instruction->reg1 == RT_HL && ctx->cur_instruction->mode == AM_MR) {
      val = bus_read(cpu_read_reg(RT_HL)) - 1;
      bus_write(cpu_read_reg(RT_HL), val);
  } else {
      cpu_set_reg(ctx->cur_instruction->reg1, val);
      val = cpu_read_reg(ctx->cur_instruction->reg1);
  }

  if ((ctx->op_code & 0x0B) == 0x0B) {
      return;
  }

  cpu_set_flags(ctx, val == 0, 1, (val & 0x0F) == 0x0F, -1);
}
static void proc_inc(cpu_context *ctx) {
  uint16_t val = cpu_read_reg(ctx->cur_instruction->reg1) + 1;

  if (is_16bit(ctx->cur_instruction->reg1)) {
      emu_cycles(1);
  }

  if (ctx->cur_instruction->reg1 == RT_HL && ctx->cur_instruction->mode == AM_MR) {
      val = bus_read(cpu_read_reg(RT_HL)) + 1;
      val &= 0xFF;
      bus_write(cpu_read_reg(RT_HL), val);
  } else {
      cpu_set_reg(ctx->cur_instruction->reg1, val);
      val = cpu_read_reg(ctx->cur_instruction->reg1);
  }

  if ((ctx->op_code & 0x03) == 0x03) {
      return;
  }

  cpu_set_flags(ctx, val == 0, 0, (val & 0x0F) == 0, -1);
}
reg_type rt_lookup[] = {RT_B, RT_C, RT_D, RT_E, RT_H, RT_L, RT_HL, RT_A};

reg_type decode_reg(uint8_t reg) {
  if (reg > 0b111)
    return RT_NONE;
  return rt_lookup[reg];
}

static void proc_cb(cpu_context *ctx) {
  uint8_t op = ctx->fetch_data;
  reg_type reg = decode_reg(op & 0b111);
  uint8_t bit = (op >> 3) & 0b111;
  uint8_t bit_op = (op >> 6) & 0b11;
  uint8_t reg_val = cpu_read_reg8(reg);

  emu_cycles(1);

  if (reg == RT_HL) {
    emu_cycles(2);
  }

  switch (bit_op) {
  case 1:
    // BIT
    cpu_set_flags(ctx, !(reg_val & (1 << bit)), 0, 1, -1);
    return;

  case 2:
    // RST
    reg_val &= ~(1 << bit);
    cpu_set_reg8(reg, reg_val);
    return;

  case 3:
    // SET
    reg_val |= (1 << bit);
    cpu_set_reg8(reg, reg_val);
    return;
  }

  bool flagC = CPU_FLAG_C;

  switch (bit) {
  case 0: {
    // RLC
    bool setC = false;
    uint8_t result = (reg_val << 1) & 0xFF;

    if ((reg_val & (1 << 7)) != 0) {
      result |= 1;
      setC = true;
    }

    cpu_set_reg8(reg, result);
    cpu_set_flags(ctx, result == 0, false, false, setC);
  }
    return;

  case 1: {
    // RRC
    uint8_t old = reg_val;
    reg_val >>= 1;
    reg_val |= (old << 7);

    cpu_set_reg8(reg, reg_val);
    cpu_set_flags(ctx, !reg_val, false, false, old & 1);
  }
    return;

  case 2: {
    // RL
    uint8_t old = reg_val;
    reg_val <<= 1;
    reg_val |= flagC;

    cpu_set_reg8(reg, reg_val);
    cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
  }
    return;

  case 3: {
    // RR
    uint8_t old = reg_val;
    reg_val >>= 1;

    reg_val |= (flagC << 7);

    cpu_set_reg8(reg, reg_val);
    cpu_set_flags(ctx, !reg_val, false, false, old & 1);
  }
    return;

  case 4: {
    // SLA
    uint8_t old = reg_val;
    reg_val <<= 1;

    cpu_set_reg8(reg, reg_val);
    cpu_set_flags(ctx, !reg_val, false, false, !!(old & 0x80));
  }
    return;

  case 5: {
    // SRA
    uint8_t u = (int8_t)reg_val >> 1;
    cpu_set_reg8(reg, u);
    cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
  }
    return;

  case 6: {
    // SWAP
    reg_val = ((reg_val & 0xF0) >> 4) | ((reg_val & 0xF) << 4);
    cpu_set_reg8(reg, reg_val);
    cpu_set_flags(ctx, reg_val == 0, false, false, false);
  }
    return;

  case 7: {
    // SRL
    uint8_t u = reg_val >> 1;
    cpu_set_reg8(reg, u);
    cpu_set_flags(ctx, !u, 0, 0, reg_val & 1);
  }
    return;
  }
  auto err = fmt::format("ERROR: INVALID CB: {:X}", op);
  ERROR(err.c_str());
}
static void proc_rst(cpu_context *ctx) {
  goto_addr(ctx, ctx->cur_instruction->param, true);
}
static void proc_ld(cpu_context *ctx) {
  if (ctx->dest_is_mem) {
    if (ctx->cur_instruction->reg2 >= RT_AF) {
      emu_cycles(1);
      bus_write16(ctx->mem_destination, ctx->fetch_data);
    } else {
      bus_write(ctx->mem_destination, ctx->fetch_data);
    }
  }

  if (ctx->cur_instruction->mode == AM_HL_SPR) {
    auto hflag = (cpu_read_reg(ctx->cur_instruction->reg2) & 0x000f) +
                     (ctx->fetch_data + 0x000f) >=
                 0x10;
    auto cflag = (cpu_read_reg(ctx->cur_instruction->reg2) & 0x00ff) +
                     (ctx->fetch_data + 0x00ff) >=
                 0x100;

    cpu_set_flags(ctx, 0, 0, hflag, cflag);
    cpu_set_reg(ctx->cur_instruction->reg1,
                cpu_read_reg(ctx->cur_instruction->reg2) +
                    (char)ctx->fetch_data);
  }
}

static void proc_ret(cpu_context *ctx) {
  if (ctx->cur_instruction->cond != CT_NONE) {
    emu_cycles(1);
  }
  if (check_cond(ctx)) {
    uint16_t low = stack_pop();
    emu_cycles(1);
    uint16_t high = stack_pop();
    emu_cycles(1);

    uint16_t value = (high << 8) | low;
    ctx->regs.PC = value;
    emu_cycles(1);
  }
}

static void proc_reti(cpu_context *ctx) {
  ctx->int_master_enabled = true;
  proc_ret(ctx);
}
static void proc_di(cpu_context *ctx) { ctx->int_master_enabled = false; }
static void proc_ei(cpu_context *ctx) { ctx->enabling_ime = true; }
static void proc_jr(cpu_context *ctx) {
  char rel = (char)(ctx->fetch_data & 0xFF);
  uint16_t address = ctx->regs.PC + rel;
  goto_addr(ctx, address, false);
}
static void proc_jp(cpu_context *ctx) {
  goto_addr(ctx, ctx->fetch_data, false);
}
static void proc_call(cpu_context *ctx) {
  goto_addr(ctx, ctx->fetch_data, true);
}

static void proc_nop(cpu_context *ctx) {}
static void proc_ldh(cpu_context *ctx) {
  if (ctx->cur_instruction->reg1 == RT_A) {
    cpu_set_reg(ctx->cur_instruction->reg1, bus_read(0xFF00 | ctx->fetch_data));
  } else {
    bus_write(0xFF00 | ctx->fetch_data, ctx->regs.A);
  }
  emu_cycles(1);
}

static void proc_push(cpu_context *ctx) {
  uint16_t high = (cpu_read_reg(ctx->cur_instruction->reg1) >> 8) & 0xFF;
  emu_cycles(1);
  stack_push(high);

  uint16_t low = (cpu_read_reg(ctx->cur_instruction->reg2)) & 0xFF;
  emu_cycles(1);
  stack_push(low);

  emu_cycles(1);
}
static void proc_pop(cpu_context *ctx) {
  uint8_t low = stack_pop();
  emu_cycles(1);
  uint8_t high = stack_pop();
  emu_cycles(1);
  uint16_t val = (high << 8) | low;

  cpu_set_reg(ctx->cur_instruction->reg1, val);
  if (ctx->cur_instruction->reg1 == RT_AF) {
    cpu_set_reg(ctx->cur_instruction->reg1, val & 0xFFF0);
  }
}
static void proc_rlca(cpu_context *ctx) {
  uint8_t u = ctx->regs.A;
  bool c = (u >> 7) & 0x01;
  u = (u << 1) | c;
  ctx->regs.A = u;

  cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_rrca(cpu_context *ctx) {
  uint8_t b = ctx->regs.A & 0x01;
  ctx->regs.A >>= 1;
  ctx->regs.A |= (b << 7);
  cpu_set_flags(ctx, 0, 0, 0, b);
}

static void proc_rla(cpu_context *ctx) {
  uint8_t u = ctx->regs.A;
  uint8_t c_flag = CPU_FLAG_C;
  uint8_t c = (u >> 7) & 1;

  ctx->regs.A = u << 1 | c_flag;
  cpu_set_flags(ctx, 0, 0, 0, c);
}

static void proc_rra(cpu_context *ctx) {
  uint8_t carry = CPU_FLAG_C;
  uint8_t new_c = ctx->regs.A & 1;

  ctx->regs.A >>= 1;
  ctx->regs.A |= (carry << 7);

  cpu_set_flags(ctx, 0, 0, 0, new_c);
}

static void proc_stop(cpu_context *ctx) {
  auto msg = fmt::format("Reached stop of the program");
  NO_IMPL(msg.c_str());
}

static void proc_daa(cpu_context *ctx) {
  uint8_t u = 0;
  int fc = 0;

  if (CPU_FLAG_H || (!CPU_FLAG_N && (ctx->regs.A & 0xf) > 9)) {
    u = 6;
  }

  if (CPU_FLAG_C || (!CPU_FLAG_N && (ctx->regs.A > 0x99))) {
    u |= 0x60;
    fc = 1;
  }
  ctx->regs.A += CPU_FLAG_N ? -u : u;

  cpu_set_flags(ctx, ctx->regs.A == 0, -1, 0, fc);
}
static void proc_cpl(cpu_context *ctx) {
  ctx->regs.A = ~ctx->regs.A;
  cpu_set_flags(ctx, -1, 1, 1, -1);
}
static void proc_scf(cpu_context *ctx) { cpu_set_flags(ctx, -1, 0, 0, 1); }
static void proc_ccf(cpu_context *ctx) {
  cpu_set_flags(ctx, -1, 0, 0, CPU_FLAG_C ^ 1);
}

static void proc_halt(cpu_context *ctx) { ctx->halted = true; }

static IN_PROC processors[] = {
    [IN_NONE] = proc_none, [IN_LD] = proc_ld,     [IN_XOR] = proc_xor,
    [IN_NOP] = proc_nop,   [IN_POP] = proc_pop,   [IN_JP] = proc_jp,
    [IN_PUSH] = proc_push, [IN_LDH] = proc_ldh,   [IN_DI] = proc_di,
    [IN_CALL] = proc_call, [IN_JR] = proc_jr,     [IN_RET] = proc_ret,
    [IN_RETI] = proc_reti, [IN_INC] = proc_inc,   [IN_DEC] = proc_dec,
    [IN_RST] = proc_rst,   [IN_SBC] = proc_sbc,   [IN_ADD] = proc_add,
    [IN_SUB] = proc_sub,   [IN_ADC] = proc_adc,   [IN_OR] = proc_or,
    [IN_AND] = proc_and,   [IN_CP] = proc_cp,     [IN_CB] = proc_cb,
    [IN_RRCA] = proc_rrca, [IN_RLCA] = proc_rlca, [IN_RRA] = proc_rra,
    [IN_RLA] = proc_rla,   [IN_STOP] = proc_stop, [IN_CCF] = proc_ccf,
    [IN_CPL] = proc_cpl,   [IN_DAA] = proc_daa,   [IN_SCF] = proc_scf,
    [IN_HALT] = proc_halt, [IN_EI] = proc_ei};

IN_PROC inst_get_processor(in_type type) { return processors[type]; }
