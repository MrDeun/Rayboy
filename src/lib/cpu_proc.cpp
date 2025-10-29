#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>

void cpu_set_flags(cpu_context *ctx, int8_t z, int8_t n, int8_t h, int8_t c){
    if(z != -1){
        BIT_SET(ctx->regs.F, 7, z);
    }
    if(n != -1){
        BIT_SET(ctx->regs.F, 6, n);
    }
    if(h != -1){
        BIT_SET(ctx->regs.F, 5, h);
    }
    if(c != -1){
        BIT_SET(ctx->regs.F, 4, c);
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

static void proc_none(cpu_context *ctx) {
  fmt::println("FATAL: Invalid Instruction!");
  exit(-8);
}

static void proc_ld(cpu_context *ctx) {
    if(ctx->dest_is_mem){
        if(ctx->cur_instruction->reg2 >= RT_AF) {
            emu_cycles(1);
            // bus_write16(ctx->mem_destination,ctx->fetch_data);
        } else {
            bus_write(ctx->mem_destination, ctx->fetch_data);
        }
    }

    if(ctx->cur_instruction->mode == AM_HL_SPR) {
        auto hflag = (cpu_read_reg(ctx->cur_instruction->reg2) & 0x000f) + (ctx->fetch_data + 0x000f) >= 0x10;
        auto cflag = (cpu_read_reg(ctx->cur_instruction->reg2) & 0x00ff) + (ctx->fetch_data + 0x00ff) >= 0x100;

        cpu_set_flags(ctx,0,0,hflag,cflag);
        cpu_set_reg(ctx->cur_instruction->reg1, cpu_read_reg(ctx->cur_instruction->reg2) + (char)ctx->fetch_data);

    }
}
static void proc_jp(cpu_context *ctx) {
  if (check_cond(ctx)) {
    ctx->regs.PC = ctx->fetch_data;
    emu_cycles(1);
  }
}

static void proc_nop(cpu_context *ctx) {}

static IN_PROC processors[] = {
    [IN_NONE] = proc_none,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_JP] = proc_jp,
};

IN_PROC inst_get_processor(in_type type) { return processors[type]; }
