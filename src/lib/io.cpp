#include "../include/all.hpp"
#include <cstdint>

static char serial_data[2];

void io_write(uint16_t address, uint8_t value) {
  switch (address) {
  case 0xff01:
    serial_data[0] = value;
    break;
  case 0xff02:
    serial_data[1] = value;
    break;
  default:
    ERROR("Unsupported bus read in io_read() ");
  }
}

uint8_t io_read(uint16_t address) {
  switch (address) {
  case 0xff01:
    return serial_data[0];
    break;
  case 0xff02:
    return serial_data[1];
    break;
  default:
    break;
  }
    ERROR("Unsupported bus read in io_read() ");
}
