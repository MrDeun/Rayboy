#include "../include/all.hpp"
#include "fmt/core.h"
#include "raylib.h"
#include <cstdint>

cpu_context ctx = {0};
uint8_t cpu_get_ie_register() { return ctx.ie_register; }

void cpu_set_ie_register(uint8_t n) { ctx.ie_register = n; }
void cpu_init() {
  ctx.regs.PC = 0x100;
  ctx.regs.SP = 0;
  *((short *)&ctx.regs.A) = 0;
  *((short *)&ctx.regs.B) = 0;
  *((short *)&ctx.regs.D) = 0;
  *((short *)&ctx.regs.H) = 0;
  ctx.ie_register = 0;
  ctx.int_flags = 0;
  ctx.int_master_enabled = false;
  ctx.enabling_ime = false;
}
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
  IN_PROC proc = inst_get_processor(ctx.cur_instruction->type);
  if (!proc) {
    auto err = fmt::format("No known instruction: 0x{:X}", ctx.op_code);
    NO_IMPL(err.c_str());
  }
  proc(&ctx);
}

void emu_cycles(int cycles) {}
void cpu_set_int_flags(uint8_t flags) { ctx.int_flags = flags; }
void *cpu_run(void *p) {
  timer_init();
  cpu_init();

  ctx.running = true;
  ctx.paused = false;
  ctx.ticks = 0;

  while (ctx.running) {
    if (ctx.paused) {
      delay(10);
      continue;
    }

    if (!cpu_step()) {
      printf("CPU Stopped\n");
      return 0;
    }
  }

  return 0;
}
uint8_t cpu_get_int_flags() { return ctx.int_flags; }
bool cpu_step() {
  if (!ctx.halted) {
    auto pc = ctx.regs.PC;
    fetch_instruction();
    emu_cycles(1);
    fetch_data();
    fmt::println("{}: {} A: {:X} BC: {:X}{:X} DE: "
                 "{:X}{:X} HL: {:X}{:X}",
                 pc, inst_toString(&ctx), ctx.regs.A, ctx.regs.B, ctx.regs.C,
                 ctx.regs.D, ctx.regs.E, ctx.regs.H, ctx.regs.L);

    dbg_update();
    dbg_print();

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
