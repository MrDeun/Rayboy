#include "../include/all.hpp"

#include "raylib.h"
#include <cstdint>
#include <thread>

cpu_context ctx = {0};
uint8_t cpu_get_ie_register() { return ctx.ie_register; }

size_t LOG_MAX = -1;
size_t log_count = 0;
void cpu_request_interupts(interrupt_type i) { ctx.int_flags |= i; }

void cpu_set_ie_register(uint8_t n) { ctx.ie_register = n; }
void cpu_init() {
  ctx.regs.PC = 0x0100;
  ctx.regs.SP = 0xfffe;
  *((short *)&ctx.regs.A) = 0xb001;
  *((short *)&ctx.regs.B) = 0x1300;
  *((short *)&ctx.regs.D) = 0xd800;
  *((short *)&ctx.regs.H) = 0x4d01;
  ctx.ie_register = 0;
  ctx.int_flags = 0;
  ctx.int_master_enabled = false;
  ctx.enabling_ime = false;

  init_ram();
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

void emu_cycles(int cycles) {

  for (int i = 0; i < cycles; i++) {
    for (int n = 0; n < 4; n++) {
      ctx.ticks++;
      timer_tick();
      ppu_tick();
    }
    dma_tick();
  }
}

std::string currentCPUState() {
  auto flags = fmt::format(
      "{:c}{:c}{:c}{:c}", ctx.regs.F & (1 << 7) ? 'Z' : '-',
      ctx.regs.F & (1 << 6) ? 'N' : '-', ctx.regs.F & (1 << 5) ? 'H' : '-',
      ctx.regs.F & (1 << 4) ? 'C' : '-');
  return fmt::format("{} A: {:02X} F:{} BC: {:02X}{:02X} DE: "
                     "{:02X}{:02X} HL: {:02X}{:02X}\n",
                     inst_toString(&ctx), ctx.regs.A, flags, ctx.regs.B,
                     ctx.regs.C, ctx.regs.D, ctx.regs.E, ctx.regs.H,
                     ctx.regs.L);
}

void cpu_set_int_flags(uint8_t flags) { ctx.int_flags = flags; }
void cpu_run_threaded(EmulatorShared* shared) {
    timer_init();
    cpu_init();
    ppu_init();
    
    shared->running.store(true, std::memory_order_release);
    
    // Stats Tracking
    auto last_stats_update = std::chrono::steady_clock::now();
    uint64_t last_cycle_count = ctx.ticks;
    uint64_t last_instruction_count = 0; // You'll need to track total instructions

    while (!shared->should_exit.load(std::memory_order_acquire)) {
        if (shared->paused.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }
        
        // 1. Execute step and track instruction count
        if (!cpu_step()) {
            break;
        }
        last_instruction_count++;

        // 2. Periodic Performance Measurement (Every 500ms or 1000ms)
        auto now = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_stats_update).count();

        if (elapsed_ms >= 1000) { // Update every second
            uint64_t current_cycles = ctx.ticks;
            
            // Calculate deltas
            uint64_t delta_cycles = current_cycles - last_cycle_count;
            uint64_t delta_inst = last_instruction_count; 
            uint64_t delta_us = std::chrono::duration_cast<std::chrono::microseconds>(now - last_stats_update).count();

            // 3. Write to the double-buffered stats
            int w_idx = shared->cpu_stats_write_index.load(std::memory_order_relaxed);
            
            shared->cpu_stats[w_idx].cycles = delta_cycles;
            shared->cpu_stats[w_idx].instructions = delta_inst;
            shared->cpu_stats[w_idx].time_us = delta_us;

            // 4. Signal UI thread that new stats are ready
            shared->cpu_stats_read_index.store(w_idx, std::memory_order_release);
            shared->cpu_stats_write_index.store(1 - w_idx, std::memory_order_relaxed);
            shared->cpu_stats_ready.store(true, std::memory_order_release);

            // 5. Reset for next interval
            last_stats_update = now;
            last_cycle_count = current_cycles;
            last_instruction_count = 0;
            
            // Also update the legacy FPS for compatibility
            shared->fps.store(shared->frame_count.exchange(0), std::memory_order_relaxed);
        }
    }
    
    shared->running.store(false, std::memory_order_release);
}
uint8_t cpu_get_int_flags() { return ctx.int_flags; }
bool cpu_step() {
  if (!ctx.halted) {
    auto pc = ctx.regs.PC;
    // fmt::println("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} "
    //              "H:{:02X} L:{:02X} "
    //              "SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}",
    //              ctx.regs.A, ctx.regs.F, ctx.regs.B, ctx.regs.C, ctx.regs.D,
    //              ctx.regs.E, ctx.regs.H, ctx.regs.L, ctx.regs.SP,
    //              ctx.regs.PC, bus_read(ctx.regs.PC), bus_read(ctx.regs.PC +
    //              1), bus_read(ctx.regs.PC + 2), bus_read(ctx.regs.PC + 3));

    fetch_instruction();
    emu_cycles(1);
    fetch_data();

    dbg_update();
    dbg_print();

    execute();
    // log_count++;
    // if (log_count > LOG_MAX) {
    //   fmt::println("Excedded max logs");
    //   exit(-1);
    // }
  } else {
    emu_cycles(1);
    if (ctx.int_flags) {
      ctx.halted = false;
    }
  }

  if (ctx.int_master_enabled) {
    cpu_handle_interrupts(&ctx);
  }

  if (dma_transferring()) {
    dma_tick();
  }

  if (ctx.enabling_ime) {
    ctx.int_master_enabled = true;
    ctx.enabling_ime = false;
  }

  return true;
}
