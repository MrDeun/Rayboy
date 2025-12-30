#include "../include/all.hpp"
#include "fmt/core.h"
#include <array>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
struct ram_context {
  std::array<uint8_t, 0x2000> wram;
  std::array<uint8_t, 0x80> hram;
};

static ram_context ctx;

void init_ram() {
  for (auto &byte : ctx.wram) {
    byte = 0xff;
  }

  for (auto &byte : ctx.hram) {
    byte = 0xff;
  }
}

uint8_t wram_read(uint16_t address) {
  auto err = fmt::format("ERROR: Invalid wram read address 0x{:X}", address);
  address -= 0xC000;
  if (address >= 0x2000) {
    fmt::println(err);
    exit(-1);
  }
  return ctx.wram[address];
}

void wram_write(uint16_t address, uint8_t value) {
  auto err = fmt::format("ERROR: Invalid wram write address 0x{:X}", address);
  address -= 0xC000;
  if (address >= 0x2000) {
    fmt::println(err);
    exit(-1);
  }
  ctx.wram[address] = value;
}

uint8_t hram_read(uint16_t address) {
  auto err = fmt::format("ERROR: Invalid hram read address 0x{:X}", address);
  if (address == 0xFF83 && ctx.hram[address - 0xFF80] != 0xFF) {
    fmt::println("CRITICAL: HRAM[0xFF83] is 0x{:02X}, but no write was logged!",
                 ctx.hram[address - 0xFF80]);
  }
  address -= 0xFF80;
  if (address >= 0x80) {
    fmt::println(err);
    exit(-1);
  }
  return ctx.hram[address];
}

void hram_write(uint16_t address, uint8_t value) {
  auto err = fmt::format("ERROR: Invalid hram write address 0x{:X}", address);
  if (address == 0xff83) {
    std::cerr << fmt::format(
        "Potential wrong write with value = {:02x} by PC = {}\n", value,
        cpu_get_regs()->PC);
    std::cerr << currentCPUState();
  }
  address -= 0xFF80;
  if (address >= 0x80) {
    fmt::println(err);
    exit(-1);
  }
  ctx.hram[address] = value;
}
