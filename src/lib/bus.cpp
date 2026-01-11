#include "../include/all.hpp"

#include <format>
#include <print>
#include <cassert>
#include <cstdint>
uint8_t bus_read(uint16_t address) {
  auto err = fmt::format("No implementation for bus_read(0x{:X})", address);
  if (address < 0x8000) /*ROM Data*/ {
    return cart_read(address);
  } else if (address < 0xA000) {
    return ppu_vram_read(address);
  } else if (address < 0xC000) {
    // Cartridge
    return cart_read(address);
  } else if (address < 0xE000) {
    return wram_read(address);
  } else if (address < 0xFE00) {
    // reserved by echo ram
    return 0;
  } else if (address < 0xFEA0) {
    if (dma_transferring()) {
      return 0xff;
    }
    return ppu_oam_read(address);
  } else if (address < 0xFF00) {
    // reserved unused
    return 0;
  } else if (address < 0xff80) {
    // IO Register
    return io_read(address);
    NO_IMPL(err.c_str());
  } else if (address == 0xffff) {
    // CPU Enable register
    return cpu_get_ie_register();
  }

  return hram_read(address);
}
void bus_write(uint16_t address, uint8_t value) {
  auto err = fmt::format("No implementation for bus_write(0x{:X})", address);
  if (address < 0x8000) {
    // ROM Data
    cart_write(address, value);
    return;
  } else if (address < 0xA000) {
    // Map/Char data
    ppu_vram_write(address, value);
    return;
  } else if (address < 0xC000) {
    // EXT-RAM
    cart_write(address, value);
    return;
  } else if (address < 0xE000) {
    //
    wram_write(address, value);
    return;
  } else if (address < 0xFE00) {
    // OAM
    if (dma_transferring()) {
      return;
    }
    ppu_oam_write(address, value);
    return;
  } else if (address < 0xFEA0) {
    NO_IMPL(err.c_str());
  } else if (address < 0xFF00) {
    // unusable
    return;
  } else if (address < 0xff80) {
    io_write(address, value);
  } else if (address == 0xffff) {
    cpu_set_ie_register(value);
    return;
  } else {
    hram_write(address, value);
  }
}

uint16_t bus_read16(uint16_t address) {
  uint16_t low = bus_read(address);
  uint16_t high = bus_read(address + 1);

  return low | (high << 8);
}

void bus_write16(uint16_t address, uint16_t value) {
  bus_write(address + 1, (value >> 8) & 0xff);
  bus_write(address, value & 0xff);
}
