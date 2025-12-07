#pragma once

#include "all.hpp"
#include <cstdint>

uint8_t io_read(uint16_t address);
void io_write(uint16_t adress, uint8_t value);
