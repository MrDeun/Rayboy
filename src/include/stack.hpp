#pragma once

#include "common.hpp"
#include <cstdint>

void stack_push(uint8_t);
void stack_push16(uint16_t);

uint8_t stack_pop();
uint16_t stack_pop16();
