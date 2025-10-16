#include "../include/cpu.hpp"
#include "fmt/core.h"

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

static void proc_ld(cpu_context *ctx) {}
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