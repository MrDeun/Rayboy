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
    fmt::println("Fetching instruction...");
    fetch_instruction();
    fmt::println("Fetching data...");
    fetch_data();
    fmt::println("Executing...");
    execute();
  }

  return true;
}
