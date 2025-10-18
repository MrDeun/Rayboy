#include "../include/cpu.hpp"
#include "../include/bus.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <cstdlib>

cpu_context ctx = {0};

void cpu_init() { ctx.regs.PC = 0x100; }

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

void fetch_instruction() {
  ctx.op_code = bus_read(ctx.regs.PC++);
  ctx.cur_instruction = get_instruction_by_opcode(ctx.op_code);

  if (ctx.cur_instruction == nullptr) {
    fmt::println("Unknown instruction! OPCODE = {:x}", ctx.op_code);
    exit(-6);
  }
}
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
void execute() {
  fmt::println("Executing Instruction: {:x} \t PC: {}", ctx.op_code,
               ctx.regs.PC);
  IN_PROC proc = inst_get_processor(ctx.cur_instruction->type);
  if (!proc) {
    auto err = fmt::format("No known instruction: {:x}", ctx.op_code);
    NO_IMPL(err.c_str());
  }
  proc(&ctx);
}

void emu_cycles(int cycles) {}

bool cpu_step() {
  if (!ctx.halted) {
    fmt::println("Fetching instruction...");
    fetch_instruction();
    fmt::println("Fetching data...");
    fetch_data();
    fmt::println("Executing...");
    execute();
  }

  return true;
}
