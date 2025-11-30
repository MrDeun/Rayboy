#pragma once

#include "common.hpp"
#include "instruction.hpp"
#include <cstdint>
struct cpu_registers {
  union {
    uint16_t AF;
    struct {
      uint8_t A;
      uint8_t F;
    };
  };
  union {
    uint16_t BC;
    struct {
      uint8_t B;
      uint8_t C;
    };
  };
  union {
    uint16_t DE;
    struct {
      uint8_t D;
      uint8_t E;
    };
  };
  union {
    uint16_t HL;
    struct {
      uint8_t H;
      uint8_t L;
    };
  };

  uint16_t SP;
  uint16_t PC;
};

struct cpu_context {
  cpu_registers regs;

  uint16_t fetch_data;
  uint16_t mem_destination;
  bool dest_is_mem;
  uint8_t op_code;
  instruction *cur_instruction;


  bool int_master_enabled;
  bool enabling_ime;
  bool halted;
  bool stepping;

  uint8_t int_flags;
};
using IN_PROC = void (*)(cpu_context *);
IN_PROC inst_get_processor(in_type type);

#define CPU_FLAG_Z BIT_GET(ctx->regs.F, 7)
#define CPU_FLAG_N BIT_GET(ctx->regs.F, 6)
#define CPU_FLAG_H BIT_GET(ctx->regs.F, 5)
#define CPU_FLAG_C BIT_GET(ctx->regs.F, 4)

cpu_registers* cpu_get_regs();

uint16_t cpu_read_reg(reg_type rt);
void cpu_set_reg(reg_type rt, uint16_t value);
void cpu_set_flags(cpu_context ctx, bool z, bool n, bool h, bool c);

void cpu_set_reg8(reg_type rt, uint8_t value);
uint8_t cpu_read_reg8(reg_type rt);

void fetch_data();

void cpu_init();
bool cpu_step();
void emu_cycles(int cycles);
