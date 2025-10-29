#include "../include/bus.hpp"
#include "fmt/core.h"

#include <cassert>
#include <cstdint>
uint8_t bus_read(uint16_t address) {
  if (address < 0x8000) {
    return cart_read(address);
  }
  return 0;
  NO_IMPL("bus_read()")
}
void bus_write(uint16_t address, uint8_t value) {
  if (address < 0x8000) {
    cart_write(address, value);
  }
  return;
  NO_IMPL("bus_write()")
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
