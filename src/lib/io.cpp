#include "../include/all.hpp"
#include "fmt/core.h"

#include <cstdint>

static char serial_data[2];

uint8_t lx = 0;
uint8_t ly = 0;

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
  case 0xff40:
  case 0xff41:
  case 0xff42:
  case 0xff43:
  case 0xff44:
  case 0xff45:
  case 0xff46:
  case 0xff47:
  case 0xff48:
  case 0xff49:
  case 0xff4A:
  case 0xff4B:
    lcd_write(address, value);
    break;
  default:
    NO_IMPL(fmt::format("Unsupported bus read in io_write({:02X}) ", address));
    break;
  }
}

uint8_t io_read(uint16_t address) {
  switch (address) {
  case 0xff00:
    NO_IMPL("Joypad not supported yet...");
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
  case 0xff40:
  case 0xff41:
  case 0xff42:
  case 0xff43:
  case 0xff44:
  case 0xff45:
  case 0xff46:
  case 0xff47:
  case 0xff48:
  case 0xff49:
  case 0xff4A:
  case 0xff4B:
    return lcd_read(address);
  default:
    break;
  }
  NO_IMPL(fmt::format("Unsupported bus read in io_read({:02X}) ", address));
  return 0;
}
