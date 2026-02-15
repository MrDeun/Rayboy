#pragma once

#include "common.hpp"
#include "emu.hpp"
#include "instruction.hpp"
#include <cstdint>
struct cpu_registers {
    uint8_t A;
    uint8_t F;
    uint8_t B;
    uint8_t C;
    uint8_t D;
    uint8_t E;
    uint8_t H;
    uint8_t L;
    uint16_t PC;
    uint16_t SP;
} ;
// namespace Rayboy {
// class CPU {
//   private:
//     cpu_registers m_regs;
//     uint32_t m_ticks;
    
//     uint16_t m_fetched_data;
//     uint16_t mem_destination;
//     bool destination_is_mem;
//     uint8_t m_op_code;
//     instruction* cur_instruction;

//     bool int_master_enabled;
//     bool enabling_ime;
//     bool halted;
//     bool paused;
//     bool running;

//     uint8_t int_flags;
//     uint8_t ie_register;
//   public:
//     cpu_registers* getRegs() {return &m_regs;}
//     void setIERegister(uint8_t value) {ie_register = value;}
// };
// }


struct cpu_context {
  cpu_registers regs;
  uint32_t ticks;

  uint16_t fetch_data;
  uint16_t mem_destination;
  bool dest_is_mem;
  uint8_t op_code;
  instruction const* cur_instruction;


  bool int_master_enabled;
  bool enabling_ime;
  bool halted;
  bool paused;
  bool running;

  uint8_t int_flags;
  uint8_t ie_register;
};
const std::string inst_toString(cpu_context *ctx);
using IN_PROC = void (*)(cpu_context *);
IN_PROC inst_get_processor(in_type type);

#define CPU_FLAG_Z BIT_GET(ctx->regs.F, 7)
#define CPU_FLAG_N BIT_GET(ctx->regs.F, 6)
#define CPU_FLAG_H BIT_GET(ctx->regs.F, 5)
#define CPU_FLAG_C BIT_GET(ctx->regs.F, 4)

std::string currentCPUState();

cpu_registers* cpu_get_regs();

uint16_t cpu_read_reg(reg_type rt);
void cpu_set_reg(reg_type rt, uint16_t value);
void cpu_set_flags(cpu_context* ctx, int8_t z, int8_t n, int8_t h, int8_t c);
uint8_t cpu_get_int_flags();
void cpu_set_int_flags(uint8_t flags);
uint8_t cpu_get_ie_register();
void cpu_set_ie_register(uint8_t n);
void cpu_set_reg8(reg_type rt, uint8_t value);
uint8_t cpu_read_reg8(reg_type rt);

void fetch_data();
void cpu_run_threaded(EmulatorShared* shared);
void cpu_init();
bool cpu_step();
void emu_cycles(int cycles);
