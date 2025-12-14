#include "../include/all.hpp"
#include <cstdint>

static ppu_context ctx = {0};

void ppu_oam_write(uint16_t address, uint8_t value) {
  if (address >= 0xfe00) {
    address -= 0xfe00;
  }
  uint8_t *oam_ptr = (uint8_t *)ctx.oam_ram;
  oam_ptr[address] = value;
}
uint8_t ppu_oam_read(uint16_t address) {
  if (address >= 0xfe00) {
    address -= 0xfe00;
  }
  uint8_t *oam_ptr = (uint8_t *)ctx.oam_ram;
  return oam_ptr[address];
}

void ppu_vram_write(uint16_t address, uint8_t value) {
    ctx.vram[address-8000] = value;
}
uint8_t ppu_vram_read(uint16_t address) {
    return ctx.vram[address-0x8000];
}
