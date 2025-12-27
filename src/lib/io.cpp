#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>

static char serial_data[2];

void io_write(uint16_t address, uint8_t value) {
  switch (address) {
  case 0xff00:
    fmt::println("Joypad not supported yet...");
    break;
  case 0xff01:
    serial_data[0] = value;
    break;
  case 0xff02:
    serial_data[1] = value;
    break;
  case 0xff04:
  case 0xff05:
  case 0xff06:
  case 0xff07:
    timer_write(address, value);
    break;
  case 0xff0f:
    cpu_set_int_flags(value);
    break;
  case 0xff46:
    dma_start(value);
    fmt::println("Started DMA...");

  default:
    NO_IMPL("Unsupported bus write in io_write() ");
  }
}

uint8_t io_read(uint16_t address) {
  switch (address) {
  case 0xff00:
    fmt::println("Joypad not supported yet...");
    return 0;
  case 0xff01:
    return serial_data[0];
    break;
  case 0xff02:
    return serial_data[1];
    break;
  case 0xff04:
  case 0xff05:
  case 0xff06:
  case 0xff07:
    return timer_read(address);
    break;
  case 0xff0f:
    return cpu_get_int_flags();
  case 0xFF44:
    return 0x94;
  default:
    break;
  }
  NO_IMPL("Unsupported bus read in io_read() ");
  return 0;
}
