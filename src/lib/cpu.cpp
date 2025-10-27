#include "../include/cpu.hpp"
#include "../include/bus.hpp"
#include "fmt/core.h"
#include <cstdint>
#include <cstdlib>

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
  case AM_R_D16:
  case AM_D16: {
    uint16_t low = bus_read(ctx.regs.PC);
    emu_cycles(1);
    uint16_t high = bus_read(ctx.regs.PC + 1);
    emu_cycles(1);
    ctx.fetch_data = low | (high << 8);
    ctx.regs.PC += 2;
    return;
  }
  case AM_MR_R:
    ctx.fetch_data = cpu_read_reg(ctx.cur_instruction->reg2);
    ctx.mem_destination = cpu_read_reg(ctx.cur_instruction->reg1);
    ctx.dest_is_mem = true;
    if (ctx.cur_instruction->reg1 == RT_C) {
      ctx.mem_destination |= 0xFF00;
    }
    return;
  case AM_R_MR: {
    uint16_t addr = cpu_read_reg(ctx.cur_instruction->reg2);
    if (ctx.cur_instruction->reg1 == RT_C) {
      ctx.mem_destination |= 0xFF00;
    }
    ctx.fetch_data = bus_read(addr);
    emu_cycles(1);
    return;
  }

  case AM_R_HLI:
    ctx.fetch_data = bus_read(cpu_read_reg(ctx.cur_instruction->reg2));
    emu_cycles(1);
    cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
    return;
  case AM_R_HLD:
    ctx.fetch_data = bus_read(cpu_read_reg(ctx.cur_instruction->reg2));
    emu_cycles(1);
    cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
    return;
  case AM_HLI_R:
    ctx.fetch_data = cpu_read_reg(ctx.cur_instruction->reg2);
    ctx.mem_destination = cpu_read_reg(ctx.cur_instruction->reg1);
    ctx.dest_is_mem = true;
    cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) + 1);
    return;
  case AM_HLD_R:
    ctx.fetch_data = cpu_read_reg(ctx.cur_instruction->reg2);
    ctx.mem_destination = cpu_read_reg(ctx.cur_instruction->reg1);
    ctx.dest_is_mem = true;
    cpu_set_reg(RT_HL, cpu_read_reg(RT_HL) - 1);
    return;
  case AM_R_A8:
    ctx.fetch_data = bus_read(ctx.regs.PC);
    emu_cycles(1);
    ctx.regs.PC++;
    return;

  case AM_A8_R:
    ctx.mem_destination = bus_read(ctx.regs.PC) | 0xFF00;
    ctx.dest_is_mem = true;
    emu_cycles(1);
    ctx.regs.PC++;
    return;

  case AM_HL_SPR:
    ctx.fetch_data = bus_read(ctx.regs.PC);
    emu_cycles(1);
    ctx.regs.PC++;
    return;

  case AM_D8:
    ctx.fetch_data = bus_read(ctx.regs.PC);
    emu_cycles(1);
    ctx.regs.PC++;
    return;
  case AM_A16_R:
  case AM_D16_R: {

    uint16_t low = bus_read(ctx.regs.PC);
    emu_cycles(1);
    uint16_t high = bus_read(ctx.regs.PC + 1);
    emu_cycles(1);

    ctx.mem_destination = low | (high << 8);
    ctx.regs.PC += 2;
    ctx.fetch_data = cpu_read_reg(ctx.cur_instruction->reg2);
    return;
  }

  case AM_MR_D8:
    ctx.fetch_data = bus_read(ctx.regs.PC);
    emu_cycles(1);
    ctx.regs.PC++;
    ctx.mem_destination = cpu_read_reg(ctx.cur_instruction->reg1);
    ctx.dest_is_mem = true;
    return;

  case AM_MR:
    ctx.mem_destination = cpu_read_reg(ctx.cur_instruction->reg1);
    ctx.dest_is_mem = true;
    ctx.fetch_data = bus_read(cpu_read_reg(ctx.cur_instruction->reg1));
    emu_cycles(1);
    return;

  case AM_R_A16: {
    uint16_t low = bus_read(ctx.regs.PC);
    emu_cycles(1);

    uint16_t high = bus_read(ctx.regs.PC + 1);
    emu_cycles(1);

    uint16_t addr = low | (high << 8);

    ctx.regs.PC +=2;
    ctx.fetch_data = bus_read(addr);
    emu_cycles(1);
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
