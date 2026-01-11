#include "../include/all.hpp"

#include <array>
#include <cassert>
#include <print>
struct ram_context {
  std::array<uint8_t, 0x2000> wram;
  std::array<uint8_t, 0x80> hram;
};

static ram_context ctx;

bool writeToFF03 = false;

void init_ram() {
  ctx.wram.fill(0xff);
  ctx.hram.fill(0xff);
  
}

uint8_t wram_read(uint16_t address) {
  auto err = fmt::format("ERROR: Invalid wram read address 0x{:X}", address);
  address -= 0xC000;
  if (address >= 0x2000) {
    fmt::println(err);
    exit(-1);
  }
  return ctx.wram.at(address);
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
  address -= 0xFF80;
  if (address >= 0x80) {
    fmt::println(err);
    exit(-1);
  }
  return ctx.hram.at(address);
}

void hram_write(uint16_t address, uint8_t value) {
  auto err = fmt::format("ERROR: Invalid hram write address 0x{:X}", address);
  address -= 0xFF80;
  if (address >= 0x80) {
    fmt::println(err);
    exit(-1);
  }
  ctx.hram[address] = value;
}
