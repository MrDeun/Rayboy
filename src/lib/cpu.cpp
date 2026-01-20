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
    
    shared->running.store(true, std::memory_order_release);
    
    int tile_update_counter = 0;
    const int TILE_UPDATE_INTERVAL = 60;
    
    auto last_fps_time = std::chrono::steady_clock::now();
    uint32_t frames_this_second = 0;
    
    while (!shared->should_exit.load(std::memory_order_acquire)) {
        if (shared->paused.load(std::memory_order_acquire)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            continue;
        }
        
        if (!cpu_step()) {
            break;
        }
        
        shared->frame_count.fetch_add(1, std::memory_order_relaxed);
        frames_this_second++;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_fps_time);
        if (elapsed.count() >= 1000) {
            shared->fps.store(frames_this_second, std::memory_order_relaxed);
            frames_this_second = 0;
            last_fps_time = now;
        }
        
        tile_update_counter++;
        if (tile_update_counter >= TILE_UPDATE_INTERVAL) {
            tile_update_counter = 0;
            
            std::vector<uint8_t>& cpu_buffer = shared->getCpuTileBuffer();
            
            for (uint16_t addr = 0x8000; addr < 0x9800; addr++) {
                cpu_buffer[addr - 0x8000] = bus_read(addr);
            }
            
            shared->signalTileDataReady();
        }
    }
    
    shared->running.store(false, std::memory_order_release);
}

uint8_t cpu_get_int_flags() { return ctx.int_flags; }
bool cpu_step() {
  if (!ctx.halted) {
    auto pc = ctx.regs.PC;
    fmt::println("A:{:02X} F:{:02X} B:{:02X} C:{:02X} D:{:02X} E:{:02X} "
                 "H:{:02X} L:{:02X} "
                 "SP:{:04X} PC:{:04X} PCMEM:{:02X},{:02X},{:02X},{:02X}",
                 ctx.regs.A, ctx.regs.F, ctx.regs.B, ctx.regs.C, ctx.regs.D,
                 ctx.regs.E, ctx.regs.H, ctx.regs.L, ctx.regs.SP,
                 ctx.regs.PC, bus_read(ctx.regs.PC), bus_read(ctx.regs.PC +
                 1), bus_read(ctx.regs.PC + 2), bus_read(ctx.regs.PC + 3));

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
