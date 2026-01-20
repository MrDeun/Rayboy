#pragma once

#include <cstdint>
#include <string>

#include <fmt/core.h>

#define BIT_GET(a, n) ((a & (1 << n)) ? 1 : 0)
#define BIT_SET(a, n, on) (on ? a |= (1 << n) : a &= ~(1 << n))
#define BETWEEN(a, b, c) ((a >= b) && (a <= c))

#define NO_IMPL(err)                                                           \
  {                                                                            \
    printf("");                \
  }
void ERROR(std::string err);
void delay(uint32_t ms);
