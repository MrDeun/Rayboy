#include "../include/cpu.hpp"
#include "../include/bus.hpp"
#include "fmt/core.h"

cpu_context ctx = {0};

void cpu_init() { ctx.regs.PC = 0x100; }

void fetch_instruction() {
  ctx.op_code = bus_read(ctx.regs.PC++);
  ctx.cur_instruction = get_instruction_by_opcode(ctx.op_code);

  if (ctx.cur_instruction == nullptr) {
    fmt::println("Unknown instruction! OPCODE = {:x}", ctx.op_code);
    exit(-6);
  }
}

void execute() {
  fmt::println("Executing Instruction: {:x} \t PC: {}", ctx.op_code,
               ctx.regs.PC);
  IN_PROC proc = inst_get_processor(ctx.cur_instruction->type);
  if (!proc) {
    auto err = fmt::format("No known instruction: 0x{:x}", ctx.op_code);
    NO_IMPL(err.c_str());
  }
  proc(&ctx);
}

void emu_cycles(int cycles) {}

bool cpu_step() {
  if (!ctx.halted) {
      auto pc = ctx.regs.PC;
    fetch_instruction();
    fetch_data();

    fmt::println("{}: {} ({:x} {:x} {:x}) A: {:x} BC: {:x}{:x} DE: {:x}{:x} HL: {:x}{:x}\n",
        pc, get_inst_name(ctx.cur_instruction->type), ctx.op_code,
        bus_read(pc + 1), bus_read(pc + 2), ctx.regs.A, ctx.regs.B, ctx.regs.B,
        ctx.regs.D, ctx.regs.E, ctx.regs.H, ctx.regs.L);
    execute();


  }

  return true;
}
