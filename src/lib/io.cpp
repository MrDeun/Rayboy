#include "../include/all.hpp"
#include "fmt/core.h"
#include <cstdint>
static char serial_data[2] = {0, 0};
uint8_t io_read(uint16_t address) {
  if (address == 0xff01) {
    return serial_data[0];
  } else if (address == 0xff02) {
    return serial_data[1];
  }

  auto err = fmt::format("Illegal IO_Read of address = {:X}", address);
  ERROR(err.c_str());
}

void io_write(uint16_t adress, uint8_t value) {
  if (adress == 0xff01) {
    serial_data[0] = value;
    return;
  } else if (adress == 0xff02) {
    serial_data[1] = value;
    return;
  }

  auto err = fmt::format("Illegal IO_Write on address = {:X}", adress);
  ERROR(err.c_str());
}