#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>

cpu_context ctx = {0};

void cpu_init() { ctx.regs.PC = 0x100; }
cpu_registers *cpu_get_regs() { return &ctx.regs; }
void fetch_instruction() {
  ctx.op_code = bus_read(ctx.regs.PC++);
  ctx.cur_instruction = get_instruction_by_opcode(ctx.op_code);

  if (ctx.cur_instruction == nullptr) {
    fmt::println("Unknown instruction! OPCODE = {:X}", ctx.op_code);
    exit(-6);
  }
}

void execute() {
  // fmt::println("Executing Instruction: {:X} \t PC: {}", ctx.op_code,
  // ctx.regs.PC);
  IN_PROC proc = inst_get_processor(ctx.cur_instruction->type);
  if (!proc) {
    auto err = fmt::format("No known instruction: 0x{:X}", ctx.op_code);
    NO_IMPL(err.c_str());
  }
  proc(&ctx);
}

void emu_cycles(int cycles) {}
void cpu_set_int_flags(uint8_t flags) { ctx.int_flags = flags; }

uint8_t cpu_get_int_flags() { return ctx.int_flags; }
bool cpu_step() {
  if (!ctx.halted) {
    auto pc = ctx.regs.PC;
    fetch_instruction();
    fetch_data();

    fmt::println("{}: {} (OP_CODE=0x{:X} {:X} {:X}) A: {:X} BC: {:X}{:X} DE: "
                 "{:X}{:X} HL: {:X}{:X}",
                 pc, get_inst_name(ctx.cur_instruction->type), ctx.op_code,
                 bus_read(pc + 1), bus_read(pc + 2), ctx.regs.A, ctx.regs.B,
                 ctx.regs.B, ctx.regs.D, ctx.regs.E, ctx.regs.H, ctx.regs.L);
    execute();
  } else {
    if (ctx.int_flags) {
      ctx.halted = false;
    }
  }

  if (ctx.int_master_enabled) {
    cpu_handle_interrupts(&ctx);
    ctx.enabling_ime = false;
  }

  if (ctx.enabling_ime) {
    ctx.int_master_enabled = true;
  }

  return true;
}
